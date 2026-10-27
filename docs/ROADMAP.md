## Nächster Schritt – RAM-Diagnose (27.10.2026)

- **v0.4.0.2-dev:** begrenzter RAMTRACE mit Messpunkten für Rendern, BLE, große Übertragungen, HTTP, Freigabe; aktualisierte Pixelart aus `docs/assets/Pixel-Art.xlsx` inklusive Ziggi II. Hardware-/Compile-Test noch offen.
- **Danach:** Ursache des 324-B-Heap-Minimums einengen, zuerst C3 optimieren, erst bei Bedarf Controllerwechsel.
- **v0.4.1:** Persistente Bibliothek auf dem ESP32, möglichst flashbasiert und ohne vollständiges Laden in den Heap; NVS-Seitenschema getrennt und unverändert lassen.

---

## Priorität und bestätigte Reihenfolge (26.10.2026, nach v0.4.0-Hardwaretest)

- **v0.4.0.1-dev:** Navigation „Kurzwahl“ immer im Menü, kompakte Listen- und Regleransichten, dezente Statusmeldung, neue Pixelbilder aus der Excel-Sammlung. Das BLE-/NVS-Schema bleibt unverändert.
- **v0.4.1:** Persistente Textbibliothek; Auswahl, Löschen mit Referenzschutz, Drag-and-drop, Backups, sechs echte Kurzwahlbelegungen.
- **v0.4.2:** Programme als Referenzen auf Bibliothekstexte. Änderungen an Texten müssen alle referenzierenden Programme aktualisieren; Datenbudget/Übertragungen minimieren.
- **v0.4.3 (optional):** Einzelne Zeichen und Textabschnitte farbig gestalten.
- **Langfristig:** native autonome Programme (`0x04`) weiter eigenständig untersuchen; Countdown, Stoppuhr, Punktestand, BLE-Gerätesuche.

Alte Roadmap-Abschnitte mit acht statt sechs Schnellzugriffen beschreiben frühere Konzeptstände. Für die aktuelle WebUX gelten **sechs**.

---

## Aktuelle Prioritäten ab v0.4.0-dev · 26.10.2026

Wir folgen dem gemeinsam entwickelten WebUX-Konzept: **v0.4.0** Navigation und vorbereitete sieben Ansichten; **v0.4.1** Bibliothek mit referenzierten Texten, Drag-and-drop, sechs Kurzwahlbelegungen; **v0.4.2** Programme als Verkettung dieser Texte mit BLE-Größen-/Kompatibilitätsprüfung; **v0.4.3 (optional)** Textabschnitte mit individuellen Farben. Erst danach priorisieren wir weitere Gerätefunktionen und die autonome Animation ohne sichtbare Uploads zwischen Seiten. Tempo/Helligkeit sind global; Modus pro Text. Mehrfarbige Symbol-Buttons verwenden tatsächliche 16×16-Sprites. Die neue Excel-Sammlung wird als separater Import behandelt.

---

## Ergänzung v0.3.5

### v0.3.5 – 2026-10-25 (neuer Testkandidat)

LED Ein/Aus über 0x09 ohne Bildupload; Tempo 60–100/Standard 80; acht endgültig benannte Modi; vier Screenshot-rekonstruierte Hersteller-Motive, sechs eigene Motive; Blume-Abstand korrigiert. Hardwaretest offen. Zeitberechnung für horizontale Laufschrift später; Spezialeffekte werden ausschließlich manuell erzwungen.

---

# 🍌 RavestickBoard · Roadmap

**Fortgeschrieben: 25. Oktober 2026 · hardwaregetesteter Stand: v0.3.3 · neuer Testkandidat: v0.3.4.**  
**Projekt:** ESP32-C3-Webcontroller für CoolLED1248/CoolLEDX, 64 × 16 Pixel, sieben LED-Farben.  
**Mission:** Hugs and Love. ❤️

Diese Roadmap beschreibt **wo wir stehen, was als Nächstes sinnvoll ist und welche größeren Möglichkeiten wir untersuchen**. Sie ist ein Entwicklungsplan, keine Behauptung bereits vorhandener Fähigkeiten und keine feste Termin- oder Versionszusage.

- **Detailliertes Ideen- und Aufgabenregister:** [`THEMENSPEICHER.md`](THEMENSPEICHER.md), einschließlich eindeutiger Themen-IDs.
- **Tatsächlich umgesetzte Versionen und Fehlerkorrekturen:** [`../CHANGELOG.md`](../CHANGELOG.md).
- **Technische Entscheidungen und offene Alternativen:** [`ENTSCHEIDUNGEN.md`](ENTSCHEIDUNGEN.md).
- **Quellen und Fremdlizenzen:** [`QUELLEN_UND_LIZENZEN.md`](QUELLEN_UND_LIZENZEN.md).

