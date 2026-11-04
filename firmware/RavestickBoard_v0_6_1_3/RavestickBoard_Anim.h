// RavestickBoard v0.6.1.3-dev (eingefuehrt in 0.6.1.0) · gespeicherte Animationen (Dateien im LittleFS) ----------------------
// Wird aus RavestickBoard_v0_6_1_3.ino hinter RavestickBoard_Library.h UND RavestickBoard_v042.h eingebunden (braucht rb42ProgramPlaying). Eine .h-Datei bekommt von der
// Arduino-IDE keine automatischen Prototypen; Typen und Variablen, die der Sketch schon vorher braucht
// (RB61_META_BYTES, rb61AnimFile, ANIM_FILE ...), stehen deshalb oben im Sketch.
//
// Dateiformat  /rb-anim/a<ID>.rba  =  Kopf (448 Byte, siehe Rb61AnimHeader) + Nutzlast des Befehls 0x04:
//   Nutzlast = 24 Byte Null + Frames(1) + ms je Frame (2, big endian) + Ebenen R, G, B (je alle Frames)
//   Je Frame und Ebene 64 Spalten x 2 Byte (oberstes Bit = oberste Zeile)  ->  384 Byte je Frame.
// Die Nutzlast liegt also genau so in der Datei, wie sie an die Tafel gesendet wird; zum Senden wird sie nur
// ab Dateiposition 448 gelesen. Der Browser baut die Nutzlast (GIF-Decoder usw.) und laedt sie in Stuecken hoch.
// Jede Datei gilt nur, wenn Kopf und Dateigroesse zusammenpassen. Hochgeladen wird in /rb-anim/up.tmp und erst
// ganz am Ende per rename() zur eigentlichen Datei (Stromausfall beim Upload hinterlaesst nur die tmp-Datei).
// Seit 0.6.1.3: Der Browser schickt die CRC-32-Pruefsumme der Nutzlast mit. Sie steht im Kopf (crc[4], flags Bit 0)
// und wird beim Abschluss des Uploads und vor jedem Senden gegen die Datei geprueft. Dateien aus 0.6.1.0 bis
// 0.6.1.2 haben keine Pruefsumme (flags = 0) und werden weiter ohne diese Pruefung akzeptiert.

struct Rb61AnimHeader {
  char magic[4];          // "RBA1"
  uint16_t frames;        // 2 .. 79
  uint16_t ms;            // 1 .. 2000
  uint32_t payload;       // Laenge der Nutzlast hinter dem Kopf = 27 + frames * 384
  uint8_t flags;          // Bit 0 = crc[] gilt (seit 0.6.1.3), sonst 0
  uint8_t nameLen;        // Bytes des Namens (UTF-8), 1 .. 40
  char name[41];          // Name, mit 0 abgeschlossen
  uint8_t crc[4];         // CRC-32 der Nutzlast, little endian (als Bytefeld, damit die Struktur 448 Byte bleibt)
  uint8_t pad[5];         // auf 64 Byte aufgefuellt
  uint8_t thumb[ANIM_FRAME_BYTES]; // erstes Bild (Ebenen R, G, B zu je 128 Byte) fuer die Listenvorschau
};
static_assert(sizeof(Rb61AnimHeader) == RB61_META_BYTES, "Kopf der Animationsdatei muss 448 Byte haben");

static constexpr uint8_t RB61_MAX_ANIMS = 40;             // so viele Animationen werden hoechstens aufgelistet/angelegt
static constexpr uint32_t RB61_FREE_RESERVE = 24576UL;    // LittleFS braucht freie Bloecke zum Umschreiben
static constexpr uint16_t RB61_CHUNK_RAW = 1200;          // Bytes je Upload-Stueck (durch 3 teilbar)
static constexpr uint16_t RB61_GET_RAW = 600;             // Bytes je Lese-Stueck beim Abruf (durch 3 teilbar, spart Stapel)
static constexpr char RB61_DIR[] = "/rb-anim";
static constexpr char RB61_TMP[] = "/rb-anim/up.tmp";
static constexpr char RB61_TEMP_PATH[] = "/rb-anim/send.rba";   // v0.6.1.2: "nur senden" - Animation ohne Listeneintrag, nach dem Senden geloescht
static constexpr uint8_t RB61_FLAG_CRC = 0x01;                  // flags Bit 0: crc[] im Kopf ist gueltig (seit 0.6.1.3)
static constexpr uint16_t RB61_TEMP_ID = 60000;                 // Kennung dafuer in rb61PendingAnimId (keine gueltige Listen-ID, die gehen bis 9999)

static bool rb61Ready = false;      // Ordner vorhanden, Dateisystem eingebunden
bool rb61AnimExists(uint32_t id);   // Vorwaertsdeklaration: wird in onAnimPut (Ersetzen) vor der Definition benutzt

static struct Rb61Upload {
  bool active = false;
  uint16_t frames = 0, ms = 0;
  uint32_t total = 0, got = 0;      // Nutzlast gesamt / schon geschrieben
  bool temp = false;                // true = "nur senden": wird send.rba statt a<ID>.rba
  uint16_t replaceId = 0;           // != 0: bestehende Animation unter dieser ID ueberschreiben (Bearbeiten)
  uint32_t crc = 0;                 // angekuendigte CRC-32 der Nutzlast (seit 0.6.1.3)
  char name[42] = "";
} rb61Up;

