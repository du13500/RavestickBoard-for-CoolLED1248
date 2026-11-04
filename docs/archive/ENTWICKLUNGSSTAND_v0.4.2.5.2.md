# Entwicklungsstand v0.4.2.5.2-dev

## Ziel
Acht frei belegbare Kurzwahlen, volle Bildschirmhöhe der Startseite nutzen, Globaltempo auch auf Startseite, Favoriten-UI in Bibliothek entfernen. Die sechs alten Belegungen dürfen bewusst neu eingerichtet werden.

## Daten und Migration
- Die Bibliothek (LittleFS) und Programme bleiben unverändert erhalten.
- Acht Kurzwahlen werden in einer neuen RBS3-Datei `/rb-lib/shortcuts3.bin` verwaltet, statt das alte RBS2-Sechs-Slot-Record zu vergrößern.
- Eine automatische Migration der sechs alten Belegungen ist **nicht** vorgesehen. Alle acht Plätze können neu belegt werden.
- Die alte RBS2-Datei wird nicht gelöscht oder überschrieben. Das Bibliotheksmetadatenjournal bleibt in seinem bisherigen Format.
- In JSON-Backups mit Schema 2 werden bis zu acht typisierte Kurzwahlen exportiert und wieder eingelesen. Alte Sechs-Platz-Backups bleiben importierbar.
- Favoritenbits in Bibliothekseinträgen werden nicht gelöscht. Nur die dafür bislang zwecklose Oberfläche entfällt.

## UI
- Grid 2 × 4 mit flexibler Höhenaufteilung, 100dvh und Safe-Area-Abstand.
- Tafelsteuerung im Dokumentfluss, auf Startseite zwei Slider: Helligkeit und Geschwindigkeit.
- Keine Überlagerung bei kurzer Bildschirmhöhe; vertikales Scrollen ist dann zulässig.
- Bootscreen mit dem gleichen lokal eingebetteten Lucide-Bananen-SVG wie das Menü.
- Mini-Schrift: ein Pixel in der letzten Rasterzeile von ß/ẞ abgeschaltet.

## Prüfstand
- Offline: gzip/ETag-Rundtrip, HTML- und JS-Syntax, generierter C++-Header, JSON-Chunkwriter: erfolgreich.
- Chromium / simulierte API: 8 Buttons, Menünavigation, Bibliothek ohne Favoritenstern, Geschwindigkeitsregler, Layout 390×844, 393×852, 430×932, 390×667, 375×580 und 844×390: erfolgreich.
- **Noch nicht geprüft:** Arduino-Gesamtcompile, physische ESP32-C3-Hardware, tatsächliche BLE-Übertragung, acht Kurzwahlen über Neustarts auf LittleFS und RAM-Stabilität.

## Weiterhin offen
- Gefährliche historische Heap-Minima aus 0.4.2.4.2-dev-A/B; kein Stabilitätsnachweis in diesem UI-Build.
- Nach Test der Kurzwahl-/Layoutänderungen RAM-Profilierung getrennt fortführen.
