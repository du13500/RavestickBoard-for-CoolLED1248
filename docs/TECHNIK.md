# Technikstand v0.4.2.1-dev (aktueller Testkandidat)

## v0.4.2.2: Programm-Timer, Anzeigegruppen und Flash-Abhängigkeiten

`v042Tick()` wird in `loop()` nach `web.handleClient()` geprüft. Nach jedem erfolgreichen Gruppen-Upload legt `v042AfterJob()` für gemischte Programme den nächsten Umschaltzeitpunkt fest. Bei **einer** homogenen Gruppe bleibt die Tafel in ihrer autonomen Endlosschleife; der ESP32 schickt keine periodischen neuen Bilder. Für zusammenhängende Blöcke wird bei Geschwindigkeit 60–90 die empirische Laufzeit aus Seitenzahl und Tempo verwendet, sonst für die Gruppe ein Fallback von 5–180 Sekunden. Lauftextgruppen verwenden bis zu einer separaten Kalibrierung ebenfalls den Fallback. Das Tempo bleibt während aktiver gemischter Wiedergabe unverändert.

`RBP2`-Dateien enthalten weiterhin die ursprüngliche Liste von Bibliotheks-IDs; bei Löschung einer ID wird **die Programmdatei nicht verändert**. `rb42ReadCollection()` validiert die Struktur, fehlende IDs werden im Listen-/Detail-JSON gezählt, und nur `rb42QueueStep()`/Start lehnen unvollständige Programme ab. Direkte Kurzwahlen auf gelöschte Bibliotheks-IDs werden einschließlich Beschriftung zurückgesetzt; `libraryMeta.shortcuts` wird ebenfalls aktualisiert. Ein gerade spielendes betroffenes Programm wird angehalten. Die Wiedergabe von `0x06`/`0x07` und BLE-Bitmapdaten ist unverändert.

Webserver und BLE befinden sich weiterhin in einer Hauptschleife; deshalb können Statusabfragen während längerer Uploads verzögert sein. Die Browser-UI blendet auf einzelne Timeouts keine Offline-Meldung mehr ein und protokolliert Verbindungsprobleme nur bei anhaltenden Fehlschlägen. **Dies ist kein asynchroner HTTP-Server** und keine garantierte Verbindungsüberwachung.


**Neu: Der Blockeditor ist ab v0.4.2.1 nur RAM-Arbeitsfläche; nur explizites Bibliotheksspeichern schreibt LittleFS. Die bisherigen NVS-Daten werden nicht gelesen, geändert oder gelöscht. Blockfolgen/Programme werden vor dem Dateiaustausch mit 128-Byte-Puffer verifiziert; die frühere große Kopie verursachte einen Stackschutz-Absturz beim Speichern.**

**Die nachfolgenden v0.2/v0.3-Abschnitte sind historische Architektur- und Testnotizen.** Das aktuelle Design steht in [BIBLIOTHEK_v0.4.2.md](BIBLIOTHEK_v0.4.2.md). Aktuell gelten: bis zu 25 Arbeitsseiten; LittleFS Textdateien RBL1, Blockbegleitdaten RBB2, Programme RBP2, Kurzwahlen RBS2; alter RBM1-Journalindex wird weiterhin gelesen. Pixelbänder verwenden ein 384-Byte-RGB-Kachelverfahren bei größeren Transfers und bleiben innerhalb des konservativen 30-KiB-Datenlimits. BLE-Sendebefehle bleiben `0x02` sowie `0x06` für den Modus, `0x07` für Tempo und `0x08` für Helligkeit. Die Pixelband-Übertragung und acht Anzeigemodi sind durch den Nutzer auf der CoolLEDX hardwarebestätigt. Die Speicherfehlerkorrektur der v0.4.2.1 ist dagegen noch nicht auf Hardware gegengeprüft.

---

## Ergänzung v0.3.5

### v0.3.5 – zusätzliche Steuer- und Symbolcodes