**Statusregel:** ✅ = auf unserer Hardware bestätigt · 🟡 = implementiert, aber nur eingeschränkt bestätigt oder mit bekanntem Problem · 🔬 = Forschung / auf unserer Hardware noch unbestätigt · ⬜ = geplant. Checklisten werden erst nach nachvollziehbaren Tests und Aktualisierung des Changelogs abgehakt.

---

## v0.3.4 – Testphase der erweiterten Modusauswahl (25.10.2026)

- ✅ **v0.3.3 hardwarebestätigt:** 650 Zeichen/30.689 B, 301 gemischte Zeichen/30.695 B, zwei Mini-Lauftextfarben, mobile Bedienung, sieben NVS-Seiten.
- 🔬 **v0.3.4 neu:** Dropdown mit `0x01`–`0x08` als Parametern zum BLE-Steuerbefehl `0x06`. Die sechs zusätzlichen Werte (`0x03`–`0x08`) sind noch nicht auf unserer Hardware bestätigt, daher keine Effektbezeichnungen als gesicherte Tatsachen behandeln.
- 🔬 **Nachrangig:** Breitenlimit und Bildheader oberhalb 30 KiB, echte autonome Animationen als **anderer** Protokollbefehl `0x04`, Bibliothek, Lauftextverkettung, neue Symbole, exportierbare NVS-Seiten und robustere Lang-BLE-Übertragung.
- 🔗 [Entwicklungsstand v0.3.4](ENTWICKLUNGSSTAND_v0.3.4_2026-10-25.md) und [Modus-Testplan](TESTPLAN.md).

## Aktueller Stand / v0.3.3 (25.10.2026)

**In v0.3.3 implementiert und am 25.10.2026 hardwaregetestet:** dynamisches datenbasiertes Lauftextlimit von vorläufig 30 KiB anstelle des 600-Zeichenlimits; laufende Breiten-/Byteberechnung mit den Originalglyphen; unabhängige Farben der beiden synchron scrollenden Mini-Zeilen. **Nachfolgender neuer Testkandidat: v0.3.4 (Modus-Dropdown).**

**Danach geplant:** (1) empirisches Limit der LED-Tafel genauer bestimmen und Datenbudget anpassen; (2) Bibliothek für separat editierbare Texte mit gespeicherten Schriften, Farben und Symbolen; (3) einfacher Programmeditor zum **einmaligen Übertragen verketteter Lauftexte** (eine Prozentanzeige für das Gesamtprogramm, sofern die Daten passen); (4) echte gemischte Programme aus Lauf- und Standbildern erst nach Protokollrecherche, da unterschiedliche Standzeiten nicht in einem einfachen Laufbild enthalten sind. Weitere Ideen bleiben im bisherigen Themenspeicher.

## Aktueller Nachtrag: v0.3.2 Korrektur 2 und nächste Architektur (25.10.2026)

**Hardwarebestätigt:** WLAN/RAM-Startreparatur; NVS-Persistenz; mobile Symbolpalette; fünf Lauftextschriften; Mini Zweizeilig synchron; 600 normale Zeichen übertragen und dargestellt. Grenzfälle: 300 gemischte Symbole funktionieren, 320 gemischte Symbole nicht; 320 Blitze funktionieren, 320 Warnsymbole nicht. `JOB OK` beweist bislang nur einen vollständig quittierten Transfer, keine sichtbare Bildübernahme.

**Als Nächstes erforschen, nicht vorschnell implementieren:**

- [ ] Tafelseitige Bildbreiten-/Payloadgrenze durch gezielte Wiederholungstests isolieren; 300/301/302 `⚠️` vorgeschlagen (noch nicht durchgeführt).
- [ ] Browser- und Firmwarelimit vom willkürlichen Zeichenzähler auf **live berechnetes Pixel-/Datenbudget** umstellen; Browser- und Controllerberechnung müssen exakt übereinstimmen. Den aktuell größten zusammenhängenden RAM-Block separat beachten.
- [ ] Lange reine Texte über 600 experimentell ermöglichen, **erst nach kontrolliertem Umbau** von Controller-Arrays, HTTP-Prüfung und bislang 80 Headerformatplätzen.
- [ ] Mini-Zweizeilen-Lauftext: individuelle Farben pro Zeile ergänzen.
- [ ] Später Bibliothek mit einzeln bearbeitbaren Lauftexten sowie **Lauftext-Programmeditor**: mehrere Inhalte inklusive Abständen zu einem Pixelstreifen zusammenrendern und einmal übertragen, um die Prozentanzeige zwischen Nachrichten zu vermeiden.
- [ ] **Echte** Blockseitenprogramme mit Stopps, wechselnden Modi und Anzeigedauer bleiben getrennte, unbestätigte Protokollforschung.

**Ausführliche Messdaten und Statusabgrenzung:** [`ENTWICKLUNGSSTAND_v0.3.2_2026-10-25.md`](ENTWICKLUNGSSTAND_v0.3.2_2026-10-25.md). Ältere Roadmap-Abschnitte dokumentieren den ursprünglichen Planungshorizont und sind nicht automatisch aktuelle To-do-Listen.

