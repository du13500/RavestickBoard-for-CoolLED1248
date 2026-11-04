// Treiber: liest Befehle (Tab-getrennt) von stdin, ruft die echten Routen aus RavestickBoard_Anim.h auf.
static std::vector<std::string> splitTab(const std::string &l) { std::vector<std::string> v; size_t a = 0; for (;;) { size_t b = l.find('\t', a); if (b == std::string::npos) { v.push_back(l.substr(a)); break; } v.push_back(l.substr(a, b - a)); a = b + 1; } return v; }
static void hexOut(const uint8_t *d, size_t n) { for (size_t i = 0; i < n; ++i) std::printf("%02x", d[i]); std::printf("\n"); }
int main() {
  std::string line;
  while (std::getline(std::cin, line)) {
    if (line.empty()) continue;
    auto t = splitTab(line);
    const std::string &c = t[0];
    if (c == "BEGIN") { rb61AnimBegin(); std::printf("ready=%d\n", (int)rb61Ready); }
    else if (c == "REQ") {
      web.args.clear(); web.out.clear(); web.status = 0; lastHttpStatus = 0;
      for (size_t i = 2; i < t.size(); ++i) { size_t e = t[i].find('='); web.args[t[i].substr(0, e)] = t[i].substr(e + 1); }
      const std::string &r = t[1];
      if (r == "list") onAnimList(); else if (r == "get") onAnimGet(); else if (r == "put") onAnimPut();
      else if (r == "send") onAnimSend(); else if (r == "rename") onAnimRename();
      else if (r == "delete") onAnimDelete(); else { std::printf("?route\n"); continue; }
      std::printf("STATUS %d\n%s\n", web.status ? web.status : lastHttpStatus, web.out.c_str());
    }
    else if (c == "STATE") { currentState = t[1].c_str(); }
    else if (c == "MOUNT") { libraryMounted = t[1] == "1"; if (!libraryMounted) rb61Ready = false; }
    else if (c == "TOTAL") { fsTotal = (size_t)std::atol(t[1].c_str()); }
    else if (c == "FAILWRITES") { fsFailWrites = t[1] == "1"; }
    else if (c == "TMPFILE") { auto n = std::make_shared<Node>(); n->bytes.assign(10, 7); fsFiles[t[1]] = n; fsDirs.insert("/rb-anim"); }
    else if (c == "FS") { for (auto &kv : fsFiles) std::printf("%s %zu\n", kv.first.c_str(), kv.second->bytes.size()); std::printf("END\n"); }
    else if (c == "POKE") { auto &b = fsFiles[t[1]]->bytes; b[(size_t)std::atol(t[2].c_str())] = (uint8_t)std::atoi(t[3].c_str()); }
    else if (c == "TRUNC") { fsFiles[t[1]]->bytes.resize((size_t)std::atol(t[2].c_str())); }
    else if (c == "PREP") {
      const size_t n = rb61PrepareAnimFile((uint16_t)std::atoi(t[1].c_str()));
      std::printf("LEN %zu %d %u %u %s\n", n, (int)rb60AnimActive, (unsigned)rb60AnimFrames, (unsigned)rb60AnimMs, rb61AnimName);
    }
    else if (c == "SLICE") {
      const size_t off = (size_t)std::atol(t[1].c_str()), len = (size_t)std::atol(t[2].c_str());
      std::vector<uint8_t> d(len); rb60CopyAnimSlice(off, len, d.data()); hexOut(d.data(), len);
    }
    else if (c == "CLEAN") { rb60AnimActive = false; if (rb61AnimFile) rb61AnimFile.close(); std::printf("closed=%d\n", (int)!rb61AnimFile); }
    else if (c == "VARS") { std::printf("job=%d pending=%d autoProgram=%d prog=%d id=%u state=%s\n", (int)nextJob, (int)pendingMode, (int)autoProgram, (int)rb42ProgramPlaying, (unsigned)rb61PendingAnimId, currentState.c_str()); }
    else if (c == "EVENTS") { for (auto &e : events) std::printf("%s\n", e.c_str()); std::printf("END\n"); }
    else if (c == "DROP") { rb61DropTemp(); std::printf("pending=%u\n", (unsigned)rb61PendingAnimId); }
    else if (c == "EXISTS") { std::printf("%d\n", (int)rb61AnimExists((uint32_t)std::atol(t[1].c_str()))); }
    else if (c == "QUEUE") { const bool q = rb61QueueAnim((uint32_t)std::atol(t[1].c_str())); std::printf("%d job=%d pending=%u state=%s\n", (int)q, (int)nextJob, (unsigned)rb61PendingAnimId, currentState.c_str()); }
    else if (c == "RENAMEOVERWRITE") { fsRenameOverwrite = t[1] == "1"; }
    else if (c == "FAILRENAME") { fsRenameCalls = 0; fsFailRenameFrom = std::atoi(t[1].c_str()); fsFailRenameTo = std::atoi(t[2].c_str()); }
    else if (c == "FAILREAD") { fsFailReadPath = t[1]; fsFailReadAfter = std::atoi(t[2].c_str()); }
    else if (c == "COPY") { auto n = std::make_shared<Node>(*fsFiles[t[1]]); fsFiles[t[2]] = n; }
    else if (c == "DEL") { fsFiles.erase(t[1]); }
    else if (c == "BYTES") {   // Bytes ab Position aus einer Datei als Hex
      auto &b = fsFiles[t[1]]->bytes; const size_t o = (size_t)std::atol(t[2].c_str()), n = (size_t)std::atol(t[3].c_str());
      hexOut(b.data() + o, n);
    }
    else if (c == "READFAILED") { std::printf("%d\n", (int)rb61ReadFailed); }
    else if (c == "CRC") {   // CRC-32 der Bytes (Hex) mit der Firmware-Funktion
      std::vector<uint8_t> b; for (size_t i = 0; i + 1 < t[1].size(); i += 2) b.push_back((uint8_t)std::stoi(t[1].substr(i, 2), nullptr, 16));
      std::printf("%u\n", (unsigned)~rb61Crc32Update(0xFFFFFFFFUL, b.data(), b.size()));
    }
    else if (c == "SETSC") {   // Kurzwahl direkt setzen: Platz (0-7), Typ, Kennung, Beschriftung
      const int i = std::atoi(t[1].c_str()); rb42Shortcuts.types[i] = (uint8_t)std::atoi(t[2].c_str());
      rb42Shortcuts.ids[i] = (uint32_t)std::atol(t[3].c_str()); std::snprintf(rb42Shortcuts.labels[i], 48, "%s", t.size() > 4 ? t[4].c_str() : "");
    }
    else if (c == "DAMAGED") { rb42ShortcutsDamaged = t[1] == "1"; }
    else if (c == "SC") {   // Kurzwahlen im Speicher und aus der geschriebenen Datei
      for (int i = 0; i < 8; ++i) std::printf("%d:%u:%s ", (int)rb42Shortcuts.types[i], (unsigned)rb42Shortcuts.ids[i], rb42Shortcuts.labels[i]);
      std::printf("\n");
    }
    else if (c == "SCFILE") {
      auto it = fsFiles.find(RB42_SHORTCUT_FILE); RB42Shortcuts f = {};
      if (it == fsFiles.end() || it->second->bytes.size() != sizeof(f)) { std::printf("none\n"); }
      else { std::memcpy(&f, it->second->bytes.data(), sizeof(f)); for (int i = 0; i < 8; ++i) std::printf("%d:%u ", (int)f.types[i], (unsigned)f.ids[i]); std::printf("\n"); }
    }
    else if (c == "B64") {   // Rundreise Base64: Hexstring rein -> kodiert -> dekodiert
      std::vector<uint8_t> b; for (size_t i = 0; i + 1 < t[1].size(); i += 2) b.push_back((uint8_t)std::stoi(t[1].substr(i, 2), nullptr, 16));
      std::string e(b.size() * 2 + 8, 0); size_t n = rb61B64Encode(b.data(), b.size(), &e[0]);
      std::vector<uint8_t> d(b.size() + 8); int m = rb61B64Decode(e.c_str(), n, d.data());
      std::printf("%s %d\n", e.c_str(), m);
    }
    std::fflush(stdout);
  }
  return 0;
}
