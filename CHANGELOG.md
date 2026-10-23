# Changelog

> **Dokumentationsnachtrag 23.10.2026:** Das frühere v0.2.0-Paket bezeichnete Hardwaretests noch als ausstehend. Diese wurden nach **Korrektur 1** vom Nutzer tatsächlich durchgeführt. Historische „zu testen“-Listen in älteren Versionsabschnitten bleiben als damalige Planung erhalten und sind keine aktuellen Offenstände.

### [0.2.0] Praxisabnahme · 23.10.2026

- **13/14** ursprüngliche Testpunkte vom Nutzer mit „Ja“ beantwortet; die einzeilige Darstellung blieb unbewertet.
- **Bestätigt:** Start, iPhone-Webinterface, Seite hinzufügen/duplizieren/verschieben, unterschiedliche Farben, Mini in zwei Zeilen, automatische Seitenfolge, individuelle Anzeigedauer, NVS-Persistenz nach Neustart und wiederholte Uploads.
- **Bekannter Mangel:** Der ESP32 lädt bei jedem Seitenwechsel den gesamten Inhalt erneut hoch. Die tafelseitige **Prozentanzeige** erscheint jedes Mal. Der Player wird deshalb als technische Vorstufe einer späteren **Playlist zwischen Programmen** betrachtet.
- **Schriftentscheidungen:** Zwei Zeilen in der aktuellen Großbuchstaben-Mini beibehalten; Kleinbuchstaben in Mini künftig automatisch als Versalien rendern; „Mini, eine Zeile“ ergänzen; `ß/ẞ` mit Entfernen des blau markierten Pixels; `g/p/q/y` an gemeinsame x-Höhe und Unterlängen anpassen.
- **Editor-Beobachtung:** Gespeicherte Blockseiten bleiben bei Browser-Reload erhalten; **Lauftextentwurf und zuletzt gewählter Reiter nicht**. Bibliothek/Entwurfswiederherstellung geplant.
- **Forschung, noch nicht implementiert:** Einmaliger Upload mehrerer Seiten über nativen Blocktext oder `0x04`-Animation; OpenSign-Referenz ist **kein** Gerätetest unserer Tafel.
- **Dokumentationsentscheidung:** Kurze Excel-kopierbare Testtabelle zu jedem späteren **Firmwarepaket direkt im Chat**, keine zusätzliche Download-Datei.

---

## [0.2.0] – 23.10.2026 (Korrektur 1, auf echter Hardware getestet)

### Korrektur 1 · 23.10.2026 (Kompilier-Hotfix)
- HTML und JavaScript aus dem `.ino` in `RavestickBoard_WebUI.h` ausgelagert, um fehlerhafte automatische Arduino-Funktionsprototypen (`function does not name a type`) zu verhindern.
- `tools/sync_web.py` aktualisiert jetzt die Headerdatei.
- Sketch und Header müssen gemeinsam im gleichen Arduino-Sketch-Ordner liegen.
- Sonst keine beabsichtigten Funktionsänderungen; reale ESP32-Kompilierung wurde anschließend vom Nutzer bestätigt (23.10.2026).


### Neu
- Getrennte Bereiche für Lauftext und Blockanzeige.
- Bis zu zwölf eigenständige Seiten mit je einer 14-Pixel-Zeile oder zwei 7-Pixel-Zeilen, eigenständigen Zeilenfarben, Ausrichtung und Anzeigedauer.
- Seiten hinzufügen, duplizieren, verschieben, löschen, speichern und einzeln hochladen.
- Nichtflüchtige Speicherung des Seitenprogramms im NVS, ohne automatische Wiedergabe nach dem Start.
- Experimenteller ESP32-Seitenplayer mit unabhängigen BLE-Uploads, manueller Start-/Stoppsteuerung und Fehlerabbruch.
- Vorschau mit festen 64×16 LED-Punkten, seitenweiser Abfolge und 1×/2×/4× Fast Forward.
- Großes ẞ zusätzlich zum kleinen ß, mit bewusst gleicher komprimierter Glyphenform.
- Diagnose-Zähler für Seitenübertragungen und Speichervorgänge.

### Korrekturen und Entscheidungen
- Im Seiteneditor keine gekappten Buchstaben und kein automatischer Seitenumbruch.
- Verschiedene Seiten werden unabhängig mit ihren eigenen Parametern gespeichert.
- Die zuvor gefundene Arduino-Typ-Prototyp-Falle (Job) bleibt behoben.
- Firmwarequellcode und `web/index.html` durch `tools/sync_web.py` synchronisierbar.

