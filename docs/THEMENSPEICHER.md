# Themenspeicher und Aufgabenregister

**Stand:** 23.10.2026, v0.1.1 hardwarebestätigt, v0.1.2 nach Korrektur gestartet; v0.2.0 (Korrektur 1) mit 13/14 positiven Testpunkten. Neue Recherchen zu OpenSign ergänzt. Aus Projektchats und realen Tests konsolidiert.  
**Status:** `[x]` = im bezeichneten Umfang durch Nutzer-/Hardwaretest bestätigt oder Dokumentationsaufgabe erledigt; `[ ]` = offen/teilweise/noch nicht abschließend verifiziert. Eine Implementierung allein ist kein Hardwaretest.  
**Prioritäten:** P0 = Basis/Fehlerdiagnose, P1 = Kerneditor, P2 = Bibliothek/Spezialprogramme, P3 = spätere Option. Die Umsetzungsreihenfolge ergibt sich aus der aktualisierten `ROADMAP.md`.

**Pflegeprinzip:** Keine Idee verschwindet. Neue Themen bekommen eine eindeutige ID. Eine Aufgabe gilt erst als erledigt, wenn Ergebnis und reales Testdatum im `CHANGELOG.md` stehen.

## A · Verbindung, Hardware, Architektur

- [x] **SYS-01 · P0** ESP32-C3 Super Mini über USB-C in Arduino IDE programmieren (22.10.2026).
- [x] **SYS-02 · P0** CoolLEDX mit BLE-Scan finden; `FFF0`/`FFF1` auslesen (22.10.2026).
- [x] **SYS-03 · P0** Helligkeit über BLE einstellen, `0xFF` sichtbar erfolgreich (22.10.2026).
- [x] **SYS-04 · P0** Eigenen mehrfarbigen Text über BLE übertragen; `FUCK YES` real sichtbar (22.10.2026).
- [x] **SYS-05 · P0** Eigenes WLAN und iPhone-Webinterface; Text `TEST 123` real sichtbar (22.10.2026).
- [x] **SYS-06 · P0** 64×16-Pixel-Daten und 7-Farben-Modus aus BLE-Advertising erkannt (22.10.2026).
- [x] **SYS-07 · P0** Ohne laufende BLE-Verbindung gespeicherte Anzeige abspielen; nach Stromverlust erhalten (Nutzerbeobachtung).
- [ ] **SYS-08 · P1** BLE-Adresse nicht hart codieren: Gerät(e) erkennen und zuordnen.
- [ ] **SYS-09 · P1** Nach fremder Verbindung durch Original-App automatisch wieder handlungsfähig werden; Wiederaufnahme in v0.1.1 beobachtet, verbindliche Tests für v0.2.0, Timeout und Statusmeldungen offen.
- [ ] **SYS-10 · P1** BLE-/WLAN-Koexistenz, Transfergrößen, Langzeitstabilität messen.
- [ ] **SYS-11 · P2** Konfiguration und nutzbare Speicherkapazität messen (Programmpartition, Dateisystem, freier RAM).
- [ ] **SYS-12 · P3** Mehr als eine LED-Tafel verwalten (Idee aus der Diskussion über weitere Boards; Nutzen/Machbarkeit prüfen).

## B · Diagnose, Troubleshooting, Wartung

- [x] **DIA-01 · P0** Diagnosebereich direkt im Webinterface. **v0.1.1 auf echter Hardware bestätigt; Regression bei neuen Versionen bleibt sinnvoll.**
- [x] **DIA-02 · P0** Diagnose-TXT-Download mit Hardware, Firmwarestand, Resetgrund, Uptime und Zeit seit Start. **v0.1.1 auf echter Hardware bestätigt; Regression bei neuen Versionen bleibt sinnvoll.**
- [x] **DIA-03 · P0** Heap frei, Minimum Heap, größter freier zusammenhängender Block. **v0.1.1 auf echter Hardware bestätigt; Regression bei neuen Versionen bleibt sinnvoll.**
- [x] **DIA-04 · P0** BLE-Verbindungs-, Disconnect-, Timeout- und Transfer-Ereignisse; BLE-Notification separat von Write-Erfolg. **Grunddiagnose in v0.1.1 auf echter Hardware bestätigt.** Detaillierte Retry-Strategie separat offen (`SYS-09`, `BLE-15`).
- [ ] **DIA-05 · P0** HTTP-Aufträge, Queue-Status und Fehlerursachen für Webinterface-Fehlersuche. Zähler/Status seit v0.1.1 vorhanden; **vollständige Fehlerpfad-Tests in v0.2.0 offen**.
- [x] **DIA-06 · P0** Begrenzter RAM-Ringpuffer für letzte Ereignisse und separate Fehlerzähler; keine dauernden Flash-Schreibzugriffe. **v0.1.1 auf echter Hardware bestätigt; Regression bei neuen Versionen bleibt sinnvoll.**
- [ ] **DIA-07 · P1** Dateisystem gesamt/belegt/frei; Speichergrenzen prüfen.
- [ ] **DIA-08 · P1** Letzten gravierenden Fehler/Resetmarker optional stromausfallsicher, nach Flashverschleiß-Abwägung.
- [x] **DIA-09 · P1** Protokollierung von Erfolgen *und* Fehlschlägen, Transferdauer, Datenmenge, Paketanzahl. **v0.1.1 auf echter Hardware bestätigt; Regression bei neuen Versionen bleibt sinnvoll.**

