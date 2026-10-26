# RavestickBoard · Testplan v0.4.0-dev

**Testkandidat:** Noch kein erfolgreicher Arduino-/Hardwaretest dokumentiert. Ergebnisse tragen wir nach dem Flashen ein. Das Upgrade soll die bisherigen NVS-Seiten erhalten; **kein Erase All Flash**.

| Nr. | Bereich | Testschritt | Erwartung | Ergebnis |
| ---: | --- | --- | --- | --- |
| 01 | Build | Sketch im Ordner `RavestickBoard_v0_4_0` kompilieren | Erfolgreich, Board ESP32C3 Dev Module | ☐ |
| 02 | Boot | USB-Reset, serielle Konsole | v0.4.0-dev, AP gestartet | ☐ |
| 03 | NVS | Vorhandene neun Blockseiten öffnen | Alle Texte, Farben, Reihenfolgen erhalten | ☐ |
| 04 | Startseite | Auf iPhone ohne Scrollen ansehen | Sechs 2×3-Kacheln und Regler sichtbar | ☐ |
| 05 | Menü | Banane auf jeder Seite antippen | Menü öffnet; Start auf Startseite ausgeblendet | ☐ |
| 06 | Menü | Sieben Ansichten durchwechseln | Header bleibt, kein Funktionsverlust | ☐ |
| 07 | Kurzwahl | Leere Taste antippen | Deutlich „nicht belegt“, nichts übertragen | ☐ |
| 08 | Kurzwahl | Bearbeiten aktivieren/deaktivieren | Visuelles Feedback, noch keine Speicherung | ☐ |
| 09 | Ein/Aus | Auf Startseite Aus/Ein antippen | LED reagiert ohne Bildtransfer | ☐ |
| 10 | Global | Helligkeit im Editor ändern | Home/Bibliothek/Programme/Ambiente zeigen identischen Wert | ☐ |
| 11 | Global | Tempo Bibliothek 23 → Editor öffnen | 23/100 wird übernommen | ☐ |
| 12 | Global | Neuen Lauftext senden | Helligkeit/Tempo bleiben unverändert | ☐ |
| 13 | Tempo | Extremwerte 1 und 100 ausprobieren | Steuerauftrag ohne 400-Fehler; Verhalten notieren | ☐ |
| 14 | Modus | Modus „Laser“ im Bibliotheks-Footer erzwingen | Aktuelle Anzeige wechselt ohne Bildtransfer | ☐ |
| 15 | Modus | Danach normalen Lauftext senden | Eigener Modus Linkslauf setzt Override zurück | ☐ |
| 16 | Editor | Normal/Bold und Mini 1-/2-zeilig ausprobieren | Darstellung wie v0.3.5.1 | ☐ |
| 17 | Editor | Bestehende NVS-Blockseite ändern/senden | Unverändert funktionsfähig | ☐ |
| 18 | Editor | Altes Seitenprogramm starten/stoppen | Unverändert funktionsfähig | ☐ |
| 19 | Symbol-Picker | Herz neben Texteingabe antippen | Thematisch sortierte echte Pixelbilder | ☐ |
| 20 | Symbole | Mehrere Motive an Cursorstelle einfügen | Korrekte Vorschau + Sendung | ☐ |
| 21 | Herz | Alle sieben Herzfarben ausprobieren | Farbkodierung wie bisher | ☐ |
| 22 | Ambiente | Alle sieben Farbflächen antippen | Gesamte 64×16-Matrix in gewählter Farbe | ☐ |
| 23 | Ambiente | Helligkeit zuvor auf 12 % setzen | Ambiente übernimmt aktuelle Helligkeit | ☐ |
| 24 | Ambiente | Nach Farbfläche normalen Text senden | Text ist wieder sichtbar, eigener Modus gilt | ☐ |
| 25 | Diagnose | Diagnose auf eigener Seite öffnen | Statuskarten/Log/Copy/TXT sind vorhanden | ☐ |
| 26 | Status | Diagnose-TXT herunterladen | v0.4.0-dev; keine Passwörter oder Textinhalte | ☐ |
| 27 | Bibliothek | Auf leere Liste zugreifen | Keine erfundenen/gefälschten Einträge | ☐ |
| 28 | Programme | Auf Programmansicht zugreifen | Platzhalter ohne versehentliche Sendeaktion | ☐ |
| 29 | Einstellungen | Gerätesuche ansehen | Noch ausdrücklich deaktiviert | ☐ |
| 30 | Dauerbetrieb | 30 Minuten, mehrfach Browserreload | Kein Freeze; Heapminimum notieren | ☐ |
| 31 | Mehrgeräte | Zweites Smartphone verbinden | Gemeinsame Sollwerte nach Statuspoll sichtbar | ☐ |

**Bei Fehlern** möglichst Diagnose-TXT, Uhrzeit seit Start, genaue Bedienfolge und Foto der tatsächlichen LED-Anzeige zusammen dokumentieren. BLE-„OK“ und sichtbare Wirkung getrennt bewerten.
