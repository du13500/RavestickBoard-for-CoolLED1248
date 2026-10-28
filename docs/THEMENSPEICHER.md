# Themenstand v0.4.2.3-dev

## Ergänzung v0.4.2.3-dev – unsere neuen Themen

- **Alles als Laufband:** Alternative zu gemischten, stehenden Blockfolgen ist programmiert: Lauftext und jede 64×16-Blockseite mit 16 Leerspalten horizontal verbinden. **Noch nicht auf CoolLEDX abgenommen.** Längere Programme werden am Inhalt/Seitenrand unterteilt; Übergänge bleiben vorerst zeitgesteuert.
- **Lucide-Oberfläche:** Iconplan als lokale Inline-SVG-Sammlung, inklusive acht manueller Direktbefehle ohne aktive Auswahlmarkierung; Lizenztexte für ISC und Feather/MIT stehen in `THIRD_PARTY_NOTICES.md` und sind in `LICENSE` referenziert. Hardware- und iPhone-Test ausstehend.
- **Backup-Import:** Erkennung identischer Namen **und** Inhalte und ID-Neuzuordnung ist eingeführt, Test mit mehrfach importierten Backups noch ausstehend.
- **UI-Korrektur:** „+ Neu“ im Editor, leere Kurzwahl öffnet Einrichtung, defektes Programm in Kurzwahl optisch hervorgehoben.
- **Noch offen:** reale Lauftextzeitkalibrierung; Tempo-Sperrverhalten bei Programmende prüfen; langfristige Transaktionssicherheit von Backup-Merges; Stromausfalltests.

## Messwerte und offene Fragen zu gemischten Programmen (v0.4.2.2)

Originalmessungen auf CoolLEDX, Zeit vom Start der ersten Seite bis zu deren erneutem Erscheinen:

| Seiten | Tempo | Start | Ende | Umlauf |
|---|---|---|---|---|
| 4 | 60 | 00:01,64 | 00:14,29 | 12,65 s |
| 4 | 70 | 00:00,93 | 00:10,35 | 9,42 s |
| 4 | 80 | 00:00,30 | 00:06,61 | 6,31 s |
| 4 | 90 | 00:00,34 | 00:03,43 | 3,09 s |
| 6 | 80 | 00:09,64 | 00:19,14 | 9,50 s |

Heuristik im getesteten Bereich: `T[s] ≈ Seiten × 0,0795 × (100 - Tempo)`, nicht automatisch auf Tempo 1–59 oder 91–100 extrapolieren. Die Formel ist für Blöcke, nicht für laufende Pixelband-Texte kalibriert. Exakter Start der Darstellung nach dem BLE-Upload und Synchronisationsjitter bleiben offen. Gemischte Programmgruppen dürfen Prozentanzeigen beim notwendigen Upload haben. Direkte Nutzertests für TEXT → BAND → TEXT und leere/verwaiste Programmschritte ausführen.


- **Datenbudget prüfen:** Das vorläufige Limit von 30 KiB bleibt aus Sicherheitsgründen in der Firmware. Tatsächliche Hardwaregrenze der CoolLEDX und Verhalten jenseits 30 KiB später systematisch bestimmen, ohne unkontrolliertes Übertragen im Livebetrieb.
- **Erweiterter Editor:** Einzelne Buchstaben/Abschnitte einfärben; erst dort ist eine benutzerdefinierte Rückgängig-/Wiederholen-Historie sinnvoll. Native Texteingabe-Rücknahme bleibt im Basis-Editor erhalten.
- **BLE-Modi:** Alle acht Darstellungsmodi laut erstem Nutzer-Hardwaretest funktionsfähig, einschließlich Anwendung auf verkettete Blockbilder. Technische Kommandonummern gehören in die Dokumentation/Diagnose, nicht in die Standardauswahl.
- **Programmgruppen:** Für gemischte Gruppen erfolgt weiterhin ein Upload bei einem Wechsel des Modus; Optimierungen und Prüfung der Gruppendauer als eigene Tests.
- **Speichergrenze:** Der Bibliotheksindex erlaubt bis zu 192 IDs; variable Dateigrößen, Flash-Verbrauch und Stromausfalltests bleiben Zukunftsthemen.

---

# Stand v0.4.2-dev: Neue, noch ungetestete Firmwarefunktionen siehe [ROADMAP](ROADMAP.md) und [ENTWICKLUNGSSTAND](ENTWICKLUNGSSTAND_v0.4.2.md). Die unten stehenden älteren Einträge sind Planungshistorie, nicht der aktuelle Status.

## Ergänzung v0.3.5

### v0.3.5 – Stand der Piktogramme

Kussmund, Blume, Biene, Kuh als vollständige 16×16-Farbmatrizen aus Original-App-Screenshots rekonstruiert; LED-Board-Abgleich fehlt noch. Eigene Bilder: Ravestick-Banane, Quietscheente, Kaktus, UFO, UFO mit Strahl, Alien grün/schwarze Augen. Stern y-zentriert, Blume rechts ein Pixel enger. Weiter offen: weitere Ideen aus früheren Listen (z. B. Tropfen, Katze) nach gesonderter Planung. Die Symbolüberlagerung über zwei Mini-Zeilen wird ausdrücklich beibehalten.

---

# Themenspeicher und Aufgabenregister


## Nachtrag 25.10.2026 – v0.3.4 ist ein neues Feature, keine v0.3.3-Korrektur