- `0x09 0x00` Display AUS, `0x09 0x01` Display EIN gemäß CoolLEDX-Referenztreiber; auf unserer Hardware noch **nicht** geprüft. UI kennt nur zuletzt gesendeten Befehl, liest den physischen Zustand nicht aus.
- Tempo `0x07` bisheriges Mapping 60..100 → 153..255 (`round(255 × slider / 100)`). Keine Laufzeitkalibrierung.
- 34 Sprite-Matrizen à 128 Byte (16×16×4 Bit); Indizes 0..23 alte Original-App, 24 Ravestick-Banane, 25 Kussmund, 26 Blume, 27 Biene, 28 Ente, 29 Kuh, 30 Kaktus, 31 UFO, 32 UFO mit grünem Strahl, 33 Alien. Vier neue App-Motive aus Screenshots beprobt, sechs eigene Motive.
- `🌼` und das historische Alias `🌹` zeigen beide Bitmap 26. UFO+Strahl über `🛸` + VS15 (U+FE0E) → interner PUA-Code U+E34D, analog zur Ravestick-Banane.
- Alle Symbole in Mini-Zweizeile bleiben 16 px hoch, Überlappung ist bewusst zulässig. Für Blume reduziert `tokenGap` den rechten Abstand um ein Pixel in Firmware **und** JS-Vorschau.

---

# RavestickBoard · Technische Bestandsaufnahme

**Aktualisiert: 25.10.2026 · Auf Hardware geprüft: v0.3.3; neues Modus-Feature v0.3.4 noch ungetestet.**

**Mess- und Architektur-Nachtrag:** In v0.3.3 erfolgreich: 650 Zeichen / 30.689 B und 301 gemischte Zeichen / 30.695 B. `107 + 6 × max(64, Pixelspalten)` ist die aktuelle Payloadformel, mit bewusst **vorläufigem 30-KiB-Softwarebudget**. Der frühere 600-Zeichen-Grenzwert ist seit v0.3.3 entfernt. Die tatsächliche Grenzgröße des LED-Boards bleibt unbekannt; BLE-Paketquittung beweist keine sichtbare Grafik. Siehe [`ENTWICKLUNGSSTAND_v0.3.4_2026-10-25.md`](ENTWICKLUNGSSTAND_v0.3.4_2026-10-25.md).

Dieses Dokument unterscheidet **durch Nutzer am konkreten Board bestätigt**, **im Quellcode vorhanden** und **für spätere Versionen geplant**. Die Firmware des LED-Boards wird nicht verändert. Historische v0.1.x-Einschränkungen sind weiter unten separat aufgeführt.

## Techniknotiz v0.3.3 – experimentelles Lauftext-Datenbudget (25.10.2026)

**Nur Lauftext**: Keine festen 600 Codepoint-Arrays mehr; UTF-8 wird mithilfe eines iterativen Decoders zweimal gescannt (Breitenmessung und Rendering). Variation-Selector U+FE0F wird übersprungen, Banane + U+FE0E bleibt das getrennte Ravestick-Piktogramm. Die tatsächlichen `tokenWidth()`-Maße aus Originalschrift/Symbolsprite plus ein Pixel Abstand je Zeichen ergeben die Bildbreite. Ein Terminatorabstand am Zeilenende wird nicht gezählt. Für Mini Zweizeilig zählt die **maximale** Breite der beiden synchronen Zeilen, nicht deren Summe; jede Zeile besitzt eine eigene RGB-Farbe. Der Frontend-Rechner benutzt dieselben Glyphenbreiten und berechnet `107 + 6 × max(64, Breite)` Bytes. Vorläufiges Budget `30*1024=30720` Bytes, maximale Spaltenzahl `floor((30720-107)/6)=5102`. Beide Seiten prüfen das Budget; Text wird nicht stillschweigend gekürzt.

