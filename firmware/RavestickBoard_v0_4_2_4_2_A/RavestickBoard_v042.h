// RavestickBoard 0.4.2.4.1-dev: block library, collections and typed shortcuts.
// Independent code; requires RavestickBoard_v0_4_2_4_2_A.ino and Library.h.
// v0.4.1 text files (RBL1) and existing shortcut IDs remain readable.
#pragma once
#include "RavestickBoard_Icon.h"

static constexpr uint8_t RB42_PAGES = 25;
static constexpr uint8_t RB42_BAND_LIMIT = (MAX_IMAGE_BYTES-IMAGE_HEADER_BYTES)/(64*BYTES_PER_COLUMN);
static constexpr uint8_t RB42_PROGRAM_LIMIT = 24;
static constexpr uint8_t RB42_STEPS = 24;
static constexpr uint32_t RB42_BLOCK_MAGIC = 0x52424232UL; // RBB2
static constexpr uint32_t RB42_PROGRAM_MAGIC = 0x52425032UL; // RBP2
static constexpr uint32_t RB42_SHORT_MAGIC = 0x52425332UL; // RBS2

struct RB42Block {
  uint32_t magic, id;
  uint8_t schema, count; uint16_t reserved;
  PageRecord pages[RB42_PAGES];
  uint32_t checksum;
};
struct RB42Collection {
  uint32_t magic, id;
  uint8_t schema, count, seconds, reserved; // reserved: 0 original, 1 alles als Laufband
  char name[48];
  uint32_t ids[RB42_STEPS];
  uint32_t checksum;
};
struct RB42Shortcuts {
  uint32_t magic; uint8_t schema;
  uint8_t types[6];  // 0 empty, 1 library, 2 collection, 3 ambience, 4 animation reserved
  uint32_t ids[6];
  char labels[6][48];
  uint32_t checksum;
};
static RB42Block rb42Block = {};
static RB42Collection rb42Collection = {};
static RB42Shortcuts rb42Shortcuts = {};
static PageRecord rb42Band[RB42_BAND_LIMIT] = {};
static uint8_t rb42BandCount = 0;
static bool rb42ProgramPlaying = false;
bool v042IsPlaying(){return rb42ProgramPlaying;}
static uint8_t rb42Step = 0;
static uint32_t rb42NextMs = 0, rb42ProgramId = 0;
static uint8_t rb42ProgramSeconds = 20; // read-only legacy field for backward-compatible storage
static uint8_t rb42ProgramSpeed = 80;
static uint32_t rb42LastGroupMs = 0;
static bool rb42SingleGroup = false;
static uint8_t rb42PageTile[64 * BYTES_PER_COLUMN] = {};
// v0.4.2.3: one horizontal RGB band for mixed text and block motifs.
// Group plan is small; the renderer uses one <=30 KiB buffer, never stack arrays.
static constexpr uint8_t RB43_SEGMENTS=96;
static constexpr uint16_t RB43_GAP=16;
struct RB43Segment {uint32_t id; uint16_t x, cols; uint8_t page;};
static RB43Segment rb43Parts[RB43_SEGMENTS] = {};
static uint8_t rb43Count=0, rb43Step=0, rb43Page=0;
static uint16_t rb43Width=64;
static bool rb43Playback=false, rb43CurrentGroup=false, rb43FirstGroup=true;
size_t rb43RenderGroup();


