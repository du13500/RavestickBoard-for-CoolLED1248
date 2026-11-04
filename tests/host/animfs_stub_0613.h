// Attrappen fuer den Hosttest von RavestickBoard_Anim.h (v0.6.1.0, seit 0.6.1.3 mit Kurzwahl-Struktur und rb42Write aus dem echten Quelltext): Arduino-String, Dateisystem im Speicher, Webserver.
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <memory>
#include <iostream>
#include <algorithm>
using std::min;
static constexpr uint16_t SIGN_WIDTH = 64;
static constexpr uint16_t SIGN_HEIGHT = 16;
static constexpr uint32_t BYTES_PER_COLUMN = (SIGN_HEIGHT / 8) * 3;
static constexpr size_t DATA_CHUNK = 128;

// ---- String ------------------------------------------------------------------------------------
class String {
 public:
  std::string s;
  String() {}
  String(const char *c) : s(c ? c : "") {}
  String(char c) : s(1, c) {}
  String(int v) : s(std::to_string(v)) {}
  String(unsigned v) : s(std::to_string(v)) {}
  String(long v) : s(std::to_string(v)) {}
  String(unsigned long v) : s(std::to_string(v)) {}
  String(const std::string &x) : s(x) {}
  size_t length() const { return s.size(); }
  const char *c_str() const { return s.c_str(); }
  char operator[](size_t i) const { return s[i]; }
  char &operator[](size_t i) { return s[i]; }
  String &operator+=(const String &o) { s += o.s; return *this; }
  String &operator+=(const char *o) { s += o; return *this; }
  String &operator+=(char o) { s += o; return *this; }
  bool endsWith(const char *x) const { size_t n = std::strlen(x); return s.size() >= n && s.compare(s.size() - n, n, x) == 0; }
  String substring(size_t a) const { return String(s.substr(a)); }
  String substring(size_t a, size_t b) const { return String(s.substr(a, b - a)); }
  int lastIndexOf(char c) const { size_t p = s.rfind(c); return p == std::string::npos ? -1 : (int)p; }
  long toInt() const { char *e; long v = std::strtol(s.c_str(), &e, 10); return v; }
  void trim() { size_t a = s.find_first_not_of(" \t\r\n"); if (a == std::string::npos) { s.clear(); return; } size_t b = s.find_last_not_of(" \t\r\n"); s = s.substr(a, b - a + 1); }
  bool operator==(const char *o) const { return s == o; }
  bool operator==(const String &o) const { return s == o.s; }
  bool operator!=(const char *o) const { return s != o; }
};
inline String operator+(const String &a, const String &b) { return String(a.s + b.s); }
inline String operator+(const String &a, const char *b) { return String(a.s + b); }
inline String operator+(const char *a, const String &b) { return String(std::string(a) + b.s); }
inline String operator+(const String &a, char b) { return String(a.s + b); }
inline String operator+(const String &a, int b) { return String(a.s + std::to_string(b)); }
inline String operator+(const String &a, unsigned b) { return String(a.s + std::to_string(b)); }
inline String operator+(const String &a, unsigned long b) { return String(a.s + std::to_string(b)); }

