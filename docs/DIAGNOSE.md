# RavestickBoard · Diagnose und Fehlerprotokoll

**Fortgeschrieben 25.10.2026 · v0.3.3 hardwaregetestet; Modus-Testversion v0.3.4 noch ungetestet.**

## Diagnose-Ergänzung v0.3.4 – Testbare Moduswerte (25.10.2026)

Der neue Modus-Dropdownwert wird vor dem BLE-Auftrag auf genau `0x01` bis `0x08` geprüft und als **Modusparameter des Steuerbefehls `0x06`** im Diagnose-Log festgehalten. Vor einem Auftrag melden Start- und `Auftrag:`-Zeile jetzt Moduswechsel statt „Helligkeit“; der TXT-Export erhält einen aus `FW_VERSION` gebildeten Dateinamen. Bei Safari-Refresh soll `Cache-Control: no-store` die aktuelle v0.3.4-Oberfläche ausliefern. **Noch keine Hardwarebestätigung für die sechs neuen Effekte**; sichtbaren Anzeigemodus getrennt von `JOB OK` notieren. Vorherige v0.3.3 bleibt Rückfallversion.

## Diagnose-Ergänzung v0.3.3 (25.10.2026)

Der seriellen HTTP-Logzeile für Lauftextaufträge werden **dekodierte Zeichen, Bildspalten und Datenbytes** hinzugefügt. Bei zu großer berechneter Datenmenge erfolgt ein HTTP-400-Abbruch **vor dem BLE-Auftrag**; der Text bleibt im Browser bestehen. Das Webinterface zeigt eine rote/fette Byte-Auslastung und deaktiviert „Lauftext senden“. Eine spätere dynamische RAM-Allokation kann trotzdem scheitern und erscheint wie bisher als `[RAM] Bildpuffer ... fehlgeschlagen` sowie `JOB FEHLER`. Eine `JOB OK`-Meldung beweist lediglich erfolgreiche Paketübertragung und Benachrichtigungen, **nicht** die Bildübernahme durch die Tafel. Den tatsächlich sichtbaren Effekt stets getrennt protokollieren. **Hardware-Nachtrag:** v0.3.3 wurde am 25.10.2026 erfolgreich getestet; 240 BLE-Pakete und 30.695 B über 52.937 ms mit `JOB OK`, tatsächlich auf der Tafel angezeigt.

## Neuer Diagnosebefund: Bildtransfer ≠ Bildanzeige (25.10.2026)

Beim v0.3.2-Korrektur-2-Praxistest wurden teilweise alle BLE-Pakete quittiert und `JOB OK` gemeldet, obwohl die CoolLEDX-Tafel keine neue Grafik anzeigte (etwa 320 gemischte Symbole, 31.853 B / 249 Pakete). Deshalb den Status **„vollständig gesendet“** nicht mit **„Tafel zeigt neue Grafik“** gleichsetzen. Ein wirkliches Display-Abschluss-ACK ist bislang nicht nachgewiesen. Gleichzeitig können sehr lange Symbolfolgen auf dem Controller bereits an einer fehlgeschlagenen zusammenhängenden RAM-Allokation scheitern. Für vollständige Messwerte und Grenzideen siehe [`ENTWICKLUNGSSTAND_v0.3.2_2026-10-25.md`](ENTWICKLUNGSSTAND_v0.3.2_2026-10-25.md).

## Aufrufen und Exportieren

1. Mit `CoolLED1248` verbinden und `http://192.168.4.1` öffnen.
2. Bereich **Diagnose / Log** aufklappen und bei Bedarf aktualisieren.
3. **„Log kopieren“**: seit v0.1.2 bevorzugt. Wegen unverschlüsseltem HTTP kann die Clipboard-API auf iPhone/Safari eingeschränkt sein; der Browser versucht Fallback bzw. bietet manuellen Text. **Der Safari-Fallback ist nicht in sämtlichen Konstellationen abgenommen.**
4. Alternativ **TXT herunterladen**. Dieser Weg wurde bereits mit v0.1.1 auf dem iPhone bestätigt.

| Schnittstelle | Inhalt |
| --- | --- |
| `GET /api/diagnostics` | Kleine JSON-Übersicht mit jüngsten Ereignissen |
| `GET /api/diagnostics.txt` | Vollständiger TXT-Bericht mit bis zu **56** RAM-Ereignissen |
| `GET /api/status` | Status laufender bzw. beendeter Aufträge |

## Erfasste Informationen

- Firmware-/Buildstand, Resetgrund (einschließlich Klartext für **USB-Reset**), Laufzeit und `millis()`-Zeitbasis.
- Freier/minimal freier Heap, größter allokierbarer RAM-Block, Sketchgröße, laufende **App-Partitionsgröße** und rechnerisch freie App-Bytes.
- WLAN-AP-Adresse und Zahl der Clients, nicht jedoch WLAN-Passwort.
- BLE-Verbindungsversuche, Erfolge, Trennungen, Schreiben, Notifications, Timeout- und Fehlerzähler.
- Transferstatistik (Pakete/Bytes, Dauer, Erfolg/Fehlschlag), HTTP-Aufträge und abgewiesene Aufträge.
- Seit **v0.2.0**: gespeicherte Seiten, Playerstatus, Seitenübertragungen OK, NVS-Seitenspeicherungen und automatische Seitenwechsel.
- Begrenzt gespeicherte Ereignisse als relativer Zeitverlauf seit Boot; jeweils mit Kategorie und Beschreibung.

## Interpretation und bekannte Grenzen