Header: Der bekannte Protokoll-Header bleibt 107 Bytes groß, die ursprünglichen 80 Formatplätze werden nicht erweitert. Bitmapdaten folgen danach. **BLE-Paket-Notifications allein sind kein Anzeigebeleg**. Bei knappen zusammenhängenden RAM-Blöcken kann selbst unterhalb von 30 KiB eine dynamische Allokation fehlschlagen; dann keine BLE-Übertragung. NVS-PageRecord-Schema und historische Blockseiten bleiben unverändert. v0.3.3 wurde am 25.10.2026 vom Nutzer erfolgreich hardwaregetestet.

## 1. Geräte und Entwicklungsumgebung

| Komponente | Aktueller Teststand |
| --- | --- |
| Controller | TENSTAR ROBOT ESP32-C3 Super Mini, 160 MHz, 4 MB Flash laut `esptool` |
| Arduino | IDE 2.3.10, Espressif Arduino-ESP32 3.3.12, `ESP32C3 Dev Module` |
| USB | USB-C, `USB CDC On Boot: Enabled`; Firmware **ausschließlich per USB**, kein OTA |
| Partitionsschema | `No OTA (2MB APP/2MB SPIFFS)`; tatsächliche Partitionen bestimmen nutzbare Bytes |
| Tafel | Original-CoolLEDX, **64×16 Pixel**, sieben LED-Farben, Advertising-Firmwarekennung `06` |
| Test-MAC | `ff:00:00:03:7e:c3`, derzeit in der Firmware festgelegt; keine allgemeine Modelladresse |
| BLE | Service `0000fff0-0000-1000-8000-00805f9b34fb`, Characteristic `0000fff1-0000-1000-8000-00805f9b34fb` (Read, Notify, Write Without Response) |
| Wi-Fi | ESP32-AP `CoolLED1248`, Passwort `Banane1248!`, Browser `http://192.168.4.1`; **bewusst dokumentiertes Testpasswort** |

Die ursprünglichen Advertising-Daten laut BLE-Scan: `e1ebff0000037ec31000400106` (mit Herstellerkennung; enthält die getestete MAC, Höhe `0x10`, Breite `0x0040`, Farbmodus `01` und Firmwarekennung `06`). Der Nutzer hat bestätigt, dass der zuletzt übertragene Inhalt auf der Tafel ohne BLE-Verbindung und nach deren Stromtrennung erhalten bleibt.

## 2. Datenfluss

```text
Browser (iPhone, Laptop)
  └─ WLAN/AP → ESP32-C3-Webserver (lokal, ohne Internet)
                  ├─ HTML/CSS/JavaScript aus `RavestickBoard_WebUI.h`
                  ├─ Blockseiten/NVS und BLE-Auftragsverwaltung
                  └─ BLE FFF0/FFF1 → originale CoolLEDX-Tafel
```

Nach der ersten v0.2.0-Kompilierungsfehlermeldung (`'function' does not name a type`) wurde die Weboberfläche aus der `.ino` in **`RavestickBoard_WebUI.h`** ausgelagert. **Beide Dateien müssen im selben Sketch-Verzeichnis liegen.** `tools/sync_web.py` dient der Synchronisation des separaten Webquellcodes. Die korrigierte Fassung wurde vom Nutzer erfolgreich geflasht.

## 3. BLE-Protokoll und Übertragung