// ---- Base64 (URL-Alphabet, ohne Fuellzeichen) -------------------------------------------------
static const char RB61_B64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
static int rb61B64Val(char c) {
  if (c >= 'A' && c <= 'Z') return c - 'A';
  if (c >= 'a' && c <= 'z') return c - 'a' + 26;
  if (c >= '0' && c <= '9') return c - '0' + 52;
  if (c == '-') return 62;
  if (c == '_') return 63;
  return -1;
}
// Dekodiert text[0..n) nach out (Platz fuer n*3/4 + 3 Byte). Rueckgabe: Anzahl Bytes oder -1.
static int rb61B64Decode(const char *text, size_t n, uint8_t *out) {
  size_t o = 0;
  uint32_t acc = 0; uint8_t bits = 0;
  for (size_t i = 0; i < n; ++i) {
    const int v = rb61B64Val(text[i]);
    if (v < 0) return -1;
    acc = (acc << 6) | (uint32_t)v; bits += 6;
    if (bits >= 8) { bits -= 8; out[o++] = (uint8_t)(acc >> bits); acc &= (1UL << bits) - 1; }
  }
  return (int)o;
}
// Kodiert in[0..n) nach out (Platz fuer (n*4+2)/3 + 1 Zeichen), mit 0 abgeschlossen. Rueckgabe: Zeichenzahl.
static size_t rb61B64Encode(const uint8_t *in, size_t n, char *out) {
  size_t o = 0;
  uint32_t acc = 0; uint8_t bits = 0;
  for (size_t i = 0; i < n; ++i) {
    acc = (acc << 8) | in[i]; bits += 8;
    while (bits >= 6) { bits -= 6; out[o++] = RB61_B64[(acc >> bits) & 63]; }
    acc &= (1UL << bits) - 1;
  }
  if (bits) out[o++] = RB61_B64[(acc << (6 - bits)) & 63];
  out[o] = 0;
  return o;
}

// ---- Dateien -----------------------------------------------------------------------------------
static String rb61Path(uint32_t id) {
  if (id == RB61_TEMP_ID) return String(RB61_TEMP_PATH);
  return String(RB61_DIR) + "/a" + String(id) + ".rba";
}

// CRC-32 (IEEE 802.3, wie zlib/PNG; Anfangswert 0xFFFFFFFF, am Ende invertieren). Ohne Tabelle: 30 KB brauchen wenige ms.
static uint32_t rb61Crc32Update(uint32_t crc, const uint8_t *d, size_t n) {
  for (size_t i = 0; i < n; ++i) {
    crc ^= d[i];
    for (uint8_t k = 0; k < 8; ++k) crc = (crc >> 1) ^ (0xEDB88320UL & (0UL - (crc & 1UL)));
  }
  return crc;
}
static uint32_t rb61HeaderCrc(const Rb61AnimHeader &h) {
  return (uint32_t)h.crc[0] | ((uint32_t)h.crc[1] << 8) | ((uint32_t)h.crc[2] << 16) | ((uint32_t)h.crc[3] << 24);
}
static void rb61SetHeaderCrc(Rb61AnimHeader &h, uint32_t c) {
  h.crc[0] = (uint8_t)c; h.crc[1] = (uint8_t)(c >> 8); h.crc[2] = (uint8_t)(c >> 16); h.crc[3] = (uint8_t)(c >> 24);
}
// CRC-32 ueber die Nutzlast einer geoeffneten Animationsdatei (ab Dateiposition 448). false = Lesefehler.
__attribute__((noinline)) static bool rb61FileCrc(File &f, uint32_t payload, uint32_t &out) {
  if (!f || !f.seek(RB61_META_BYTES)) return false;
  uint8_t buf[256];
  uint32_t crc = 0xFFFFFFFFUL, left = payload;
  while (left) {
    const size_t take = left < sizeof(buf) ? left : sizeof(buf);
    if (f.read(buf, take) != (int)take) return false;
    crc = rb61Crc32Update(crc, buf, take);
    left -= (uint32_t)take;
    rbPhase("anim_crc");
  }
  out = ~crc;
  return true;
}

static bool rb61HeaderValid(const Rb61AnimHeader &h, size_t fileSize) {
  if (memcmp(h.magic, "RBA1", 4) != 0) return false;
  if (h.frames < 2 || h.frames > ANIM_MAX_FRAMES || h.ms < 1 || h.ms > 2000) return false;
  if (h.payload != (uint32_t)ANIM_HEADER_BYTES + (uint32_t)h.frames * ANIM_FRAME_BYTES) return false;
  if (h.nameLen < 1 || h.nameLen > 40 || h.name[h.nameLen] != 0) return false;
  if (h.flags & ~RB61_FLAG_CRC) return false;
  return fileSize == (size_t)RB61_META_BYTES + h.payload;
}
static bool rb61ReadHeader(File &f, Rb61AnimHeader &h) {
  if (!f || f.size() < (size_t)RB61_META_BYTES) return false;
  f.seek(0);
  if (f.read(reinterpret_cast<uint8_t *>(&h), sizeof(h)) != (int)sizeof(h)) return false;
  return rb61HeaderValid(h, f.size());
}