---

## 1. Wo wir heute stehen

| Stand | Hardwarestatus | Ergebnis |
| --- | --- | --- |
| **BLE-Bring-up / FirstText (22.10.2026)** | ✅ | ESP32-C3 findet die Tafel; Helligkeitsbefehl und eigener mehrfarbiger Text erfolgreich übertragen. |
| **v0.1.0** | ✅ | Eigenes WLAN, iPhone-Weboberfläche, sieben Farben und Textupload funktionieren. |
| **v0.1.1** | ✅ | Diagnose und BLE-Statistiken auf der Tafel/im Webinterface praktisch erprobt. |
| **v0.1.2** | 🟡 | Deutsche Zeichen, getrennte Standbild-/Lauftextmodi, Punktvorschau und weitere Steuerung entwickelt; auffällige Glyphen und UX-Themen gesammelt. |
| **v0.2.0, Korrektur 1** | ✅ mit Einschränkungen | ESP32- und iPhone-Test erfolgreich: erster Blockeditor, zwei verschiedenfarbige Mini-Zeilen, Seitenverwaltung, individuelle Dauer, automatische BLE-Seitenfolge und Speicherung über Neustart. |

### Ergebnis des v0.2.0-Praxistests vom 23.10.2026

**13 von 14 Testpunkten mit „Ja“ beantwortet; die einzeilige Schrift blieb als separater Testpunkt ohne Ergebnis.** Diese Bewertung bestätigt die Grundfunktionen, nicht die endgültige Bedienqualität.

- [x] Start, WLAN-Weboberfläche und BLE-Übertragung auf echter Hardware.
- [x] Blockseiten anlegen, duplizieren, verschieben, löschen und farbig gestalten.
- [x] Die **zweizeilige, farblich getrennte Minischrift** ist auf der echten Tafel sehr gut lesbar; ihre bisherige Großbuchstabenform bleibt erhalten.
- [x] Automatischer Seitenwechsel und individuelle Anzeigezeiten funktionieren **technisch**.
- [x] Gespeicherte Blockseiten bleiben nach Neuladen des Editors und Neustart des ESP32 erhalten (NVS).
- [x] Wiederholte Übertragungen beziehungsweise eine Abfolge mehrerer Inhalte funktionieren.
- [ ] Einzeilige Mini-Darstellung ergänzen und separat testen.

**Wichtigste Einschränkung:** v0.2.0 überträgt **jede Seite erneut**. Dadurch erscheint zwischen den Seiten immer wieder die **Prozentanzeige der Tafel**. Das ist für ein *zusammenhängendes Blockprogramm* unerwünscht; der vorhandene Mechanismus eignet sich aber als spätere **Playlist-Steuerung zwischen eigenständigen Programmen**.

**Weiteres bestätigtes Verhalten:** Im Lauftexteditor geht der ungespeicherte Entwurf beim Neuladen verloren. **Gespeicherte Blockseiten bleiben erhalten.** Das ist kein allgemeiner NVS-Speicherfehler.

---

## 2. Unser Zielbild: Seite → Programm → Playlist

**Seite** → Ein individuell gestaltetes 64×16-Bild mit eigenen Texten, Schriftarten, Farben, Positionierungen und gegebenenfalls Anzeigedauer. Keine automatisch abgeschnittenen Buchstaben, kein ungefragter Seitenumbruch.

**Programm** → Eine oder mehrere Seiten, die **idealerweise in einer einzigen BLE-Übertragung** auf die Tafel gelangen und dort selbstständig abgespielt werden, **ohne Prozentanzeige zwischen den Seiten**. Ob und mit welchen Grenzen unsere konkrete Tafel dies unterstützt, muss getestet werden.

**Playlist** → Mehrere eigenständige Programme mit Reihenfolge, Wiedergabedauer und Rotation. **Zwischen Programmen darf der ESP32 erneut über BLE übertragen.** Hier können wir den bereits funktionierenden v0.2.0-Seitenplayer weiterverwenden.

Außerdem bleiben **Entwurf**, **gespeichertes Bibliotheksobjekt** und **aktuell auf der Tafel laufender Inhalt** ausdrücklich getrennte Zustände.

---

## 3. Nächste Etappe: v0.2.x stabilisieren und Schriften verfeinern

**Ziel:** Den ersten Editor zuverlässig und angenehm bedienbar machen, ohne das bereits funktionierende Zwei-Zeilen-Layout zu beschädigen.

### Schrift und Darstellung

