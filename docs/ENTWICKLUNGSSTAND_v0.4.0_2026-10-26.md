# Entwicklungsstand v0.4.0-dev · 26.10.2026

## Ausgangspunkt

Wir haben v0.3.5.1 am 25.10.2026 auf unserer ESP32-C3-/CoolLEDX-Kombination mit erfolgreichen BLE-Aufträgen, ohne protokollierte Fehler und mit den neuen 16×16-Piktogrammen getestet. Der Frosch benötigt noch eine grafische Überarbeitung. v0.4.0 baut auf der unveränderten NVS-Seitenstruktur und dem bisherigen BLE-Übertragungsmechanismus auf.

## Unsere WebUX-Vorgaben

- Gemeinsamer fixierter Header auf jeder Seite, Banane als Menütaste.
- Startbildschirm: sechs große Kurzwahlen in 2×3-Anordnung ohne Scrollen; globale Helligkeit, Ein/Aus, Bearbeiten-Modus.
- Menü: Start (auf Startseite selbst ausgeblendet), Bibliothek, Programme, Texteditor, Ambiente, Diagnose, Einstellungen.
- Bibliothek: feste Toolbar *Editor · Löschen · Sortieren · Übertragen*, mittlerer Scrollbereich, globale Regler und Modus-Override im unteren Bereich. Drag-and-drop-Sortierung folgt mit der echten Speicherung.
- Programme: feste Toolbar *Neu · Löschen · Sortieren · Übertragen*, spätere Referenzen auf Bibliothekstexte; keine Kopien der Textinhalte. Änderungen an Bibliothekstexten sollen alle referenzierenden Programme aktualisieren.
- Texteditor: beide bisherigen Lauftext-/Blockseiten-Bereiche voll zugänglich. Gemischte Farben einzelner Zeichen sind für v0.4.3 vorgemerkt.
- Ambiente: sieben leuchtende Farbtasten, füllen bei Auswahl die ganze Tafel; Helligkeit unten.
- Diagnose: bisheriges Protokoll vollständig auf eigener Seite.
- Einstellungen: weitere BLE-Tafeln suchen/auswählen als noch nicht aktiver Platzhalter.
- Symbol-Picker: gemeinsamer thematischer Zeichenvorrat, Herz ♥ als Öffner und Pixelraster-Vorschaubilder direkt aus unseren Daten.

## Technische Änderungen

1. Die neue Navigation versteckt inaktive Bereiche; die bewährten Editor-Element-IDs und deren JavaScript-Aufrufe bleiben erhalten.
2. Helligkeit/Tempo sind **globale Laufzeiteinstellungen**. Die HTTP-Route `/api/text` liest beides jetzt aus der ESP32-Steuerung statt aus dem Formular. `/api/status` meldet die aktuellen Sollwerte `brightness` und `speed` für geräteübergreifende UI-Synchronisierung. Tempo 1–100 statt 60–100; Standard 80; Umrechnung `round(Tempo × 255 / 100)` bleibt unverändert.
3. Die neuen Ambiente-Aufträge verwenden `POST /api/ambience` (Feld `color`, Zahlen 1–7). Die Firmware erzeugt ein RGB-Bitplane-Bild von 64×16 Pixeln (107 Byte Header, 384 Byte Pixel, insgesamt 491 Byte) und nutzt **denselben** Bildtransport wie Lauftext und Blockseiten; der Modus wird auf Standbild gesetzt.
4. Der Picker zeichnet pro Motiv ein 16×16-Canvas; die Bilddaten stammen aus dem bestehenden 36-Motive-Flasharchiv, keine Fremd-Emoji-Illustrationen.
5. Die neue Bibliothek und Programme erhalten sichtbare, **nicht als aktiv getarnte** Platzhalter. Alte NVS-Seiten werden nicht verschoben oder migriert.

## Teststatus

**Statisch geprüft:** JavaScript-Syntax (Node), HTML-DOM (7 Ansichten), Browsernavigation und Pixel-Picker in mehreren Viewportgrößen; Startbildschirm ohne Scrollen bei 390×844, 360×640 und 1366×768 CSS-Pixeln; 7 Ambiente-Tasten vorhanden; Status-/Slider-Konzept.

**Noch nicht bestätigt:** Arduino-Kompilierung mit ESP32-Core 3.3.12, Speicherverbrauch, BLE-Ambiente-Farbwirkung, Anzeigeverhalten neuer Textaufträge und zeitgleicher Mehrgerätebetrieb. Die neue Firmware bleibt bis zum Test ein Entwicklungsstand und **kein hardwarebestätigtes Release**.

## Offen und bewusst verschoben

- v0.4.1: persistente Bibliothek mit Namens-/Textverwaltung, Drag-and-drop, Referenzschutz, Backup/Import und sechs echten Kurzzuordnungen.
- v0.4.2: Programmmodell, Größenberechnung, Kombinationen von Block/Lauftext und möglichst wenige sichtbare Uploads. Kompatibilität des Animationsprotokolls `0x04` unbestätigt.
- v0.4.3: Zeichen-/Abschnittsfarben im Editor.
- Weitere Piktogramme aus der neu hinzugefügten `docs/assets/Pixel-Art.xlsx` (GitHub): darunter neue Krabbe als Ersatz, neue Schneeflocke **zusätzlich**, Frosch später überarbeiten.
- BLE-Scan anderer Tafeln sowie Konfiguration der Geräteadresse im Einstellungsbereich.

Unsere Arbeitsweise: erst eine lesbare Oberfläche und saubere Versionsdokumentation, danach separat die Daten- und Programmverwaltung – ohne die bestehende Blockseitensammlung zu gefährden.
