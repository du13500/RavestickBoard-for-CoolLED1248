// RavestickBoard v0.4.2.1-dev: one library shared over the ESP32 access point.
// LittleFS lives in the sketch's filesystem partition, NEVER in the existing
// Preferences namespace holding the 12 block pages. No auto-format on failure.
#pragma once
#include <LittleFS.h>

static constexpr uint16_t LIB_SCHEMA = 1;
static constexpr uint32_t LIB_MAGIC = 0x52424C31UL; // RBL1
static constexpr uint32_t LIB_META_MAGIC = 0x52424D31UL; // RBM1
static constexpr uint8_t LIB_LIMIT = 192;
static constexpr uint16_t LIB_TEXT_SIZE = 1600;
static constexpr uint8_t LIB_SHORTCUTS = 6;

// Fixed-size on-flash storage, bounded single scratch record in RAM. This is
// deliberate: the library is NOT read into a vector of Strings at boot.
struct LibraryRecord {
  uint32_t magic;
  uint32_t id;
  uint16_t schema;
  uint8_t layout;
  uint8_t color1;
  uint8_t color2;
  uint8_t mode;     // 1=static, 2=scroll; saved per text
  uint8_t favorite;
  char name[48];
  char text[LIB_TEXT_SIZE];
  char line2[LIB_TEXT_SIZE];
  uint32_t checksum;
};
struct LibraryMeta {
  uint32_t magic;
  uint16_t schema;
  uint16_t count;
  uint32_t sequence;
  uint32_t nextId;
  uint32_t ids[LIB_LIMIT];       // library order
  uint32_t shortcuts[LIB_SHORTCUTS];
  uint32_t checksum;
};

// Existing v0.4.1 RBM1 journal remains compatible after increasing index capacity.
struct LibraryMetaLegacy {
  uint32_t magic;
  uint16_t schema, count;
  uint32_t sequence, nextId;
  uint32_t ids[64];
  uint32_t shortcuts[LIB_SHORTCUTS];
  uint32_t checksum;
};
static LibraryRecord libraryItem = {};
static LibraryMeta libraryMeta = {};
bool libraryReady = false;
static bool libraryMounted = false;
static bool libraryMetadataDamaged = false;

