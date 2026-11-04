# 🍌 RavestickBoard – Entwicklungsstand v0.3.2

**Stand:** 25. Oktober 2026, ca. 11:45 Uhr MEZ (Abschluss des bisherigen Testdurchlaufs)  
**Aktuelle getestete Firmware:** **v0.3.2, Korrektur 2 (WLAN/RAM)**  
**Controller:** ESP32-C3 Super Mini · **LED-Tafel:** CoolLEDX / CoolLED1248, 64 × 16 Pixel, sieben darstellbare LED-Farben  
**Status:** auf echter Hardware kompiliert, geflasht und praktisch getestet; **kein vollständiger Abnahmetest aller Funktionen**

> Dieses Dokument ist eine Momentaufnahme vom 25.10.2026. Es trennt reale Messungen von Schlussfolgerungen und geplanten Funktionen. Maßgeblich für spätere Änderungen sind die Firmware und erneute Hardwaretests. Das öffentliche GitHub-Repository wurde durch die Dokumentationserstellung **nicht automatisch geändert**.

## 1. Kurzfazit

**v0.3.2 Korrektur 2 funktioniert wieder mit WLAN und BLE.** Die neue Symbolpalette ist auf dem iPhone bedienbar; fünf Schriftarten stehen auch beim Lauftext bereit; der synchron zweizeilige Mini-Lauftext ist auf der Tafel bestätigt. Neue Eingabefelder starten ohne Demotext, vorhandene Blockseiten bleiben gespeichert. Der Zeichenzähler wird ab 601 rot und fett, ohne Eingaben abzuschneiden oder ein Popup auszulösen.

**600 normale Textzeichen wurden erfolgreich übertragen und sichtbar dargestellt.** Lange Symbolfolgen benötigen jedoch viel mehr Bilddaten: 300 gemischte Symbole funktionieren, 320 gemischte Symbole und 320 Warnsymbole (`⚠️`) nicht, während 320 Blitze (`⚡`) funktionieren. Ein Erfolgseintrag im BLE-Log allein belegt **keine erfolgreiche Bildübernahme durch die Tafel**.

Die derzeitige 600-Zeichen-Eingabegrenze ist ein **Experiment**, keine ermittelte Hardwareobergrenze. Als nächster Architekturansatz wird **eine live berechnete Pixel-/Datenkapazität statt eines starren Zeichenlimits** untersucht. Außerdem ist eine **Bibliothek mit verketteten Lauftexten** vorgesehen, um mehrere Nachrichten durch **einen** BLE-Upload laufen zu lassen – und damit die Prozentanzeige nicht zwischen jeder Nachricht erneut zu sehen.

## 2. Versionsgeschichte und Reparaturen

| Version | Tatsächlich beobachteter Zustand |
| --- | --- |
| v0.3.1 | Kompiliert und hardwaregetestet; nutzbarer Rückfallstand. Mobile Symbolpalette auf iPhone noch fehlerhaft; Lauftextschriftwahl eingeschränkt; Grenze 80 Zeichen. |
| v0.3.2 (erste Ausgabe) | Compilerfehler: `drawInline()` erhielt zu viele Parameter. |
| v0.3.2 Korrektur 1 | Kompiliert und geflasht; WLAN-Access-Point startet nicht. Im seriellen Log: `wifi:alloc pp wdev funcs fail`; NVS-Seiten weiterhin vorhanden. |
| **v0.3.2 Korrektur 2** | **Vom Nutzer kompiliert, geflasht und praktisch getestet:** AP `CoolLED1248`, Webinterface und BLE wieder funktionsfähig; zehn ursprüngliche NVS-Seiten beim Start geladen. Längere Testreihe erfolgreich durchgeführt. |

**Reparatur Korrektur 2:** Die erste v0.3.2-Version reservierte zwei permanente Bildpuffer von zusammen **122.507 Bytes**. Korrektur 2 verwendet stattdessen einen temporären, passend dimensionierten Übertragungspuffer. Die zusätzlichen RAM-Diagnosen zeigen für einen erfolgreichen Start:

```text
[RAM] Start: frei=219248 B, groesster Block=114676 B
[NVS] Seiten geladen: 10
[RAM] Nach BLE/NVS: frei=146660 B, groesster Block=114676 B
[WLAN] AP bereit, SSID=CoolLED1248
[RAM] Nach WLAN: frei=88512 B, groesster Block=77812 B
[HTTP] Webserver gestartet
```

Der Speicherfix behebt den beobachteten Startfehler; er garantiert **nicht**, dass ein beliebig großer Bildpuffer später reserviert werden kann. Deshalb weiterhin den größten zusammenhängenden freien RAM-Block berücksichtigen.

