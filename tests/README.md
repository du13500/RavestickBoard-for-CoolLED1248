# Offline-Tests – v0.4.2.5.4-dev

- `python tests/test_build_4253.py`: prüft Version, Schema-Konstanten, gzip/ETag, JavaScript/C++-Syntax und JSON-Chunks. Enthält Regressionen auf die sieben `<span>`-Menülabels, bereits gerenderte Kurzwahl-SVGs und den initialen Home-Zustand.
- `python tests/browser_hotfix_smoke.py`: prüft mit einem headless Chromium und simulierten lokalen APIs die sichtbare Startansicht, den Ladezustand, das Hauptmenü und den Wechsel Home ↔ Texteditor.

Der Browser-Test erfordert Playwright und Chromium; Arduino-Boardpaket und reale Hardware werden dadurch nicht ersetzt. Insbesondere wird die kritische RAM-Untergrenze hier **nicht** getestet oder als behoben behauptet.

- `python tests/browser_8_shortcuts.py`: prüft alle acht Kurzwahlen, Bibliothek ohne Favoritensterne, Geschwindigkeitsregler sowie adaptive Höhe auf sechs Bildschirmgrößen.

## Aktuelle v0.4.2.5.4-Prüfungen

`python tests/test_build_4254.py`: statische Verträge, JavaScript-Syntax, gzip, ETag, C++-Header und JSON-Writer-Stub.

`python tests/test_browser_save_as_4254.py`: Simulierter Browser prüft Text- und Blockkopie gegen vorgetäuschte API, dass Originale unverändert bleiben, Varianten neue IDs erhalten, ein Schriftartwechsel die Vorlage nicht ablöst und Änderungen an Kopien nicht das Original überschreiben.

`python tests/browser_8_shortcuts.py`: responsives Layout mit acht Kurzwahlen und Reglern.

**Nicht enthalten:** Arduino-Compile oder ESP32-Hardwarelasttest.
