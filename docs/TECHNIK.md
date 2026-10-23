# Technische Bestandsaufnahme

## Hardware und Funk

- ESP32: **TENSTAR ROBOT ESP32-C3 Super Mini**, 160 MHz, 4 MB Flash laut `esptool`, BLE 5 LE und 2,4-GHz-WLAN.
- Boardprofil: `ESP32C3 Dev Module`, serieller USB-C-Port (beim ersten Test COM3).
- Arduino IDE 2.3.10, Espressif Arduino-ESP32 3.3.12.
- Partitionierung: `No OTA (2MB APP/2MB SPIFFS)`; die exakten nutzbaren Bytes bestimmen die tatsächlichen Partitionen, nicht nur die Beschriftung.
- LED-Gerät: Bluetooth-Anzeigename `CoolLEDX`; beim Test MAC `ff:00:00:03:7e:c3` (nur für das getestete Exemplar).
- GATT-Service `0000fff0-0000-1000-8000-00805f9b34fb`.
- Characteristic `0000fff1-0000-1000-8000-00805f9b34fb`: Read, Notify, Write Without Response (kein normales Write-With-Response).
- Aus Hersteller-Advertising abgeleitet: **64 × 16 LEDs**, Farbmodus `01` (7-Farben), Firmwarekennung `06`.
- Die vollständigen Advertising-Hexbytes aus dem Test waren `e1ebff0000037ec31000400106`. Die ersten zwei Bytes sind Herstellerkennung; danach folgen sechs MAC-Bytes, anschließend Höhe `0x10`, Breite `0x0040`, Farbmodus `0x01`, Firmware `0x06`.
- Nutzerbeobachtung: Die Tafel spielt hochgeladene Inhalte **ohne laufende Bluetooth-Verbindung** ab und speichert sie **über die Stromtrennung** hinaus.

## Verbindungsweg

```text
iPhone / anderer Browser
         |
         | WLAN-AP, passwortgeschützt, ohne Internet
         v
 ESP32-C3 Webserver   (http://192.168.4.1)
         |
         | Bluetooth LE, Service FFF0 / Characteristic FFF1
         v
  CoolLEDX Original-Anzeige, unveränderte Hardware
```

## Bereits implementierte Übertragung

- Benutzeraktion im Webinterface erzeugt einen Auftrag, kein unaufgeforderter Transfer beim Start.
- Befehle verwenden die in Community-Projekten dokumentierte CoolLEDX-Rahmung (`0x01`, escapen, `0x03`).
- Bei Textdaten: RGB-Bitfelder, chunkweise übertragen, Notification nach jedem Paket abwarten.
- Helligkeit über Steuerbefehl `0x08`; Senden des Werts `0xFF` war real sichtbar erfolgreich.
- Bei v0.1.0 entscheidet derzeit Pixelbreite >64 automatisch auf Lauftext. **Dies wird ersetzt** durch eine manuelle Moduswahl.

## V0.1.x: bekannte Code-Randbedingungen

- `MAX_CHARS = 40`, 5×7-Font, nur Großbuchstaben, `ä/ö/ü` werden zu Großbuchstaben.
- `SIGN_WIDTH = 64`, `SIGN_HEIGHT = 16`, Glyphen 5 Pixel breit und 7 Pixel hoch, vertikal verdoppelt.
- JavaScript und C++ enthalten derzeit **je eine eigene Glyphentabelle**. Gefahr der Abweichung; gemeinsames Fontdatenmodell vorbereiten.
- Weboberfläche als HTML/CSS/JS direkt im Firmware-Flash; aktuell kein persistenter ESP32-Bibliotheksspeicher implementiert.
- Feste BLE-Adresse im Sketch; später konfigurierbarer Scan und stabile Wiederverbindung sinnvoll.
- Nur eine einzelne Textfarbe pro Upload, kein Zeichenmix.
- API v0.1.0: `GET /`, `GET /api/status`, `POST /api/text`, `POST /api/brightness`. **Zusätzlich ab v0.1.1:** `GET /api/diagnostics` (JSON), `GET /api/diagnostics.txt` (TXT).
- Webserver-Bearbeitung ist während synchroner BLE-Aufträge eingeschränkt; im Test bisher nutzbar, für lange Transfers gezielt testen.

## Status- und Sicherheitshinweise

- `Write Without Response` auf BLE-Ebene darf **nicht** mit „Tafel hat Inhalt sicher übernommen“ gleichgesetzt werden. Notification, Fehlercode und sichtbares Resultat getrennt festhalten.
- Das WLAN-Passwort steht im Sketch im Klartext. Nicht unverändert in eine öffentliche Repo-Version übernehmen.
- Keine OTA-Schnittstelle vorsehen. Das verhindert keine Fremdsteuerung über die originale BLE-Schnittstelle.
- Speicherüberwachung: `ESP.getFreeHeap()`, `ESP.getMinFreeHeap()`, `heap_caps_get_largest_free_block(MALLOC_CAP_8BIT)` für Diagnose prüfen; bei größtem freien Block korrekte Vergleichsbasis dokumentieren.
- Persistenter Resetmarker und schwerwiegender letzter Fehler nur nach Kosten-/Flashverschleiß-Prüfung, tägliche Ereignisse zunächst im RAM-Ringpuffer.

