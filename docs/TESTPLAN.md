# RavestickBoard · Hardwaretests und Abnahmeprotokolle

**Stand: 23.10.2026 · Firmware: v0.2.0 Korrektur 1.**

Dieses Dokument hält die **realen Ergebnisse** und weiter offenen, gezielt nachprüfbaren Testfälle fest. Es ist **nicht** die vom Nutzer gewünschte Excel-Datei: **Zu jeder neuen Firmware liefern wir die kurze, direkt in Excel kopierbare Testtabelle ausschließlich im Chat**, mit leeren Spalten `Ergebnis` und `Bemerkungen` (siehe `ENTSCHEIDUNGEN.md`, ADR-028).

## 1. Bereits protokollierter Nutzer-Praxistest v0.2.0 (23.10.2026)

Die Ergebnisangaben wurden direkt aus der vom Nutzer im Chat zurückgegebenen 14-Punkte-Tabelle übernommen. **„Ja“ heißt funktional beobachtet, nicht mängelfrei.**

| Nr. | Bereich | Ursprünglicher Testpunkt | Ergebnis | Beobachtungen und Folgeaufgaben |
| --- | --- | --- | --- | --- |
| 1 | System | ESP32 startet nach Einschalten | **Ja** | Nach v0.2.0 Korrektur 1 |
| 2 | Webinterface | Oberfläche auf iPhone erreichbar | **Ja** | Browser verbindet sich mit ESP32-WLAN |
| 3 | Blockeditor | Neue Seite hinzufügen | **Ja** | |
| 4 | Blockeditor | Seite duplizieren | **Ja** | |
| 5 | Blockeditor | Seitenreihenfolge ändern | **Ja** | |
| 6 | Blockeditor | Unterschiedliche Farben verwenden | **Ja** | Zwei Mini-Zeilen in unterschiedlichen Farben sichtbar |
| 7 | Schrift | Einzeilige Darstellung | **Offen** | Zusätzlich „Mini, eine Zeile“, ca. 7 Pixel, vertikal zentriert gewünscht |
| 8 | Schrift | Zweizeilige Darstellung | **Ja** | Großbuchstaben sehr gut; **Grundform beibehalten** |
| 9 | Schrift | Kleinbuchstaben und Unterlängen | **Ja, mit Mangel** | Kleinbuchstaben in Mini unleserlich; Mini künftig **immer Versalien** anzeigen |
| 10 | Schrift | Umlaute und Sonderzeichen | **Ja, mit Mangel** | ß/ẞ: im Nutzerbild blau markierten unteren Pixel deaktivieren |
| 11 | Wiedergabe | Automatischer Seitenwechsel | **Ja, mit Mangel** | Bei jedem Seiten-Upload läuft eine Prozentanzeige der Tafel |
| 12 | Wiedergabe | Individuelle Anzeigedauer | **Ja, mit Mangel** | Zeiten wirken, aber zwischen Seiten Upload samt Prozentanzeige |
| 13 | Speicherung | Seiten bleiben nach Neustart erhalten | **Ja** | NVS-Persistenz bestätigt; zusätzlich Browser-Reload der Blockseiten bestätigt |
| 14 | Stabilität | Mehrere Programme nacheinander übertragen | **Ja** | Aktueller ESP32-Seitenplayer ist mögliche Grundlage künftiger Programmplaylist |

**Zusätzliche Nutzerbeobachtung:** Lauftext-Entwurf und zuletzt geöffneter Reiter werden nach Browser-Reload **nicht wiederhergestellt**; gespeicherte Blockseiten dagegen schon. Die ursprüngliche Tabelle enthält dazu noch keinen separaten Punkt.

## 2. Offene Regressionstests der funktionierenden v0.2.0

Die Kästchen unten bleiben offen, sofern der spezielle Teiltest nicht mit einem eigenen Befund abgenommen wurde. **Nicht mit dem allgemeinen „Ja“ aus Abschnitt 1 verwechseln.**