## 3. Funktionstests v0.3.2 Korrektur 2

### Bestätigt („ja“ im Nutzer-Testprotokoll)

| Bereich | Ergebnis |
| --- | --- |
| Arduino/Start | Sketch kompiliert; Webinterface meldet v0.3.2. |
| Speicherung | Bisherige Blockseiten erhalten (im Startlog zunächst zehn). |
| iPhone/Symbolpalette | Vollständig sichtbar; mehrere Symbole hintereinander einfügbar; kein automatisches Öffnen der Tastatur. |
| Schriftarten | `Standard`, `Standard Bold`, `Engschrift`, `Mini Einzeilig`, `Mini Zweizeilig` auswählbar und auf LED-Tafel korrekt. |
| Zweizeiliger Mini-Lauftext | Beide Zeilen laufen synchron. **Offen:** Schriftfarbe derzeit nicht separat pro Zeile einstellbar. |
| Neue Entwürfe | Neuer Lauftext und neue Blockseite beginnen leer, ohne `Grüße` bzw. `UMARMUNG`. |
| Zeichenzähler | Bis 600 normal, ab 601 rot und fett; keine automatische Kürzung und kein Popup. |
| BLE-Text | 600 normale Textzeichen erfolgreich auf das Board übertragen und dargestellt. |

**Nicht systematisch geprüft:** alle Regressionstests, Langzeitbetrieb, automatische Blockseitenfolge unter Belastung, genau 80/100/150/200/300/400/500 Zeichen als *jeweils einzeln protokollierte* Textlängen sowie eine vollständige Wiederholbarkeit der Grenzfälle. Der vorliegende Log enthält zusätzlich eine erfolgreiche **200-Zeichen-Übertragung**, aber nicht sämtliche ursprünglich geplanten Zwischenlängen.

## 4. Langtext- und Symboltests: Messdaten

Die **Bytezahlen und BLE-Pakete** stammen aus dem seriellen Testlog vom 25.10.2026. „Anzeige“ bezeichnet ausdrücklich die Rückmeldung des Hardwaretesters und nicht bloß eine positive Meldung im BLE-Log.

| Testeingabe | Nutzlast gesamt | BLE-Pakete | Controller-Job | Sichtbare Tafelanzeige |
| --- | ---: | ---: | ---: | --- |
| 200 normale Zeichen | 9.221 B | 73 | OK · 20,8 s | im Log OK; keine gesonderte Sichtprüfung in der Tabelle |
| **600 normale Zeichen** | **27.461 B** | **215** | **OK · 54,3 s** | **Ja** |
| 14 Symbole | 1.529 B | 12 | OK · 4,9 s | Ja |
| 90 gemischte Symbole | 9.173 B | 72 | OK · 21,2 s | Ja |
| 180 gemischte Symbole | 18.245 B | 143 | OK · 39,6 s | Ja |
| 270 gemischte Symbole | 27.317 B | 214 | OK · 53,3 s | Ja |
| **300 gemischte Symbole** | **29.837 B** | **234** | **OK · 62,7 s** | **Ja** |
| 320 × `⚡` | 25.061 B | 196 | OK · 53,8 s | Ja |
| **320 gemischte Symbole** | **31.853 B** | **249** | **OK · 64,0 s** | **Nein** |
| **320 × `⚠️`** | **32.741 B** | **256** | **OK · 68,1 s** | **Nein** |
| 450 gemischte Symbole | 43.817 B | 343 | OK · 91,1 s | Nein |
| 500 gemischte Symbole | 48.683 B | 381 | OK · 99,3 s | Nein |

Die ursprüngliche Testliste nennt außerdem **600 Symbole: Fehler**; der Log zeigt dafür eine bereits **vor BLE** gescheiterte RAM-Reservierung: 58.355 Bytes angefordert, größter freier Block nur 49.140 Bytes. Ebenso scheiterten weitere sehr große Eingaben (514, 522, 530, 540 und 553 dekodierte Zeichen) bei der Pufferspeicherreservierung. Im Gegensatz dazu konnten die 48.683 Bytes für 500 Symbole noch angelegt und vollständig per BLE geschrieben werden – die Tafel übernahm die Darstellung aber nicht sichtbar.

**Wichtig zur Zählweise:** Emoji wie `❤️` oder `⚠️` können aus mehreren Unicode-Codepoints bestehen. Die Firmware ignoriert bestimmte Darstellungsselektoren beim Dekodieren. Die im Log gezählten „Zeichen“ entsprechen deshalb nicht unter allen Umständen einer identischen Anzahl sichtbarer Emoji-Grapheme. Für die spätere Kapazitätsanzeige zählt die **gerenderte Breite**, nicht die Länge einer JavaScript- oder UTF-8-Zeichenkette.