## v0.1.1-Diagnose (noch auf Hardware zu testen)

- Versionskonstante `FW_VERSION = "0.1.1"`, Header im Browser sichtbar.
- Ereignisspeicher: 56 feste RAM-Einträge, jedes Ereignis mit ms seit Boot, Kategorie und Kurzbeschreibung; ältere Einträge werden überschrieben.
- Metriken: `ESP.getFreeHeap()`, `ESP.getMinFreeHeap()`, `ESP.getMaxAllocHeap()`, `ESP.getSketchSize()`, `ESP.getFreeSketchSpace()`, `ESP.getFlashChipSize()`.
- Resetgrund: `esp_reset_reason()`. Uptime: `millis()` (32-Bit-Zähler, nach ca. 49,7 Tagen Überlauf).
- BLE Write-Erfolg, Notification und tatsächliche Anzeigeübernahme sind getrennte Bedeutungen.
- Die vorhandene synchrone BLE-Übertragung blockiert kurzzeitig HTTP-Anfragen. Live-Diagnose und Download funktionieren zwischen Aufträgen, nicht während eines langen BLE-Transfers.
- Kein Mount oder Schreiben des SPIFFS-Dateisystems. Die v0.1.1-Diagnose kann daher keinen tatsächlichen belegten Dateisystemspeicher ausweisen.
- WLAN-SSID bleibt in v0.1.1 absichtlich `CoolLED1248` für rückwärtskompatible Smartphone-Verbindungen.
- Der Reset löscht die Ereignisse und Fehlerzähler; Flash bleibt geschont. Eine spätere sparsame Persistenz ist gesondert zu entscheiden.

## Ergänzung v0.1.2

- Die Fonttabelle liegt ausschließlich in C++ (`FONT` mit Unicode-Codepunkten), die Browser-Vorschau lädt sie über `GET /api/font`. Unterstützt sind A–Z, a–z, 0–9, Ä Ö Ü ä ö ü ß, Herz und ausgewählte Zeichen. Andere Zeichen werden derzeit als `?` gerendert.
- `GET /api/font` wird nur beim Laden der Oberfläche verwendet. Bei Verbindungsproblemen weist die Vorschau auf den fehlgeschlagenen Schriftabruf hin.
- Benutzerdefinierter Anzeigemodus: `0x06 0x01` Standbild, `0x06 0x02` Linkslauf. Diese BLE-Modi sind aus vorhandenen Tests und Protokollreferenzen abgeleitet. Das Verhalten für Textbreite > 64 Pixel im Standbild muss noch verifiziert werden.
- Neuer Testbefehl für Laufgeschwindigkeit: `0x07 <0x00..0xFF>`, UI-Skala 1–100 wird auf 0–255 skaliert. Der Wirkbereich und die Richtung werden am echten Board erst kalibriert.
- Die Browser-Vorschau animiert lokal über `requestAnimationFrame` und verändert dabei nicht die auf dem Board gespeicherten Inhalte.
- EEPROM/SPIFFS/LittleFS weiterhin nicht eingebunden; kein Firmware-OTA, Log weiterhin RAM-only.

## v0.2.0 Techniknotizen
- `PageRecord`: zwei UTF-8-Zeilen (je 47 Byte im Speicherfeld), Layout 0/1, Ausrichtung 0/1/2, Farben 1..7, Dauer 5..60s. Zwölf Einträge in NVS-Namensraum `rbpages` unter Schema 1.
- `renderBlockPage`: eine 64×16-Pixelseite, identisches RGB-Bitfeld wie v0.1.2. Payload mit 24 Nullbytes, Textmetadaten (80 Byte), 16-bit-RGB-Länge, RGB-Ebenen R/G/B; ergibt 491 Byte und vier BLE-Chunks zu max. 128 Rohdatenbytes.
- Zweizeilenmodus: zwei unabhängige Zeilen 5×7 bei y=0..6 und y=8..14; eine Leerzeile dazwischen. Einzeilenmodus: 5×7 bei y=1..14 vertikal 2×.
- Abbruch bei Zeilen >10 Codepoints (statt Abschneiden). Unicode-Unterstützung folgt der vorhandenen Glyphentabelle, inklusive großem ẞ als gleiche Silhouette wie ß.
- `GET /api/program`: Seiten und Playerstatus; `POST /api/program/{set,add,delete,move,show,start,stop}`: explizite Benutzeraktionen.
- Automatische Wiedergabe im Hauptloop, nach jedem übertragenen Bild startet dessen Anzeigedauer. BLE wird je Seite verbunden und getrennt. Fehler stoppt die Playlist.
- Eine neue Firmware schaltet das Seitenprogramm nie von selbst ein; gespeicherte Inhalte werden nur im Editor geladen.
