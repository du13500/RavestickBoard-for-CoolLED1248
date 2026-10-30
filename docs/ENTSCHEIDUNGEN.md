## Entscheidung v0.4.0-dev · 26.10.2026

Wir wechseln von einer Editor-zentrierten Oberfläche zu sieben Bereichen mit festem Header und Menü über den Bananen-Button. Die Startseite besitzt sechs Kurzwahlen statt der früheren acht. Bibliothek und Programme sind getrennte Datenebenen, Programmzeilen referenzieren Bibliothekseinträge und übernehmen spätere Textänderungen. Globale Helligkeit und Geschwindigkeit bleiben bei Inhaltswechseln unverändert; ein manueller Modus-Override gilt nur bis zum nächsten Inhalt, dessen gespeicherter Modus wieder gilt. Einzelheiten dokumentiert `ENTWICKLUNGSSTAND_v0.4.0_2026-10-26.md`. Unsere **eigenen Codebestandteile** veröffentlichen wir ab dieser Version unter MIT; Herstellerpixel und andere Fremdmaterialien sind davon ausgenommen.

---

## Ergänzung v0.3.5

### 2026-10-25: Umfang v0.3.5

Auf der stabilen v0.3.4 aufbauen; neue Komfortfunktionen und Symbole als v0.3.5 statt v0.3.4-Korrektur. Die erweiterten Modi bleiben manuell; keine Laufzeitkalibrierung für Sonderanimationen. Horizontale Textlaufzeit später für Playlist/Programmwechsel. LED Ein/Aus kurz und ohne Pixelupload, Wirkung auf realem Board noch zu testen. NVS-Schema bleibt unverändert. Symbole der zwei Mini-Zeilen dürfen sich absichtlich überlagern.

---

# Entscheidungen und Designprinzipien

**Stand 23.10.2026.** Entscheidungslog (ADR = Architecture Decision Record). Bestehende IDs bleiben stabil; historische Entscheidungen und spätere Präzisierungen werden getrennt dokumentiert. Eine Entscheidung ist nicht automatisch bereits implementiert.