- [ ] **Mini in zwei Zeilen erhalten**: das bestätigte Erscheinungsbild der Großbuchstaben nicht pauschal verändern (`FNT-10`).
- [ ] **Mini-Ausgabe immer in Versalien**: `a→A`, `ä→Ä`, `ö→Ö`, `ü→Ü`, `ß→ẞ` als ein Zeichen. Der eingegebene Originaltext bleibt im Editor erhalten (`FNT-11`).
- [ ] **„Mini, eine Zeile“** als zusätzliche Darstellungsoption; 7 Pixel hoch und vertikal zentriert (`FNT-12`).
- [ ] **ß und ẞ** technisch separat unterstützen, optisch gleich oder nahezu gleich. In der kleinen Schrift **den vom Nutzer blau markierten unteren Pixel entfernen** (`FNT-13`).
- [ ] **Engschrift-Unterlängen** bei `g`, `p`, `q`, `y` sinnvoll bis zur untersten Pixelreihe führen, ohne die gemeinsame x-Höhe zu verlassen; auch `j` und Satzzeichen prüfen (`FNT-14`).
- [ ] **Ä, Ö, Ü** mit den vorhandenen Original-App-Referenzen in Normal und Bold vergleichen und gezielt korrigieren; keine wahllose Font-Neugestaltung.
- [ ] Glyphenregression auf der echten 64×16-Tafel für Großbuchstaben, Kleinbuchstaben, Umlaute, ß/ẞ und Satzzeichen.

### Editor, Vorschau und Alltagstauglichkeit

- [ ] Überlänge erkennen: **keine Buchstaben am rechten Seitenrand zerteilen**, keine unfreiwillige nächste Seite und keine automatische Schriftverkleinerung; deutlich sichtbare Warnung (`BLK-05`, `EDT-09`).
- [ ] Vorschau weiter als **exakte 64×16-Punktmatrix** führen; ausgeschaltete LEDs, reale Farbkombinationen und Positionierung darstellen (`PRE-01` bis `PRE-08`).
- [ ] Vorschau-Tempo auf **1×, 2×, 4×** beschränken; kein 8×; Play/Pause und gezielte Seitennavigation erhalten.
- [ ] Zuletzt geöffneten Editor-Reiter sowie laufende Entwürfe nach Neuladen automatisch wiederherstellen; sparsame Speicherung statt Flash-Schreiben bei jedem Tastendruck (`LIB-11`).
- [ ] Statusmeldungen, mobile Bedienbarkeit, Safari-Safe-Area, Farbwahl und Diagnose auf dem iPhone regressionsprüfen (`UI-*`, `DIA-*`).
- [ ] BLE-Verbindungsfehler verständlich anzeigen, wenn die Original-App das Board gerade verwendet; Wiederherstellung ohne ESP-Neustart testen (`SYS-09`, `BLE-13`).

### Live-Steuerung ohne vollständigen Neu-Upload

- [ ] **Helligkeit `0x08`** direkt beim Schieben verändern; ohne Text-Neuübertragung.
- [ ] **Geschwindigkeit `0x07`** direkt beim Schieben verändern; Original-App-Skala um **80** gegen Rohwerte messen statt identische Werte anzunehmen.
- [ ] **Modus `0x06`** direkt umschalten: statisch/Block, links, rechts, aufwärts, abwärts, Schneefall, Laser und weitere **nachweislich unterstützte** Effekte.
- [ ] Kurze BLE-Verbindung während aktiver Bedienung halten, Zwischenwerte zusammenfassen, nach Inaktivität freigeben; Original-App-Koexistenz erhalten (`BLE-14` bis `BLE-16`).

**Abnahme:** Hauptfunktionen v0.2.0 bleiben stabil; kleine Schrift verwendet gut lesbare Großbuchstaben; Entwürfe überstehen einen Browser-Reload; Live-Kommandos verursachen keine neue Prozentanzeige.

---

## 4. Forschungs-Meilenstein: Was kann die LED-Tafel selbst?

**Ziel:** Den nächsten Architekturentscheid auf **eigenen Hardwaretests** aufbauen statt auf Annahmen über fremde CoolLEDX-Modelle.

### Blockprogramme und gespeicherte Animationen

- [ ] 🔬 **Animierte Bildfolgen über `0x04`** zunächst mit einem kleinen, ungefährlichen Testpaket auf unserer Tafel prüfen (`ANM-01`).
- [ ] 🔬 Zwei Seiten, z. B. **„SUCHE:“ → „JOINT / ZIGARETTE“**, **einmal** hochladen und anschließend autonom wiederholen lassen, **ohne zwischenzeitliche Prozentanzeige** (`ANM-03`).
- [ ] 🔬 Den **nativen Text-/Blockmodus `0x02`** dagegen testen: Gibt es einen einfacheren Weg für mehrere Seiten und Formatierungen? (`ANM-05`).
- [ ] 🔬 Eigene Hardwaregrenzen messen: Frameanzahl, Speicher, Paketgröße, ACKs, Transferdauer, Loop, Verhalten nach Stromverlust (`ANM-02`, `ANM-13`, `ANM-14`).
- [ ] 🔬 **Anzeigedauer pro Seite** klären. OpenSign beschreibt für seine Referenztafel **eine gemeinsame Bildhaltezeit**. Wiederholte Frames und deren Grenzen untersuchen, individuelle 5-/7-/10-Sekunden-Seiten **nicht ungeprüft versprechen** (`ANM-04`).
- [ ] 🔬 Originalmodi `0x06` und selbst entworfene, vorgerenderte `0x04`-Effekte klar unterscheiden (`ANM-11`).
- [ ] 🔬 Echtzeit-Streaming, dynamische Anzeige und mehrere gleichzeitig gespeicherte Programme **nicht voraussetzen** (`ANM-10`).