- v0.3.3 **auf echter Hardware bestätigt**: 650 Textzeichen / 30.689 B, 301 gemischte Zeichen / 30.695 B, zwei Farben für synchronen Mini-Lauftext; sieben gespeicherte Seiten erhalten. Die damaligen „ungeprüft“-Markierungen in älteren Abschnitten sind historisch.
- v0.3.4 führt `0x03`–`0x08` als **zusätzliche Parameter des Kommandos `0x06`** im normalen Modus-Dropdown; Tests auf der konkreten Tafel noch offen. Ein separater „Testbereich“ ist nicht gewünscht.
- Priorität: reale Namen, Wirkungen und sichtbare Prozentanzeige zu jedem Wert dokumentieren; `0x04` als Modus **nicht** mit `0x04` als eigenständigem Animationsbefehl verwechseln.

## Nachtrag 25.10.2026 – Nächste Entwicklungsschritte nach v0.3.3

- v0.3.3: Testkandidat für datenbasierten Lauftext-Editor ohne 600-Zeichenlimit, vorläufige 30-KiB-Auslastung, individuelle Farbe pro Zeile in Mini Zweizeilig (noch nicht auf echter Hardware getestet).
- Nach neuer Grenzmessung: Die Byte-Obergrenze vom tatsächlichen LED-Board-Verhalten abhängig machen, inklusive RAM-Limit des ESP32-C3. 300 × ⚠️ (30.701 B) ist knapp innerhalb des vorläufigen Budgets, 301 × ⚠️ außerhalb.
- **Bibliothek:** Einzeltexte unabhängig mit Schrift, Farben, Symbolen, Titel und ggf. Geschwindigkeit speichern und bearbeiten.
- **Verketteter Lauftext als Programm:** Auswahl mehrerer Bibliothekseinträge, Reihenfolge und Leer-Spalten frei wählbar, offline zu einem einzigen 16-Pixel-hohen Pixelstreifen zusammenstellen, einmal per BLE übertragen. Vorteil: nicht zwischen den Nachrichten erneut die Fortschrittsprozentanzeige. Die Datenkapazität muss für alle zusammengesetzten Beiträge reichen.
- Echte Blockseite mit eigener Standzeit / eigenem Modus ist **nicht** automatisch in einem verknüpften Lauftext darstellbar; dafür später Hersteller-BLE-Format genauer erforschen oder separates Ersatzkonzept.
- Fehlende Symbole (u. a. Kuss, Blume, Biene), neu gezeichnete Banane/Gummiente, selektive Zeichenformatierung, Vorschau-Rework und BLE-Seitenplayer-Stabilität bleiben auf der alten Roadmap.

## Zwischenstand 24.10.2026 · v0.3.1-Testkandidat, noch nicht hardwarebestätigt

**Nicht mit einer fertigen Abnahme verwechseln:** Für v0.3.1 sind die nachfolgenden Teilumsetzungen im Quellcode enthalten und browserseitig syntaktisch beziehungsweise per simuliertem API-Smoke geprüft, aber Arduino-Kompilierung, LED-BLE-Transfer und iPhone-Praxistest sind noch ausstehend.

- **`EDT-11`, `QA-02` (Teilumsetzung):** 80 statt 40 Lauftext-Zeichen, größere Pixelpuffer und passende Browserprüfung. Da der bestehende Protokollkopf nur 80 Zeichen-Slots vorsieht, ist darüber hinaus keine Erhöhung zugesagt. Die reale maximale Zeichen-/Pixelbreite der Tafel bleibt offen. Tests mit Symbolketten besonders wichtig.
- **`EDT-22`, `LIB-11` (Teilumsetzung):** Demotext aus dem Lauftextfeld entfernt, bisheriger `localStorage`-Schlüssel für den Browserentwurf bleibt. Dauerhafte Bibliothek und Speicherung des vollständigen Undo-Verlaufs sind weiterhin offen.
- **`EDT-15` (Teilumsetzung):** Undo und Redo für **Textänderungen in Lauftext / Zeile 1 / Zeile 2**, inklusive per Picker eingefügter Symbole, maximal 100 Zustände je Eingabefeld pro Browsersitzung. **Noch offen:** Löschen/Verschieben von Seiten, Farben, Fonts, Mehrfachauswahl, nach Reload rekonstruierter Undo-Verlauf. Kein Undo-Schritt überträgt automatisch ans Board.
- **Symbol-Picker / mobile UI (Teilumsetzung):** Knopf am jeweiligen Textfeld, Cursorposition vor Öffnen sichern, Overlay öffnen, Symbol an Position einfügen. Der alte Picker am unteren Seitenende entfällt.
- **`FNT-19` (Teilumsetzung):** Mini-Leerzeichen nun **2 Pixel interne Leerbreite + 1 Pixel Zwischenabstand**. Noch per Vergleichsbild und auf dem realen Board beurteilen. Originalschriftarten und deren Leerzeichen unverändert.
- **Vorschau/Helligkeit (Teilumsetzung):** Sticky-Vorschau und Entfernen des redundanten Helligkeit-Buttons. Helligkeitssignal bleibt am Live-Schieberegler. Vier gewünschte Schriftlabels vorrangig; alte Engschrift bleibt zusätzlich als Bestands-/Kompatibilitätseintrag sichtbar, damit gespeicherte Altseiten weiterhin bearbeitbar sind.
- **`EDT-24` offen:** Jede Zeile der zweizeiligen Mini-Blockseite separat links/zentriert/rechts ausrichten. Hierfür noch keine NVS-Migration eingebaut; bestehende Seiten niemals still überschreiben.
- **`SYM-09` bis `SYM-16` offen:** Kussmund, Blume, Biene, Sternlage, Blüten-Abstand, neue Banane und Quietscheente bleiben **geplant**, nicht in v0.3.1 enthalten.
- **🎨-Formatierungseditor und freie Hybridlayouts offen:** Nicht in v0.3.1; gezielt als gesonderte größere Entwicklungsschritte behandeln.