## C · Vorschau als LED-Dot-Matrix

- [ ] **PRE-01 · P0** Fester **64×16-Ausschnitt** statt Textbreite mitzuskalieren. **v0.1.2 implementiert; Pixelgenauigkeit gegenüber dem realen Gerät noch nicht systematisch abgenommen.**
- [ ] **PRE-02 · P0** Einzeln sichtbare **runde LED-Punkte** inklusive dunkler/unbeleuchteter Punkte und Punktabstand (nicht nur glatte Pixelblöcke). **v0.1.2 implementiert; gezielter iPhone-Vorschauregressionstest weiterhin offen.**
- [ ] **PRE-03 · P0** Lauftext im festen Fenster animieren; keine automatische Verkleinerung. **v0.1.2 implementiert; reale Geschwindigkeit unkalibriert.**
- [ ] **PRE-04 · P0** Playersteuerung: Play/Pause, Neustart und **Fast Forward** für lange Texte (1×, 2×, 4×; 8× explizit gestrichen). **v0.2.0 implementiert, 1×/2×/4×; separate systematische Vorschauprüfung noch offen.**
- [ ] **PRE-05 · P1** Optional Vorschau-Zeitleiste/Scrubbing oder Sprung zu Folgeseite, damit lange Programme schnell prüfbar sind.
- [ ] **PRE-06 · P1** Pixelvorschau für mehrere Seiten, zeilenweise Darstellung und gemischte Schriftarten/Farben.
- [ ] **PRE-07 · P1** Vorschaubewegung möglichst realitätsgetreu: Laufrichtung und Geschwindigkeitsabgleich mit echter Tafel.
- [ ] **PRE-08 · P1** Statische Seiten mit Positionierung/Überlaufmarkierung; kein ungefragter Auto-Umbruch/Schrift-Shrink.

## D · Texteditor, Modi und Seiten

- [x] **EDT-01 · P0** **Explizit** auswählen: Lauftext vs. statische Seiten; unabhängig von Textlänge. **v0.2.0 auf echter Hardware bestätigt: Lauftext- und Blockeditor getrennt.**
- [x] **EDT-02 · P1** Seiten separat erstellen, gestalten, sortieren, duplizieren, löschen.
- [ ] **EDT-03 · P1** Seiten individuell konfigurieren (Text, Font/Größe, Farbe, Position, Dauer).
- [ ] **EDT-04 · P1** Manueller Seitenwechsel und automatischer Seitenwechsel mit individuell einstellbarer Dauer.
- [ ] **EDT-05 · P1** Endlos/Einmal/Wiederholung und Pausen für Seitenfolge/Lauftext.
- [ ] **EDT-06 · P1** Auswahl eines Zeichens, Worts oder Abschnitts, Farbwahl **je Zeichen** möglich.
- [ ] **EDT-07 · P1** Auswahl eines Zeichens, Worts oder Abschnitts, **Schriftart je Zeichen** möglich.
- [ ] **EDT-08 · P1** Einstellbare Positionierung (links, mittig, rechts; bei statischer Seite präzise Koordinaten später prüfen).
- [ ] **EDT-09 · P1** Für Seiten keine automatische Textkürzung, Umbruch oder Größenänderung; Überlauf klar anzeigen.
- [ ] **EDT-10 · P1** Geschwindigkeit per Schieberegler beim Lauftext, zusätzlich tatsächliche BLE-Speed-Werte kalibrieren. **Regler/Befehl v0.1.2 vorhanden, Kalibrierung offen.**
- [ ] **EDT-11 · P1** Auf 40 Zeichen begrenzten Prototyp erweitern; realistische technische Grenzen messen.
- [ ] **EDT-12 · P2** Freie Textblöcke oder Seiten in Programmen mischen, ohne die Inhalte zu duplizieren.
- [ ] **EDT-13 · P2** Sonderzeichen, Herz/Emojis und Kombination mit eigenen Grafiken.
- [ ] **EDT-14 · P3** Mehr Gestaltungsmöglichkeiten (z. B. Farbwechsel/Verläufe) nur nach Prüfung der tatsächlichen 7-Farben-Hardware.

## E · Schriftbibliothek

