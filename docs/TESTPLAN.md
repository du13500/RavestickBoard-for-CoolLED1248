# RavestickBoard · Hardwaretests und Abnahmeprotokolle

## Abnahme-Nachtrag 25.10.2026 – reale Hardwaretests v0.3.2 Korrektur 2

**Getestet / bestätigt:** Arduino-Kompilierung, AP/Webinterface, vorhandene NVS-Seiten, mobile Symbolpalette mit Mehrfach-Einfügung ohne iPhone-Tastatur, fünf Lauftextschriften, Mini Zweizeilig synchron, leere neue Texte, Zähler normal bis 600 und rot/fett ab 601, keine Popup-/Kürzungsreaktion. 600 normale Textzeichen wurden erfolgreich dargestellt. Bei gemischten Symbolen funktionieren 300, 320 jedoch nicht; 320 Blitze funktionieren, 320 Warnsymbole nicht. **Nicht abgenommen:** separate Schriftfarbe beider Mini-Lauftextzeilen, gesicherte Tafeldatengrenze, eindeutiges Display-Abschluss-ACK, Langzeitstabilität.

| Nächster Grenztest | Erwartete Payload aus gegenwärtiger Renderbreite | Hardwareergebnis |
| --- | ---: | --- |
| 300 × `⚠️` | 30.701 B | noch offen |
| 301 × `⚠️` | 30.803 B | noch offen |
| 302 × `⚠️` | 30.905 B | noch offen |
| Lange reine Texte >600 nach kontrolliertem Firmwareumbau | abhängig von Schrift/Breite | nicht mit v0.3.2 regulär möglich |

**Gesamte Messreihe:** [`ENTWICKLUNGSSTAND_v0.3.2_2026-10-25.md`](ENTWICKLUNGSSTAND_v0.3.2_2026-10-25.md). Die folgenden ursprünglichen Testtabellen bleiben als **historische, vor dem Test erstellte Checklisten** dokumentiert; deren leere Ergebnisfelder sind **kein aktueller Negativstatus**.

---

## v0.3.2 · Korrektur 2 · gezielter WLAN/RAM-Wiederholungstest (25.10.2026)

**Bestätigte Vorbedingung:** Korrektur 1 wurde kompiliert/geflasht, zeigte aber `wifi:alloc pp wdev funcs fail` beim Start. Das NVS enthält weiter 10 Blockseiten. Korrektur 2 war beim Erstellen dieses ursprünglichen Testplans noch nicht hardwaregeprüft; der Nutzer bestätigte anschließend Kompilierung, WLAN-Start und weitere Funktionen (siehe Nachtrag oben).

| Nr. | Testpunkt | Ergebnis | Bemerkungen |
|---:|---|---|---|
| 1 | Korrektur 2 kompiliert und wird geflasht | | |
| 2 | WLAN `CoolLED1248` startet ohne `alloc pp wdev funcs fail` | | |
| 3 | Webinterface unter `192.168.4.1` erreichbar | | |
| 4 | `NVS`: 10 alte Blockseiten geladen | | |
| 5 | `[RAM]`-Zeilen beim Start sichtbar | | |
| 6 | Kurzer Lauftext wird übertragen und angezeigt | | |
| 7 | Mini Zweizeilig / eine Blockseite wird übertragen | | |
| 8 | Langer Text 80/100/200/400/600 nur schrittweise testen | | |
| 9 | Nach Übertragung bleibt WLAN/Webinterface erreichbar | | |
| 10 | Neustart: WLAN und zehn Blockseiten bleiben erhalten | | |

---

## v0.3.2 · Testprotokoll (25.10.2026)

**Historischer Plan vor dem Hardwaretest:** Vor Langtext-Experimenten war v0.3.1 als Rückfallstand aufzubewahren; der v0.3.2-Korrektur-2-Test wurde inzwischen durchgeführt. Bitte bei längeren Texten Paketanzahl, Bytes, Laufzeit und sichtbares Ergebnis festhalten.