String rb42BlockPath(uint32_t id) { return String("/rb-lib/b")+id+".bin"; }
String rb42CollectionPath(uint32_t id) { return String("/rb-lib/p")+id+".bin"; }
// Fixed-size records are read directly into the preallocated destination.
// No 25-page (~2.6 KiB) temporary objects on the 8 KiB Arduino loopTask stack.
template <typename T> bool rb42Read(const String &path, T &result, uint32_t magic) {
  if (!libraryReady) return false;
  auto readVerified = [&](const String &name) -> bool {
    File f = LittleFS.open(name, "r");
    if (!f) return false;
    const bool goodSize = f.size() == sizeof(result);
    const bool loaded = goodSize && f.readBytes(reinterpret_cast<char*>(&result), sizeof(result)) == sizeof(result);
    f.close();
    return loaded && result.magic == magic && result.schema == 1 &&
           result.checksum == libraryDigest(result);
  };
  if (readVerified(path)) return true;
  if (path.endsWith(".tmp")) return false;
  const String bak = path + ".bak";
  if (!readVerified(bak)) return false;
  // Keep a valid copy in RAM if rollback cannot be moved to the primary path.
  if (LittleFS.exists(path)) LittleFS.remove(path);
  if (!LittleFS.rename(bak, path)) {
    diagnosticEvent("LIB", "Gueltige Backup-Datei konnte nicht umbenannt werden", true);
  }
  return true;
}
template <typename T> bool rb42Write(const String &path, T &value) {
  if (!libraryReady) return false;
  value.checksum = libraryDigest(value);
  const String tmp = path + ".tmp", bak = path + ".bak";
  LittleFS.remove(tmp);
  File f = LittleFS.open(tmp, "w");
  if (!f) return false;
  const bool written = f.write(reinterpret_cast<const uint8_t*>(&value), sizeof(value)) == sizeof(value);
  f.flush(); f.close();
  if (!written) { LittleFS.remove(tmp); return false; }
  // Verify the bytes with a 128-byte scratch buffer, not a second large record.
  File check = LittleFS.open(tmp, "r");
  bool verified = check && check.size() == sizeof(value);
  uint8_t chunk[128];
  const uint8_t *original = reinterpret_cast<const uint8_t*>(&value);
  for (size_t at = 0; verified && at < sizeof(value); at += sizeof(chunk)) {
    const size_t n = min(sizeof(chunk), sizeof(value) - at);
    verified = check.read(chunk, n) == n && memcmp(chunk, original + at, n) == 0;
  }
  if (check) check.close();
  if (!verified) { LittleFS.remove(tmp); diagnosticEvent("LIB", "Block/Programm Schreibpruefung fehlgeschlagen", true); return false; }
  const bool exists = LittleFS.exists(path);
  LittleFS.remove(bak);
  if (exists && !LittleFS.rename(path, bak)) { LittleFS.remove(tmp); return false; }
  if (!LittleFS.rename(tmp, path)) {
    if (exists) LittleFS.rename(bak, path);
    LittleFS.remove(tmp); return false;
  }
  LittleFS.remove(bak);
  return true;
}
bool rb42ReadBlock(uint32_t id) {
  if (!rb42Read(rb42BlockPath(id),rb42Block,RB42_BLOCK_MAGIC)) return false;
  if (rb42Block.id!=id || rb42Block.count<1 || rb42Block.count>RB42_PAGES) return false;
  for (uint8_t i=0;i<rb42Block.count;++i) {
    const PageRecord &p=rb42Block.pages[i];
    if (p.line1[47] || p.line2[47] || p.layout>4 || p.align>2 ||
      p.color1<1 || p.color1>7 || p.color2<1 || p.color2>7) return false;
  }
  return true;
}
uint8_t v042BlockPageCount(uint32_t id) {
  if (!rb42ReadBlock(id)) return 0;
  return rb42Block.count;
}
void v042EraseBlock(uint32_t id) {
  LittleFS.remove(rb42BlockPath(id));
  LittleFS.remove(rb42BlockPath(id)+".bak");
}
bool rb42ReadCollection(uint32_t id) {
  if (!rb42Read(rb42CollectionPath(id),rb42Collection,RB42_PROGRAM_MAGIC)) return false;
  if (rb42Collection.id!=id || rb42Collection.count<1 || rb42Collection.count>RB42_STEPS ||
      rb42Collection.name[47]!=0 || rb42Collection.reserved>1 || rb42Collection.seconds<5 || rb42Collection.seconds>180) return false;
  // Do not hide or discard damaged references: the editor must repair them.
  return true;
}
uint8_t rb42MissingCount() {
  uint8_t missing=0;
  for (uint8_t j=0; j<rb42Collection.count; ++j) {
    if (libraryIndex(rb42Collection.ids[j])<0) ++missing;
  }
  return missing;
}
// A deleted record may be referenced by a saved program. Keep the references;
// the program will remain visible as "repair needed" and cannot run.
bool v042DetachLibraryShortcuts(uint32_t id, LibraryMeta &nextMeta) {
  RB42Shortcuts nextShortcuts=rb42Shortcuts;
  bool changed=false;
  for(uint8_t i=0;i<6;++i) {
    if(nextShortcuts.types[i]==1 && nextShortcuts.ids[i]==id) {
      nextShortcuts.types[i]=0;
      nextShortcuts.ids[i]=0;
      nextShortcuts.labels[i][0]=0;
      changed=true;
    }
    if(nextMeta.shortcuts[i]==id) nextMeta.shortcuts[i]=0;
  }
  if(changed) {
    if(!rb42Write(String("/rb-lib/shortcuts2.bin"),nextShortcuts))return false;
    rb42Shortcuts=nextShortcuts;
  }
  // Stop playback before deleting an entry used in the active program.
  if(rb42ProgramPlaying) {
    for(uint8_t n=1;n<=RB42_PROGRAM_LIMIT;++n) {
      if(n!=rb42ProgramId || !rb42ReadCollection(n))continue;
      for(uint8_t j=0;j<rb42Collection.count;++j)if(rb42Collection.ids[j]==id) {
        rb42ProgramPlaying=false;rb42NextMs=0;
        diagnosticEvent("PROGRAM","Aktives Programm wegen geloeschtem Bibliothekseintrag gestoppt",true);
        break;
      }
    }
  }
  return true;
}
void v042Begin() {
  if (!libraryReady) return;
  for(uint8_t i=0;i<6;++i) {rb42Shortcuts.types[i]=libraryMeta.shortcuts[i]?1:0;rb42Shortcuts.ids[i]=libraryMeta.shortcuts[i];}
  RB42Shortcuts prior={};
  if(rb42Read(String("/rb-lib/shortcuts2.bin"),prior,RB42_SHORT_MAGIC)) rb42Shortcuts=prior;
  else {
    rb42Shortcuts.magic=RB42_SHORT_MAGIC; rb42Shortcuts.schema=1;
    // Migration on first startup: retain v0.4.1 six slots even before first write.
    for(uint8_t i=0;i<6;++i) if(rb42Shortcuts.ids[i] && libraryLoad(rb42Shortcuts.ids[i]))
      snprintf(rb42Shortcuts.labels[i],48,"%s",libraryItem.name);
    if(!LittleFS.exists("/rb-lib/shortcuts2.bin"))
      rb42Write(String("/rb-lib/shortcuts2.bin"),rb42Shortcuts);
    else diagnosticEvent("V042","Kurzwahl-Datei beschädigt; keine automatische Überschreibung",true);
  }
  diagnosticEvent("V042","Blockfolgen, Programme und Kurzwahlen initialisiert");
}
void v042ServeIcon() {
  web.sendHeader("Cache-Control","public, max-age=86400");
  web.send_P(200,"image/png",(const char*)RB42_ICON_PNG,RB42_ICON_LENGTH);
}
void v042ResetBlock() {
  if(!reserveJob()) return;
  pageCount=1;
  for(auto &p:boardPages) defaultPage(p);
  autoProgram=false;
  // Draft lives only in RAM. Deliberately no NVS write.
  respond(true,"Blockeditor geleert. Keine Bibliothekseinträge verändert.");
}
void v042SaveBlock() {
  if(!libraryAvailable()) return;
  if (pageCount<1 || pageCount>RB42_PAGES) {respond(false,"Ungültige Seitenzahl.",400);return;}
  uint32_t id=web.hasArg("id")?libraryRequestId():0;
  bool update=id>0;
  if (update && (!libraryLoad(id) || libraryItem.layout!=5)) {respond(false,"Blockfolge nicht vorhanden.",404);return;}
  if (!update && (libraryMeta.count>=LIB_LIMIT || libraryMeta.nextId==0xFFFFFFFFUL ||
                  LittleFS.totalBytes()-LittleFS.usedBytes()<65536UL)) {respond(false,"Nicht genügend Bibliotheksplätze oder Flashreserve.",507);return;}
  const String name=web.arg("name");
  if (!name.length() || name.length()>=48) {respond(false,"Name maximal 47 UTF-8-Bytes.",400);return;}
  const uint32_t target=update?id:libraryMeta.nextId;
  memset(&rb42Block,0,sizeof(rb42Block));
  rb42Block.magic=RB42_BLOCK_MAGIC; rb42Block.schema=1; rb42Block.id=target; rb42Block.count=pageCount;
  for(uint8_t i=0;i<pageCount;++i) {
    const PageRecord &p=boardPages[i];
    const TextMeasure a=measureText(String(p.line1),p.layout);
    const TextMeasure b=p.layout==1?measureText(String(p.line2),p.layout):TextMeasure{0,0};
    if(a.pixels>64 || b.pixels>64 || p.layout>4) {respond(false,String("Seite ")+(i+1)+" ist breiter als 64 Pixel.",400);return;}
    rb42Block.pages[i]=p;
  }
  if(!rb42Write(rb42BlockPath(target),rb42Block)) {respond(false,"Blockfolge konnte nicht gespeichert werden.",500);return;}
  if(!update) memset(&libraryItem,0,sizeof(libraryItem));
  libraryItem.magic=LIB_MAGIC;libraryItem.schema=LIB_SCHEMA;
  libraryItem.id=target;libraryItem.layout=5;libraryItem.mode=1;libraryItem.color1=7;libraryItem.color2=7;
  snprintf(libraryItem.name,48,"%s",name.c_str());
  snprintf(libraryItem.text,LIB_TEXT_SIZE,"%s","[Blockfolge]");
  libraryItem.line2[0]=0;
  libraryItem.checksum=libraryDigest(libraryItem);
  if(!libraryWriteRecord(libraryItem)) {respond(false,"Bibliotheks-Metadaten nicht gespeichert.",500);return;}
  if(!update) {
    LibraryMeta next=libraryMeta;next.ids[next.count++]=target;++next.nextId;
    if(!libraryWriteMeta(next)) {respond(false,"Block gespeichert, aber Index konnte nicht aktualisiert werden.",500);return;}
  }
  web.send(200,"application/json; charset=utf-8",String("{\"ok\":true,\"id\":")+target+",\"message\":\"Blockfolge gespeichert.\"}");
}
void v042BlockMeta() {
  if(!libraryAvailable()) return;
  uint32_t id=libraryRequestId();
  if(!libraryLoad(id) || libraryItem.layout!=5) {respond(false,"Blockfolge fehlt.",404);return;}
  if(web.hasArg("name")) {
    String name=web.arg("name");
    if(!name.length() || name.length()>=48) {respond(false,"Name maximal 47 UTF-8-Bytes.",400);return;}
    snprintf(libraryItem.name,48,"%s",name.c_str());
  }
  if(web.hasArg("favorite")) libraryItem.favorite=web.arg("favorite")=="1"?1:0;
  libraryItem.checksum=libraryDigest(libraryItem);
  if(!libraryWriteRecord(libraryItem)) {respond(false,"Änderung nicht gespeichert.",500);return;}
  respond(true,"Blockfolge aktualisiert.");
}
void v042GetBlock() {
  if(!libraryAvailable()) return;
  uint32_t id=libraryRequestId();
  if(!libraryLoad(id) || libraryItem.layout!=5 || !rb42ReadBlock(id)) {respond(false,"Blockfolge fehlt oder ist beschädigt.",404);return;}
  libraryStartJson();
  web.sendContent(String("{\"ok\":true,\"id\":")+id+",\"name\":"+jsonQuote(String(libraryItem.name))+",\"pages\":[");
  for(uint8_t i=0;i<rb42Block.count;++i){
    if(i)web.sendContent(",");
    const PageRecord &p=rb42Block.pages[i];
    web.sendContent(String("{\"line1\":")+jsonQuote(p.line1)+",\"line2\":"+jsonQuote(p.line2)+
      ",\"layout\":"+p.layout+",\"align\":"+p.align+",\"color1\":"+p.color1+
      ",\"color2\":"+p.color2+",\"duration\":"+p.seconds+"}");
  }
  web.sendContent("]}");web.sendContent("");
}
void v042LoadBlock() {
  if(!reserveJob() || !libraryAvailable()) return;
  uint32_t id=libraryRequestId();
  if(!libraryLoad(id) || libraryItem.layout!=5 || !rb42ReadBlock(id)) {respond(false,"Blockfolge beschädigt oder nicht vorhanden.",404);return;}
  for(auto &p:boardPages)defaultPage(p);
  pageCount=rb42Block.count;
  for(uint8_t i=0;i<pageCount;++i)boardPages[i]=rb42Block.pages[i];
  if(!savePages()) {respond(false,"Blockfolge konnte nicht als Arbeitskopie geladen werden.",500);return;}
  respond(true,"Blockfolge im Editor geöffnet. Bibliothekseintrag bleibt bis Speichern unverändert.");
}
// Every page is rendered into a fixed 384-byte tile. The 107-byte legacy
// header is preserved, and long bands are streamed in wire order without a
// 30 KiB bitmap allocation. Both routes share the same RGB-plane mapping.
static uint16_t rb42CachedPage=0xFFFF;
bool rb42RenderPageTile(uint8_t i){
  if(i>=rb42BandCount)return false;
  if(rb42CachedPage==i)return true;
  const PageRecord &p=rb42Band[i];
  uint32_t a[48]={},b[48]={};
  const int na=decodeText(String(p.line1),a);
  const int nb=p.layout==1?decodeText(String(p.line2),b):0;
  if(na<0 || nb<0 || p.layout>4 || linePixelWidth(a,na,p.layout)>64 ||
      (p.layout==1 && linePixelWidth(b,nb,1)>64))return false;
  memset(rb42PageTile,0,sizeof(rb42PageTile));
  uint8_t *former=pixelData;
  pixelData=rb42PageTile;
  if(p.layout==1){
    drawInline(a,na,1,p.align,0,p.color1,64);
    drawInline(b,nb,1,p.align,8,p.color2,64);
  }else{
    uint8_t top=p.layout==0?1:p.layout==2?4:0;
    drawInline(a,na,p.layout,p.align,top,p.color1,64);
  }
  pixelData=former;
  rb42CachedPage=i;
  return true;
}
void rb42CopyBandSlice(size_t offset,size_t len,uint8_t *dest){
  const size_t planeBytes=size_t(rb42BandCount)*64*2;
  for(size_t i=0;i<len;++i){
    const size_t pos=offset+i;
    if(pos<IMAGE_HEADER_BYTES){dest[i]=streamHeader[pos];continue;}
    const size_t image=pos-IMAGE_HEADER_BYTES;
    const size_t plane=image/planeBytes;
    const size_t local=image%planeBytes;
    const uint8_t page=local/(64*2);
    if(plane>2 || !rb42RenderPageTile(page)){dest[i]=0;continue;}
    dest[i]=rb42PageTile[plane*128+(local%(64*2))];
  }
}
size_t renderProgramBand(){
  if(rb42BandCount<1 || rb42BandCount>RB42_BAND_LIMIT)return 0;
  const uint16_t width=uint16_t(rb42BandCount)*64;
  const uint32_t rgbBytes=uint32_t(width)*BYTES_PER_COLUMN;
  const size_t payloadLength=IMAGE_HEADER_BYTES+rgbBytes;
  rb42CachedPage=0xFFFF;
  // Validate every page before beginning BLE; an invalid page must never be
  // silently replaced by blank pixels during a streaming transfer.
  for(uint8_t i=0;i<rb42BandCount;++i)
    if(!rb42RenderPageTile(i))return 0;
  rb42CachedPage=0xFFFF;
  if(payloadLength>=STREAM_THRESHOLD_BYTES){
    if(textPayload){free(textPayload);textPayload=nullptr;}
    pixelData=nullptr;
    streamPayload=true;rb42StreamBand=true;
    memset(streamHeader,0,sizeof(streamHeader));
    streamHeader[24]=1;streamHeader[25]=0x30;
    streamHeader[105]=(rgbBytes>>8)&255;streamHeader[106]=rgbBytes&255;
    ramTraceMark(RT_RENDER_BEGIN,(uint16_t)payloadLength);
    ramTraceMark(RT_BUFFER_ALLOC,sizeof(rb42PageTile));
    diagnosticEvent("BAND",String("Tile-Streaming: ")+rb42BandCount+" Seiten, "+payloadLength+" Bytes");
    return payloadLength;
  }
  if(!allocateRenderBuffer(rgbBytes))return 0;
  uint8_t *whole=pixelData;
  for(uint8_t i=0;i<rb42BandCount;++i){
    if(!rb42RenderPageTile(i)){pixelData=whole;return 0;}
    for(uint8_t plane=0;plane<3;++plane)
      memcpy(whole+size_t(plane)*width*2+size_t(i)*128,rb42PageTile+size_t(plane)*128,128);
  }
  pixelData=whole;
  textPayload[24]=1;textPayload[25]=0x30;
  textPayload[105]=(rgbBytes>>8)&255;textPayload[106]=rgbBytes&255;
  return payloadLength;
}

