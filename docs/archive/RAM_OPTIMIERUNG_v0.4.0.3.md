# RavestickBoard v0.4.0.3-dev · RAM-Optimierung und BLE-MTU-Recovery

## Ziel

Die Belastungsdiagnose v0.4.0.2 zeigte beim ca. 30,6-KiB-Lauftext (Job 26) ein System-Heap-Minimum von 3.808 Byte, einen beobachteten freien Heap von 20.924 Byte und einen größten freien Block von zeitweise 7.668 Byte. Während eines einzelnen anderen Jobs meldete die BLE-Bibliothek MTU=0 und die Übertragung wurde sicher beendet. Ziel ist eine geringere, vorhersehbare Heap-Nutzung mit identischem Pixelbild und unverändertem CoolLEDX-Protokoll.

## Änderung 1: 64-Spalten-Kachelrenderer für lange Texte

- Große Lauftexte ab 8.192 Byte Bildpayload werden nicht mehr vollständig im RAM abgelegt. Die 107-Byte-Kopfzeile wird separat in einem statischen Array gespeichert.
- Die drei Farbkanäle liegen wie bisher **planar** im BLE-Bildpayload: zuerst Rot, dann Grün, dann Blau; pro Kanal zwei Bytes je Spalte (16 Pixel). **Diese Reihenfolge wird nicht geändert.**
- Beim Zusammenbauen eines 128-Byte-Protokollpakets wird die gerade benötigte 64-Spalten-Kachel mit dem bisherigen Zeichensatz, den bisherigen Textbreiten und den bisherigen Symbolen gerendert. Es braucht nur einen statischen Puffer von 64 × 6 = **384 Byte** statt bis zu ca. **30.720 Byte dynamischen Bildpuffer**. Es gibt zusätzlich den 107-Byte-Header und die bereits vorhandenen Roh-/BLE-Framepuffer.
- Nur Texte nutzen diesen Weg, Blockseiten, Ambiente und kurze Texte bleiben beim zuvor bewährten Renderer. Gemischte Symbole, Normal/Bold, Mini einzeilig und Mini zweizeilig sind berücksichtigt.
- Die bestehende Paketlänge (128 Datenbytes pro Protokollpaket), XOR-Prüfsumme, das Framing und die MTU-Vorprüfung bleiben identisch. Der Stream-Renderer erzeugt möglicherweise **zusätzliche Rechenlast**; Performance ist am Board zu messen.
- `STREAM_THRESHOLD_BYTES` (8.192) kann bei Bedarf später angepasst werden.

## Änderung 2: MTU=0

- Wenn `getMTU()` einen Wert unter 160 liefert, wird die BLE-Verbindung genau **einmal** geschlossen und neu aufgebaut; Service FFF0 und Charakteristik FFF1 werden anschließend erneut abgefragt.
- Erst nach erneuter Prüfung beginnt irgendein Schreibvorgang dieses Jobs. Schlägt auch der zweite Versuch fehl, wird wie bisher sicher abgebrochen und der Grund protokolliert.
- Dies ist ein gezielter Recovery-Versuch, **keine Garantie**, dass der sporadische MTU=0-Fehler auf allen Geräten verschwunden ist.

## Änderung 3: RAMTRACE

- Der jeweils zuletzt abgeschlossene Job erhält eine kompakte Zusammenfassung des niedrigsten an Messpunkten beobachteten freien Heaps und größten Block-Einbruchs. Die Werte werden unabhängig vom 64-Einträge-Ring gespeichert.
- Ein historisches systemweites Minimum kann **zwischen** den Messpunkten entstehen; auch die Zuordnung eines einzelnen `malloc` bleibt nicht sichtbar.
- Diagnose-Dateien geben nun die korrekte Firmwareversion aus (`v0.4.0.3-dev`).

## Was ausdrücklich unverändert bleibt

- NVS-Schema und zwölf bestehende Blockseiten, keine Initialisierung von LittleFS, kein automatischer Start nach Stromausfall.
- LED-Pixelpalette, 64 Symbol-Slots, Symbol-IDs (auch verworfene Warnsymbole bleiben reserviert) und die durch Nutzer überarbeitete Pixel-Art-Excel vom 27.10.2026.
- WebUX und alle Programm-/Kurzwahlfunktionen; UI zeigt lediglich die neue Firmwareversion an.

## Prüfung ohne Hardware

- Hostseitiger C++-Differenztest vergleicht den ursprünglichen vollständigen Bitmaprenderer und den neuen 64-Spalten-Renderer mit **135 Testfällen**, darunter 5 Schriftlayouts, mehrfarbige Symbole, Unicode, lange sowie zweizeilige Texte.
- Alle Testfälle wurden bei sieben verschiedenen Abfragegrößen (1, 3, 7, 32, 64, 128, 197 Byte) bytegenau verglichen: bestanden. Die hostseitige C++-Kompilierung ersetzt **nicht** Arduino-ESP32-Kompilierung oder BLE-Hardwaretest.

## Risiken und Rückkehr

- Der neue Renderer ist ein experimenteller Firmwarezweig. Auf dem Board vor Festivalbetrieb unbedingt mit Vergleichsfotos und einem ≥30-KiB-Lauftext testen.
- Bei Abstürzen, falschen Pixeln oder wesentlich verlängerten Uploads wieder v0.4.0.2 flashen **ohne `Erase All Flash`**. Die NVS-Daten sind unverändert.
- Stromausfall während OTA/USB-Flash und das Risiko eines nicht getesteten Builds bleiben grundsätzlich bestehen.
