/*
  RavestickBoard Webcontroller v0.4.2.5-dev
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
#include <esp_timer.h>
#include <esp_ota_ops.h>
#include <Preferences.h>
#include <LittleFS.h>
#include "RavestickBoard_WebUI.h"
#include "RavestickBoard_Assets.h"

// Named types are placed BEFORE their functions so Arduino's generated prototypes
// don't see an unknown type (a regression we fixed after the v0.1.2 test).
enum Job : uint8_t { NONE, TEXT, BRIGHTNESS, PAGE, SPEED, MODE, POWER, AMBIENCE, BAND };
static constexpr uint8_t PAGE_LIMIT = 25;
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
extern bool libraryReady;

static constexpr char FW_VERSION[] = "0.4.2.5-dev";
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

// v0.4.0.3: The CoolLEDX payload is 107 header bytes followed by three
// full-width 2-byte/column bitplanes. For long texts, generate small raster
// tiles on demand instead of holding a 30-KiB image during BLE transfer.
// Short transfers and all block pages deliberately retain the proven renderer.
static constexpr size_t STREAM_THRESHOLD_BYTES = 8192;
static constexpr uint16_t STREAM_TILE_COLUMNS = 64;
static uint8_t streamHeader[IMAGE_HEADER_BYTES] = {};
static uint8_t streamTile[STREAM_TILE_COLUMNS * BYTES_PER_COLUMN] = {};
static bool streamPayload = false, streamDrawingTile = false;
static bool rb42StreamBand = false;
static bool rb43StreamBand = false;
void rb43CopyMixedSlice(size_t offset, size_t length, uint8_t *dest);
void rb42CopyBandSlice(size_t offset, size_t length, uint8_t *dest);
static uint16_t streamImageWidth = 0, streamTileStart = 0, streamTileCols = 0;
static uint16_t streamCachedTile = 0xFFFF;
static const String *streamFirstLine = nullptr, *streamSecondLine = nullptr;
static uint8_t streamLayout = 0, streamFirstColor = 7, streamSecondColor = 7;
static uint32_t streamFirstPixels = 0, streamSecondPixels = 0;
static uint32_t streamTileRenders = 0;


// v0.4.0.2: bounded RAM timing trace, no heap allocations or NVS writes.
// A recorded minimum is an observed checkpoint, not a full heap profiler.
enum RamStage : uint8_t {
  RT_BOOT, RT_WEB_BEGIN, RT_WEB_END, RT_STATUS_LOW,
  RT_JOB_BEGIN, RT_RENDER_BEGIN, RT_BUFFER_ALLOC, RT_RENDER_END,
  RT_BLE_BEGIN, RT_BLE_LINK, RT_BLE_READY,
  RT_TX_BEGIN, RT_TX_PROGRESS, RT_TX_END,
  RT_RELEASE_BEGIN, RT_RELEASE_END, RT_JOB_END
};
struct RamSample {
  uint32_t atMs;
  uint32_t freeBytes;
  uint32_t minBytes;
  uint32_t maxBlock;
  uint16_t jobSeq;    // wraps after 65535 jobs, acceptable for one session
  uint16_t amount;    // payload bytes (or packet count for TX_PROGRESS)
  uint8_t stage;
  uint8_t wifiClients;
  uint8_t jobType;
};
static constexpr uint8_t RAM_TRACE_CAP = 64;
RamSample ramTrace[RAM_TRACE_CAP] = {};
uint8_t ramTraceNext = 0, ramTraceCount = 0;
RamSample ramLowestFree = {}, ramLowestMin = {}, ramLowestBlock = {};
bool ramHasSample = false;
uint32_t ramJobSeq = 0, ramActiveJob = 0;
uint8_t ramActiveJobType = 0;
RamSample ramJobLowestFree = {}, ramJobLowestBlock = {};
RamSample ramLastJobLowestFree = {}, ramLastJobLowestBlock = {};
uint32_t ramJobBeginMin = 0, ramLastJobBeginMin = 0, ramLastJobEndMin = 0;
uint16_t ramLastJobId = 0;
bool ramJobHasSample = false, ramLastJobHasSample = false;

const char *ramStageLabel(RamStage s) {
  switch (s) {
    case RT_BOOT: return "BOOT";
    case RT_WEB_BEGIN: return "WEB_BEGIN";
    case RT_WEB_END: return "WEB_END";
    case RT_STATUS_LOW: return "STATUS_LOW";
    case RT_JOB_BEGIN: return "JOB_BEGIN";
    case RT_RENDER_BEGIN: return "RENDER_BEGIN";
    case RT_BUFFER_ALLOC: return "BUFFER_ALLOC";
    case RT_RENDER_END: return "RENDER_END";
    case RT_BLE_BEGIN: return "BLE_BEGIN";
    case RT_BLE_LINK: return "BLE_LINK";
    case RT_BLE_READY: return "BLE_READY";
    case RT_TX_BEGIN: return "TX_BEGIN";
    case RT_TX_PROGRESS: return "TX_PROGRESS";
    case RT_TX_END: return "TX_END";
    case RT_RELEASE_BEGIN: return "RELEASE_BEGIN";
    case RT_RELEASE_END: return "RELEASE_END";
    case RT_JOB_END: return "JOB_END";
    default: return "UNKNOWN";
  }
}
const char *ramJobLabel(uint8_t job) {
  switch (job) {
    case TEXT: return "TEXT";
    case PAGE: return "PAGE";
    case BAND: return "BAND";
    case AMBIENCE: return "AMBIENCE";
    case BRIGHTNESS: return "BRIGHTNESS";
    case SPEED: return "SPEED";
    case MODE: return "MODE";
    case POWER: return "POWER";
    default: return "NONE";
  }
}
// force=true stores the phase in the ring; false is used every BLE packet.
// A falling observed free-heap or system low-water mark is always preserved.
void ramTraceMark(RamStage stage, uint16_t amount = 0, bool force = true) {
  RamSample s{};
  s.atMs = millis();
  s.jobSeq = (uint16_t)ramActiveJob;
  s.freeBytes = ESP.getFreeHeap();
  s.minBytes = ESP.getMinFreeHeap();
  s.maxBlock = ESP.getMaxAllocHeap();
  s.amount = amount;
  s.stage = (uint8_t)stage;
  s.wifiClients = WiFi.softAPgetStationNum();
  s.jobType = ramActiveJobType;
  const bool newMin = !ramHasSample || s.freeBytes < ramLowestFree.freeBytes ||
      s.minBytes < ramLowestMin.minBytes || s.maxBlock < ramLowestBlock.maxBlock;
  if (!ramHasSample || s.freeBytes < ramLowestFree.freeBytes) ramLowestFree = s;
  if (!ramHasSample || s.minBytes < ramLowestMin.minBytes) ramLowestMin = s;
  if (!ramHasSample || s.maxBlock < ramLowestBlock.maxBlock) ramLowestBlock = s;
  ramHasSample = true;
  // A separate per-job checkpoint minimum survives overwriting the 64-sample ring.
  // System-Min remains historical; comparison with JOB_BEGIN is explicit.
  if (stage == RT_JOB_BEGIN) {
    ramJobHasSample = true;
    ramJobLowestFree = ramJobLowestBlock = s;
    ramJobBeginMin = s.minBytes;
  } else if (ramActiveJob && ramJobHasSample) {
    if (s.freeBytes < ramJobLowestFree.freeBytes) ramJobLowestFree = s;
    if (s.maxBlock < ramJobLowestBlock.maxBlock) ramJobLowestBlock = s;
    if (stage == RT_JOB_END) {
      ramLastJobHasSample = true;
      ramLastJobId = s.jobSeq;
      ramLastJobLowestFree = ramJobLowestFree;
      ramLastJobLowestBlock = ramJobLowestBlock;
      ramLastJobBeginMin = ramJobBeginMin;
      ramLastJobEndMin = s.minBytes;
    }
  }
  if (force || newMin) {
    ramTrace[ramTraceNext] = s;
    ramTraceNext = (ramTraceNext + 1) % RAM_TRACE_CAP;
    if (ramTraceCount < RAM_TRACE_CAP) ++ramTraceCount;
  }
  if (force || (newMin && stage != RT_TX_PROGRESS)) {
    Serial.printf("[RAMTRACE] %lu ms | Job %lu (%s) | %s | frei=%lu min=%lu block=%lu WLAN=%u daten=%u\n",
      (unsigned long)s.atMs, (unsigned long)s.jobSeq, ramJobLabel(ramActiveJobType),
      ramStageLabel(stage), (unsigned long)s.freeBytes, (unsigned long)s.minBytes,
      (unsigned long)s.maxBlock, s.wifiClients, s.amount);
  }
}

WebServer web(80);

// v0.4.2.5 HTTP profiler retained from A/B; compile-time controlled instrumentation.
// Records URL paths only (no query strings, POST bodies, credentials or device MACs).
// A periodic esp_timer callback samples heap during synchronous HTTP sends, even
// when web.handleClient() is blocked. The timer has no own FreeRTOS task/stack.
// A 20 ms sample can still miss shorter peaks; the system min is historical.
static constexpr uint8_t HTTP_TRACE_CAP = 32;
struct HttpTraceRecord {
  uint32_t beginMs, durationMs;
  uint32_t freeBefore, freeAfter, sampledLowFree;
  uint32_t blockBefore, blockAfter, sampledLowBlock;
  uint32_t systemMinBefore, systemMinAfter;
  uint16_t status;
  uint8_t clients;
  char method;
  char path[52];
};
static HttpTraceRecord httpTrace[HTTP_TRACE_CAP] = {};
static uint8_t httpTraceNext = 0, httpTraceCount = 0;
static HttpTraceRecord httpInFlight = {};
static bool httpActive = false;
static portMUX_TYPE httpTraceMux = portMUX_INITIALIZER_UNLOCKED;
static esp_timer_handle_t httpTimer = nullptr;
static uint32_t httpIdleLowFree = UINT32_MAX, httpIdleLowBlock = UINT32_MAX;
static uint32_t httpIdleLowAtMs = 0;
static uint8_t httpIdleLowClients = 0;
static volatile uint8_t httpClientsCached = 0;
static uint32_t httpSamples = 0;
// Aggregates survive the fixed-size ring rolling over while 20 reloads are tested.
// Buckets: HTML /, unknown 404, /api/*, icon, other.
struct HttpBucket {
  uint32_t count, responses200, responses304, responses404;
  uint32_t lowestSampledFree, lowestSampledBlock, longestMs, systemMinDrops;
};
static HttpBucket httpBuckets[5] = {};
static const char * const httpBucketNames[5] = {"/", "404", "/api/*", "icon", "other"};

static void httpSampleTimer(void *) {
  const uint32_t freeBytes = ESP.getFreeHeap();
  const uint32_t block = ESP.getMaxAllocHeap();
  portENTER_CRITICAL(&httpTraceMux);
  if (httpActive) {
    if (freeBytes < httpInFlight.sampledLowFree) httpInFlight.sampledLowFree = freeBytes;
    if (block < httpInFlight.sampledLowBlock) httpInFlight.sampledLowBlock = block;
  } else {
    if (freeBytes < httpIdleLowFree) {
      httpIdleLowFree = freeBytes;
      httpIdleLowAtMs = millis();
      httpIdleLowClients = httpClientsCached;
    }
    if (block < httpIdleLowBlock) httpIdleLowBlock = block;
  }
  ++httpSamples;
  portEXIT_CRITICAL(&httpTraceMux);
}

static void httpBegin() {
  HttpTraceRecord r{};
  r.beginMs = millis();
  r.freeBefore = ESP.getFreeHeap();
  r.blockBefore = ESP.getMaxAllocHeap();
  r.sampledLowFree = r.freeBefore;
  r.sampledLowBlock = r.blockBefore;
  r.systemMinBefore = ESP.getMinFreeHeap();
  r.clients = WiFi.softAPgetStationNum();
  r.method = web.method() == HTTP_GET ? 'G' : web.method() == HTTP_POST ? 'P' : '?';
  r.status = 200;
  const String uri = web.uri();
  // Only ASCII path characters, no query params. A non-path character is redacted.
  uint8_t p = 0;
  for (size_t i=0; i<uri.length() && p<sizeof(r.path)-1; ++i) {
    const char c = uri[i];
    if (c=='?') break;
    if ((c>='a' && c<='z') || (c>='A' && c<='Z') ||
        (c>='0' && c<='9') || c=='/' || c=='_' || c=='-' || c=='.') r.path[p++] = c;
    else { r.path[p++] = '_'; }
  }
  if (!p) { r.path[0] = '/'; r.path[1] = '\0'; }
  portENTER_CRITICAL(&httpTraceMux);
  httpInFlight = r;
  httpActive = true;
  portEXIT_CRITICAL(&httpTraceMux);
}

static void httpSetStatus(uint16_t code) {
  portENTER_CRITICAL(&httpTraceMux);
  if (httpActive) httpInFlight.status = code;
  portEXIT_CRITICAL(&httpTraceMux);
}

static void httpEnd() {
  const uint32_t freeBytes = ESP.getFreeHeap();
  const uint32_t block = ESP.getMaxAllocHeap();
  const uint32_t systemMin = ESP.getMinFreeHeap();
  const uint32_t finishedMs = millis();
  HttpTraceRecord done;
  portENTER_CRITICAL(&httpTraceMux);
  if (freeBytes < httpInFlight.sampledLowFree) httpInFlight.sampledLowFree = freeBytes;
  if (block < httpInFlight.sampledLowBlock) httpInFlight.sampledLowBlock = block;
  httpInFlight.freeAfter = freeBytes;
  httpInFlight.blockAfter = block;
  httpInFlight.systemMinAfter = systemMin;
  httpInFlight.durationMs = finishedMs - httpInFlight.beginMs;
  done = httpInFlight;
  httpActive = false;
  httpTrace[httpTraceNext] = done;
  httpTraceNext = (httpTraceNext+1) % HTTP_TRACE_CAP;
  if (httpTraceCount < HTTP_TRACE_CAP) ++httpTraceCount;
  uint8_t bucket = 4;
  if (done.status == 404) bucket = 1;
  else if (strcmp(done.path, "/") == 0) bucket = 0;
  else if (strncmp(done.path, "/api/", 5) == 0) bucket = 2;
  else if (strstr(done.path, "icon") != nullptr) bucket = 3;
  HttpBucket &agg = httpBuckets[bucket];
  if (agg.count == 0 || done.sampledLowFree < agg.lowestSampledFree) agg.lowestSampledFree = done.sampledLowFree;
  if (agg.count == 0 || done.sampledLowBlock < agg.lowestSampledBlock) agg.lowestSampledBlock = done.sampledLowBlock;
  if (done.durationMs > agg.longestMs) agg.longestMs = done.durationMs;
  if (done.status == 200) ++agg.responses200;
  if (done.status == 304) ++agg.responses304;
  if (done.status == 404) ++agg.responses404;
  if (done.systemMinAfter < done.systemMinBefore) ++agg.systemMinDrops;
  ++agg.count;
  portEXIT_CRITICAL(&httpTraceMux);
  Serial.printf("[HTTPTRACE] %c %s %u %lu ms WLAN=%u heap=%lu/%lu/%lu block=%lu/%lu/%lu sysMin=%lu->%lu\n",
      done.method, done.path, done.status, (unsigned long)done.durationMs, done.clients,
      (unsigned long)done.freeBefore, (unsigned long)done.sampledLowFree,
      (unsigned long)done.freeAfter, (unsigned long)done.blockBefore,
      (unsigned long)done.sampledLowBlock, (unsigned long)done.blockAfter,
      (unsigned long)done.systemMinBefore, (unsigned long)done.systemMinAfter);
}

static void httpRoute(const char *path, HTTPMethod method, void (*handler)()) {
  web.on(path, method, [handler]() { httpBegin(); handler(); httpEnd(); });
}

static void onHttpTraceDownload() {
  // Also profiled itself; snapshot existing entries before streaming.
  web.sendHeader("Cache-Control", "no-store");
  web.sendHeader("Content-Disposition", String("attachment; filename=RavestickBoard_HTTP_v") + FW_VERSION + ".txt");
  web.setContentLength(CONTENT_LENGTH_UNKNOWN);
  web.send(200, "text/plain; charset=utf-8", "");
  char line[240];
  web.sendContent(String("HTTP/RAMTRACE ") + FW_VERSION + " | 20ms-Sampler | Zeit ms seit Boot\n");
  web.sendContent("Status 200/304/404/Fehler; Queryparameter und Request-Bodies werden nie protokolliert.\n");
  web.sendContent("Beobachtete Minima sind Stichproben, System-Minimum ist seit Boot kumulativ.\n");
  uint32_t idleFree, idleBlock, idleAt, sampleCount;
  uint8_t idleClients, count, next;
  portENTER_CRITICAL(&httpTraceMux);
  idleFree = httpIdleLowFree; idleBlock = httpIdleLowBlock;
  idleAt = httpIdleLowAtMs; idleClients = httpIdleLowClients;
  sampleCount = httpSamples; count = httpTraceCount; next = httpTraceNext;
  portEXIT_CRITICAL(&httpTraceMux);
  snprintf(line, sizeof(line), "WLAN jetzt=%u | Samples=%lu | Idle-Low-Heap=%lu bei ms %lu (WLAN %u) | Idle-Low-Block=%lu | System-Min=%lu\n",
      WiFi.softAPgetStationNum(), (unsigned long)sampleCount,
      (unsigned long)(idleFree==UINT32_MAX?0:idleFree), (unsigned long)idleAt,
      idleClients, (unsigned long)(idleBlock==UINT32_MAX?0:idleBlock),
      (unsigned long)ESP.getMinFreeHeap());
  web.sendContent(String(line));
  web.sendContent("Bucket | gesamt | 200 | 304 | 404 | Heap-Min-Sample | Block-Min-Sample | MaxDauer ms | System-Min-Drops\n");
  for (uint8_t i=0; i<5; ++i) {
    HttpBucket agg;
    portENTER_CRITICAL(&httpTraceMux);
    agg = httpBuckets[i];
    portEXIT_CRITICAL(&httpTraceMux);
    snprintf(line, sizeof(line), "%s | %lu | %lu | %lu | %lu | %lu | %lu | %lu | %lu\n",
      httpBucketNames[i], (unsigned long)agg.count, (unsigned long)agg.responses200,
      (unsigned long)agg.responses304, (unsigned long)agg.responses404,
      (unsigned long)agg.lowestSampledFree, (unsigned long)agg.lowestSampledBlock,
      (unsigned long)agg.longestMs, (unsigned long)agg.systemMinDrops);
    web.sendContent(String(line));
  }
  web.sendContent("\nLetzte 32 Einzelanfragen (Ringpuffer):\n");
  web.sendContent("Start ms | Methode | Pfad | Status | Dauer ms | WLAN | Heap vor/min/nach | Block vor/min/nach | SystemMin vor/nach\n");
  const uint8_t start = (next + HTTP_TRACE_CAP - count) % HTTP_TRACE_CAP;
  for (uint8_t i=0; i<count; ++i) {
    HttpTraceRecord s;
    portENTER_CRITICAL(&httpTraceMux);
    s = httpTrace[(start+i) % HTTP_TRACE_CAP];
    portEXIT_CRITICAL(&httpTraceMux);
    snprintf(line, sizeof(line), "%lu | %c | %s | %u | %lu | %u | %lu/%lu/%lu | %lu/%lu/%lu | %lu/%lu\n",
        (unsigned long)s.beginMs, s.method, s.path, s.status, (unsigned long)s.durationMs,
        s.clients, (unsigned long)s.freeBefore, (unsigned long)s.sampledLowFree,
        (unsigned long)s.freeAfter, (unsigned long)s.blockBefore,
        (unsigned long)s.sampledLowBlock, (unsigned long)s.blockAfter,
        (unsigned long)s.systemMinBefore, (unsigned long)s.systemMinAfter);
    web.sendContent(String(line));
  }
  web.sendContent("");
}

static void httpStartSampler() {
  esp_timer_create_args_t timerArgs{};
  timerArgs.callback = &httpSampleTimer;
  timerArgs.dispatch_method = ESP_TIMER_TASK;
  timerArgs.name = "rb_http_ram";
  esp_err_t rc = esp_timer_create(&timerArgs, &httpTimer);
  if (rc == ESP_OK) rc = esp_timer_start_periodic(httpTimer, 20000); // 20 ms
  if (rc != ESP_OK) Serial.printf("[HTTPTRACE] Sampler nicht gestartet: %d\n", (int)rc);
}

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
  ramTraceMark(RT_RENDER_BEGIN, (uint16_t)needed);
  textPayload = static_cast<uint8_t *>(malloc(needed));
  if (!textPayload) {
    Serial.printf("[RAM] Bildpuffer %u B fehlgeschlagen; frei=%u B, groesster Block=%u B\n",
                  (unsigned)needed, (unsigned)ESP.getFreeHeap(), (unsigned)ESP.getMaxAllocHeap());
    return false;
  }
  pixelData = textPayload + 107;
  memset(textPayload, 0, needed);
  ramTraceMark(RT_BUFFER_ALLOC, (uint16_t)needed);
  Serial.printf("[RAM] Bildpuffer %u B reserviert; frei=%u B\n",
                (unsigned)needed, (unsigned)ESP.getFreeHeap());
  return true;
}

struct RenderBufferCleanup {
  ~RenderBufferCleanup() {
    if (textPayload) ramTraceMark(RT_RELEASE_BEGIN);
    free(textPayload);
    textPayload = nullptr;
    pixelData = nullptr;
    streamPayload = false;
    rb42StreamBand = false;
    rb43StreamBand = false;
    streamDrawingTile = false;
    streamFirstLine = streamSecondLine = nullptr;
    streamCachedTile = 0xFFFF;
    ramTraceMark(RT_RELEASE_END);
  }
};

// v0.4.2.5-dev: Lauftext is decoded as a stream. No 600-character buffer or hard
// character limit is required; only the rendered data budget is enforced.

// The block editor lives only in RAM; library entries persist in LittleFS.
// NVS Preferences deliberately unused for the volatile block editor.
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
  if (ramHasSample) {
    out += "RAMTRACE niedrigste beobachtete Momentaufnahme: ";
    out += String(ramLowestFree.freeBytes); out += " Bytes bei ms ";
    out += String(ramLowestFree.atMs); out += " / ";
    out += ramStageLabel((RamStage)ramLowestFree.stage); out += " / Job ";
    out += String(ramLowestFree.jobSeq); out += "\n";
    out += "RAMTRACE niedrigstes System-Minimum beobachtet: ";
    out += String(ramLowestMin.minBytes); out += " Bytes bei ms ";
    out += String(ramLowestMin.atMs); out += " / ";
    out += ramStageLabel((RamStage)ramLowestMin.stage); out += "\n";
    out += "RAMTRACE kleinster beobachteter zusammenhaengender Block: ";
    out += String(ramLowestBlock.maxBlock); out += " Bytes bei ms ";
    out += String(ramLowestBlock.atMs); out += "\n";
    out += "RAMTRACE: /api/ram.txt fuer Messreihe, Zeitpunkte und Jobzuordnung\n";
    if (ramLastJobHasSample) {
      out += "RAMTRACE letzter Job "; out += String(ramLastJobId);
      out += ": kleinster beobachteter Heap="; out += String(ramLastJobLowestFree.freeBytes);
      out += " B ("; out += ramStageLabel((RamStage)ramLastJobLowestFree.stage);
      out += "), kleinster Block="; out += String(ramLastJobLowestBlock.maxBlock);
      out += " B ("; out += ramStageLabel((RamStage)ramLastJobLowestBlock.stage);
      out += "), System-Min vorher/nachher="; out += String(ramLastJobBeginMin);
      out += "/"; out += String(ramLastJobEndMin); out += " B\n";
    }
  }

  out += "Flash physisch: "; out += String(ESP.getFlashChipSize()); out += " Bytes\n";
  out += "Sketch verwendet: "; out += String(ESP.getSketchSize()); out += " Bytes\n";
  out += "App-Partition gesamt: "; out += String(appPartitionSize()); out += " Bytes\n";
  out += "App-Partition rechnerisch frei: "; out += String(appAvailable()); out += " Bytes\n";
  out += "LittleFS-Bibliothek: "; out += libraryReady ? "bereit" : "nicht bereit"; out += "\n";
  out += "Dateisystem: LittleFS-Bibliothek; Blockeditor-Entwurf im RAM; Diagnose-Log im RAM\n";
  out += "Seiten gespeichert: "; out += String(pageCount); out += "\n";
  out += "Seitenprogramm läuft: "; out += autoProgram ? "ja\n" : "nein\n";
  out += "Aktuelle Seite: "; out += String(currentPage + 1); out += "\n";
  out += "\nZÄHLER SEIT BOOT\n";
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


void onRamTraceDownload() {
  ramTraceMark(RT_WEB_BEGIN);
  web.sendHeader("Cache-Control", "no-store");
  web.sendHeader("Content-Disposition", String("attachment; filename=RavestickBoard_RAM_v") + FW_VERSION + ".txt");
  web.setContentLength(CONTENT_LENGTH_UNKNOWN);
  web.send(200, "text/plain; charset=utf-8", "");
  char line[200];
  snprintf(line, sizeof(line), "RavestickBoard RAMTRACE v%s | ms seit Boot\n", FW_VERSION);
  web.sendContent(String(line));
  web.sendContent("Momentaufnahmen in RAM; nach Neustart verloren. Keine Texte, Passwoerter oder BLE-Adressen.\n");
  web.sendContent("Speicher-Minimum (System) kann zwischen zwei Messpunkten entstanden sein.\n");
  web.sendContent("TX_PROGRESS daten = zuletzt bearbeitete Paketnummer; sonst Bild-Payload (Bytes).\n");
  snprintf(line, sizeof(line), "Uptime=%lu ms | Ring=%u/%u | WLAN=%u | aktuell frei=%lu min=%lu block=%lu\n",
      (unsigned long)millis(), ramTraceCount, RAM_TRACE_CAP, (unsigned)WiFi.softAPgetStationNum(),
      (unsigned long)ESP.getFreeHeap(), (unsigned long)ESP.getMinFreeHeap(), (unsigned long)ESP.getMaxAllocHeap());
  web.sendContent(String(line));
  if (ramHasSample) {
    const RamSample *rs[3] = {&ramLowestFree,&ramLowestMin,&ramLowestBlock};
    const char *nm[3] = {"Niedrigster gemessener freier Heap", "Niedrigster beobachteter System-Min-Heap", "Kleinster gemessener Maxblock"};
    for (int i=0; i<3; ++i) {
      const RamSample &s=*rs[i];
      snprintf(line,sizeof(line),"%s: %lu ms | Job %lu | %s | frei=%lu | System-Min=%lu | Block=%lu | WLAN=%u\n",
        nm[i],(unsigned long)s.atMs,(unsigned long)s.jobSeq,ramStageLabel((RamStage)s.stage),
        (unsigned long)s.freeBytes,(unsigned long)s.minBytes,(unsigned long)s.maxBlock,s.wifiClients);
      web.sendContent(String(line));
    }
  }
  if (ramLastJobHasSample) {
    snprintf(line, sizeof(line), "Letzter Job %u: Heap-Min-Messpunkt=%lu B (%s), Block-Min=%lu B (%s)\n",
      ramLastJobId, (unsigned long)ramLastJobLowestFree.freeBytes,
      ramStageLabel((RamStage)ramLastJobLowestFree.stage),
      (unsigned long)ramLastJobLowestBlock.maxBlock,
      ramStageLabel((RamStage)ramLastJobLowestBlock.stage));
    web.sendContent(String(line));
    snprintf(line, sizeof(line), "System-Min vor/nach letztem Job: %lu / %lu B (historisch)\n",
      (unsigned long)ramLastJobBeginMin, (unsigned long)ramLastJobEndMin);
    web.sendContent(String(line));
  }
  web.sendContent("\nms | job | jobTyp | phase | daten | frei B | System-Min B | maxBlock B | WLAN-Clients\n");
  const uint8_t start = (ramTraceNext + RAM_TRACE_CAP - ramTraceCount) % RAM_TRACE_CAP;
  for (uint8_t i=0; i<ramTraceCount; ++i) {
    const RamSample &s = ramTrace[(start + i) % RAM_TRACE_CAP];
    snprintf(line,sizeof(line),"%lu | %lu | %s | %s | %u | %lu | %lu | %lu | %u\n",
      (unsigned long)s.atMs,(unsigned long)s.jobSeq,ramJobLabel(s.jobType),ramStageLabel((RamStage)s.stage),s.amount,
      (unsigned long)s.freeBytes,(unsigned long)s.minBytes,(unsigned long)s.maxBlock,s.wifiClients);
    web.sendContent(String(line));
  }
  web.sendContent("");
  ramTraceMark(RT_WEB_END);
}

void onDiagnosticsJson() {
  ramTraceMark(RT_WEB_BEGIN);
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
  json += ",\"libraryReady\":"; json += libraryReady ? "true" : "false";
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
  ramTraceMark(RT_WEB_END);
}

void onDiagnosticsDownload() {
  ramTraceMark(RT_WEB_BEGIN);
  web.sendHeader("Cache-Control", "no-store");
  web.sendHeader("Content-Disposition", String("attachment; filename=RavestickBoard_Diagnose_v") + FW_VERSION + ".txt");
  web.send(200, "text/plain; charset=utf-8", diagnosticReport());
  ramTraceMark(RT_WEB_END);
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
      else if (count > 0 && output[count - 1] == 0x1F6F8) output[count - 1] = 0xE34D;
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
// A trailing VS15 selects Ravestick banana or the UFO with beam (same as decodeText).
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
    if ((cp == 0x1F34C || cp == 0x1F6F8) && pos < input.length()) {
      size_t lookahead = pos;
      if (readUtf8Codepoint(input, lookahead) == 0xFE0E) {
        pos = lookahead;
        cp = cp == 0x1F34C ? 0xE34C : 0xE34D;
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
  if (streamDrawingTile) {
    if (x < streamTileStart || x >= streamTileStart + streamTileCols) return;
    x -= streamTileStart;
    width = streamTileCols;
  }
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

// 24 original icons plus separately implemented custom and screenshot-derived sprites.
static const uint32_t RB_SYMBOL_CODES[RB_ORIGINAL_SYMBOL_COUNT] = {
  0x1F413, 0x2B50, 0x2744, 0x2764, 0x260E, 0x1F440,
  0x1F6D1, 0x26A0, 0x1F53A, 0x1F412, 0x1F41F, 0x1F980,
  0x1F34E, 0x1F347, 0x1F34C, 0x1F418, 0x1F95A, 0x1F357,
  0x1F3C6, 0x1F68C, 0x1F680, 0x2601, 0x1F308, 0x26A1
};
// Screenshot-reconstructed: 25 kiss, 26 flower (legacy rose alias), 27 bee, 29 cow.
// Own pixel art: 24 Ravestick banana, 28 rubber duck, 30 cactus, 31 UFO,
// 32 UFO with green beam (private use U+E34D), 33 green alien with dark eyes.
// All earlier indices remain compatible. Slot 0 (Huhn) intentionally updated
// from user's Excel. Slots 25/26/27/29 re-extracted from app screenshots
// without the former one-column shift or lost rightmost pixels.
static const uint32_t RB_NEW_SYMBOL_CODES[] = {
  0x1F48B, 0x1F339, 0x1F41D, 0x1F986, 0x1F404,
  0x1F335, 0x1F6F8, 0xE34D, 0x1F47D
};

// Excel motifs: original extra ID 36..62, second Ziggi variant appended as ID 63.
// Earlier 0..35 IDs remain stable; the crab graphic itself is replaced in slot 11.
static const uint32_t RB_EXCEL_EXTRA_CODES[] = {
  0x1F7E6, 0x1F328, 0x1F48E, 0x1F343, 0x1F6AC, 0x1F33F, 0x2B07,
  0x2B06, 0x2B05, 0x27A1, 0x2600, 0x1F534, 0x1F7E5, 0x1F4A7,
  0x1F32B, 0x1F7E3, 0x1F7E1, 0x1F7E2, 0x1F6AD, 0x1F6AB, 0x1F539,
  0x1F535, 0x26AA, 0x1FAAD, 0x1F36D, 0x1F964, 0x1F4F9, 0x1F53B
};
static constexpr uint8_t RB_EXCEL_EXTRA_COUNT = 28;

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
  if (cp == 0x1F33C) return 26; // Blume (modern name), old 🌹 stays supported
  if (cp == 0x1F424) return 34; // Kueken, pixelart from Excel
  if (cp == 0x1F438) return 35; // Frosch, pixelart from Excel
  for (uint8_t i = 0; i < RB_EXCEL_EXTRA_COUNT; ++i) {
    if (RB_EXCEL_EXTRA_CODES[i] == cp) return 36 + i;
  }
  for (uint8_t i = 0; i < 9; ++i) {
    if (RB_NEW_SYMBOL_CODES[i] == cp) return 25 + i;
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

// Blume screenshot has an excessive right margin beside adjacent letters.
// Keep original 16x16 sprite pixels unchanged; tighten only the right-hand advance.
uint8_t tokenGap(uint32_t cp) { return cp == 0x1F339 || cp == 0x1F33C ? 0 : 1; }

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
  int w = 0;
  for (int i = 0; i < count; ++i) {
    const uint32_t cp = displayCp(codes[i], layout);
    w += tokenWidth(cp, layout) + (i + 1 < count ? tokenGap(cp) : 0);
  }
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
    x0 += tokenWidth(cp, layout) + tokenGap(cp); // Blume: compact right margin
  }
}

// Width and count are computed with exactly the same tokenWidth/displayCp
// as the firmware renderer. Limit is pixel/byte based, not character based.
TextMeasure measureText(const String &text, uint8_t layout) {
  TextMeasure m = {0, 0};
  size_t pos = 0;
  uint32_t raw = 0;
  uint8_t finalGap = 0;
  while (nextTextCodepoint(text, pos, raw)) {
    const uint32_t cp = displayCp(raw, layout);
    finalGap = tokenGap(cp);
    const uint32_t step = uint32_t(tokenWidth(cp, layout)) + finalGap;
    ++m.chars;
    m.pixels += step;
    if (m.pixels > MAX_COLUMNS + 1) break; // budget exceeded; reject early
  }
  if (m.chars) m.pixels -= finalGap; // last token has no trailing gap
  return m;
}

void drawInlineUtf8(const String &text, uint8_t layout, uint8_t align,
                    uint8_t top, uint8_t color, uint16_t width,
                    uint16_t offset=0, uint16_t available=0) {
  const TextMeasure m = measureText(text, layout);
  if (layout == 2) align = 1;
  const int span=available?available:width;
  int x0 = int(offset)+(align == 0 ? 0 : align == 2 ? span - int(m.pixels) : (span - int(m.pixels)) / 2);
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
    x0 += tokenWidth(cp, layout) + tokenGap(cp);
  }
}

// Only draw tokens intersecting the current 64-column viewport. The pixel
// painter and all glyph/symbol rules are copied from drawInlineUtf8 above;
// only clipping and the pre-measured width differ.
void drawInlineUtf8Window(const String &text, uint8_t layout, uint8_t align,
                          uint8_t top, uint8_t color, uint16_t width, uint32_t measuredPixels,
                          uint16_t offset=0, uint16_t available=0) {
  if (layout == 2) align = 1;
  const int span = available ? available : width;
  int x0 = int(offset) + (align == 0 ? 0 : align == 2 ? span - int(measuredPixels) : (span - int(measuredPixels)) / 2);
  const int right = int(streamTileStart + streamTileCols);
  const int left = int(streamTileStart);
  size_t pos = 0;
  uint32_t raw = 0;
  while (nextTextCodepoint(text, pos, raw)) {
    const uint32_t cp = displayCp(raw, layout);
    const uint8_t advanceWidth = tokenWidth(cp, layout);
    if (x0 >= right) break;
    if (x0 + advanceWidth > left) {
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

    }
    x0 += advanceWidth + tokenGap(cp);
  }
}

// For a long text, create only its original 107-byte protocol header.
// The following RGB bitplanes are generated lazily, as the BLE sender asks
// for bytes in wire order. No change to CoolLEDX framing, CRC/XOR or chunks.
size_t prepareStreamText(const String &line1, const String *line2,
                         uint8_t layout, uint8_t color1, uint8_t color2,
                         uint16_t width, uint32_t chars1, uint32_t chars2,
                         uint32_t pixels1, uint32_t pixels2) {
  if (width < SIGN_WIDTH || width > MAX_COLUMNS) return 0;
  const size_t rgbBytes = size_t(width) * BYTES_PER_COLUMN;
  if (rgbBytes + IMAGE_HEADER_BYTES > MAX_IMAGE_BYTES) return 0;
  if (textPayload) { free(textPayload); textPayload = nullptr; }
  pixelData = nullptr;
  streamPayload = true;
  streamDrawingTile = false;
  streamImageWidth = width;
  streamCachedTile = 0xFFFF;
  streamTileRenders = 0;
  streamFirstLine = &line1; streamSecondLine = line2;
  streamLayout = layout;
  streamFirstColor = color1; streamSecondColor = color2;
  streamFirstPixels = pixels1; streamSecondPixels = pixels2;
  memset(streamHeader, 0, sizeof(streamHeader));
  streamHeader[24] = (uint8_t)min(chars1 + chars2, (uint32_t)80);
  for (uint8_t i = 0; i < streamHeader[24]; ++i) streamHeader[25 + i] = 0x30;
  streamHeader[105] = rgbBytes >> 8;
  streamHeader[106] = rgbBytes & 0xFF;
  ramTraceMark(RT_RENDER_BEGIN, (uint16_t)(rgbBytes + IMAGE_HEADER_BYTES));
  ramTraceMark(RT_BUFFER_ALLOC, (uint16_t)sizeof(streamTile));
  Serial.printf("[STREAM] %u Bildbytes im 384-B-Tile statt Vollpuffer; Gesamtlaenge=%u B\n",
    (unsigned)rgbBytes, (unsigned)(rgbBytes + IMAGE_HEADER_BYTES));
  return rgbBytes + IMAGE_HEADER_BYTES;
}

void renderStreamTile(uint16_t column) {
  const uint16_t begin = (column / STREAM_TILE_COLUMNS) * STREAM_TILE_COLUMNS;
  if (streamCachedTile == begin) return;
  streamTileStart = begin;
  streamTileCols = min((uint16_t)STREAM_TILE_COLUMNS, (uint16_t)(streamImageWidth - begin));
  memset(streamTile, 0, streamTileCols * BYTES_PER_COLUMN);
  // setPixel sees the true x coordinate but writes only the local tile.
  pixelData = streamTile;
  streamDrawingTile = true;
  if (streamLayout == 1) {
    drawInlineUtf8Window(*streamFirstLine, 1, 0, 0, streamFirstColor, streamImageWidth, streamFirstPixels);
    if (streamSecondLine)
      drawInlineUtf8Window(*streamSecondLine, 1, 0, 8, streamSecondColor, streamImageWidth, streamSecondPixels);
  } else {
    drawInlineUtf8Window(*streamFirstLine, streamLayout, 1,
      streamLayout == 0 ? 1 : streamLayout == 2 ? 4 : 0,
      streamFirstColor, streamImageWidth, streamFirstPixels);
  }
  streamDrawingTile = false;
  pixelData = nullptr;
  streamCachedTile = begin;
  ++streamTileRenders;
}

void copyPayloadSlice(size_t offset, size_t length, uint8_t *dest) {
  if (rb43StreamBand) { rb43CopyMixedSlice(offset,length,dest); return; }
  if (rb42StreamBand) { rb42CopyBandSlice(offset,length,dest); return; }
  if (!streamPayload) { memcpy(dest, textPayload + offset, length); return; }
  const size_t planeBytes = size_t(streamImageWidth) * (SIGN_HEIGHT / 8);
  for (size_t i = 0; i < length; ++i) {
    const size_t pos = offset + i;
    if (pos < IMAGE_HEADER_BYTES) { dest[i] = streamHeader[pos]; continue; }
    const size_t image = pos - IMAGE_HEADER_BYTES;
    const size_t plane = image / planeBytes;
    const size_t withinPlane = image % planeBytes;
    const uint16_t col = withinPlane / (SIGN_HEIGHT / 8);
    const uint8_t rowBlock = withinPlane % (SIGN_HEIGHT / 8);
    renderStreamTile(col);
    dest[i] = streamTile[plane * streamTileCols * (SIGN_HEIGHT / 8) +
      (col - streamTileStart) * (SIGN_HEIGHT / 8) + rowBlock];
  }
}


size_t renderText(const String &text, uint8_t color, uint16_t &width) {
  const TextMeasure m = measureText(text, pendingLayout);
  if (m.pixels < 1 || m.pixels > MAX_COLUMNS) return 0;
  width = m.pixels < SIGN_WIDTH ? SIGN_WIDTH : (uint16_t)m.pixels;
  const uint32_t rgbBytes = uint32_t(width) * BYTES_PER_COLUMN;
  if (rgbBytes + IMAGE_HEADER_BYTES >= STREAM_THRESHOLD_BYTES)
    return prepareStreamText(text, nullptr, pendingLayout, color, 0, width,
                             m.chars, 0, m.pixels, 0);
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
  if (n1 < 0 || n2 < 0) return 0;
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
  copyPayloadSlice(offset, length, raw + p);
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
uint8_t pendingSpeed = 80;
bool pendingPowerOn = true;
bool powerStateKnown = false;
bool lastCommandedPowerOn = true;

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
  httpSetStatus((uint16_t)status);
  web.send(status, "application/json; charset=utf-8",
           "{\"ok\":" + String(ok ? "true" : "false") + ",\"message\":" + jsonQuote(message) + "}");
}

bool v042IsPlaying();
void onStatus() {
  web.sendHeader("Cache-Control", "no-store");
  web.send(200, "application/json; charset=utf-8",
           "{\"state\":" + jsonQuote(currentState) + ",\"message\":" + jsonQuote(currentMessage) + ",\"playing\":" + String(autoProgram ? "true" : "false") + ",\"current\":" + String(currentPage) +
           ",\"programPlaying\":" + String(v042IsPlaying() ? "true" : "false") + ",\"powerKnown\":" + String(powerStateKnown ? "true" : "false") + ",\"powerOn\":" + String(lastCommandedPowerOn ? "true" : "false") + ",\"brightness\":" + String(pendingBrightness) + ",\"speed\":" + String(pendingSpeed) + "}");
  ramTraceMark(RT_STATUS_LOW, 0, false);
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

struct LibraryMeta;
bool v042DetachLibraryShortcuts(uint32_t, LibraryMeta &);
void v042EraseBlock(uint32_t);
uint8_t v042BlockPageCount(uint32_t);
void v042SendBlock();
#include "RavestickBoard_Library.h"

void defaultPage(PageRecord &p) {
  memset(&p, 0, sizeof(p));
  p.line1[0] = 0;
  p.line2[0] = 0;
  p.layout = 0; p.align = 1; p.color1 = 6; p.color2 = 7;
  p.seconds = 7;
}

// The block editor is a volatile workspace, not a second persistent library.
// The old rbpages namespace is intentionally left intact for backwards compatibility
// with archived firmware; it is neither read nor erased by v0.4.2.1.
void loadPages() {
  for (auto &page : boardPages) defaultPage(page);
  pageCount = 1;
  diagnosticEvent("EDITOR", "Leere RAM-Arbeitsflaeche initialisiert; kein NVS-Autospeichern");
}

bool savePages() {
  // Legacy editor endpoints still call this, but no flash write is performed.
  // Only an explicit /api/block/save persists a complete sequence in LittleFS.
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
      // Blank pages are valid: they can serve as a pause or dark screen.
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
  respond(true, "Seite im RAM-Editor aktualisiert.");
}

void onProgramAdd() {
  if (pageCount >= PAGE_LIMIT) { respond(false, "Maximal 25 Seiten möglich.", 400); return; }
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

#include "RavestickBoard_v042.h"

void queueText() {
  if (!reserveJob()) return;
  const String text = web.arg("text");
  const String second = web.arg("line2");
  const int color = web.arg("color").toInt();
  const int color2 = web.hasArg("color2") ? web.arg("color2").toInt() : color;
  const int brightness = pendingBrightness; // v0.4.0.1: global live setting
  const String mode = web.arg("mode");
  const int speed = pendingSpeed; // v0.4.0.1: global live setting
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
  autoProgram = false;rb42ProgramPlaying=false;
  pendingText = text;
  pendingSecondLine = second;
  pendingLayout = layout;
  pendingColor = color;
  pendingColor2 = color2;
  pendingMode = mode == "scroll" ? 0x02 : 0x01;
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
  if (rb42ProgramPlaying) { respond(false, "Während eines Programms bleibt das Tempo fest. Erst Programm stoppen.", 409); return; }
  if (!reserveJob()) return;
  const int val = web.arg("speed").toInt();
  if (val < 1 || val > 100) { respond(false, "Tempo muss zwischen 1 und 100 liegen.", 400); return; }
  pendingSpeed = val;
  nextJob = SPEED; currentState = "queued";
  currentMessage = "Tempo wird ohne Bild-Upload angepasst.";
  respond(true, currentMessage, 202);
}
// Ambiente: one normal 64x16 bitmap with all pixels set to the chosen RGB bitplanes.
// The normal picture upload is used; no unverified new CoolLEDX command.
size_t renderAmbience(uint8_t color) {
  const uint16_t rgbBytes = SIGN_WIDTH * BYTES_PER_COLUMN;
  if (color < 1 || color > 7 || !allocateRenderBuffer(rgbBytes)) return 0;
  const uint16_t plane = SIGN_WIDTH * (SIGN_HEIGHT / 8);
  if (color & 4) memset(pixelData, 0xFF, plane);
  if (color & 2) memset(pixelData + plane, 0xFF, plane);
  if (color & 1) memset(pixelData + plane * 2, 0xFF, plane);
  textPayload[24] = 1;
  textPayload[25] = 0x30;
  textPayload[105] = rgbBytes >> 8;
  textPayload[106] = rgbBytes & 0xFF;
  return IMAGE_HEADER_BYTES + rgbBytes;
}

void queueAmbience() {
  if (!reserveJob()) return;
  const int color = web.arg("color").toInt();
  if (color < 1 || color > 7) { respond(false, "Ungueltige Ambiente-Farbe.", 400); return; }
  autoProgram = false;rb42ProgramPlaying=false;
  pendingColor = color;
  pendingMode = 0x01;
  nextJob = AMBIENCE;
  currentState = "queued";
  currentMessage = "Ambiente: einfarbiges 64x16-Bild wird gesendet ...";
  diagnosticEvent("AMBIENTE", String("Vollbild-Farbe ") + color);
  respond(true, currentMessage, 202);
}

// CoolLEDX SetMode: command 0x06, value 0x01..0x08.
// All eight 0x01..0x08 modes hardware-tested on our own CoolLEDX sign in v0.3.4.
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
  diagnosticEvent("MODE", String("0x06 Steuerbefehl, hardwarebestaetigter Moduswert ") + hexValue);
  currentMessage = String("Anzeigemodus ") + hexValue + " wird ohne Bild-Upload angepasst.";
  respond(true, currentMessage, 202);
}

// CoolLEDX app power switch (reference driver): 0x09 0x00 = OFF, 0x09 0x01 = ON.
// This controls the LED display, NOT ESP32/WLAN power. Physical state is not read back.
void queuePower() {
  if (!reserveJob()) return;
  const String state = web.arg("state");
  if (state != "on" && state != "off") {
    respond(false, "Bitte 'on' oder 'off' angeben.", 400); return;
  }
  pendingPowerOn = (state == "on");
  nextJob = POWER;
  currentState = "queued";
  currentMessage = "Anzeige wird ohne Bild-Upload geschaltet.";
  diagnosticEvent("POWER", String("BLE 0x09, Wert ") + (pendingPowerOn ? "0x01 EIN" : "0x00 AUS"));
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
  ramTraceMark(RT_JOB_BEGIN);
  Serial.printf("Auftrag: %s\n", job == TEXT ? "TEXT" : job == PAGE ? "SEITE" : job == BAND ? "BLOCKBAND" : job == BRIGHTNESS ? "HELLIGKEIT" : job == SPEED ? "TEMPO" : job == MODE ? "MODUS" : job == AMBIENCE ? "AMBIENTE" : "POWER");
  uint16_t width = SIGN_WIDTH;
  size_t payloadLen = 0;
  uint8_t raw[140];
  uint8_t frame[300];
  const uint16_t MAX_FRAME = sizeof(frame);
  if (job == BAND) {
    payloadLen = renderProgramBand();
    if (!payloadLen) { currentMessage = "Blockband nicht renderbar oder Datenlimit überschritten."; return false; }
  } else if (job == TEXT && rb43CurrentGroup) {
    payloadLen=rb43RenderGroup();
    if(!payloadLen){currentMessage="Gemeinsames Laufband konnte nicht gerendert werden.";return false;}
  } else if (job == TEXT) {
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
      if (rgbBytes + IMAGE_HEADER_BYTES >= STREAM_THRESHOLD_BYTES) {
        payloadLen = prepareStreamText(message, &pendingSecondLine, 1, color, pendingColor2,
          width, first.chars, bottom.chars, first.pixels, bottom.pixels);
      } else {
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
      }
    } else {
      payloadLen = renderText(message, color, width);
    }
    if (!payloadLen) {
      currentMessage = "Text passt nicht in den Pufferspeicher.";
      return false;
    }
  } else if (job == AMBIENCE) {
    payloadLen = renderAmbience(color);
    if (!payloadLen) { currentMessage = "Ambiente-Bild konnte nicht erstellt werden."; return false; }
  } else if (job == PAGE) {
    if (pageIndex >= pageCount) { currentMessage = "Seite nicht vorhanden."; return false; }
    payloadLen = renderBlockPage(boardPages[pageIndex]);
    if (!payloadLen) { currentMessage = "Seite passt nicht auf die 64x16-Pixelmatrix."; return false; }
  }

  ramTraceMark(RT_RENDER_END, (uint16_t)payloadLen);
  ramTraceMark(RT_BLE_BEGIN, (uint16_t)payloadLen);
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
  ramTraceMark(RT_BLE_LINK, (uint16_t)payloadLen);
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
  ramTraceMark(RT_BLE_READY, (uint16_t)payloadLen);

  // Check negotiated MTU BEFORE writing any text or mode packets.
  const size_t chunks = (payloadLen + DATA_CHUNK - 1) / DATA_CHUNK;
  uint16_t mtu = bleClient->getMTU();
  // Rare v0.4.0.2 failure: a BLE client reported MTU=0 after connection.
  // Retry exactly once by rebuilding the connection, BEFORE sending commands.
  if (mtu < DATA_CHUNK + 32) {
    diagnosticEvent("BLE", String("MTU ungueltig (") + mtu + "), Neuverbindung einmalig");
    disconnectSign();
    delay(250);
    ++diagnostics.bleAttempts;
    if (!bleClient->connect(address, 0xFF, 10000)) {
      currentMessage = "Bluetooth-MTU ungueltig und Neuverbindung fehlgeschlagen.";
      diagnosticEvent("BLE", currentMessage, true);
      return false;
    }
    ++diagnostics.bleConnected;
    diagnosticEvent("BLE", "Neuverbindung fuer MTU erfolgreich");
    delay(650);
    svc = bleClient->getService(BLEUUID((uint16_t)0xFFF0));
    if (!svc) { currentMessage = "BLE-Service nach MTU-Neuverbindung fehlt."; disconnectSign(); return false; }
    c = svc->getCharacteristic(BLEUUID((uint16_t)0xFFF1));
    if (!c || !c->canWriteNoResponse()) {
      currentMessage = "BLE-Charakteristik nach MTU-Neuverbindung fehlt.";
      disconnectSign(); return false;
    }
    if (c->canNotify()) c->registerForNotify(onNotify);
    mtu = bleClient->getMTU();
    ramTraceMark(RT_BLE_READY, (uint16_t)payloadLen);
  }
  Serial.printf("MTU: %u; Text: %u Bytes; Pakete: %u\n", mtu,
                (unsigned)payloadLen, (unsigned)chunks);
  diagnosticEvent("BLE", String("MTU=") + mtu + " Text-Payload=" + (unsigned)payloadLen + " Bytes, Pakete=" + (unsigned)chunks);
  if (mtu < DATA_CHUNK + 32) {
    currentMessage = "Bluetooth-MTU weiterhin ungueltig nach Neuverbindung.";
    diagnosticEvent("BLE", currentMessage, true);
    disconnectSign(); return false;
  }
  if (job == TEXT || job == PAGE || job == AMBIENCE || job == BAND) {
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
  if ((job == TEXT || job == BAND || job == BRIGHTNESS || job == AMBIENCE) && !sendSimple(c, 0x08, rawBrightness)) {
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
  if (job == POWER && !sendSimple(c, 0x09, pendingPowerOn ? 0x01 : 0x00)) {
    currentMessage = "Ein/Aus-Befehl konnte nicht gesendet werden.";
    disconnectSign(); return false;
  }
  if (job == TEXT || job == PAGE || job == AMBIENCE || job == BAND) {
    const uint8_t actualMode = (job == PAGE || job == AMBIENCE || job == BAND) ? 0x01 : mode;
    if (!sendSimple(c, 0x06, actualMode)) {
      currentMessage = "Anzeigemodus konnte nicht gesetzt werden.";
      disconnectSign(); return false;
    }
    // Originalhardware: 0x07, Rohwert 0..255; UI-Regler 1..100. Laufzeitkalibrierung spaeter.
    if ((job == BAND || (job == TEXT && mode == 0x02)) && !sendSimple(c, 0x07, (uint16_t(speed) * 255 + 50) / 100)) {
      currentMessage = "Laufgeschwindigkeit konnte nicht gesetzt werden.";
      disconnectSign(); return false;
    }
    ramTraceMark(RT_TX_BEGIN, (uint16_t)payloadLen);
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
      ramTraceMark(RT_TX_PROGRESS, (uint16_t)(i+1), ((i+1)%32==0 || i+1==chunks));
    }
    ramTraceMark(RT_TX_END, (uint16_t)payloadLen);
    if (streamPayload || rb43StreamBand) {
      Serial.printf("[STREAM] Fertig: %lu Tiles berechnet (einschl. MTU-Vorabpruefung)\n",
                    (unsigned long)streamTileRenders);
    }
  }
  delay((job == PAGE || job == TEXT || job == AMBIENCE) ? 220 : 30);
  const bool controlJob = job == BRIGHTNESS || job == SPEED || job == MODE || job == POWER;
  if (controlJob) controlIdleUntilMs = millis() + 2500UL;
  else { controlIdleUntilMs = 0; disconnectSign(); }
  currentMessage = job == BAND ? "Blockfolge mit einem Upload übertragen."
      : job == PAGE ? String("Seite ") + (pageIndex + 1) + " angezeigt."
      : job == AMBIENCE ? "Ambiente-Farbe übertragen."
      : job == TEXT ? String("Text erfolgreich gesendet. ") + (mode == 0x02 ? "Lauftext." : "Standbild.")
      : job == BRIGHTNESS ? "Helligkeit ohne Bild-Upload gesendet."
      : job == SPEED ? "Laufgeschwindigkeit ohne Bild-Upload gesendet."
      : job == MODE ? "Anzeigemodus ohne Bild-Upload gesendet."
      : String("Anzeige ") + (pendingPowerOn ? "EIN" : "AUS") + "-Befehl ohne Bild-Upload gesendet (Hardwarewirkung testen).";
  if (job == POWER) { lastCommandedPowerOn = pendingPowerOn; powerStateKnown = true; }
  if (job == PAGE) { currentPage = pageIndex; ++pageShows; }
  return true;
}

static void onHomePage() {
  web.sendHeader("Cache-Control", "no-cache");
  web.sendHeader("ETag", PAGE_HTML_ETAG);
  web.sendHeader("Vary", "Accept-Encoding");
  if (web.header("If-None-Match") == PAGE_HTML_ETAG) {
    httpSetStatus(304);
    web.send(304, "text/html; charset=utf-8", "");
    return;
  }
  web.sendHeader("Content-Encoding", "gzip");
  // The binary gzip payload contains NUL bytes. Explicit byte length is REQUIRED.
  web.send_P(200, "text/html; charset=utf-8", (PGM_P)PAGE_HTML_GZIP, PAGE_HTML_GZIP_LEN);
}

void setup() {
  Serial.begin(115200);
  delay(1200);
  Serial.println("RavestickBoard Webcontroller v0.4.2.5-dev");
  diagnosticEvent("SYSTEM", String("Start v") + FW_VERSION + " Reset=" + resetDescription());
  Serial.printf("[RAM] Start: frei=%u B, groesster Block=%u B\n",
                (unsigned)ESP.getFreeHeap(), (unsigned)ESP.getMaxAllocHeap());
  BLEDevice::init("");
  BLEDevice::setMTU(247);
  bleClient = BLEDevice::createClient();
  loadPages();
  libraryBegin();
  v042Begin();
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

  ramTraceMark(RT_BOOT);
  httpStartSampler();
  static const char *requestHeaders[] = {"If-None-Match"};
  web.collectHeaders(requestHeaders, 1);
  httpRoute("/", HTTP_GET, onHomePage);
  httpRoute("/api/status", HTTP_GET, onStatus);
  httpRoute("/api/font", HTTP_GET, onFontJson);
  httpRoute("/api/diagnostics", HTTP_GET, onDiagnosticsJson);
  httpRoute("/api/diagnostics.txt", HTTP_GET, onDiagnosticsDownload);
  httpRoute("/api/ram.txt", HTTP_GET, onRamTraceDownload);
  httpRoute("/api/http.txt", HTTP_GET, onHttpTraceDownload);
  httpRoute("/api/program", HTTP_GET, onProgramGet);
  httpRoute("/api/program/set", HTTP_POST, onProgramSet);
  httpRoute("/api/program/add", HTTP_POST, onProgramAdd);
  httpRoute("/api/program/delete", HTTP_POST, onProgramDelete);
  httpRoute("/api/program/move", HTTP_POST, onProgramMove);
  httpRoute("/api/program/show", HTTP_POST, onProgramShow);
  httpRoute("/api/library", HTTP_GET, onLibraryList);
  httpRoute("/api/library/get", HTTP_GET, onLibraryGet);
  httpRoute("/api/library/init", HTTP_POST, onLibraryInit);
  httpRoute("/api/library/save", HTTP_POST, onLibrarySave);
  httpRoute("/api/library/delete", HTTP_POST, onLibraryDelete);
  httpRoute("/api/library/move", HTTP_POST, onLibraryMove);
  httpRoute("/api/library/shortcut", HTTP_POST, onLibraryShortcut);
  httpRoute("/api/library/send", HTTP_POST, onLibrarySend);
  httpRoute("/api/block/save", HTTP_POST, v042SaveBlock);
  httpRoute("/api/block/clone", HTTP_POST, v042CloneBlock);
  httpRoute("/api/block/show", HTTP_POST, v042SendDraftBand);
  httpRoute("/api/block/get", HTTP_GET, v042GetBlock);
  httpRoute("/api/block/load", HTTP_POST, v042LoadBlock);
  httpRoute("/api/block/send", HTTP_POST, v042SendBlock);
  httpRoute("/api/block/reset", HTTP_POST, v042ResetBlock);
  httpRoute("/api/block/meta", HTTP_POST, v042BlockMeta);
  httpRoute("/api/collections", HTTP_GET, v042ListCollections);
  httpRoute("/api/collections/get", HTTP_GET, v042GetCollection);
  httpRoute("/api/collections/save", HTTP_POST, v042SaveCollection);
  httpRoute("/api/collections/delete", HTTP_POST, v042DeleteCollection);
  httpRoute("/api/collections/run", HTTP_POST, v042RunCollection);
  httpRoute("/api/collections/stop", HTTP_POST, v042StopCollection);
  httpRoute("/api/shortcuts", HTTP_GET, v042GetShortcuts);
  httpRoute("/api/shortcuts/set", HTTP_POST, v042SetShortcut);
  httpRoute("/api/shortcuts/run", HTTP_POST, v042RunShortcut);
  httpRoute("/apple-touch-icon.png", HTTP_GET, v042ServeIcon);
  httpRoute("/favicon.ico", HTTP_GET, v042ServeIcon);
  httpRoute("/api/text", HTTP_POST, queueText);
  httpRoute("/api/brightness", HTTP_POST, queueBrightness);
  httpRoute("/api/speed", HTTP_POST, queueSpeed);
  httpRoute("/api/mode", HTTP_POST, queueMode);
  httpRoute("/api/power", HTTP_POST, queuePower);
  httpRoute("/api/ambience", HTTP_POST, queueAmbience);
  web.onNotFound([]() { httpBegin(); respond(false, "Nicht gefunden.", 404); httpEnd(); });
  if (apStarted) {
    web.begin();
    Serial.println("Webserver bereit.");
    diagnosticEvent("HTTP", "Webserver gestartet");
  } else {
    Serial.println("Webserver nicht gestartet (kein WLAN-AP).");
  }
}

void loop() {
  // Read once per loop; timer callback uses this cached value without Wi-Fi calls.
  httpClientsCached = WiFi.softAPgetStationNum();
  web.handleClient();
  v042Tick();
  if (nextJob == NONE && controlIdleUntilMs &&
      (int32_t)(millis() - controlIdleUntilMs) >= 0) {
    disconnectSign(); controlIdleUntilMs = 0;
  }
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
    const uint8_t pageIndex = pendingPage;
    const String text = pendingText;
    nextJob = NONE;
    currentState = "sending";
    currentMessage = "Bluetooth-Übertragung läuft …";
    const uint32_t started = millis();
    diagnosticEvent("JOB", job == TEXT ? "Textübertragung startet"
      : job == PAGE ? "Seitenübertragung startet"
      : job == BAND ? "Blockband-Übertragung startet"
      : job == SPEED ? "Tempowechsel startet"
      : job == MODE ? "Moduswechsel startet"
      : job == POWER ? "Anzeige Ein/Aus startet" : job == AMBIENCE ? "Ambiente startet" : "Helligkeit startet");
    ramActiveJob = ++ramJobSeq;
    ramActiveJobType = (uint8_t)job;
    bool ok = runJob(job, color, brightness, text, mode, speed, pageIndex);
    // Restart a failed picture upload from packet 1; never resume a partial bitmap.
    // No automatic retry of manual BLE control commands (they may have taken effect).
    if (!ok && (job==TEXT || job==BAND || job==PAGE || job==AMBIENCE) &&
        currentMessage.indexOf("nicht bestaetigt")>=0) {
      diagnosticEvent("BLE", "Vollstaendiger Wiederholungsversuch (1/1) nach Paketfehler");
      disconnectSign();
      delay(400);
      ok=runJob(job, color, brightness, text, mode, speed, pageIndex);
    }
    const bool finishedScrollGroup = (job == TEXT && rb43CurrentGroup);
    v042AfterJob(job, ok);
    // A completed one-shot scrollband must not hijack the next ordinary text upload.
    if (finishedScrollGroup) rb43CurrentGroup = false;
    ramTraceMark(RT_JOB_END);
    ramActiveJob = 0;
    ramActiveJobType = NONE;
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