static void rb61EnsureDir() {
  if (!libraryMounted) { rb61Ready = false; return; }
  if (!LittleFS.exists(RB61_DIR)) LittleFS.mkdir(RB61_DIR);
  rb61Ready = LittleFS.exists(RB61_DIR);
}
// Beim Ersetzen (Bearbeiten) kann die alte Datei kurz als a<ID>.rba.bak daliegen. Stromausfall dazwischen: fehlt die
// eigentliche Datei, wird die Sicherung zurueckgeholt, sonst weggeraeumt. Erst sammeln, dann aendern (Ordner nicht beim Durchlaufen veraendern).
static void rb61RecoverBackups() {
  String found[4]; uint8_t n = 0;
  File dir = LittleFS.open(RB61_DIR);
  if (dir && dir.isDirectory()) {
    for (File e = dir.openNextFile(); e; e = dir.openNextFile()) {
      String name = e.name();
      e.close();
      const int slash = name.lastIndexOf('/');
      if (slash >= 0) name = name.substring(slash + 1);
      if (name.endsWith(".rba.bak") && n < 4) found[n++] = name;
    }
  }
  if (dir) dir.close();
  for (uint8_t i = 0; i < n; ++i) {
    const String bak = String(RB61_DIR) + "/" + found[i];
    const String orig = bak.substring(0, bak.length() - 4);
    if (LittleFS.exists(orig)) { LittleFS.remove(bak); continue; }
    const bool back = LittleFS.rename(bak, orig);
    diagnosticEvent("ANIM", back ? "Alte Animation nach unterbrochenem Ersetzen wiederhergestellt" : "Sicherung einer Animation konnte nicht zurueckgeholt werden", !back);
  }
}
// Beim Start: unfertigen Upload wegraeumen.
static void rb61AnimBegin() {
  rb61Up.active = false;
  rb61EnsureDir();
  if (rb61Ready) rb61RecoverBackups();
  if (rb61Ready && LittleFS.exists(RB61_TMP)) {
    LittleFS.remove(RB61_TMP);
    diagnosticEvent("ANIM", "Unfertiger Animations-Upload entfernt");
  }
  if (rb61Ready && LittleFS.exists(RB61_TEMP_PATH)) LittleFS.remove(RB61_TEMP_PATH);   // Rest eines "nur senden"
}
// Nach jedem Sendeauftrag (auch bei Fehler): die voruebergehende Animation loeschen. Wird vom Sketch aufgerufen.
void rb61DropTemp() {
  if (rb61PendingAnimId != RB61_TEMP_ID) return;
  rb61PendingAnimId = 0;
  if (libraryMounted && LittleFS.exists(RB61_TEMP_PATH)) LittleFS.remove(RB61_TEMP_PATH);
}

// Gueltige IDs sortiert in ids[] (hoechstens RB61_MAX_ANIMS), Rueckgabe: Anzahl. Optional hoechste ID.
static uint8_t rb61ListIds(uint16_t *ids, uint32_t *maxId) {
  uint8_t n = 0; uint32_t top = 0;
  File dir = LittleFS.open(RB61_DIR);
  if (dir && dir.isDirectory()) {
    for (File e = dir.openNextFile(); e; e = dir.openNextFile()) {
      String name = e.name();
      e.close();
      const int slash = name.lastIndexOf('/');
      if (slash >= 0) name = name.substring(slash + 1);
      if (name.length() < 6 || name[0] != 'a' || !name.endsWith(".rba")) continue;
      const String digits = name.substring(1, name.length() - 4);
      if (!digits.length() || digits.length() > 4) continue;
      bool ok = true;
      for (size_t i = 0; i < digits.length(); ++i) if (digits[i] < '0' || digits[i] > '9') ok = false;
      if (!ok) continue;
      const uint32_t id = (uint32_t)digits.toInt();
      if (!id) continue;
      if (id > top) top = id;
      if (n < RB61_MAX_ANIMS) ids[n++] = (uint16_t)id;
    }
  }
  for (uint8_t i = 1; i < n; ++i) {   // Einfuegesortierung
    const uint16_t x = ids[i]; int8_t j = (int8_t)i - 1;
    while (j >= 0 && ids[j] > x) { ids[j + 1] = ids[j]; --j; }
    ids[j + 1] = x;
  }
  if (maxId) *maxId = top;
  return n;
}

