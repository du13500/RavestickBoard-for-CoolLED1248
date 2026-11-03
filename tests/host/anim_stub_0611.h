// Attrappen fuer den Hosttest des Animationsblocks (v0.6.0.0).
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <string>
#include <algorithm>
using std::min;
// v0.6.1.0: Attrappe fuer fs::File (nur was der Animationsblock braucht: Datei im Speicher, seek/read/close)
struct File {
  const uint8_t *data = nullptr; size_t len = 0, pos = 0;
  explicit operator bool() const { return data != nullptr; }
  bool seek(size_t p) { if (!data || p > len) return false; pos = p; return true; }
  int read(uint8_t *d, size_t n) { if (!data) return -1; size_t k = pos + n <= len ? n : len - pos; std::memcpy(d, data + pos, k); pos += k; return (int)k; }
  void close() { data = nullptr; len = pos = 0; }
};
static constexpr uint16_t SIGN_WIDTH = 64;
static constexpr uint16_t SIGN_HEIGHT = 16;
static constexpr uint32_t BYTES_PER_COLUMN = (SIGN_HEIGHT / 8) * 3;
static constexpr size_t DATA_CHUNK = 128;
