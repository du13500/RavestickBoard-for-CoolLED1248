# Mitmachen bei RavestickBoard 🍌

Danke, dass du RavestickBoard verbessern möchtest. Wir entwickeln den Offline-Webcontroller gemeinsam und transparent mit Unterstützung von ChatGPT und Claude; reale Hardwaretests sind von lokalem Quellcode-Test zu unterscheiden.

## Bevor du etwas änderst

Lies die aktuelle [README](README.md), [Roadmap](docs/ROADMAP.md), [Themenspeicher](docs/THEMENSPEICHER.md) und den [Testplan](docs/TESTPLAN_v0.4.2.4.md). Verwendete Hardware: ESP32-C3 Super Mini + CoolLEDX 64×16. Bitte keine Herstellerkompatibilität behaupten, die wir noch nicht getestet haben.

Ein Fehlerbericht ist besonders hilfreich mit Firmwareversion, Browser/Endgerät, ESP32-Boardpaket, konkreten Schritten, erwartetem/tatsächlichem Verhalten und – wenn sinnvoll – dem exportierten Diagnose-/RAMTRACE-Log. **Keine Passwörter, privaten BLE-Adressen oder persönlichen Daten veröffentlichen.**

## Beiträge zum Code

- Neue Firmware als **neuen Versionsordner** `firmware/RavestickBoard_v...` anlegen. Alte lauffähige Versionen bleiben unverändert.
- Das Webinterface in `web/index.html` ändern und mit `python tools/sync_web.py` in `RavestickBoard_WebUI.h` synchronisieren.
- LittleFS/NVS-Binärformate, Datensatz-IDs, Sicherung und Partitionierung nicht ohne Migration und dokumentierte Tests ändern.
- Externe Icons/Assets mit korrekter Lizenz und Herkunft in `THIRD_PARTY_NOTICES.md` kennzeichnen. Projektcode steht unter MIT; fremde Inhalte sind nicht automatisch MIT.
- Tests nach Möglichkeit lokal durchführen und ehrlich dokumentieren, was **nicht** auf der echten Tafel geprüft wurde. Besonders bei BLE-Paketgrenzen, RAM, Unterbrechungen und mehrfachen Clients ist ein Hardwaretest entscheidend.
- Pull Requests mit Ziel, Änderung, Risiken, Testschritten sowie Dokumentations- und Lizenzfolgen beschreiben.

Wir freuen uns über hilfreiche Korrekturen, nachvollziehbare Hardwaremessungen und kleine, klar abgegrenzte Verbesserungsvorschläge – **hugs and love**. 🍌
