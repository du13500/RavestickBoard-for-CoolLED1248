# RavestickBoard · Technische Bestandsaufnahme

**Stand: 23.10.2026 · Firmwarebasis: v0.2.0 Korrektur 1 · Status: hardwaregetestet, mit bekannten Einschränkungen.**

Dieses Dokument unterscheidet **durch Nutzer am konkreten Board bestätigt**, **im Quellcode vorhanden** und **für spätere Versionen geplant**. Die Firmware des LED-Boards wird nicht verändert. Historische v0.1.x-Einschränkungen sind weiter unten separat aufgeführt.

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
- **`0x06` Anzeigemodus**: im Code vor Text-/Seitenübertragungen eingesetzt (`0x01` statisch, `0x02` Linkslauf). Weitere Modi wie Rechtslauf, Auf/Ab, Schneefall oder Laser sind als Protokollreferenz bekannt, aber **noch nicht alle auf unserer Tafel geprüft**. Ein eigenständiger Moduswechsel ohne Neu-Upload ist geplant.
- **`0x04` gespeicherte Animation**: OpenSign meldet autonome Animationen auf **einem anderen Referenzboard** (ca. 30 KiB bzw. 80 Frames). **Nicht in unserer v0.2.0-Firmware implementiert, auf unserer Hardware ungetestet.**

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