// Kuerzt auf 40 Byte, ohne ein UTF-8-Zeichen zu zerschneiden; entfernt Steuerzeichen und Rand-Leerzeichen.
static uint8_t rb61CleanName(const String &in, char *out /* >= 41 */) {
  String t;
  for (size_t i = 0; i < in.length(); ++i) { const uint8_t c = (uint8_t)in[i]; if (c >= 32 && c != 127) t += (char)c; }
  t.trim();
  size_t n = t.length();
  if (n > 40) { n = 40; while (n > 0 && (((uint8_t)t[n]) & 0xC0) == 0x80) --n; }
  while (n > 0 && t[n - 1] == ' ') --n;
  memcpy(out, t.c_str(), n);
  out[n] = 0;
  return (uint8_t)n;
}
static bool rb61FreeFor(uint32_t bytes) {
  const size_t total = LittleFS.totalBytes(), used = LittleFS.usedBytes();
  return total > used && (total - used) > (size_t)bytes + RB61_FREE_RESERVE;
}

static void rb61Reply(bool ok, const String &message, int status, uint32_t id = 0) {
  web.sendHeader("Cache-Control", "no-store");
  httpSetStatus((uint16_t)status);
  String body = String("{\"ok\":") + (ok ? "true" : "false") + ",\"id\":" + String((unsigned long)id) +
                ",\"message\":" + jsonQuote(message) + "}";
  web.send(status, "application/json; charset=utf-8", body);
}
static bool rb61Usable() {
  if (!libraryMounted) { rb61Reply(false, "Dateisystem nicht eingerichtet. Zuerst in der Bibliothek einrichten.", 503); return false; }
  if (!rb61Ready) rb61EnsureDir();
  if (!rb61Ready) { rb61Reply(false, "Animationsordner konnte nicht angelegt werden.", 500); return false; }
  return true;
}
static uint32_t rb61RequestId() {
  if (!web.hasArg("id")) return 0;
  const String v = web.arg("id");
  if (!v.length() || v.length() > 4) return 0;
  for (size_t i = 0; i < v.length(); ++i) if (v[i] < '0' || v[i] > '9') return 0;
  return (uint32_t)v.toInt();
}
static bool rb61Busy() {   // BLE-Auftrag laeuft oder wartet: keine Dateien aendern, die gesendet werden koennten
  return currentState == "queued" || currentState == "sending";
}

// ---- Routen --------------------------------------------------------------------------------------
// GET /api/anim/list
void onAnimList() {
  if (!libraryMounted) {
    web.sendHeader("Cache-Control", "no-store");
    web.send(200, "application/json; charset=utf-8", "{\"ok\":true,\"ready\":false,\"items\":[]}");
    return;
  }
  if (!rb61Ready) rb61EnsureDir();
  uint16_t ids[RB61_MAX_ANIMS];
  const uint8_t n = rb61Ready ? rb61ListIds(ids, nullptr) : 0;
  const size_t total = LittleFS.totalBytes(), used = LittleFS.usedBytes();
  libraryStartJson();
  JsonChunkWriter w;
  w.add(String("{\"ok\":true,\"ready\":") + (rb61Ready ? "true" : "false") + ",\"max\":" + String(ANIM_MAX_FRAMES) +
        ",\"limit\":" + String(RB61_MAX_ANIMS) + ",\"free\":" + String((unsigned long)(total > used ? total - used : 0)) +
        ",\"total\":" + String((unsigned long)total) + ",\"items\":[");
  bool first = true;
  static Rb61AnimHeader h;
  char b64[(ANIM_FRAME_BYTES * 4 + 2) / 3 + 2];
  for (uint8_t i = 0; i < n; ++i) {
    File f = LittleFS.open(rb61Path(ids[i]), "r");
    const size_t fileSize = f ? f.size() : 0;
    const bool ok = rb61ReadHeader(f, h);
    if (f) f.close();
    if (!ok) {   // beschaedigte Datei: nicht verstecken, sondern als "broken" melden, damit man sie loeschen kann
      w.add(first ? "{" : ",{"); first = false;
      w.add(String("\"id\":") + String((unsigned)ids[i]) + ",\"name\":\"\",\"broken\":true,\"frames\":0,\"ms\":0,\"bytes\":" +
            String((unsigned long)fileSize) + ",\"thumb\":\"\"}");
      continue;
    }
    rb61B64Encode(h.thumb, ANIM_FRAME_BYTES, b64);
    w.add(first ? "{" : ",{"); first = false;
    w.add(String("\"id\":") + String((unsigned)ids[i]) + ",\"name\":" + jsonQuote(String(h.name)) +
          ",\"frames\":" + String((unsigned)h.frames) + ",\"ms\":" + String((unsigned)h.ms) +
          ",\"broken\":false,\"bytes\":" + String((unsigned long)(h.payload + RB61_META_BYTES)) + ",\"thumb\":\"");
    w.add(b64);
    w.add("\"}");
  }
  w.add("]}");
  w.finish();
}