**Externe Erkenntnis, nicht unsere Testbestätigung:** OpenSign meldet auf seiner eigenen CoolLEDX-Referenztafel ca. **30 KiB / bis zu 80 Frames** und gespeicherte autonome Animation. Unsere Tafel kann andere Grenzen haben. 64×16 Pixel mit drei 1-Bit-Farbebenen benötigen **384 Byte je unkomprimiertem Vollbild**.

**Entscheidung nach den Tests:**

1. **Wenn möglich:** ein Programm als nativen Blocktext oder gespeicherte Animation mit einem Upload an die Tafel senden.
2. **Wenn nur eingeschränkt möglich:** Grenzen und angenäherte Anzeigedauern deutlich im Editor kennzeichnen.
3. **Falls nicht möglich:** Die Einschränkung ehrlich dokumentieren und Controller-gesteuerte Übergänge nur dort nutzen, wo die sichtbare Prozentanzeige akzeptabel ist. Den bestehenden Player behalten.

---

## 5. Programm- und Playlist-Architektur

**Abhängigkeit:** Ergebnisse der Block-/Animationsforschung aus Abschnitt 4.

- [ ] **Seitenobjekt** als eigenständig speicher- und bearbeitbares 64×16-Layout definieren (`BLK-01`, `EDT-02` bis `EDT-09`).
- [ ] **Programmobjekt** mit Seitenfolge, Einstellungen, Loop, Übertragungsweg und gegebenenfalls Zeitraster entwickeln (`BLK-01`, `BLK-03`, `BLK-06`).
- [ ] **Playlistobjekt** mit mehreren Programmen, Reihenfolge, Dauer, Wiederholung und optional Zufall erstellen (`LIB-04`, `LIB-05`).
- [ ] Den vorhandenen **v0.2.0-BLE-Seitenplayer** in die Playlist-Ebene übernehmen und nicht ersatzlos entfernen (`BLK-04`).
- [ ] Manuelle Navigation „vorheriges/nächstes Programm“, Start/Pause/Stop, kontrollierte Wiederaufnahme nach Fehler (`EDT-04`, `EDT-05`).
- [ ] Sofort-Nachrichten einschieben und danach auf Wunsch zur aktiven Playlist zurückkehren (`LIB-06`).
- [ ] Aktuelle Anzeige **beim ESP32-Neustart nicht ungefragt überschreiben**; automatischen Start nur nach ausdrücklicher Aktivierung (`LIB-09`).
- [ ] Übertragungsstatus, BLE-Retries und Fehler pro Programm sichtbar machen.

**Abnahme:** Unterschied zwischen einer Seite, einem autonomen Programm und einer ESP32-Playlist ist in der Bedienung klar. Neue Uploads finden, sofern technisch möglich, nur zwischen Programmen statt.

---

## 6. Bibliothek und Speicherung

**Ziel:** Texte, Seiten, Programme und später Animationen dauerhaft auf dem ESP32 verwalten, unabhängig vom verwendeten Telefon.

- [ ] **Entwurf / Bibliothek / aktives Programm** im Datenmodell und in der Benutzerführung trennen (`LIB-12`).
- [ ] ESP32-Flash mit **LittleFS** für viele benannte Programme untersuchen; NVS für kleine Konfigurationswerte und bestehende Seitenmigration berücksichtigen (`LIB-13`).
- [ ] Format mit stabilen IDs, Versionsnummern, Schriftarten, Farben, Positionen, Dauer, Seitenfolge und Metadaten festlegen (`LIB-14`).
- [ ] Bibliothek: **speichern, öffnen, duplizieren, umbenennen, löschen**, suchen und favorisieren (`LIB-01`, `LIB-02`).
- [ ] **Acht frei belegbare Schnellzugriffe** auf der Startseite (`LIB-03`).
- [ ] Ordner/Tags/Festival-Profile und wiederverwendbare Vorlagen (`LIB-02`, `LIB-07`).
- [ ] **Export/Import/Backup** einzelner Programme und der gesamten Bibliothek; der ESP32 darf niemals die einzige Kopie sein (`LIB-08`, `LIB-15`).
- [ ] Speicherbelegung, Grenzen, Datenintegrität und Flash-Verschleiß messen; Migrationen zwischen Firmwareversionen testen (`SYS-11`, `DIA-07`).
- [ ] ⬜ **microSD per SPI** nur bei tatsächlichem Bedarf, etwa für große Bildfolgen, vorsehen; 3,3 V, GPIO und Gehäuseaufbau separat prüfen (`LIB-16`).

