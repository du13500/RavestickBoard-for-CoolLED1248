# RavestickBoard: Diagnose und Fehlerprotokoll

**Einführung:** v0.1.1 (Testkandidat, noch nicht auf echter Hardware bestätigt)

## Aufrufen und herunterladen

1. Mit dem ESP32-WLAN `CoolLED1248` verbinden.
2. `http://192.168.4.1` im Browser öffnen.
3. Unten **Diagnose & Log-Export** aufklappen.
4. Werte aktualisieren oder **TXT herunterladen** auswählen.

API: `GET /api/diagnostics` liefert eine kleine JSON-Statusübersicht mit maximal 14 neuen Ereignissen, `GET /api/diagnostics.txt` den vollständigen TXT-Bericht mit maximal 56 RAM-Ereignissen.

## Daten

- Firmwareversion, Kompilierzeit, Resetgrund, Uptime ab Boot
- Freier und minimal freier Heap, größter freier allokierbarer Block
- Sketchgröße und freier App-Partitionsplatz, physischer Flash
- WLAN-Clientzahl und AP-Adresse (keine WLAN-Schlüssel)
- BLE-Verbindungsversuche, Verbindungen, Trennungen, Schreiben, Notifications und Zeitüberschreitungen
- Übertragene Pakete/Bytes sowie letzte Jobdauer und Erfolgs-/Fehlerzähler
- 56 letzte Ereignisse mit relativen Millisekunden-Zeitstempeln

## Grenzen und Interpretation

Die Daten bleiben **nur im RAM**. Neustart/Stromverlust leert den Ringpuffer; es gibt keinen persistenten letzten Fehler wie später eventuell im SmallTV. Der TXT-Download findet ausschließlich auf Nutzeraktion statt.

**BLE write succeeded** heißt nur, dass der Schreibvorgang lokal akzeptiert wurde. Eine Notification zeigt eine empfangene Antwort an, aber ihr Inhalt wird bislang nicht vollständig ausgewertet. Das sichtbare Ergebnis auf der LED-Tafel separat prüfen.

Da BLE-Jobs derzeit synchron laufen, reagiert der ESP32-Webserver während der Übertragung zeitweise verzögert. Ein kurz nicht aktualisierbarer Live-Status ist daher nicht automatisch ein Fehler.

Die alten Einschränkungen der v0.1.0-Vorschau und der ausschließlich groß geschriebenen Engschrift bestehen noch.

Für Troubleshooting möglichst **vor dem Reset** die TXT-Datei herunterladen und an die Fehlermeldung/Symptombeschreibung anhängen. Keine Passwörter, Textinhalte und BLE-Adressen werden absichtlich mitgeloggt.