**Stand:** 24.10.2026, einschließlich Praxistest der v0.3.0-dev und der danach vereinbarten Änderungswünsche. Die vorherigen Einträge bleiben zur Rückverfolgbarkeit erhalten. Neue Funktionen gelten erst nach Umsetzung und Prüfung als verfügbar.  
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

## Zukunft: Hardware-Animationen und Quellenkontrolle

- [ ] **ANM-01 · P1** Separaten CoolLEDX-Opcode `0x04` zunächst mit sieben vorhandenen 64×16-Ambiente-Farbbildern und einer sicheren seriellen Testkonsole auf der eigenen Tafel erproben. Nur real bestätigte Ergebnisse in die Firmware übernehmen.
- [ ] **ANM-02 · P1** Frame-Limit, Zeitsteuerung, Wiederholung, Dateiformat, ACK und reales Anzeigeverhalten messen. Nie aus BLE-Quittungen auf sichtbaren Erfolg schließen.
- [ ] **ANM-03 · P2** GIF-Import, Farbkonvertierung auf sieben LED-Farben, browserseitige Vorberechnung und Animationsvorschau erforschen.
- [ ] **ANM-04 · P2** Native Animation `0x04`, Effektmodus `0x06` und Controller-gesteuerte Programme voneinander trennen.
- [ ] **SYM-WIDE · P2** Frei gestaltbare Symbole mit 16 Pixel Höhe und variabler Breite (z. B. 40×16 Flugzeug) im Renderer und in der Vorschau ermöglichen; 16×16-Originalsymbole bleiben erhalten.
- [ ] **LIC-04 · P1** Bei tatsächlicher Übernahme fremder MIT- oder Apache-lizenzierter Bestandteile Lizenztexte und Urheberhinweise beibehalten; nur technische Protokollrecherche erfordert keine Codeübernahme.

## 23.10.2026 · GitHub, Dokumentationsdesign und fortlaufender Testprozess

- [x] **DOC-10 · P1** Dokumente werden zu jeder neuen Firmwarefassung zusammen mit README, Changelog und Roadmap aktualisiert. **Keine alten Statusbehauptungen zurückkopieren.**
- [x] **DOC-11 · P1** README-Gestaltung: Zentriertes eigenes Totem-Foto `docs/assets/totem.jpg` **ohne width/style-Vorgabe**, zentrierter 🍌-Titel, englische Badges `version`, `hardware`, `status: hardware testing`, `project: non-commercial`, `license: MIT`, `mission: hugs and love`. Fließtext deutsch. **Keinen ausgeschiedenen Abschnitt „Projektstruktur“ wieder einführen.**
- [ ] **DOC-12 · P1** Bei nächster README-Überarbeitung die **eigens entwickelte zweizeilige Minischrift** ausdrücklich benennen. Nicht eigens die vom Nutzer angepasste README bei einer reinen Dokumentenpflege überschreiben.
- [x] **DOC-13 · P1** **Zu jedem Firmware-Updatepaket eine kurze, Excel-kopierbare Testtabelle ausschließlich im Chat** (Nr., Bereich, Testpunkt, Ergebnis, Bemerkungen), mit leeren Feldern für Nutzerbefunde; kein separater Excel-Download und keine Pflichtdatei im Updatepaket. `docs/TESTPLAN.md` bleibt technische Testhistorie/Checkliste.
- [x] **LIC-06 · P1** Bewusste Freigabeentscheidung: Repo darf sichtbar gemacht werden, eigene Codebestandteile sind inzwischen **MIT-lizenziert**. Die GitHub-Sichtbarkeit ist keine neue Firmwarefunktion und kein offener „Release“-Meilenstein. Das Repository ist Public, die aktuelle Lizenz des eigenen Codes ist MIT.
- [x] **LIC-07 · P1** Festes AP-Passwort `Banane1248!` ist bewusst öffentlich dokumentierbar; individuell änderbares Passwort bleibt Zukunftsthema. Fest codierte BLE-MAC ist ein dokumentierter Gerätespezialfall. Totem-EXIF (iPhone-Modell/Festivalgeotag) wird akzeptiert.
- [ ] **SYS-13 · P2** WLAN-AP-Passwort optional über Weboberfläche ändern, mit sicherer Wiederherstellung bei Verlust und klarer Bedienanleitung.

**Hinweis zur IDs-Konsolidierung:** In einer alten Fassung war `FNT-06` zweimal vergeben. Die gemeinsame Glyphentabelle wird ausschließlich unter `FNT-08` geführt; `FNT-06` bezeichnet allein die Mini-Schrift. Bestehende Aufgaben werden nicht gelöscht.