**Abnahme:** Eine auf dem iPhone erstellte Vorlage lässt sich später am Laptop wieder öffnen; Reload und Stromausfall verlieren weder gespeicherte Programme noch bewusst gesicherte Entwürfe.

---

## 7. Erweiterter visueller Editor und Schriftarten

- [ ] **Originalschrift Normal** anhand gesammelter Referenzscreenshots rekonstruieren (`FNT-03`).
- [ ] **Originalschrift Bold** unabhängig von Normal rekonstruieren (`FNT-04`).
- [ ] Auffällige Hersteller-Glyphen **einzeln** zur Übernahme/Korrektur entscheiden; Originalvorlagen als Referenz erhalten (`FNT-05`).
- [ ] **Engschrift** und **Mini** als eigenständige Fonts weiterentwickeln (`FNT-02`, `FNT-06` bis `FNT-09`).
- [ ] **Farbe pro Zeichen, Wort oder Textabschnitt**, ohne Beschränkung auf eine Farbe je Zeile (`EDT-06`).
- [ ] **Schriftart pro Zeichen/Wort/Abschnitt**, Mischungen innerhalb derselben Seite (`EDT-07`).
- [ ] Mehrere Textobjekte, ein- oder zweizeilige Abschnitte, zentrierte Elemente und **freie Koordinaten** pro Seite (`EDT-08`, `EDT-12`).
- [ ] Sonderzeichen, Herzen, Symbole, Emojis soweit als Pixelgrafik darstellbar (`EDT-13`).
- [ ] Zeichenbasierte Überlaufwarnungen und optionale Hilfen zur Verteilung, **keine automatische Umformatierung ohne Benutzerwunsch** (`BLK-05`).
- [ ] Touchoptimierter Seiteneditor für iPhone und größere Bildschirme; Drag-and-drop für Reihenfolge und Elemente (`UI-05` bis `UI-07`).

**Gestaltungsregel:** Die echte Tafel hat **sieben leuchtende Farben plus Schwarz**. Browserdarstellung und Mischfarben dürfen keine physisch nicht vorhandenen LED-Farben versprechen.

---

## 8. Grafiken, Animationseditor und eigene Effekte

**Abhängigkeit:** Speicher- und Animationsprotokoll der konkreten Tafel nachweisen.

- [ ] Pixelgrafiken auf der 64×16-Matrix zeichnen, importieren und bearbeiten (`SPC-05`, `ANM-09`).
- [ ] Animierte Bilddateien **GIF, WebP, APNG** browserseitig dekodieren und in passende Frames umwandeln (`ANM-07`).
- [ ] Skalierung, Beschnitt, Ausrichtung, Transparenz und klare Vorschau anbieten.
- [ ] Echte 7-Farben-Palette, Quantisierung und optional räumliches Dithering; temporale Farbmischung nur nach Flimmertests (`ANM-08`).
- [ ] Eigene Animationseffekte: **Wellenlauf, Glitzer/Sternenhimmel, pulsierendes Herz, Ein-/Ausblenden, Pixelvorhang, Aufblitzen, Auflösen** (`ANM-06`).
- [ ] Übergänge zwischen Seiten als vorberechnete Pixelbilder oder native Effekte, jeweils mit nachvollziehbaren Limits (`ANM-09`).
- [ ] Effekte der Original-App (einschließlich Schneefall, Laser, Bild-/Musikmodi) systematisch dokumentieren und nach Eignung anbinden (`BLE-16`, `SPC-06`).
- [ ] Speicherbedarf, Uploadzeiten und autonome Laufzeit gegenüber ESP32-Live-Steuerung messen (`ANM-14`).

---

## 9. Spezialprogramme für das Festival-Totem

- [ ] **Countdown** mit Einstellen, Start, Pause, Fortsetzen und Reset (`SPC-01`).
- [ ] **Stoppuhr** mit Start, Pause, Reset und gut lesbarem Displaylayout (`SPC-02`).
- [ ] **Scoreboard** mit zwei oder mehr unabhängigen Zählern, zum Beispiel ❤️ / 💋 (`SPC-03`).
- [ ] **Lichtfunktion** als schnelles Weiß-/Rot-Display (`SPC-04`).
- [ ] Weitere Grafiken, Symbole, Animationen und kleine Interaktionen als wiederverwendbare Bibliothekselemente.
- [ ] Für dynamische Zähler eine **realistische Update-Strategie** entwickeln: Tafel-nativ, periodische BLE-Änderungen oder nachgewiesener Streaming-Weg; Prozentanzeige und Latenz berücksichtigen (`SPC-07`).
- [ ] Zeitbasierte Programmauswahl oder Rotation erst nach Klärung einer zuverlässigen Uhrzeitquelle ohne Internet (`LIB-10`).