| Nr. | Bereich | Testpunkt | Ergebnis | Bemerkungen |
|---:|---|---|---|---|
| 1 | Arduino | Sketch v0.3.2 kompiliert | | |
| 2 | Start | Webinterface v0.3.2 und gespeicherte Seiten vorhanden | | |
| 3 | Mobil | Symbolpalette breit, lesbar, kein abgeschnittener Inhalt | | |
| 4 | Mobil | 5 Symbole nacheinander einfügen, Palette bleibt offen | | |
| 5 | Mobil | Beim Einfügen erscheint keine iOS-Tastatur | | |
| 6 | Lauftext | Alle fünf Schriften verfügbar, Darstellung auf Board | | |
| 7 | Lauftext | Mini Zweizeilig, beide Zeilen scrollen synchron | | |
| 8 | Texte | Keine Demo-Feldwerte bei neuem Entwurf / neuer Seite | | |
| 9 | Texte | Alte Entwürfe und gespeicherte Seiten bleiben erhalten | | |
| 10 | Zähler | 600 normal; ab 601 rot/fett; keine Kürzung | | |
| 11 | Limit | Über 600: keine Übertragung, kein Popup | | |
| 12 | BLE | 80, 100, 150, 200 Zeichen | | |
| 13 | BLE | 300, 400, 500, 600 Zeichen, nur bei Erfolg der Vorstufen | | |
| 14 | BLE | 100+ Symbole: Payload, Pakete und Boardverhalten | | |
| 15 | BLE | Helligkeit, Tempo, Modus und Blockseiten-Player unverändert | | |
| 16 | Stabilität | BLE-Fehler und erneuter Versuch; serielles Log | | |


## v0.3.1 · Testpunkte vor Commit/Push (24.10.2026)

**Die Firmware ist noch nicht am ESP32 getestet.** Vor dem Flash prüfen, dass die alten gespeicherten Seiten unter v0.3.0 vorhanden sind; bei Problemen v0.3.0 wieder per USB aufspielen. Keine automatische Migration oder Löschung der NVS-Seiten vorgesehen.

| Nr. | Bereich | Testpunkt | Ergebnis | Bemerkungen |
|---:|---|---|---|---|
| 1 | Arduino | Neuer Sketch v0.3.1 mit beiden Headern kompiliert | | |
| 2 | Start | WLAN, Webinterface, Versionsanzeige v0.3.1 | | |
| 3 | NVS | Alle bestehenden Blockseiten und ihre Inhalte unverändert | | |
| 4 | Lauftext | Erster Aufruf ohne „Grüße!“, vorhandener eigener Entwurf bleibt | | |
| 5 | Lauftext | Text eingeben, Seite neu laden, Entwurf wieder vorhanden | | |
| 6 | Lauftext | 40, 50, 60, 80 Zeichen: HTTP-Annahme und LED-Ergebnis notieren | | |
| 7 | Lauftext | 10 / 20 / 40 Symbole: HTTP, BLE-Pakete, tatsächliche Darstellung | | |
| 8 | Schrift | Mini zweizeilig: verkürzte Leerzeichen wie Vorschau | | |
| 9 | Schrift | Mini einzeilig zentriert trotz kürzerer Leerzeichen | | |
| 10 | Symbole | Textcursor mittig, Symbol einsetzen, Position korrekt | | |
| 11 | Symbole | Eingefügtes Symbol mit Undo entfernen und Redo zurückholen | | |
| 12 | Undo | Mehrere Buchstaben löschen; ein Undo stellt den gelöschten Bereich wieder her | | |
| 13 | Undo | Blockseite Zeile 1 und 2 separat ändern, undo/redo, speichern | | |
| 14 | Mobile | Vorschau bleibt beim Scrollen sichtbar, blockiert keine Schaltflächen | | |
| 15 | Live-BLE | Helligkeit ohne separaten Senden-Button; Tempo/Modus weiterhin | | |
| 16 | Blockseiten | Vorhandene Seiten zeigen, automatisch spielen, nach Neustart noch vorhanden | | |
| 17 | Stabilität | Mehrmals nacheinander übertragen, Log auf HTTP/BLE-Fehler prüfen | | |
| 18 | Fallback | Falls nötig v0.3.0 erneut flashen, Altseiten bleiben erhalten | | |

**Für den Hardwarevergleich besonders wichtig:** Bei längeren Texten die tatsächliche Anzeige, Wartezeit, Prozentanzeige und Diagnose notieren. HTTP-Annahme beweist keine Board-Unterstützung. Undo gilt in v0.3.1 ausschließlich für Textfeldbearbeitung und nicht für komplette Seitenaktionen.

---

**Stand: 24.10.2026 · Letzter hardwarebestätigter Firmwarestand: v0.3.0-dev (Nutzerprüfung). Neuer, noch ungetesteter Quelltextkandidat: v0.3.1.**

