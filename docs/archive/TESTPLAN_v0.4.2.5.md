# v0.4.2.5-dev – kurzer Hardware-Test und bekannte Grenzen

## Warum diese Version?

A/B-Untersuchung v0.4.2.4.2 ergab: gzip/ETag/HTTP 304 funktionieren und die Startseite wird schneller ausgeliefert. `ESP.getMinFreeHeap()` sank dennoch bis 592 B, teils bei nur einem WLAN-Client. Die genaue Ursache ist nicht geklärt; diese Version **behauptet keinen nachgewiesenen Fix**. Hauptänderungen: gebündelte JSON-Ausgabe (`/api/library`, `/api/collections`) und schon im HTML enthaltene Lucide-Symbole.

## Kurzer Basistest (ohne Testmarathon)

1. Backup der Bibliothek erstellen und frühere Firmware als Rückfallebene aufbewahren. Partitionen, LittleFS und NVS nicht löschen.
2. Arduino IDE v0.4.2.5-dev flashen, seriellen Boot und `Firmware: v0.4.2.5-dev` prüfen.
3. Über WLAN `CoolLED1248` `http://192.168.4.1` aufrufen. Beim Laden dürfen Banane, Menübuttons und Steuerungsleisten nicht von alten Platzhalterzeichen auf SVGs umspringen.
4. Bibliothek und Programme öffnen; Anzahl und Namen mit v0.4.2.4.2-B vergleichen, einen Eintrag bearbeiten und wieder korrekt öffnen. Falls unsicher, nicht speichern/löschen.
5. Eine kurze Anzeige über BLE senden und Sichtbarkeit am LED-Board kontrollieren. Dieser Test prüft **nicht** die vollständige BLE-/Programmregression.
6. Optional: `/api/http.txt` und `/api/diagnostics.txt` jeweils einmal exportieren. Bei kritischen Tiefstwerten, Fehlermeldungen oder Neustarts Logs aufbewahren.

## Interpretation

- HTTP 304 ist erwartbar für eine unveränderte Seite (ETag); 200 ist erwartbar nach Firmwarewechsel/Cache-Miss.
- Die Tafel-Fortschrittsanzeige während BLE-Upload bleibt unverändert.
- **Ein historisches Minimum < 1 KiB ist weiterhin kritisch**, auch wenn der aktuelle freie Heap 30–40 KiB beträgt.
- Der 20-ms-Sampler kann sehr kurze Peaks verpassen; `ESP.getMinFreeHeap()` bleibt pro Boot historisch.
- Da A/B-Lastfälle nicht streng identisch waren, sind einzelne Minima keine statistisch belastbaren Vergleiche. Die Hardware-Abnahme dieser neuen Version steht aus.