| ID | Datum | Entscheidung | Grund / Auswirkung |
| --- | --- | --- | --- |
| ADR-001 | 2026-10 | **Original-LED-Tafel bleibt unangetastet**; ESP32-C3 steuert über BLE | Kein Löten/Umbau an der Anzeige nötig. |
| ADR-002 | 2026-10-22 | **Eigenes ESP32-WLAN**, kein notwendiges Heimnetz/Internet | Festivalbetrieb mit dem Smartphone möglich. |
| ADR-003 | 2026-10-22 | **Firmware nur per USB-C**, keine OTA-Funktion | Einfache Wartung, geringere Angriffsfläche und mehr Flash-Spielraum. |
| ADR-004 | 2026-10-22 | Passwortgeschütztes WLAN, **kein zweites Webinterface-Login** | Möglichst unkomplizierte Bedienung; direkte Fremdsteuerung der Originaltafel über BLE wird dadurch ohnehin nicht verhindert. |
| ADR-005 | 2026-10-22 | **Textlänge bestimmt nicht den Wiedergabemodus** | Lauftext oder statische Seiten werden immer explizit ausgewählt. Die v0.1.0-Längenautomatik ist nur ein Prototyp. |
| ADR-006 | 2026-10-22 | **Statische Anzeigen als individuell gestaltbare Seiten**, nicht über Leerzeichen simuliert | Pro Seite eigene Inhalte, Farben, Schriftwahl, Dauer; manuelle oder automatische Navigation. |
| ADR-007 | 2026-10-22 | **Zeichenweise bzw. bereichsweise Farben und Schriftarten** | Texteditor soll nicht an ein einheitliches Format gebunden sein. |
| ADR-008 | 2026-10-22 | **Vorschau zeigt echte LED-Punkte in festem 64×16-Raster** | Kein Schrumpfen langer Texte; Animation nur im festen Anzeigefenster. |
| ADR-009 | 2026-10-22 | **Vorschau mit Fast Forward** | Lange Laufschriften schnell prüfen; **1×/2×/4×**, **kein 8×** (am 23.10. endgültig entschieden). |
| ADR-010 | 2026-10-22 | **Normal und Bold als Originalschrift separat rekonstruieren** | Pixelgenaue Aufnahme der beiden Original-Modi, nicht versehentlich vermischen. |
| ADR-011 | 2026-10-22 | **Hersteller-Unregelmäßigkeiten einzeln diskutieren** | Originalreferenz bewahren; nicht pauschal übernehmen oder automatisch „verbessern“. |
| ADR-012 | 2026-10-22 | **Mini-Font selbst entwickeln**, Engschrift behalten | Mini ist kein Originalfont; Engschrift bereits erfolgreich ausprobiert. |
| ADR-013 | 2026-10-22 | **Zweizeilig ohne künstlich erzwungenen Leerabstand** | 16 verfügbare Pixel sinnvoll nutzen, Lesbarkeit praktisch prüfen. |
| ADR-014 | 2026-10-22 | **Versionierte Dokumentation + Diagnose von Anfang an** | Aus SmallTV-Erfahrungen lernen; sporadische Probleme nachvollziehbar machen. |
| ADR-015 | 2026-10 | **Webinterface gestalterisch von SmallTV unterscheiden** | An vertraute CoolLED1248-App angelehnt, aber moderner und besser bedienbar; nicht SmallTV-grau/gelb. |
| ADR-016 | 2026-10-22 | **Keine automatische Überschreibung beim Booten** | Bereits gespeicherte Anzeige unangetastet lassen, bis Nutzer aktiv sendet oder ein bewusst aktiviertes Programm läuft. |
| ADR-017 | 2026-10-22 | **Lizenz-/Provenienzprüfung vor der Entscheidung über öffentliche Sichtbarkeit** | Initiale technische Prüfung 23.10.2026 dokumentiert; keine pauschale Freigabe fremden Codes. |
| ADR-018 | 2026-10-22 | **Projektname „RavestickBoard for CoolLED1248“**, GitHub-Repository | Unterscheidung vom Original-App-Namen; Rechte anderer Projekte bleiben zu beachten; Public-Sichtbarkeit ist kein Firmwareziel. |
| ADR-019 | 2026-10-22 | **Diagnose-Ringpuffer nur im RAM, TXT-Export auf Anfrage** | Weniger Flash-Verschleiß, keine zusätzliche Anmeldung, keine gespeicherten Textinhalte oder Kennwörter im Log. |
| ADR-020 | 2026-10-22 | **Firmwareversion im Header sichtbar und im Diagnosebericht** | Direkt erkennbare Softwareversion bei Screenshots und Support; Versionsstand passt zum Git-Commit. |

## Entscheidung 25.10.2026 – v0.3.4 als eigenständiges Feature-Release

v0.3.3 wurde bereits auf echter Hardware erfolgreich geprüft (u. a. 650 Zeichen/30.689 B, 2-Farben-Mini-Lauftext). Die zusätzlichen Werte `0x03`–`0x08` für **einen** BLE-Anzeigemodusbefehl `0x06` werden deshalb **nicht** als „v0.3.3 Korrektur 1“ bezeichnet, sondern als **v0.3.4** mit eigenem Sketchordner veröffentlicht. `0x01`/`0x02` bestätigt, weitere Werte noch ungetestet. Die ursprüngliche v0.3.3 bleibt unverändert. NVS-Datenformat und Bildrenderer ebenfalls unverändert. Getestete Grundlagen nicht mit neuen unbestätigten Modi vermischen.

## Entscheidung 25.10.2026 – v0.3.3: Datenbudget statt Zeichenzählerlimit