// GET /api/anim/get?id=N  ->  {"ok":true,"id":..,"name":..,"frames":..,"ms":..,"data":"<Nutzlast, Base64-URL>"}
void onAnimGet() {
  if (!rb61Usable()) return;
  const uint32_t id = rb61RequestId();
  File f = LittleFS.open(rb61Path(id), "r");
  static Rb61AnimHeader h;
  if (!id || !rb61ReadHeader(f, h)) { f.close(); rb61Reply(false, "Animation nicht gefunden oder beschaedigt.", 404); return; }
  libraryStartJson();
  JsonChunkWriter w;
  w.add(String("{\"ok\":true,\"id\":") + String((unsigned)id) + ",\"name\":" + jsonQuote(String(h.name)) +
        ",\"frames\":" + String((unsigned)h.frames) + ",\"ms\":" + String((unsigned)h.ms) + ",\"data\":\"");
  w.flush();
  uint8_t raw[RB61_GET_RAW];
  char text[RB61_GET_RAW / 3 * 4 + 4];
  f.seek(RB61_META_BYTES);
  uint32_t left = h.payload;
  while (left) {
    const size_t take = left < RB61_GET_RAW ? left : RB61_GET_RAW;
    if (f.read(raw, take) != (int)take) break;
    rb61B64Encode(raw, take, text);
    web.sendContent(text);
    left -= take;
    rbPhase("anim_get");
  }
  f.close();
  w.add("\"}");
  w.finish();
}