## 5. Technische Einordnung: Pixelbreite und Datenlimit

### Bereits im Quellcode nachvollziehbar

Beim derzeitigen 16-Pixel-hohen, farbigen Übertragungsraster werden je Pixelspalte **6 Bytes** benötigt (2 Bytes pro Farbkanal für 16 LEDs); der bisherige Übertragungsheader umfasst **107 Bytes**. Daher gilt für den gerenderten Lauftext:

```text
Payload-Bytes = 107 + 6 × max(64, gerenderte_Pixelspalten)
```

Beispiele: Die 29.837 Bytes des erfolgreichen 300-Symbol-Tests entsprechen 4.955 Pixelspalten; die 31.853 Bytes des fehlgeschlagenen 320-Symbol-Tests entsprechen 5.291 Spalten. Der 320-Blitze-Test mit nur 4.159 Spalten funktioniert.

### Gut begründete, aber unbestätigte Hypothese

Die Tafel könnte bei ungefähr **30 KiB Übertragungsdaten** oder bei ungefähr **5.120 Pixelspalten** eine interne Obergrenze haben. Die bisherige Testreihe **beweist keinen dieser Werte**. Auch ein anderes protokollbedingtes Limit ist möglich.

Ein gezielter Unterscheidungstest wäre beispielsweise **300 / 301 / 302 × `⚠️`** unter möglichst identischen Bedingungen. Aus der bisherigen gerenderten Breite ergeben sich rechnerisch:

| Inhalt | Breite (erwartet) | Payload (erwartet) |
| --- | ---: | ---: |
| 300 × `⚠️` | 5.099 px | 30.701 B |
| 301 × `⚠️` | 5.116 px | 30.803 B |
| 302 × `⚠️` | 5.133 px | 30.905 B |

30 KiB = 30.720 Bytes; eine **reine 30-KiB-Grenze** würde sich anders zeigen als eine **5.120-Pixel-Grenze**. Die Eingaben sind **noch nicht getestet**; die erwarteten Werte müssen mit dem tatsächlichen Renderer abgeglichen werden.

**Vorsicht:** Eine maximale BLE-Nutzlast ist nicht dasselbe wie eine RAM-Grenze auf dem ESP32, eine maximale Datenmenge im CoolLEDX-Board oder eine Zeichengrenze des Webeditors. Diese drei Größen müssen separat geprüft werden.

### BLE-Quittung ist keine Anzeigequittung

Bei mehreren gescheiterten Anzeigetests wurden **alle** Datenpakete geschrieben und Notifications empfangen; der Controller protokollierte anschließend `JOB OK`. Das belegt bisher nur einen abgeschlossenen Übertragungsvorgang nach der derzeitigen Logik, **nicht**, dass das Board die Grafik angenommen oder dargestellt hat. Künftige Statusmeldungen müssen deshalb sprachlich zwischen „Pakete übertragen“ und „Anzeige auf der Tafel bestätigt“ unterscheiden. Ob das Protokoll eine echte Abschlussbestätigung bietet, ist offen.

Zusätzlich enthält der aktuell verstandene BLE-Textheader **80 Format-Slots** und einen entsprechend begrenzten Zählwert. In v0.3.2 wird der Header bei längeren Texten weiterhin auf höchstens 80 gesetzt, während die längere Pixelmatrix übertragen wird. Der Test mit 600 normalen Zeichen belegt, dass eine längere Darstellung **in diesem Versuch** möglich war; er klärt nicht abschließend sämtliche Headerfelder und Modi.

## 6. Produktentscheidung: Datenbudget statt starrem Zeichenlimit (Idee, noch nicht implementiert)

**Ziel:** Der Editor soll die tatsächliche Bildbreite und daraus die Payloadgröße **live beim Tippen und beim Einfügen von Symbolen** berechnen. Ein schmaler Buchstabe, ein breites Originalsymbol, eine andere Schriftart und der Abstand zwischen Zeichen verbrauchen jeweils ihren tatsächlichen Platz.