Die 600-Zeichen-Grenze wird **nicht** als Produktspezifikation behandelt. Der Editor berechnet die Bytegröße aus Originalglyphen, tatsächlichen Symbolbreiten und `107 + 6 × max(64, gerenderte Spalten)`. Der Zeichenzähler bleibt rein informativ. Bei Überschreitung erscheint die Auslastung **rot und fett**, ohne Textverlust; Senden wird blockiert. **30 KiB sind vorläufig**, die Hardwaregrenze bleibt unbekannt (29.837 B funktionierten, 31.853 B wurden gesendet, aber nicht dargestellt). Der Firmwarecode dekodiert lange Lauftexte iterativ, um beim Start keinen großen statischen Bild-/Zeichenpuffer zu reservieren.

Als kleine Zusatzfunktion erlaubt Mini Zweizeilig als **Lauftext** getrennte Farben pro Zeile; die zwei Zeilen scrollen synchron. Andere Layouts bleiben gleich. Bibliothek und Verkettung mehrerer Nachrichten sind **noch nicht implementiert**; sie gehören in einen späteren Release nach Hardwarevalidierung.

## Noch nicht technisch geklärt

- Ob **native Blockseiten** oder **gespeicherte Animationen `0x04`** auf unserer konkreten Tafel mit **einem** Upload autonom laufen; weitere Details in ADR-021/022.
- Ob die Original-Tafel pro Bild individuelle Haltezeiten beherrscht; wenn nicht, ob wiederholte Frames eine ausreichend gute Lösung darstellen.
- Wie Original-App-Geschwindigkeitsanzeige (~80) in BLE-Rohwerte übersetzt wird.
- Welche Speicherkapazitäten/Kompressionsstrategien für Bibliothek und Animationen realistisch sind.
- Ob eine optionale native App/Watch/Widget-Oberfläche später einen Mehrwert bietet.
- GitHub ist inzwischen **Public**, unabhängig bestätigt am 24.10.2026; die Lizenz bleibt All Rights Reserved.

## 22.10.2026 · v0.1.2: deutsche Sprache, Pixelvorschau und Modi

- Die Benutzeroberfläche schreibt Deutsch mit **Ä, Ö, Ü, ä, ö, ü und ß**. Keine pauschale Umschrift in `ae/oe/ue/ss`.
- Der Engschrift-Font bekommt echte Kleinbuchstaben; der Browser erhält seine Glyphen aus der ESP32-Tabelle über `/api/font`. Keine zweite manuell gepflegte Fonttabelle in JavaScript.
- Statischer Text und Lauftext werden **manuell gewählt**, unabhängig von Länge. Vorerst kein automatischer Seitenwechsel und kein generischer Auto-Umbruch. Zu breite Standbilder werden ausdrücklich gekennzeichnet.
- Die LED-Vorschau benutzt einzelne **runde** Punkte in fester 64×16-Matrix und bietet schnellen Vorlauf **1×/2×/4×**; **8× wurde ausdrücklich gestrichen**.
- Der Geschwindigkeitsregler für die LED-Tafel und die Vorschau-Fast-Forward-Funktion sind unabhängig voneinander. Geschwindigkeit bleibt bis zur Kalibrierung experimentell.
- Kopieren in die Zwischenablage wird zum bevorzugten Diagnose-Export, TXT bleibt als Alternative.

## Entscheidung 22.10.2026: v0.2.0-Blockeditor
- Seiten sind eigenständige Design-Objekte, nicht automatisch abgeschnittene Textstücke.
- Keine erzwungenen Zeilenumbrüche und kein automatisches Nachrücken von Seiten.
- Jede Seite hat eigenes Layout, Texte, Zeilenfarben, Ausrichtung und Anzeigedauer.
- 12 Seiten zunächst als pragmatisches Limit. Die gespeicherten Seiten bleiben nach Neustart erhalten; der konkrete v0.2.0-Test bestätigt das. NVS schreibt nicht pro laufendem Vorschauframe.
- Seitenplayer sendet vollständig über BLE; das ist ausdrücklich ein Experiment bis zur Klärung der Board-internen Mehrseitenfähigkeit.
- Firmware bleibt USB-only ohne OTA. Webinterface benötigt kein gesondertes Login.
- Typografische Detailkorrekturen g/p/q/y/Ä/Ö/Ü bleiben separate Entscheidungen anhand der Referenzen.


