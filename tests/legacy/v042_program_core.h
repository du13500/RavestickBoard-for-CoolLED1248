// RavestickBoard v0.4.2 independent, host-testable program packing core.
// Integration into the Arduino/ESP32 firmware is pending. This header is NOT
// an Arduino sketch. Copyright 2026 RavestickBoard contributors, MIT.
#pragma once
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <vector>
#include <string>
#include <stdexcept>

namespace ravestick {
constexpr unsigned kWidth = 64;
constexpr unsigned kHeight = 16;
constexpr unsigned kBytesPerColumn = 6; // R/G/B: 2 bytes per column
constexpr unsigned kPageBytes = kWidth * kBytesPerColumn;
constexpr unsigned kHeaderBytes = 107;
constexpr unsigned kBudgetBytes = 30 * 1024;
constexpr unsigned kPagesPerEditor = 25;
constexpr unsigned kMaxBandPages = (kBudgetBytes - kHeaderBytes) / kPageBytes;

enum class Kind : uint8_t { Block = 1, Scroll = 2 };
struct Page {
  std::string line1, line2;
  uint8_t layout = 0, alignment = 1, color1 = 7, color2 = 7;
};
struct Entry {
  uint32_t id = 0;
  std::string name;
  Kind kind = Kind::Block;
  uint8_t effect = 1;
  std::vector<Page> pages;
  std::string text;
};
struct Step {
  uint32_t entryId = 0;
  Kind kind = Kind::Block;
  uint8_t effect = 1;
};
struct Group {
  Kind kind = Kind::Block;
  uint8_t effect = 1;
  std::vector<Step> steps;
  unsigned pages = 0;
};

// Side effects (BLE and filesystem) are deliberately outside this planner.
// For block entries the caller provides their page count. Groups never cut a
// 64-pixel-wide page. Different effects are never accidentally coalesced.
template <class Lookup>
std::vector<Group> plan(const std::vector<Step> &steps, Lookup lookup) {
  std::vector<Group> groups;
  for (const Step &step : steps) {
    unsigned n = lookup(step.entryId, step.kind);
    if (!n) throw std::invalid_argument("missing/empty program entry");
    if (step.kind == Kind::Scroll) n = 1; // byte-budget check at render time
    if (step.kind == Kind::Block && n > kMaxBandPages)
      throw std::length_error("single block entry exceeds budget");
    bool fresh = groups.empty() || groups.back().kind != step.kind ||
      groups.back().effect != step.effect ||
      (step.kind == Kind::Block && groups.back().pages + n > kMaxBandPages);
    if (fresh) groups.push_back(Group{step.kind, step.effect, {}, 0});
    groups.back().steps.push_back(step);
    groups.back().pages += n;
  }
  return groups;
}

// Framebuffer layout follows the existing six-bytes-per-column CoolLEDX image
// representation: all red columns, then green, then blue, each 2-byte tall.
// Rendering callbacks supply one complete 64x16 page in planar arrangement.
template <class RenderPage>
void packBlockBand(unsigned count, RenderPage render, std::vector<uint8_t>& rgb) {
  if (count == 0 || count > kMaxBandPages)
    throw std::length_error("page count exceeds one upload");
  const unsigned wholeWidth = count * kWidth;
  const unsigned plane = wholeWidth * 2;
  rgb.assign(wholeWidth * kBytesPerColumn, 0);
  uint8_t page[kPageBytes] = {};
  for (unsigned i = 0; i < count; ++i) {
    std::memset(page, 0, sizeof(page));
    render(i, page);
    for (unsigned color=0; color<3; ++color) {
      const unsigned src = color * kWidth * 2;
      const unsigned dst = color * plane + i * kWidth * 2;
      std::memcpy(rgb.data()+dst, page+src, kWidth*2);
    }
  }
}

// A caller can stream packBlockBand without allocating the full vector by
// rendering the selected page and extracting the correct color plane on demand.
template <class RenderPage>
uint8_t blockBandByte(unsigned count, unsigned offset, RenderPage render) {
  if (count == 0 || count > kMaxBandPages ||
      offset >= count*kPageBytes) throw std::out_of_range("pixel offset");
  const unsigned fullPlane = count * kWidth * 2;
  const unsigned channel = offset / fullPlane;
  const unsigned planeOffset = offset % fullPlane;
  const unsigned pageIndex = planeOffset / (kWidth*2);
  const unsigned localOffset = planeOffset % (kWidth*2);
  uint8_t page[kPageBytes] = {};
  render(pageIndex, page);
  return page[channel*kWidth*2+localOffset];
}

// Explicit user-visible labels are stored independently of the target title.
enum class Source : uint8_t { Empty=0, Library=1, Program=2, Ambience=3, Animation=4 };
struct Shortcut {
  Source source=Source::Empty;
  uint32_t id=0;
  std::string label;
};
inline bool shortcutValid(const Shortcut& s) {
  return s.source <= Source::Animation && s.label.size() <= 47 &&
      (s.source==Source::Empty || s.source==Source::Ambience || s.id!=0) &&
      (s.source!=Source::Ambience || (s.id>=1 && s.id<=7));
}
} // namespace ravestick
