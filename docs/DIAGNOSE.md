# RavestickBoard · Diagnose und Fehlerprotokoll

**Stand 23.10.2026 · Einführung v0.1.1, auf realer Hardware bestätigt; Ergänzungen bis v0.2.0 Korrektur 1.**

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