---

## 10. Stabilität, Diagnose und Geräteverwaltung

- [ ] Die bereits vorhandene Diagnose bei jeder Version regressionsprüfen: Firmwarestand, Resetgrund, Uptime, Heap, zusammenhängender Speicherblock, BLE-Pakete, ACKs und Fehler (`DIA-01` bis `DIA-09`).
- [ ] **Diagnose mit einem Tipp kopieren**, TXT-Download als Alternative beibehalten; Safari-Fallback testen (`DIA-10`).
- [ ] BLE-Schreibvorgang, empfangenes ACK und tatsächliche Anzeige **getrennt** bewerten (`ANM-13`).
- [ ] **Langzeittests** für mehrstündigen Betrieb, wiederholte Übertragungen, WLAN/BLE-Koexistenz, Speicherfragmentierung und Neustarts (`SYS-10`).
- [ ] BLE-Geräteerkennung und frei konfigurierbare Tafel statt fest codierter Adresse (`SYS-08`).
- [ ] Wiederverbindung nach Übernahme der Tafel durch die Original-App; klare Fehler und sinnvolle Timeouts (`SYS-09`).
- [ ] Optional mehrere kompatible LED-Tafeln verwalten; tatsächlich unterstützte Protokoll- und Hardwaregenerationen getrennt dokumentieren (`SYS-12`, `DOC-09`).
- [ ] WLAN-Konfiguration später über Webinterface anpassbar machen. Das bisherige veröffentlichte Standardpasswort ist eine **bewusste Projektentscheidung**, kein als geheim behandelter Schlüssel.
- [ ] Firmwareinstallation bleibt **USB-only, ohne OTA**, solange keine bewusste neue Entscheidung getroffen wird.

---

## 11. GitHub, Lizenz, Dokumentation und Zusammenarbeit

**Kein zukünftiger Meilenstein „irgendwann öffentlich releasen“.** Das GitHub-Repository **existiert bereits**; die öffentliche Sichtbarkeit ist eine laufende Projekt- bzw. Repository-Einstellung und keine noch zu entwickelnde Firmwarefunktion. Der genaue derzeitige Visibility-Status wird hier nicht als technische Leistung behauptet.

**Rechtliche Ausrichtung:** RavestickBoard ist ein **nichtkommerzielles Hobbyprojekt**, derzeit **All Rights Reserved**, **keine allgemeine Open-Source-Codefreigabe**. Ein öffentlich sichtbares GitHub-Repository ist damit vereinbar. Eine spätere Lizenzänderung wäre eine separate bewusste Entscheidung, keine Voraussetzung für die weitere Entwicklung.

- [x] GitHub-Repository `du13500/RavestickBoard-for-CoolLED1248` angelegt; Firmwareversionen dokumentiert (`DOC-01` bis `DOC-03`).
- [x] Projekt-README mit Totem-Titelbild und dokumentiertem v0.2.0-Hardwaretest erstellt.
- [x] `LICENSE` für die derzeitige Rechtevorbehaltung vorbereitet; Code- und Quellenprüfung als erster Auditdurchgang durchgeführt.
- [ ] README, Changelog, Roadmap, Themenspeicher und Entscheidungen **bei relevanten Änderungen gemeinsam** fortschreiben (`DOC-04`).
- [ ] Bei **jeder Firmwareversion und jedem Korrekturpaket** eine **Excel-kopierbare Testtabelle direkt im Chat** liefern: `Nr. | Bereich | Testpunkt | Ergebnis | Bemerkungen`; letzte zwei Spalten leer; **keine separate Download-Datei** (`QA-01`).
- [ ] Für neue Funktionen sauber zwischen **implementiert**, **experimentell**, **auf unserer Hardware bestätigt** und **noch offen** unterscheiden (`ANM-12`).
- [x] OpenSign/`schulzad` **am 23.10.2026 per E-Mail** zur Nutzung kontaktiert (`LIC-03`). **Antwort/Erlaubnis weiter offen** (`LIC-08`); keine geschützten Codeteile übernehmen oder portieren.
- [ ] Herkunft neuer Referenzen und Codeanteile dokumentieren; MIT-Bedingungen bei tatsächlicher Übernahme fremden MIT-Codes beachten (`LIC-04`, `LIC-05`).
- [ ] Original-App-Screenshots/Herstellerfonts und Fremdassets bei öffentlicher Weitergabe gesondert auf Nutzungsrechte prüfen.
- [ ] Projektseite ohne überflüssigen Dateibaum im README halten; Badges auf Englisch, Projektbeschreibung auf Deutsch, Motto **„Mission: Hugs and Love“**.

---

## 12. Reihenfolge und Abhängigkeiten

