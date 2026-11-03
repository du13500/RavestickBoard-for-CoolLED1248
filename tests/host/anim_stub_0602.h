// Attrappen fuer den Hosttest des Animationsblocks (v0.6.0.0).
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <string>
#include <algorithm>
using std::min;
static constexpr uint16_t SIGN_WIDTH = 64;
static constexpr uint16_t SIGN_HEIGHT = 16;
static constexpr uint32_t BYTES_PER_COLUMN = (SIGN_HEIGHT / 8) * 3;
static constexpr size_t DATA_CHUNK = 128;