## Architektur-, Forschungs- und Dokumentationsentscheidungen (23.10.2026)

| ID | Datum | Entscheidung / Designvorgabe | Begründung / Einschränkung |
| --- | --- | --- | --- |
| ADR-021 | 2026-10-23 | **Seite → Programm → Playlist** als drei getrennte Ebenen | Keine ständigen Uploads zwischen Seiten eines Programms; bewusste neue Uploads zwischen Programmen erlaubt. |
| ADR-022 | 2026-10-23 | **`0x04`-Animation als Untersuchungspriorität** | OpenSign meldet autonome gespeicherte Animation, aber nur auf Referenzpanel; bei unserer Tafel erst isoliert nachweisen. |
| ADR-023 | 2026-10-23 | **Mini zweizeilig unverändert als Ausgangsbasis; zusätzlich Mini einzeilig; Mini-Ausgabe in Versalien** | Echte Nutzerfotos überzeugen für Großbuchstaben; Kleinbuchstaben in Mini unleserlich; ß-Pixelkorrektur als Detail. |
| ADR-024 | 2026-10-23 | **Entwurf, Bibliothek, aktives Programm trennen** | Lauftexteditor verliert Entwurf nach Reload, gespeicherte Blockseiten bleiben jedoch in NVS. Browser-Entwurf später wiederherstellen. |
| ADR-025 | 2026-10-23 | **Bibliothek zuerst im ESP32-Flash (LittleFS), microSD optional** | Wenig Zusatzhardware; einheitlich von verschiedenen Browsern erreichbar, Ex-/Import als Backup. |
| ADR-026 | 2026-10-23 | **OpenSign keine Codequelle ohne Erlaubnis** | Repo enthält ausdrücklich keine Lizenz; Eigenimplementierung aus Protokollfakten und eigenen Tests bzw. explizite Freigabe erforderlich. |
| ADR-027 | 2026-10-23 | **Live-Steuerbefehle ohne Neu-Upload** | `0x08` Helligkeit, `0x07` Geschwindigkeit, `0x06` Modus getrennt nutzen. |
| ADR-028 | 2026-10-23 | **Excel-kopierbare Testtabelle zu jedem Firmware-Updatepaket ausschließlich im Chat** | Unabhängiges manuelles Testprotokoll mit Ergebnis und Bemerkungen; keine zusätzliche Test-Datei. |
| ADR-029 | 2026-10-23 | **Firmware v0.2.0 vorerst nicht verändern** | Erst aktuelles Hardwareverhalten systematisch testen, dann Änderungen gebündelt umsetzen. |


## Weitere Entscheidungen vom 23.10.2026