## 24.10.2026 · v0.3.0 Entwicklungsstand (historischer Stand vor dem Gerätetest)

- [ ] **SYM-01 · P1** 24 Originalsymbole aus `CoolLED1248_Originalsymbole_v01.json` inline in Lauf- und Blocktext integrieren. **Code zum Zeitpunkt dieser Planung vorhanden; siehe nachfolgenden Praxistest.**
- [ ] **SYM-02 · P1** Symbolbreiten anhand beleuchteter Spalten berechnen; effektive Original-App-Zeichenabstände/Advance in späterem Vergleich nachmessen. **Vorläufig +1 px; nicht boardvalidiert.**
- [ ] **SYM-03 · P1** Das Hersteller-Herz in allen sieben LED-Farben einfärben; unabhängige Emoji-Farbvarianten. **Implementiert, Praxistest siehe nachfolgenden Abschnitt.**
- [ ] **SYM-04 · P1** Eigenes RavestickBoard-Bananen-Symbol in vertikal gespiegelter Emoji-Anmutung entwickeln; unabhängig von Hersteller-Banane kennzeichnen. **Erster Entwurf integriert, Nutzerabnahme offen.**
- [ ] **SYM-05 · P1** Originalsymbol- und Eigensymbol-Bibliotheken sichtbar getrennt verwalten, Quellenhinweis in `QUELLEN_UND_LIZENZEN.md`. **Implementiert, visuelle Abnahme offen.**
- [ ] **FNT-15 · P1** 82 Normal- und 82 Bold-Originalglyphen aus `CoolLED1248_Originalglyphen_v02.json` laden; tatsächlichen Vorschub und reale Darstellung testen. **Code zum Zeitpunkt dieser Planung vorhanden; siehe nachfolgenden Praxistest.**
- [ ] **FNT-16 · P1** „Mini, eine Zeile“ horizontal und vertikal zentrieren; Großbuchstabenautomatik inklusive Ä/Ö/Ü/ẞ. **Code zum Zeitpunkt dieser Planung vorhanden; siehe nachfolgenden Praxistest.**
- [ ] **BLE-14/15/16 (Referenz)** Helligkeit, Tempo und Modus ohne Content-Upload mit kurzzeitiger Wiederverwendung der BLE-Verbindung senden. **Code vor dem Praxistest vorhanden; Ergebnis siehe nachfolgenden Abschnitt.**
- [ ] **LIB-11 (Referenz)** Lauftextentwurf/aktiven Reiter im Browser wiederherstellen. **Implementiert; Praxistest siehe nachfolgenden Abschnitt.**

Die bestehenden v0.2.0-NVS-Seiten werden im v0.3.0-Entwurf nicht automatisch gelöscht oder migriert. **Keine Auto-Ausführung nach dem Booten.**


## 24.10.2026 · Praxistest v0.3.0-dev (nachgetragen, kein neuer Firmwarestand)

**Quelle:** Nutzer-Testtabelle mit zwölf nummerierten Prüfungen sowie den Merkern Z1 bis Z3, Screenshots der Original-App und der RavestickBoard-Vorschau. Diese Nachtragung dokumentiert Beobachtungen und Anforderungen. Sie ist **keine Implementierung** von v0.4.0. Die zuvor als „ohne Gerätetest“ beschriebenen v0.3.0-Funktionen wurden am 24.10.2026 praktisch überprüft.

**Bestätigt im durchgeführten Test:**

- Firmware v0.3.0 kompiliert, startet auf ESP32-C3; gespeicherte v0.2.0-Blockseiten bleiben erhalten.
- Rekonstruierte Originalfonts **Normal und Bold** passen in der geprüften Darstellung zur Hersteller-App; „Mini, eine Zeile“ ist horizontal und vertikal zentriert, „Mini, zwei Zeilen“ funktioniert weiterhin.
- Originalsymbole lassen sich mitten im Text platzieren; das Herz kann in **allen sieben LED-Farben** angezeigt werden.
- Helligkeit, Geschwindigkeit und Modus lassen sich per BLE ohne neuen Bild-Upload beeinflussen; der Helligkeitsschieber reagiert unmittelbar.
- Wiederholte Übertragungen und Diagnose funktionierten im kurzen Praxistest.
- Erste Ravestick-Banane ist technisch darstellbar, gestalterisch aber **nicht** akzeptiert: Form wirkt wie eine Bananenschale statt wie die Totem-Banane.

**Offen oder auffällig:** Symbolbreiten/Abstände wurden noch nicht systematisch mit der Hersteller-App vermessen (`SYM-02`); der Lauftextentwurf sprang nach Neuladen wieder auf „Grüße!“ (`LIB-11`, `EDT-22`); der feste Textlängencheck lehnt lange Lauftexte und Symbolfolgen bereits auf HTTP-Ebene ab (`EDT-11`, `QA-02`). Die stabilen Kurztests ersetzen noch keinen mehrstündigen Dauertest (`SYS-10`).

### Korrekturen vor dem nächsten größeren Funktionsausbau