// POST /api/anim/put  (Upload in Stuecken)
//   Stueck 0:  off=0 name frames ms total crc thumb data (thumb = erstes Bild, 384 Byte, Base64-URL;
//              crc = CRC-32 der gesamten Nutzlast, dezimal; optional temp=1 oder replace=<ID>)
//   weitere:   off=<bisher geschrieben> data
//   Das letzte Stueck (off+len == total) schliesst ab und antwortet mit der neuen ID.
//   abort=1 verwirft einen angefangenen Upload.
void onAnimPut() {
  if (!rb61Usable()) return;
  if (web.arg("abort") == "1") {
    rb61Up.active = false;
    if (LittleFS.exists(RB61_TMP)) LittleFS.remove(RB61_TMP);
    rb61Reply(true, "Upload verworfen.", 200);
    return;
  }
  if (rb61Busy()) { rb61Reply(false, "Der Controller ist beschaeftigt.", 409); return; }
  const uint32_t off = (uint32_t)web.arg("off").toInt();
  const String data = web.arg("data");
  if (!data.length() || data.length() > (size_t)RB61_CHUNK_RAW / 3 * 4 + 8) { rb61Reply(false, "Datenstueck fehlt oder ist zu gross.", 400); return; }
  uint8_t raw[RB61_CHUNK_RAW + 8];
  const int got = rb61B64Decode(data.c_str(), data.length(), raw);
  if (got <= 0 || got > RB61_CHUNK_RAW) { rb61Reply(false, "Datenstueck ist kein gueltiges Base64.", 400); return; }

  if (off == 0) {
    // Neuer Upload: alles pruefen, bevor die erste Schreibaktion passiert.
    const int frames = web.arg("frames").toInt(), ms = web.arg("ms").toInt();
    const uint32_t total = (uint32_t)web.arg("total").toInt();
    if (frames < 2 || frames > ANIM_MAX_FRAMES) { rb61Reply(false, "Frameanzahl muss zwischen 2 und 79 liegen.", 400); return; }
    if (ms < 1 || ms > 2000) { rb61Reply(false, "Zeit je Frame muss zwischen 1 und 2000 ms liegen.", 400); return; }
    if (total != (uint32_t)ANIM_HEADER_BYTES + (uint32_t)frames * ANIM_FRAME_BYTES) { rb61Reply(false, "Gesamtgroesse passt nicht zur Frameanzahl.", 400); return; }
    if ((uint32_t)got > total) { rb61Reply(false, "Datenstueck groesser als die Animation.", 400); return; }
    static Rb61AnimHeader h;
    memset(&h, 0, sizeof(h));
    memcpy(h.magic, "RBA1", 4);
    h.frames = (uint16_t)frames; h.ms = (uint16_t)ms; h.payload = total;
    h.nameLen = rb61CleanName(web.arg("name"), h.name);
    if (!h.nameLen) { rb61Reply(false, "Der Name darf nicht leer sein.", 400); return; }
    const String thumb = web.arg("thumb");
    uint8_t tb[ANIM_FRAME_BYTES + 4];
    if (thumb.length() > (size_t)ANIM_FRAME_BYTES / 3 * 4 + 4 || rb61B64Decode(thumb.c_str(), thumb.length(), tb) != (int)ANIM_FRAME_BYTES) {
      rb61Reply(false, "Vorschaubild fehlt oder ist ungueltig.", 400); return;
    }
    memcpy(h.thumb, tb, ANIM_FRAME_BYTES);
    const String crcText = web.arg("crc");   // nur Ziffern, hoechstens 0xFFFFFFFF (strtoul wuerde "-5" und Ueberlauf durchlassen)
    uint64_t crcVal = 0;
    bool crcOk = crcText.length() >= 1 && crcText.length() <= 10;
    for (size_t i = 0; crcOk && i < crcText.length(); ++i) {
      if (crcText[i] < '0' || crcText[i] > '9') crcOk = false;
      else crcVal = crcVal * 10 + (uint64_t)(crcText[i] - '0');
    }
    if (!crcOk || crcVal > 0xFFFFFFFFULL) { rb61Reply(false, "Pruefsumme fehlt oder ist ungueltig.", 400); return; }
    h.flags = RB61_FLAG_CRC; rb61SetHeaderCrc(h, (uint32_t)crcVal);
    uint16_t ids[RB61_MAX_ANIMS];
    const bool temp = web.arg("temp") == "1";
    const long replaceArg = web.arg("replace").toInt();     // Bearbeiten: vorhandene Animation ersetzen
    if (replaceArg != 0 && (temp || replaceArg < 1 || replaceArg > 9999)) { rb61Reply(false, "Ersetzen und voruebergehend schliessen sich aus.", 400); return; }
    if (replaceArg != 0 && !rb61AnimExists((uint32_t)replaceArg)) { rb61Reply(false, "Die zu ersetzende Animation fehlt oder ist beschaedigt.", 404); return; }
    if (!temp && replaceArg == 0 && rb61ListIds(ids, nullptr) >= RB61_MAX_ANIMS) { rb61Reply(false, "Es sind schon 40 Animationen gespeichert. Bitte erst eine loeschen.", 409); return; }
    // Reste frueherer Versuche zuerst entfernen, sonst zaehlen sie bei der Platzpruefung mit (bis ~30 KB) und
    // ein erneuter Versuch scheitert zu Unrecht (Befund N1).
    rb61Up.active = false;
    if (LittleFS.exists(RB61_TMP)) LittleFS.remove(RB61_TMP);
    if (temp && LittleFS.exists(RB61_TEMP_PATH)) LittleFS.remove(RB61_TEMP_PATH);
    if (!rb61FreeFor(total + RB61_META_BYTES)) { rb61Reply(false, "Nicht genug freier Speicher im Dateisystem.", 507); return; }
    File f = LittleFS.open(RB61_TMP, "w");
    if (!f) { rb61Reply(false, "Zwischendatei konnte nicht angelegt werden.", 500); return; }
    const bool ok = f.write(reinterpret_cast<const uint8_t *>(&h), sizeof(h)) == sizeof(h) && f.write(raw, got) == (size_t)got;
    f.close();
    if (!ok) { LittleFS.remove(RB61_TMP); rb61Reply(false, "Schreiben ins Dateisystem fehlgeschlagen.", 500); return; }
    rb61Up.active = true; rb61Up.temp = temp; rb61Up.replaceId = (uint16_t)replaceArg; rb61Up.frames = (uint16_t)frames; rb61Up.ms = (uint16_t)ms;
    rb61Up.total = total; rb61Up.got = (uint32_t)got; rb61Up.crc = (uint32_t)crcVal;
    strncpy(rb61Up.name, h.name, sizeof(rb61Up.name) - 1); rb61Up.name[sizeof(rb61Up.name) - 1] = 0;
  } else {
    if (!rb61Up.active || off != rb61Up.got) { rb61Reply(false, "Upload-Reihenfolge stimmt nicht. Bitte neu beginnen.", 409); return; }
    if (rb61Up.got + (uint32_t)got > rb61Up.total) { rb61Reply(false, "Mehr Daten als angekuendigt.", 400); return; }
    File f = LittleFS.open(RB61_TMP, "a");
    const bool ok = f && f.write(raw, got) == (size_t)got;
    if (f) f.close();
    if (!ok) { rb61Up.active = false; LittleFS.remove(RB61_TMP); rb61Reply(false, "Schreiben ins Dateisystem fehlgeschlagen.", 500); return; }
    rb61Up.got += (uint32_t)got;
  }
  rbPhase("anim_put");
  if (rb61Up.got < rb61Up.total) { rb61Reply(true, "Stueck gespeichert.", 200); return; }

  // Abschluss: Nutzlastkopf pruefen, dann rename.
  File f = LittleFS.open(RB61_TMP, "r");
  uint8_t head[ANIM_HEADER_BYTES];
  bool ok = f && f.size() == (size_t)RB61_META_BYTES + rb61Up.total && f.seek(RB61_META_BYTES) && f.read(head, sizeof(head)) == (int)sizeof(head);
  if (f) f.close();
  for (uint8_t i = 0; ok && i < 24; ++i) if (head[i] != 0) ok = false;
  if (ok && (head[24] != rb61Up.frames || ((uint16_t)head[25] << 8 | head[26]) != rb61Up.ms)) ok = false;
  if (!ok) { rb61Up.active = false; LittleFS.remove(RB61_TMP); rb61Reply(false, "Animationsdaten sind unvollstaendig oder passen nicht zum Kopf.", 400); return; }
  // Pruefsumme: die Datei wird noch einmal ganz gelesen. Das faengt Uebertragungs- UND Schreibfehler.
  uint32_t crcFile = 0;
  File fc = LittleFS.open(RB61_TMP, "r");
  const bool crcRead = rb61FileCrc(fc, rb61Up.total, crcFile);
  if (fc) fc.close();
  if (!crcRead || crcFile != rb61Up.crc) {
    rb61Up.active = false; LittleFS.remove(RB61_TMP);
    diagnosticEvent("ANIM", crcRead ? "Upload verworfen: Pruefsumme stimmt nicht" : "Upload verworfen: Datei beim Pruefen nicht lesbar", true);
    rb61Reply(false, "Pruefsumme stimmt nicht: Die Daten sind unterwegs oder beim Schreiben beschaedigt worden. Bitte erneut speichern.", 422);
    return;
  }
  if (rb61Up.temp) {   // "nur senden": feste Datei, kein Listeneintrag; eine alte voruebergehende Datei wird ersetzt
    if (LittleFS.exists(RB61_TEMP_PATH)) LittleFS.remove(RB61_TEMP_PATH);
    const bool moved = LittleFS.rename(RB61_TMP, RB61_TEMP_PATH);
    rb61Up.active = false;
    if (!moved) { LittleFS.remove(RB61_TMP); rb61Reply(false, "Animation konnte nicht bereitgestellt werden.", 500); return; }
    diagnosticEvent("ANIM", String("Animation voruebergehend bereitgestellt (nur senden), ") + rb61Up.frames + " Frames");
    rb61Reply(true, "Animation bereit zum Senden.", 200, RB61_TEMP_ID);
    return;
  }
  if (rb61Up.replaceId) {   // Bearbeiten: gleiche ID, Kurzwahlen bleiben gueltig
    const uint32_t rid = rb61Up.replaceId;
    const String target = rb61Path(rid), bak = target + ".bak";
    // 1. Versuch: rename ersetzt das Ziel in einem Schritt. 2. Versuch: die alte Datei vorher als .bak zur Seite legen
    // (nie loeschen, bevor die neue steht); schlaegt das Umbenennen dann fehl, kommt die alte zurueck (Befund M3).
    bool moved = LittleFS.rename(RB61_TMP, target);
    if (!moved) {
      LittleFS.remove(bak);
      if (LittleFS.rename(target, bak)) {
        moved = LittleFS.rename(RB61_TMP, target);
        if (moved) LittleFS.remove(bak);
        else if (!LittleFS.rename(bak, target)) diagnosticEvent("ANIM", "Ersetzen fehlgeschlagen und die alte Animation liegt noch als .bak da (wird beim Start zurueckgeholt)", true);
      }
    }
    rb61Up.active = false; rb61Up.replaceId = 0;
    if (!moved) { LittleFS.remove(RB61_TMP); rb61Reply(false, "Animation konnte nicht ersetzt werden. Die alte bleibt erhalten.", 500); return; }
    diagnosticEvent("ANIM", String("Animation ersetzt, ID=") + rid + ", " + rb61Up.frames + " Frames, " + (unsigned long)(rb61Up.total + RB61_META_BYTES) + " Byte");
    rb61Reply(true, "Animation aktualisiert.", 200, rid);
    return;
  }
  uint16_t ids[RB61_MAX_ANIMS]; uint32_t top = 0;
  rb61ListIds(ids, &top);
  const uint32_t id = top + 1;
  if (id > 9999) { rb61Up.active = false; LittleFS.remove(RB61_TMP); rb61Reply(false, "Keine freie Animations-ID mehr.", 507); return; }
  if (!LittleFS.rename(RB61_TMP, rb61Path(id))) { rb61Up.active = false; LittleFS.remove(RB61_TMP); rb61Reply(false, "Animation konnte nicht abgeschlossen werden.", 500); return; }
  rb61Up.active = false;
  diagnosticEvent("ANIM", String("Animation gespeichert, ID=") + id + ", " + rb61Up.frames + " Frames, " + (unsigned long)(rb61Up.total + RB61_META_BYTES) + " Byte");
  rb61Reply(true, "Animation gespeichert.", 200, id);
}

