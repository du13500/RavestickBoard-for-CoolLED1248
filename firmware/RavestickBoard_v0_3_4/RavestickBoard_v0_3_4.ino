/*
  RavestickBoard Webcontroller v0.3.4
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
#include <Preferences.h>
#include "RavestickBoard_WebUI.h"
#include "RavestickBoard_Assets.h"

// Named types are placed BEFORE their functions so Arduino's generated prototypes
// don't see an unknown type (a regression we fixed after the v0.1.2 test).
enum Job : uint8_t { NONE, TEXT, BRIGHTNESS, PAGE, SPEED, MODE };
static constexpr uint8_t PAGE_LIMIT = 12;
// Mixed text/symbol lines are constrained by their actual rendered pixel width.
static constexpr uint16_t PAGE_MIN_SECONDS = 5;

struct PageRecord {
  char line1[48];
  char line2[48];
  uint8_t layout;    // 0=Eng 14px, 1=Mini two rows, 2=Mini center, 3=Normal, 4=Bold
  uint8_t align;     // 0=left, 1=center, 2=right
  uint8_t color1;    // 3-bit RGB (1..7)
  uint8_t color2;
  uint16_t seconds;  // on-screen duration AFTER transfer completes
};

// Defined before Arduino-generated function prototypes (auto-prototype safe).
struct TextMeasure { uint32_t chars; uint32_t pixels; };

bool runJob(Job job, uint8_t color, uint8_t brightness,
            const String &message, uint8_t mode, uint8_t speed,
            uint8_t pageIndex);
String jsonQuote(const String &s);

static constexpr char FW_VERSION[] = "0.3.4";
static constexpr char APP_NAME[] = "RavestickBoard";
static constexpr char WIFI_SSID[] = "CoolLED1248";
static constexpr char WIFI_PASSWORD[] = "Banane1248!"; // Public default; configurable credentials are planned.
static constexpr char SIGN_ADDRESS[] = "ff:00:00:03:7e:c3";

uint8_t pendingLayout = 0;
static constexpr uint16_t SIGN_WIDTH = 64;
static constexpr uint16_t SIGN_HEIGHT = 16;
// Provisional data budget, NOT a confirmed CoolLEDX hardware maximum.
// Last observed: 29,837 B displayed; ~31,853 B transmitted but NOT displayed.
// BLE legacy header has only 80 known format slots. Preserve that behavior.
static constexpr uint32_t MAX_IMAGE_BYTES = 30UL * 1024UL;
static constexpr uint32_t IMAGE_HEADER_BYTES = 107;
static constexpr uint32_t BYTES_PER_COLUMN = (SIGN_HEIGHT / 8) * 3;
static constexpr uint32_t MAX_COLUMNS = (MAX_IMAGE_BYTES - IMAGE_HEADER_BYTES) / BYTES_PER_COLUMN;
static constexpr uint32_t RGB_CAPACITY = MAX_COLUMNS * BYTES_PER_COLUMN;
static constexpr uint32_t PAYLOAD_CAPACITY = IMAGE_HEADER_BYTES + RGB_CAPACITY;
static constexpr size_t DATA_CHUNK = 128;

WebServer web(80);
BLEClient *bleClient = nullptr;
volatile uint32_t notifyCount = 0;

// Correction 2: Never reserve ~122 KiB of image data at boot.
// Use ONE temporary, exactly sized allocation per text/page transfer.
// The pixel bitmap is written directly into the payload after its 107-byte header.
uint8_t *textPayload = nullptr;
uint8_t *pixelData = nullptr;

bool allocateRenderBuffer(uint32_t rgbBytes) {
  free(textPayload);
  textPayload = nullptr;
  pixelData = nullptr;
  if (rgbBytes == 0 || rgbBytes > RGB_CAPACITY || IMAGE_HEADER_BYTES + rgbBytes > MAX_IMAGE_BYTES) return false;
  const size_t needed = 107UL + rgbBytes;
  textPayload = static_cast<uint8_t *>(malloc(needed));
  if (!textPayload) {
    Serial.printf("[RAM] Bildpuffer %u B fehlgeschlagen; frei=%u B, groesster Block=%u B\n",
                  (unsigned)needed, (unsigned)ESP.getFreeHeap(), (unsigned)ESP.getMaxAllocHeap());
    return false;
  }
  pixelData = textPayload + 107;
  memset(textPayload, 0, needed);
  Serial.printf("[RAM] Bildpuffer %u B reserviert; frei=%u B\n",
                (unsigned)needed, (unsigned)ESP.getFreeHeap());
  return true;
}

struct RenderBufferCleanup {
  ~RenderBufferCleanup() {
    free(textPayload);
    textPayload = nullptr;
    pixelData = nullptr;
  }
};

// v0.3.4: Lauftext is decoded as a stream. No 600-character buffer or hard
// character limit is required; only the rendered data budget is enforced.

// A page program lives on the ESP32 and survives a power cycle via NVS.
// It does NOT auto-start after a reboot; the LED sign retains its last picture.
Preferences prefs;
PageRecord boardPages[PAGE_LIMIT] = {};
uint8_t pageCount = 1;
uint8_t currentPage = 0;
bool autoProgram = false;
uint8_t autoNextPage = 0;
uint32_t autoNextAtMs = 0;
uint32_t pageShows = 0;
uint32_t autoCycles = 0;
uint32_t pageSaveCount = 0;
uint8_t pendingPage = 0;


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
  {0x1E9E, {14,17,18,20,18,17,30}}, // ẞ: bewusst dieselbe schmale Form
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
  out += "Dateisystem: nicht eingebunden; Seiten persistent in NVS; Log im RAM\n";
  out += "Seiten gespeichert: "; out += String(pageCount); out += "\n";
  out += "Seitenprogramm läuft: "; out += autoProgram ? "ja\n" : "nein\n";
  out += "Aktuelle Seite: "; out += String(currentPage + 1); out += "\n";
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
  out += "Seitenübertragungen OK: "; out += String(pageShows); out += '\n';
  out += "NVS-Seitenspeicherungen: "; out += String(pageSaveCount); out += '\n';
  out += "Automatische Seitenwechsel: "; out += String(autoCycles); out += '\n';
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
  json += ",\"pageShows\":"; json += String(pageShows);
  json += ",\"savedPages\":"; json += String(pageCount);
  json += ",\"autoProgram\":"; json += autoProgram ? "true" : "false";
  json += ",\"recent\":"; json += jsonQuote(recentEvents(14));
  json += ",\"lastError\":"; json += jsonQuote(latestError);
  json += "}";
  web.sendHeader("Cache-Control", "no-store");
  web.send(200, "application/json; charset=utf-8", json);
}

void onDiagnosticsDownload() {
  web.sendHeader("Cache-Control", "no-store");
  web.sendHeader("Content-Disposition", String("attachment; filename=RavestickBoard_Diagnose_v") + FW_VERSION + ".txt");
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
// The full HTML/JavaScript UI is defined in RavestickBoard_WebUI.h.
// Arduino must not attempt to auto-prototype JavaScript functions.

// Decode short NVS block-page fields only; preserved for binary compatibility.
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
    if (cp == 0xFE0E) { // VS15 after banana selects our separately drawn Ravestick icon.
      if (count > 0 && output[count - 1] == 0x1F34C) output[count - 1] = 0xE34C;
      continue;
    }
    if (cp == 0xFE0F) continue; // standard emoji presentation selector
    if (cp == '\r' || cp == '\n' || cp == '\t') cp = ' ';
    if (count >= 48) return -1;
    output[count++] = cp;
  }
  return count;
}

// Streaming UTF-8 decoder: no global codepoint arrays for long Lauftext.
// A trailing VS15 selects the separate Ravestick banana (same as decodeText).
uint32_t readUtf8Codepoint(const String &input, size_t &pos) {
  if (pos >= input.length()) return 0xFFFFFFFFUL;
  const uint8_t *data = reinterpret_cast<const uint8_t *>(input.c_str());
  const size_t len = input.length();
  uint8_t c = data[pos++];
  uint32_t cp = c;
  if (c >= 0xC2 && c <= 0xDF && pos < len)
    cp = ((c & 0x1F) << 6) | (data[pos++] & 0x3F);
  else if (c >= 0xE0 && c <= 0xEF && pos + 1 < len) {
    cp = ((c & 0x0F) << 12) | ((data[pos] & 0x3F) << 6) | (data[pos + 1] & 0x3F);
    pos += 2;
  } else if (c >= 0xF0 && c <= 0xF4 && pos + 2 < len) {
    cp = ((c & 7) << 18) | ((data[pos] & 63) << 12) | ((data[pos + 1] & 63) << 6) | (data[pos + 2] & 63);
    pos += 3;
  } else if (c >= 0x80) cp = '?';
  return cp;
}

bool nextTextCodepoint(const String &input, size_t &pos, uint32_t &cp) {
  while (pos < input.length()) {
    cp = readUtf8Codepoint(input, pos);
    if (cp == 0xFE0E || cp == 0xFE0F) continue;
    if (cp == 0x1F34C && pos < input.length()) {
      size_t lookahead = pos;
      if (readUtf8Codepoint(input, lookahead) == 0xFE0E) {
        pos = lookahead;
        cp = 0xE34C;
      }
    }
    if (cp == '\r' || cp == '\n' || cp == '\t') cp = ' ';
    return true;
  }
  return false;
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

// Reconstructed manufacturer glyphs and 16x16 symbols are stored in flash.
// Glyph encoding: Unicode BMP u16 (LE), visible width u8, 16 raster rows u16 (LE).
static const uint8_t *findOriginalGlyph(uint32_t cp, uint8_t layout) {
  if (cp > 0xFFFF) return nullptr;
  const uint8_t *base = layout == 4 ? RB_FONT_BOLD : RB_FONT_NORMAL;
  for (uint8_t i = 0; i < RB_ORIG_GLYPHS_PER_FACE; ++i) {
    const uint8_t *record = base + i * 35;
    const uint16_t code = pgm_read_byte(record) | (uint16_t(pgm_read_byte(record + 1)) << 8);
    if (code == cp) return record;
  }
  return nullptr;
}

// 24 original icons and one separate RavestickBoard banana.
static const uint32_t RB_SYMBOL_CODES[RB_ORIGINAL_SYMBOL_COUNT] = {
  0x1F413, 0x2B50, 0x2744, 0x2764, 0x260E, 0x1F440,
  0x1F6D1, 0x26A0, 0x1F53A, 0x1F412, 0x1F41F, 0x1F980,
  0x1F34E, 0x1F347, 0x1F34C, 0x1F418, 0x1F95A, 0x1F357,
  0x1F3C6, 0x1F68C, 0x1F680, 0x2601, 0x1F308, 0x26A1
};

// Emoji hearts map to all seven LED colors; the pixel silhouette is the
// reconstructed original heart, but its source color is intentionally overridden.
int symbolIndex(uint32_t cp, uint8_t &recolor) {
  recolor = 0;
  switch (cp) {
    case 0x2764: recolor = 4; return 3;   // red
    case 0x1F49B: recolor = 6; return 3;  // yellow
    case 0x1F49A: recolor = 2; return 3;  // green
    case 0x1FA75: recolor = 3; return 3;  // light blue = cyan
    case 0x1F499: recolor = 1; return 3;  // blue
    case 0x1F49C: recolor = 5; return 3;  // violet = magenta
    case 0x1F90D: recolor = 7; return 3;  // white
    case 0xE34C: return 24;              // RavestickBoard banana (vertical mirror)
  }
  for (uint8_t i = 0; i < RB_ORIGINAL_SYMBOL_COUNT; ++i) {
    if (RB_SYMBOL_CODES[i] == cp) return i;
  }
  return -1;
}

uint8_t symbolColor(uint8_t icon, uint8_t x, uint8_t y) {
  const uint8_t packed = pgm_read_byte(RB_SYMBOLS_PACKED + uint16_t(icon) * 128 + uint16_t(y) * 8 + x / 2);
  return x & 1 ? packed & 0x0F : packed >> 4;
}

void symbolBounds(uint8_t icon, uint8_t &xmin, uint8_t &xmax) {
  xmin = 16; xmax = 0;
  for (uint8_t x = 0; x < 16; ++x) for (uint8_t y = 0; y < 16; ++y) {
    if (symbolColor(icon, x, y)) {
      if (x < xmin) xmin = x;
      if (x > xmax) xmax = x;
    }
  }
  if (xmin == 16) { xmin = 0; xmax = 0; }
}

uint8_t tokenWidth(uint32_t cp, uint8_t layout) {
  uint8_t color = 0;
  const int icon = symbolIndex(cp, color);
  if (icon >= 0) {
    uint8_t a, b; symbolBounds(icon, a, b);
    return b - a + 1;
  }
  if (layout == 3 || layout == 4) {
    const uint8_t *record = findOriginalGlyph(cp, layout);
    if (record) return pgm_read_byte(record + 2);
  }
  // Compact spaces in the two Mini fonts; other layouts keep their spacing.
  if (cp == ' ' && (layout == 1 || layout == 2)) return 2;
  return 5;
}

// Mini is uppercase-only ON THE DISPLAY. The original UTF-8 text is kept in NVS.
uint32_t displayCp(uint32_t cp, uint8_t layout) {
  if (layout != 1 && layout != 2) return cp;
  if (cp >= 'a' && cp <= 'z') return cp - 32;
  if (cp == 0xE4) return 0xC4;
  if (cp == 0xF6) return 0xD6;
  if (cp == 0xFC) return 0xDC;
  if (cp == 0xDF) return 0x1E9E;
  return cp;
}

int linePixelWidth(const uint32_t *codes, int count, uint8_t layout) {
  if (count <= 0) return 0;
  int w = -1;
  for (int i = 0; i < count; ++i) w += tokenWidth(displayCp(codes[i], layout), layout) + 1;
  return w;
}

void drawInline(const uint32_t *codes, int count, uint8_t layout, uint8_t align,
                uint8_t top, uint8_t color, uint16_t width) {
  const int used = linePixelWidth(codes, count, layout);
  // Layout 2: mini single is ALWAYS centered horizontally and vertically.
  if (layout == 2) align = 1;
  int x0 = align == 0 ? 0 : align == 2 ? int(width) - used : (int(width) - used) / 2;
  for (int i = 0; i < count; ++i) {
    const uint32_t cp = displayCp(codes[i], layout);
    uint8_t overrideColor = 0;
    const int icon = symbolIndex(cp, overrideColor);
    if (icon >= 0) {
      uint8_t minx, maxx; symbolBounds(icon, minx, maxx);
      for (uint8_t y = 0; y < 16; ++y) for (uint8_t x = minx; x <= maxx; ++x) {
        const uint8_t col = symbolColor(icon, x, y);
        if (col && x0 + x - minx >= 0 && x0 + x - minx < width)
          setPixel(x0 + x - minx, y, overrideColor ? overrideColor : col, width);
      }
    } else if (layout == 3 || layout == 4) {
      const uint8_t *record = findOriginalGlyph(cp, layout);
      if (record) {
        const uint8_t gw = pgm_read_byte(record + 2);
        for (uint8_t y = 0; y < 16; ++y) {
          const uint16_t bits = pgm_read_byte(record + 3 + 2 * y) | (uint16_t(pgm_read_byte(record + 4 + 2 * y)) << 8);
          for (uint8_t x = 0; x < gw; ++x) if (bits & (uint16_t(1) << (gw - 1 - x))) {
            if (x0 + x >= 0 && x0 + x < width) setPixel(x0 + x, y, color, width);
          }
        }
      }
    } else {
      const uint8_t *rows = lookupGlyph(cp);
      const uint8_t scale = layout == 0 ? 2 : 1;
      for (uint8_t y = 0; y < 7; ++y) for (uint8_t x = 0; x < 5; ++x) {
        if (!(rows[y] & (1 << (4 - x)))) continue;
        for (uint8_t z = 0; z < scale; ++z)
          if (x0 + x >= 0 && x0 + x < width) setPixel(x0 + x, top + y * scale + z, color, width);
      }
    }
    x0 += tokenWidth(cp, layout) + 1; // temporary 1px gap; original app spacing needs measurement
  }
}

// Width and count are computed with exactly the same tokenWidth/displayCp
// as the firmware renderer. Limit is pixel/byte based, not character based.
TextMeasure measureText(const String &text, uint8_t layout) {
  TextMeasure m = {0, 0};
  size_t pos = 0;
  uint32_t raw = 0;
  while (nextTextCodepoint(text, pos, raw)) {
    const uint32_t step = uint32_t(tokenWidth(displayCp(raw, layout), layout)) + 1;
    ++m.chars;
    m.pixels += step;
    if (m.pixels > MAX_COLUMNS + 1) break; // budget exceeded; reject early
  }
  if (m.chars) --m.pixels; // no trailing inter-character gap
  return m;
}

void drawInlineUtf8(const String &text, uint8_t layout, uint8_t align,
                    uint8_t top, uint8_t color, uint16_t width) {
  const TextMeasure m = measureText(text, layout);
  if (layout == 2) align = 1;
  int x0 = align == 0 ? 0 : align == 2 ? int(width) - int(m.pixels) : (int(width) - int(m.pixels)) / 2;
  size_t pos = 0;
  uint32_t raw = 0;
  while (nextTextCodepoint(text, pos, raw)) {
    const uint32_t cp = displayCp(raw, layout);
    uint8_t overrideColor = 0;
    const int icon = symbolIndex(cp, overrideColor);
    if (icon >= 0) {
      uint8_t minx, maxx; symbolBounds(icon, minx, maxx);
      for (uint8_t y = 0; y < 16; ++y) for (uint8_t x = minx; x <= maxx; ++x) {
        const uint8_t col = symbolColor(icon, x, y);
        if (col && x0 + x - minx >= 0 && x0 + x - minx < width)
          setPixel(x0 + x - minx, y, overrideColor ? overrideColor : col, width);
      }
    } else if (layout == 3 || layout == 4) {
      const uint8_t *record = findOriginalGlyph(cp, layout);
      if (record) {
        const uint8_t gw = pgm_read_byte(record + 2);
        for (uint8_t y = 0; y < 16; ++y) {
          const uint16_t bits = pgm_read_byte(record + 3 + 2 * y) | (uint16_t(pgm_read_byte(record + 4 + 2 * y)) << 8);
          for (uint8_t x = 0; x < gw; ++x) if (bits & (uint16_t(1) << (gw - 1 - x))) {
            if (x0 + x >= 0 && x0 + x < width) setPixel(x0 + x, y, color, width);
          }
        }
      }
    } else {
      const uint8_t *rows = lookupGlyph(cp);
      const uint8_t scale = layout == 0 ? 2 : 1;
      for (uint8_t y = 0; y < 7; ++y) for (uint8_t x = 0; x < 5; ++x) {
        if (!(rows[y] & (1 << (4 - x)))) continue;
        for (uint8_t z = 0; z < scale; ++z)
          if (x0 + x >= 0 && x0 + x < width) setPixel(x0 + x, top + y * scale + z, color, width);
      }
    }
    x0 += tokenWidth(cp, layout) + 1;
  }
}

size_t renderText(const String &text, uint8_t color, uint16_t &width) {
  const TextMeasure m = measureText(text, pendingLayout);
  if (m.pixels < 1 || m.pixels > MAX_COLUMNS) return 0;
  width = m.pixels < SIGN_WIDTH ? SIGN_WIDTH : (uint16_t)m.pixels;
  const uint32_t rgbBytes = uint32_t(width) * BYTES_PER_COLUMN;
  if (!allocateRenderBuffer(rgbBytes)) return 0;
  drawInlineUtf8(text, pendingLayout, 1, pendingLayout == 0 ? 1 : pendingLayout == 2 ? 4 : 0, color, width);
  textPayload[24] = (uint8_t)min(m.chars, (uint32_t)80);
  for (uint8_t i = 0; i < textPayload[24]; ++i) textPayload[25 + i] = 0x30;
  textPayload[105] = rgbBytes >> 8;
  textPayload[106] = rgbBytes & 255;
  return IMAGE_HEADER_BYTES + rgbBytes;
}

size_t renderBlockPage(const PageRecord &p) {
  // PageRecord lines contain at most 47 UTF-8 bytes each, so 48 codepoints suffice.
  // Avoid two unnecessary large arrays on the small ESP32-C3 loop stack.
  uint32_t topCodes[48] = {}, bottomCodes[48] = {};
  const int n1 = decodeText(String(p.line1), topCodes);
  const int n2 = p.layout == 1 ? decodeText(String(p.line2), bottomCodes) : 0;
  if (n1 < 0 || n2 < 0 || (n1 == 0 && n2 == 0)) return 0;
  const int w1 = linePixelWidth(topCodes, n1, p.layout);
  const int w2 = linePixelWidth(bottomCodes, n2, p.layout);
  if (w1 > SIGN_WIDTH || w2 > SIGN_WIDTH) return 0; // never crop text or symbols
  const uint16_t rgbBytes = SIGN_WIDTH * (SIGN_HEIGHT / 8) * 3;
  if (!allocateRenderBuffer(rgbBytes)) return 0;
  if (p.layout == 1) {
    drawInline(topCodes, n1, 1, p.align, 0, p.color1, SIGN_WIDTH);
    drawInline(bottomCodes, n2, 1, p.align, 8, p.color2, SIGN_WIDTH);
  } else {
    const uint8_t top = p.layout == 0 ? 1 : p.layout == 2 ? 4 : 0;
    drawInline(topCodes, n1, p.layout, p.align, top, p.color1, SIGN_WIDTH);
  }
  const uint8_t characters = n1 + n2;
  textPayload[24] = characters;
  for (uint8_t i = 0; i < characters; ++i) textPayload[25 + i] = 0x30;
  textPayload[105] = rgbBytes >> 8;
  textPayload[106] = rgbBytes & 0xFF;
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
String pendingSecondLine;

uint8_t pendingColor = 7;
uint8_t pendingColor2 = 7; // independent bottom-line color for Mini Zweizeilig Lauftext
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
           "{\"state\":" + jsonQuote(currentState) + ",\"message\":" + jsonQuote(currentMessage) + ",\"playing\":" + String(autoProgram ? "true" : "false") + ",\"current\":" + String(currentPage) + "}");
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

void defaultPage(PageRecord &p) {
  memset(&p, 0, sizeof(p));
  p.line1[0] = 0;
  p.line2[0] = 0;
  p.layout = 0; p.align = 1; p.color1 = 6; p.color2 = 7;
  p.seconds = 7;
}

void loadPages() {
  for (auto &p : boardPages) defaultPage(p);
  pageCount = 1;
  if (!prefs.begin("rbpages", false)) {
    diagnosticEvent("NVS", "Seitenspeicher konnte nicht geöffnet werden", true);
    return;
  }
  if (prefs.getUChar("schema", 0) == 1 &&
      prefs.getBytesLength("data") == sizeof(boardPages)) {
    prefs.getBytes("data", boardPages, sizeof(boardPages));
    pageCount = constrain((int)prefs.getUChar("count", 1), 1, (int)PAGE_LIMIT);
    for (uint8_t i = 0; i < pageCount; ++i) {
      PageRecord &p = boardPages[i];
      p.line1[sizeof(p.line1) - 1] = 0;
      p.line2[sizeof(p.line2) - 1] = 0;
      p.layout = min((int)p.layout, 4);
      p.align = min((int)p.align, 2);
      p.color1 = constrain((int)p.color1, 1, 7);
      p.color2 = constrain((int)p.color2, 1, 7);
      p.seconds = constrain((int)p.seconds, (int)PAGE_MIN_SECONDS, 60);
    }
    diagnosticEvent("NVS", String("Seiten geladen: ") + pageCount);
  } else {
    diagnosticEvent("NVS", "Erste Initialisierung: eine leere Seite");
  }
}

bool savePages() {
  if (!prefs.isKey("schema")) prefs.putUChar("schema", 1);
  const size_t wrote = prefs.putBytes("data", boardPages, sizeof(boardPages));
  const size_t counted = prefs.putUChar("count", pageCount);
  if (wrote != sizeof(boardPages) || counted != 1) {
    diagnosticEvent("NVS", "Speichern der Seiten fehlgeschlagen", true);
    return false;
  }
  ++pageSaveCount;
  diagnosticEvent("NVS", String("Seiten gespeichert, Anzahl=") + pageCount);
  return true;
}

void onProgramGet() {
  String out;
  out.reserve(2800);
  out += "{\"ok\":true,\"count\":"; out += String(pageCount);
  out += ",\"playing\":"; out += autoProgram ? "true" : "false";
  out += ",\"current\":"; out += String(currentPage);
  out += ",\"pages\":[";
  for (uint8_t i = 0; i < pageCount; ++i) {
    if (i) out += ',';
    PageRecord &p = boardPages[i];
    out += "{\"line1\":"; out += jsonQuote(String(p.line1));
    out += ",\"line2\":"; out += jsonQuote(String(p.line2));
    out += ",\"layout\":"; out += String(p.layout);
    out += ",\"align\":"; out += String(p.align);
    out += ",\"color1\":"; out += String(p.color1);
    out += ",\"color2\":"; out += String(p.color2);
    out += ",\"duration\":"; out += String(p.seconds);
    out += '}';
  }
  out += "]}";
  web.sendHeader("Cache-Control", "no-store");
  web.send(200, "application/json; charset=utf-8", out);
}

bool validIndex(int idx) { return idx >= 0 && idx < pageCount; }

void onProgramSet() {
  const int idx = web.arg("index").toInt();
  const String top = web.arg("line1"), bottom = web.arg("line2");
  const int layout = web.arg("layout").toInt();
  const int align = web.arg("align").toInt();
  const int c1 = web.arg("color1").toInt(), c2 = web.arg("color2").toInt();
  const int seconds = web.arg("duration").toInt();
  uint32_t topCodes[48] = {}, bottomCodes[48] = {};
  const int n1 = decodeText(top, topCodes), n2 = decodeText(bottom, bottomCodes);
  if (!web.hasArg("index") || !validIndex(idx) || top.length() >= sizeof(boardPages[0].line1) ||
      bottom.length() >= sizeof(boardPages[0].line2) || n1 < 0 || n2 < 0 ||
      (n1 == 0 && (layout != 1 || n2 == 0)) ||
      layout < 0 || layout > 4 || align < 0 || align > 2 ||
      c1 < 1 || c1 > 7 || c2 < 1 || c2 > 7 || seconds < PAGE_MIN_SECONDS || seconds > 60 ||
      linePixelWidth(topCodes, n1, layout) > SIGN_WIDTH ||
      (layout == 1 && linePixelWidth(bottomCodes, n2, layout) > SIGN_WIDTH)) {
    ++diagnostics.httpRejectedRequests;
    respond(false, "Seite ungültig: Text/Symbole breiter als 64 Pixel oder zu viele UTF-8-Bytes.", 400);
    return;
  }
  PageRecord &p = boardPages[idx];
  snprintf(p.line1, sizeof(p.line1), "%s", top.c_str());
  snprintf(p.line2, sizeof(p.line2), "%s", bottom.c_str());
  p.layout = layout; p.align = align;
  p.color1 = c1; p.color2 = c2; p.seconds = seconds;
  if (!savePages()) { respond(false, "Speicherfehler beim Sichern der Seite.", 500); return; }
  respond(true, "Seite auf dem ESP32 gespeichert.");
}

void onProgramAdd() {
  if (pageCount >= PAGE_LIMIT) { respond(false, "Maximal 12 Seiten möglich.", 400); return; }
  const int copy = web.hasArg("copy") ? web.arg("copy").toInt() : -1;
  if (copy >= 0 && validIndex(copy)) boardPages[pageCount] = boardPages[copy];
  else { defaultPage(boardPages[pageCount]); boardPages[pageCount].line1[0] = 0; }
  ++pageCount;
  if (!savePages()) { respond(false, "Speicherfehler.", 500); return; }
  respond(true, "Seite hinzugefügt.");
}
void onProgramDelete() {
  const int index = web.arg("index").toInt();
  if (!web.hasArg("index") || !validIndex(index) || pageCount <= 1) { respond(false, "Diese Seite kann nicht gelöscht werden.", 400); return; }
  autoProgram = false;
  for (int i = index; i < pageCount - 1; ++i) boardPages[i] = boardPages[i + 1];
  --pageCount;
  defaultPage(boardPages[pageCount]);
  currentPage = 0;
  if (!savePages()) { respond(false, "Speicherfehler.", 500); return; }
  respond(true, "Seite gelöscht.");
}
void onProgramMove() {
  const int index = web.arg("index").toInt();
  const int dir = web.arg("dir").toInt();
  if (!web.hasArg("index") || !validIndex(index) || (dir != -1 && dir != 1) || !validIndex(index + dir)) {
    respond(false, "Ungültige Seitenreihenfolge.", 400); return;
  }
  autoProgram = false;
  PageRecord tmp = boardPages[index];
  boardPages[index] = boardPages[index + dir];
  boardPages[index + dir] = tmp;
  currentPage = 0;
  if (!savePages()) { respond(false, "Speicherfehler.", 500); return; }
  respond(true, "Reihenfolge geändert.");
}
void onProgramShow() {
  if (!reserveJob()) return;
  const int idx = web.arg("index").toInt();
  if (!web.hasArg("index") || !validIndex(idx)) { respond(false, "Unbekannte Seite.", 400); return; }
  autoProgram = false;
  pendingPage = idx; nextJob = PAGE;
  currentState = "queued";
  currentMessage = "Seite wird übertragen …";
  diagnosticEvent("PAGE", String("Manueller Sendeauftrag, Seite ") + (idx+1));
  respond(true, currentMessage, 202);
}
void onProgramStart() {
  if (!reserveJob()) return;
  autoProgram = true;
  autoNextPage = 0;
  autoNextAtMs = millis();
  diagnosticEvent("PAGE", String("Seitenprogramm gestartet, Seiten=") + pageCount);
  respond(true, "Seitenprogramm gestartet. Erste Seite wird übertragen.", 202);
}
void onProgramStop() {
  autoProgram = false;
  diagnosticEvent("PAGE", "Seitenprogramm gestoppt");
  respond(true, "Seitenprogramm gestoppt. Letztes Bild bleibt auf der Tafel.");
}

void queueText() {
  if (!reserveJob()) return;
  const String text = web.arg("text");
  const String second = web.arg("line2");
  const int color = web.arg("color").toInt();
  const int color2 = web.hasArg("color2") ? web.arg("color2").toInt() : color;
  const int brightness = web.arg("brightness").toInt();
  const String mode = web.arg("mode");
  const int speed = web.arg("speed").toInt();
  const uint8_t layout = constrain(web.arg("layout").toInt(), 0, 4);
  String trimmed = text;
  trimmed.trim();
  String trimmedSecond = second;
  trimmedSecond.trim();
  const TextMeasure first = measureText(text, layout);
  const TextMeasure bottom = layout == 1 ? measureText(second, layout) : TextMeasure{0, 0};
  const uint32_t pixels = max(first.pixels, bottom.pixels);
  const uint32_t bytes = IMAGE_HEADER_BYTES + (pixels < SIGN_WIDTH ? SIGN_WIDTH : pixels) * BYTES_PER_COLUMN;
  if ((trimmed.isEmpty() && (layout != 1 || trimmedSecond.isEmpty())) || bytes > MAX_IMAGE_BYTES) {
    ++diagnostics.httpRejectedRequests;
    diagnosticEvent("HTTP", String("Lauftext abgewiesen: Daten=") + bytes + " B / Budget=" + MAX_IMAGE_BYTES + " B", true);
    respond(false, "Bilddatenlimit überschritten oder Lauftext leer. Text bleibt im Editor.", 400);
    return;
  }
  if (color < 1 || color > 7 || (layout == 1 && (color2 < 1 || color2 > 7)) ||
      brightness < 1 || brightness > 100 || speed < 1 || speed > 100 ||
      !(mode == "static" || mode == "scroll")) {
    ++diagnostics.httpRejectedRequests;
    diagnosticEvent("HTTP", "Ungültige Farbe, Helligkeit, Geschwindigkeit oder Modus", true);
    respond(false, "Ungültige Farbe, Helligkeit, Geschwindigkeit oder Anzeigemodus.", 400);
    return;
  }
  ++diagnostics.httpTextRequests;
  diagnosticEvent("HTTP", String("Textauftrag angenommen: Zeichen=") + (first.chars + bottom.chars) +
                  ", Spalten=" + pixels + ", Daten=" + bytes + " B");
  autoProgram = false;
  pendingText = text;
  pendingSecondLine = second;
  pendingLayout = layout;
  pendingColor = color;
  pendingColor2 = color2;
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

void queueSpeed() {
  if (!reserveJob()) return;
  const int val = web.arg("speed").toInt();
  if (val < 1 || val > 100) { respond(false, "Tempo 1 bis 100 angeben.", 400); return; }
  pendingSpeed = val;
  nextJob = SPEED; currentState = "queued";
  currentMessage = "Tempo wird ohne Bild-Upload angepasst.";
  respond(true, currentMessage, 202);
}
// CoolLEDX SetMode: command 0x06, value 0x01..0x08.
// 0x03..0x08 originate from reference driver; not yet verified on our own sign.
// Keep /api/text compatible with its existing 'scroll'/'static' modes.
uint8_t liveModeValue(const String &mode) {
  if (mode == "static") return 0x01;
  if (mode == "scroll") return 0x02;
  if (mode == "right") return 0x03;
  if (mode == "up") return 0x04;
  if (mode == "down") return 0x05;
  if (mode == "snowflake") return 0x06;
  if (mode == "picture") return 0x07;
  if (mode == "laser") return 0x08;
  return 0x00;
}
void queueMode() {
  if (!reserveJob()) return;
  const String mode = web.arg("mode");
  const uint8_t value = liveModeValue(mode);
  if (!value) { respond(false, "Unbekannter Modus.", 400); return; }
  pendingMode = value;
  nextJob = MODE; currentState = "queued";
  char hexValue[8];
  snprintf(hexValue, sizeof(hexValue), "0x%02X", (unsigned)value);
  diagnosticEvent("MODE", String("0x06 Steuerbefehl, Moduswert ") + hexValue + (value > 0x02 ? " (experimentell)" : ""));
  currentMessage = String("Anzeigemodus ") + hexValue + " wird ohne Bild-Upload angepasst.";
  respond(true, currentMessage, 202);
}

uint32_t controlIdleUntilMs = 0;

void disconnectSign() {
  if (bleClient && bleClient->isConnected()) {
    bleClient->disconnect();
    ++diagnostics.bleDisconnected;
    diagnosticEvent("BLE", "Verbindung getrennt");
  }
}

bool runJob(Job job, uint8_t color, uint8_t brightness, const String &message, uint8_t mode, uint8_t speed, uint8_t pageIndex) {
  // Releases temporary image memory on every exit path (success and error).
  RenderBufferCleanup renderBufferCleanup;
  Serial.printf("Auftrag: %s\n", job == TEXT ? "TEXT" : job == PAGE ? "SEITE" : job == BRIGHTNESS ? "HELLIGKEIT" : job == SPEED ? "TEMPO" : "MODUS");
  uint16_t width = SIGN_WIDTH;
  size_t payloadLen = 0;
  uint8_t raw[140];
  uint8_t frame[300];
  const uint16_t MAX_FRAME = sizeof(frame);
  if (job == TEXT) {
    const TextMeasure first = measureText(message, pendingLayout);
    const TextMeasure bottom = pendingLayout == 1 ? measureText(pendingSecondLine, 1) : TextMeasure{0, 0};
    const uint32_t columns = max(first.pixels, bottom.pixels);
    if (columns > MAX_COLUMNS || !columns) {
      currentMessage = "Text überschreitet das Bilddatenlimit.";
      return false;
    }
    if (pendingLayout == 1) {
      width = columns < SIGN_WIDTH ? SIGN_WIDTH : (uint16_t)columns;
      const uint32_t rgbBytes = uint32_t(width) * BYTES_PER_COLUMN;
      if (!allocateRenderBuffer(rgbBytes)) {
        currentMessage = "Nicht genug zusammenhängender RAM für diesen Lauftext.";
        return false;
      }
      drawInlineUtf8(message, 1, 0, 0, color, width);
      drawInlineUtf8(pendingSecondLine, 1, 0, 8, pendingColor2, width);
      textPayload[24] = (uint8_t)min(first.chars + bottom.chars, (uint32_t)80);
      for (uint8_t i = 0; i < textPayload[24]; ++i) textPayload[25 + i] = 0x30;
      textPayload[105] = rgbBytes >> 8;
      textPayload[106] = rgbBytes & 255;
      payloadLen = IMAGE_HEADER_BYTES + rgbBytes;
    } else {
      payloadLen = renderText(message, color, width);
    }
    if (!payloadLen) {
      currentMessage = "Text passt nicht in den Pufferspeicher.";
      return false;
    }
  } else if (job == PAGE) {
    if (pageIndex >= pageCount) { currentMessage = "Seite nicht vorhanden."; return false; }
    payloadLen = renderBlockPage(boardPages[pageIndex]);
    if (!payloadLen) { currentMessage = "Seite passt nicht auf die 64x16-Pixelmatrix."; return false; }
  }

  BLEAddress address{String(SIGN_ADDRESS)};
  const bool reused = bleClient && bleClient->isConnected();
  if (!reused) {
    ++diagnostics.bleAttempts;
    diagnosticEvent("BLE", "Verbindungsversuch zu CoolLEDX");
  }
  if (!reused && !bleClient->connect(address, 0xFF, 10000)) {
    currentMessage = "LED-Tafel nicht erreichbar. Ist sie eingeschaltet oder mit der Original-App verbunden?";
    diagnosticEvent("BLE", "Verbindungsversuch fehlgeschlagen", true);
    return false;
  }
  if (!reused) {
    ++diagnostics.bleConnected;
    diagnosticEvent("BLE", "Verbindung hergestellt");
    delay(650);
  }
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
  if (job == TEXT || job == PAGE) {
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
  if ((job == TEXT || job == BRIGHTNESS) && !sendSimple(c, 0x08, rawBrightness)) {
    currentMessage = "Helligkeit konnte nicht gesendet werden.";
    disconnectSign(); return false;
  }
  if (job == SPEED && !sendSimple(c, 0x07, (uint16_t(speed) * 255 + 50) / 100)) {
    currentMessage = "Tempo konnte nicht eingestellt werden.";
    disconnectSign(); return false;
  }
  if (job == MODE && !sendSimple(c, 0x06, mode)) {
    currentMessage = "Anzeigemodus konnte nicht eingestellt werden.";
    disconnectSign(); return false;
  }
  if (job == TEXT || job == PAGE) {
    const uint8_t actualMode = job == PAGE ? 0x01 : mode;
    if (!sendSimple(c, 0x06, actualMode)) {
      currentMessage = "Anzeigemodus konnte nicht gesetzt werden.";
      disconnectSign(); return false;
    }
    // Originalhardware: 0x07, Rohwert 0..255; UI-Regler 1..100. Kalibrierung folgt.
    if (job == TEXT && mode == 0x02 && !sendSimple(c, 0x07, (uint16_t(speed) * 255 + 50) / 100)) {
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
  delay((job == PAGE || job == TEXT) ? 220 : 30);
  const bool controlJob = job == BRIGHTNESS || job == SPEED || job == MODE;
  if (controlJob) controlIdleUntilMs = millis() + 2500UL;
  else { controlIdleUntilMs = 0; disconnectSign(); }
  currentMessage = job == PAGE ? String("Seite ") + (pageIndex + 1) + " angezeigt."
      : job == TEXT ? String("Text erfolgreich gesendet. ") + (mode == 0x02 ? "Lauftext." : "Standbild.")
      : job == BRIGHTNESS ? "Helligkeit ohne Bild-Upload gesendet."
      : job == SPEED ? "Laufgeschwindigkeit ohne Bild-Upload gesendet."
      : "Anzeigemodus ohne Bild-Upload gesendet.";
  if (job == PAGE) { currentPage = pageIndex; ++pageShows; }
  return true;
}

void setup() {
  Serial.begin(115200);
  delay(1200);
  Serial.println("RavestickBoard Webcontroller v0.3.4");
  diagnosticEvent("SYSTEM", String("Start v") + FW_VERSION + " Reset=" + resetDescription());
  Serial.printf("[RAM] Start: frei=%u B, groesster Block=%u B\n",
                (unsigned)ESP.getFreeHeap(), (unsigned)ESP.getMaxAllocHeap());
  BLEDevice::init("");
  BLEDevice::setMTU(247);
  bleClient = BLEDevice::createClient();
  loadPages();
  Serial.printf("[RAM] Nach BLE/NVS: frei=%u B, groesster Block=%u B\n",
                (unsigned)ESP.getFreeHeap(), (unsigned)ESP.getMaxAllocHeap());

  const bool apStarted = WiFi.mode(WIFI_AP) && WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
  if (!apStarted) {
    Serial.println("WLAN konnte nicht gestartet werden!");
    currentState = "error";
    currentMessage = "WLAN-Start fehlgeschlagen.";
    diagnosticEvent("WLAN", "Access Point Start fehlgeschlagen", true);
  } else {
    Serial.print("WLAN: "); Serial.println(WIFI_SSID);
    Serial.print("Adresse: http://"); Serial.println(WiFi.softAPIP());
    diagnosticEvent("WLAN", String("AP bereit, SSID=") + WIFI_SSID);
  }
  Serial.printf("[RAM] Nach WLAN: frei=%u B, groesster Block=%u B\n",
                (unsigned)ESP.getFreeHeap(), (unsigned)ESP.getMaxAllocHeap());

  web.on("/", HTTP_GET, []() {
    web.sendHeader("Cache-Control", "no-store, no-cache, must-revalidate");
    web.send_P(200, "text/html; charset=utf-8", PAGE_HTML);
  });
  web.on("/api/status", HTTP_GET, onStatus);
  web.on("/api/font", HTTP_GET, onFontJson);
  web.on("/api/diagnostics", HTTP_GET, onDiagnosticsJson);
  web.on("/api/diagnostics.txt", HTTP_GET, onDiagnosticsDownload);
  web.on("/api/program", HTTP_GET, onProgramGet);
  web.on("/api/program/set", HTTP_POST, onProgramSet);
  web.on("/api/program/add", HTTP_POST, onProgramAdd);
  web.on("/api/program/delete", HTTP_POST, onProgramDelete);
  web.on("/api/program/move", HTTP_POST, onProgramMove);
  web.on("/api/program/show", HTTP_POST, onProgramShow);
  web.on("/api/program/start", HTTP_POST, onProgramStart);
  web.on("/api/program/stop", HTTP_POST, onProgramStop);
  web.on("/api/text", HTTP_POST, queueText);
  web.on("/api/brightness", HTTP_POST, queueBrightness);
  web.on("/api/speed", HTTP_POST, queueSpeed);
  web.on("/api/mode", HTTP_POST, queueMode);
  web.onNotFound([]() { respond(false, "Nicht gefunden.", 404); });
  if (apStarted) {
    web.begin();
    Serial.println("Webserver bereit.");
    diagnosticEvent("HTTP", "Webserver gestartet");
  } else {
    Serial.println("Webserver nicht gestartet (kein WLAN-AP).");
  }
}

void loop() {
  web.handleClient();
  if (nextJob == NONE && controlIdleUntilMs &&
      (int32_t)(millis() - controlIdleUntilMs) >= 0) {
    disconnectSign(); controlIdleUntilMs = 0;
  }
  const uint8_t wifiClients = WiFi.softAPgetStationNum();
  if (wifiClients != knownWifiClients) {
    knownWifiClients = wifiClients;
    diagnosticEvent("WLAN", String("Verbundene Clients: ") + wifiClients);
  }
  if (autoProgram && nextJob == NONE && currentState != "sending" &&
      (int32_t)(millis() - autoNextAtMs) >= 0) {
    pendingPage = autoNextPage;
    nextJob = PAGE;
    currentState = "queued";
    currentMessage = "Automatischer Seitenwechsel wird übertragen …";
    diagnosticEvent("PAGE", String("Automatische Seite ") + (pendingPage + 1));
  }
  if (nextJob != NONE) {
    const Job job = nextJob;
    const uint8_t color = pendingColor;
    const uint8_t brightness = pendingBrightness;
    const uint8_t mode = pendingMode;
    const uint8_t speed = pendingSpeed;
    const uint8_t pageIndex = pendingPage;
    const String text = pendingText;
    nextJob = NONE;
    currentState = "sending";
    currentMessage = "Bluetooth-Übertragung läuft …";
    const uint32_t started = millis();
    diagnosticEvent("JOB", job == TEXT ? "Textübertragung startet"
      : job == PAGE ? "Seitenübertragung startet"
      : job == SPEED ? "Tempowechsel startet"
      : job == MODE ? "Moduswechsel startet" : "Helligkeit startet");
    bool ok = runJob(job, color, brightness, text, mode, speed, pageIndex);
    diagnostics.lastJobDurationMs = millis() - started;
    currentState = ok ? "ok" : "error";
    if (ok) ++diagnostics.jobsOK;
    else ++diagnostics.jobsFailed;
    diagnosticEvent("JOB", String(ok ? "OK" : "FEHLER") +
                    " nach " + diagnostics.lastJobDurationMs + " ms: " + currentMessage, !ok);
    Serial.println(currentMessage);
    if (job == PAGE && autoProgram) {
      if (!ok) { autoProgram = false; diagnosticEvent("PAGE", "Automatische Wiedergabe wegen BLE-Fehler gestoppt", true); }
      else {
        ++autoCycles;
        autoNextPage = (pageIndex + 1) % pageCount;
        autoNextAtMs = millis() + (uint32_t)boardPages[pageIndex].seconds * 1000UL;
      }
    }
  }
  delay(3);
}
