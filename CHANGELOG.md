# Changelog

## [0.3.4] · 25.10.2026 · Erweiterte BLE-Anzeigemodi (Hardwaretest ausstehend)

**Eigenständiges Feature-Release auf Basis der am 25.10.2026 hardwarebestätigten v0.3.3.** Die zunächst intern „v0.3.3 Korrektur 1“ genannte Erweiterung wird als **v0.3.4** ausgeliefert; kein Fehlerfix am bereits funktionsfähigen v0.3.3-Release.

### Neu und geändert
- Das bestehende Dropdown für direkte Moduswechsel über BLE-Kommando `0x06` führt zusätzlich `0x03` Rechtslauf, `0x04` Aufwärts, `0x05` Abwärts, `0x06` Schneeflocke, `0x07` Bild/Picture und `0x08` Laser. **Alle sechs neuen Werte sind am eigenen Board noch ungetestet.** Referenznamen stammen aus `UpDryTwist/coolledx-driver`; Anzeigeeffekte können je nach Board abweichen.
- Standbild (`0x01`) und Linkslauf (`0x02`) bleiben erhalten. Server validiert die erlaubten Modusnamen und protokolliert den Parameter als Hexwert. Moduswechsel selbst sind kurze BLE-Steueraufträge ohne neuen Bild-Upload.
- HTTP-Startseite liefert `Cache-Control: no-store` gegen veraltetes Safari-HTML. Laufzeitdiagnose unterscheidet Helligkeit, Tempo und Moduswechsel auch bei der `Auftrag:`-Ausgabe; der exportierte Diagnose-TXT-Dateiname wird aus `FW_VERSION` gebildet.
- Firmware/HTML sichtbar `v0.3.4`; neuer separater Sketchordner `firmware/RavestickBoard_v0_3_4/`. Aktualisierter `web/index.html`, Testplan, README, Roadmap, technische Dokumente und Entwicklungsstand.

### Unverändert
- Das in v0.3.3 erfolgreich hardwaregetestete datenbasierte Lauftextlimit (vorläufig 30 KiB), zwei Mini-Zeilenfarben, Glyphen/Symbole und gespeicherte NVS-Seiten. **Keine Migration und kein Datenverlust beabsichtigt.**
- Neue Textuploads wählen weiterhin ihre eigenen Textmodi; die zusätzlichen Werte werden nur über das separate Live-Dropdown getestet.
- Keine Bibliothek, keine neuen Symbole, kein Animationsupload und keine verketteten Programme.

### Prüfen auf echter Hardware
- Arduino-IDE-Kompilierung, WLAN-Start, bestehende sieben Blockseiten und Übertragung eines kurzen Texts.
- Modusparameter `0x01` bis `0x08` einzeln: Beobachtung auf der Tafel, BLE-Log, Prozentanzeige und Rückkehr zu bekannten Modi. Keine tatsächliche Effektunterstützung für `0x03`–`0x08` vorab behaupten.
- **Ein BLE-JOB-OK bestätigt nur die Übertragung/Notification, nicht die Bildanzeige.** Das vorige v0.3.3 ist die Rückfallversion.

---

## Historischer Zwischenstand · Arbeitstitel „v0.3.3 Korrektur 1“ (nicht eigenständig veröffentlicht)