- [ ] **QA-02 · P0 · Längentest und BLE-Grenzen.** **40 Zeichen sind eine Begrenzung unserer Firmware, kein nachgewiesenes Hardwarelimit.** Referenz-Screenshots zeigen mehrere Hundert Zeichenpositionen in der Original-App. Fehlermeldung `Ungültige Textlänge oder leer` kommt bei unseren gescheiterten Versuchen schon aus der HTTP-Validierung, bevor BLE sendet. Im nächsten Firmwarekandidaten logische Zeichen/Emoji-Sequenzen, UTF-8-Bytes, Glyphenbreite, gesamte Pixelspalten und Payload getrennt begrenzen bzw. messen. Versuchsreihe mit 50/100/200/300 Buchstaben, schmalen `i`, breiten `W`, verschiedenen Symbolfolgen; BLE-ACK **und** bis zum Ende sichtbare Anzeige prüfen. Kein willkürliches neues Hardware-Maximum behaupten. Aussagekräftige Fehler statt einer irreführenden pauschalen Zeichengrenze.
- [ ] **QA-03 · P1 · Regressionsmatrix v0.3.0.** Beim nächsten Firmwarepaket gezielt Version/Start, NVS-Bestandsschutz, Fonts, Mini-Ausrichtung, Symbol-Farben, Inline-Symbole, Live-BLE, Entwürfe, Mehrfachübertragung, Langzeittest und längere Texte nachprüfen; `Ergebnis`/`Bemerkungen` zunächst leer lassen. Historische Nutzerresultate nicht als Freigabe für zukünftige Änderungen übernehmen.
- [ ] **EDT-20 · P1 · Symboleingabe am Textfeld.** Statt tief unten platziertem Symbolbereich ein direkt am jeweiligen Eingabefeld angeordnetes Symbol-Picker-Icon; beim Öffnen Cursor-/Auswahlposition merken und Symbol dort einfügen, nicht starr ans Zeilenende. Originalsymbole und Ravestick-Eigensymbole unterscheidbar lassen.
- [ ] **EDT-21 · P1 · Schriftauswahl verständlich benennen und sortieren.** Exakte Reihenfolge: **`Original - Normal`**, **`Original - Bold`**, **`Mini - Zweizeilig`**, **`Mini - Einzeilig`**. Keine technischen Erklärungen zu Zentrierung oder Pixelhöhe in der Auswahlliste. Die bisherige Engschrift als eigenständige Schrift im Entwicklungsbestand nicht versehentlich löschen; ihre zukünftige Präsentation separat abstimmen.
- [ ] **EDT-22 · P1 · Lauftext ohne Demo-Vorbelegung.** In einem neuen, bewusst leeren Lauftextentwurf kein `Grüße!` einsetzen. Begonnene **ungespeicherte** Entwürfe hingegen automatisch wiederherstellen, auch nach Browser-Reload. „Neuer Entwurf“ von „letzten Entwurf fortsetzen“ unterscheiden; keine stillschweigende Löschung. Bestehende v0.2.0-NVS-Blockseiten unverändert lassen.
- [ ] **UI-13 · P1 · Feststehende LED-Vorschau.** 64×16-Vorschau beim Scrollen im Editor sichtbar halten (sticky); auf kleinen iPhones bei Bedarf zu platzsparender oberer Ansicht verkleinern und beim Zurückscrollen wieder groß darstellen. Keine störende Überdeckung von Eingaben, Tastatur oder iOS-Browserleisten. Während der Formatierung Ergebnis live sichtbar machen.
- [ ] **UI-14 · P1 · Live-BLE-Bedienung vereinfachen.** „Helligkeit übertragen“ entfernen, sofern Schieber bereits zuverlässig direkt sendet. Für den Modus statt Dropdown **zwei nebeneinanderliegende Icon-Schaltflächen** für Lauftext und Standbild untersuchen; Tippen sendet jeweiligen Modusbefehl ohne Bildneuübertragung. Die **Editorwahl** Lauftext/Blockanzeige bleibt davon getrennt. Geschwindigkeit und Helligkeit als kompakte Fernbedienung der laufenden Anzeige bündeln, ohne unbeabsichtigten Content-Upload.
- [ ] **LIB-17 · P2 · Neues Blatt vs. Bibliothek.** Bis zur Einführung der Bibliothek bestehende gespeicherte Blockseiten im Editor belassen. Danach neuen Blockeditor **leer beginnen lassen**, vorhandene Programme aus Bibliothek explizit öffnen, bearbeiten und speichern. Alte NVS-Seiten bei Migration übernehmen, nicht überschreiben. Entwurf, Bibliothek und gerade aktiver Tafelinhaltsstand bleiben getrennt (`LIB-11` bis `LIB-15`, `BLK-01`).

## 24.10.2026 · Erweiterter Formatierungseditor, vereinbartes Bedienkonzept

**Grundsatz: einfache Bedienung beibehalten.** Die globale Farbpalette färbt wie bisher den ganzen Lauftext bzw. bei zweizeiligen Blockseiten jeweils die entsprechende Zeile; die globale Schriftwahl je Text/Seite bleibt vorhanden. Die Individualisierung ist eine **freiwillige zusätzliche Ebene**, keine Pflichtbedienung.