// ---- Dateisystem im Speicher -----------------------------------------------------------------
struct Node { std::vector<uint8_t> bytes; };
static std::map<std::string, std::shared_ptr<Node>> fsFiles;
static std::set<std::string> fsDirs;
static size_t fsTotal = 1500000;
static bool fsFailWrites = false;
static bool fsRenameOverwrite = true;   // false: rename auf eine vorhandene Zieldatei scheitert (Nachbildung eines Dateisystems ohne Ersetzen)
static int fsRenameCalls = 0, fsFailRenameFrom = 0, fsFailRenameTo = 0;   // rename-Aufrufe Nr. from..to (1-basiert, seit RENAMECOUNT 0) scheitern
static int fsFailReadAfter = -1;         // >=0: nach so vielen Lesezugriffen liefert read() einen Fehler (-1)
static std::string fsFailReadPath;       // nur fuer diese Datei
class File {
 public:
  std::shared_ptr<Node> node; size_t pos = 0; bool dir = false; std::string path; char mode = 'r';
  std::vector<std::string> kids; size_t kidIdx = 0;
  explicit operator bool() const { return node != nullptr || dir; }
  size_t size() const { return node ? node->bytes.size() : 0; }
  bool seek(size_t p) { if (!node || p > node->bytes.size()) return false; pos = p; return true; }
  int read(uint8_t *d, size_t n) {
    if (!node) return -1;
    if (fsFailReadAfter >= 0 && path == fsFailReadPath) { if (fsFailReadAfter == 0) return -1; --fsFailReadAfter; }
    size_t k = pos + n <= node->bytes.size() ? n : node->bytes.size() - pos;
    std::memcpy(d, node->bytes.data() + pos, k); pos += k; return (int)k;
  }
  size_t write(const uint8_t *d, size_t n) {
    if (!node || mode == 'r' || fsFailWrites) return 0;   // 'u' = r+ (ueberschreiben ohne Kuerzen)
    if (mode == 'a') pos = node->bytes.size();
    if (pos + n > node->bytes.size()) node->bytes.resize(pos + n);
    std::memcpy(node->bytes.data() + pos, d, n); pos += n; return n;
  }
  void flush() {}
  void close() { node = nullptr; dir = false; }
  bool isDirectory() const { return dir; }
  const char *name() const { return nameBuf.c_str(); }
  std::string nameBuf;
  File openNextFile() {
    File f;
    if (!dir || kidIdx >= kids.size()) return f;
    const std::string full = kids[kidIdx++];
    f.node = fsFiles[full]; f.path = full; f.mode = 'r';
    f.nameBuf = full.substr(full.rfind('/') + 1);
    return f;
  }
};
static struct LittleFSStub {
  bool exists(const String &p) { return fsFiles.count(p.s) || fsDirs.count(p.s); }
  bool mkdir(const String &p) { fsDirs.insert(p.s); return true; }
  bool remove(const String &p) { return fsFiles.erase(p.s) > 0; }
  bool rename(const String &a, const String &b) {
    ++fsRenameCalls;
    if (fsFailRenameFrom > 0 && fsRenameCalls >= fsFailRenameFrom && fsRenameCalls <= fsFailRenameTo) return false;
    auto it = fsFiles.find(a.s); if (it == fsFiles.end()) return false;
    if (!fsRenameOverwrite && fsFiles.count(b.s)) return false;
    fsFiles[b.s] = it->second; fsFiles.erase(a.s); return true;
  }
  File open(const String &p, const char *mode = "r") {
    File f; f.path = p.s; f.mode = (mode[0] == 'r' && mode[1] == '+') ? 'u' : mode[0]; f.nameBuf = p.s.substr(p.s.rfind('/') + 1);
    if (fsDirs.count(p.s) && f.mode == 'r') {
      f.dir = true;
      for (auto &kv : fsFiles) if (kv.first.compare(0, p.s.size() + 1, p.s + "/") == 0) f.kids.push_back(kv.first);
      return f;
    }
    auto it = fsFiles.find(p.s);
    if (mode[0] == 'w') { fsFiles[p.s] = std::make_shared<Node>(); f.node = fsFiles[p.s]; return f; }
    if (it == fsFiles.end()) return File();
    f.node = it->second; if (mode[0] == 'a') f.pos = f.node->bytes.size();
    return f;
  }
  size_t totalBytes() { return fsTotal; }
  size_t usedBytes() { size_t u = 0; for (auto &kv : fsFiles) u += (kv.second->bytes.size() + 4095) / 4096 * 4096; return u; }
} LittleFS;

// ---- Web / Sketch-Umgebung ---------------------------------------------------------------------
static constexpr int CONTENT_LENGTH_UNKNOWN = -1;
struct WebStub {
  std::map<std::string, std::string> args; std::string out; int status = 0; int headers = 0;
  bool hasArg(const char *k) { return args.count(k) > 0; }
  String arg(const char *k) { auto it = args.find(k); return it == args.end() ? String() : String(it->second); }
  void sendHeader(const char *, const char *) { ++headers; }
  void setContentLength(int) {}
  void send(int code, const char *, const String &body) { status = code; out += body.s; }
  void sendContent(const String &b) { out += b.s; }
} web;
static String currentState = "idle", currentMessage;
static bool autoProgram = true, rb42ProgramPlaying = true;
static uint8_t pendingMode = 0x55;
enum Job : uint8_t { NONE, TEXT, BRIGHTNESS, PAGE, SPEED, MODE, POWER, AMBIENCE, BAND, ANIM };
static Job nextJob = NONE;
static std::vector<std::string> events;
static bool libraryMounted = true;
static bool libraryReady = true;
static int lastHttpStatus = 0;
static void httpSetStatus(uint16_t c) { lastHttpStatus = c; }
static void rbPhase(const char *) {}
static void diagnosticEvent(const char *tag, const String &m, bool = false) { events.push_back(std::string(tag) + ": " + m.s); }
static bool reserveJob() {
  if (currentState == "queued" || currentState == "sending") { web.sendHeader("Cache-Control", "no-store"); httpSetStatus(409); web.send(409, "application/json", "{\"ok\":false}"); return false; }
  return true;
}
static String jsonQuote(const String &in) {
  std::string o = "\"";
  for (char c : in.s) { if (c == '"' || c == '\\') { o += '\\'; o += c; } else if ((uint8_t)c < 32) o += ' '; else o += c; }
  return String(o + "\"");
}
static void libraryStartJson() { web.sendHeader("Cache-Control", "no-store"); web.setContentLength(CONTENT_LENGTH_UNKNOWN); web.status = 200; }
struct JsonChunkWriter {   // gleiche Schnittstelle wie im Sketch (Zusammenfassen in Stuecke ist hier unerheblich)
  String buf;
  void flush() { web.sendContent(buf); buf = ""; }
  void add(const String &p) { buf += p; }
  void add(const char *p) { buf += p; }
  void finish() { flush(); web.sendContent(""); }
};