- Live-Modus-Dropdown um Referenz-Moduswerte `0x03`–`0x08` erweitert: rechts, aufwärts, abwärts, Schneeflocke, Bild, Laser. Bereits auf unserer Tafel getestete Werte `0x01` (Standbild) und `0x02` (Linkslauf) bleiben erhalten. **Alle neuen Werte noch ungetestet.** Modus wird weiterhin ausschließlich über den eigenständigen BLE-Steuerbefehl `0x06` ohne Bildneuübertragung geändert; der neue Textupload bleibt wie gehabt mit Linkslauf.
- Firmware validiert Modusnamen serverseitig und protokolliert den tatsächlich gesendeten Hexwert. Startlog unterscheidet Tempo, Modus und Helligkeit. HTML-Startseite sendet `Cache-Control: no-store` gegen veraltete Safari-Oberflächen.
- **Hardwaretests der ursprünglichen v0.3.3 erfolgreich** (noch **nicht** der vorliegenden Korrektur 1): WLAN, NVS-Seiten (7), mobile Palette, fünf Fonts, zweizeiliger Mini-Lauftext mit zwei Farben, datenbasierter Zähler; 650 Zeichen / 5.097 Spalten / 30.689 Bytes real dargestellt. Gemischte 301-Zeichen-Folge bei 30.695 B erfolgreich. Grenzwert 301 × ⚠️ (30.803 B) wird im Editor blockiert. Im seriellen Log 240 quittierte BLE-Pakete bei 30.695 B, Dauer 52.937 ms; freier Heap nach WLAN 93.352 B, größter Block 81.908 B. In diesem Log keine `JOB FEHLER`-Zeilen.
- **Firmwarestatus zum Zeitpunkt dieser Notiz:** Moduserweiterung noch nicht mit Arduino kompiliert oder am Board getestet. **Anschließend als v0.3.4 veröffentlicht**, ohne das bestätigte v0.3.3 umzubenennen; NVS-Format unverändert.

## v0.3.3 — Ursprünglicher Testkandidat (25.10.2026)

**Nachträglich an realer Hardware erfolgreich getestet.** Die ursprünglich als ausstehend bezeichneten Tests wurden am 25.10.2026 durchgeführt (siehe Korrektur-1-Eintrag oben).

- Experimentelles **datenbasiertes Lauftextlimit** statt 600-Zeichen-Begrenzung; berechnete Original-Glyphen-/Symbolbreiten, Bytes einschließlich 107-Byte-Header und Prozentbalken live im Webeditor. Vorläufiges 30-KiB-Limit, keine automatische Textkürzung; Überschreitung rot/fett, Senden gesperrt.
- UTF-8-Lauftext wird vom ESP32 gestreamt und ohne globale 600-Zeichen-Arrays gerendert. Datenlimit auch in HTTP-Validierung und Bildrendering geprüft; blockseitenbezogene Datenstruktur und NVS-Schema unverändert.
- Mini Zweizeilig im Lauftext: unabhängige Schriftfarben für obere und untere Zeile, auch in der 64×16-Pixel-Vorschau.
- `web/index.html` und `RavestickBoard_WebUI.h` synchronisiert; aktualisierte README/Dokumentation und neuer reproduzierbarer Testplan.
- **Nicht enthalten:** Bibliothek, Lauftext-Programmeditor, Verkettung mehrerer Lauftexte in einer BLE-Übertragung. Diese Ideen sind für Folgereleases dokumentiert.

**Risikohinweis:** 30 KiB ist kein gesichertes CoolLEDX-Limit. Tests von v0.3.2 zeigten 29.837 B erfolgreich, 31.853 B ohne Anzeige trotz BLE-Erfolg. Umfangreichere Daten brauchen ggf. mehr zusammenhängenden RAM, als zum Sendezeitpunkt verfügbar ist.

## [0.3.2] · 25.10.2026 · Mobile-Bedienung und experimenteller Langtext

