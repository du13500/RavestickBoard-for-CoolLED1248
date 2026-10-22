/*
  RavestickBoard Webcontroller v0.1.1
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

static constexpr char FW_VERSION[] = "0.1.1";
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
    case ESP_RST_POWERON: return "Power on";
    case ESP_RST_SW: return "Software reset";
    case ESP_RST_PANIC: return "Exception / panic";
    case ESP_RST_INT_WDT: return "Interrupt watchdog";
    case ESP_RST_TASK_WDT: return "Task watchdog";
    case ESP_RST_WDT: return "Other watchdog";
    case ESP_RST_BROWNOUT: return "Brownout / low voltage";
    case ESP_RST_DEEPSLEEP: return "Deep sleep";
    default: return String("Reset code ") + (int)esp_reset_reason();
  }
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
  out += "Groesster allokierbarer Block: "; out += String(ESP.getMaxAllocHeap()); out += " Bytes\n";
  out += "Heap gesamt: "; out += String(ESP.getHeapSize()); out += " Bytes\n";
  out += "Flash physisch: "; out += String(ESP.getFlashChipSize()); out += " Bytes\n";
  out += "Sketch verwendet: "; out += String(ESP.getSketchSize()); out += " Bytes\n";
  out += "Sketch Partition Rest: "; out += String(ESP.getFreeSketchSpace()); out += " Bytes\n";
  out += "Dateisystem: in v0.1.1 nicht eingebunden, kein persistenter Log\n";
  out += "\nZAeHLER SEIT BOOT\n";
  out += "BLE Versuche: "; out += String(diagnostics.bleAttempts); out += '\n';
  out += "BLE Erfolge: "; out += String(diagnostics.bleConnected); out += '\n';
  out += "BLE Trennungen: "; out += String(diagnostics.bleDisconnected); out += '\n';
  out += "BLE Writes akzeptiert: "; out += String(diagnostics.bleWriteOK); out += '\n';
  out += "BLE Write-Fehler: "; out += String(diagnostics.bleWriteFailed); out += '\n';
  out += "BLE Notifications empfangen: "; out += String(notifyCount); out += '\n';
  out += "BLE Notification-Bytes: "; out += String(notificationBytes); out += '\n';
  out += "BLE auf Notifications gewartet/erhalten: "; out += String(diagnostics.notificationsProcessed); out += '\n';
  out += "BLE Notify-Timeouts: "; out += String(diagnostics.bleTimeouts); out += '\n';
  out += "Pakete gesendet: "; out += String(diagnostics.chunksSent); out += '\n';
  out += "BLE-Daten gesendet: "; out += String(diagnostics.bytesSent); out += " Bytes\n";
  out += "HTTP Textauftraege: "; out += String(diagnostics.httpTextRequests); out += '\n';
  out += "HTTP Helligkeitsauftraege: "; out += String(diagnostics.httpBrightnessRequests); out += '\n';
  out += "HTTP Auftraege abgewiesen: "; out += String(diagnostics.httpRejectedRequests); out += '\n';
  out += "Jobs OK: "; out += String(diagnostics.jobsOK); out += '\n';
  out += "Jobs Fehler: "; out += String(diagnostics.jobsFailed); out += '\n';
  out += "Letzte Job-Dauer: "; out += String(diagnostics.lastJobDurationMs); out += " ms\n";
  out += "Fehler insgesamt: "; out += String(diagnostics.errors); out += '\n';
  out += "Letzter Fehler bei ms "; out += String(latestErrorAtMs); out += ": "; out += latestError; out += "\n";
  out += "\nHINWEISE\n";
  out += "BLE Write-Erfolg/Notification bedeutet nicht automatisch, dass die Anzeige sichtbar aktualisiert wurde.\n";
  out += "Log-Ringpuffer: nur RAM; nach Stromverlust und Neustart leer.\n";
  out += "Keine Textinhalte, Zugangsdaten oder Geraeteadressen im Export.\n";
  out += "Zeitstempel: Millisekunden seit letztem Start, keine Uhrzeit.\n";
  out += "\nLETZTE EREIGNISSE (aeltestes zuerst)\n";
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
  json += ",\"sketchFree\":"; json += String(ESP.getFreeSketchSpace());
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
  web.sendHeader("Content-Disposition", "attachment; filename=RavestickBoard_Diagnose_v0.1.1.txt");
  web.send(200, "text/plain; charset=utf-8", diagnosticReport());
}

// HTML is served directly from the ESP32 flash; no filesystem upload needed.
static const char PAGE[] PROGMEM = R"HTML(
<!doctype html><html lang="de"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>RavestickBoard · v0.1.1</title>
<style>
:root{color-scheme:dark;font-family:system-ui,-apple-system,"Segoe UI",sans-serif;background:#101218;color:#f3f5fa}
*{box-sizing:border-box}body{margin:0;padding:18px 14px calc(96px + env(safe-area-inset-bottom,0px))}main{max-width:560px;margin:0 auto}
header{display:flex;align-items:center;gap:12px;margin:8px 0 25px;min-width:0}
.logo{width:42px;height:42px;border-radius:11px;background:#232c3d;display:grid;place-items:center;font-size:25px}
h1{font-size:22px;margin:0;letter-spacing:-.5px;overflow-wrap:anywhere}header p{margin:2px 0 0;color:#9da6b7;font-size:13px}
.panel{background:#1b202b;border:1px solid #323948;border-radius:17px;padding:20px;margin-bottom:15px}
.label{font-size:12px;letter-spacing:1px;text-transform:uppercase;color:#a3aebf;font-weight:700;margin-bottom:10px}
canvas{display:block;width:100%;height:auto;max-height:160px;image-rendering:pixelated;border-radius:8px;background:#030405}
.hint{font-size:12px;color:#a3aebf;margin-top:10px;line-height:1.5}
input[type=text]{width:100%;border:1px solid #485166;border-radius:10px;background:#111620;color:white;padding:14px;font-size:19px;outline:none}
input:focus{border-color:#ffc94c}#count{font-size:12px;color:#a3aebf;text-align:right;margin:7px 0 17px}
.swatches{display:grid;grid-template-columns:repeat(7,minmax(0,1fr));gap:4px;margin:3px 0 24px;justify-items:center}
.swatches input{position:absolute;opacity:0;pointer-events:none}
.swatches label{width:clamp(28px,7vw,38px);height:clamp(28px,7vw,38px);border-radius:50%;border:3px solid #262d3b;cursor:pointer;display:block}
.swatches input:checked+label{outline:2px solid #fff;outline-offset:2px}
.rangeRow{display:flex;align-items:center;gap:14px}.rangeRow input[type=range]{width:100%;min-width:0;flex:1;accent-color:#ffcc4d}.rangeRow b{white-space:nowrap;min-width:55px;text-align:right;font-variant-numeric:tabular-nums}
button{font:inherit;cursor:pointer;border:0;border-radius:11px;padding:13px 16px;font-weight:750}
button:disabled{opacity:.55;cursor:wait}.primary{background:#ffcc4d;color:#111}.secondary{background:#333b4b;color:#fff}
.buttons{display:grid;grid-template-columns:1fr 1fr;gap:10px;margin-top:18px}
#status{margin-top:14px;border-radius:10px;background:#292e38;padding:12px 14px;font-size:13px;line-height:1.5;color:#dde4ed;min-height:44px}
.small{font-size:12px;color:#a3aebf;line-height:1.6}.version{display:inline-block;background:#343b49;border-radius:999px;padding:3px 8px;font-size:11px;font-weight:700;color:#ffdb68;white-space:nowrap;margin-left:4px}.diagBtn{display:inline-block;text-decoration:none;text-align:center}.diagGrid{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:10px}.diagCell{background:#101620;border-radius:9px;padding:10px;min-width:0}.diagCell strong{display:block;font-size:14px;overflow-wrap:anywhere}.diagCell small{display:block;color:#a3aebf;margin-bottom:3px}.diagTools{display:flex;gap:8px;flex-wrap:wrap;margin:13px 0}.diagTools button,.diagTools a{font-size:13px;padding:11px 13px}.diagEventLog{background:#101620;padding:11px;border-radius:9px;font:11px/1.6 ui-monospace,monospace;white-space:pre-wrap;overflow-wrap:anywhere;max-height:250px;overflow:auto}.diagDisclosure{cursor:pointer;font-weight:750;list-style:none}.diagDisclosure::-webkit-details-marker{display:none}.diagDisclosure:before{content:"▸";margin-right:8px}.diagnostics[open] .diagDisclosure:before{content:"▾"}
@media(max-width:380px){.buttons{grid-template-columns:1fr}}
</style></head><body><main>
<header><div class="logo">🍌</div><div><h1>RavestickBoard <span class="version">v0.1.1</span></h1><p>for CoolLED1248 · ESP32-C3</p></div></header>
<section class="panel"><div class="label">LED-Vorschau · Engschrift</div>
<canvas id="preview" width="512" height="128" aria-label="Pixel-Vorschau"></canvas>
<div class="hint">5×7 Pixel, vertikal verdoppelt · Einzeilig · Automatischer Lauftext ab 65 Pixeln</div></section>
<section class="panel"><div class="label">Anzeigetext</div>
<input id="message" type="text" maxlength="40" value="FUCK YES" placeholder="Dein Text" autocomplete="off" spellcheck="false">
<div id="count">8 / 40 Zeichen</div>
<div class="label">Textfarbe</div><div class="swatches" id="swatches"></div>
<div class="label">Helligkeit</div><div class="rangeRow"><input id="brightness" type="range" min="1" max="100" value="75"><b id="percent">75 %</b></div>
<div class="buttons"><button class="primary" id="send">Text senden</button><button class="secondary" id="apply">Nur Helligkeit</button></div>
<div role="status" aria-live="polite" id="status">Verbindung zum Controller wird geprüft …</div></section>
<details class="panel diagnostics" id="diagnostics"><summary class="diagDisclosure">Diagnose &amp; Log-Export</summary>
<p class="hint">Live-Messwerte und die letzten Ereignisse seit dem Start des ESP32. Keine Zugangsdaten oder Textinhalte im Export.</p>
<div id="diagGrid" class="diagGrid"><div class="small">Öffne diesen Bereich, um die Daten abzurufen.</div></div>
<div class="diagTools"><button class="secondary" type="button" id="diagReload">Aktualisieren</button><a class="secondary diagBtn" href="/api/diagnostics.txt" download="RavestickBoard_Diagnose_v0.1.1.txt">TXT herunterladen</a></div>
<div class="label">Letzte Ereignisse</div><div class="diagEventLog" id="diagLog">Noch nicht geladen.</div>
<div id="diagError" class="hint"></div></details>
<p class="small">Verbunden mit dem WLAN <b>CoolLED1248</b> · Diese Seite benötigt kein Internet.<br>
Der Text wird nur beim Tippen auf „Text senden“ übertragen. Die Anzeige behält den zuletzt gespeicherten Inhalt auch ohne ESP32.</p>
</main><script>
const $=id=>document.getElementById(id);
const colors=[['Rot',4,'#ff3030'],['Gelb',6,'#ffff30'],['Grün',2,'#30ff30'],['Cyan',3,'#30ffff'],['Blau',1,'#4c4cff'],['Magenta',5,'#ff30ff'],['Weiß',7,'#ffffff']];
let selected=7;
$('swatches').innerHTML=colors.map(([name,bit,hex])=>`<div><input type="radio" id="c${bit}" name="color" value="${bit}" ${bit===7?'checked':''}><label for="c${bit}" style="background:${hex}" title="${name}" aria-label="${name}"></label></div>`).join('');
$('swatches').addEventListener('change',e=>{selected=Number(e.target.value);draw();});
$('brightness').addEventListener('input',()=>{$('percent').textContent=$('brightness').value+' %';});
const FONT={
' ':[0,0,0,0,0,0,0],A:[14,17,17,31,17,17,17],B:[30,17,17,30,17,17,30],C:[15,16,16,16,16,16,15],D:[30,17,17,17,17,17,30],E:[31,16,16,30,16,16,31],F:[31,16,16,30,16,16,16],G:[15,16,16,23,17,17,15],H:[17,17,17,31,17,17,17],I:[31,4,4,4,4,4,31],J:[7,2,2,2,18,18,12],K:[17,18,20,24,20,18,17],L:[16,16,16,16,16,16,31],M:[17,27,21,21,17,17,17],N:[17,25,21,19,17,17,17],O:[14,17,17,17,17,17,14],P:[30,17,17,30,16,16,16],Q:[14,17,17,17,21,18,13],R:[30,17,17,30,20,18,17],S:[15,16,16,14,1,1,30],T:[31,4,4,4,4,4,4],U:[17,17,17,17,17,17,14],V:[17,17,17,17,17,10,4],W:[17,17,17,21,21,21,10],X:[17,17,10,4,10,17,17],Y:[17,17,10,4,4,4,4],Z:[31,1,2,4,8,16,31],
'0':[14,17,19,21,25,17,14],'1':[4,12,4,4,4,4,14],'2':[14,17,1,2,4,8,31],'3':[30,1,1,14,1,1,30],'4':[2,6,10,18,31,2,2],'5':[31,16,16,30,1,1,30],'6':[14,16,16,30,17,17,14],'7':[31,1,2,4,8,8,8],'8':[14,17,17,14,17,17,14],'9':[14,17,17,15,1,1,14],
'!':[4,4,4,4,4,0,4],'?':[14,17,1,2,4,0,4],'.':[0,0,0,0,0,12,12],',':[0,0,0,0,4,4,8],':':[0,4,4,0,4,4,0],'-':[0,0,0,31,0,0,0],'+':[0,4,4,31,4,4,0],'/':[1,1,2,4,8,16,16],"\'":[4,4,8,0,0,0,0],
'(':[2,4,8,8,8,4,2],')':[8,4,2,2,2,4,8],'%':[17,18,2,4,8,9,17],'&':[12,18,20,8,21,18,13],'#':[10,10,31,10,31,10,10],Ä:[10,0,14,17,31,17,17],Ö:[10,0,14,17,17,17,14],Ü:[10,0,17,17,17,17,14],ß:[14,17,18,20,18,17,30], '❤':[0,10,31,31,31,14,4]
};
const cv=$('preview'),ctx=cv.getContext('2d');
function glyph(c){return FONT[c]||FONT['?'];}
function draw(){const txt=[...$('message').value.replace(/[a-zäöü]/g,c=>c.toUpperCase()).replaceAll('❤️','❤')].filter(c=>c!=='\uFE0F');
$('count').textContent=txt.length+' / 40 Zeichen';
const width=Math.max(64,txt.length*6-1);const scale=Math.min(8,Math.max(2,Math.floor(512/width)));cv.width=width*scale;cv.height=16*scale;
ctx.fillStyle='#030405';ctx.fillRect(0,0,cv.width,cv.height);
const hex=colors.find(a=>a[1]===selected)[2];ctx.fillStyle=hex;
const start=Math.floor((width-(txt.length*6-1))/2);
txt.forEach((c,i)=>{const rows=glyph(c);for(let y=0;y<7;y++)for(let x=0;x<5;x++)if(rows[y]&(1<<(4-x)))ctx.fillRect((start+i*6+x)*scale,(1+y*2)*scale,scale,scale*2);});
}
$('message').addEventListener('input',draw);draw();
let busy=false;let pollTimer=null;
function lock(b){busy=b;$('send').disabled=b;$('apply').disabled=b;}
async function post(path,data){try{lock(true);$('status').textContent='Befehl wird vorbereitet …';const r=await fetch(path,{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:new URLSearchParams(data)});const j=await r.json();if(!r.ok)throw Error(j.message||'HTTP '+r.status);$('status').textContent=j.message||'Auftrag angenommen';await poll();}catch(e){$('status').textContent='Fehler: '+e.message;lock(false);}}
$('send').addEventListener('click',()=>{if(!$('message').value.trim()){$('status').textContent='Bitte einen Text eingeben.';return;}post('/api/text',{text:$('message').value,color:selected,brightness:$('brightness').value});});
$('apply').addEventListener('click',()=>post('/api/brightness',{brightness:$('brightness').value}));
async function poll(){try{const r=await fetch('/api/status',{cache:'no-store'});const j=await r.json();$('status').textContent=j.message;lock(j.state==='queued'||j.state==='sending');}catch(e){$('status').textContent='Controller derzeit nicht erreichbar.';lock(false);}}
async function refreshDiagnostics(){
  const box=$('diagGrid'),log=$('diagLog');
  try{
    box.textContent='Messwerte werden geladen …';
    const r=await fetch('/api/diagnostics',{cache:'no-store'});
    if(!r.ok)throw Error('HTTP '+r.status);
    const d=await r.json();
    const rows=[['Firmware','v'+d.version],['Laufzeit',d.uptime],['Resetgrund',d.reset],
      ['Freier Heap',(d.freeHeap/1024).toFixed(1)+' KiB'],
      ['Min. freier Heap',(d.minHeap/1024).toFixed(1)+' KiB'],
      ['Größter Block',(d.maxBlock/1024).toFixed(1)+' KiB'],
      ['Sketchgröße',(d.sketchSize/1024).toFixed(0)+' KiB'],
      ['Freier App-Platz',(d.sketchFree/1024).toFixed(0)+' KiB'],
      ['WLAN-Geräte',d.wifiClients],['Bluetooth',d.bleConnected?'Verbunden':'Getrennt'],
      ['BLE-Versuche',d.bleAttempts],['BLE-Verbindungen',d.bleSuccess],
      ['BLE-Timeouts',d.bleTimeouts],['Schreibfehler',d.writeFailures],
      ['Benachrichtigungen',d.notifications],['Aufträge OK / Fehler',d.jobsOK+' / '+d.jobsFailed],
      ['Fehler gesamt',d.errors]];
    box.replaceChildren();
    for(const [label,value] of rows){const cell=document.createElement('div');cell.className='diagCell';
      const small=document.createElement('small');small.textContent=label;
      const strong=document.createElement('strong');strong.textContent=String(value);
      cell.append(small,strong);box.append(cell);}
    log.textContent=d.recent||'Noch keine Ereignisse.';
    $('diagError').textContent='Letzter Fehler: '+d.lastError;
  }catch(e){box.textContent='Diagnose gerade nicht verfügbar.';log.textContent='Fehler: '+e.message;}
}
$('diagnostics').addEventListener('toggle',()=>{if($('diagnostics').open)refreshDiagnostics();});
$('diagReload').addEventListener('click',refreshDiagnostics);
setInterval(poll,1200);poll();
</script></body></html>
)HTML";

// Decode UTF-8 into a fixed 40-codepoint buffer, normalizing lowercase.
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
    if (cp >= 'a' && cp <= 'z') cp -= 32;
    if (cp == 0x00E4 || cp == 0x00F6 || cp == 0x00FC) cp -= 32; // äöü
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
enum Job : uint8_t { NONE, TEXT, BRIGHTNESS };
Job nextJob = NONE;
String pendingText;
uint8_t pendingColor = 7;
uint8_t pendingBrightness = 75;

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
    diagnosticEvent("HTTP", "Auftrag abgewiesen: Controller beschaeftigt");
    respond(false, "Der Controller ist beschaeftigt.", 409);
    return false;
  }
  return true;
}

void queueText() {
  if (!reserveJob()) return;
  const String text = web.arg("text");
  const int color = web.arg("color").toInt();
  const int brightness = web.arg("brightness").toInt();
  const int count = decodeText(text, characterCodes);
  String trimmed = text;
  trimmed.trim();
  if (count <= 0 || count > MAX_CHARS || trimmed.isEmpty()) {
    ++diagnostics.httpRejectedRequests;
    diagnosticEvent("HTTP", "Ungueltige Textlaenge oder leer", true);
    respond(false, "Bitte 1 bis 40 Zeichen eingeben.", 400); return;
  }
  if (color < 1 || color > 7 || brightness < 1 || brightness > 100) {
    ++diagnostics.httpRejectedRequests;
    diagnosticEvent("HTTP", "Ungueltige Farbe oder Helligkeit", true);
    respond(false, "Ungueltige Farbe oder Helligkeit.", 400); return;
  }
  ++diagnostics.httpTextRequests;
  diagnosticEvent("HTTP", String("Textauftrag angenommen, Zeichen=") + count);
  pendingText = text;
  pendingColor = color;
  pendingBrightness = brightness;
  nextJob = TEXT;
  currentState = "queued";
  currentMessage = "Text angenommen. Bluetooth-Uebertragung startet …";
  respond(true, currentMessage, 202);
}

void queueBrightness() {
  if (!reserveJob()) return;
  const int brightness = web.arg("brightness").toInt();
  if (brightness < 1 || brightness > 100) {
    ++diagnostics.httpRejectedRequests;
    diagnosticEvent("HTTP", "Helligkeit ausserhalb 1..100", true);
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

bool runJob(Job job, uint8_t color, uint8_t brightness, const String &message) {
  Serial.printf("Auftrag: %s\n", job == TEXT ? "TEXT" : "HELLIGKEIT");
  uint16_t width = SIGN_WIDTH;
  size_t payloadLen = 0;
  uint8_t raw[140];
  uint8_t frame[300];
  const uint16_t MAX_FRAME = sizeof(frame);
  if (job == TEXT) {
    const int count = decodeText(message, characterCodes);
    if (count <= 0) { currentMessage = "Ungueltiger Text."; return false; }
    payloadLen = renderText(characterCodes, count, color, width);
    if (!payloadLen) { currentMessage = "Text passt nicht in den Pufferspeicher."; return false; }
  }

  BLEAddress address{String(SIGN_ADDRESS)};
  ++diagnostics.bleAttempts;
  diagnosticEvent("BLE", "Verbindungsversuch zu CoolLEDX");
  if (!bleClient->connect(address)) {
    currentMessage = "Bluetooth-Verbindung fehlgeschlagen. Ist CoolLEDX eingeschaltet?";
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
  diagnosticEvent("BLE", String("MTU=") + mtu + " Nutzdaten=" + (unsigned)payloadLen + " Chunks=" + (unsigned)chunks);
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
    const uint8_t mode = width > SIGN_WIDTH ? 0x02 : 0x01;
    if (!sendSimple(c, 0x06, mode)) {
      currentMessage = "Anzeigemodus konnte nicht gesetzt werden.";
      disconnectSign(); return false;
    }
    for (size_t i = 0; i < chunks; ++i) {
      const size_t rawN = makeChunk(i, payloadLen, raw);
      const size_t frameN = framePacket(raw, rawN, frame);
      Serial.printf("Textpaket %u/%u (%u Bytes)\n", (unsigned)(i+1),
                    (unsigned)chunks, (unsigned)frameN);
      if (!sendPacket(c, frame, frameN, true)) {
        currentMessage = "Textuebertragung abgebrochen: Paket " + String((unsigned)(i+1)) +
                         " von " + String((unsigned)chunks) + " nicht bestaetigt.";
        disconnectSign(); return false;
      }
    }
  }
  delay(220);
  disconnectSign();
  currentMessage = job == TEXT
      ? String("Text erfolgreich gesendet. ") + (width > SIGN_WIDTH ? "Lauftext." : "Statische Anzeige.")
      : "Helligkeitsbefehl gesendet. Anzeigeinhalt unveraendert.";
  return true;
}

void setup() {
  Serial.begin(115200);
  delay(1200);
  Serial.println("RavestickBoard Webcontroller v0.1.1");
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
    const String text = pendingText;
    nextJob = NONE;
    currentState = "sending";
    currentMessage = "Bluetooth-Uebertragung laeuft …";
    const uint32_t started = millis();
    diagnosticEvent("JOB", job == TEXT ? "Textuebertragung startet" : "Helligkeit startet");
    bool ok = runJob(job, color, brightness, text);
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
