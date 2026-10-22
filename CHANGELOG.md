# Änderungs- und Testhistorie

**Prinzip:** Tatsächlich getestete Funktionen und geplante Änderungen getrennt führen. Eine neue Versionsnummer bezeichnet eine bewusst archivierte Firmware, nicht nur eine Idee.

## [Unreleased] · Sammlung ab 22.10.2026

### Geplant
- Diagnosebereich mit Speicherstatistik, BLE-/WLAN-Ereignislog und TXT-Export (`DIA-*`).
- Vorschau als echte 64×16-LED-Punktmatrix, Lauftext-Simulation, 1×/2×/4×/8× und Vorspulen (`PRE-*`).
- UI-Fixes für iPhone-Safari: sieben Farben in einer Reihe, Helligkeit `100 %` ohne Umbruch, Bedienelemente nicht hinter Browserleiste (`UI-*`).
- Explizite Wahl zwischen Lauftext und statischen Seiten, keine Längenautomatik (`EDT-*`).
- Ergänzen echter Kleinbuchstaben für Engschrift (`FNT-*`).

### Beobachtungen
- Aktuelles Webinterface skaliert bei langen Texten die ganze Darstellung statt auf feste 64×16 Pixel zu begrenzen.
- Aktuelle Engschrift besitzt keine echten Kleinbuchstaben; sie werden auf dem Controller in Großbuchstaben umgewandelt.

## [0.1.0] · 22.10.2026 · Erster funktionsfähiger Webcontroller

### Neu
- Eigenes WLAN `CoolLED1248` mit Offline-Webinterface unter `192.168.4.1`.
- Textfeld, 7 Einzelfarben, Helligkeitsregler, einfache Textvorschau.
- Bluetooth-Übertragung über CoolLEDX (`FFF0`/`FFF1`).
- Einzeilige 5×7-Engschrift, vertikal verdoppelt auf der 16-Pixel-Matrix.
- Automatischer Wechsel auf Laufschrift bei Textbreite über 64 Pixeln (temporäre Prototyp-Regel).
- Kein automatisches Überschreiben des Tafel-Inhalts beim Start.

### Geprüft am realen Gerät
- Smartphone verbindet sich mit ESP32-WLAN.
- Weboberfläche ist auf iPhone/Safari erreichbar.
- Text `TEST 123` gelangt vom Webinterface zur Tafel und erscheint passend zur Vorschau.
- Farbwechsel über das Webinterface funktioniert nach Nutzer-Rückmeldung.

### Bekannte Einschränkungen
- Bislang keine lokale Bibliothek, keine Textseiten, keine gemischten Fonts/Farben, keine Diagnose zum Herunterladen.
- Nur Großbuchstaben; 40 Zeichen Eingabelimit; fehlerhafte Vorschau-Skalierung langer Texte.
- Mobile Darstellung: Farbe Weiß in zweiter Zeile; Helligkeit `100 %` umgebrochen; Browserleiste überdeckt Bedienelemente.
- BLE-Adresse in der Firmware fest einprogrammiert; keine robuste Wiederverbindung nach fremder App-Steuerung getestet.

### Build-Konfiguration
- Arduino IDE 2.3.10; esp32 by Espressif 3.3.12; ESP32C3 Dev Module.
- Erste Kompilierung mit 1.310.720 B App-Maximum scheiterte: 1.312.457 B gebraucht, 1.737 B zu viel.
- Partitionierung auf `No OTA (2MB APP/2MB SPIFFS)` umgestellt; erfolgreicher Webbetrieb danach bestätigt.

## [Experiment: FirstText] · 22.10.2026 · BLE-Pixelübertragung
- Eigene mehrfarbige Anzeige `FUCK YES` erfolgreich auf die Tafel übertragen.
- Engschrift: 5×7-Pixelgrundform, auf 14 Pixel Höhe verdoppelt.
- Protokollbeispiele aus Community-GitHub-Projekten als technische Grundlage untersucht.

## [Experiment: BLE-Bring-up] · 22.10.2026 · Verbindung und Helligkeit
- ESP32-C3 erstmals per USB-C programmiert, Ausgabe im seriellen Monitor erfolgreich.
- Gerät `CoolLEDX` gefunden, Service `FFF0`, Characteristic `FFF1` ausgelesen.
- Schreibtyp `Write Without Response`; Benachrichtigungen verfügbar.
- Helligkeitsbefehl mit Maximalwert `0xFF` sichtbar erfolgreich getestet.
- LED-Tafel speichert bereits hochgeladene Inhalte über Stromverlust hinweg (Nutzerbeobachtung).

## Vorlage für die nächste Version

```markdown
## [x.y.z] · YYYY-MM-DD
### Neu / Geändert / Behoben
- ... (Themen-ID)
### Real getestet
- Gerät, Szenario, Ergebnis ...
### Noch offen / bekannte Fehler
- ...
### Build
- Arduino-/ESP32-Version, Partition, Speicherverbrauch, Besonderheiten
```