**Hardwaretest-Nachtrag · 25.10.2026, ca. 11:45 Uhr MEZ:** **Korrektur 2 wurde vom Nutzer erfolgreich kompiliert, geflasht und auf ESP32-C3 + CoolLEDX getestet.** Der zuvor fehlgeschlagene WLAN-AP startet wieder; NVS-Blockseiten blieben erhalten. Mobile Symbolpalette, Mehrfach-Einfügen ohne Tastatur, alle fünf Lauftextschriften, synchroner Mini-Zweizeilen-Lauftext, leere neue Eingaben und rot/fetter Zähler ab 601 bestätigt. **600 normale Zeichen werden auf der Tafel dargestellt** (27.461 B, 215 BLE-Pakete, 54,3 s). 300 gemischte Symbole funktionieren (29.837 B), 320 gemischte Symbole nicht (31.853 B); 320 Blitze funktionieren (25.061 B), 320 Warnsymbole nicht (32.741 B). Anzeigefehler werden trotz vollständig quittierter BLE-Pakete teilweise als `JOB OK` gemeldet. Bei 514 bis 600 sehr breiten Symbolen scheitert teils bereits die RAM-Allokation. **Keine feste Tafeldatengrenze bestätigt.** Zwei Zeilen in Mini-Lauftext haben noch keine getrennte Farbauswahl. **Ideen, nicht implementiert:** datenbasierte Live-Kapazität statt Zeichenlimit und ein aus Bibliothekseinträgen zusammengefügtes Laufband als einzelner BLE-Upload. Details: [`docs/ENTWICKLUNGSSTAND_v0.3.2_2026-10-25.md`](docs/ENTWICKLUNGSSTAND_v0.3.2_2026-10-25.md).

**Leseregel:** Die nachfolgenden Hinweise „noch nicht getestet“ kennzeichnen den damaligen *Auslieferungsstand vor diesem Hardwaretest*, nicht den heutigen Status von Korrektur 2.

**Korrektur 2 (25.10.2026, WLAN/RAM):** Nutzer hat Korrektur 1 erfolgreich kompiliert und geflasht, der WLAN-AP konnte jedoch wegen fehlgeschlagener WLAN-RAM-Allokation nicht starten (`wifi:alloc pp wdev funcs fail`, Folgefehler `0x3001`). Der Quelltext hatte zwei dauerhaft reservierte Bildpuffer von zusammen **122.507 Bytes** (`61.200 + 61.307`). Jetzt wird nur ein auf die tatsächlich gerenderte Bildbreite dimensionierter Payload-Puffer unmittelbar vor dem Versand angelegt (RGB-Daten liegen direkt ab Byte 107) und auf sämtlichen Erfolgs-/Fehlerpfaden wieder freigegeben. Kleinere Stackpuffer beim Blockseitenrendering und zusätzliche Heap-Diagnose beim Start. Der Webserver wird bei fehlendem AP nicht irreführend als bereit gemeldet. **Zum Zeitpunkt der Auslieferung von Korrektur 2 stand der Arduino-/Hardwaretest noch aus; dieser wurde am 25.10.2026 anschließend erfolgreich vom Nutzer durchgeführt (siehe Nachtrag oben).** Experimentelle 600-Codepoint-Grenze und NVS-Format bleiben unverändert. Bei sehr langen Symbolketten kann die dynamische Allokation dennoch scheitern; dann wird nicht gesendet.

**Korrektur 1 (25.10.2026):** Compilerfehler in `renderText()` behoben: `drawInline()` erhielt versehentlich einen zusätzlichen Skalierungsparameter. Die Skalierung wird bereits innerhalb von `drawInline()` aus `layout` bestimmt. Keine Änderung an NVS, BLE-Protokoll oder Webinterface. **Zum Zeitpunkt von Korrektur 1 stand die Arduino-Neukompilierung noch aus; sie erfolgte anschließend erfolgreich.**

**Historischer Auslieferungsstatus vor Hardwareprüfung:** JavaScript-Syntax und mobile Chromium-Browser-Smoketests geprüft (375px, 351px Dialog); die Arduino-Kompilierung und der LED-Hardwaretest waren zu diesem Zeitpunkt noch ausstehend. Grundlage: v0.3.1, am 25.10.2026 vom Nutzer erfolgreich kompiliert und praktisch getestet.

