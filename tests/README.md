# RavestickBoard v0.4.2.3-dev – lokale Tests

```bash
python tests/source_invariants_v0423.py
python tests/browser_v0423_smoke.py
python tests/browser_v0423_layout.py
python tests/browser_v0423_icons.py
python tools/sync_web.py
```

Die Browsertests arbeiten mit simulierten ESP32-Antworten. Vorhandene
C++-Hosttests aus dem vollständigen Repository prüfen außerdem
Renderer und Bibliotheksformate, ersetzen aber **weder einen Arduino-Build**
noch echte BLE- oder LittleFS-Hardwaretests.

Neue Hardwaretests: `docs/TESTPLAN_v0.4.2.3.md`.
