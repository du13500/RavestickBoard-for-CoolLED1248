# 🍌 RavestickBoard v0.4.0.2-dev – RAM-/Regressionstest

| Nr. | Prüfung | Erwartung | Ergebnis |
| ---: | --- | --- | --- |
| 1 | Arduino-IDE, ESP32C3 Dev Module, Sketch `RavestickBoard_v0_4_0_2.ino` kompilieren | Erfolgreich | ☐ |
| 2 | Ohne `Erase All Flash` flashen | NVS-Daten bleiben erhalten | ☐ |
| 3 | Version/AP/Web-Start | v0.4.0.2-dev, `CoolLED1248` | ☐ |
| 4 | 12 alte Blockseiten einzeln senden | Inhalt/Zeilen/Symbole weiterhin vorhanden | ☐ |
| 5 | Diagnose → RAM-Verlauf TXT | Download lesbar, Zeitstempel und Werte vorhanden | ☐ |
| 6 | Standard-Diagnose TXT | RAMTRACE-Zusammenfassung enthalten, keine Texte/Passwörter | ☐ |
| 7 | Kleiner Lauftext | Messpunkte `JOB_BEGIN`, `RENDER_BEGIN`, `BUFFER_ALLOC`, `RENDER_END`, `BLE_*`, `TX_*`, `RELEASE_*` | ☐ |
| 8 | 30-KiB-Lauftext / 240 Pakete | Paketfortschritt im RAMTRACE, BLE-Bildübertragung wie vorher | ☐ |
| 9 | Nach großer Übertragung | `RELEASE_END` vorhanden, freier Heap steigt plausibel | ☐ |
| 10 | 1→2→3→4 WLAN-Clients | RAM/Maxblock aufzeichnen, keine unbeabsichtigten Resets | ☐ |
| 11 | 12-Seiten-Programm | Fortlaufende Jobs und RAM-Proben, kein Freeze | ☐ |
| 12 | Pixelart: Frosch/Zigarette/Joint/Verbote | Kleine überarbeitete Pixelgrafiken korrekt | ☐ |
| 13 | Pixelart: Ziggi I und Ziggi II | Beide separat im Picker, Pixelvorschau = Anzeige | ☐ |
| 14 | Sechs bunte Warnungen | Nicht mehr im Picker; alte gespeicherte Texte weiterhin lesbar | ☐ |
| 15 | Zwei Schneeflocken, Krabbe, Herzen, Banane | Bestehende Grafiken erhalten | ☐ |
| 16 | Helligkeit/Tempo/Modi/Ein-Aus/Ambiente | Keine Regression der bekannten Steuerbefehle | ☐ |
| 17 | 30-Minuten-Betrieb / Browserreload | Kein Freeze; RAM-Low und maxBlock dokumentieren | ☐ |

**Teststatus:** nur statische Prüfung, die Einträge müssen nach realem Hardwaretest abgehakt werden. Bei Auffälligkeiten beide TXT-Dateien sichern.