- [x] **FNT-01 · P0** Engschrift als 5×7-Matrix (vertikal verdoppelt) mit großen Buchstaben etabliert und real getestet; Kleinbuchstaben seit v0.1.2 vorhanden, gestalterisch noch fehlerhaft.
- [ ] **FNT-02 · P1** Engschrift: **echte Kleinbuchstaben**, Umlaute und Zeichensatz vervollständigen. **v0.1.2 implementiert und einzelne Tests am echten Gerät; Qualitätsmängel bei g/p/q/y und Minischrift-Kleinbuchstaben, Sonderzeichen weiter testen.**
- [ ] **FNT-03 · P1** **Normal** aus Referenzaufnahmen jedes Originalzeichens rekonstruieren (separater Originalanzeigen-Chat).
- [ ] **FNT-04 · P1** **Bold** aus eigenen Originalreferenzen rekonstruieren (separat von Normal).
- [ ] **FNT-05 · P1** Hersteller-Unregelmäßigkeiten sammeln und einzeln über Übernahme oder Korrektur entscheiden; Originalreferenz behalten.
- [ ] **FNT-06 · P1** **Mini** als eigener kleiner Font: zweizeilige Großbuchstabenvariante hardwarebestätigt; **zentrierte einzeilige Mini** und automatische Versalien noch offen. Keine Originalvorlage behaupten.
- [x] **FNT-07 · P1** Zweizeilige Großbuchstaben in 16 Pixeln real getestet (23.10.2026); aktuelle Umsetzung y=0–6 und y=8–14 mit einer unbeleuchteten Trennreihe, ohne daraus ein Pflichtlayout für andere Schriften abzuleiten.
- [ ] **FNT-08 · P1** Gemeinsame Glyphentabelle in C++ für Controller und Browser (`GET /api/font`) seit v0.1.2 **implementiert**; bei neuen Zeichen auf identische Darstellung prüfen und weitere Schriften konsistent einbinden.
- [ ] **FNT-09 · P2** Anzeigebeispiele/Regressionstests für alle Zeichen und Schriften.

## F · Bibliothek, Favoriten, Automatisierung

- [ ] **LIB-01 · P2** Gespeicherte Texte/Seiten/Animationen als Bibliothek; umbenennen, bearbeiten, löschen.
- [ ] **LIB-02 · P2** Thematische Ordner/Tags/Kategorien, Suche und Favoriten.
- [ ] **LIB-03 · P2** **Acht Schnellzugriffe** auf Startseite frei belegen.
- [ ] **LIB-04 · P2** Playlist/Programm mit mehreren Seiten/Einträgen, Reihenfolge, Dauer, Wiederholung.
- [ ] **LIB-05 · P2** Programmrotation automatisch, optional zufällig.
- [ ] **LIB-06 · P2** Sofort-Nachricht anzeigen und danach ggf. zum Programm zurückkehren.
- [ ] **LIB-07 · P2** Programme/Profile für unterschiedliche Festival-Situationen.
- [ ] **LIB-08 · P2** Export/Import von Vorlagen und Programmen zum Sichern/Teilen.
- [ ] **LIB-09 · P2** Geräteinhalt nicht beim Controller-Boot ungefragt überschreiben; nur bewusst aktivierte Automatik.
- [ ] **LIB-10 · P3** Drag-and-drop-Sortierung, Zeitplan/Rotation nach Tageszeit prüfen (Zeitquelle ohne Internet klären).

## G · Spezialprogramme

- [ ] **SPC-01 · P2** Countdown mit Start/Pause/Reset, gewünschtem Ziel und eigener Tafelanzeige.
- [ ] **SPC-02 · P2** Stoppuhr mit Start/Pause/Reset.
- [ ] **SPC-03 · P2** Manuelles **Scoreboard** mit zwei oder mehr Seiten/Punktständen, z. B. ❤️/💋; nicht zwingend dreistellig.
- [ ] **SPC-04 · P2** Weiß-/Rot-Lichtmodus mit Schnellbedienung.
- [ ] **SPC-05 · P3** Grafischer Pixel-Editor und Import von Bildern/Animationen.
- [ ] **SPC-06 · P3** Musikmodus/visuelle Effekte der Original-App untersuchen und ggf. verbessern.
- [ ] **SPC-07 · P3** Technische Strategie für Countdown/Stoppuhr: Firmware der Tafel vs. periodischer BLE-Neu-Upload.
- [ ] **SPC-08 · P3** Animationen, Pixelgrafiken und andere Dateiformate auf realen Flashbedarf und BLE-Übertragungszeit prüfen.

## H · App-Oberfläche und mobile Bedienung

- [x] **UI-01 · P0** Erste dunkle, eigenständige Oberfläche mit LED-Farben auf iPhone erreichbar.
- [x] **UI-02 · P0** 7 Farbtasten ohne unerwünschte zweite Reihe darstellen. **v0.1.1 auf echter Hardware bestätigt; Regression bei neuen Versionen bleibt sinnvoll.**
- [x] **UI-03 · P0** `100 %` neben Helligkeitsregler ohne Zeilenumbruch. **v0.1.1 auf echter Hardware bestätigt; Regression bei neuen Versionen bleibt sinnvoll.**
- [x] **UI-04 · P0** Buttons/Status außerhalb der überlagernden mobilen Browserleiste positionieren; Safe-Area testen. **v0.1.1 auf echter Hardware bestätigt; Regression bei neuen Versionen bleibt sinnvoll.**
- [ ] **UI-05 · P1** Navigationsstruktur Startseite, Player, Bibliothek, Programme, Editor, Banane/Licht, System/Diagnose.
- [ ] **UI-06 · P1** Bewusst an nostalgische CoolLED1248-App erinnern, deren Einschränkungen aber nicht übernehmen; **nicht** das SmallTV-Grau/Gelb.
- [ ] **UI-07 · P2** Touchoptimierung für markierte Textbereiche, Fonts, Farben, Vorlagen und Seitenübersicht.
- [ ] **UI-08 · P3** Optionale iOS-App, Apple-Watch-/Widget-Steuerung nur nach Nutzenabwägung; Browser ist die Basis.

## I · Qualität, Dokumentation, GitHub und Lizenzpflege

