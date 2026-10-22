/*
  RavestickBoard Webcontroller v0.1.2
  Hardware: TENSTAR ROBOT ESP32-C3 Super Mini
  Board: ESP32C3 Dev Module, Arduino-ESP32 3.3.12
  USB CDC On Boot: Enabled

  Wi-Fi: CoolLED1248 / Banane1248! (SSID unchanged for compatible setup)
  Website: http://192.168.4.1
  LED sign: CoolLEDX at ff:00:00:03:7e:c3, GATT FFF0 / FFF1

  Based on the working first-text sketch and the protocols documented in
  UpDryTwist/coolledx-driver and ohtaras/led (GitHub).

  Diagnostic data stays in RAM and is lost on a power cut.
  Note: Uploading a new text will overwrite the sign's current content.
  Nothing is transmitted automatically on ESP32 startup.
*/

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <BLEDevice.h>
#include <BLEClient.h>
#include <BLERemoteCharacteristic.h>
#include <cstring>
#include <esp_system.h>
#include <esp_ota_ops.h>

// Forward declaration of custom type before Arduino's auto-generated prototypes.
// Otherwise the IDE may place runJob(Job, ...) ahead of the enum definition.
enum Job : uint8_t { NONE, TEXT, BRIGHTNESS };

// Explicit declaration prevents Arduino sketch preprocessing from synthesizing
// a runJob prototype before Job is known.
bool runJob(Job job, uint8_t color, uint8_t brightness,
            const String &message, uint8_t mode, uint8_t speed);

static constexpr char FW_VERSION[] = "0.1.2";
static constexpr char APP_NAME[] = "RavestickBoard";
static constexpr char WIFI_SSID[] = "CoolLED1248";
static constexpr char WIFI_PASSWORD[] = "Banane1248!"; // Change before public events!
static constexpr char SIGN_ADDRESS[] = "ff:00:00:03:7e:c3";

static constexpr uint16_t SIGN_WIDTH = 64;
static constexpr uint16_t SIGN_HEIGHT = 16;
static constexpr uint8_t MAX_CHARS = 40;
static constexpr uint16_t MAX_COLUMNS = MAX_CHARS * 6;
static constexpr uint16_t RGB_CAPACITY = MAX_COLUMNS * (SIGN_HEIGHT / 8) * 3;
static constexpr uint16_t PAYLOAD_CAPACITY = 107 + RGB_CAPACITY;
static constexpr size_t DATA_CHUNK = 128;

WebServer web(80);
BLEClient *bleClient = nullptr;
volatile uint32_t notifyCount = 0;

uint8_t pixelData[RGB_CAPACITY];
uint8_t textPayload[PAYLOAD_CAPACITY];
uint32_t characterCodes[MAX_CHARS];

// 5x7 font; y is doubled to occupy 14 of the 16 LED rows.
struct Glyph { uint32_t code; uint8_t rows[7]; };
static const Glyph FONT[] = {
  {' ', {0,0,0,0,0,0,0}},
  {'A', {14,17,17,31,17,17,17}},
  {'B', {30,17,17,30,17,17,30}},
  {'C', {15,16,16,16,16,16,15}},
  {'D', {30,17,17,17,17,17,30}},
  {'E', {31,16,16,30,16,16,31}},
  {'F', {31,16,16,30,16,16,16}},
  {'G', {15,16,16,23,17,17,15}},
  {'H', {17,17,17,31,17,17,17}},
  {'I', {31,4,4,4,4,4,31}},
  {'J', {7,2,2,2,18,18,12}},
  {'K', {17,18,20,24,20,18,17}},
  {'L', {16,16,16,16,16,16,31}},
  {'M', {17,27,21,21,17,17,17}},
  {'N', {17,25,21,19,17,17,17}},
  {'O', {14,17,17,17,17,17,14}},
  {'P', {30,17,17,30,16,16,16}},
  {'Q', {14,17,17,17,21,18,13}},
  {'R', {30,17,17,30,20,18,17}},
  {'S', {15,16,16,14,1,1,30}},
  {'T', {31,4,4,4,4,4,4}},
  {'U', {17,17,17,17,17,17,14}},
  {'V', {17,17,17,17,17,10,4}},
  {'W', {17,17,17,21,21,21,10}},
  {'X', {17,17,10,4,10,17,17}},
  {'Y', {17,17,10,4,4,4,4}},
  {'Z', {31,1,2,4,8,16,31}},
  {'0', {14,17,19,21,25,17,14}},
  {'1', {4,12,4,4,4,4,14}},
  {'2', {14,17,1,2,4,8,31}},
  {'3', {30,1,1,14,1,1,30}},
  {'4', {2,6,10,18,31,2,2}},
  {'5', {31,16,16,30,1,1,30}},
  {'6', {14,16,16,30,17,17,14}},
  {'7', {31,1,2,4,8,8,8}},
  {'8', {14,17,17,14,17,17,14}},
  {'9', {14,17,17,15,1,1,14}},
  {'!', {4,4,4,4,4,0,4}},
  {'?', {14,17,1,2,4,0,4}},
  {'.', {0,0,0,0,0,12,12}},
  {',', {0,0,0,0,4,4,8}},
  {':', {0,4,4,0,4,4,0}},
  {'-', {0,0,0,31,0,0,0}},
  {'+', {0,4,4,31,4,4,0}},
  {'/', {1,1,2,4,8,16,16}},
  {'\'', {4,4,8,0,0,0,0}},
  {'(', {2,4,8,8,8,4,2}},
  {')', {8,4,2,2,2,4,8}},
  {'%', {17,18,2,4,8,9,17}},
  {'&', {12,18,20,8,21,18,13}},
  {'#', {10,10,31,10,31,10,10}},
  {'=', {0,31,0,31,0,0,0}},
  {';', {0,4,4,0,4,4,8}},
  {'_', {0,0,0,0,0,0,31}},
  {'*', {0,21,14,31,14,21,0}},
  // Echte Kleinbuchstaben in Engschrift. 5x7 Pixel, vertikal verdoppelt.
  {'a', {0,0,14,1,15,17,15}},
  {'b', {16,16,30,17,17,17,30}},
  {'c', {0,0,15,16,16,16,15}},
  {'d', {1,1,15,17,17,17,15}},
  {'e', {0,0,14,17,31,16,14}},
  {'f', {6,8,8,30,8,8,8}},
  {'g', {0,14,17,17,15,1,14}},
  {'h', {16,16,30,17,17,17,17}},
  {'i', {4,0,12,4,4,4,14}},
  {'j', {2,0,6,2,2,18,12}},
  {'k', {16,16,18,20,24,20,18}},
  {'l', {12,4,4,4,4,4,14}},
  {'m', {0,0,26,21,21,21,21}},
  {'n', {0,0,30,17,17,17,17}},
  {'o', {0,0,14,17,17,17,14}},
  {'p', {0,30,17,17,30,16,16}},
  {'q', {0,15,17,17,15,1,1}},
  {'r', {0,0,23,24,16,16,16}},
  {'s', {0,0,15,16,14,1,30}},
  {'t', {8,8,30,8,8,8,6}},
  {'u', {0,0,17,17,17,19,13}},
  {'v', {0,0,17,17,17,10,4}},
  {'w', {0,0,17,17,21,21,10}},
  {'x', {0,0,17,10,4,10,17}},
  {'y', {0,17,17,17,15,1,14}},
  {'z', {0,0,31,2,4,8,31}},
  {0x00E4, {10,0,14,1,15,17,15}},   // ä
  {0x00F6, {10,0,14,17,17,17,14}},  // ö
  {0x00FC, {10,0,17,17,17,19,13}},  // ü
  {0x00C4, {10,0,14,17,31,17,17}},  // Ä
  {0x00D6, {10,0,14,17,17,17,14}},  // Ö
  {0x00DC, {10,0,17,17,17,17,14}},  // Ü
  {0x00DF, {14,17,18,20,18,17,30}}, // ß
  {0x2764, {0,10,31,31,31,14,4}},   // ❤
};

