#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
using std::min; using std::max;
// v0.6.1.0: Attrappe fuer fs::File (der Animationsblock des Sketches nutzt sie)
struct File {
  const uint8_t *data = nullptr; size_t len = 0, pos = 0;
  explicit operator bool() const { return data != nullptr; }
  bool seek(size_t p) { if (!data || p > len) return false; pos = p; return true; }
  int read(uint8_t *d, size_t n) { if (!data) return -1; size_t k = pos + n <= len ? n : len - pos; std::memcpy(d, data + pos, k); pos += k; return (int)k; }
  void close() { data = nullptr; len = pos = 0; }
};
#define PROGMEM
#define pgm_read_byte(x) (*(const uint8_t *)(x))
struct String{
  std::string s;
  String(){} String(const char*c):s(c){} String(const std::string&c):s(c){}
  size_t length()const{return s.size();} const char*c_str()const{return s.c_str();}
  char operator[](size_t i)const{return s[i];}
  String operator+(const String&o)const{return String(s+o.s);}
};
static constexpr uint16_t SIGN_WIDTH=64,SIGN_HEIGHT=16;
static constexpr uint32_t MAX_IMAGE_BYTES=30UL*1024UL,IMAGE_HEADER_BYTES=107,BYTES_PER_COLUMN=6;
static constexpr uint32_t MAX_COLUMNS=(MAX_IMAGE_BYTES-IMAGE_HEADER_BYTES)/BYTES_PER_COLUMN;
static constexpr size_t STREAM_THRESHOLD_BYTES=8192;
static constexpr uint16_t STREAM_TILE_COLUMNS=64;
static uint8_t streamHeader[IMAGE_HEADER_BYTES]={};
static uint8_t streamTile[STREAM_TILE_COLUMNS*BYTES_PER_COLUMN]={};
static bool streamPayload=false,streamDrawingTile=false;
static uint16_t streamImageWidth=0,streamTileStart=0,streamTileCols=0,streamCachedTile=0xFFFF;
static const String *streamFirstLine=nullptr,*streamSecondLine=nullptr;
static uint8_t streamLayout=0,streamFirstColor=7,streamSecondColor=7;
static uint32_t streamFirstPixels=0,streamSecondPixels=0,streamTileRenders=0;
uint8_t *pixelData=nullptr,*textPayload=nullptr;
uint8_t pendingLayout=0;
struct TextMeasure { uint32_t chars; uint32_t pixels; };
struct TextStyle { uint8_t baseColor; uint8_t color; uint8_t baseLayout; uint8_t layout; };
enum RamStage { RT_RENDER_BEGIN,RT_BUFFER_ALLOC };
void ramTraceMark(RamStage,uint16_t){}
struct { template<typename... A> void printf(const char*,A...){} } Serial;
static bool rb43StreamBand=false,rb42StreamBand=false;
void rb43CopyMixedSlice(size_t,size_t,uint8_t*){}
void rb42CopyBandSlice(size_t,size_t,uint8_t*){}
static bool allocateRenderBuffer(uint32_t rgb){ if(textPayload)free(textPayload); textPayload=(uint8_t*)calloc(IMAGE_HEADER_BYTES+rgb,1); pixelData=textPayload+IMAGE_HEADER_BYTES; return true; }

