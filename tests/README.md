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

## v0.4.3.1-dev (30.10.2026)

`python tests/test_0431.py` prüft die JavaScript-Syntax, HTML/gzip-/ETag-Konsistenz, die korrigierte Versionskennung, den entkoppelten Diagnose-TXT-Download und die interaktive Darstellung in drei Chromium-Mobilgrößen. Der Stream-Puffer wurde außerdem separat mit einem Host-C++-Test kompiliert. Dies ersetzt **keinen Arduino-Gesamtcompile** und keinen Heap-/BLE-Test auf dem ESP32.

## v0.4.3.4-dev (31.10.2026)

`python tests/test_build_0434.py`: wie `test_build_0433.py`, zusätzlich Invarianten zum Abmelden vor dem Wächter-Neustart (`esp_wifi_deauth_sta(0)` vor `abort()`, Spur vor Abmeldung, Flag in Spurtext), zum Test-Auslöser (Route, Blockade in `loop()`, Parameter `deauth`) und dazu, dass die Weboberfläche keinen Knopf dafür enthält. `python tests/test_0434.py [Screenshot-Ordner]`: Browser-Test der Programme-Ansicht (unverändert, nur Version). Der Wächter-/Abmeldecode wurde zusätzlich in einem einmaligen Host-Stubtest (g++) auf Ablauf geprüft; dieser Test liegt nicht im Repository. Kein Ersatz für Arduino-Compile und Hardwaretest.

## v0.4.3.3-dev (30.10.2026)

`python tests/test_build_0433.py`: Version, gzip/ETag, JavaScript-Syntax, Diagnose-Verträge, Mobilgrößen und Firmware-Invarianten zur RAM-Messung (Sketch-Größe nur ein Aufruf, `bleEnsureInit`, Standardwerte der Schalter, `softAP` mit Client-Limit, `[RAM]`-Marken, Export-Abschnitte) und zum Wächter (Wächter-Schwelle, RTC-Spur, Phasenmarken im BLE-Pfad, Anzeige des Vorlaufs). `python tests/test_0433.py [Screenshot-Ordner]`: Browser-Test der Programme-Ansicht (unverändert gegenüber 0.4.3.2, nur Version). Kein Ersatz für Arduino-Compile und Hardwaretest.

## v0.4.3.2-dev (30.10.2026)

`python tests/test_0432.py [Screenshot-Ordner]`: Browser-Test der Programme-Ansicht gegen eine simulierte API auf 360/390/430 px (Liste, Auswahl, Bearbeiten, Abbrechen/Verwerfen, Speichern, Neu, Löschen, defektes Programm, Werkzeugleiste einzeilig, kein horizontaler Überlauf). Benötigt Playwright und Chromium.

`python tests/test_build_0432.py`: Version, gzip/ETag, JavaScript-Syntax, Diagnose-Verträge und Mobilgrößen (Nachfolger von `test_0431.py`).

Hinweis: `tests/test_browser_save_as_4254.py` schlägt bereits gegen v0.4.3.1 fehl (Speichern-Auswahl wurde in v0.4.3 umgebaut) und ist nicht angepasst. Weiterhin gilt: kein Arduino-Compile, kein Hardware- oder Heap-Test.