static const uint8_t UNKNOWN_GLYPH[7] = {14,17,1,2,4,0,4};

String jsonQuote(const String &s);

// Bounded in-RAM diagnostics. No background writes to SPIFFS or NVS.
// The BLE callback only increments counters; the main loop records events.
struct DiagnosticEvent {
  uint32_t atMs;
  char category[14];
  char details[92];
};
static constexpr uint8_t EVENT_CAPACITY = 56;
DiagnosticEvent eventRing[EVENT_CAPACITY] = {};
uint8_t eventNext = 0;
uint8_t eventCount = 0;

struct DiagnosticCounters {
  uint32_t bleAttempts = 0;
  uint32_t bleConnected = 0;
  uint32_t bleDisconnected = 0;
  uint32_t bleWriteOK = 0;
  uint32_t bleWriteFailed = 0;
  uint32_t bleTimeouts = 0;
  uint32_t chunksSent = 0;
  uint32_t bytesSent = 0;
  uint32_t notificationsProcessed = 0;
  uint32_t jobsOK = 0;
  uint32_t jobsFailed = 0;
  uint32_t httpTextRequests = 0;
  uint32_t httpBrightnessRequests = 0;
  uint32_t httpRejectedRequests = 0;
  uint32_t lastJobDurationMs = 0;
  uint32_t errors = 0;
} diagnostics;

String latestError = "Kein Fehler seit Systemstart.";
uint32_t latestErrorAtMs = 0;
uint8_t knownWifiClients = 0;
volatile uint32_t notificationBytes = 0;

void diagnosticEvent(const char *category, const String &details, bool error = false) {
  DiagnosticEvent &e = eventRing[eventNext];
  e.atMs = millis();
  snprintf(e.category, sizeof(e.category), "%s", category);
  snprintf(e.details, sizeof(e.details), "%s", details.c_str());
  eventNext = (eventNext + 1) % EVENT_CAPACITY;
  if (eventCount < EVENT_CAPACITY) ++eventCount;
  if (error) {
    ++diagnostics.errors;
    latestError = String(category) + ": " + details;
    latestErrorAtMs = e.atMs;
  }
  Serial.printf("[%lu ms] [%s] %s\n", (unsigned long)e.atMs, e.category, e.details);
}

String resetDescription() {
  switch (esp_reset_reason()) {
    case ESP_RST_POWERON: return "Einschalten / Stromversorgung";
    case ESP_RST_USB: return "USB-Reset";
    case ESP_RST_EXT: return "Externer Reset";
    case ESP_RST_JTAG: return "JTAG-Reset";
    case ESP_RST_SW: return "Software-Neustart";
    case ESP_RST_PANIC: return "Ausnahmefehler (Panic)";
    case ESP_RST_INT_WDT: return "Interrupt-Watchdog";
    case ESP_RST_TASK_WDT: return "Task-Watchdog";
    case ESP_RST_WDT: return "Watchdog-Reset";
    case ESP_RST_BROWNOUT: return "Spannungsabfall (Brownout)";
    case ESP_RST_DEEPSLEEP: return "Aufwachen aus Tiefschlaf";
    default: return String("Reset code ") + (int)esp_reset_reason();
  }
}

uint32_t appPartitionSize() {
  const esp_partition_t *p = esp_ota_get_running_partition();
  return p ? p->size : 0;
}
uint32_t appAvailable() {
  const uint32_t total = appPartitionSize(), used = ESP.getSketchSize();
  return total > used ? total - used : 0;
}

String uptimeText() {
  const uint32_t seconds = millis() / 1000;
  const uint32_t days = seconds / 86400;
  const uint32_t hours = (seconds % 86400) / 3600;
  const uint32_t minutes = (seconds % 3600) / 60;
  const uint32_t secs = seconds % 60;
  char temp[48];
  snprintf(temp, sizeof(temp), "%lu d %02lu:%02lu:%02lu", (unsigned long)days,
           (unsigned long)hours, (unsigned long)minutes, (unsigned long)secs);
  return String(temp);
}

String recentEvents(uint8_t limit = EVENT_CAPACITY) {
  String result;
  result.reserve(4800);
  uint8_t wanted = (eventCount < limit) ? eventCount : limit;
  const uint8_t start = (eventNext + EVENT_CAPACITY - wanted) % EVENT_CAPACITY;
  for (uint8_t i = 0; i < wanted; ++i) {
    const DiagnosticEvent &e = eventRing[(start + i) % EVENT_CAPACITY];
    result += String(e.atMs);
    result += " ms | ";
    result += e.category;
    result += " | ";
    result += e.details;
    result += '\n';
  }
  return result;
}

