# Tests für v0.4.2.4-dev

```bash
python tests/source_invariants_v0424.py
python tests/browser_v0424_smoke.py
python tools/sync_web.py
```

Die Browser-Tests arbeiten mit simulierten ESP32-HTTP-Antworten. Sie **ersetzen keine Arduino-Kompilierung oder Hardwaretests**. Die eigentliche neue BLE-Wiederholung, das Streaming und die Zeitsteuerung müssen gemäß `docs/TESTPLAN_v0.4.2.4.md` auf der CoolLEDX geprüft werden.