- [x] **DOC-01 · P0** Erste Firmware v0.1.0 unter eigenem Versionsnamen gesichert.
- [x] **DOC-02 · P0** Projekt-Themenspeicher, Roadmap, Änderungs- und Entscheidungslog angelegt (22.10.2026).
- [x] **DOC-03 · P0** Git eingerichtet, initial privates Repository unter `du13500/RavestickBoard-for-CoolLED1248` auf GitHub gesichert (22.10.2026).
- [ ] **DOC-04 · P1** README/Installationsanleitung und Changelog **bei jeder künftigen Version fortlaufend** aktualisieren; v0.2.0-Dokumente werden im aktuellen Audit nachgezogen.
- [ ] **DOC-05 · P1** Einheitliche Fehlermeldungen, Verhalten nach Stromausfall, Boot- und Dauerbetriebstests.
- [ ] **DOC-06 · P2** Originalanzeigen separat referenzieren, Normal/Bold und Hersteller-Eigenheiten dokumentieren.
- [x] **DOC-07 · P2** Initiale Codeherkunfts-/Lizenzprüfung durchgeführt und dokumentiert (23.10.2026), mit ausdrücklichen Prüfgrenzen; für neue Abhängigkeiten fortlaufend wiederholen.
- [x] **DOC-08 · P3** Entscheidung für bei Bedarf öffentliche Sichtbarkeit bei vorbehaltenen Rechten nach Lizenz-/Herkunftssichtung; Repository ist seit 23.10.2026 Public; am 24.10.2026 über GitHub bestätigt.
- [ ] **DOC-09 · P3** Mehrgerätefähigkeit und Verteilung an andere CoolLEDX-Besitzer testen, bevor allgemeine Kompatibilität behauptet wird.

## Themen, für die noch Entscheidungen/Versuche fehlen

- **Zeitquelle:** Countdown/Stoppuhr brauchen nur lokale Zeitmessung, geplante Uhrzeit-/Tageszeit-Programme eventuell eine externe Uhr oder Synchronisation vom Smartphone.
- **Bleiben Programme auf der Tafel?** Der letzte Anzeigeinhalt bleibt bei Stromverlust erhalten; mehrstufige ESP32-Programme sind noch nicht als tafelintern unterstützt nachgewiesen.
- **Helligkeit/Farbe vs. Vorschau:** Browserfarben entsprechen den 7 LED-Farben nur näherungsweise, reale Farberscheinung ist gesondert zu prüfen.
- **Schrift-/Pixelabmessungen:** Das Gerät meldet 64×16, trotz Produktname „CoolLED1248“; reale Punktgeometrie in Vorschau nachbilden.
- **Speicherbudget:** Große Animationen und viele Vorlagen benötigen Limits, Kompression und Diagnosewerte.
- **Reset und Logging:** Debugging ohne unkontrolliertes Flash-Schreiben, wichtige letzte Fehler ggf. gezielt persistent.

## Pflegebeispiel

Nach nachgewiesener Umsetzung einer Funktion: den passenden Eintrag (etwa `PRE-01`) abhaken und im `CHANGELOG` die aktuelle Version und einen echten Gerätetest festhalten. Bei einer Fehlfunktion neue ID oder Untereintrag aufnehmen und nicht stillschweigend vergessen.

## Aktualisierungen für v0.1.2

- [ ] **DIA-10 · P0** Diagnosebericht mit einem Tipp kopieren, einschließlich Safari-Fallback; v0.1.2 intern browsergetestet, iPhone offen.
- [ ] **UI-12 · P0** Deutsche UI-Texte mit Ä/Ö/Ü/ä/ö/ü/ß, keine ASCII-Ersatzformen; Code seit v0.1.2 umgesetzt, Browser-Regression offen.
- [ ] **BLE-13 · P1** Konkurrierende Original-App-Verbindung: 10-s-Timeout und sinnvolle Meldung unter Last real prüfen.

## Aktueller Blockeditor (v0.2.0 Korrektur 1, am 23.10.2026 hardwaregetestet)
- [x] Separater Reiter „Blockanzeige“ (eigene Seiten statt Textabschneiden).
- [x] Seiten hinzufügen, duplizieren, löschen und verschieben vom Nutzer bestätigt; **Grenzfall genau 12 Seiten** separat prüfen.
- [x] Individueller Text, Farben pro Zeile und Anzeigezeit vom Nutzer bestätigt; **Ausrichtungsvarianten** noch gesondert prüfen.
- [x] **Zwei kleine Zeilen** auf echter 64×16-LED-Matrix geprüft; **große einzeilige Darstellung** in der systematischen 14-Punkte-Tabelle unbewertet.
- [x] Speichern im ESP32-NVS, keine automatische Wiedergabe beim Booten.
- [x] Erste BLE-Wiedergabe von Einzelseiten und experimentelle **Seitenrotation**; das ist **noch keine Programm-Playlist**.
- [ ] Vorschau mit Wiedergabe 1× / 2× / 4× (kein 8×) **implementiert**; Bedienablauf auf dem iPhone gesondert testen.
- [x] ESP32-Kompilierung nach Korrektur 1, Zwei-Zeilen-Übertragung und automatische Seitenrotation praktisch bestätigt (23.10.2026); bekannter Mangel: Prozentanzeige zwischen Uploads.
- [ ] Feste Engschrift g/p/q/y: x-Höhe angleichen, unterste Pixelzeile für Unterlängen.
- [ ] Die Großumlaute Ä/Ö/Ü mit vorhandenen Normal-/Bold-Originalreferenzen überarbeiten.
- [ ] Live-BLE-Helligkeit und -Geschwindigkeit wie Original-App mit Feinskalierung nahe 80.
- [ ] Individuelle Textfarben und Schriftarten **pro Buchstabe**, gemischte Fonts pro Seite.
- [ ] Frei positionierbare Blöcke, Grafiken, eigene Animationen und Seitenübergänge.
- [ ] Native Mehrseiten-Wiedergabe der Herstellerhardware untersuchen.