- **RAM-only:** Neustart und Stromausfall löschen den Diagnose-Ringpuffer und die laufenden Fehlerzähler. **NVS-Blockseiten bleiben hingegen erhalten.**
- **Schreiben ≠ Anzeigen:** Ein akzeptierter `write-without-response`-Auftrag beweist noch keine Bilddarstellung. Notification-Empfang, Transfer-Ende und sichtbares Display separat prüfen.
- **Blockierende BLE-Übertragung:** Während eines synchronen Transfers kann der Webserver für einige Sekunden verzögert reagieren.
- **Kein eingebundenes SPIFFS/LittleFS:** Die Anzeige kann den tatsächlichen belegten Dateisystemspeicher nicht bestimmen. Angaben zur App-Partition sind keine freien RAM-Bytes.
- **Keine Echtzeituhr:** Ereignisse nutzen Millisekunden seit Boot, nicht Uhrzeiten. `millis()` läuft als 32-Bit-Zähler nach rund 49,7 Tagen über.
- **Verbindungskonflikt:** Eine Fehlermeldung kann auf die Original-App als mögliche Ursache hinweisen, aber der ESP32 kann deren aktive Verbindung aus einem Timeout nicht sicher nachweisen.
- **Datensparsamkeit:** TXT-Export enthält absichtlich keine Anzeigeinhalte, Passwörter oder BLE-MAC-Adressen. Trotzdem Fehlerberichte vor öffentlichem Teilen kurz prüfen.

## Praktisch beobachteter Stand

**v0.1.1:** Der Nutzer testete u. a. fünf erfolgreiche Textaufträge, fünf Verbindungen, 20 BLE-Notifications, keine Schreib-/Timeoutfehler; nach Trennung der konkurrierenden Original-App war der Controller ohne Neustart wieder einsatzbereit. Diagnose und TXT-Export funktionierten auf dem iPhone.

**v0.2.0 Korrektur 1:** Seitenplayer, NVS-Blockseiten und reale mehrfache Uploads sind vom Nutzer bestätigt. Eine gesonderte Abnahme **sämtlicher neuer v0.2.0-Diagnosefelder** wurde noch nicht dokumentiert.

## Empfohlener Fehlerbericht

1. Firmwareversion, Gerät, Uhrzeit/Situation, Betriebsdauer notieren.
2. **Vor dem Reset** Diagnose kopieren oder TXT speichern.
3. Betroffene Seite/Programm, gewünschtes vs. tatsächliches Verhalten und Foto/Screenshot angeben.
4. Bei BLE-Fehlern festhalten, ob die Original-App gleichzeitig verbunden war.
5. Bei reproduzierbarem Fehler eine Testfolge mit möglichst wenigen Schritten dokumentieren.

**Offen für später:** persistenter kritischer letzter Fehler (nur nach Flashverschleiß-Abwägung), weitere Timeout-/NAK-/ACK-Diagnose, Langzeit-/Heap-Test, laufende Grafikanimationen und deren Speicher-/Transferstatistik.

## 24.10.2026 · Entwicklungszusatz v0.3.0

Separate Steuerjobs `BRIGHTNESS`, `SPEED`, `MODE` verwenden kurze BLE-Kommandos und für ca. 2,5 Sekunden nach dem letzten Kommando eine bestehende Verbindung. Die eigentliche Wirkung auf der Original-Tafel, konkurrierende App-Verbindungen und der BLE-Timeoutpfad müssen erneut auf Hardware getestet werden. Die Diagnose bleibt RAM-only. Dieser Abschnitt ist noch kein Hardwaretestbericht.


## Für den 600-Zeichen-Versuch unter v0.3.2

Bei jeder Teststufe 80, 100, 150, 200, 300, 400, 500, 600 bitte erfassen: (1) Anzahl Zeichen laut Editor, (2) angegebene Text-Payload-Bytes, (3) BLE-Paketzahl, (4) `JOB OK` bzw. Fehler, (5) tatsächliche sichtbare Darstellung auf der LED-Tafel. Ein `JOB OK` besagt, dass die Paketkommunikation erfolgreich verlief, nicht zwingend, dass das Board den Text vollständig anzeigt.

**Auffälligkeiten aus v0.3.1:** Einzelne BLE-Verbindungsfehler konnten durch den nächsten Verbindungsversuch überwunden werden; beim automatischen Seitenplayer stoppte ein Verbindungsfehler hingegen die Wiedergabe. Begrenzte Auto-Retry-Strategie bleibt auf später verschoben.

## WLAN-Startfehler unter v0.3.2 Korrektur 1 (25.10.2026)

Nutzerlog: Nach `Seiten geladen: 10` scheiterte der AP-Start mit `wifi:alloc pp wdev funcs fail`; anschließend meldete der Treiber beim Aufräumen `0x3001`. Diese Zeilen sind mit **zu wenig freiem Arbeitsspeicher beim Initialisieren des WLAN-Treibers** vereinbar. In Korrektur 1 waren allein die beiden globalen Bild-/Payload-Puffer mit zusammen 122.507 Bytes reserviert.

Korrektur 2 vermeidet diese globale Vorbelegung. Im seriellen Monitor erscheinen vor BLE, nach BLE/NVS und nach WLAN jetzt `[RAM]`-Zeilen mit freiem Heap und größtem allokierbaren Block sowie beim Senden Puffergröße und Rest-Heap. Bei fehlgeschlagenem WLAN-Start wird der HTTP-Webserver nicht als bereit gemeldet.

**NVS-Warnung:** Zur Diagnose **keinen Flash-Erase** und kein NVS-Reset ausführen. Das Firmwareupdate mit der üblichen Partitionseinstellung bewahrt das Seitenformat, sofern die IDE nicht auf `Erase All Flash` gestellt wurde. Bei weiterer Störung v0.3.1 als funktionierenden Rückfallstand verwenden.