// Leave passwords, text content and BLE MAC addresses out of exported reports.
String diagnosticReport() {
  String out;
  out.reserve(11000);
  out += "RavestickBoard for CoolLED1248 | Diagnose\n";
  out += "Firmware: v"; out += FW_VERSION; out += "\n";
  out += "Build: "; out += __DATE__; out += ' '; out += __TIME__; out += "\n";
  out += "Board: ESP32-C3 Super Mini (ESP32C3 Dev Module)\n";
  out += "Resetgrund: "; out += resetDescription(); out += "\n";
  out += "Laufzeit: "; out += uptimeText(); out += "\n";
  out += "Uptime ms (32 bit): "; out += String(millis()); out += "\n";
  out += "WLAN: Access Point / "; out += WIFI_SSID; out += "\n";
  out += "WLAN Clients: "; out += String(WiFi.softAPgetStationNum()); out += "\n";
  out += "AP-IP: "; out += WiFi.softAPIP().toString(); out += "\n";
  out += "BLE verbunden (Momentaufnahme): ";
  out += (bleClient && bleClient->isConnected()) ? "ja\n" : "nein\n";
  out += "Freier Heap: "; out += String(ESP.getFreeHeap()); out += " Bytes\n";
  out += "Minimal freier Heap: "; out += String(ESP.getMinFreeHeap()); out += " Bytes\n";
  out += "Größter allokierbarer Block: "; out += String(ESP.getMaxAllocHeap()); out += " Bytes\n";
  out += "Heap gesamt: "; out += String(ESP.getHeapSize()); out += " Bytes\n";
  out += "Flash physisch: "; out += String(ESP.getFlashChipSize()); out += " Bytes\n";
  out += "Sketch verwendet: "; out += String(ESP.getSketchSize()); out += " Bytes\n";
  out += "App-Partition gesamt: "; out += String(appPartitionSize()); out += " Bytes\n";
  out += "App-Partition rechnerisch frei: "; out += String(appAvailable()); out += " Bytes\n";
  out += "Dateisystem: nicht eingebunden, RAM-Ereignisprotokoll\n";
  out += "\nZAeHLER SEIT BOOT\n";
  out += "BLE Versuche: "; out += String(diagnostics.bleAttempts); out += '\n';
  out += "BLE Erfolge: "; out += String(diagnostics.bleConnected); out += '\n';
  out += "BLE Trennungen: "; out += String(diagnostics.bleDisconnected); out += '\n';
  out += "BLE Writes akzeptiert: "; out += String(diagnostics.bleWriteOK); out += '\n';
  out += "BLE Write-Fehler: "; out += String(diagnostics.bleWriteFailed); out += '\n';
  out += "BLE Notifications empfangen: "; out += String(notifyCount); out += '\n';
  out += "BLE Notification-Bytes: "; out += String(notificationBytes); out += '\n';
  out += "BLE-Bestätigungen empfangen: "; out += String(diagnostics.notificationsProcessed); out += '\n';
  out += "BLE Notify-Timeouts: "; out += String(diagnostics.bleTimeouts); out += '\n';
  out += "Pakete gesendet: "; out += String(diagnostics.chunksSent); out += '\n';
  out += "BLE-Daten gesendet: "; out += String(diagnostics.bytesSent); out += " Bytes\n";
  out += "HTTP-Textaufträge: "; out += String(diagnostics.httpTextRequests); out += '\n';
  out += "HTTP-Helligkeitsaufträge: "; out += String(diagnostics.httpBrightnessRequests); out += '\n';
  out += "HTTP-Aufträge abgewiesen: "; out += String(diagnostics.httpRejectedRequests); out += '\n';
  out += "Jobs OK: "; out += String(diagnostics.jobsOK); out += '\n';
  out += "Jobs Fehler: "; out += String(diagnostics.jobsFailed); out += '\n';
  out += "Letzte Job-Dauer: "; out += String(diagnostics.lastJobDurationMs); out += " ms\n";
  out += "Fehler insgesamt: "; out += String(diagnostics.errors); out += '\n';
  out += "Letzter Fehler bei ms "; out += String(latestErrorAtMs); out += ": "; out += latestError; out += "\n";
  out += "\nHINWEISE\n";
  out += "BLE-Schreiberfolg und Bestätigung bedeuten nicht automatisch eine sichtbare Anzeigeänderung.\n";
  out += "Log-Ringpuffer: nur RAM; nach Stromverlust und Neustart leer.\n";
  out += "Keine Textinhalte, Zugangsdaten oder Geraeteadressen im Export.\n";
  out += "Zeitstempel: Millisekunden seit letztem Start, keine Uhrzeit.\n";
  out += "\nLETZTE EREIGNISSE (ältestes zuerst)\n";
  out += recentEvents();
  return out;
}

void onDiagnosticsJson() {
  String json;
  json.reserve(5000);
  json += "{\"version\":\""; json += FW_VERSION;
  json += "\",\"uptime\":"; json += jsonQuote(uptimeText());
  json += ",\"reset\":"; json += jsonQuote(resetDescription());
  json += ",\"freeHeap\":"; json += String(ESP.getFreeHeap());
  json += ",\"minHeap\":"; json += String(ESP.getMinFreeHeap());
  json += ",\"maxBlock\":"; json += String(ESP.getMaxAllocHeap());
  json += ",\"sketchSize\":"; json += String(ESP.getSketchSize());
  json += ",\"appPartitionSize\":"; json += String(appPartitionSize());
  json += ",\"appAvailable\":"; json += String(appAvailable());
  json += ",\"wifiClients\":"; json += String(WiFi.softAPgetStationNum());
  json += ",\"bleConnected\":"; json += ((bleClient && bleClient->isConnected()) ? "true" : "false");
  json += ",\"bleAttempts\":"; json += String(diagnostics.bleAttempts);
  json += ",\"bleSuccess\":"; json += String(diagnostics.bleConnected);
  json += ",\"bleTimeouts\":"; json += String(diagnostics.bleTimeouts);
  json += ",\"writeFailures\":"; json += String(diagnostics.bleWriteFailed);
  json += ",\"notifications\":"; json += String(notifyCount);
  json += ",\"jobsOK\":"; json += String(diagnostics.jobsOK);
  json += ",\"jobsFailed\":"; json += String(diagnostics.jobsFailed);
  json += ",\"errors\":"; json += String(diagnostics.errors);
  json += ",\"recent\":"; json += jsonQuote(recentEvents(14));
  json += ",\"lastError\":"; json += jsonQuote(latestError);
  json += "}";
  web.sendHeader("Cache-Control", "no-store");
  web.send(200, "application/json; charset=utf-8", json);
}

void onDiagnosticsDownload() {
  web.sendHeader("Cache-Control", "no-store");
  web.sendHeader("Content-Disposition", "attachment; filename=RavestickBoard_Diagnose_v0.1.2.txt");
  web.send(200, "text/plain; charset=utf-8", diagnosticReport());
}

void onFontJson() {
  String json;
  json.reserve(3300);
  json += '{';
  bool first = true;
  for (const Glyph &g : FONT) {
    if (!first) json += ',';
    first = false;
    json += '"'; json += String(g.code); json += "\":[";
    for (uint8_t i = 0; i < 7; i++) {
      if (i) json += ',';
      json += String(g.rows[i]);
    }
    json += ']';
  }
  json += '}';
  web.sendHeader("Cache-Control", "no-store");
  web.send(200, "application/json; charset=utf-8", json);
}