void v042SendBlock() {
  if(!reserveJob() || !libraryAvailable())return;
  uint32_t id=libraryRequestId();
  if(!libraryLoad(id) || libraryItem.layout!=5 || !rb42ReadBlock(id)) {respond(false,"Blockfolge fehlt.",404);return;}
  rb42BandCount=rb42Block.count;
  for(uint8_t i=0;i<rb42BandCount;++i)rb42Band[i]=rb42Block.pages[i];
  rb42ProgramPlaying=false;autoProgram=false;
  nextJob=BAND;currentState="queued";currentMessage="Blockfolge wird in einem BLE-Upload übertragen.";
  respond(true,currentMessage,202);
}
void v042ListCollections() {
  if(!libraryAvailable())return;
  libraryStartJson();web.sendContent("{\"ok\":true,\"items\":[");
  bool first=true;
  for(uint8_t id=1;id<=RB42_PROGRAM_LIMIT;++id){
    if(!rb42ReadCollection(id))continue;
    if(!first)web.sendContent(",");first=false;
    web.sendContent(String("{\"id\":")+id+",\"name\":"+jsonQuote(rb42Collection.name)+
      ",\"count\":"+rb42Collection.count+",\"missing\":"+rb42MissingCount()+
      ",\"seconds\":"+rb42Collection.seconds+",\"playback\":"+rb42Collection.reserved+",\"ids\":[");
    for(uint8_t k=0;k<rb42Collection.count;++k) {if(k)web.sendContent(","); web.sendContent(String(rb42Collection.ids[k]));}
    web.sendContent("]}");
  }
  web.sendContent("]}");web.sendContent("");
}
void v042GetCollection() {
  if(!libraryAvailable())return;
  uint32_t id=libraryRequestId();
  if(!rb42ReadCollection(id)){respond(false,"Programm fehlt oder verweist auf gelöschten Inhalt.",404);return;}
  libraryStartJson();web.sendContent(String("{\"ok\":true,\"id\":")+id+",\"name\":"+
    jsonQuote(rb42Collection.name)+",\"seconds\":"+rb42Collection.seconds+
    ",\"missing\":"+rb42MissingCount()+",\"playback\":"+rb42Collection.reserved+",\"ids\":[");
  for(uint8_t i=0;i<rb42Collection.count;++i){if(i)web.sendContent(",");web.sendContent(String(rb42Collection.ids[i]));}
  web.sendContent("]}");web.sendContent("");
}
void v042SaveCollection() {
  if(!libraryAvailable())return;
  const uint32_t id=web.hasArg("id")?libraryRequestId():0;
  if(id>RB42_PROGRAM_LIMIT) {respond(false,"Ungültige Programm-ID.",400);return;}
  if(id && !rb42ReadCollection(id)) {respond(false,"Programm nicht vorhanden.",404);return;}
  const String name=web.arg("name"), csv=web.arg("ids");
  const int seconds=web.arg("seconds").toInt();
  const int playback=web.hasArg("playback")?web.arg("playback").toInt():0;
  if(!name.length() || name.length()>=48 || seconds<5 || seconds>180 || playback<0 || playback>1) {respond(false,"Name oder Gruppenlaufzeit ungültig.",400);return;}
  const uint32_t target=id?id:([&]()->uint32_t{for(uint32_t i=1;i<=RB42_PROGRAM_LIMIT;++i)if(!LittleFS.exists(rb42CollectionPath(i)))return i;return 0;})();
  if(!target) {respond(false,"Maximal 24 Programme.",507);return;}
  memset(&rb42Collection,0,sizeof(rb42Collection));
  rb42Collection.magic=RB42_PROGRAM_MAGIC;rb42Collection.schema=1;rb42Collection.id=target;
  rb42Collection.seconds=seconds;rb42Collection.reserved=playback;snprintf(rb42Collection.name,48,"%s",name.c_str());
  int start=0;
  while(start<(int)csv.length()) {
    int end=csv.indexOf(',',start);if(end<0)end=csv.length();
    String chunk=csv.substring(start,end);chunk.trim();
    if(chunk.length()<1 || chunk.length()>10 || rb42Collection.count>=RB42_STEPS) {respond(false,"Programm enthält ungültige Schritte.",400);return;}
    for(unsigned i=0;i<chunk.length();++i)if(chunk[i]<'0'||chunk[i]>'9'){respond(false,"Ungültige ID.",400);return;}
    uint32_t item=(uint32_t)strtoul(chunk.c_str(),nullptr,10);
    if(libraryIndex(item)<0 || !libraryLoad(item)) {respond(false,"Programm referenziert unbekannten Inhalt.",400);return;}
    rb42Collection.ids[rb42Collection.count++]=item;
    start=end+1;
  }
  if(!rb42Collection.count) {respond(false,"Programm braucht mindestens einen Eintrag.",400);return;}
  if(!rb42Write(rb42CollectionPath(target),rb42Collection)) {respond(false,"Programm konnte nicht gespeichert werden.",500);return;}
  web.send(200,"application/json; charset=utf-8",String("{\"ok\":true,\"id\":")+target+",\"message\":\"Programm gespeichert.\"}");
}
void v042DeleteCollection() {
  if(!libraryAvailable())return;
  uint32_t id=libraryRequestId();
  if(!rb42ReadCollection(id)) {respond(false,"Programm nicht vorhanden.",404);return;}
  for(uint8_t i=0;i<6;++i)if(rb42Shortcuts.types[i]==2&&rb42Shortcuts.ids[i]==id){respond(false,"Programm ist einer Kurzwahl zugewiesen.",409);return;}
  if(rb42ProgramId==id) rb42ProgramPlaying=false;
  LittleFS.remove(rb42CollectionPath(id));
  LittleFS.remove(rb42CollectionPath(id)+".bak"); // Loeschung darf nicht als Wiederherstellungsfall gelten.
  respond(true,"Programm gelöscht.");
}
// One group per BLE upload. Contiguous block entries share a single bitmap;
// Content-order group builder for the alternate "Alles als Laufband" mode.
// Blocks are split on complete 64-column pages; text is never cut mid-glyph.
bool rb43QueueScrollGroup(){
  if(!rb42ReadCollection(rb42ProgramId) || rb42MissingCount())return false;
  if(rb43Step>=rb42Collection.count){rb43Step=0;rb43Page=0;}
  rb43Count=0;uint32_t used=0;
  for(;rb43Step<rb42Collection.count;){
    const uint32_t id=rb42Collection.ids[rb43Step];
    if(!libraryLoad(id))return false;
    const bool block=libraryItem.layout==5;
    uint8_t count=1;
    if(block){if(!rb42ReadBlock(id))return false;count=rb42Block.count;}
    for(;rb43Page<count;){
      uint32_t cols=64;
      if(!block){
        const TextMeasure first=measureText(libraryItem.text,libraryItem.layout);
        const TextMeasure second=libraryItem.layout==1?measureText(libraryItem.line2,1):TextMeasure{0,0};
        cols=max((uint32_t)1,max(first.pixels,second.pixels));
      }
      const uint32_t next=used+(rb43Count?RB43_GAP:0)+cols;
      if(rb43Count && (next>MAX_COLUMNS || rb43Count>=RB43_SEGMENTS))goto complete;
      if(next>MAX_COLUMNS)return false;
      const uint16_t x=used+(rb43Count?RB43_GAP:0);
      rb43Parts[rb43Count++]={id,x,(uint16_t)cols,(uint8_t)(block?rb43Page:255)};
      used=next;++rb43Page;
    }
    ++rb43Step;rb43Page=0;
  }
complete:
  if(!rb43Count)return false;
  rb43Width=max((uint16_t)64,(uint16_t)used);
  rb42SingleGroup=rb43FirstGroup && rb43Step>=rb42Collection.count;
  rb43FirstGroup=false;
  rb43CurrentGroup=true;nextJob=TEXT;pendingMode=2;pendingLayout=0;
  currentState="queued";currentMessage="Gemeinsames Laufband wird übertragen.";
  return true;
}
// matching scrolling texts share a single banner. Mixed groups require uploads.
bool rb42QueueStep() {
  if(rb43Playback)return rb43QueueScrollGroup();
  rb43CurrentGroup=false;
  if(!rb42ReadCollection(rb42ProgramId) || rb42MissingCount())return false;
  if(rb42Step>=rb42Collection.count)rb42Step=0;
  uint8_t i=rb42Step;
  const uint8_t firstStep=i;
  uint32_t id=rb42Collection.ids[i];
  if(!libraryLoad(id))return false;
  if(libraryItem.layout==5){
    rb42BandCount=0;
    for(;i<rb42Collection.count;++i){
      const uint32_t entry=rb42Collection.ids[i];
      if(!libraryLoad(entry))return false;
      if(libraryItem.layout!=5)break;
      if(!rb42ReadBlock(entry))return false;
      if(rb42BandCount+rb42Block.count>RB42_BAND_LIMIT)break;
      for(uint8_t p=0;p<rb42Block.count;++p)rb42Band[rb42BandCount++]=rb42Block.pages[p];
    }
    if(!rb42BandCount)return false;
    rb42LastGroupMs=0; // calculated when the transfer succeeds
    nextJob=BAND;
  }else{
    const uint8_t layout=libraryItem.layout,c1=libraryItem.color1,c2=libraryItem.color2,mode=libraryItem.mode;
    pendingText=libraryItem.text;pendingSecondLine=libraryItem.line2;
    pendingLayout=layout;pendingColor=c1;pendingColor2=c2;pendingMode=mode;
    ++i;
    // Merge adjacent compatible scrolls when the combined image is within budget.
    if(mode==2 && layout!=1)for(;i<rb42Collection.count;++i){
      if(!libraryLoad(rb42Collection.ids[i]))return false;
      if(libraryItem.layout!=layout||libraryItem.color1!=c1||libraryItem.color2!=c2||libraryItem.mode!=2)break;
      String joined=pendingText+"     "+libraryItem.text;
      const TextMeasure m=measureText(joined,layout);
      if(IMAGE_HEADER_BYTES+max((uint32_t)64,m.pixels)*BYTES_PER_COLUMN>MAX_IMAGE_BYTES)break;
      pendingText=joined;
    }
    nextJob=TEXT;
  }
  rb42Step=i;
  rb42SingleGroup=(firstStep==0 && i>=rb42Collection.count);
  currentState="queued";currentMessage="Programmgruppe wird übertragen.";
  return true;
}
// Assemble RGB bitplanes without re-encoding text or losing symbol colors.
// Text is drawn directly at its segment's horizontal offset; block pages
// reuse the exactly tested 64x16 page renderer.
// The mixed stream is built from the established 384-byte bitmap tile.
// It never allocates a 30-KiB full RGB bitmap on the ESP32-C3 heap.
static uint32_t rb43CachedBlockId=0;
size_t rb43RenderGroup(){
  if(!rb43Count || rb43Width>MAX_COLUMNS)return 0;
  const size_t rgbBytes=size_t(rb43Width)*BYTES_PER_COLUMN;
  if(rgbBytes+IMAGE_HEADER_BYTES>MAX_IMAGE_BYTES)return 0;
  free(textPayload);textPayload=nullptr;pixelData=nullptr;
  rb42StreamBand=false;rb43StreamBand=true;streamPayload=false;
  streamImageWidth=rb43Width;streamCachedTile=0xFFFF;streamTileRenders=0;
  rb43CachedBlockId=0;rb42CachedPage=0xFFFF;
  memset(streamHeader,0,sizeof(streamHeader));
  streamHeader[24]=1;streamHeader[25]=0x30;
  streamHeader[105]=(rgbBytes>>8)&255;streamHeader[106]=rgbBytes&255;
  ramTraceMark(RT_RENDER_BEGIN,(uint16_t)(IMAGE_HEADER_BYTES+rgbBytes));
  ramTraceMark(RT_BUFFER_ALLOC,(uint16_t)sizeof(streamTile));
  diagnosticEvent("PROGRAM",String("Scrollband (Tile-Streaming): ")+rb43Count+" Teile, "+rb43Width+" Spalten");
  return IMAGE_HEADER_BYTES+rgbBytes;
}
void rb43RenderMixedTile(uint16_t col){
  const uint16_t begin=(col/STREAM_TILE_COLUMNS)*STREAM_TILE_COLUMNS;
  if(streamCachedTile==begin)return;
  streamTileStart=begin;
  streamTileCols=min((uint16_t)STREAM_TILE_COLUMNS,(uint16_t)(rb43Width-begin));
  memset(streamTile,0,streamTileCols*BYTES_PER_COLUMN);
  const uint16_t end=begin+streamTileCols;
  for(uint8_t i=0;i<rb43Count;++i){
    const RB43Segment &segment=rb43Parts[i];
    if(segment.x>=end || uint32_t(segment.x)+segment.cols<=begin)continue;
    if(!libraryLoad(segment.id))continue;
    if(segment.page!=255){
      if(libraryItem.layout!=5)continue;
      if(rb43CachedBlockId!=segment.id){
        if(!rb42ReadBlock(segment.id))continue;
        rb43CachedBlockId=segment.id;
      }
      if(segment.page>=rb42Block.count)continue;
      rb42Band[0]=rb42Block.pages[segment.page];rb42BandCount=1;
      rb42CachedPage=0xFFFF;
      streamDrawingTile=false;
      if(!rb42RenderPageTile(0))continue;
      const uint16_t left=max((uint16_t)begin,segment.x);
      const uint16_t right=min(end,(uint16_t)(segment.x+64));
      for(uint8_t plane=0;plane<3;++plane){
        memcpy(streamTile+size_t(plane)*streamTileCols*2+size_t(left-begin)*2,
          rb42PageTile+size_t(plane)*128+size_t(left-segment.x)*2,
          size_t(right-left)*2);
      }
    }else{
      pixelData=streamTile;streamDrawingTile=true;
      const uint8_t layout=libraryItem.layout;
      if(layout==1){
        const TextMeasure first=measureText(libraryItem.text,1);
        const TextMeasure second=measureText(libraryItem.line2,1);
        drawInlineUtf8Window(libraryItem.text,1,0,0,libraryItem.color1,rb43Width,first.pixels,segment.x,segment.cols);
        drawInlineUtf8Window(libraryItem.line2,1,0,8,libraryItem.color2,rb43Width,second.pixels,segment.x,segment.cols);
      }else{
        const TextMeasure first=measureText(libraryItem.text,layout);
        drawInlineUtf8Window(libraryItem.text,layout,1,layout==0?1:layout==2?4:0,
          libraryItem.color1,rb43Width,first.pixels,segment.x,segment.cols);
      }
      streamDrawingTile=false;pixelData=nullptr;
    }
  }
  streamDrawingTile=false;pixelData=nullptr;
  streamCachedTile=begin;++streamTileRenders;
}
void rb43CopyMixedSlice(size_t offset,size_t length,uint8_t *dest){
  const size_t planeBytes=size_t(rb43Width)*2;
  for(size_t i=0;i<length;++i){
    const size_t pos=offset+i;
    if(pos<IMAGE_HEADER_BYTES){dest[i]=streamHeader[pos];continue;}
    const size_t image=pos-IMAGE_HEADER_BYTES;
    const size_t plane=image/planeBytes;
    const size_t within=image%planeBytes;
    const uint16_t col=within/2;
    rb43RenderMixedTile(col);
    dest[i]=streamTile[plane*streamTileCols*2+(col-streamTileStart)*2+(within%2)];
  }
}