## 23.10.2026 · Praxistest v0.2.0, Bibliothek und Architektur (Ergänzung, nicht die historischen Einträge überschreibend)

**Bestätigter Nutzer-Test:** 13/14 ursprüngliche Testpunkte mit „ja“; einzeilige Schrift hat stattdessen einen Erweiterungswunsch. ESP32-Start, iPhone-Oberfläche, Seiten hinzufügen/duplizieren/verschieben, Farben pro Zeile, zweizeilige Mini, Seitenwechsel, individuelle Anzeigedauer, NVS-Persistenz und wiederholte BLE-Übertragungen funktionieren. **Wichtig:** Der bestehende automatische Seitenplayer verursacht **bei jedem Wechsel eine tafelseitige Prozentanzeige**, weil jede Seite neu übertragen wird. Lauftext-Entwürfe gehen nach Browserneuladen verloren; gespeicherte Blockseiten bleiben erhalten.

- [ ] **BLK-01 · P1** **Drei Ebenen getrennt führen: Seite → Programm → Playlist.** Seite ist unabhängig gestaltbar. Ein Programm umfasst eine oder mehrere Seiten, die *idealerweise mit einem Upload* auf der Tafel autonom ablaufen. Die Playlist umfasst mehrere Programme, zwischen denen der ESP32 neu übertragen darf.
- [ ] **BLK-02 · P1** Prozentanzeige **innerhalb eines Programms vermeiden**: prüfen, ob der herstellereigene längere Blocktext oder die gespeicherte Animation `0x04` mehrere Seiten in einem Transfer tragen kann. Prozentanzeige bei tatsächlichem Programmwechsel innerhalb einer Playlist akzeptabel.
- [ ] **BLK-03 · P1** Individuelle Seitenanzeigezeiten sollen als Editorfunktion bestehen bleiben; bei boardnativen Beschränkungen wirkliche Unterstützung, Näherung oder klaren Hinweis unterscheiden. Nichts stillschweigend gleichschalten.
- [ ] **BLK-04 · P1** Aktuell funktionierenden BLE-Seitenplayer als **Grundlage der zukünftigen Playlist-Wiedergabe** erhalten, nicht ersatzlos löschen.
- [ ] **BLK-05 · P1** Eine eigenständige Seite niemals ungefragt an Pixelkante abschneiden, automatisch vergrößern/verkleinern oder auf Folgeseite verschieben. Überlauf sichtbar markieren; Assistenzfunktion zum Aufteilen höchstens optional.
- [ ] **BLK-06 · P2** Programmweites Verhalten (Wiederholung, gemeinsames Tempo, manueller Seitenwechsel, Übertragung/Status) getrennt von einzelner Seitengestaltung modellieren.
- [ ] **FNT-10 · P1** Bestehende **zweizeilige Mini-Schrift beibehalten**: Großbuchstaben-Praxistest überzeugt (JOINT / ZIGARETTE in zwei Farben).
- [ ] **FNT-11 · P1** Mini-Ausgabe grundsätzlich **in Großbuchstaben** umsetzen; auch bei klein eingegebener Schrift. Deutsche Sonderzeichen erhalten (`ä→Ä`, `ö→Ö`, `ü→Ü`, `ß→ẞ` als **ein** Zeichen). Originaltext für den Editor nicht verändern.
- [ ] **FNT-12 · P1** Zusätzliche Darstellung **„Mini, eine Zeile“**, 7 Pixel Schrift, vertikal mittig wie große Einzelzeile.
- [ ] **FNT-13 · P1** `ß` / `ẞ` in Mini gezielt korrigieren: den im Nutzerbild vom 23.10.2026 **blau markierten Punkt der untersten Pixelreihe ausschalten**. Beide Codepoints getrennt vorhalten, Aussehen gleich oder nahezu gleich.
- [ ] **FNT-14 · P1** Engschrift-Kleinbuchstaben `g/p/q/y` an **gemeinsamer x-Höhe** ausrichten, Unterlängen nach unten in die bisher freigelassene letzte Pixelreihe führen. `Ä/Ö/Ü` mit Original-App-Fotos überprüfen. Auf die gesamte Grundlinie achten.
- [ ] **LIB-11 · P1** Unfertigen Lauftext-/Blockentwurf automatisch und sparsam sichern; nach Browserneuladen **Reiter, ausgewählten Entwurf, Seite und Bearbeitungszustand** wiederherstellen. Nicht bei jedem Tastendruck ins Flash schreiben.
- [ ] **LIB-12 · P1** Trennung **Entwurf / Bibliothek / aktives Programm**. Das Öffnen einer Seite im Browser darf keinen nicht gespeicherten aktuellen Tafelinhalts-Upload auslösen.
- [ ] **LIB-13 · P1** Bibliothek primär auf dem **ESP32 selbst** vorsehen, sodass mehrere Geräte (iPhone, Laptop) auf dieselben gespeicherten Programme zugreifen; **LittleFS** im eingebauten Flash evaluieren (aktuell 4 MB Flash, Partition `No OTA (2MB APP/2MB SPIFFS)`). Begrenzungen/Flashverschleiß messen.
- [ ] **LIB-14 · P1** Programme als versionierte strukturierte Dateien (z. B. JSON) speichern, mit eindeutigen IDs, Namen, Formatierungen, Reihenfolge; öffnen, bearbeiten, duplizieren, umbenennen, löschen, favorisieren.
- [ ] **LIB-15 · P1** Export/Import und Backup der gesamten Bibliothek; **ESP32 darf nicht einzige Kopie** sein.
- [ ] **LIB-16 · P3** microSD über SPI am ESP32-C3 optional möglich (GPIO/3,3 V/Modul-Kompatibilität prüfen); **vorerst nicht nötig** für einfache Text-/Seitenprogramme, später für große Animationsmengen prüfen.
- [ ] **BLE-14 · P1** Bestehende Anzeigeninhalte ohne Neu-Upload steuern: `0x08` Helligkeit, `0x07` Tempo, `0x06` Anzeigemodus; **live während Schiebereglerbewegung**, letzte Werte bevorzugen, veraltete Werte verwerfen.
- [ ] **BLE-15 · P1** Für Live-Regler BLE-Session kurz halten, nach Idle gezielt lösen, damit Original-App wieder verbinden kann. Verbindungskonflikte, Rückmeldung und Timeout messen.
- [ ] **BLE-16 · P1** Modi der Original-App (Block, links, rechts, Laser, Schneefall usw.) per Steuerbefehl getrennt vom Inhalt testen. Moduscodes einzeln verifizieren, nicht erraten. Geschwindigkeits-Skala mit Original-App-Wert ~80 experimentell abgleichen.
- [ ] **QA-01 · P0** **Jeder neuen Firmwareversion und jedem Korrekturpaket eine kompakte, direkt im Chat kopierbare Testtabelle** (`Nr.`, `Bereich`, `Testpunkt`, `Ergebnis`, `Bemerkungen`) beigeben. Letzte zwei Spalten **leer**, keine Testtabelle als zusätzliche Download-Datei. Funktionstests + Regression.