// HTML is served directly from the ESP32 flash; no filesystem upload needed.
static const char PAGE[] PROGMEM = R"HTML(
<!doctype html>
<html lang="de">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1, viewport-fit=cover">
<meta name="theme-color" content="#11141c">
<title>RavestickBoard · v0.1.2</title>
<style>
:root{color-scheme:dark;font-family:system-ui,-apple-system,"Segoe UI",sans-serif;background:#11141c;color:#f3f5fa}
*{box-sizing:border-box}
body{margin:0;padding:16px 13px calc(94px + env(safe-area-inset-bottom,0px))}
main{max-width:630px;margin:0 auto}
header{display:flex;align-items:center;gap:12px;margin:7px 0 24px;min-width:0}
.logo{width:44px;height:44px;flex:0 0 44px;border-radius:12px;background:#273044;display:grid;place-items:center;font-size:25px}
h1{font-size:clamp(20px,5vw,24px);margin:0;letter-spacing:-.5px;line-height:1.25}
.sub{font-size:12px;color:#a4b0c1;margin-top:3px}
.version{font-size:11px;color:#ffda73;background:#39404d;padding:4px 7px;border-radius:99px;display:inline-block;vertical-align:middle;white-space:nowrap;letter-spacing:0;margin-left:4px}
.panel{background:#1d222e;border:1px solid #363e4e;border-radius:17px;padding:18px;margin-bottom:14px;min-width:0}
.label{font-weight:800;font-size:12px;letter-spacing:1px;text-transform:uppercase;color:#a9b4c5;margin:0 0 10px}
.hint{color:#abb6c7;font-size:12px;line-height:1.55;margin:9px 0 0}
#preview{display:block;width:100%;height:auto;aspect-ratio:4/1;background:#070a0e;border-radius:9px}
.preview-controls{display:flex;align-items:center;gap:7px;flex-wrap:wrap;margin:13px 0 5px}
button{font:inherit;font-weight:750;border:0;border-radius:10px;padding:12px 13px;cursor:pointer}
button:disabled{opacity:.55;cursor:wait}
.small-btn{font-size:12px;padding:10px 12px;background:#323a4a;color:#e6ecf6;white-space:nowrap}
.small-btn.active{background:#ffcf4e;color:#10151d}
.player-label{margin-left:auto;color:#a9b4c5;font-size:12px;white-space:nowrap}
input[type=text]{background:#111720;color:#fff;border:1px solid #505b70;border-radius:10px;padding:13px 14px;font-size:19px;width:100%;outline:none}
input[type=text]:focus{border-color:#ffce4f}
#count{text-align:right;color:#a9b4c5;font-size:12px;margin:6px 0 16px}
.segment{display:grid;grid-template-columns:1fr 1fr;gap:8px;margin-bottom:14px}
.segment label{display:block;background:#111822;border:1px solid #505a6b;border-radius:11px;padding:12px;text-align:center;font-size:14px;font-weight:750;cursor:pointer}
.segment input{position:absolute;opacity:0;pointer-events:none}
.segment input:checked+label{border-color:#ffcf4e;background:#3f371f;color:#ffe199}
.swatches{display:grid;grid-template-columns:repeat(7,minmax(0,1fr));gap:4px;justify-items:center;margin:8px 0 24px}
.swatches input{position:absolute;opacity:0;pointer-events:none}
.swatches label{display:block;width:clamp(26px,8vw,40px);height:clamp(26px,8vw,40px);border-radius:50%;border:3px solid #2d3545;cursor:pointer}
.swatches input:checked+label{outline:2px solid #fff;outline-offset:2px}
.range-row{display:flex;align-items:center;gap:14px;margin:8px 0 22px}
.range-row input{min-width:0;flex:1;width:100%;accent-color:#ffd050}
.range-row b{font-variant-numeric:tabular-nums;min-width:65px;text-align:right;white-space:nowrap}
.warning{border:1px solid #926d2c;background:#31291b;color:#ffdb97;padding:11px;border-radius:9px;font-size:12px;line-height:1.5;margin:10px 0}
.buttons{display:grid;grid-template-columns:1fr 1fr;gap:9px}
.primary{background:#ffcf4d;color:#171a20}.secondary{background:#343d4f;color:#f5f7fb}
#status{margin-top:14px;border-radius:9px;background:#2a303e;padding:11px 12px;font-size:13px;line-height:1.5;min-height:42px}
.small{font-size:12px;color:#aab5c5;line-height:1.6}
.diag-disclosure{cursor:pointer;font-weight:800;list-style:none}
.diag-disclosure::-webkit-details-marker{display:none}.diag-disclosure:before{content:"▸";margin-right:7px}details[open] .diag-disclosure:before{content:"▾"}
.diag-grid{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:10px;margin:13px 0}
.diag-cell{background:#121924;border-radius:9px;padding:10px;min-width:0}.diag-cell small{display:block;color:#aab5c5;margin:0 0 4px}.diag-cell strong{display:block;font-size:14px;overflow-wrap:anywhere}
.diag-tools{display:flex;flex-wrap:wrap;gap:8px;margin:14px 0}.diag-tools button,.diag-tools a{font-size:13px;padding:11px 13px;text-decoration:none;text-align:center}
.diag-log{background:#10161f;border-radius:9px;padding:12px;font:11px/1.65 ui-monospace,monospace;white-space:pre-wrap;overflow-wrap:anywhere;max-height:230px;overflow:auto}
#copyManual{margin-top:9px}#copyManual textarea{width:100%;height:140px;border-radius:9px;background:#10161f;color:#f2f3f7;border:1px solid #63728a;font:12px/1.4 ui-monospace,monospace}
@media(max-width:380px){.buttons{grid-template-columns:1fr}.player-label{width:100%;margin:0}}
</style>
</head><body><main>
<header><div class="logo" aria-hidden="true">🍌</div><div>
<h1>RavestickBoard <span class="version">v0.1.2</span></h1>
<div class="sub">for CoolLED1248 · ESP32-C3</div></div></header>
<section class="panel" aria-labelledby="preview-heading">
<div class="label" id="preview-heading">LED-Vorschau · 64 × 16 Punkte</div>
<canvas id="preview" width="768" height="192" aria-label="LED-Vorschau mit 64 mal 16 einzelnen Leuchtpunkten"></canvas>
<div class="preview-controls"><button class="small-btn" id="restart" type="button" title="Vorschau von vorn">⏮ Neustart</button>
<button class="small-btn active" id="play" type="button">⏸ Pause</button>
<button class="small-btn" id="fast" type="button">⏩ 1×</button>
<span class="player-label" id="previewInfo">Statische Anzeige</span></div>
<div class="hint">Die Vorschau zeigt immer genau 64 × 16 LED-Punkte. Vorspulen verändert nur die Vorschau, nicht die Tafel.</div>
</section>
<section class="panel" aria-labelledby="editor-heading">
<div class="label" id="editor-heading">Anzeigetext</div>
<input id="message" type="text" value="Grüße!" placeholder="Dein Text mit ä, ö, ü und ß" autocomplete="off" spellcheck="false" aria-label="Anzeigetext">
<div id="count">0 / 40 Zeichen</div>
<div class="label">Anzeigemodus</div>
<div class="segment" id="modes"><div><input type="radio" name="mode" id="static" value="static" checked><label for="static">▣ Standbild</label></div>
<div><input type="radio" name="mode" id="scroll" value="scroll"><label for="scroll">↔ Lauftext</label></div></div>
<div class="hint" style="margin:-4px 0 16px">Die Anzeigeart wird bewusst gewählt und hängt nicht von der Textlänge ab. Ein mehrseitiger Editor folgt später.</div>
<div class="label">Textfarbe</div><div class="swatches" id="swatches"></div>
<div class="label">Helligkeit</div><div class="range-row"><input id="brightness" type="range" min="1" max="100" value="75"><b id="percent">75 %</b></div>
<div class="label">Laufgeschwindigkeit</div><div class="range-row"><input id="speed" type="range" min="1" max="100" value="50"><b id="speedValue">50 / 100</b></div>
<div class="hint" style="margin:-15px 0 16px">Der Regler sendet den Geschwindigkeitsbefehl der Originalhardware. Der genaue Geschwindigkeitsverlauf wird noch am Board kalibriert.</div>
<div id="overflow" class="warning" hidden></div>
<div class="buttons"><button class="primary" id="send" type="button">Text senden</button><button class="secondary" id="apply" type="button">Nur Helligkeit</button></div>
<div role="status" aria-live="polite" id="status">Verbinde mit dem Controller …</div>
</section>
<details class="panel" id="diagnostics"><summary class="diag-disclosure">Diagnose und Ereignisprotokoll</summary>
<p class="hint">Aktuelle Messwerte und Ereignisse seit dem letzten Start. Textinhalte, Passwörter und BLE-Adressen werden nicht exportiert.</p>
<div class="diag-grid" id="diagGrid"><div class="small">Diagnose wird beim Öffnen geladen.</div></div>
<div class="diag-tools"><button class="secondary" id="diagReload" type="button">Aktualisieren</button>
<button class="primary" id="diagCopy" type="button">📋 Log kopieren</button>
<a class="secondary" href="/api/diagnostics.txt" download="RavestickBoard_Diagnose_v0.1.2.txt">TXT herunterladen</a></div>
<div class="label">Letzte Ereignisse</div><div id="diagLog" class="diag-log">Noch nicht geladen.</div>
<div id="diagFeedback" class="hint" role="status" aria-live="polite"></div>
<div id="copyManual" hidden><p class="hint">Safari blockiert das Kopieren? Textfeld antippen, alles auswählen und manuell kopieren.</p>
<textarea id="manualText" readonly aria-label="Diagnosebericht zum Kopieren"></textarea></div>
</details>
<p class="small">WLAN: <b>CoolLED1248</b> · Webinterface: <b>192.168.4.1</b> · Keine Internetverbindung notwendig.<br>
Nur ein bewusst gesendeter Auftrag verändert die LED-Tafel. Firmware-Updates erfolgen ausschließlich über USB.</p>
</main>
<script>
'use strict';
const $=id=>document.getElementById(id);
const colors=[['Rot',4,'#ff3939'],['Gelb',6,'#ffff45'],['Grün',2,'#4bf641'],['Cyan',3,'#53eeee'],['Blau',1,'#4d51ff'],['Magenta',5,'#ef47eb'],['Weiß',7,'#ffffff']];
let selectedColor=7,displayMode='static',fontMap={},fontReady=false;
const cv=$('preview'),ctx=cv.getContext('2d');
const WIDTH=64,HEIGHT=16,PITCH=12,CHARWIDTH=6;
const fastRates=[1,2,4,8];let fastIndex=0,playing=true,position=WIDTH,lastFrame=0;
const GLYPH_FALLBACK=[14,17,1,2,4,0,4];
const swatchRoot=$('swatches');
swatchRoot.innerHTML=colors.map(([name,n,hex])=>`<div><input type="radio" id="c${n}" name="color" value="${n}" ${n===7?'checked':''}><label for="c${n}" style="background:${hex}" title="${name}" aria-label="${name}"></label></div>`).join('');
swatchRoot.addEventListener('change',e=>{if(e.target.name==='color'){selectedColor=Number(e.target.value);draw();}});
function cps(text){return Array.from(text.replaceAll('❤️','❤')).filter(c=>c.codePointAt(0)!==0xFE0F && c.codePointAt(0)!==0xFE0E);}
function glyph(c){return fontMap[c.codePointAt(0)]||GLYPH_FALLBACK;}
function textWidth(){return Math.max(0,cps($('message').value).length*CHARWIDTH-1);}
function updateControls(){
 const count=cps($('message').value).length,tw=textWidth();
 $('count').textContent=`${count} / 40 Zeichen`;
 const overflow=displayMode==='static' && tw>WIDTH;
 $('overflow').hidden=!overflow;
 if(overflow)$('overflow').textContent=`Der Text ist ${tw} Pixel breit, das Display nur ${WIDTH}. Im Standbild wird nur der linke Ausschnitt angezeigt. Für weitere Seiten benötigen wir den kommenden Seiteneditor.`;
 if(count>40){$('overflow').hidden=false;$('overflow').textContent='Maximal 40 Zeichen: Bitte den Text kürzen.';}
 $('previewInfo').textContent=displayMode==='scroll'?'Lauftext, '+fastRates[fastIndex]+'× Vorschau':'Standbild';
 $('speed').disabled=displayMode!=='scroll';
 $('play').disabled=displayMode!=='scroll';
 $('fast').disabled=displayMode!=='scroll';
 $('restart').disabled=displayMode!=='scroll';
}
function colorHex(){return colors.find(x=>x[1]===selectedColor)[2];}
function draw(){
 const text=cps($('message').value),tw=textWidth();
 ctx.fillStyle='#070a0e';ctx.fillRect(0,0,cv.width,cv.height);
 let left=displayMode==='scroll'?position:(tw<=WIDTH?Math.floor((WIDTH-tw)/2):0);
 const lit=colorHex();
 for(let x=0;x<WIDTH;x++)for(let y=0;y<HEIGHT;y++){
   const px=x*PITCH+PITCH/2,py=y*PITCH+PITCH/2;
   ctx.beginPath();ctx.arc(px,py,3.25,0,Math.PI*2);ctx.fillStyle='#242b35';ctx.fill();
 }
 if(fontReady){
 ctx.fillStyle=lit;
 for(let i=0;i<text.length;i++){
   const rows=glyph(text[i]);const base=left+i*CHARWIDTH;
   for(let yy=0;yy<7;yy++)for(let xx=0;xx<5;xx++){
     const x=Math.floor(base+xx);
     if(x<0||x>=WIDTH||!(rows[yy]&(1<<(4-xx))))continue;
     for(let dy=0;dy<2;dy++){
       const y=1+yy*2+dy;ctx.beginPath();ctx.arc(x*PITCH+PITCH/2,y*PITCH+PITCH/2,4,0,Math.PI*2);ctx.fill();
     }
   }
 }
 }else{ctx.fillStyle='#aeb6c5';ctx.font='13px system-ui';ctx.fillText('Schrift wird geladen …',12,cv.height/2);}
}
function restart(){position=WIDTH;draw();}
function step(ts){
 if(!lastFrame)lastFrame=ts;
 const dt=Math.min(100,ts-lastFrame);lastFrame=ts;
 if(displayMode==='scroll'&&playing&&fontReady){
   const boardSpeed=Number($('speed').value);
   const pixelsPerSecond=4+boardSpeed*0.26; // UI approximation, not calibrated to hardware yet.
   position-=dt/1000*pixelsPerSecond*fastRates[fastIndex];
   if(position<-(textWidth()+8))position=WIDTH;
   draw();
 }
 requestAnimationFrame(step);
}
function updateText(){restart();updateControls();draw();}
$('message').addEventListener('input',updateText);
$('modes').addEventListener('change',e=>{if(e.target.name==='mode'){displayMode=e.target.value;updateText();}});
$('brightness').addEventListener('input',()=>{$('percent').textContent=$('brightness').value+' %';});
$('speed').addEventListener('input',()=>{$('speedValue').textContent=$('speed').value+' / 100';});
$('restart').addEventListener('click',restart);
$('play').addEventListener('click',()=>{playing=!playing;$('play').textContent=playing?'⏸ Pause':'▶ Weiter';});
$('fast').addEventListener('click',()=>{fastIndex=(fastIndex+1)%fastRates.length;$('fast').textContent='⏩ '+fastRates[fastIndex]+'×';updateControls();});
async function loadFont(){try{const r=await fetch('/api/font',{cache:'no-store'});if(!r.ok)throw Error('HTTP '+r.status);fontMap=await r.json();fontReady=true;draw();}
 catch(e){$('status').textContent='Schrift konnte nicht geladen werden. Seite neu öffnen.';}}
let busy=false;function lock(b){busy=b;$('send').disabled=b;$('apply').disabled=b;}
async function post(path,data){
 try{
   lock(true);$('status').textContent='Auftrag wird vorbereitet …';
   const r=await fetch(path,{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:new URLSearchParams(data)});
   const j=await r.json();if(!r.ok)throw Error(j.message||'HTTP '+r.status);
   $('status').textContent=j.message||'Auftrag angenommen.';
 }catch(e){$('status').textContent='Fehler: '+e.message;lock(false);}
}
$('send').addEventListener('click',()=>{
 const text=$('message').value;if(!text.trim()){$('status').textContent='Bitte einen Text eingeben.';return;}
 if(cps(text).length>40){$('status').textContent='Bitte höchstens 40 Zeichen eingeben.';return;}
 post('/api/text',{text,color:selectedColor,brightness:$('brightness').value,mode:displayMode,speed:$('speed').value});
});
$('apply').addEventListener('click',()=>post('/api/brightness',{brightness:$('brightness').value}));
async function poll(){try{
 const r=await fetch('/api/status',{cache:'no-store'});if(!r.ok)throw Error('HTTP '+r.status);
 const d=await r.json();$('status').textContent=d.message;lock(d.state==='queued'||d.state==='sending');
 }catch(e){$('status').textContent='Controller nicht erreichbar.';lock(false);}}
let reportCache='';
async function refreshDiagnostics(){
 const grid=$('diagGrid'),log=$('diagLog');
 try{
  grid.textContent='Messwerte werden geladen …';
  const [r,t]=await Promise.all([fetch('/api/diagnostics',{cache:'no-store'}),fetch('/api/diagnostics.txt',{cache:'no-store'})]);
  if(!r.ok||!t.ok)throw Error('HTTP '+(r.ok?t.status:r.status));
  const d=await r.json();reportCache=await t.text();
  const rows=[['Firmware','v'+d.version],['Laufzeit',d.uptime],['Resetgrund',d.reset],
    ['Freier Heap',(d.freeHeap/1024).toFixed(1)+' KiB'],
    ['Minimal frei',(d.minHeap/1024).toFixed(1)+' KiB'],
    ['Größter Block',(d.maxBlock/1024).toFixed(1)+' KiB'],
    ['Firmwaregröße',(d.sketchSize/1024).toFixed(0)+' KiB'],
    ['App-Partition',(d.appPartitionSize/1024).toFixed(0)+' KiB'],
    ['App-Rest',(d.appAvailable/1024).toFixed(0)+' KiB'],
    ['WLAN-Geräte',d.wifiClients],['Bluetooth',d.bleConnected?'Verbunden':'Getrennt'],
    ['BLE-Versuche',d.bleAttempts],['BLE-Verbindungen',d.bleSuccess],
    ['BLE-Timeouts',d.bleTimeouts],['Schreibfehler',d.writeFailures],
    ['Benachrichtigungen',d.notifications],['Aufträge OK / Fehler',d.jobsOK+' / '+d.jobsFailed],
    ['Fehler gesamt',d.errors]];
  grid.replaceChildren();
  for(const [label,value] of rows){const cell=document.createElement('div');cell.className='diag-cell';const a=document.createElement('small');a.textContent=label;const b=document.createElement('strong');b.textContent=String(value);cell.append(a,b);grid.append(cell);}
  log.textContent=d.recent||'Noch keine Ereignisse.';
  $('diagFeedback').textContent='Letzter Fehler: '+d.lastError;
 }catch(e){grid.textContent='Diagnose nicht verfügbar.';log.textContent='Fehler: '+e.message;$('diagFeedback').textContent=e.message;}
}
function copyFallback(str){
 const node=document.createElement('textarea');node.value=str;node.style.position='fixed';node.style.opacity='0';node.style.left='-2000px';document.body.append(node);node.focus();node.select();node.setSelectionRange(0,str.length);
 let ok=false;try{ok=document.execCommand('copy');}catch(e){}node.remove();return ok;
}
$('diagCopy').addEventListener('click',async()=>{
 let text=reportCache;
 if(!text){$('diagFeedback').textContent='Bitte zuerst auf „Aktualisieren“ tippen.';return;}
 const fallback=()=>copyFallback(text);
 if(!navigator.clipboard||!window.isSecureContext){
   if(fallback()){$('diagFeedback').textContent='Diagnosebericht in die Zwischenablage kopiert.';return;}
 }else{
   try{await navigator.clipboard.writeText(text);$('diagFeedback').textContent='Diagnosebericht in die Zwischenablage kopiert.';return;}catch(e){if(fallback()){$('diagFeedback').textContent='Diagnosebericht kopiert.';return;}}
 }
 $('copyManual').hidden=false;$('manualText').value=text;$('manualText').focus();$('manualText').select();
 $('diagFeedback').textContent='Automatisches Kopieren blockiert. Bericht unten markieren und manuell kopieren.';
});
$('diagnostics').addEventListener('toggle',()=>{if($('diagnostics').open)refreshDiagnostics();});
$('diagReload').addEventListener('click',refreshDiagnostics);
updateControls();draw();loadFont();requestAnimationFrame(step);setInterval(poll,1200);poll();
</script>
</body></html>

)HTML";

// Decode UTF-8 into a fixed 40-codepoint buffer; preserve original case.
int decodeText(const String &input, uint32_t *output) {
  int count = 0;
  const uint8_t *data = reinterpret_cast<const uint8_t *>(input.c_str());
  const size_t len = input.length();
  for (size_t i = 0; i < len;) {
    uint8_t c = data[i++];
    uint32_t cp = c;
    if (c >= 0xC2 && c <= 0xDF && i < len) {
      cp = ((c & 0x1F) << 6) | (data[i++] & 0x3F);
    } else if (c >= 0xE0 && c <= 0xEF && i + 1 < len) {
      cp = ((c & 0x0F) << 12) | ((data[i] & 0x3F) << 6) | (data[i+1] & 0x3F);
      i += 2;
    } else if (c >= 0xF0 && c <= 0xF4 && i + 2 < len) {
      cp = ((c & 7) << 18) | ((data[i] & 63) << 12) | ((data[i+1] & 63) << 6) | (data[i+2] & 63);
      i += 3;
    } else if (c >= 0x80) {
      cp = '?';
    }
    if (cp == 0xFE0F || cp == 0xFE0E) continue; // emoji presentation selector
    if (cp == '\r' || cp == '\n' || cp == '\t') cp = ' ';
    if (count >= MAX_CHARS) return -1;
    output[count++] = cp;
  }
  return count;
}

const uint8_t *lookupGlyph(uint32_t code) {
  for (const Glyph &g : FONT) if (g.code == code) return g.rows;
  return UNKNOWN_GLYPH;
}

void setPixel(uint16_t x, uint8_t y, uint8_t color, uint16_t width) {
  if (x >= width || y >= SIGN_HEIGHT) return;
  const uint16_t plane = width * (SIGN_HEIGHT / 8);
  const uint16_t offset = x * (SIGN_HEIGHT / 8) + y / 8;
  const uint8_t mask = 0x80 >> (y & 7);
  if (color & 4) pixelData[offset] |= mask;
  if (color & 2) pixelData[plane + offset] |= mask;
  if (color & 1) pixelData[plane * 2 + offset] |= mask;
}

size_t renderText(const uint32_t *codes, uint8_t count, uint8_t color,
                  uint16_t &width) {
  const int glyphWidth = count * 6 - 1;
  width = max((int)SIGN_WIDTH, glyphWidth);
  const uint16_t rgbBytes = width * (SIGN_HEIGHT / 8) * 3;
  if (rgbBytes > RGB_CAPACITY || 107 + rgbBytes > PAYLOAD_CAPACITY) return 0;
  memset(pixelData, 0, rgbBytes);
  memset(textPayload, 0, 107);
  const int startX = (width - glyphWidth) / 2;
  for (uint8_t i = 0; i < count; ++i) {
    const uint8_t *rows = lookupGlyph(codes[i]);
    for (uint8_t y = 0; y < 7; ++y) {
      for (uint8_t x = 0; x < 5; ++x) {
        if (rows[y] & (1 << (4 - x))) {
          setPixel(startX + 6 * i + x, 1 + 2 * y, color, width);
          setPixel(startX + 6 * i + x, 2 + 2 * y, color, width);
        }
      }
    }
  }
  textPayload[24] = count;
  for (uint8_t i = 0; i < count; ++i) textPayload[25 + i] = 0x30;
  textPayload[105] = rgbBytes >> 8;
  textPayload[106] = rgbBytes & 255;
  memcpy(textPayload + 107, pixelData, rgbBytes);
  return 107 + rgbBytes;
}

// CoolLEDX escaped framing: 01 + escaped(u16be(length) + raw) + 03.
size_t framePacket(const uint8_t *raw, size_t length, uint8_t *frame) {
  size_t n = 0;
  frame[n++] = 0x01;
  auto append = [&](uint8_t b) {
    if (b >= 1 && b <= 3) { frame[n++] = 0x02; frame[n++] = b + 4; }
    else frame[n++] = b;
  };
  append(length >> 8);
  append(length & 255);
  for (size_t i = 0; i < length; ++i) append(raw[i]);
  frame[n++] = 0x03;
  return n;
}

size_t makeChunk(size_t index, size_t payloadLen, uint8_t *raw) {
  const size_t offset = index * DATA_CHUNK;
  const size_t length = min((size_t)DATA_CHUNK, payloadLen - offset);
  size_t p = 0;
  raw[p++] = 0x02; // TEXT
  raw[p++] = 0x00;
  raw[p++] = payloadLen >> 8;
  raw[p++] = payloadLen & 255;
  raw[p++] = index >> 8;
  raw[p++] = index & 255;
  raw[p++] = length;
  memcpy(raw + p, textPayload + offset, length);
  p += length;
  uint8_t xorByte = 0;
  for (size_t i = 1; i < p; ++i) xorByte ^= raw[i];
  raw[p++] = xorByte;
  return p;
}

void onNotify(BLERemoteCharacteristic *, uint8_t *data, size_t length, bool) {
  ++notifyCount;
  notificationBytes += length;
  Serial.print("Antwort der Tafel: ");
  for (size_t i = 0; i < length; ++i) Serial.printf("%02X ", data[i]);
  Serial.println();
}

bool sendPacket(BLERemoteCharacteristic *c, uint8_t *data, size_t length, bool ack) {
  const uint32_t before = notifyCount;
  if (!c->writeValue(data, length, false)) {
    ++diagnostics.bleWriteFailed;
    diagnosticEvent("BLE", "Write-Fehler (nicht uebertragen)", true);
    return false;
  }
  ++diagnostics.bleWriteOK;
  ++diagnostics.chunksSent;
  diagnostics.bytesSent += length;
  if (!ack) { delay(90); return true; }
  const uint32_t start = millis();
  while (notifyCount == before && millis() - start < 2500) delay(10);
  if (notifyCount == before) {
    ++diagnostics.bleTimeouts;
    diagnosticEvent("BLE", "Timeout: keine Notification zum Paket", true);
    return false;
  }
  ++diagnostics.notificationsProcessed;
  diagnosticEvent("BLE", "Notification nach Schreibpaket empfangen");
  delay(60);
  return true;
}

bool sendSimple(BLERemoteCharacteristic *c, uint8_t cmd, uint8_t val) {
  uint8_t raw[] = {cmd, val};
  uint8_t frame[12];
  const size_t n = framePacket(raw, sizeof(raw), frame);
  return sendPacket(c, frame, n, false);
}

String currentState = "idle";
String currentMessage = "Bereit. Noch kein Inhalt uebertragen.";
Job nextJob = NONE;
String pendingText;
uint8_t pendingColor = 7;
uint8_t pendingBrightness = 75;
uint8_t pendingMode = 0x01;
uint8_t pendingSpeed = 50;

String jsonQuote(const String &s) {
  String out = "\"";
  for (size_t i = 0; i < s.length(); ++i) {
    char c = s[i];
    if (c == '\\' || c == '"') { out += '\\'; out += c; }
    else if (c == '\r') out += "\\r";
    else if (c == '\n') out += "\\n";
    else if ((uint8_t)c >= 0x20) out += c;
  }
  out += '"';
  return out;
}

void respond(bool ok, const String &message, int status = 200) {
  web.sendHeader("Cache-Control", "no-store");
  web.send(status, "application/json; charset=utf-8",
           "{\"ok\":" + String(ok ? "true" : "false") + ",\"message\":" + jsonQuote(message) + "}");
}

void onStatus() {
  web.sendHeader("Cache-Control", "no-store");
  web.send(200, "application/json; charset=utf-8",
           "{\"state\":" + jsonQuote(currentState) + ",\"message\":" + jsonQuote(currentMessage) + "}");
}

bool reserveJob() {
  if (currentState == "queued" || currentState == "sending") {
    ++diagnostics.httpRejectedRequests;
    diagnosticEvent("HTTP", "Auftrag abgewiesen: Controller beschäftigt");
    respond(false, "Der Controller ist beschäftigt.", 409);
    return false;
  }
  return true;
}

void queueText() {
  if (!reserveJob()) return;
  const String text = web.arg("text");
  const int color = web.arg("color").toInt();
  const int brightness = web.arg("brightness").toInt();
  const String mode = web.arg("mode");
  const int speed = web.arg("speed").toInt();
  const int count = decodeText(text, characterCodes);
  String trimmed = text;
  trimmed.trim();
  if (count <= 0 || count > MAX_CHARS || trimmed.isEmpty()) {
    ++diagnostics.httpRejectedRequests;
    diagnosticEvent("HTTP", "Ungültige Textlänge oder leer", true);
    respond(false, "Bitte 1 bis 40 Zeichen eingeben.", 400); return;
  }
  if (color < 1 || color > 7 || brightness < 1 || brightness > 100 ||
      speed < 1 || speed > 100 || !(mode == "static" || mode == "scroll")) {
    ++diagnostics.httpRejectedRequests;
    diagnosticEvent("HTTP", "Ungültige Farbe, Helligkeit, Geschwindigkeit oder Modus", true);
    respond(false, "Ungültige Farbe, Helligkeit, Geschwindigkeit oder Anzeigemodus.", 400); return;
  }
  ++diagnostics.httpTextRequests;
  diagnosticEvent("HTTP", String("Textauftrag angenommen, Zeichen=") + count);
  pendingText = text;
  pendingColor = color;
  pendingBrightness = brightness;
  pendingMode = mode == "scroll" ? 0x02 : 0x01;
  pendingSpeed = speed;
  nextJob = TEXT;
  currentState = "queued";
  currentMessage = "Text angenommen. Bluetooth-Übertragung startet …";
  respond(true, currentMessage, 202);
}

void queueBrightness() {
  if (!reserveJob()) return;
  const int brightness = web.arg("brightness").toInt();
  if (brightness < 1 || brightness > 100) {
    ++diagnostics.httpRejectedRequests;
    diagnosticEvent("HTTP", "Helligkeit außerhalb 1..100", true);
    respond(false, "Helligkeit muss zwischen 1 und 100 liegen.", 400); return;
  }
  ++diagnostics.httpBrightnessRequests;
  diagnosticEvent("HTTP", "Helligkeitsauftrag angenommen");
  pendingBrightness = brightness;
  nextJob = BRIGHTNESS;
  currentState = "queued";
  currentMessage = "Helligkeit angenommen. Verbindung startet …";
  respond(true, currentMessage, 202);
}

void disconnectSign() {
  if (bleClient && bleClient->isConnected()) {
    bleClient->disconnect();
    ++diagnostics.bleDisconnected;
    diagnosticEvent("BLE", "Verbindung getrennt");
  }
}

bool runJob(Job job, uint8_t color, uint8_t brightness, const String &message, uint8_t mode, uint8_t speed) {
  Serial.printf("Auftrag: %s\n", job == TEXT ? "TEXT" : "HELLIGKEIT");
  uint16_t width = SIGN_WIDTH;
  size_t payloadLen = 0;
  uint8_t raw[140];
  uint8_t frame[300];
  const uint16_t MAX_FRAME = sizeof(frame);
  if (job == TEXT) {
    const int count = decodeText(message, characterCodes);
    if (count <= 0) { currentMessage = "Ungültiger Text."; return false; }
    payloadLen = renderText(characterCodes, count, color, width);
    if (!payloadLen) { currentMessage = "Text passt nicht in den Pufferspeicher."; return false; }
  }

  BLEAddress address{String(SIGN_ADDRESS)};
  ++diagnostics.bleAttempts;
  diagnosticEvent("BLE", "Verbindungsversuch zu CoolLEDX");
  if (!bleClient->connect(address, 0xFF, 10000)) {
    currentMessage = "LED-Tafel nicht erreichbar. Ist sie eingeschaltet oder mit der Original-App verbunden?";
    diagnosticEvent("BLE", "Verbindungsversuch fehlgeschlagen", true);
    return false;
  }
  ++diagnostics.bleConnected;
  diagnosticEvent("BLE", "Verbindung hergestellt");
  delay(650);
  BLERemoteService *svc = bleClient->getService(BLEUUID((uint16_t)0xFFF0));
  if (!svc) {
    currentMessage = "BLE-Service FFF0 nicht gefunden.";
    disconnectSign(); return false;
  }
  BLERemoteCharacteristic *c = svc->getCharacteristic(BLEUUID((uint16_t)0xFFF1));
  if (!c || !c->canWriteNoResponse()) {
    currentMessage = "FFF1 ist nicht schreibbar.";
    disconnectSign(); return false;
  }
  if (c->canNotify()) c->registerForNotify(onNotify);

  // Check negotiated MTU BEFORE writing any text or mode packets.
  const size_t chunks = (payloadLen + DATA_CHUNK - 1) / DATA_CHUNK;
  const uint16_t mtu = bleClient->getMTU();
  Serial.printf("MTU: %u; Text: %u Bytes; Pakete: %u\n", mtu,
                (unsigned)payloadLen, (unsigned)chunks);
  diagnosticEvent("BLE", String("MTU=") + mtu + " Text-Payload=" + (unsigned)payloadLen + " Bytes, Pakete=" + (unsigned)chunks);
  if (job == TEXT) {
    for (size_t i = 0; i < chunks; ++i) {
      size_t rawN = makeChunk(i, payloadLen, raw);
      size_t frameN = framePacket(raw, rawN, frame);
      if (frameN > MAX_FRAME || frameN + 3 > mtu) {
        currentMessage = "Bluetooth-MTU zu klein fuer die Textpakete.";
        disconnectSign(); return false;
      }
    }
  }

  const uint8_t rawBrightness = (uint16_t(brightness) * 255 + 50) / 100;
  if (!sendSimple(c, 0x08, rawBrightness)) {
    currentMessage = "Helligkeit konnte nicht gesendet werden.";
    disconnectSign(); return false;
  }
  if (job == TEXT) {
    if (!sendSimple(c, 0x06, mode)) {
      currentMessage = "Anzeigemodus konnte nicht gesetzt werden.";
      disconnectSign(); return false;
    }
    // Originalhardware: 0x07, Rohwert 0..255; UI-Regler 1..100. Kalibrierung folgt.
    if (mode == 0x02 && !sendSimple(c, 0x07, (uint16_t(speed) * 255 + 50) / 100)) {
      currentMessage = "Laufgeschwindigkeit konnte nicht gesetzt werden.";
      disconnectSign(); return false;
    }
    for (size_t i = 0; i < chunks; ++i) {
      const size_t rawN = makeChunk(i, payloadLen, raw);
      const size_t frameN = framePacket(raw, rawN, frame);
      Serial.printf("Textpaket %u/%u (%u Bytes)\n", (unsigned)(i+1),
                    (unsigned)chunks, (unsigned)frameN);
      if (!sendPacket(c, frame, frameN, true)) {
        currentMessage = "Textübertragung abgebrochen: Paket " + String((unsigned)(i+1)) +
                         " von " + String((unsigned)chunks) + " nicht bestaetigt.";
        disconnectSign(); return false;
      }
    }
  }
  delay(220);
  disconnectSign();
  currentMessage = job == TEXT
      ? String("Text erfolgreich gesendet. ") + (mode == 0x02 ? "Lauftext." : "Standbild.")
      : "Helligkeitsbefehl gesendet. Anzeigeinhalt unveraendert.";
  return true;
}

void setup() {
  Serial.begin(115200);
  delay(1200);
  Serial.println("RavestickBoard Webcontroller v0.1.2");
  diagnosticEvent("SYSTEM", String("Start v") + FW_VERSION + " Reset=" + resetDescription());
  BLEDevice::init("");
  BLEDevice::setMTU(247);
  bleClient = BLEDevice::createClient();

  WiFi.mode(WIFI_AP);
  if (!WiFi.softAP(WIFI_SSID, WIFI_PASSWORD)) {
    Serial.println("WLAN konnte nicht gestartet werden!");
    currentState = "error";
    currentMessage = "WLAN-Start fehlgeschlagen.";
    diagnosticEvent("WLAN", "Access Point Start fehlgeschlagen", true);
  } else {
    Serial.print("WLAN: "); Serial.println(WIFI_SSID);
    Serial.print("Adresse: http://"); Serial.println(WiFi.softAPIP());
    diagnosticEvent("WLAN", String("AP bereit, SSID=") + WIFI_SSID);
  }

  web.on("/", HTTP_GET, []() {
    web.send_P(200, "text/html; charset=utf-8", PAGE);
  });
  web.on("/api/status", HTTP_GET, onStatus);
  web.on("/api/font", HTTP_GET, onFontJson);
  web.on("/api/diagnostics", HTTP_GET, onDiagnosticsJson);
  web.on("/api/diagnostics.txt", HTTP_GET, onDiagnosticsDownload);
  web.on("/api/text", HTTP_POST, queueText);
  web.on("/api/brightness", HTTP_POST, queueBrightness);
  web.onNotFound([]() { respond(false, "Nicht gefunden.", 404); });
  web.begin();
  Serial.println("Webserver bereit.");
  diagnosticEvent("HTTP", "Webserver gestartet");
}

void loop() {
  web.handleClient();
  const uint8_t wifiClients = WiFi.softAPgetStationNum();
  if (wifiClients != knownWifiClients) {
    knownWifiClients = wifiClients;
    diagnosticEvent("WLAN", String("Verbundene Clients: ") + wifiClients);
  }
  if (nextJob != NONE) {
    const Job job = nextJob;
    const uint8_t color = pendingColor;
    const uint8_t brightness = pendingBrightness;
    const uint8_t mode = pendingMode;
    const uint8_t speed = pendingSpeed;
    const String text = pendingText;
    nextJob = NONE;
    currentState = "sending";
    currentMessage = "Bluetooth-Übertragung läuft …";
    const uint32_t started = millis();
    diagnosticEvent("JOB", job == TEXT ? "Textübertragung startet" : "Helligkeit startet");
    bool ok = runJob(job, color, brightness, text, mode, speed);
    diagnostics.lastJobDurationMs = millis() - started;
    currentState = ok ? "ok" : "error";
    if (ok) ++diagnostics.jobsOK;
    else ++diagnostics.jobsFailed;
    diagnosticEvent("JOB", String(ok ? "OK" : "FEHLER") +
                    " nach " + diagnostics.lastJobDurationMs + " ms: " + currentMessage, !ok);
    Serial.println(currentMessage);
  }
  delay(3);
}