void v042RunCollection() {
  if(!libraryAvailable() || !reserveJob())return;
  uint32_t id=libraryRequestId();
  if(!rb42ReadCollection(id)){respond(false,"Programm nicht vorhanden.",404);return;}
  if(rb42MissingCount()){respond(false,"Programm ist unvollständig. Bitte zuerst im Programmeditor reparieren.",409);return;}
  rb42ProgramPlaying=true;rb42ProgramId=id;rb42Step=0;
  rb42NextMs=0;rb42ProgramSpeed=pendingSpeed;
  rb43Playback=rb42Collection.reserved==1;rb43Step=0;rb43Page=0;rb43FirstGroup=true;
  rb42ProgramSeconds=rb42Collection.seconds;
  autoProgram=false;
  if(!rb42QueueStep()){rb42ProgramPlaying=false;respond(false,"Programm kann nicht vorbereitet werden.",500);return;}
  respond(true,"Programm gestartet. Zusammenhängende Blöcke werden mit einem Upload gesendet.",202);
}
void v042StopCollection(){rb42ProgramPlaying=false;rb43CurrentGroup=false;rb43Playback=false;rb42NextMs=0;respond(true,"ESP32-Programmwechsel gestoppt. Tafel läuft autonom weiter.");}
void v042Tick(){
  if(!rb42ProgramPlaying || nextJob!=NONE || currentState=="sending" || !rb42NextMs)return;
  if((int32_t)(millis()-rb42NextMs)<0)return;
  rb42NextMs=0;
  if(!rb42QueueStep()){rb42ProgramPlaying=false;diagnosticEvent("PROGRAM","Programmausführung gestoppt",true);}
}
void v042AfterJob(Job job,bool ok){
  if(!rb42ProgramPlaying || (job!=TEXT && job!=BAND))return;
  if(!ok){rb42ProgramPlaying=false;rb42NextMs=0;return;}
  if(!rb42ReadCollection(rb42ProgramId)){rb42ProgramPlaying=false;return;}
  // Single group remains on the panel indefinitely and loops natively.
  if(rb42SingleGroup){rb42ProgramPlaying=false;rb42NextMs=0;return;}
  // Hardware-calibrated at 60/70/80/90 for a 4-page band and 6 pages at 80:
  // T ~= pages * 79.5 ms * (100-speed). Validated only within [60..90].
  // The old 5..180s group setting is retained on disk ONLY for compatibility.
  // Scrolling time is estimated from travel width, not yet hardware-calibrated:
  // base: observed page step / 64 columns, +35% buffer to avoid early clipping.
  // For speeds outside our measured 60..90 range use a capped conservative factor.
  const uint32_t speedFactor=100UL-constrain((int)rb42ProgramSpeed,10,95);
  const uint32_t pixels=rb43Playback ? uint32_t(rb43Width)+64UL :
    uint32_t(max((uint32_t)64,measureText(pendingText,pendingLayout).pixels))+64UL;
  // The CoolLEDX scroll travel time has NOT been calibrated on hardware.
  // Use a deliberately conservative guard window for split groups, to avoid
  // chopping off the end. Visible repeats are preferable to lost content.
  uint32_t duration=max(4000UL,(uint32_t)((uint64_t)pixels*speedFactor*275ULL/100ULL));
  // PROVISIONAL, not a guaranteed exact cycle. Measure scroll widths at speed 80.
  // Recalibrate from independent scrolling measurements before stable release.
  if(job==BAND && !rb43Playback && rb42ProgramSpeed>=60 && rb42ProgramSpeed<=90) {
    duration=(uint32_t)rb42BandCount*795UL*(100-rb42ProgramSpeed)/10UL;
    duration=max((uint32_t)500UL,duration);
  }
  if(rb43Playback) diagnosticEvent("PROGRAM",String("Laufband grob geschaetzt: ")+rb43Width+" Spalten, "+duration+" ms; keine 20s-Grenze");
  rb42LastGroupMs=duration;
  rb42NextMs=millis()+duration;
  diagnosticEvent("PROGRAM",String("Gruppe ")+(job==BAND?"Block":"Text")+" Dauer="+duration+" ms, naechster Upload danach");
}
void v042GetShortcuts(){
  if(!libraryAvailable())return;
  libraryStartJson();web.sendContent("{\"ok\":true,\"items\":[");
  for(uint8_t i=0;i<6;++i){if(i)web.sendContent(",");
    web.sendContent(String("{\"type\":")+rb42Shortcuts.types[i]+",\"id\":"+rb42Shortcuts.ids[i]+
      ",\"label\":"+jsonQuote(rb42Shortcuts.labels[i])+"}");
  }
  web.sendContent("]}");web.sendContent("");
}
void v042SetShortcut(){
  if(!libraryAvailable())return;
  int slot=web.arg("slot").toInt(),type=web.arg("type").toInt();
  uint32_t id=web.hasArg("id")?libraryRequestId():0;
  String label=web.arg("label");
  if(slot<1||slot>6||type<0||type>3||label.length()>=48){respond(false,"Kurzwahl ungültig (maximal 47 UTF-8-Bytes Beschriftung).",400);return;}
  if(type==1 && (!id||!libraryLoad(id))){respond(false,"Bibliothekseintrag fehlt.",400);return;}
  if(type==2 && (!id||!rb42ReadCollection(id)||rb42MissingCount())){respond(false,"Programm fehlt oder muss zuerst repariert werden.",400);return;}
  if(type==3 && (id<1||id>7)){respond(false,"Ambiente-Farbe muss 1 bis 7 sein.",400);return;}
  if(type==0){id=0;label="";} // Deleting a shortcut removes its label as well.
  RB42Shortcuts next=rb42Shortcuts;
  next.types[slot-1]=type;next.ids[slot-1]=id;
  snprintf(next.labels[slot-1],48,"%s",label.c_str());
  if(!rb42Write(String("/rb-lib/shortcuts2.bin"),next)){respond(false,"Kurzwahl konnte nicht gespeichert werden.",500);return;}
  rb42Shortcuts=next;
  // Preserve compatibility with the old v0.4.1 journal and backup.
  LibraryMeta updated=libraryMeta;
  updated.shortcuts[slot-1]=(type==1)?id:0;
  if(!libraryWriteMeta(updated)){respond(false,"Kurzwahl gespeichert, altes Metadatenjournal nicht synchron.",500);return;}
  respond(true,"Kurzwahl gespeichert.");
}
void v042RunShortcut(){
  if(!libraryAvailable()||!reserveJob())return;
  int slot=web.arg("slot").toInt();
  if(slot<1||slot>6){respond(false,"Kurzwahl fehlt.",400);return;}
  uint8_t type=rb42Shortcuts.types[slot-1];uint32_t id=rb42Shortcuts.ids[slot-1];
  if(type==2){if(!rb42ReadCollection(id)){respond(false,"Programm beschädigt.",404);return;}
    if(rb42MissingCount()){respond(false,"Programm unvollständig. Bitte zuerst reparieren.",409);return;}
    rb42ProgramPlaying=true;autoProgram=false;rb42ProgramId=id;rb42Step=0;rb42ProgramSeconds=rb42Collection.seconds;
    rb42NextMs=0;rb42ProgramSpeed=pendingSpeed;
    rb43Playback=rb42Collection.reserved==1;rb43Step=0;rb43Page=0;rb43FirstGroup=true;
    if(!rb42QueueStep()){rb42ProgramPlaying=false;respond(false,"Programm nicht darstellbar.",500);return;}
    respond(true,"Programm über Kurzwahl gestartet.",202);return;}
  if(type==3){rb42ProgramPlaying=false;autoProgram=false;pendingColor=id;
    nextJob=AMBIENCE;currentState="queued";respond(true,"Ambiente per Kurzwahl gestartet.",202);return;}
  if(type==1 && libraryLoad(id)){
    if(libraryItem.layout==5){
      if(!rb42ReadBlock(id)){respond(false,"Blockfolge beschädigt.",404);return;}
      rb42BandCount=rb42Block.count;
      for(uint8_t i=0;i<rb42BandCount;++i)rb42Band[i]=rb42Block.pages[i];
      nextJob=BAND;
    }else{
      pendingText=libraryItem.text;pendingSecondLine=libraryItem.line2;
      pendingLayout=libraryItem.layout;pendingColor=libraryItem.color1;pendingColor2=libraryItem.color2;
      pendingMode=libraryItem.mode;nextJob=TEXT;
    }
    rb42ProgramPlaying=false;autoProgram=false;currentState="queued";
    respond(true,"Kurzwahl wird an die Tafel übertragen.",202);return;
  }
  respond(false,"Kurzwahl ist nicht belegt oder Inhalt fehlt.",404);
}

