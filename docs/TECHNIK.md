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

## V0.1.0: bekannte Code-Randbedingungen

- `MAX_CHARS = 40`, 5×7-Font, nur Großbuchstaben, `ä/ö/ü` werden zu Großbuchstaben.
- `SIGN_WIDTH = 64`, `SIGN_HEIGHT = 16`, Glyphen 5 Pixel breit und 7 Pixel hoch, vertikal verdoppelt.
- JavaScript und C++ enthalten derzeit **je eine eigene Glyphentabelle**. Gefahr der Abweichung; gemeinsames Fontdatenmodell vorbereiten.
- Weboberfläche als HTML/CSS/JS direkt im Firmware-Flash; aktuell kein persistenter ESP32-Bibliotheksspeicher implementiert.
- Feste BLE-Adresse im Sketch; später konfigurierbarer Scan und stabile Wiederverbindung sinnvoll.
- Nur eine einzelne Textfarbe pro Upload, kein Zeichenmix.
- API derzeit: `GET /`, `GET /api/status`, `POST /api/text`, `POST /api/brightness`.
- Webserver-Bearbeitung ist während synchroner BLE-Aufträge eingeschränkt; im Test bisher nutzbar, für lange Transfers gezielt testen.

## Status- und Sicherheitshinweise

- `Write Without Response` auf BLE-Ebene darf **nicht** mit „Tafel hat Inhalt sicher übernommen“ gleichgesetzt werden. Notification, Fehlercode und sichtbares Resultat getrennt festhalten.
- Das WLAN-Passwort steht im Sketch im Klartext. Nicht unverändert in eine öffentliche Repo-Version übernehmen.
- Keine OTA-Schnittstelle vorsehen. Das verhindert keine Fremdsteuerung über die originale BLE-Schnittstelle.
- Speicherüberwachung: `ESP.getFreeHeap()`, `ESP.getMinFreeHeap()`, `heap_caps_get_largest_free_block(MALLOC_CAP_8BIT)` für Diagnose prüfen; bei größtem freien Block korrekte Vergleichsbasis dokumentieren.
- Persistenter Resetmarker und schwerwiegender letzter Fehler nur nach Kosten-/Flashverschleiß-Prüfung, tägliche Ereignisse zunächst im RAM-Ringpuffer.