- [ ] **EDT-15 · P1 · Rückgängig / Wiederholen.** Gut zugängliche Buttons **↶ / ↷** für normalen Editor **möglichst früh** entwickeln, nicht erst mit dem erweiterten Formatierungseditor. Text einfügen/löschen, mehrere gleichzeitig gelöschte Zeichen als eine Aktion, Symbol-Einfügung, Seitenreihenfolge und gelöschte Seiten sinnvoll als Aktionen bündeln. Später auch Farbauswahl, Fontwechsel, mehrfache Zeichenformate und Zurücksetzen. Rückgängig löst **keinen automatischen BLE-Upload** aus. Optional Undo-Historie über Reload erst nach Kosten-/Speicherbewertung, Entwurfssicherung ist davon unabhängig (`LIB-11`).
- [ ] **EDT-16 · P2 · Globale Änderung bei Einzelabweichungen.** Wenn bereits individuelle Farben existieren und die **globale Farbe** gewechselt wird, kompakt fragen: **„Einzelfarben beibehalten?“** mit **„Beibehalten“** / **„Alle angleichen“**. Entsprechendes nur für Schriftänderungen: **„Einzelschriften beibehalten?“**. **Nur die betroffene Eigenschaft** ändern, niemals Farben beim Schriftwechsel oder Schriften beim Farbwechsel stillschweigend zurücksetzen. Keine Nachfrage, wenn keine Einzelabweichung besteht. Sicherheitsorientierter Standard „Beibehalten“, Rückgängig möglich.
- [ ] **EDT-17 · P2 · 🎨 an der Farbreihe.** Unmittelbar neben den sieben bestehenden Farbtasten eine achte, farblose **🎨-Schaltfläche** zum Öffnen des gemeinsamen erweiterten Formatierungseditors einplanen. Auf schmalen Smartphones die einzeilige, gut tappbare Farbpalette prüfen. Standard-Farben und globale Schriftwahl bleiben trotz Individualisierung nutzbar.
- [ ] **EDT-18 · P2 · Auswahlarten im erweiterten Editor.** Zwei Ansichten derselben zugrunde liegenden Zeichen-/Formateigenschaften: **(1) „Text markieren“** für zusammenhängende Wörter/Abschnitte, **(2) „Einzeln“** als Kachelansicht nach Vorbild der Original-App für **jedes sichtbare Zeichen, Leerzeichen und Symbol**. Beliebig viele Kacheln antippen und wieder abwählen, danach einmalig Farbe oder Schrift anwenden. **Alles auswählen**, **Auswahl aufheben**, **Auswahl umkehren**; Leerzeichen beispielsweise mit `·` als Platzhalter nur im Editor anzeigen. Zwischen den Modi ohne Formatverlust wechseln, Auswahl auf iPhone/Safari wirklich testen.
- [ ] **EDT-19 · P2 · Vererbung und gezielter Reset.** Datenmodell mit **globaler Standardfarbe/-schrift** sowie optionaler **individueller Abweichung pro Zeichen/Abschnitt**. Ohne Abweichung folgt ein Zeichen einer späteren globalen Änderung; mit Abweichung behält es diese bei „Beibehalten“. **„Standard übernehmen“** entfernt gezielt nur die ausgewählte Einzelabweichung; „Weiß“ ist hingegen eine echte Farbe und **nicht** automatisch „zur Standardfarbe zurückkehren“. Farben/Fonts unabhängig zurücksetzbar. Inline-Symbole und mehrteilige Unicode-Emoji-Sequenzen atomar behandeln. Persistenz und Vorschau müssen Formatierung verlustfrei übernehmen.
- [ ] **FNT-17 · P2 · Gemischte Schriften in einer Textzeile.** Unter derselben globalen Schriftart gezielte Schriftwechsel einzelner Buchstaben/Wörter ermöglichen, insbesondere **Original Normal**, **Original Bold**, **Engschrift** und als Option **Mini - Einzeilig**. Die Standardauswahl bleibt erhalten. Fontwechsel im erweiterten 🎨-Editor auf aktuelle Markierung/Kachelauswahl anwenden. Variable Glyphenbreite, Grundlinie, horizontale/vertikale Position, Vorschub und Bildschirmvorschau gegenüber der realen Tafel prüfen. Nicht mit einem komplett freien zweizeiligen Layout gleichsetzen.
- [ ] **FNT-18 · P3 · Freie hybride Layouts nur merken.** **Einzeilige und zweizeilige Mini-Texte, große Schriftbereiche und 16×16-Symbole** innerhalb **einer** Komposition nach Nutzerskizze mischen (z. B. groß links, zwei kleine Zeilen daneben, Symbol rechts). Noch **keine Layout-Architektur** festlegen und keinesfalls unmittelbar umsetzen. Zunächst auf die angekündigte Nutzerzeichnung warten; dann Positionen, Überlagerungen und Bedienkonzept gemeinsam definieren.

### Symbole im Formatierungseditor

- [ ] **SYM-06 · P1 · Unveränderliche Originalfarben respektieren.** Rekonstruierte mehrfarbige Originalsymbole behalten standardmäßig exakt ihre eigene Pixelgestalt samt Farben. Globale Farbänderung, Schriftwechsel und Zurücksetzen wirken auf diese Symbole nicht. In einer gemischten Markierung werden formatierbare Buchstaben bearbeitet und unveränderliche Symbole **stillschweigend übersprungen**. Herz mit sieben ausdrücklich unterstützten Varianten bleibt ein Sonderfall.
- [ ] **SYM-07 · P2 · Symbolkacheln dürfen markiert werden, ohne Effekt.** Auch festfarbige Symbole bleiben in der Einzelzeichenansicht antippbar, einzeln auswähl- und abwählbar; Farb- oder Schriftänderungsversuch verändert sie nicht. **Kein Warnhinweis, kein Toast, kein Pop-up** für diese erwartete Inaktivität. Das gilt ausdrücklich auch bei ausschließlich markierten unveränderlichen Symbolen. Referenz: herkömmliche Emojis lassen sich auch nicht beliebig wie Schrift umfärben.
- [ ] **SYM-08 · P3 · Selektive alternative Symbolfarben.** Später einzelne Originalsymbole auf sinnvolle neue Farbvarianten prüfen, z. B. **Warndreieck mit Ausrufezeichen**. Keine pauschale Umfärbung sämtlicher Originalmotive. Jede mögliche Variante separat entscheiden, identische Originalversion beibehalten. **Jetzt nicht entwickeln**.