- Anstelle einer willkürlichen Grenze „maximal 600 Zeichen“ soll künftig die berechnete **Bilddatenkapazität** entscheidend sein. Die erlaubte Menge kann damit je nach Schriftart und Inhalt stark variieren.
- Anzeige beispielsweise als `Pixelspalten`, `KiB` und Kapazitätsbalken. Die endgültige Obergrenze wird erst nach Hardwaremessungen festgelegt.
- Bei Überschreitung **kein Popup und keine automatische Textkürzung**: vorhandene Eingabe behalten, Kapazitätsanzeige **fett und rot**, Übertragung verhindern, bis der Inhalt wieder passt.
- Berechnung im Browser und auf dem Controller muss **denselben Renderer / dieselben Glyphenbreiten** zugrunde legen. Andernfalls kann die Webanzeige von der tatsächlichen Payload abweichen.
- Unabhängig davon die **aktuelle maximale Einzelallokation** des ESP32-C3 berücksichtigen; freier Heap insgesamt reicht nicht aus, wenn kein hinreichend großer zusammenhängender Block verfügbar ist.
- Die festen Firmware-Arrays, die bisherige 600-Codepoint-Prüfung, die 80 Headerformatplätze sowie HTTP-/BLE-Datenstrukturen dürfen bei einem späteren Umbau nicht durch bloßes Anheben einer UI-Zahl übersehen werden.

**Noch nicht umgesetzt oder freigegeben:** Eine konkrete feste maximale KiB-Zahl, eine größere als 600 Zeichen umfassende Firmwareeingabe und die neue datenbasierte Live-Zähleranzeige.

## 7. Produktidee: Lauftextprogramme durch Verkettung (noch nicht implementiert)

**Motivation:** Beim automatischen Blockseitenplayer wird für jede Seite erneut eine komplette Grafik übertragen. Das löst jeweils die eingebaute **Prozentanzeige** der CoolLEDX-Tafel aus und stört eine durchgehende Festival-Anzeige.

**Einfacher, plausibler Einstieg:** Mehrere separat gespeicherte **Lauftexte** aus einer zukünftigen Bibliothek auswählen und im Webinterface zu **einem gemeinsamen horizontalen Pixelstreifen** kombinieren. Die Einzeltexte dürfen sich in Schriftart, Farbe und Symbolen unterscheiden, sofern der kombinierte Renderer sie korrekt zusammenführt. Zwischen den Beiträgen frei wählbare Leerabstände. Anschließend ein BLE-Upload, dann ein durchlaufendes Laufband.

```text
Bibliothekseintrag A ─┐
Bibliothekseintrag B ─┼─> Programmeditor ─> gemeinsames Pixelraster ─> 1 BLE-Upload
Bibliothekseintrag C ─┘
```

**Erwarteter Vorteil:** Nur **eine** prozentuale Fortschrittsanzeige beim Hochladen des gesamten Lauftextprogramms statt einer Fortschrittsanzeige zwischen den Teilnachrichten. Ob die gesamte Kombination in die Tafel passt, hängt vom später bestätigten Datenlimit ab. Unterschiedliche Abspielgeschwindigkeiten je Teilnachricht sind bei einem einfachen gemeinsamen Laufband nicht zugesichert.

**Nicht verwechseln mit echten Blockseitenprogrammen:** Ein einzelner Pixelstreifen kann nicht automatisch einen stehenden Text fünf Sekunden halten, dann einen Lauftext anzeigen und danach einen neuen Block aufrufen. Dafür müsste die CoolLEDX-Tafel passende zeit- oder modusgesteuerte Mehrteilprogramme verstehen. Ein solches Protokoll ist **auf unserer Tafel nicht nachgewiesen**; alternative Experimente mit nativen Programmen/Animationen bleiben Forschung.

**Begriffsarchitektur:**

- **Bibliothekseintrag** – ein unabhängig bearbeitbarer Text oder eine Seite mit Schrift, Farbe, Inhalt und später ggf. Tags/Favoriten; dauerhafte Gerätespeicherung noch nicht implementiert.
- **Lauftextprogramm** – aus mehreren Einträgen zu **einem** scrollenden Pixelband gerendert; geplante erste Programmart.
- **Echtes Blockseitenprogramm** – mehrere unterschiedliche Anzeigeschritte ohne erneuten Upload; Hardware-/Protokollfrage offen.
- **Playlist** – mehrere fertige Programme, zwischen denen der ESP32 bei Bedarf neu übertragen darf; späteres separates Feature.

Einträge sollen unabhängig editierbar bleiben. Der Programmeditor speichert **Referenzen/Reihenfolge/Abstände**, nicht bloß einen schwer editierbaren zusammenkopierten Langtext. Für künftige persistent gespeicherte Programme ist ESP32-Flash/LittleFS als Ansatz vorgemerkt; noch keine fertige Bibliothek vorhanden.

## 8. Weitere offene Punkte / Backlog

### Beobachteter Änderungsbedarf

