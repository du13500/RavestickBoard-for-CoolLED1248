# Hardwaretestplan v0.4.2.4-dev

**Vorbereitung:** JSON-Backup und alte Firmware extern sichern. ESP32-C3, Partition **No OTA (2 MB APP / 2 MB SPIFFS)** unverändert, `Erase All Flash` deaktivieren; bei COM-Port-Problemen 115200 Baud. Immer nach lokalem Kompilieren flashen.

| Nr. | Test | Ergebnis / Beobachtung |
|---|---|---|
| 1 | Firmware kompiliert / Bootlog ohne Panic; Version 0.4.2.4, Bibliothek und Kurzwahlen erhalten | offen |
| 2 | Kleine Laufbandkombination (Lauftext → 4 Blockseiten → Lauftext), ein Upload, Farben und Reihenfolge korrekt | offen |
| 3 | 16 echte Leer-Pixelspalten zwischen Einträgen überprüfen, inklusive Blockseiten | offen |
| 4 | Gemischtes Laufband größer als 30 KiB: alle Gruppen vollständig abgespielt, kein Abschneiden nach 20 s | offen |
| 5 | Wiederholte große Übertragungen mit RAMTRACE: Speicher vor/nach Job, größter zusammenhängender Block | offen |
| 6 | BLE-Paketfehler provozieren: maximal ein Voll-Retry; kein stilles Überspringen / nach erneutem Fehler Stop mit Hinweis | offen |
| 7 | Normales gemischtes Programm mit stehenden Blöcken: Wechsel und 4 Blockseiten bei Tempo 80 (~6,3 s) | offen |
| 8 | Home: sechs kompakte Kurzwahlen, 2×3, leere Kachel öffnet Setup | offen |
| 9 | Eine Tafelsteuerleiste: Helligkeit/Tempo synchron auf Home, Bibliothek, Editor, Programme und Ambiente; keine verdeckten Buttons | offen |
| 10 | Acht Modus-Icons wiederholt antippen: **jeder** Klick sendet einen BLE-Befehl (ohne Upload) | offen |
| 11 | Preview Anfang/Play/Pause/Tempo Schnecke → Vogel → Hase; keine Unicode-Icons nach Klick | offen |
| 12 | Diagnose-Menü `bug`, keine „Navigation“-Überschrift, kein technischer Editor-Fußtext | offen |
| 13 | Library bearbeiten & überschreiben, Programme nach Löschen reparieren, Kurzwahlen automatisch leeren | offen |
| 14 | Backup exportieren und zweimal zusammenführen, keine unnötigen Duplikate | offen |
| 15 | Mehrere WLAN-Clients, 10+ Minuten Betrieb, Netzverbindungswechsel und Neustart; Diagnoselog sichern | offen |

**Wichtig:** Geteilte Laufbänder besitzen noch keine hardwarekalibrierte Laufzeit. Ein Segment darf länger als nötig wiederholt werden; entscheidend ist, dass **kein Inhalt abgeschnitten oder übersprungen** wird. Einzelne BLE-Erfolgspakete bestätigen nicht die sichtbare Darstellung.
