# Testplan v0.4.2.2-dev – Hardwareabnahme

**Vorbereitung:** JSON-Backup sicher aufbewahren, Arduino-ESP32 3.3.12, ESP32C3 Dev Module, Partition `No OTA (2MB APP/2MB SPIFFS)`, kein `Erase All Flash`.

| Nr. | Test | Ergebnis / Beobachtung |
|---|---|---|
| 1 | Sketch kompilieren, ESP32 starten, Bootlog ohne Fehler | |
| 2 | Alte Bibliothek, Programme, sechs Kurzwahlen nach Neustart vorhanden | |
| 3 | Gemischtes Programm Lauftext → Blockfolge (4 Seiten) → Lauftext starten | |
| 4 | Nach erster Lauftextgruppe wechselt ESP32 tatsächlich zur Blockfolge und danach wieder zum Lauftext | |
| 5 | Tempo 80: vier Blockseiten laufen ca. 6,3 s vor dem nächsten Programmupload | |
| 6 | Zwei direkt aufeinanderfolgende Blockfolgen werden zu **einem** Blockband zusammengefügt | |
| 7 | Reines Block- oder Lauftextprogramm: einmaliger Upload, danach autonome Endlosschleife | |
| 8 | Während aktivem gemischtem Programm: Geschwindigkeitsregler vorübergehend gesperrt | |
| 9 | Bibliothekseintrag bearbeiten: eigener Editormodus, ID bleibt beim Speichern unverändert | |
| 10 | Rückkehr aus Editor ohne Änderung sofort, mit Änderung Sicherheitsabfrage | |
| 11 | LED-Vorschau bleibt beim Scrollen unter dem fixierten Header | |
| 12 | Bibliothekstext, der in Programm und Kurzwahl verwendet wird, nach Abfrage löschen | |
| 13 | Kurzwahl automatisch unbelegt; Programm sichtbar rot markiert und nicht startbar | |
| 14 | Fehlenden Programmschritt ersetzen, Programm speichern und wieder starten | |
| 15 | Langer BLE-Upload: keine wiederholte Meldung „ESP32 nicht erreichbar“ | |
| 16 | Backup exportieren (Schema 2), Inhalt prüfen, unveränderte Daten nach Neustart | |

**Bitte in den Diagnoselogs notieren:** `TX_END`, `JOB_END`, `PROGRAM`-Dauer und ggf. WLAN-/BLE-Fehler. **Einschränkung:** Lauftextgruppen verwenden vorläufig weiterhin den eingestellten Sekunden-Fallback; exact-once ist noch nicht hardwarebestätigt.