| ID | Datum | Entscheidung | Grund / Auswirkung |
| --- | --- | --- | --- |
| ADR-030 | 2026-10-23 | **Eigenen Code öffentlich sichtbar zeigen ist keine Open-Source-Lizenz** | `LICENSE` bleibt „All Rights Reserved“, Rechte Dritter und GitHub-Plattformrechte bleiben bestehen. Keine neue Firmwarefunktion. |
| ADR-031 | 2026-10-23 | **Festes Testpasswort und BLE-MAC bewusst dokumentiert** | AP-Passwort `Banane1248!` darf im öffentlichen Quelltext bleiben; individuelles WLAN-Passwort per UI später optional. Die MAC gilt nur für die Testtafel. |
| ADR-032 | 2026-10-23 | **Titelbild im README zentriert und ohne Größenattribut** | `docs/assets/totem.jpg` soll in natürlicher Größe erscheinen; kein `width`/CSS-Zwang. |
| ADR-033 | 2026-10-23 | **README: Badges in Englisch, Beschreibung in Deutsch** | `status: hardware testing`, `mission: hugs and love`, außerdem Version/Hardware/Projekt/Lizenz. Kein gesonderter Abschnitt „Projektstruktur“ mehr. |
| ADR-034 | 2026-10-23 | **README als Nutzer-Vorlage bewahren** | Nicht bei Dokumentationspflege die Nutzeranpassungen durch ältere generierte README-Fassungen ersetzen. Bei nächster inhaltlicher Änderung eigene Minischrift klar benennen. |
| ADR-035 | 2026-10-23 | **v0.2.0 als erfolgreich hardwaregetesteter Prototyp** | Nutzer hat 13 von 14 Testpunkten mit Ja bewertet, ein Punkt unbewertet; konkrete Mängel (Kleinbuchstaben-Mini, ß, Prozentanzeige) bleiben offen. |
| ADR-036 | 2026-10-23 | **Schrift-Feinschliff gezielt und nicht pauschal** | Großbuchstaben-Mini zweizeilig beibehalten; Mini-Text in Versalien; zusätzliche einzeilige Mini; g/p/q/y mit gemeinsamer x-Höhe und Unterlängen; ß/ẞ nur minimal unterschiedlich, markierter Pixel aus. |
| ADR-037 | 2026-10-23 | **OpenSign-Kontakt per E-Mail statt öffentlichem Issue** | Die englische Lizenzanfrage an Adam wurde **am 23.10.2026 per E-Mail versendet**. **Antwort und Erlaubnis ausstehend**; kein OpenSign-Quellcode übernommen. |
| ADR-038 | 2026-10-23 | **Dokumente regelmäßig gegen aktuellen Firmwarestand abgleichen** | Jede neue Firmware erhält im Chat eine direkt nach Excel kopierbare Testtabelle; `docs/TESTPLAN.md` dient für historische Befunde und spezifische Regressionstests. |

## Designentscheidungen v0.3.0 (24.10.2026)

| ID | Datum | Entscheidung | Grund / Auswirkung |
| --- | --- | --- | --- |
| ADR-039 | 2026-10-24 | **Mini, eine Zeile: horizontal und vertikal zentriert** | Unabhängig vom bisherigen links/mitte/rechts-Wert; 7 Pixel Schrift hoch. |
| ADR-040 | 2026-10-24 | **16×16-Symbole inline als Emoji in Textfeldern** | Kein separater „Symbol statt Text“-Modus; bei zwei Zeilen sind überlappende Bereiche zu prüfen. |
| ADR-041 | 2026-10-24 | **Herz-Silhouette in allen sieben Farben** | Gewählte Emoji-Variante bestimmt die LED-Herzfarbe, unabhängig von der Textfarbe. |
| ADR-042 | 2026-10-24 | **Vorläufig effektive Symbolbreite plus 1 px Abstand** | Hersteller-App-Abstände später anhand Original-App messen und bei Bedarf korrigieren. |
| ADR-043 | 2026-10-24 | **Getrennte Hersteller- und RavestickBoard-Symbole** | 24 Originalsymbole mit kurzer Quellenangabe; selbst gestaltete, vertikal gespiegelte Ravestick-Banane separat. |
| ADR-044 | 2026-10-24 | **Neue Fontrenderpfade erhalten altes NVS-Seitenformat** | Bestehende bis zu 12 Seiten bleiben lesbar; Layout-ID 2/3/4 ergänzen. |
| ADR-045 | 2026-10-24 | **v0.3.0 ist bis zum Boardtest nur Entwicklungskandidat** | Browser-Simulation bestätigt nicht Arduino-Kompilierung, BLE-Funktion oder Pixelabstände auf der Tafel. |


## Ergänzungen vom 25.10.2026 (v0.3.2 Testkandidat)

