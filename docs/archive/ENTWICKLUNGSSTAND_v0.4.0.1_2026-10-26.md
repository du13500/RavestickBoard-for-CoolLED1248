# RavestickBoard v0.4.0.1-dev · Entwicklungsstand vom 26.10.2026

## Ausgangspunkt und Testbefund

Unsere v0.4.0 wurde auf dem ESP32-C3 kompiliert und auf der CoolLEDX-Hardware getestet: **30 von 31 Prüfpunkten OK**. Nur der 30-Minuten-Dauerbetrieb blieb offen. Neun vorhandene NVS-Blockseiten wurden unverändert geladen. Alle sieben Ambiente-Farben, die globalen Helligkeits-/Temposteuerungen, der Editor und die BLE-Modi wurden bestätigt.

Der Diagnoselog von v0.4.0 dokumentierte nach 4:18 Minuten unter anderem 6/6 BLE-Verbindungen, 7 erfolgreiche Jobs, 0 Fehler sowie 62.868 Byte freien und **9.416 Byte minimal freien Heap**. Dieser minimale Wert ist ein Beobachtungspunkt, aber kein alleiniger Nachweis für ein Speicherleck.

## Beschlossene Korrekturen

1. **Kurzwahl** steht als Startseite immer im Menü an erster Stelle. Danach Texteditor, Bibliothek, Programme, Ambiente, Diagnose, Einstellungen.
2. Bibliothek/Programme: Header und Aktionen oben fixiert, Liste füllt den verfügbaren Mittelbereich, kompakte Footer mit globaler Helligkeit, globalem Tempo und Modus-Override.
3. Eigene **monochrome 16×16-Pixelbilder** als Regler-Endsymbole: kleine/große Glühlampe und Schnecke/galoppierendes Pferd; keine gesonderte Überschrift.
4. Kein dauerhafter „Bereit“- beziehungsweise „Kurzwahl nicht belegt“-Fuß mehr. Erfolgshinweis verschwindet nach ca. 2,7 Sekunden, Fehler bleiben, leere Kurzwahl-Kacheln reagieren optisch.
5. Die neueste Excel `docs/Pixel-Art.xlsx` enthält 35 16×16-Motive; davon werden 27 neue Sprites angehängt. Die Excel-Krabbe **ersetzt** die Grafik an der bisherigen ID 11, die originale Schneeflocke bleibt und die neue wird **zusätzlich** geführt. Die zuvor integrierten sieben Excel-Motive behalten ihre alten IDs. Alien in „Weltall & Fahrzeuge“.
6. Ein gemeinsamer, thematisch sortierter Symbol-Picker zeigt die Pixelbilder selbst; Ursprung aus Hersteller-App oder Eigenentwurf ist nur in Quellenangaben, nicht in der Bedienoberfläche von Bedeutung.

## Technischer Rahmen

- Bisheriger Originalbestand: 36 Symbole à 128 Byte. Neuer Bestand: 63 Symbole à 128 Byte; zusätzliche **3.456 Byte Flash-Symboldaten**.
- Web-`symbols` und C++-`RB_SYMBOLS_PACKED` werden auf identische Bytes geprüft, ohne neues Dateisystem und ohne Änderung am NVS.
- Neues XML/Excel-Farbraster aus Zellfüllungen auf die vorhandene Palette R/G/B/C/M/Y/W + Aus übertragen. Thematisch sortierte Symbolbuttons verwenden jeweils den echten 16×16-Canvas.
- Für Text und Live-Steuerung bleibt das bekannte BLE-Protokoll und die 30-KiB-Bilddatenbegrenzung unverändert.
- Die neuen Bibliotheks- und Programmlisten sind weiter **Platzhalter**. Persistente Bibliothek, echtes Sortieren und Kurzwahlbelegung folgen erst in v0.4.1.
- Geschwindigkeitsregler 1–100; beide Werte bleiben global, nur der für den laufenden Inhalt erzwungene Modus ist individuell.
- Der Frosch benötigt weiterhin eine grafische Überarbeitung und wird in diesem Build nicht verändert.

## Prüfung und Freigabe

**Erfolgreich:** Browser mit Chromium bei 360×640, 390×844 und 1366×768, sieben Menüseiten, UI-Symbole einschließlich Alien und zwei Schneeflocken, keine JS-Ausnahme, optische Startseite ohne vertikales Scrollen. Identische Pixelarrays in der statischen Webdatei und der im Sketch eingebetteten WebUI; alle alten Sprites außer der Krabbe bytegleich zum Vorbuild; 63 gültige Pixelmatrizen.

**Offen:** Arduino-IDE-Kompilierung, echter BLE-Texttransfer einschließlich neuer Icons, Kontrollblick auf die 9 gespeicherten NVS-Seiten, erneuter Heap-Minimum-/30-Minuten-Test. Nicht als hardwarebestätigtes Release kennzeichnen.