// POST /api/anim/rename  id name
void onAnimRename() {
  if (!rb61Usable()) return;
  if (rb61Busy()) { rb61Reply(false, "Der Controller ist beschaeftigt.", 409); return; }
  const uint32_t id = rb61RequestId();
  char name[41];
  const uint8_t n = rb61CleanName(web.arg("name"), name);
  if (!id || !n) { rb61Reply(false, "Name oder Animation fehlt.", 400); return; }
  File f = LittleFS.open(rb61Path(id), "r+");
  static Rb61AnimHeader h;
  if (!rb61ReadHeader(f, h)) { if (f) f.close(); rb61Reply(false, "Animation nicht gefunden oder beschaedigt.", 404); return; }
  memset(h.name, 0, sizeof(h.name)); memcpy(h.name, name, n); h.nameLen = n;
  f.seek(0);
  const bool ok = f.write(reinterpret_cast<const uint8_t *>(&h), 64) == 64;   // nur die ersten 64 Byte (Kopf ohne Vorschau)
  f.close();
  rb61Reply(ok, ok ? "Umbenannt." : "Umbenennen fehlgeschlagen.", ok ? 200 : 500, id);
}

// Leert alle Kurzwahlen vom Typ 4 (Animation), die auf diese ID zeigen. Wie bei Bibliothekseintraegen (Typ 1):
// zuerst die Kurzwahl-Datei sicher schreiben, erst danach wird die Animation geloescht. Sonst koennte eine spaeter
// neu angelegte Animation mit derselben ID (neue ID = hoechste + 1) unbemerkt hinter der alten Kurzwahl landen.
// true = nichts zu tun oder sicher gespeichert.
static bool rb61DetachShortcuts(uint32_t id) {
  RB42Shortcuts next = rb42Shortcuts;
  bool changed = false;
  for (uint8_t i = 0; i < RB42_SHORTCUT_COUNT; ++i) {
    if (next.types[i] == 4 && next.ids[i] == id) {
      next.types[i] = 0; next.ids[i] = 0; next.labels[i][0] = 0;
      changed = true;
    }
  }
  if (!changed) return true;
  if (rb42ShortcutsDamaged || !rb42Write(String(RB42_SHORTCUT_FILE), next)) return false;
  rb42Shortcuts = next;
  return true;
}