### Noch offen
- **Erledigt am 23.10.2026:** ESP32-Arduino-Kompilierung nach Korrektur 1 sowie BLE-/NVS-Grundtests. Offen bleiben spezielle Regressionen und Dauerbelastung.
- Farb- und Schriftwechsel pro Zeichen sowie Rekonstruktion der Herstellerfonts.
- Live-BLE-Regler für Helligkeit/Geschwindigkeit, ohne Verbindungsaufbau je Schritt.
- Prüfung nativer Mehrseitenfähigkeit der Tafel und verkürzter Umschaltzeiten.
- Schriftformen g, p, q, y, Ä, Ö, Ü gemäß Original-Referenzen verfeinern.

---

# Änderungs- und Testhistorie

**Prinzip:** Tatsächlich getestete Funktionen und geplante Änderungen getrennt führen. Eine neue Versionsnummer bezeichnet eine bewusst archivierte Firmware, nicht nur eine Idee.

## [0.1.2] · 22.10.2026 · Nach Compilerkorrektur auf ESP32 gestartet; detaillierte Glyphenprobleme später gesammelt

### Neu und verbessert (damals zunächst implementiert; teilweise später auf Hardware beobachtet)
- **Deutsche Oberfläche und Groß-/Kleinschreibung:** alle UI-Texte deutsch und mit Umlauten/ß; 26 echte Kleinbuchstaben und ä/ö/ü zusätzlich zum bisherigen Zeichensatz (`FNT-02`).
- **Font aus einer Quelle:** Das Webinterface bezieht dieselbe Engschrift-Glyphentabelle wie der ESP32 über `/api/font`.
- **LED-Vorschau als Punktmatrix:** stets genau 64×16 runde LEDs, auch bei langem Text; keine automatische Verkleinerung (`PRE-01`, `PRE-02`, `PRE-03`).
- **Vorschau-Player:** Play/Pause, Neustart, 1×/2×/4×/8× (`PRE-04`). Browsergeschwindigkeit vorläufig angenähert, noch kein echter Abgleich.
- **Anzeigemodus wählbar:** Standbild oder Lauftext unabhängig von Textlänge; lange Standbilder zeigen einen Ausschnitt und eine Überlaufwarnung (`EDT-01`, `EDT-09` teilweise). Seiteneditor bleibt offen.
- **Laufgeschwindigkeit:** UI-Regler (1–100), sendet dokumentierten CoolLEDX-Speed-Befehl `0x07` mit umgerechneten Rohwerten 0–255. Unkalibrierter Hardwaretest (`EDT-10` teilweise).
- **Diagnose kopieren:** direkter Clipboard-Versuch, Fallback und manuell auswählbarer Volltext; vorhandener TXT-Download bleibt (`DIA-10`).
- **Resetgrund / Speicher:** `USB-Reset` statt Code 11; freie App-Bytes über aktive Firmwarepartition statt der bisher irreführenden `getFreeSketchSpace()`-Angabe.
- **BLE-Fehlerfall:** Hinweise auf konkurrierende Hersteller-App, Verbindungsversuch mit auf 10 s gesetztem Timeout (muss real getestet werden).

### Intern geprüft
- JavaScript mit Node.js auf Syntax geprüft.
- C++ mit Arduino-API-Dummys syntaktisch geprüft; **keine echte Arduino-Kompilierung**.
- Webinterface bei 390 px Breite in Chromium mit simulierten API-Antworten geprüft: Umlaute/Zählung, Standbild/Lauftext, Player, Diagnose, Kopieren.
- Fontdefinition für Groß-/Kleinbuchstaben einschließlich `ÄÖÜäöüß` automatisiert überprüft.

### Zum damaligen Zeitpunkt noch nicht bestätigt
- ESP32-Kompilierung mit Espressif 3.3.12 und reale Flash-Belegung; **inzwischen nach Korrektur bestätigt**.
- Tatsächliche Zeichendarstellung von Groß-/Kleinbuchstaben und Umlauten auf der Tafel.
- Tatsächliches Lauftext-Tempo und Richtung; Einfluss der Rohwertskala.
- Clipboard-Fallback auf iPhone/Safari unter unverschlüsseltem `http://192.168.4.1`.
- Statischer Überlauf: tatsächlicher Ausschnitt auf dem Board kann noch von der Simulation abweichen.
- BLE-Timeout, Stabilität und Heap bei mehreren Aufträgen.

