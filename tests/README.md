# Host- und Browsertests v0.4.2.2-dev

Neue Prüfungen:

```bash
python tests/browser_v0422_smoke.py
python tests/browser_v0422_layout.py
python tests/source_invariants_v0422.py
python tools/sync_web.py
```

Zusätzlich in der vollständigen Repository-Arbeitskopie (`g++`/`-std=c++17`) die vorhandenen Tests `stream_renderer_host_test.cpp` (135 Vergleiche) und `library_host_test.cpp` ausführen. **Browser-Smoke-Tests verwenden HTTP-Mocks**; die C++-Tests simulieren Renderer und Flash. Eine vollständige ESP32-Firmwarekompilierung und echte Bluetooth-/LittleFS-Läufe sind damit nicht ersetzt. Details im Hardwaretestplan `docs/TESTPLAN_v0.4.2.2.md`.