Dieses Dokument hält die **realen Ergebnisse** und weiter offenen, gezielt nachprüfbaren Testfälle fest. Es ist **nicht** die vom Nutzer gewünschte Excel-Datei: **Zu jeder neuen Firmware liefern wir die kurze, direkt in Excel kopierbare Testtabelle direkt im Chat**, mit leeren Spalten `Ergebnis` und `Bemerkungen` (siehe `ENTSCHEIDUNGEN.md`, ADR-028).

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

## 3. Zum Stand v0.2.0 noch nicht implementierte Funktionen (historische Planung)

- [ ] Slider für Helligkeit/Speed **live** auf dem Board, ohne erneuten Textupload; Rohwertkalibrierung gegen Original-App.
- [ ] Direkte Moduswechsel `0x06` ohne Textupload: Block, rechts/links, Schneefall, Laser usw. kontrolliert testen.
- [ ] Mehrseitiges **Programm in einem Upload** (`0x02`/`0x04`) statt BLE-Seite-für-Seite; keine Prozentanzeige zwischen Seiten.
- [ ] Eigenständige Animationen `0x04`: Größe, Frames, Dauer/Loop, Stromausfall und ACKs messen.
- [ ] Einzeilige Mini, automatische Großbuchstaben in Mini, ß-Pixelkorrektur und Engschrift-Unterlängen.
- [ ] Bibliothek LittleFS, Entwurfswiederherstellung, Export-/Import, Favoriten und Playlists.

## 4. Evidenz- und Dokumentationsregel

`nicht geprüft` → `experimentell` → `auf unserer Tafel bestätigt`. Ein anderer CoolLEDX-Protokolltreiber ist kein Ersatz für einen eigenen Test. Zu jedem Befund Firmwarestand, konkretes Gerät, Testschritte und Ergebnis notieren; Status erst dann in `THEMENSPEICHER.md`, `ROADMAP.md` und `CHANGELOG.md` aktualisieren.

## 24.10.2026 · Zusätzliche Tests für v0.3.0 (alle noch offen)

- [ ] Arduino IDE mit `ESP32C3 Dev Module`, `USB CDC On Boot: Enabled`, `No OTA (2MB APP/2MB SPIFFS)`: **drei Dateien** im selben Sketchverzeichnis fehlerfrei kompilieren.
- [ ] Nach Flash: vorhandene v0.2.0-NVS-Blockseiten erscheinen unverändert; keine automatische Übertragung beim Neustart.
- [ ] Normal/Bold am realen Board für A, Ä, ß, Ziffern, Satzzeichen testen und mit App-Screenshots abgleichen.
- [ ] Mini einzeilig an beiden Achsen zentriert, auch mit kurzer/langen Texten; Mini zweizeilig unverändert gut lesbar.
- [ ] Mini wandelt äöüß automatisch zu ÄÖÜẞ in der **Anzeige**, Textfeld behält Original.
- [ ] Symbole per Picker an Cursorposition in Lauftext/Blocktext, vor/in/nach Wörtern; keine abgeschnittenen LED-Spalten.
- [ ] Originale 24 Pixelpiktogramme visuell auf echter Tafel prüfen, Sonderfälle mit unbekannter Anzeigezuordnung notieren.
- [ ] Hersteller-Banane `🍌` und separate vertikal gespiegelte Eigensymbol-Banane `🍌︎` sichtbar unterscheidbar.
- [ ] Herz in Rot, Gelb, Grün, Cyan, Blau, Magenta und Weiß; andere Symbole erhalten Originalfarben.
- [ ] Mehrere Symbole mit unterschiedlichen belegten Breiten; Abstände zur Original-App weiterhin als provisorisch kennzeichnen.
- [ ] Mini-Zweizeilen mit Symbolen: 16-Pixel-Kollisionen erkennen/beurteilen; aktuell kein automatischer Kollisionsschutz.
- [ ] Helligkeit, Geschwindigkeit und Effekt/Modus per eigenständigen Befehlen verändern: keine Prozentanzeige, Verbindung nach Inaktivität frei.
- [ ] Hersteller-App danach wieder verbinden und Controller erneut verwenden; Busy-Fehler ohne Neustart.
- [ ] Lauftextentwurf und letzter Reiter über Browser-Reload erhalten, aber nicht irrtümlich als ESP32-Bibliothek ausgegeben.
- [ ] BLE-Diagnose und Fortschrittsanzeige bei einem **echten Inhaltsupload** weiterhin korrekt; Langzeittests.

**Ergebnis bisher:** Browser-JavaScript ohne Laufzeitfehler; Assets korrekt dekomprimiert. **Kein** ESP32-Kompiliernachweis, keine BLE-Hardwareprüfung. Keine Checkbox als erledigt markieren.
