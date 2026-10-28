# Tests RavestickBoard v0.4.2-dev

**Diese Tests sind Host- und Browserprüfungen, keine reale ESP32-Kompilierung und kein Test auf der CoolLEDX-Tafel.**

```bash
g++ -std=c++17 -O2 tests/v042_program_core_test.cpp -o /tmp/rb042_test
/tmp/rb042_test
python tests/browser_v042_smoke.py
python tools/sync_web.py
```

Der C++-Test prüft Seite/Verkettungsgrenzen, RGB-Ebenen, Tile-Streaming und Programmplanung anhand eines portablen Kernmodells. Der Browser-Mocktest prüft Webnavigation, Textspeichern, Blockfolge, Programmspeichern und Kurzwahlen. Er testet simulierte Serverantworten, **nicht** die reale LittleFS- oder BLE-Anbindung. Die frühere Regression v0.4.1 liegt weiterhin unter `library_host_test.cpp`, `browser_library_test.py` und `stream_renderer_host_test.cpp`.

Für die Hardwareabnahme siehe `docs/TESTPLAN_v0.4.2.md`.
