#include <bits/stdc++.h>
using namespace std;
using String = std::string;
#define PROGMEM
#define pgm_read_byte(x) (*(const uint8_t *)(x))
#include "../firmware/RavestickBoard_v0_4_0_3/RavestickBoard_Assets.h"
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
struct TextMeasure { uint32_t chars,pixels; };
enum RamStage { RT_RENDER_BEGIN,RT_BUFFER_ALLOC };
void ramTraceMark(RamStage,uint16_t){}
struct { template<typename... Args> void printf(const char *,Args...){} } Serial;
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
    x0 += tokenWidth(cp, layout) + tokenGap(cp);
  }
}

// Only draw tokens intersecting the current 64-column viewport. The pixel
// painter and all glyph/symbol rules are copied from drawInlineUtf8 above;
// only clipping and the pre-measured width differ.
void drawInlineUtf8Window(const String &text, uint8_t layout, uint8_t align,
                          uint8_t top, uint8_t color, uint16_t width, uint32_t measuredPixels) {
  if (layout == 2) align = 1;
  int x0 = align == 0 ? 0 : align == 2 ? int(width) - int(measuredPixels) : (int(width) - int(measuredPixels)) / 2;
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


static size_t tests=0,failures=0;
void check(const String &t1,const String &t2,uint8_t layout,uint8_t color1,uint8_t color2) {
 const TextMeasure m1=measureText(t1,layout),m2=layout==1?measureText(t2,1):TextMeasure{0,0};
 size_t width=max<size_t>(SIGN_WIDTH,max(m1.pixels,m2.pixels));
 if(width>MAX_COLUMNS || !m1.chars || width<1) return;
 size_t total=IMAGE_HEADER_BYTES+width*BYTES_PER_COLUMN;
 vector<uint8_t> expected(total,0);
 pixelData=expected.data()+IMAGE_HEADER_BYTES;
 if(layout==1) {
   drawInlineUtf8(t1,1,0,0,color1,width);
   drawInlineUtf8(t2,1,0,8,color2,width);
 } else drawInlineUtf8(t1,layout,1,layout==0?1:layout==2?4:0,color1,width);
 expected[24]=min((uint32_t)80,m1.chars+m2.chars);
 for(int i=0;i<expected[24];i++)expected[25+i]=0x30;
 expected[105]=(width*6)>>8;
 expected[106]=(width*6)&255;
 pixelData=nullptr;
 size_t streamTotal=prepareStreamText(t1,layout==1?&t2:nullptr,layout,color1,color2,width,m1.chars,m2.chars,m1.pixels,m2.pixels);
 if(streamTotal!=total) {cerr<<"Mismatch length "<<total<<" "<<streamTotal<<"\n";exit(1);}
 vector<uint8_t> got(total,0);
 size_t chunkLengths[]={1,3,7,32,64,128,197};
 for(size_t size:chunkLengths) {
   streamCachedTile=0xFFFF;
   for(size_t pos=0;pos<total;pos+=size)copyPayloadSlice(pos,min(size,total-pos),got.data()+pos);
   if(got!=expected) {
     for(size_t i=0;i<total;i++)if(got[i]!=expected[i]) {
       cerr<<"FAIL layout="<<(int)layout<<" chars="<<m1.chars<<" width="<<width<<" chunk="<<size<<" pos="<<i<<" expect="<<(int)expected[i]<<" got="<<(int)got[i]<<"\n";exit(2);
     }
   }
 }
 streamPayload=false;streamDrawingTile=false;streamFirstLine=streamSecondLine=nullptr;
 ++tests;
}
int main() {
 vector<String> examples={"SUCHE JOINT", "BIETE QUACK 🦆", "🍌 RAVE ❤️ 💛 💚 🩵 💙 💜 🤍", "🎉🌹🌸🐝🦀🚬🌿", "ÄÖÜ äöü ß ß 1234567890 XYZ", "A     A", "😊 🤔 ♫ ✨", "🥚🐸🍭"};
 for(auto &str:examples)for(uint8_t l=0;l<5;l++)for(uint8_t c=1;c<=7;c+=3)check(str,"JOINT 🍌",l,c,7);
 string long1;for(int i=0;i<330;i++)long1+="ALLES ";
 string long2;for(int i=0;i<320;i++)long2+="🦆QUACK ";
 for(uint8_t l=0;l<5;l++)check(long1,"ZU WEIT",l,3,6);
 for(uint8_t l=0;l<5;l++)check(long2,"ZU WEIT",l,5,7);
 for(int n:{40,150,400,850})for(uint8_t l=0;l<5;l++) {
   string txt;for(int i=0;i<n;i++)txt+=(i%17==0?"🍌":i%31==0?"🦆":i%7==0?"♥":"R");
   check(txt,"RARESTICK",l,7,4);
 }
 cerr<<"PASS "<<tests<<" byte-exact tiled image test cases against full renderer, at 7 read chunk sizes\n";
 return 0;
}