- Gerätespezifischer Rahmen: Start `0x01`, Big-Endian-Länge, Byte-Escaping der Werte `0x01..0x03`, Ende `0x03`.
- Text-/Bildübertragung nutzt **RGB-Bitebenen**, Spaltenreihenfolge, 16 Pixel Höhe = zwei Bytes pro Spalte und Kanal. Acht Zustände einschließlich Schwarz, davon sieben leuchtende Kombinationen.
- Nutzdaten werden für BLE in Blöcke mit bis zu **128 Bytes Rohdatenteil** samt Blockmetadaten/XOR-Prüfsumme übertragen. Notification und tatsächliche Bilddarstellung müssen separat bewertet werden.
- **`0x08` Helligkeit**: als eigenständiger Steuerbefehl auf unserer Hardware bereits getestet. Das aktuelle Webinterface führt einen eigenen Helligkeitsauftrag aus, aber noch **keine durchgängige Live-Aktualisierung während der Fingerbewegung**.
- **`0x07` Geschwindigkeit**: im bisherigen Lauftext-Sendeablauf verwendet. UI-Wert 1–100 wird aktuell auf Rohwert 0–255 skaliert; eine Entsprechung zur Original-App-Zahl **80** ist **nicht** nachgewiesen. Ein Live-Speed-Regler ohne Textupload fehlt.
- **`0x06` Anzeigemodus**: kurzer, eigenständiger Steuerbefehl mit *einem Byte Moduswert*. In v0.3.4 sind die Referenzwerte über das Live-Dropdown ohne Bild-Upload erreichbar. **Nur `0x01` und `0x02` sind auf der konkreten Tafel bestätigt**, übrige Namen stammen aus der [CoolLEDX-Referenzimplementierung](https://github.com/UpDryTwist/coolledx-driver/blob/main/src/coolledx/__init__.py):

  | `0x06`-Parameter | Referenzbezeichnung | Status am Ravestick |
  |---|---|---|
  | `0x01` | Standbild | getestet |
  | `0x02` | Linkslauf | getestet |
  | `0x03` | Rechtslauf | offen |
  | `0x04` | Aufwärts | offen |
  | `0x05` | Abwärts | offen |
  | `0x06` | Schneeflocke | offen |
  | `0x07` | Bild/Picture | offen |
  | `0x08` | Laser | offen |

  **Nicht verwechseln:** Die Moduswerte `0x04`, `0x06` und `0x08` sind Parameter des Befehls `0x06`, keine selbstständigen Opcodes. Der separate **Opcode `0x04` für gespeicherte Animationen** ist hiervon unabhängig. Ein Moduswechsel bestätigt noch keinen autonomen Szenenwechsel/keine Unterstützung einer bestimmten Animation.
- **`0x04` gespeicherte Animation** bleibt Forschungsgegenstand, an unserer Tafel noch nicht hardwarebestätigt und nicht als eigenständiges Animationsprogramm implementiert.

### Verbindungskonflikte und Uploadverhalten

Die aktuelle Firmware verbindet sich für eine Übertragung zum Board, schickt Steuerbefehle und Daten und trennt anschließend. Während die Original-App die BLE-Verbindung belegt, kann ein Auftrag scheitern. Unter v0.1.1 wurde das automatische Wiederfunktionieren nach Freigabe durch die Original-App beobachtet, ohne ESP32-Neustart. **Eine dauerhaft offene Verbindung für Echtzeitregler ist noch nicht implementiert.**

Die vom LED-Board eingeblendete **Prozentanzeige erscheint bei jedem vollständigen Inhaltsupload** und lässt sich mit den bisherigen Steuerbefehlen nicht einfach im ESP32-Webinterface verbergen. Separate Modus-/Helligkeitsbefehle sollen dieses Verhalten vermeiden.

## 4. v0.2.0: Blockeditor und Seitenübertragung

| Eigenschaft | Tatsächlich umgesetzt |
| --- | --- |
| Seitenstruktur | `PageRecord`: UTF-8-Zeilen `line1[48]`, `line2[48]` (je maximal 47 gespeicherte Textbytes plus Abschluss), Layout, Ausrichtung, Farben, Dauer |
| Seitenanzahl | Maximal **12** Seiten in NVS (`Preferences`, Namespace `rbpages`, Schema 1) |
| Zeilenlimit | Maximal **10 Unicode-Codepoints pro Zeile**; zusätzlich Begrenzung der Bytefelder beachten |
| Anzeigedauer | **5–60 Sekunden je Seite**, gerechnet **nach** vollständig erfolgtem BLE-Upload |
| Große Zeile | 5×7-Glyphen mit vertikaler Verdopplung; aktuell ungefähr y=1 bis y=14 |
| Zwei Mini-Zeilen | 5×7-Glyphen bei y=0–6 sowie y=8–14; dazwischen **eine** unbeleuchtete Pixelreihe |
| Farben und Ausrichtung | Zwei separate Zeilenfarben (je 1–7), links/zentriert/rechts; keine Mischfarben pro Zeichen |
| Text-Payload | 24 führende Nullbytes + Textlänge + 80 Byte Zeichenmetadaten + 2 Byte Pixel-Länge + **384 Byte 64×16-Bitfelddaten** = **491 Bytes**, normalerweise **vier Datenchunks** |
| Steuerung | Manuelle Einzelseite oder automatischer ESP32-Seitenplayer; **jeder Wechsel vollständiger BLE-Upload** |
| Datenspeicherung | NVS-Blockseiten bleiben nach Browserreload und Controllerneustart erhalten; **kein automatischer Player-Neustart** |

**Am 23.10.2026 real bestätigt:** Start, iPhone-Webseite, Seiten anlegen/duplizieren/verschieben, Farbtrennung, zweizeilige Großbuchstaben-Minischrift, automatische Seitenfolge, individuelle Dauer, NVS-Persistenz und wiederholte Übertragungen. **13/14 ursprüngliche Testpunkte positiv, einzeiliger Schrift-Test ohne Ergebnis.** Das ist kein vollständiger Regressionstest aller API-Pfade.

**Wichtige Produktarchitektur:**

- **Seite:** Individuell gestaltetes Bild/Objekt.
- **Programm:** Eine oder mehrere Seiten, die *idealerweise mit einem* Transfer auf der Tafel autonom ablaufen. **Noch nicht implementiert.**
- **Playlist:** Mehrere Programme, deren Wechsel per ESP32/BLE erfolgen dürfen. **Der v0.2.0-Seitenplayer liefert dafür eine technische Grundlage, aber noch keine vollständige Programmbibliothek.**

## 5. Speicher und Diagnose

- **Vorhanden:** NVS für die aktuellen Blockseiten; RAM-Ringpuffer für Diagnose (56 Einträge); Fehler-/Transferzähler seit Boot; Status und TXT-Bericht.
- **Noch nicht vorhanden:** LittleFS-Programmbibliothek, dauerhaft gespeicherte Lauftext-Entwürfe, Browser-Reiterwiederherstellung, JSON-Backup/Import, Ordner/Tags/Favoriten.
- **Geplant:** LittleFS im vorhandenen ESP32-Flash für benannte Programme; Ex-/Import für externe Sicherung. microSD am ESP32-C3 via SPI nur **optionale spätere Erweiterung**, nicht für die heutige Textbibliothek nötig.
- **Speicheranzeigen:** `ESP.getSketchSize()` und Größe der **laufenden App-Partition** für rechnerisch freie App-Bytes; kein belegbarer SPIFFS/LittleFS-Verbrauch, solange kein Dateisystem gemountet wird. `ESP.getFreeHeap()`, `ESP.getMinFreeHeap()`, `ESP.getMaxAllocHeap()` für RAM-Diagnose.
- **Sicherheit:** AP-Passwort steht bewusst im Quelltext; kein zweites Weblogin. Der fehlende Geheimnischarakter ist bekannt und akzeptiert. Änderung per Weboberfläche bleibt geplant.

## 6. Schnittstellen von v0.2.0

| Methode/Pfad | Funktion |
| --- | --- |
| `GET /` | Weboberfläche aus Header |
| `GET /api/status` | Lauf-/Auftragsstatus |
| `GET /api/font` | Gemeinsame Glyphentabelle für Browser und Controller |
| `GET /api/diagnostics` | Diagnosestatus (JSON) |
| `GET /api/diagnostics.txt` | Vollständiger Diagnose-TXT-Export |
| `GET /api/program` | Aktuelle gespeicherte Seiten und Playerstatus |
| `POST /api/program/set` | Seite ändern und in NVS sichern |
| `POST /api/program/add` | Seite hinzufügen/duplizieren |
| `POST /api/program/delete` | Seite löschen |
| `POST /api/program/move` | Seite verschieben |
| `POST /api/program/show` | Seite einzeln übertragen |
| `POST /api/program/start` | ESP32-Seitenabfolge starten |
| `POST /api/program/stop` | Abfolge stoppen |
| `POST /api/text` | Lauftext/Standbild übertragen |
| `POST /api/brightness` | Helligkeit separat übertragen |

Alle genannten Schnittstellen sind **im korrigierten v0.2.0-Code vorhanden**. Dies bedeutet nicht, dass jede Fehlerbehandlung bereits hardwaregetestet wurde.

## 7. Historische Entwicklungsstufen

- **FirstText / v0.1.0:** Erster BLE-Text; eigene 5×7-Engschrift, ursprünglich Großbuchstaben und bei Überbreite automatischer Lauftext (Prototyp-Regel, später aufgegeben).
- **v0.1.1:** Diagnose im Browser und TXT-Export **auf Hardware bestätigt**. Log bewusst RAM-only. Erste Beobachtung von BLE-Konkurrenz mit Original-App und Wiederaufnahme nach Freigabe.
- **v0.1.2:** Unicode/Kleinbuchstaben, `GET /api/font`, 64×16-Punktvorschau, explizite Standbild-/Scrollwahl, Diagnose-Kopieren, Korrektur der App-Platz-Rechnung; nach erstem Typfehler kompilierte Korrekturfassung. Einzelne Glyphen sind unbefriedigend.
- **v0.2.0:** Separater Blockeditor, NVS-Seiten und Seitenplayer; erste `.ino` hatte JavaScript-/Arduino-Prototyp-Kollision, **Korrektur 1** trennt WebUI-Header ab; danach erfolgreich getestet.

## 8. Noch zu prüfen

Siehe [`ROADMAP.md`](ROADMAP.md) und [`THEMENSPEICHER.md`](THEMENSPEICHER.md): exakte Schriftkorrekturen (g/p/q/y, Ä/Ö/Ü, ß/ẞ), Mini stets in Versalien, Mini-einzeilig, Pixelüberlauf, Originalfonts Normal und Bold, Dauerlast/Heap, Live-Modus/SPEED/BRIGHTNESS, Animation `0x04`, native Blocktexte, bibliotheksfähiges Datei-/Objektmodell.

## 24.10.2026 · v0.3.0-Entwicklungszweig (nur Browser-/Quelltextprüfung)

- Weiterhin drei Sketch-Dateien im selben Verzeichnis: `.ino`, `RavestickBoard_WebUI.h`, `RavestickBoard_Assets.h`. Webquelltext wird per `tools/sync_web.py` synchronisiert.
- 82 Glyphen Normal + 82 Bold aus dem Archiv v02: pro Glyph Unicode-BMP `uint16` (LE), belegte Breite `uint8`, 16 Zeilen à `uint16` (LE). **35 Bytes pro Glyph**, 2 × 82 × 35 = 5.740 Bytes Pixel-/Metadaten.
- 24 rekonstruierte Originalsymbole sowie ein eigenes Bananensymbol. 16×16 Pixel mit 4-bit RGB-Code pro Pixel, **128 Bytes je Symbol**, 25 × 128 = 3.200 Bytes. 0=aus; RGB-Bits 4/2/1 für Rot/Grün/Blau.
- Sichtbare Symbolbreite = Differenz der äußersten tatsächlich beleuchteten Spalten +1; 1 Pixel Abstand vorläufig. **Original-App-Vorschub nicht abgeglichen.**
- Bei Emoji `🍌` wird Hersteller-Banane verwendet; `🍌︎` (U+1F34C + VS15) ist als separates Text-Token für die selbst gestaltete Ravestick-Banane reserviert. Browser/ESP32 interpretieren die Auswahl gleich. Das ist eine **projektspezifische** Unterscheidung, kein Unicode-Standard für Bananenarten.
- Farbcodierung des Herzens über ❤️ 💛 💚 🩵 💙 💜 🤍; C++ und JS verwenden denselben RGB-Farbcode. Die 16×16-Herzmaske wird umgefärbt.
- Layoutwerte in `PageRecord.layout`: 0 Engschrift 14px; 1 Mini zwei Zeilen; 2 Mini eine Zeile **horizontal und vertikal zentriert**; 3 Normal; 4 Bold. Das bestehende NVS-Schema 1 und die Recordgröße bleiben unverändert.
- `/api/speed` sendet `0x07`, `/api/mode` sendet `0x06`, `/api/brightness` bleibt `0x08`, jeweils **ohne vollständige Bilddaten**. BLE-Verbindung für Folgekommandos maximal ca. 2,5 Sekunden inaktiv behalten; Hersteller-App-Koexistenz muss getestet werden.
- `localStorage` sichert Browser-Entwurf und Reiter, **nicht** als plattformübergreifende Controllerbibliothek. NVS behält die gespeicherten Blockseiten.
- **Nicht implementiert:** gespeicherte `0x04`-Animationen, native Programme ohne Prozentanzeige, LittleFS-Bibliothek, pixelgenaue frei formatierte Zeichenabschnitte.
- **Sicherheit:** Kein reales Arduino-Kompilat/ESP32- oder LED-Tafel-Testprotokoll für v0.3.0 vorhanden. Änderungen nicht als hardwarebestätigt kennzeichnen.


## Technischer Nachtrag v0.3.2 (25.10.2026)

**Lauftext und BLE:** Controllerdatenpuffer `MAX_CHARS=600`, `MAX_COLUMNS=10.200`, RGB-Bilddaten bis 61.200 Bytes, Gesamtpayload bis 61.307 Bytes (einschließlich 107-Byte-Header). In v0.3.2 Korrektur 1 waren beide Felder statisch angelegt (122.507 Bytes dauerhaft). **Korrektur 2** allokiert dynamisch nur einen tatsächlich benötigten Payload-Puffer und rendert die RGB-Bilddaten direkt ab Byte 107. Freigabe erfolgt nach dem Übertragungsauftrag auch bei Fehlern. Für sehr lange Texte bleibt ausreichend zusammenhängender freier Heap eine Voraussetzung. Header-Formatplätze 25–104 reichen nur für 80 Zeichencodes, Feld 24 wird experimentell auf höchstens 80 gesetzt. Pixelmatrix und Paketanzahl können dennoch darüber hinausgehen. Das reale Boardverhalten ist ausdrücklich **unbekannt**. Bei Validierung werden UTF-8-Codepoints statt JS-UTF-16-Codeunits gezählt, die beiden Mini-Zeilen gemeinsam. Die Payloadlänge und Paketnummern sind 16-Bit-Werte.

**Layoutcodes (Speicherschema unverändert):** `0=Engschrift`, `1=Mini Zweizeilig`, `2=Mini Einzeilig`, `3=Standard`, `4=Standard Bold`. Für zweizeiligen Lauftext werden die beiden 7-Pixel-Zeilen auf eine gemeinsame Breite gerendert und synchron horizontal bewegt. Gleiche Farbe, keine neue Blockseitenstruktur.

**Webinterface:** v0.3.1 hatte im HTML des Symbol-Overlays einen fehlenden schließenden `</div>`-Tag. Dadurch wurden Status und Diagnose als weitere Flex-Kinder im Overlay angeordnet, was den schmalen, unlesbaren Dialog sowie überlagernde Statusfelder erklärt. In v0.3.2 ist die Hierarchie korrigiert und mit einem emulierten 375-Pixel-Mobilviewport auf eine 351-Pixel-Dialogbreite geprüft. Dies ist ein Browser-Smoketest, noch kein Test in mobilem Safari. Der Browserentwurf bleibt auf dem Endgerät in `localStorage`, NVS-Blockseiten auf dem ESP32.
