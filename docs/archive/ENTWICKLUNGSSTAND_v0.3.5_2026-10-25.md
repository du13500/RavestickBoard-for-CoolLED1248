# Entwicklungsstand RavestickBoard v0.3.5 – 25.10.2026

**Buildstatus:** Quellcode/Paket erzeugt; JavaScript-Syntax, Bitmap-Konsistenz und Archivstruktur geprüft. **Noch nicht per Arduino IDE kompiliert und noch nicht mit der echten Tafel getestet.**

## Vererbt aus v0.3.4 (hardwarebestätigt)

- 8 Modi 0x01..0x08 per 0x06, jeweils ohne erneuten Bild-Upload; 0x07 künftig „Aufdecken“.
- Helligkeit und Tempo als separate BLE-Kurzbefehle; Blockseiten, 5 Schriften, Mini-Zweizeile mit zwei Farben und zulässiger Symbolüberlagerung.
- Die bisherige 30-KiB-Grenze ist eine experimentelle Sicherheitsgrenze, kein nachgewiesenes maximales Hardwarelimit.

## Neu in v0.3.5

1. Ein/Aus über `0x09 0x00/01` ohne Datenupload (Referenzimplementation, auf unserer Tafel noch unbestätigt).
2. Geschwindigkeit 60..100, Startwert 80, BLE 0..255 linear wie zuvor.
3. Geänderte Beschriftung „Anzeigemodus erzwingen (keine Vorschau möglich)“ und Aufdecken.
4. Kussmund, Blume, Biene und Kuh aus 16×16-App-Screenshots abgelesen; alte Piktogramme erhalten.
5. Neue eigene 16×16-Motive Banane (vertikal gespiegelt), Quietscheente, Kaktus, UFO, UFO+Strahl, Alien (grün, schwarze/unbeleuchtete Augen).
6. Rechter Zeichenvorschub bei Blume um 1 Pixel reduziert; kein Wechsel der NVS-Struktur.

## Testprotokoll zum Ausfüllen

| Nr. | Bereich | Prüfung | Ergebnis | Bemerkung |
|---|---|---|---|---|
| 01 | Build | Sketch v0.3.5 kompiliert und flasht | | |
| 02 | Start | WLAN/Webinterface, korrekte v0.3.5 | | |
| 03 | NVS | Gespeicherte Blockseiten unverändert | | |
| 04 | EIN/AUS | Ausschalten ohne Prozentanzeige | | |
| 05 | EIN/AUS | Einschalten ohne Prozentanzeige, letzter Inhalt bleibt | | |
| 06 | EIN/AUS | Log zeigt POWER 0x09 0x00/0x01 | | |
| 07 | Geschwindigkeit | Regler 60..100, Standard 80 | | |
| 08 | Geschwindigkeit | 60, 80 und 100 ändern Bewegung | | |
| 09 | Modus | 8 Modi vorhanden, Aufdecken statt Bild | | |
| 10 | Modus | Überschrift „Anzeigemodus erzwingen (keine Vorschau möglich)“ | | |
| 11 | Symbol | Kussmund gegenüber Screenshot (Farbe/Form/Abstand) | | |
| 12 | Symbol | Blume mit engerem rechten Abstand | | |
| 13 | Symbol | Biene gegenüber Screenshot | | |
| 14 | Symbol | Kuh gegenüber Screenshot | | |
| 15 | Symbol | Quietscheente | | |
| 16 | Symbol | Ravestick-Banane vertikal gespiegelt | | |
| 17 | Symbol | Kaktus | | |
| 18 | Symbol | UFO ohne Strahl | | |
| 19 | Symbol | UFO mit Traktorstrahl als eigene Variante | | |
| 20 | Symbol | Alien grün, Augen schwarz/unbeleuchtet | | |
| 21 | Symbol | Stern vertikal zentriert | | |
| 22 | Regression | Mini Zweizeilig: Symbolüberlagerung weiterhin erlaubt | | |
| 23 | Regression | Alle 5 Schriften und beide Farben | | |
| 24 | Regression | Blockseitenprogramm und bestehende Piktogramme | | |
| 25 | Stabilität | Mehrfachsteuerung/Upload ohne Fehler | | |

**Diagnose:** Export über Webinterface und serielles Log aufheben. Ein/Aus testweise vor dem Langtext-Upload testen, falls die Tafel nach `0x09 0x00` nicht über BLE wieder eingeschaltet werden kann. Nicht „Erase All Flash“ ausführen.

## Ausdrücklich später

- Kalibrierung und Berechnung der Lauftextdauer für zukünftige Playlists nur horizontal; keine Berechnung für Aufwärts/Abwärts/Schneeflocke/Laser/Aufdecken.
- Bibliotheks-/Playlist-Editor, NVS-Migration, umfangreiche Refaktorierungen und Backup-Export.
