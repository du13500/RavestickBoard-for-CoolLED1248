# Offline-Tests – v0.4.2.5.1-dev

- `python tests/test_build_4251.py`: prüft Version, Schema-Konstanten, gzip/ETag, JavaScript/C++-Syntax und JSON-Chunks. Enthält Regressionen auf die sieben `<span>`-Menülabels, bereits gerenderte Kurzwahl-SVGs und den initialen Home-Zustand.
- `python tests/browser_hotfix_smoke.py`: prüft mit einem headless Chromium und simulierten lokalen APIs die sichtbare Startansicht, den Ladezustand, das Hauptmenü und den Wechsel Home ↔ Texteditor.

Der Browser-Test erfordert Playwright und Chromium; Arduino-Boardpaket und reale Hardware werden dadurch nicht ersetzt. Insbesondere wird die kritische RAM-Untergrenze hier **nicht** getestet oder als behoben behauptet.