1. **Mini Zweizeilig im Lauftext:** getrennte Farbe pro Zeile wie im Blockeditor ermöglichen; gemeinsames Scrollen beibehalten.
2. **Grenzfälle:** reale Pixel-/Byte-Schwelle des CoolLEDX-Boards isolieren; insbesondere identische Symbole bei variierender Stückzahl.
3. **BLE-Rückmeldung:** nach vollständigem Upload nicht automatisch behaupten, dass die LED-Tafel das Bild angezeigt hat.
4. **Große Datenpuffer:** temporäre Speicherverwaltung, zusammenhängende Heap-Blöcke und Fehlerbehandlung weiter absichern.

### Spätere Ideen (nicht Teil von v0.3.2)

- Visueller Einzelzeichen-Editor für unterschiedliche Farben/Schriftarten, inklusive sinnvoller Rückgängig-/Wiederholen-Funktion; festfarbige Piktogramme ignorieren abweichende Textfarben.
- Vorschauplatzierung verbessern und individuelle Zeilenausrichtung für Mini-Zeilen untersuchen.
- Herstellernahe Symbolabstände prüfen; Ravestick-Banane überarbeiten und zusätzliche selbst gestaltete Icons ergänzen.
- Bibliothek, Lauftext-Programmeditor, Programmspeicherung, Sicherung/Import/Export und später Playlists.
- Stabilere Wiederverbindungen/Fehlerbehandlung im automatischen Seitenplayer.
- Den BOOT-Knopf des ESP32-C3 eventuell später für eine einfache Hardwarebedienung nutzen; **nicht implementiert**, Reset-Taster bleibt Reset.

## 9. Nächste sinnvolle Testreihenfolge

1. Aktuellen **v0.3.2-Korrektur-2-Stand** als funktionsfähigen Teststand dokumentieren; Firmware unverändert sichern.
2. Die **300/301/302-Warnsymbol-Tests** gezielt vergleichen und Pixel-/Bytezahlen zusammen mit realer Anzeige notieren. Wenn instabil, Ergebnisse wiederholen und keine feste Grenze behaupten.
3. Längere reine Texte über 600 sind mit der **aktuellen Firmware nicht regulär freigegeben**; zuerst den Editor und Controller dafür kontrolliert umbauen, mit RAM- und Headerprüfung.
4. Erst danach datenbasierten Live-Kapazitätszähler und Schutzprüfung integrieren und auf iPhone + ESP32 testen.
5. Schließlich einen ersten **einfachen Lauftextketten-Prototyp** aus zwei bis drei unabhängigen Inhalten erproben. Echte zeitgesteuerte Blockprogramme bleiben gesonderte Forschung.

## 10. GitHub-Hinweise

Die letzte vollständige Firmwareausgabe ist **`RavestickBoard_v0_3_2_Korrektur_2_WLAN_RAM.zip`**. Diese Dokumentation ist **kein Firmwareupdate**: Sie ändert keine `.ino`- oder `.h`-Datei. Der aktive Sketch befindet sich im Ordner `firmware/RavestickBoard_v0_3_2/` und benötigt die zugehörigen Dateien `RavestickBoard_WebUI.h` und `RavestickBoard_Assets.h`.

**Empfohlener Commit:** `docs: document v0.3.2 hardware tests and transfer-limit findings`

- Dokumente und den tatsächlich getesteten Firmwarestand in den **lokalen** Repositoryordner übernehmen; vor dem Push im GitHub Desktop alle geänderten Dateien kontrollieren.
- Bestehende Firmwareordner, `.git`, `LICENSE`, Quellen-/Lizenzdokumentation, historische Releases und reale Referenzbilder nicht versehentlich überschreiben oder löschen.
- Vorschläge zum Entfernen veralteter TXT-Dateien stehen in `docs/REPO_AUFRÄUMEN_v0.3.2.md`; eventuelle Bereinigung bewusst in einem **separaten Commit** vornehmen.
- Historische Dateien dürfen ihren damaligen Teststand nennen; die **aktuelle** Statusaussage steht in diesem Dokument, im README und im neuesten Changelog-Nachtrag.

---

**Quellenbasis:** manueller v0.3.2-Testbericht des Nutzers vom 25.10.2026; serieller Log mit RAM-/BLE-Angaben und Testzeitpunkten; Firmwarequellcode `RavestickBoard_v0_3_2.ino` aus Korrektur 2; frühere Projektentscheidungen und Testpläne. Messungen und gemeldete Sichtprüfungen werden als Nutzerhardwaretests bezeichnet, nicht als unabhängig durch den Assistenten ausgeführte Geräteprüfungen.