| Reihenfolge | Arbeitspaket | Warum in dieser Reihenfolge? |
| --- | --- | --- |
| **Jetzt** | **v0.2.x-Feinschliff, Schriften, Entwurfsspeicherung und Live-BLE-Regler** | Die bereits funktionierende Basis wird zuverlässig und die unmittelbaren Testergebnisse fließen ein. |
| **Parallel als gezielte Forschung** | **`0x04`-Animation und nativer Blockmodus** | Erst verstehen, wie die Tafel mehrere Seiten autonom behandeln kann. |
| **Nach erfolgreicher Protokollklärung** | **Seite → Programm → Playlist** | Verhindert eine Architektur, die ständig unnötige Uploads erzwingt. |
| **Darauf aufbauend** | **Bibliothek, LittleFS, Favoriten und acht Shortcuts** | Programme müssen erst als eigenständige Objekte existieren. |
| **Danach / teilweise parallel** | **Rich-Text-Editor, Originalfonts, freie Pixelpositionierung** | Ein gemeinsamer Renderer erleichtert Erweiterungen. |
| **Bei nachgewiesener Hardwareunterstützung** | **Grafik-/Animation-Import, Effekte, Übergänge** | Speicher-, Timing- und Framelimits bestimmen die Umsetzung. |
| **Langfristig** | **Countdown, Stoppuhr, Scoreboard, Licht, Mehrgerät, optionale microSD** | Dynamische Anzeigen benötigen bewusst gewählte Ausführungswege. |

Die Reihenfolge ist **veränderbar**. Ein erfolgreiches Hardwareexperiment oder ein kritischer Bug kann Prioritäten verschieben. Wir vergeben neue Versionsnummern erst, wenn klar ist, welche Änderungen zu einem gemeinsam testbaren Paket gehören.

## Nicht vergessen

- **Original-App-Inhalte** sind Referenzen, keine automatisch übernommene Schriftbibliothek.
- **Eine Programmseite darf nie ohne Zustimmung zerschnitten werden.**
- **Ein Programmwechsel ist etwas anderes als ein Seitenwechsel.**
- **Die zweizeilige Minischrift ist bereits ein echter Erfolg und wird nicht ohne Grund ersetzt.**
- **Die Prozentanzeige gehört zum Uploadverhalten der Tafel; wir lösen das durch ein besseres Übertragungskonzept, nicht durch bloßes Verstecken im Webinterface.**
- **Neue Ideen werden zunächst im [`THEMENSPEICHER.md`](THEMENSPEICHER.md) mit ihrer ID erfasst, bevor daraus Meilensteine werden.**

*Made for the rave, powered by a banana. 🍌*

## Entwicklungsnachtrag 24.10.2026: v0.3.0

**v0.3.0-dev** enthält einen ersten Quelltext- und Browser-Testkandidaten für Originalschriften (Normal/Bold), Inline-Symbole, sieben Herzfarben, gespiegelte Ravestick-Banane, horizontale und vertikale Mini-Einzelzeile, eigenständige Live-BLE-Befehle und Browser-Entwurfswiederherstellung. **Noch kein Arduino- und kein Boardtest.**

Zu prüfen: echte Vorschub-/Zeichenabstände mit der Original-App, Symbolüberlagerungen bei Mini-Zweizeilern, sämtliche Glyphen im realen Raster, Short-Command-Verbindungsfreigabe, Rückwärtskompatibilität des NVS-Speichers und Nutzung der Originalsymbole in öffentlichen Quellen. `0x04` und die vollständige Programm-/Playlist-Architektur bleiben eigenständige Entwicklungsmeilensteine.


## Stand nach v0.3.1 und Prioritäten für v0.3.2 (25.10.2026)

**Durch Nutzer auf Hardware bestätigt:** v0.3.1 mit übertragenen 40/60/80 Zeichen, umfangreichen Symbolketten, Mini-Leerzeichenkorrektur, persistenten NVS-Seiten und direkt steuerbaren BLE-Parametern. Mobile-Safari-Symbolpalette unbenutzbar, Lauftext-Schriftwahl fehlte und Test-/Demotexte waren weiterhin störend.

**In v0.3.2 vorbereitet, noch zu testen:** mobile Symbolpalette mit korrekter DOM-Schließung und Mehrfachauswahl ohne Tastaturzwang; fünf einheitliche Schriftbezeichnungen in beiden Editoren; synchroner Mini-Zweizeilen-Lauftext; neue leere Textfelder; Experiment mit 600 Lauftextzeichen, rot/fetter Zähler ab 601 ohne Popup. Protokollunterstützung jenseits 80 Zeichen ist ungeklärt.

**Nicht in v0.3.2:** neue Symbole, neue Banane, eigene Mini-Zeilenausrichtung, kompaktere Vorschau, visueller Einzelzeichen-Formatierungseditor, NVS-Bibliotheksumbau, robuster automatischer BLE-Retry, native Seitenprogramme ohne Zwischen-Upload. Historische Tabellen vor diesem Nachtrag sind zeitpunktbezogen zu lesen.
