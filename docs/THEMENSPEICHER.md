# Themenspeicher und Aufgabenregister

**Stand:** 22.10.2026, v0.1.1 als Testkandidat ergänzt. Aus Projektchats und realen Tests konsolidiert.  
**Status:** `[x]` geprüft/real umgesetzt, `[ ]` offen. Bei **teilweise** ist es nicht abhaktauglich.  
**Prioritäten:** P0 = Basis/Fehlerdiagnose, P1 = Kerneditor, P2 = Bibliothek/Spezialprogramme, P3 = spätere Option.

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
- [ ] **SYS-09 · P1** Nach fremder Verbindung durch Original-App automatisch wieder handlungsfähig werden; Timeout und Retry nachvollziehbar testen.
- [ ] **SYS-10 · P1** BLE-/WLAN-Koexistenz, Transfergrößen, Langzeitstabilität messen.
- [ ] **SYS-11 · P2** Konfiguration und nutzbare Speicherkapazität messen (Programmpartition, Dateisystem, freier RAM).
- [ ] **SYS-12 · P3** Mehr als eine LED-Tafel verwalten (Idee aus der Diskussion über weitere Boards; Nutzen/Machbarkeit prüfen).

## B · Diagnose, Troubleshooting, Wartung

- [ ] **DIA-01 · P0** Diagnosebereich direkt im Webinterface. **Code in v0.1.1 vorhanden; Gerätetest ausstehend.**
- [ ] **DIA-02 · P0** Diagnose-TXT-Download mit Hardware, Firmwarestand, Resetgrund, Uptime und Zeit seit Start. **Code in v0.1.1 vorhanden; Gerätetest ausstehend.**
- [ ] **DIA-03 · P0** Heap frei, Minimum Heap, größter freier zusammenhängender Block. **Code in v0.1.1 vorhanden; Gerätetest ausstehend.**
- [ ] **DIA-04 · P0** BLE-Verbindungs-, Retry-, Disconnect-, Timeout- und Transfer-Ereignisse; BLE-Notification separat von Write-Erfolg. **Code in v0.1.1 vorhanden; Gerätetest ausstehend.**
- [ ] **DIA-05 · P0** HTTP-Aufträge, Queue-Status und Fehlerursachen für Webinterface-Fehlersuche. **Code in v0.1.1 vorhanden; Gerätetest ausstehend.**
- [ ] **DIA-06 · P0** Begrenzter RAM-Ringpuffer für letzte Ereignisse und separate Fehlerzähler; keine dauernden Flash-Schreibzugriffe. **Code in v0.1.1 vorhanden; Gerätetest ausstehend.**
- [ ] **DIA-07 · P1** Dateisystem gesamt/belegt/frei; Speichergrenzen prüfen.
- [ ] **DIA-08 · P1** Letzten gravierenden Fehler/Resetmarker optional stromausfallsicher, nach Flashverschleiß-Abwägung.
- [ ] **DIA-09 · P1** Protokollierung von Erfolgen *und* Fehlschlägen, Transferdauer, Datenmenge, Paketanzahl. **Code in v0.1.1 vorhanden; Gerätetest ausstehend.**

## C · Vorschau als LED-Dot-Matrix

- [ ] **PRE-01 · P0** Fester **64×16-Ausschnitt** statt Textbreite mitzuskalieren.
- [ ] **PRE-02 · P0** Einzeln sichtbare **runde LED-Punkte** inklusive dunkler/unbeleuchteter Punkte und Punktabstand (nicht nur glatte Pixelblöcke).
- [ ] **PRE-03 · P0** Lauftext im festen Fenster animieren; keine automatische Verkleinerung.
- [ ] **PRE-04 · P0** Playersteuerung: Play/Pause, Neustart und **Fast Forward** für lange Texte (z. B. 1×, 2×, 4×, 8×; finale Stufen testen).
- [ ] **PRE-05 · P1** Optional Vorschau-Zeitleiste/Scrubbing oder Sprung zu Folgeseite, damit lange Programme schnell prüfbar sind.
- [ ] **PRE-06 · P1** Pixelvorschau für mehrere Seiten, zeilenweise Darstellung und gemischte Schriftarten/Farben.
- [ ] **PRE-07 · P1** Vorschaubewegung möglichst realitätsgetreu: Laufrichtung und Geschwindigkeitsabgleich mit echter Tafel.
- [ ] **PRE-08 · P1** Statische Seiten mit Positionierung/Überlaufmarkierung; kein ungefragter Auto-Umbruch/Schrift-Shrink.