## [0.1.1] · 22.10.2026 · Auf Hardware bestätigt

### Nachträglich am Gerät geprüft
- WLAN-Webinterface und Header mit v0.1.1 auf iPhone sichtbar.
- Diagnoseansicht und TXT-Export funktionieren auf iPhone.
- Reale BLE-Diagnose: fünf Textaufträge, fünf erfolgreiche Verbindungen, fünf erfolgreiche Jobs, 20 Notifications und keine Timeout-/Schreibfehler in diesem Test.
- Erfolgreicher erneuter BLE-Auftrag nach Freigabe einer Bluetooth-Verbindung durch die Original-App. Während die Original-App verbunden blieb, scheiterte der Auftrag wie erwartet nach ca. 30 Sekunden.
- Von RavestickBoard hochgeladene Inhalte können über die Original-App anschließend in Wiedergabemodus und Helligkeit verändert werden.

### Noch nicht getestet
- Mehrstündige Speicherstabilität; Fehlerhistorie nach Stromverlust (RAM-Ringpuffer wird zurückgesetzt).


### Implementiert (Code erstellt; Gerätetest ausstehend)
- **Rebranding:** Name „RavestickBoard for CoolLED1248“ und sichtbare Versionsnummer `v0.1.1` im Header. WLAN-SSID `CoolLED1248` und bisheriger Verbindungsweg bleiben bewusst unverändert.
- **Diagnose:** aufklappbare Systemdaten im Webinterface, inklusive Uptime, Resetgrund, freiem/minimalem Heap, größtem Speicherblock, Sketchgröße und freiem App-Platz.
- **Log-Export:** TXT-Download unter `/api/diagnostics.txt`, JSON-Status unter `/api/diagnostics`, fester RAM-Ringpuffer für 56 Ereignisse; keine dauernden Flash-Schreibvorgänge.
- **BLE-Metriken:** Verbindungsversuche, Erfolge, Disconnects, lokale BLE-Write-Fehler, Notifications, Timeouts, Paket- und Bytezähler, letzter fehlgeschlagener Auftrag.
- **Mobile UI:** sieben Farben in einer Rasterzeile, Helligkeitswert mit `white-space:nowrap`, zusätzliche Scrollreserve für Safari-Toolbar und iPhone-Safe-Area.
- **Datensparsamkeit:** Keine Texte, Passwörter oder Geräteadressen im TXT-Bericht. Fehlerverlauf seit Systemstart, keine über Neustarts persistierte Historie.

### Intern geprüft
- JavaScript syntaktisch mit Node.js geprüft; HTML/CSS als mobile Chromium-Ansicht gerendert (keine ESP32-Verbindung).

### Ursprüngliche Testcheckliste (danach teilweise bestätigt)
- Arduino-Kompilierung und Flash unter ESP32 Core 3.3.12 mit `No OTA (2MB APP/2MB SPIFFS)`.
- WLAN-Webseite auf iPhone/Safari, v0.1.1 sichtbar, Diagnose und Download geprüft.
- Textübertragung, Farben und separate Helligkeit gegenüber v0.1.0 regressionsgeprüft.
- Erfolgreicher und fehlgeschlagener BLE-Auftrag erscheinen nachvollziehbar im Download.
- Heap und größte Blockgröße nach wiederholten BLE-Aufträgen; Reset löscht das Ereignislog.

### Unveränderte Einschränkungen
- Vorschau langer Texte noch nicht pixelgenau, keine Kleinbuchstaben, Auto-Lauftext noch aktiv.
- Diagnose im RAM verliert Ereignisse bei Neustart. Keine vollständige Ursache für Fremd-BLE-Steuerung erkennbar.

## [Historische Planung vor v0.2.0] · ursprüngliche Vorschläge ab 22.10.2026

### Geplant
- Nach realem v0.1.1-Gerätetest Diagnosefunktionen als bestätigt markieren; später Diagnose-Persistenz prüfen (`DIA-*`).
- Vorschau als echte 64×16-LED-Punktmatrix, Lauftext-Simulation, 1×/2×/4×/8× und Vorspulen (**8× später ausdrücklich verworfen; aktuell 1×/2×/4×**) (`PRE-*`).
- Mobile-UI-Korrekturen aus v0.1.1 auf realem iPhone/Safari verifizieren (`UI-*`).
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