## 24.10.2026 · Symbolarchiv ergänzen und Pixelabstände nacharbeiten

**Quelle:** neue Screenshots aus der Original-App mit **Lippen/Kussmund**, **Blume/Rose** und **Biene**, außerdem ein Vergleich `U U [Blume] U`, eine Totem-/Festivalaufnahme als Bananenreferenz und ein Foto einer **gelben Gummi-Quietscheente mit rotem Schnabel**. Diese drei neu entdeckten Hersteller-Motive fehlen in der bestehenden Sammlung von **24 rekonstruierten Originalsymbolen**. Bestehende Werte nicht stillschweigend als endgültiges, vollständiges Herstellerarchiv bezeichnen.

- [ ] **SYM-09 · P1 · Kussmund/Lippenmotiv originalgetreu aufnehmen.** Das Motiv ist auf dem neuen Original-App-Screenshot sichtbar (rote Lippen mit hellen Akzenten), im bisherigen 24er-Symbolarchiv noch nicht vorhanden. Original-16×16-Pixelmuster aus dem Screenshot rekonstruieren, als **Originalsymbol** einordnen und mit der Original-App vergleichen. Nicht durch frei gezeichnetes Unicode-Emoji ersetzen.
- [ ] **SYM-10 · P1 · Blume/Rose ergänzen.** Mehrfarbiges Blütenmotiv mit grünem Stiel und Blättern aus der neuen Original-App-Referenz pixelgetreu rekonstruieren. Ursprüngliche Symbolfarben bewahren.
- [ ] **SYM-11 · P1 · Biene ergänzen.** Gelb-blau-weißes Original-Pixelmotiv aus Screenshot rekonstruieren; gestalterisch der vorhandenen Hersteller-Symbolreihe zuordnen, Originalfarben bewahren.
- [ ] **SYM-12 · P2 · Stern vertikal zentrieren.** Bei der bisherigen Originalstern-Darstellung beginnt das Motiv oben an der Matrixkante, darunter bleiben mehrere Pixelreihen frei. **Nur die vertikale Platzierung/Offset korrigieren**, nicht das Pixelmotiv selbst verändern; gegen Vorschau und reales Board kontrollieren.
- [ ] **SYM-13 · P2 · Blume: symmetrischer Abstand.** Beim Test `U U [Blume] U` ist links der Zwischenraum akzeptabel, rechts der Nachlauf deutlich zu groß. **Effektive Zeichenbreite/Advance bzw. Randabstand** so justieren, dass links/rechts vergleichbar sind; die Blume selbst nicht umzeichnen. Andere Symbole mit unterschiedlichen Leuchtbreiten später separat vermessen (`SYM-02`).
- [ ] **SYM-14 · P1 · Ravestick-Banane neu entwerfen.** Erste eigene Banane technisch funktionsfähig, aber laut Praxistest nicht als Frucht erkennbar, wirkt eher wie **Mario-Kart-Bananenschale**. 16×16-Silhouette anhand **Totem-Banane** gestalten, charakteristische Krümmung/Ausrichtung und bereits gewünschte vertikale Spiegelung berücksichtigen. Nicht einfach das Standard-Emoji spiegeln. Erst Browser-, dann reale LED-Beurteilung; Eigensymbol, nicht Hersteller-Rekonstruktion.
- [ ] **SYM-15 · P2 · Quietscheente als Eigensymbol.** Auf Basis des Nutzerfotos ein kleines **gelbes Gummientchen** mit rot/orangem Schnabel und schwarzem Auge als eigenes 16×16-Pixelmotiv zeichnen, im gestalterischen Stil des bisherigen Hahn-/Hühnchenmotivs der Original-App. Klar als Quietscheente erkennbar, aber als **RavestickBoard-Eigensymbol** kennzeichnen; kein angebliches Herstelleroriginal.
- [ ] **SYM-16 · P2 · Gesamte Symbolbibliothek nachtesten.** Für Originale und neue Eigensymbole **linker/rechter Abstand**, individuelle sichtbare Breite, Darstellung direkt neben Buchstaben, Cursor-Einfügung und mögliche 16px-Kollision bei Mini zweizeilig prüfen. Abstandsmessungen an Original-App-Screenshots sind ein gesonderter, **nicht dringender** Abgleich; nur belegte Korrekturen übernehmen.

## 24.10.2026 · Neue Mini- und Zweizeilen-Wünsche (jüngste Ergänzung)