## D · Texteditor, Modi und Seiten

- [ ] **EDT-01 · P0** **Explizit** auswählen: Lauftext vs. statische Seiten; unabhängig von Textlänge.
- [ ] **EDT-02 · P1** Seiten separat erstellen, gestalten, sortieren, duplizieren, löschen.
- [ ] **EDT-03 · P1** Seiten individuell konfigurieren (Text, Font/Größe, Farbe, Position, Dauer).
- [ ] **EDT-04 · P1** Manueller Seitenwechsel und automatischer Seitenwechsel mit individuell einstellbarer Dauer.
- [ ] **EDT-05 · P1** Endlos/Einmal/Wiederholung und Pausen für Seitenfolge/Lauftext.
- [ ] **EDT-06 · P1** Auswahl eines Zeichens, Worts oder Abschnitts, Farbwahl **je Zeichen** möglich.
- [ ] **EDT-07 · P1** Auswahl eines Zeichens, Worts oder Abschnitts, **Schriftart je Zeichen** möglich.
- [ ] **EDT-08 · P1** Einstellbare Positionierung (links, mittig, rechts; bei statischer Seite präzise Koordinaten später prüfen).
- [ ] **EDT-09 · P1** Für Seiten keine automatische Textkürzung, Umbruch oder Größenänderung; Überlauf klar anzeigen.
- [ ] **EDT-10 · P1** Geschwindigkeit per Schieberegler beim Lauftext, zusätzlich tatsächliche BLE-Speed-Werte kalibrieren.
- [ ] **EDT-11 · P1** Auf 40 Zeichen begrenzten Prototyp erweitern; realistische technische Grenzen messen.
- [ ] **EDT-12 · P2** Freie Textblöcke oder Seiten in Programmen mischen, ohne die Inhalte zu duplizieren.
- [ ] **EDT-13 · P2** Sonderzeichen, Herz/Emojis und Kombination mit eigenen Grafiken.
- [ ] **EDT-14 · P3** Mehr Gestaltungsmöglichkeiten (z. B. Farbwechsel/Verläufe) nur nach Prüfung der tatsächlichen 7-Farben-Hardware.

## E · Schriftbibliothek

- [x] **FNT-01 · P0** Engschrift als 5×7-Matrix (vertikal verdoppelt) als zusätzlichen Font etabliert und real getestet; **nur Großbuchstaben**.
- [ ] **FNT-02 · P1** Engschrift: **echte Kleinbuchstaben**, Umlaute und Zeichensatz vervollständigen.
- [ ] **FNT-03 · P1** **Normal** aus Referenzaufnahmen jedes Originalzeichens rekonstruieren (separater Originalanzeigen-Chat).
- [ ] **FNT-04 · P1** **Bold** aus eigenen Originalreferenzen rekonstruieren (separat von Normal).
- [ ] **FNT-05 · P1** Hersteller-Unregelmäßigkeiten sammeln und einzeln über Übernahme oder Korrektur entscheiden; Originalreferenz behalten.
- [ ] **FNT-06 · P1** **Mini** als eigener kleiner Font für zwei Zeilen und optional vertikal zentrierte Einzelzeile entwickeln; keine Originalvorlage behaupten.
- [ ] **FNT-07 · P1** Zweizeilige Darstellung im 16-Pixel-Raster testen, **keinen Abstand erzwingen**.
- [ ] **FNT-08 · P1** Zeichensätze an zentraler Stelle definieren, um C++/Browser-Vorschau konsistent zu halten.
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
- [ ] **UI-02 · P0** 7 Farbtasten ohne unerwünschte zweite Reihe darstellen. **Code in v0.1.1 vorhanden; Gerätetest ausstehend.**
- [ ] **UI-03 · P0** `100 %` neben Helligkeitsregler ohne Zeilenumbruch. **Code in v0.1.1 vorhanden; Gerätetest ausstehend.**
- [ ] **UI-04 · P0** Buttons/Status außerhalb der überlagernden mobilen Browserleiste positionieren; Safe-Area testen. **Code in v0.1.1 vorhanden; Gerätetest ausstehend.**
- [ ] **UI-05 · P1** Navigationsstruktur Startseite, Player, Bibliothek, Programme, Editor, Banane/Licht, System/Diagnose.
- [ ] **UI-06 · P1** Bewusst an nostalgische CoolLED1248-App erinnern, deren Einschränkungen aber nicht übernehmen; **nicht** das SmallTV-Grau/Gelb.
- [ ] **UI-07 · P2** Touchoptimierung für markierte Textbereiche, Fonts, Farben, Vorlagen und Seitenübersicht.
- [ ] **UI-08 · P3** Optionale iOS-App, Apple-Watch-/Widget-Steuerung nur nach Nutzenabwägung; Browser ist die Basis.