// POST /api/anim/delete  id
void onAnimDelete() {
  if (!rb61Usable()) return;
  if (rb61Busy()) { rb61Reply(false, "Der Controller ist beschaeftigt.", 409); return; }
  const uint32_t id = rb61RequestId();
  const String path = rb61Path(id);
  if (!id || !LittleFS.exists(path)) { rb61Reply(false, "Animation nicht gefunden.", 404); return; }
  if (!rb61DetachShortcuts(id)) { rb61Reply(false, "Kurzwahlen konnten nicht sicher aktualisiert werden. Nichts geloescht.", 500); return; }
  const bool ok = LittleFS.remove(path);
  rb61Reply(ok, ok ? "Animation geloescht." : "Loeschen fehlgeschlagen.", ok ? 200 : 500, id);
}

// Prueft, ob es die Animation gibt (id 1..9999, Kopf und Groesse gueltig). Auch fuer Kurzwahlen (Typ 4).
bool rb61AnimExists(uint32_t id) {
  if (!id || id > 9999 || !libraryMounted) return false;
  File f = LittleFS.open(rb61Path(id), "r");
  static Rb61AnimHeader h;
  const bool ok = rb61ReadHeader(f, h);
  if (f) f.close();
  return ok;
}
// Reiht den Sendeauftrag fuer eine Datei ein (ohne reserveJob: das macht der Aufrufer). true = eingereiht.
// id == RB61_TEMP_ID: voruebergehende Animation ("nur senden"), wird nach dem Auftrag geloescht.
bool rb61QueueAnim(uint32_t id) {
  File f = LittleFS.open(rb61Path(id), "r");
  static Rb61AnimHeader h;
  const bool ok = id && rb61ReadHeader(f, h);
  if (f) f.close();
  if (!ok) return false;
  autoProgram = false; rb42ProgramPlaying = false;
  rb61PendingAnimId = (uint16_t)id;
  pendingMode = 0x00;   // der Anzeigemodus spielt fuer Animationen keine Rolle (Hardwaretest 0.6.0.2)
  nextJob = ANIM;
  currentState = "queued";
  currentMessage = "Animation angenommen. Bluetooth-Uebertragung startet …";
  diagnosticEvent("ANIM", String("Datei ") + (id == RB61_TEMP_ID ? String("(nur senden)") : String(id)) + " \"" + h.name + "\", " + h.frames + " Frames, " + h.ms + " ms");
  return true;
}

// POST /api/anim/send  id | temp=1       (Auftrag ANIM mit Datei als Quelle)
void onAnimSend() {
  if (!rb61Usable() || !reserveJob()) return;
  const bool temp = web.arg("temp") == "1";
  const uint32_t id = temp ? (uint32_t)RB61_TEMP_ID : rb61RequestId();
  if (!rb61QueueAnim(id)) { rb61Reply(false, "Animation nicht gefunden oder beschaedigt.", 404); return; }
  rb61Reply(true, currentMessage, 202, id);
}

// Fuer den Sendeauftrag: Datei oeffnen, Kopf pruefen, Zustand setzen. Rueckgabe = Nutzlaenge (0 = Fehler).
static size_t rb61PrepareAnimFile(uint16_t id) {
  if (rb61AnimFile) rb61AnimFile.close();
  rb61AnimFile = LittleFS.open(rb61Path(id), "r");
  static Rb61AnimHeader h;
  if (!rb61ReadHeader(rb61AnimFile, h)) { if (rb61AnimFile) rb61AnimFile.close(); return 0; }
  if (h.flags & RB61_FLAG_CRC) {   // seit 0.6.1.3: Nutzlast gegen die gespeicherte Pruefsumme pruefen, bevor Bluetooth beteiligt ist
    uint32_t crc = 0;
    if (!rb61FileCrc(rb61AnimFile, h.payload, crc) || crc != rb61HeaderCrc(h)) {
      diagnosticEvent("ANIM", "Senden abgebrochen: Pruefsumme der Animationsdatei stimmt nicht", true);
      rb61AnimFile.close(); return 0;
    }
  }
  rb61ReadFailed = false;
  rb60AnimActive = true;
  rb60AnimPattern = ANIM_FILE; rb60AnimFrames = (uint8_t)h.frames; rb60AnimMs = h.ms;
  strncpy(rb61AnimName, h.name, sizeof(rb61AnimName) - 1); rb61AnimName[sizeof(rb61AnimName) - 1] = 0;
  return h.payload;
}