## 23.10.2026 · Forschung: OpenSign / CoolLEDX-Animationen (NEU)

**Quelle:** [schulzad/coolled-sign-controller](https://github.com/schulzad/coolled-sign-controller), README, `protocol.md`, `ROADMAP.md` (abgerufen 23.10.2026). **Achtung: Ohne Lizenz; im README ausdrücklich „all-rights-reserved until one is added“. Keine Sourcecode-Übernahme!** Das Projekt bestätigt Funktionen an **seiner** 64×16-CoolLEDX-Referenztafel, Firmware 6; unsere konkrete Tafel muss separat geprüft werden.

- [ ] **ANM-01 · P1** **Gespeicherte Animation** über CoolLEDX-Opcode **`0x04`** kontrolliert auf unserer Tafel testen. OpenSign bezeichnet Speicherung und autonomen Loop für die Referenztafel als verifiziert; **nicht** als Nachweis für unsere Hardware markieren.
- [ ] **ANM-02 · P1** Referenzgrenze **ca. 30 KiB / bis zu 80 Frames bei 64×16** messen und die tatsächlich akzeptierten Größen, ACKs, Anzeigeverhalten, Stromausfall-Persistenz und Uploadzeiten unserer Tafel dokumentieren.
- [ ] **ANM-03 · P1** Zwei individuell gerenderte Seiten (Beispiel „SUCHE:“ / „JOINT + ZIGARETTE“) als **ein gespeichertes Animationspaket** hochladen, autonomen Wechsel **ohne wiederholte Prozentanzeige** testen.
- [ ] **ANM-04 · P1** **Zeitmodell der Animation** untersuchen: OpenSign dokumentiert **global gleiche Haltezeit pro Einzelbild**, kürzere Zeit = schneller; individuelle 5/7/10-s-Seitenzeiten nicht ohne Weiteres unterstützt. Möglichkeiten mit wiederholten Frames, abgestufter Gesamtdauer, Frameslimit und Bildrate prüfen; keine freie Einzelbilddauer behaupten.
- [ ] **ANM-05 · P1** **Native Blocktext-Darstellung** (mehrere Seiten über `0x02`/Modus) gegen `0x04` Animation vergleichen: Qualität, Handhabung, Farbmix, 2 Zeilen, Übertragung, Grenzen und Prozentanzeige.
- [ ] **ANM-06 · P2** Eigene Effekte als **vorgerenderte 64×16-Bildfolgen**: Wellenlauf, Glitzer/Sternenhimmel, pulsierendes Herz, Pixel-Vorhang, einblendende Buchstaben, Aufblitzen, Auflösen, freie Seitenübergänge; Browser/ESP32 als Renderer, Tafel als Loop-Player.
- [ ] **ANM-07 · P2** Grafik-/Animation-Import: **GIF, WebP, APNG**, Skalierung/Seitenverhältnis, Beschnitt, transparente/key-color Hintergründe, Vorschau und Importlimits; bei ESP32-Ressourcen Browser-seitige Vorberechnung bevorzugen.
- [ ] **ANM-08 · P2** Nur hardwareechte **7 LED-Farben + Schwarz** rendern, optional Ordered-/Floyd–Steinberg-Dithering für Bilder; **temporales Dithering/FRC** und Flimmern separat prüfen, keine echten zusätzlichen LED-Farben behaupten.
- [ ] **ANM-09 · P2** Pixel-/Grafikeditor, Spritebewegung und programmierbare Übergänge entwickeln; Wellen-/Partikeleffekte bei Framelimit auf Speicher, Geschwindigkeit und Qualität prüfen.
- [ ] **ANM-10 · P1** **Echtzeit-Streaming nicht voraussetzen.** OpenSign beschreibt für sein Referenzboard „store-and-loop“, kein funktionierendes permanentes Frame-Streaming. Unsere dynamischen Uhren/Countdowns/Scoreboards brauchen eigene technische Strategie.
- [ ] **ANM-11 · P1** **Native Effekte (`0x06`) vs. eigene `0x04`-Animationen vs. Controller-Playlist** als getrennte Ausführungswege; Fähigkeit/Protokoll-Kompatibilität nach Modell/Firmware versionieren.
- [ ] **ANM-12 · P2** OpenSign-Idee **Evidenzstufen** adaptieren: `unverified` → `experimental` → `verified` nur nach realem eigenen Hardwaretest, mit protokollierter Firmware-/Boardrevision, erwarteten Notifications, Rollback und manueller Sichtkontrolle. Keine fremden Testergebnisse als eigene deklarieren.
- [ ] **ANM-13 · P2** Protokolldiagnose: Chunk-ACKs, akzeptierte Übertragung, Timeout/NAK, reales sichtbares Ergebnis separat messen, niemals BLE-Write automatisch mit erfolgreicher Anzeige gleichsetzen.
- [ ] **ANM-14 · P2** Latenz und RAM-Bedarf für Animation kompilieren/übertragen gegen reine Textübergänge messen. Firmware-/Datenspeicher des ESP32 sowie Tafel-Puffer sauber getrennt betrachten.

## 23.10.2026 · Lizenz-/Provenienzentscheidungen zu OpenSign

- [x] **LIC-01 · P0** OpenSign/`schulzad/coolled-sign-controller` ist **öffentlich sichtbar, aber ohne Nutzungslizenz**. Das README verlangt, die Implementierung als **all rights reserved** zu behandeln. **Keinen geschützten Sourcecode, keine Grafiken oder langen Textpassagen übernehmen oder aus Python nahezu zeilengleich nach C++ übertragen.**
- [ ] **LIC-02 · P0** **Eigenständige ESP32-C3-Implementierung** ausschließlich anhand protokolltechnischer Fakten, öffentlich zugänglicher Funktionsbeschreibungen und eigener BLE-Messungen. Testdaten dokumentieren, eigene Architektur, eigene Benennungen, eigener Code und eigene Testvektoren. Ein bloßes Umbenennen/Umformatieren ist **kein Clean-Room-Verfahren**.
- [x] **LIC-03 · P1** E-Mail an Adam Schulz am **23.10.2026 versendet**: schriftliche Freigabe für Übernahme, Anpassung, Portierung nach C++ und Veröffentlichung auf GitHub angefragt. **Kontaktaufnahme erledigt; Antwort und Lizenzberechtigung offen.**
- [ ] **LIC-04 · P1** Separat `UpDryTwist/coolledx-driver` mit **MIT-Lizenz** als mögliche rechtssichere Codequelle prüfen; bei tatsächlicher Übernahme Lizenz- und Copyright-Hinweise beibehalten. Keine pauschale Lizenzübertragung von MIT auf andere Repositories annehmen.
- [ ] **LIC-05 · P1** Jede neue Implementierung/Quelle mit URL, Abrufdatum, technischem Fakt vs. übernommenem Code, Nachweis und Teststatus im Lizenzregister dokumentieren. Initialer Sketch-/Paketabgleich wurde 23.10.2026 durchgeführt, eine private Git-Historie nicht unabhängig geprüft; laufende Pflege erforderlich.

**Offene technische Entscheidung:** Ist das gewünschte „mehrere Seiten, pro Seite individuell formatiert und unterschiedlich lange sichtbar, nur ein Upload“ als native Boardfunktion möglich, oder brauchen wir eine begrenzte Bildfolge mit entsprechendem Timeraster? Erst anhand unserer eigenen Hardwareversuche entscheiden. **Bestehende v0.2.0-Firmware nicht ungeprüft ersetzen.**


## 23.10.2026 · GitHub, Dokumentationsdesign und fortlaufender Testprozess

- [x] **DOC-10 · P1** Dokumente werden zu jeder neuen Firmwarefassung zusammen mit README, Changelog und Roadmap aktualisiert. **Keine alten Statusbehauptungen zurückkopieren.**
- [x] **DOC-11 · P1** README-Gestaltung: Zentriertes eigenes Totem-Foto `docs/assets/totem.jpg` **ohne width/style-Vorgabe**, zentrierter 🍌-Titel, englische Badges `version`, `hardware`, `status: hardware testing`, `project: non-commercial`, `license: all rights reserved`, `mission: hugs and love`. Fließtext deutsch. **Keinen ausgeschiedenen Abschnitt „Projektstruktur“ wieder einführen.**
- [ ] **DOC-12 · P1** Bei nächster README-Überarbeitung die **eigens entwickelte zweizeilige Minischrift** ausdrücklich benennen. Nicht eigens die vom Nutzer angepasste README bei einer reinen Dokumentenpflege überschreiben.
- [x] **DOC-13 · P1** **Zu jedem Firmware-Updatepaket eine kurze, Excel-kopierbare Testtabelle ausschließlich im Chat** (Nr., Bereich, Testpunkt, Ergebnis, Bemerkungen), mit leeren Feldern für Nutzerbefunde; kein separater Excel-Download und keine Pflichtdatei im Updatepaket. `docs/TESTPLAN.md` bleibt technische Testhistorie/Checkliste.
- [x] **LIC-06 · P1** Bewusste Freigabeentscheidung: Repo darf sichtbar gemacht werden, eigene Teile bleiben mangels Open-Source-Freigabe **all rights reserved**. Die GitHub-Sichtbarkeit ist keine neue Firmwarefunktion und kein offener „Release“-Meilenstein. Das Repository ist Public, die Lizenz des eigenen Codes bleibt All Rights Reserved.
- [x] **LIC-07 · P1** Festes AP-Passwort `Banane1248!` ist bewusst öffentlich dokumentierbar; individuell änderbares Passwort bleibt Zukunftsthema. Fest codierte BLE-MAC ist ein dokumentierter Gerätespezialfall. Totem-EXIF (iPhone-Modell/Festivalgeotag) wird akzeptiert.
- [ ] **LIC-08 · P1** Antwort von Adam abwarten und Bedingungen einer eventuellen Erlaubnis schriftlich dokumentieren, bevor OpenSign-Code übernommen oder portiert wird. **Keine Freigabe bislang bestätigt.**
- [ ] **SYS-13 · P2** WLAN-AP-Passwort optional über Weboberfläche ändern, mit sicherer Wiederherstellung bei Verlust und klarer Bedienanleitung.

**Hinweis zur IDs-Konsolidierung:** In einer alten Fassung war `FNT-06` zweimal vergeben. Die gemeinsame Glyphentabelle wird ausschließlich unter `FNT-08` geführt; `FNT-06` bezeichnet allein die Mini-Schrift. Bestehende Aufgaben werden nicht gelöscht.

## 24.10.2026 · v0.3.0 Entwicklungsstand (ohne Gerätetest)

- [ ] **SYM-01 · P1** 24 Originalsymbole aus `CoolLED1248_Originalsymbole_v01.json` inline in Lauf- und Blocktext integrieren. **Code vorhanden, Hardwareprüfung offen.**
- [ ] **SYM-02 · P1** Symbolbreiten anhand beleuchteter Spalten berechnen; effektive Original-App-Zeichenabstände/Advance in späterem Vergleich nachmessen. **Vorläufig +1 px; nicht boardvalidiert.**
- [ ] **SYM-03 · P1** Das Hersteller-Herz in allen sieben LED-Farben einfärben; unabhängige Emoji-Farbvarianten. **Implementiert, Hardwareprüfung offen.**
- [ ] **SYM-04 · P1** Eigenes RavestickBoard-Bananen-Symbol in vertikal gespiegelter Emoji-Anmutung entwickeln; unabhängig von Hersteller-Banane kennzeichnen. **Erster Entwurf integriert, Nutzerabnahme offen.**
- [ ] **SYM-05 · P1** Originalsymbol- und Eigensymbol-Bibliotheken sichtbar getrennt verwalten, Quellenhinweis in `QUELLEN_UND_LIZENZEN.md`. **Implementiert, visuelle Abnahme offen.**
- [ ] **FNT-15 · P1** 82 Normal- und 82 Bold-Originalglyphen aus `CoolLED1248_Originalglyphen_v02.json` laden; tatsächlichen Vorschub und reale Darstellung testen. **Code vorhanden, Boardtest offen.**
- [ ] **FNT-16 · P1** „Mini, eine Zeile“ horizontal und vertikal zentrieren; Großbuchstabenautomatik inklusive Ä/Ö/Ü/ẞ. **Code vorhanden, Boardtest offen.**
- [ ] **BLE-16 · P1** Helligkeit, Tempo, Modus ohne Content-Upload und mit kurzer Wiederverwendung der BLE-Verbindung senden. **Code vorhanden, Hardwaretest offen.**
- [ ] **LIB-16 · P1** Lauftextentwurf/aktiven Reiter im Browser wiederherstellen. **Implementiert, iPhone-Test offen.**
- [ ] **DOC-10 · P1** OpenSign-Mail 23.10.2026 und öffentliches GitHub-Repository durchgehend aktuell dokumentieren. Antwort ausstehend.

Die bestehenden v0.2.0-NVS-Seiten werden im v0.3.0-Entwurf nicht automatisch gelöscht oder migriert. **Keine Auto-Ausführung nach dem Booten.**