### Firmware und Datenhaltung

- [ ] Firmware bei Kaltstart und mehreren USB-Neustarts ohne spontanen Upload stabil.
- [ ] Grenzen **12 Seiten**, 10 Unicode-Zeichen je Zeile, leere Seite, UTF-8-Länge und ungültige Zeit/Farbe/Ausrichtung prüfen.
- [ ] Blockseite speichern, Browser neuladen, alle Felder/Zeilenfarben/Anzeigedauer unverändert; danach USB trennen und erneut prüfen.
- [ ] Seitenumordnung/duplizieren/löschen und NVS-Persistenz nach Wiederanlauf erneut prüfen.

### Grafik und Vorschau

- [ ] „Mini, eine Zeile“ erst **nach Implementierung** testen; bei v0.2.0 nicht als vorhandene Option behandeln.
- [ ] Kleinbuchstaben- und Umlauttest für große Engschrift (g/p/q/y, Ä/Ö/Ü, ß/ẞ, j und Satzzeichen).
- [ ] Überlaufhinweis: keine zerteilten Buchstaben, keine ungewollte Folgeseite, keine automatische Verkleinerung.
- [ ] Vorschau: 64×16 LED-Punkte, dunkel/leuchtend, Position/Farben korrekt; Player Play/Pause, 1×/2×/4×, kein 8×.
- [ ] Mobil: iPhone-Safari/Safe-Area, Farbtasten, Helligkeitsbeschriftung, Anzeigesperre/Rotation.

### Player, BLE, Diagnose

- [ ] Seite 1 und zweizeilige Seite 2 nacheinander senden, reale Transferdauer und Sichtzeit dokumentieren.
- [ ] Seitenplayer stoppen: letztes Bild bleibt; danach manuell anderes Bild anzeigen.
- [ ] Konflikt Original-App / erneute Freigabe: Fehlermeldung, Timeout und Wiederaufnahme ohne Neustart prüfen.
- [ ] Diagnose „Log kopieren“ auf dem iPhone, gegebenenfalls Safari-Fallback und TXT-Export getrennt prüfen.
- [ ] Diagnosefelder: Heap/Low-Water/Block, Resetgrund, NVS-Seitenspeicherungen, automatische Wechsel, Erfolge/Fehler vor und nach längerer Wiedergabe.
- [ ] Dauerbetrieb, wiederholte BLE-Verbindungstrennung, Stromverlust im Speichervorgang.

## 3. Noch nicht implementierte Funktionen (spätere Versionstests)

- [ ] Slider für Helligkeit/Speed **live** auf dem Board, ohne erneuten Textupload; Rohwertkalibrierung gegen Original-App.
- [ ] Direkte Moduswechsel `0x06` ohne Textupload: Block, rechts/links, Schneefall, Laser usw. kontrolliert testen.
- [ ] Mehrseitiges **Programm in einem Upload** (`0x02`/`0x04`) statt BLE-Seite-für-Seite; keine Prozentanzeige zwischen Seiten.
- [ ] Eigenständige Animationen `0x04`: Größe, Frames, Dauer/Loop, Stromausfall und ACKs messen.
- [ ] Einzeilige Mini, automatische Großbuchstaben in Mini, ß-Pixelkorrektur und Engschrift-Unterlängen.
- [ ] Bibliothek LittleFS, Entwurfswiederherstellung, Export-/Import, Favoriten und Playlists.

## 4. Evidenz- und Dokumentationsregel

`nicht geprüft` → `experimentell` → `auf unserer Tafel bestätigt`. Ein anderer CoolLEDX-Protokolltreiber ist kein Ersatz für einen eigenen Test. Zu jedem Befund Firmwarestand, konkretes Gerät, Testschritte und Ergebnis notieren; Status erst dann in `THEMENSPEICHER.md`, `ROADMAP.md` und `CHANGELOG.md` aktualisieren.