## I · Qualität, Dokumentation, öffentlicher Release

- [x] **DOC-01 · P0** Erste Firmware v0.1.0 unter eigenem Versionsnamen gesichert.
- [x] **DOC-02 · P0** Projekt-Themenspeicher, Roadmap, Änderungs- und Entscheidungslog angelegt (22.10.2026).
- [x] **DOC-03 · P0** Git eingerichtet, initiales Repository privat unter `du13500/RavestickBoard-for-CoolLED1248` auf GitHub gesichert (22.10.2026).
- [ ] **DOC-04 · P1** README/Installationsanleitung und Changelog bei jeder Version aktualisieren.
- [ ] **DOC-05 · P1** Einheitliche Fehlermeldungen, Verhalten nach Stromausfall, Boot- und Dauerbetriebstests.
- [ ] **DOC-06 · P2** Originalanzeigen separat referenzieren, Normal/Bold und Hersteller-Eigenheiten dokumentieren.
- [ ] **DOC-07 · P2** Fremdcode- und Lizenzherkunft der BLE-Protokollimplementierung prüfen.
- [ ] **DOC-08 · P3** GitHub-Veröffentlichung nach Lizenz-/Datenschutz-/Sicherheitssichtung entscheiden.
- [ ] **DOC-09 · P3** Mehrgerätefähigkeit und Verteilung an andere CoolLEDX-Besitzer testen, bevor allgemeine Kompatibilität behauptet wird.

## Themen, für die noch Entscheidungen/Versuche fehlen

- **Zeitquelle:** Countdown/Stoppuhr brauchen nur lokale Zeitmessung, geplante Uhrzeit-/Tageszeit-Programme eventuell eine externe Uhr oder Synchronisation vom Smartphone.
- **Bleiben Programme auf der Tafel?** Der letzte Anzeigeinhalt bleibt bei Stromverlust erhalten; mehrstufige ESP32-Programme sind noch nicht als tafelintern unterstützt nachgewiesen.
- **Helligkeit/Farbe vs. Vorschau:** Browserfarben entsprechen den 7 LED-Farben nur näherungsweise, reale Farberscheinung ist gesondert zu prüfen.
- **Schrift-/Pixelabmessungen:** Das Gerät meldet 64×16, trotz Produktname „CoolLED1248“; reale Punktgeometrie in Vorschau nachbilden.
- **Speicherbudget:** Große Animationen und viele Vorlagen benötigen Limits, Kompression und Diagnosewerte.
- **Reset und Logging:** Debugging ohne unkontrolliertes Flash-Schreiben, wichtige letzte Fehler ggf. gezielt persistent.

## Pflegebeispiel

Bei Umsetzung der Vorschau: `PRE-01` abhaken, im `CHANGELOG` unter `v0.1.2` die Änderung und einen echten iPhone-Test vermerken. Bei einer Fehlfunktion neue ID oder Untereintrag aufnehmen und nicht stillschweigend vergessen.