- [ ] **FNT-19 · P1 · Mini-Leerzeichen verschmälern.** In **Mini - Einzeilig** und **Mini - Zweizeilig** ist der sichtbare horizontale Abstand für ein normales Leerzeichen derzeit **zu groß**. Tatsächlichen Space-Advance prüfen, verringern und ein einheitliches gut lesbares Schriftbild an Beispielen mit mehreren Wörtern und Symbolen testen. Schriftglyphen nicht unnötig verändern; Browser-Vorschau und übertragenes Pixelbild müssen übereinstimmen.
- [ ] **EDT-24 · P1 · Horizontale Ausrichtung pro Mini-Zeile.** In der **zweizeiligen Blockanzeige** die horizontale Ausrichtung für **obere und untere Zeile separat** auf **Links / Zentriert / Rechts** setzen, genauso unabhängig wie die bereits getrennten Zeilenfarben. Beispiel: obere Zeile links, untere rechts; beide auch unabhängig zentrierbar. **Aktuell gilt eine gemeinsame Ausrichtung pro Seite**. Datenmodell und NVS-Altseiten auf rückwärtskompatible Standardwerte prüfen. Nicht mit der späteren frei platzierbaren Mischung aus großen/kleinen Blöcken (`FNT-18`) verwechseln.

## 24.10.2026 · Dokumentations- und Repo-Pflege (noch nicht ausführen)

- [ ] **DOC-14 · P1 · Dokumentation am realen v0.3.0-Teststand ausrichten.** README und Themenspeicher dokumentieren das **tatsächliche Nutzerergebnis** vom 24.10.2026, nicht mehr pauschal „v0.3.0 ungetestet“. Neue Ideen eindeutig als offen markieren. **Diese Dokumentationspflege ist kein Firmware-Release v0.4.0.** Bei der nächsten echten Firmwareänderung erst Changelog um bestätigte neue Tests ergänzen.
- [ ] **DOC-15 · P2 · Versionsgebundene Dateien aufräumen.** Prüfen, ob `TEST_HINWEIS.txt` und `UPDATE_ANLEITUNG.txt` (aus v0.1.1), `RELEASE_NOTES.txt` (alte Versionsnotizen) und `ENTWICKLUNGSSTAND_v0.3.0.md` im Stammverzeichnis noch sinnvoll sind; nach Inhalts-/Linkprüfung archivieren oder entfernen. `docs/preview_v0.3.0-dev.png` und `docs/ravestick_banana_v0.3.0-dev.png` bei Bedarf als historische Entwürfe archivieren, insbesondere nicht blind löschen, wenn Dokumente darauf verlinken. **Keine automatische Löschung; die bisherigen Firmwareordner, `LICENSE`, Quellen- und Prüfunterlagen sowie das Totem-Foto behalten.** Entscheidung/Commit erfolgt ausschließlich im lokalen Repo durch den Nutzer.
- [ ] **DOC-16 · P1 · Lieferform künftiger Versionen.** Standardlieferung: **fertiges ZIP** mit bereits ausgearbeiteten Firmware-/Dokudateien in korrekter Repositorystruktur, direkt über bestehendes lokales Repo entpackbar. **Kein vorgeschalteter Python- oder Batch-Generator erforderlich.** Keine GitHub-Commits/Pushes durch die Assistenz; der Nutzer prüft, flasht und veröffentlicht selbst über GitHub Desktop. Bei reinen Dokumentationsupdates nur geänderte Dateien bereitstellen.

**Versionierungsentscheidung:** Nach dem v0.3.0-Test wurden zahlreiche Ideen für einen späteren v0.4.0-Schritt besprochen. **Heute kein neuer Firmware-Build.** Die Version v0.3.0 bleibt der aktuelle hardwaregetestete Entwicklungsstand mit dokumentierten Einschränkungen; künftige v0.4.0-Funktionen gelten erst nach Erstellung und realem Test als umgesetzt.


## Testbefunde 25.10.2026 und v0.3.2

- **v0.3.1 real getestet:** Arduino kompiliert; NVS-Seiten erhalten; 40, 60, 80 Zeichen sowie umfangreiche Symbolketten übertragen; Mini-Leerzeichen verbessert; Live-BLE, Vorschau und Blockseiten funktionieren.
- **Offen/Fehler v0.3.1:** iOS-Symbolpalette zu schmal und unbedienbar; Dialog schließt nach jedem Symbol und Tastatur springt auf; Lauftext-Schriftwahl fehlt; Demo-Feldwerte „Grüße“ und „UMARMUNG“ unerwünscht; Vorschau langfristig verkleinern; Undo/Redo erst für künftige individuelle Formatierung wirklich wertvoll; automatischer Blockseiten-Player stoppt nach BLE-Verbindungsfehler.
- **v0.3.2 Kandidat:** mobile Palette mit Mehrfacheinfügung, fünf einheitliche Fontnamen, Mini Zweizeilig auch im Lauftext, keine neuen Demotexte, experimentelle 600-Zeichen-Grenze mit rotem/fettem Zähler bei Überschreitung. Technisch kritisch: BLE-Header weiterhin nur 80 Format-Slots; längere Übertragung ohne bestätigte Protokollunterstützung.
- **Repository-Pflege:** ältere Dokumente vor Löschen auf historische Informationen prüfen; `RELEASE_NOTES.txt`, `TEST_HINWEIS.txt`, `UPDATE_ANLEITUNG.txt`, `ENTWICKLUNGSSTAND_v0.3.0.md` und `docs/VEROEFFENTLICHUNGS_AUDIT_2026-10-23.md` sind Archivkandidaten, nicht automatisch löschen.
