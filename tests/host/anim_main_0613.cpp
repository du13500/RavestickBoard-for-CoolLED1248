
static void hex(const uint8_t *d, size_t n) { for (size_t i = 0; i < n; ++i) std::printf("%02x", d[i]); std::printf("\n"); }
static int hexval(char c) { return c <= '9' ? c - '0' : (c | 32) - 'a' + 10; }
int main(int argc, char **argv) {
  if (argc >= 5 && !std::strcmp(argv[1], "anim")) {
    const uint8_t pattern = (uint8_t)std::atoi(argv[2]);
    const uint8_t frames = (uint8_t)std::atoi(argv[3]);
    const uint16_t ms = (uint16_t)std::atoi(argv[4]);
    const size_t payloadLen = rb60PrepareAnim(pattern, frames, ms);
    std::printf("LEN %zu\n", payloadLen);
    if (!payloadLen) return 0;
    const size_t chunks = (payloadLen + DATA_CHUNK - 1) / DATA_CHUNK;
    uint8_t raw[140], frame[300];
    for (size_t i = 0; i < chunks; ++i) {
      const size_t rawN = makeChunk(i, payloadLen, raw);
      const size_t frameN = framePacket(raw, rawN, frame);
      assert(frameN <= sizeof(frame));
      hex(frame, frameN);
    }
    return 0;
  }
  if (argc >= 3 && !std::strcmp(argv[1], "ack")) {
    for (int k = 2; k < argc; ++k) {
      uint8_t b[64]; size_t n = 0;
      for (const char *p = argv[k]; p[0] && p[1] && n < sizeof b; p += 2) b[n++] = (uint8_t)(hexval(p[0]) * 16 + hexval(p[1]));
      uint8_t cmd = 0, st = 0; uint16_t idx = 0;
      if (rb60ParseAck(b, n, cmd, idx, st) == 0) std::printf("OK %u %u %u\n", cmd, idx, st);
      else std::printf("BAD\n");
    }
    return 0;
  }
  if (argc >= 3 && !std::strcmp(argv[1], "rejects")) {   // ungueltige Parameter
    std::printf("%zu %zu %zu %zu %zu %zu\n", rb60PrepareAnim(0, 7, 10), rb60PrepareAnim(1, 7, 0), rb60PrepareAnim(2, 80, 10),
                rb60PrepareAnim(2, 1, 10), rb60PrepareAnim(3, 81, 10), rb60PrepareAnim(3, 80, 10));
    return 0;
  }
  return 2;
}