- **ADR-2026-10-25-A · Testweise Langtexte:** Die alte 80-Zeichen-Softwaregrenze wird zur Messung vorläufig auf 600 Zeichen erhöht. Dies bestätigt **keine** korrekte Hardware- oder Protokollunterstützung. Der bisher rekonstruierte Header bleibt mit 80 Format-Slots unverändert und begrenzt den darin übermittelten Zeichenwert auf 80. Einzelne BLE-Erfolge und sichtbare Korrektheit getrennt bewerten.
- **ADR-2026-10-25-B · Dezentes Überlängenfeedback:** Bei mehr als 600 Codepoints wird lediglich der Live-Zähler rot und fett. Bereits getippter Text wird nicht gekürzt; ungültige Überlängen werden nicht übertragen. Kein neues Popup. Für zweizeiligen Lauftext zählen beide Zeilen zusammen.
- **ADR-2026-10-25-C · Mobil-Symbolauswahl:** Die Symbolpalette bleibt während mehrerer Einfügungen offen und fokussiert das iPhone-Textfeld beim Einfügen nicht automatisch. Diagnosestatus und Fußbereich gehören ausdrücklich **nicht** zum Overlay-DOM.
- **ADR-2026-10-25-D · Einheitliche Fontnamen:** Sichtbare Namen und Reihenfolge: Standard, Standard Bold, Engschrift, Mini Einzeilig, Mini Zweizeilig. Interne numerische Layout-IDs und gespeicherte NVS-Daten bleiben erhalten.
- **ADR-2026-10-25-E · Leere neue Eingaben:** Neue Texte und neue Blockseiten starten ohne Demo-Inhalte. Bereits gespeicherte Seiten/Browserentwürfe werden nicht stillschweigend gelöscht.
- **ADR-2026-10-25-F · Repository-Bereinigung:** Keine Datei automatisiert entfernen; die im Cleanup-Bericht vorgeschlagenen Löschungen/Verschiebungen erst nach Hardwaretest in einem separaten GitHub-Desktop-Commit.


## Architekturentscheidungen und Hypothesen nach dem v0.3.2-Hardwaretest (25.10.2026)

| ID | Status | Festlegung bzw. Untersuchung |
| --- | --- | --- |
| ADR-2026-10-25-G | **Entwicklungsrichtung beschlossen, noch nicht implementiert** | Starres 600-Zeichenlimit später durch **live berechnetes Pixel-/Datenbudget** ersetzen; bei Überlauf fett/rot markieren, nicht still kürzen, kein Popup. Der Controller validiert dieselben Renderingdaten. |
| ADR-2026-10-25-H | **Hardwaregrenze unbestätigt** | Erfolgreiche 29.837 B und fehlgeschlagene 31.853 B begrenzen den bisher beobachteten Bereich, beweisen aber weder 30 KiB noch 5.120 Spalten. Gezielter Test mit 300/301/302 Warnsymbolen geplant. |
| ADR-2026-10-25-I | **Diagnoseanforderung** | BLE-Paketquittung/`JOB OK` darf ohne weiteres Protokollwissen nicht als sichtbare LED-Bildübernahme bezeichnet werden. |
| ADR-2026-10-25-J | **Konzept, noch nicht implementiert** | Mehrere eigenständige **Lauftexte** aus einer späteren Bibliothek mit frei definierten Abständen zu einem einzigen Pixelstreifen und einem BLE-Upload verketten. Dadurch nur einmal Fortschrittsanzeige für den gesamten Inhalt. |
| ADR-2026-10-25-K | **Separate Forschungsaufgabe** | Echte Blockseitenprogramme mit Standzeiten/Moduswechseln nicht mit einem einzelnen gleichmäßig scrollenden Bildstreifen verwechseln; native Board-Fähigkeit noch ungeklärt. |
| ADR-2026-10-25-L | **Offener UI-Fix** | Mini Zweizeilig als Lauftext erhält später zwei unabhängig wählbare Textfarben; synchroner Scroll bleibt. |

Aktueller Teststand und Primärbeobachtungen: [`ENTWICKLUNGSSTAND_v0.3.2_2026-10-25.md`](ENTWICKLUNGSSTAND_v0.3.2_2026-10-25.md).
