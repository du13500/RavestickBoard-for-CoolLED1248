# Host-Differenztest für den neuen Stream-Renderer

Der C++-Testcode enthält den ursprünglichen Renderer und den Kachelrenderer mit Daten aus den echten Firmware-Assets. Er vergleicht die **107-Byte-Header und alle drei Farbebenen Byte für Byte**. Die Testfälle umfassen 5 Layouts, sieben Chunkgrößen, Symbole und Unicode, einzeilige und zweizeilige sowie sehr lange Texte.

Unter Linux/WSL mit g++:

```bash
cd tests
g++ -std=c++17 -O2 -I. -o stream_renderer_test stream_renderer_host_test.cpp
./stream_renderer_test
```

Erwartung: `PASS 135 byte-exact tiled image test cases against full renderer, at 7 read chunk sizes`.

Die Testdatei ist eine hostseitige Kopie der relevanten Rendering-Funktionen, kein Ersatz für die Firmwarekompilierung und **kein Hardware-Nachweis** für BLE-Verbindungen und Laufzeit. Alle Prüfergebnisse beziehen sich nur auf diese Version.

## v0.4.1-dev – Library host/browser regression

- `g++ -std=c++17 -Itests/mock tests/library_host_test.cpp -o tests/library_host_test && tests/library_host_test`: **PASS** (simuliertes LittleFS; CRUD, CRC, Journal, Neustart, Wiederherstellung und Kurzwahl-Löschschutz). Der Test nutzt absichtlich Mocks und ersetzt keine native ESP32-Kompilierung.
- `python tests/browser_library_test.py`: **PASS** (Headless Chromium/Playwright mit simulierten HTTP-Antworten, Editor- und Bibliotheks-Interaktion, Kurzwahl, Duplizieren, aktualisiertes Speichern).
- `node` JavaScript-Syntaxprüfung des in `web/index.html` eingebetteten Scripts: **PASS**.
- `firmware/.../RavestickBoard_WebUI.h` enthält HTML aus `web/index.html` byteidentisch: **PASS**.

**Offen:** Arduino-ESP32-Kompilierung, echte LittleFS-Dateisystempartition, BLE-Hardware, Transfer und Dauertest.