uint32_t libraryChecksum(const uint8_t *p, size_t count) {
  // FNV-1a (checks truncated/corrupt file; not a cryptographic authenticator).
  uint32_t x = 2166136261UL;
  for (size_t i=0; i<count; ++i) { x ^= p[i]; x *= 16777619UL; }
  return x;
}
template <typename T> uint32_t libraryDigest(const T &obj) {
  return libraryChecksum(reinterpret_cast<const uint8_t *>(&obj), sizeof(T)-sizeof(uint32_t));
}
String libraryPath(uint32_t id, const char *suffix=".bin") {
  return String("/rb-lib/t") + String(id) + suffix;
}
int libraryIndex(uint32_t id) {
  for (uint8_t n=0; n<libraryMeta.count; ++n) if (libraryMeta.ids[n] == id) return n;
  return -1;
}
bool libraryMetaValid(const LibraryMeta &m) {
  if (m.magic != LIB_META_MAGIC || m.schema != LIB_SCHEMA || m.count > LIB_LIMIT ||
      m.nextId == 0 || m.checksum != libraryDigest(m)) return false;
  for (uint8_t a=0;a<m.count;++a) {
    if (!m.ids[a] || m.ids[a] >= m.nextId) return false;
    for (uint8_t b=0;b<a;++b) if (m.ids[a] == m.ids[b]) return false;
  }
  return true;
}
bool libraryReadMeta(const char *path, LibraryMeta &candidate) {
  File f=LittleFS.open(path,"r");
  if (!f) return false;
  const size_t bytes=f.size();
  bool ok=false;
  if(bytes==sizeof(candidate)) {
    ok=f.readBytes(reinterpret_cast<char *>(&candidate),sizeof(candidate))==sizeof(candidate) && libraryMetaValid(candidate);
  } else if(bytes==sizeof(LibraryMetaLegacy)) {
    LibraryMetaLegacy prior={};
    ok=f.readBytes(reinterpret_cast<char *>(&prior),sizeof(prior))==sizeof(prior) &&
       prior.magic==LIB_META_MAGIC && prior.schema==1 && prior.count<=64 &&
       prior.nextId>0 && prior.checksum==libraryDigest(prior);
    if(ok) {
      memset(&candidate,0,sizeof(candidate));
      candidate.magic=prior.magic;candidate.schema=LIB_SCHEMA;
      candidate.count=prior.count;candidate.sequence=prior.sequence;candidate.nextId=prior.nextId;
      memcpy(candidate.ids,prior.ids,sizeof(prior.ids));
      memcpy(candidate.shortcuts,prior.shortcuts,sizeof(prior.shortcuts));
      candidate.checksum=libraryDigest(candidate);
      ok=libraryMetaValid(candidate);
    }
  }
  f.close();return ok;
}
bool libraryWriteMeta(const LibraryMeta &newMeta) {
  static LibraryMeta candidate;
  candidate=newMeta;
  candidate.sequence=libraryMeta.sequence+1;
  candidate.magic=LIB_META_MAGIC; candidate.schema=LIB_SCHEMA;
  candidate.checksum=libraryDigest(candidate);
  // Two alternating journal slots. A power failure cannot invalidate both.
  const char *dst=(candidate.sequence & 1)?"/rb-lib/meta-a.bin":"/rb-lib/meta-b.bin";
  File f=LittleFS.open(dst,"w");
  if (!f) return false;
  bool ok=f.write(reinterpret_cast<const uint8_t *>(&candidate),sizeof(candidate))==sizeof(candidate);
  f.flush(); f.close();
  static LibraryMeta verify;
  if (!ok || !libraryReadMeta(dst,verify) || verify.sequence!=candidate.sequence) return false;
  libraryMeta=candidate; return true;
}
bool libraryLoad(uint32_t id) {
  if (!libraryReady || libraryIndex(id)<0) return false;
  File f=LittleFS.open(libraryPath(id),"r");
  if (!f) return false;
  const bool ok=f.size()==sizeof(libraryItem) && f.readBytes(reinterpret_cast<char *>(&libraryItem),sizeof(libraryItem))==sizeof(libraryItem);
  f.close();
  return ok && libraryItem.magic==LIB_MAGIC && libraryItem.schema==LIB_SCHEMA && libraryItem.id==id &&
    libraryItem.name[sizeof(libraryItem.name)-1]=='\0' &&
    libraryItem.text[LIB_TEXT_SIZE-1]=='\0' && libraryItem.line2[LIB_TEXT_SIZE-1]=='\0' &&
    libraryItem.checksum==libraryDigest(libraryItem);
}
bool libraryWriteRecord(const LibraryRecord &item) {
  const String path=libraryPath(item.id);
  const String temp=libraryPath(item.id,".tmp");
  const String bak=libraryPath(item.id,".bak");
  LittleFS.remove(temp);
  File f=LittleFS.open(temp,"w");
  if (!f) return false;
  const bool written=f.write(reinterpret_cast<const uint8_t *>(&item),sizeof(item))==sizeof(item);
  f.flush(); f.close();
  if (!written) { LittleFS.remove(temp); return false; }
  // Verify what flash actually contains before replacing the existing entry.
  File check=LittleFS.open(temp,"r");
  bool verify=check && check.size()==sizeof(item);
  uint8_t buffer[128];
  const uint8_t *source=reinterpret_cast<const uint8_t *>(&item);
  for (size_t off=0; verify && off<sizeof(item); off+=sizeof(buffer)) {
    const size_t len=min(sizeof(buffer),sizeof(item)-off);
    verify=check.read(buffer,len)==len && memcmp(buffer,source+off,len)==0;
  }
  if (check) check.close();
  if (!verify) { LittleFS.remove(temp); return false; }
  // Existing file is held as rollback copy until replacement is committed.
  LittleFS.remove(bak);
  bool existing=LittleFS.exists(path);
  if (existing && !LittleFS.rename(path,bak)) { LittleFS.remove(temp); return false; }
  if (!LittleFS.rename(temp,path)) {
    if (existing) LittleFS.rename(bak,path);
    LittleFS.remove(temp); return false;
  }
  // A crash here leaves .bak; read prioritizes the new valid .bin.
  LittleFS.remove(bak);
  return true;
}
void libraryBegin() {
  libraryReady=false; libraryMetadataDamaged=false;
  libraryMounted=LittleFS.begin(false); // NEVER auto-format a damaged filesystem
  if (!libraryMounted) {
    diagnosticEvent("LIB", "LittleFS noch nicht eingerichtet oder nicht einbindbar");
    return;
  }
  if (!LittleFS.exists("/rb-lib")) LittleFS.mkdir("/rb-lib");
  static LibraryMeta a,b;
  memset(&a,0,sizeof(a));memset(&b,0,sizeof(b));
  bool aa=libraryReadMeta("/rb-lib/meta-a.bin",a);
  bool bb=libraryReadMeta("/rb-lib/meta-b.bin",b);
  if (aa || bb) libraryMeta=(!bb || (aa && (int32_t)(a.sequence-b.sequence)>0))?a:b;
  else {
    if (LittleFS.exists("/rb-lib/meta-a.bin") || LittleFS.exists("/rb-lib/meta-b.bin")) {
      libraryMetadataDamaged=true;
      diagnosticEvent("LIB", "Bibliotheksmetadaten unlesbar, kein automatisches Loeschen",true);
      return;
    }
    memset(&libraryMeta,0,sizeof(libraryMeta));
    libraryMeta.magic=LIB_META_MAGIC; libraryMeta.schema=LIB_SCHEMA; libraryMeta.nextId=1;
    if (!libraryWriteMeta(libraryMeta)) { diagnosticEvent("LIB","Erstinitialisierung fehlgeschlagen",true); return; }
  }
  // Recover an interrupted replacement before accepting requests.
  for (uint8_t i=0; i<libraryMeta.count; ++i) {
    const uint32_t id=libraryMeta.ids[i];
    const String path=libraryPath(id),bak=libraryPath(id,".bak");
    if (LittleFS.exists(bak)) {
      if (!LittleFS.exists(path)) LittleFS.rename(bak,path);
      else LittleFS.remove(bak);
    }
  }
  libraryReady=true;
  diagnosticEvent("LIB",String("LittleFS bereit, Texte=")+libraryMeta.count);
}
void onLibraryInit() {
  if (libraryReady) { respond(true,"Bibliothek ist bereits eingerichtet."); return; }
  if (libraryMetadataDamaged || libraryMounted) {
    respond(false,"Dateisystem vorhanden, aber Metadaten fehlerhaft. Nicht formatieren! Backup und Diagnose pruefen.",409); return;
  }
  if (web.arg("confirm")!="INIT_LIBRARY_FS") { respond(false,"Ausdrueckliche Ersteinrichtung erforderlich.",400); return; }
  // Only the filesystem partition is formatted, never NVS/program pages.
  if (!LittleFS.format()) { respond(false,"LittleFS-Formatierung fehlgeschlagen.",500); return; }
  libraryBegin();
  respond(libraryReady,libraryReady?"Bibliothek eingerichtet. NVS-Blockseiten unveraendert.":"Bibliothek konnte nicht initialisiert werden.",libraryReady?200:500);
}
bool libraryAvailable() {
  if (!libraryReady) { respond(false,"Bibliothek nicht eingerichtet. Keine Daten wurden geloescht.",503); return false; }
  return true;
}
uint32_t libraryRequestId() {
  if (!web.hasArg("id")) return 0;
  const String value=web.arg("id");
  if (!value.length() || value.length()>10) return 0;
  for (size_t i=0;i<value.length();++i) if (value[i]<'0' || value[i]>'9') return 0;
  const unsigned long long parsed=strtoull(value.c_str(),nullptr,10);
  return parsed>0xFFFFFFFFULL?0:static_cast<uint32_t>(parsed);
}
void libraryStartJson() {
  web.sendHeader("Cache-Control","no-store");
  web.setContentLength(CONTENT_LENGTH_UNKNOWN);
  web.send(200,"application/json; charset=utf-8","");
}
void onLibraryList() {
  if (!libraryReady) {
    web.send(200,"application/json; charset=utf-8",String("{\"ok\":true,\"ready\":false,\"damaged\":")+(libraryMetadataDamaged?"true":"false")+",\"items\":[],\"shortcuts\":[0,0,0,0,0,0]}");
    return;
  }
  libraryStartJson();
  web.sendContent(String("{\"ok\":true,\"ready\":true,\"count\":")+libraryMeta.count+
    ",\"total\":"+LittleFS.totalBytes()+",\"used\":"+LittleFS.usedBytes()+",\"shortcuts\":[");
  for (uint8_t n=0;n<LIB_SHORTCUTS;++n) {
    if (n) web.sendContent(",");
    web.sendContent(String(libraryMeta.shortcuts[n]));
  }
  web.sendContent("],\"items\":[");
  for (uint8_t n=0;n<libraryMeta.count;++n) {
    const uint32_t id=libraryMeta.ids[n];
    const bool valid=libraryLoad(id);
    if (n) web.sendContent(",");
    if (!valid) { web.sendContent(String("{\"id\":")+id+",\"corrupt\":true}"); continue; }
    web.sendContent(String("{\"id\":")+id+",\"name\":"+jsonQuote(libraryItem.name)+
      ",\"layout\":"+libraryItem.layout+",\"color\":"+libraryItem.color1+
      ",\"mode\":"+libraryItem.mode+",\"kind\":\""+(libraryItem.layout==5?"block":"text")+
      "\",\"pages\":"+(libraryItem.layout==5?v042BlockPageCount(id):0)+
      ",\"favorite\":"+(libraryItem.favorite?"true":"false")+"}");
  }
  web.sendContent("]}"); web.sendContent("");
}
void onLibraryGet() {
  if (!libraryAvailable()) return;
  uint32_t id=libraryRequestId();
  if (!libraryLoad(id)) { respond(false,"Text nicht gefunden oder beschaedigt.",404); return; }
  libraryStartJson();
  web.sendContent(String("{\"ok\":true,\"schema\":1,\"id\":")+id+
    ",\"name\":"+jsonQuote(libraryItem.name)+",\"text\":"+jsonQuote(libraryItem.text)+
    ",\"line2\":"+jsonQuote(libraryItem.line2)+",\"layout\":"+libraryItem.layout+
    ",\"color1\":"+libraryItem.color1+",\"color2\":"+libraryItem.color2+
    ",\"mode\":"+libraryItem.mode+",\"kind\":\""+(libraryItem.layout==5?"block":"text")+
    "\",\"favorite\":"+(libraryItem.favorite?"true":"false")+"}");
  web.sendContent("");
}
void onLibrarySave() {
  if (!libraryAvailable()) return;
  const uint32_t id=web.hasArg("id")?libraryRequestId():0;
  const bool update=id!=0;
  if (web.hasArg("id") && !update) { respond(false,"ID ungueltig.",400); return; }
  if (update && !libraryLoad(id)) { respond(false,"Text nicht gefunden.",404); return; }
  if (update && libraryItem.layout==5) {respond(false,"Blockfolgen über Blockeditor bearbeiten.",409);return;}
  if (!update && (libraryMeta.count>=LIB_LIMIT || libraryMeta.nextId==0xFFFFFFFFUL ||
       LittleFS.totalBytes()-LittleFS.usedBytes()<65536UL)) {
    respond(false,"Bibliotheksindex voll; bitte Einträge sichern.",507); return;
  }
  const String name=web.arg("name"),text=web.arg("text"),line2=web.arg("line2");
  const int layout=web.arg("layout").toInt(),c1=web.arg("color1").toInt(),c2=web.arg("color2").toInt(),mode=web.arg("mode").toInt();
  const bool fav=web.arg("favorite")=="1";
  const TextMeasure m1=measureText(text,layout),m2=layout==1?measureText(line2,layout):TextMeasure{0,0};
  const uint32_t columns=max((uint32_t)SIGN_WIDTH,max(m1.pixels,m2.pixels));
  if (!name.length() || name.length()>=sizeof(libraryItem.name) ||
      text.length()>=LIB_TEXT_SIZE || line2.length()>=LIB_TEXT_SIZE ||
      (text.isEmpty() && (layout!=1 || line2.isEmpty())) || layout<0 || layout>4 ||
      c1<1 || c1>7 || c2<1 || c2>7 || (mode!=1 && mode!=2) ||
      IMAGE_HEADER_BYTES+columns*BYTES_PER_COLUMN>MAX_IMAGE_BYTES || (mode==1 && columns>SIGN_WIDTH)) {
    respond(false,"Ungueltiger Name/Text oder LED-Datenbudget ueberschritten.",400); return;
  }
  // If updating, prepare replacement. If creating, start clean.
  if (!update) memset(&libraryItem,0,sizeof(libraryItem));
  libraryItem.magic=LIB_MAGIC; libraryItem.schema=LIB_SCHEMA;
  libraryItem.id=update?id:libraryMeta.nextId;
  libraryItem.layout=layout; libraryItem.color1=c1; libraryItem.color2=c2; libraryItem.mode=mode; libraryItem.favorite=fav;
  snprintf(libraryItem.name,sizeof(libraryItem.name),"%s",name.c_str());
  snprintf(libraryItem.text,sizeof(libraryItem.text),"%s",text.c_str());
  snprintf(libraryItem.line2,sizeof(libraryItem.line2),"%s",line2.c_str());
  libraryItem.checksum=libraryDigest(libraryItem);
  if (!libraryWriteRecord(libraryItem)) { respond(false,"Flash-Speichern fehlgeschlagen.",500); return; }
  if (!update) {
    LibraryMeta next=libraryMeta;
    next.ids[next.count++]=libraryItem.id;
    ++next.nextId;
    if (!libraryWriteMeta(next)) { respond(false,"Text geschrieben, Index konnte nicht aktualisiert werden.",500); return; }
  }
  diagnosticEvent("LIB",String(update?"Text aktualisiert, ID=":"Text angelegt, ID=")+libraryItem.id);
  web.send(200,"application/json; charset=utf-8",String("{\"ok\":true,\"id\":")+libraryItem.id+",\"message\":\"Text dauerhaft gespeichert.\"}");
}
void onLibraryDelete() {
  if (!libraryAvailable()) return;
  const uint32_t id=libraryRequestId(); const int ix=libraryIndex(id);
  if (ix<0) { respond(false,"Text nicht gefunden.",404); return; }
  LibraryMeta next=libraryMeta;
  // Clear direct shortcut links, while dependent programs keep their missing IDs.
  if (!v042DetachLibraryShortcuts(id,next)) { respond(false,"Kurzwahlen konnten nicht sicher aktualisiert werden.",500); return; }
  for (int i=ix;i<next.count-1;++i) next.ids[i]=next.ids[i+1];
  next.ids[--next.count]=0;
  if (!libraryWriteMeta(next)) { respond(false,"Index konnte nicht gespeichert werden.",500); return; }
  LittleFS.remove(libraryPath(id)); LittleFS.remove(libraryPath(id,".bak"));
  v042EraseBlock(id);
  diagnosticEvent("LIB",String("Text geloescht, ID=")+id);
  respond(true,"Text geloescht.");
}
void onLibraryMove() {
  if (!libraryAvailable()) return;
  const uint32_t id=libraryRequestId(); const int ix=libraryIndex(id);
  const int to=web.arg("to").toInt();
  if (ix<0 || to<0 || to>=libraryMeta.count) { respond(false,"Sortierung ungueltig.",400); return; }
  if (ix==to) { respond(true,"Reihenfolge unveraendert."); return; }
  LibraryMeta next=libraryMeta; const uint32_t moving=next.ids[ix];
  if (ix<to) for (int i=ix;i<to;++i) next.ids[i]=next.ids[i+1];
  else for (int i=ix;i>to;--i) next.ids[i]=next.ids[i-1];
  next.ids[to]=moving;
  const bool saved=libraryWriteMeta(next);
  respond(saved,saved?"Reihenfolge gespeichert.":"Index konnte nicht gespeichert werden.",saved?200:500);
}
void onLibraryShortcut() {
  if (!libraryAvailable()) return;
  const int slot=web.arg("slot").toInt();
  const uint32_t id=web.hasArg("id")?libraryRequestId():0;
  if (!web.hasArg("slot") || slot<1 || slot>LIB_SHORTCUTS || (id && libraryIndex(id)<0)) {
    respond(false,"Kurzwahl oder ID ungueltig.",400); return;
  }
  LibraryMeta next=libraryMeta; next.shortcuts[slot-1]=id;
  if (!libraryWriteMeta(next)) { respond(false,"Kurzwahl konnte nicht gespeichert werden.",500); return; }
  respond(true,"Kurzwahl dauerhaft gespeichert.");
}
void onLibrarySend() {
  if (!libraryAvailable() || !reserveJob()) return;
  const uint32_t id=libraryRequestId();
  if (!libraryLoad(id)) { respond(false,"Text nicht gefunden oder beschaedigt.",404); return; }
  if (libraryItem.layout==5) {v042SendBlock();return;}
  const TextMeasure a=measureText(libraryItem.text,libraryItem.layout);
  const TextMeasure b=libraryItem.layout==1?measureText(libraryItem.line2,1):TextMeasure{0,0};
  const uint32_t w=max((uint32_t)SIGN_WIDTH,max(a.pixels,b.pixels));
  if (IMAGE_HEADER_BYTES+w*BYTES_PER_COLUMN>MAX_IMAGE_BYTES || (libraryItem.mode==1 && w>SIGN_WIDTH)) { respond(false,"Text uebersteigt aktuelles Datenbudget oder Standbildbreite.",400); return; }
  autoProgram=false;
  pendingText=libraryItem.text; pendingSecondLine=libraryItem.line2;
  pendingLayout=libraryItem.layout; pendingColor=libraryItem.color1; pendingColor2=libraryItem.color2;
  pendingMode=libraryItem.mode; nextJob=TEXT; currentState="queued";
  currentMessage="Bibliothekstext wird an die Tafel gesendet ...";
  diagnosticEvent("LIB",String("Text gesendet, ID=")+id);
  respond(true,currentMessage,202);
}