void v042SendDraftBand() {
  if(!reserveJob())return;
  if(pageCount<1||pageCount>RB42_BAND_LIMIT){respond(false,"Ungültige Blockfolge.",400);return;}
  rb42BandCount=pageCount;
  for(uint8_t i=0;i<pageCount;++i)rb42Band[i]=boardPages[i];
  rb42ProgramPlaying=false;autoProgram=false;
  nextJob=BAND;currentState="queued";
  respond(true,"Gesamte Blockfolge wird mit einem Upload gesendet.",202);
}
void v042CloneBlock() {
  if(!libraryAvailable())return;
  uint32_t id=libraryRequestId();
  String name=web.arg("name");
  if(!id||!name.length()||name.length()>=48 || libraryMeta.count>=LIB_LIMIT ||
    libraryMeta.nextId==0xFFFFFFFFUL || LittleFS.totalBytes()-LittleFS.usedBytes()<65536UL) {
    respond(false,"Ungültiger Name oder zu wenig Flashreserve.",400);return;
  }
  if(!libraryLoad(id)||libraryItem.layout!=5||!rb42ReadBlock(id)){
    respond(false,"Blockfolge nicht vorhanden.",404);return;
  }
  uint32_t target=libraryMeta.nextId;
  rb42Block.id=target;
  if(!rb42Write(rb42BlockPath(target),rb42Block)){respond(false,"Blockkopie konnte nicht gespeichert werden.",500);return;}
  libraryItem.id=target;snprintf(libraryItem.name,48,"%s",name.c_str());
  libraryItem.checksum=libraryDigest(libraryItem);
  if(!libraryWriteRecord(libraryItem)){respond(false,"Blockkopie-Metadaten konnten nicht gespeichert werden.",500);return;}
  LibraryMeta next=libraryMeta;next.ids[next.count++]=target;++next.nextId;
  if(!libraryWriteMeta(next)){respond(false,"Kopie geschrieben, Index konnte nicht aktualisiert werden.",500);return;}
  web.send(200,"application/json; charset=utf-8",String("{\"ok\":true,\"id\":")+target+",\"message\":\"Blockfolge dupliziert.\"}");
}