- Mobile Symbolpalette: **fehlendes schließendes HTML-`div` behoben**. Dadurch gehören Diagnose/Status nicht länger zum Overlay, die Palette hat wieder volle nutzbare Dialogbreite. Mehrere Symbole ohne erneutes Öffnen einfügen; beim Einfügen kein automatisches Fokussieren des Textfelds und kein Tastatur-Popup.
- Lauftext-Schriftauswahl und Blockseiten-Auswahl einheitlich: Standard, Standard Bold, Engschrift, Mini Einzeilig, Mini Zweizeilig. Layout-Codes und gespeicherte NVS-Struktur bleiben unverändert.
- Experimenteller synchron zweizeiliger Mini-Lauftext mit zwei Eingabefeldern und gemeinsamem horizontalen Scrollen.
- Neue Blockseiten und neue Browserentwürfe ohne Demo-Feldwerte; bestehende gespeicherte Seiten bleiben erhalten. Browserseitige lokale Entwürfe werden weiterhergestellt.
- Experimenteller Controller-Puffer für bis zu 600 Unicode-Codepoints, rote/fette Zählerdarstellung ab 601; Texte werden nicht gekürzt. Über 600 wird nicht gesendet, ohne Popup.
- **Protokoll-Risiko:** Die bekannten 80 Format-Slots und das Ein-Byte-Zeichenfeld des BLE-Headers wurden nicht reverse-engineert. Bei mehr als 80 Zeichen wird das Header-Feld auf 80 begrenzt, die vollständige Pixelmatrix aber übertragen. Ob die Tafel längere Inhalte annimmt, ist unbekannt. Lange Symbolketten können große BLE-Paketzahlen und lange Sendezeiten verursachen. Test bei 80, 100, 150, 200, 300, 400, 500, 600 schrittweise.
- Web-Mirror aktualisiert; bestehende Firmwareversionen und gespeicherte Blockseiten bleiben unangetastet.

**Offen:** Mobile Safari-Test, Arduino-Build, BLE-Langtext, iOS-Overlay, zweizeiliger Lauftext und Persistenztest.


## [0.3.1] · 24.10.2026 · begrenzter Korrektur- und Testkandidat

**Nachtrag 25.10.:** Vom Nutzer erfolgreich mit Arduino IDE kompiliert und auf Hardware getestet. 40/60/80 Zeichen und lange Symbolketten übertragen; bestehende Seiten erhalten. UI-Probleme der Symbolpalette, verbleibende Demotexte und fehlende Lauftext-Schriftwahl in v0.3.2 aufgenommen.

**Status:** Quelltext erstellt, JavaScript-Syntax und zentrale Browser-Bedienabläufe mit simulierten API-Antworten geprüft. **Noch keine vollständige Arduino-/ESP32-Kompilierung und kein Hardwaretest**; Version nicht als abgenommen markieren. Ausgangsbasis ist das auf echter Hardware bestätigte v0.3.0-dev.

### Neu / geändert
- `EDT-11`, `QA-02`: Experimentelles 80-Zeichen-Limit für Lauftext statt 40; Pixelpuffer auf bis zu 1.360 Spalten ausgelegt. Die 80 Slots des bisherigen Protokollkopfs werden nicht überschritten. Die Tafelkapazität muss real geprüft werden, insbesondere bei vielen 16×16-Piktogrammen.
- `LIB-11`, `EDT-22`: Demotext „Grüße!“ als voreingestellten Feldwert entfernt; bisheriger Lauftextentwurf bleibt im Browser gespeichert und wird wiederhergestellt.
- `UI-03`, `SYM-02`: Symbolauswahl als Overlay neben jedem Textfeld, mit Einfügen an der gespeicherten Cursorposition.
- `EDT-15`: Rückgängig/Wiederholen für **Textänderungen und Symbol-Einfügen** in Lauftext sowie Zeile 1/2, jeweils lokaler Verlauf von bis zu 100 Zuständen pro Feld. Farbwechsel/Seitenoperationen ausdrücklich noch nicht rückgängig.
- `FNT-19`: Leerzeichen bei Mini ein- und zweizeilig schmaler (2 + 1 statt 5 + 1 Pixel). Vorschau und Controller rechnen einheitlich.
- `PRE-07`, `UI-04`: Vorschau beim Scrollen sticky; redundanter Helligkeits-Button entfernt. Vier vorrangige Fontnamen aufgeräumt; bestehende Engschrift bleibt als Kompatibilitätsoption verfügbar.

### Unverändert / zurückgestellt
- Firmwareordner v0.3.0 und alle früheren Stände unberührt.
- NVS-Struktur der bis zu zwölf Blockseiten **nicht verändert**, bisherige Seiten bleiben erhalten.
- Keine individuelle Zeilenausrichtung, keine neuen Symbole, kein überarbeiteter Bananenentwurf, keine vollständige Bibliothek, kein erweitertes Zeichenformatierungs-UI.

### Prüfung
- JavaScript durch Node.js `--check` geprüft; Chromium-Smoketest im simulierten Webumfeld: Leerstarter, Textzähler, Undo/Redo, Symbol-Einfügung an Cursorposition und Undo, Blockseitentext-Undo. Kein echter BLE-/USB-Test.
- Offene Praxisprüfung: Arduino-Kompilierung, UI auf iPhone/Safari, 40/50/60/80-Zeichen-Übertragungen, viele Symbole, Helligkeit, Persistenz alter Seiten, Speicher und BLE-Diagnose.

---


## [0.3.0-dev] · 24.10.2026 · Entwicklungs- und Browser-Testkandidat

**Noch nicht mit Arduino-ESP32 3.3.12 kompiliert und nicht auf dem ESP32 oder der CoolLEDX-Tafel geprüft. Nicht als hardwarebestätigte Version kennzeichnen.**

- 82 vollständige Originalglyphen **Normal**, 82 **Bold** aus dem Originalglyphenarchiv v02 in Flash-Daten vorbereitet. Pixelvorschub/Abstände experimentell.
- 24 Originalsymbole aus dem Herstellerarchiv v01 (16×16 und sieben Farben), dazu eine unabhängige, vertikal gespiegelte RavestickBoard-Banane. Symbole in Texte einfügbar; für unterschiedliche effektive Breiten wird die tatsächlich beleuchtete Breite benutzt, plus vorläufig 1 Pixel Abstand.
- Herz in sieben Farben, durch Emoji-Farbvarianten auswählbar und unabhängig von der normalen Textfarbe.
- Mini, eine Zeile: 7 px hoch, horizontal und vertikal zentriert. Mini in zwei Zeilen bleibt erhalten; kleine Buchstaben werden in der Anzeige zu Versalien, ohne den gespeicherten Text zu verändern.
- Separate BLE-Kommandos für Helligkeit (`0x08`), Tempo (`0x07`) und Anzeigeart (`0x06`), kurzzeitige Verbindungswiederverwendung. Wirkung/Antwort noch nicht hardwaregetestet.
- Browserseitige Wiederherstellung von Lauftextentwurf und zuletzt verwendetem Editor-Reiter.
- NVS-Datensatzformat der bestehenden 12 Seiten bleibt unverändert; neue Layoutwerte 2–4 werden ergänzt.
- Kein `0x04`-Animationsupload, keine LittleFS-Programmbibliothek und keine vollständige Zeichen-für-Zeichen-Formatierung in diesem Kandidaten.
- Browser-Smoketest ohne JavaScript-Ausnahme; vollständige Arduino-/Hardwareprüfung ausstehend.
- Lizenzanfrage an OpenSign-Autor Adam Schulz wurde am 23.10.2026 **per E-Mail** versendet; Antwort/Erlaubnis weiterhin ausstehend. Repository ist auf GitHub **Public**.

---


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
