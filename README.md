# CoolLED1248 · BananaBoard

**Stand:** 22. Oktober 2026  
**Projektstatus:** funktionsfähiger Prototyp v0.1.0; weitere Funktionen geplant  
**Plattform:** TENSTAR ROBOT ESP32-C3 Super Mini, eigenes WLAN, BLE-Verbindung zur unveränderten LED-Tafel.

## Leitidee

Eine eigenständige, mobil bedienbare Alternative zur CoolLED1248-App für das Festival-Totem. Der ESP32 dient als WLAN-Webcontroller und steuert die Originalhardware über Bluetooth Low Energy (BLE). **Kein Eingriff in die LED-Tafel**, keine dauerhafte Telefonverbindung, keine OTA-Firmwareupdates. Bedienung offline über `http://192.168.4.1`.

Wir möchten die vorhandenen Stärken der alten App behalten und deren Einschränkungen überwinden: frei gestaltete Seiten, expliziter Lauftext oder statische Seiten, individuelle Zeichenfarben und Schriften, Vorlagen, Programme und Spezialanzeigen. Die alte App bleibt gestalterische Inspiration, kein unverändert zu kopierendes Design.

## Status auf einen Blick

| Bestandteil | Stand |
| --- | --- |
| ESP32-C3 über USB programmierbar | ✅ real getestet |
| CoolLEDX per BLE gefunden und verbunden | ✅ real getestet |
| BLE-Service `FFF0`, Characteristic `FFF1` | ✅ ausgelesen |
| Helligkeit per ESP32 verändert | ✅ real getestet |
| Eigenen mehrfarbigen Text direkt übertragen | ✅ real getestet |
| Eigenes WLAN mit Webinterface | ✅ real getestet |
| Text und Farbe über iPhone-Webinterface übertragen | ✅ real getestet |
| Engschrift mit Großbuchstaben | ✅ real getestet |
| Originalschriften Normal/Bold | ⬜ Rekonstruktion offen |
| Präzise 64×16-Punktvorschau | ⬜ offen |
| Lauftext/Seitenmodus frei wählen | ⬜ offen |
| Diagnose-TXT-Export im Webinterface | ⬜ offen |
| Bibliothek, Programme, Countdown, Scoreboard | ⬜ offen |

**Achtung:** Nicht alles, was kompiliert oder auf dem Display sichtbar war, wurde bereits dauerhaft auf Robustheit geprüft. Die detaillierten Statusangaben stehen in `docs/THEMENSPEICHER.md`.

## Wichtige Dokumente

- [`docs/THEMENSPEICHER.md`](docs/THEMENSPEICHER.md): vollständiger, abhackbarer Themen- und Aufgabenbestand mit IDs.
- [`docs/ROADMAP.md`](docs/ROADMAP.md): geplante Reihenfolge, keine endgültigen Liefertermine.
- [`CHANGELOG.md`](CHANGELOG.md): nachvollziehbare Änderungen und Testmeilensteine nach Version.
- [`docs/ENTSCHEIDUNGEN.md`](docs/ENTSCHEIDUNGEN.md): warum wir etwas so bauen.
- [`docs/TECHNIK.md`](docs/TECHNIK.md): Hardware, Schnittstellen, bekannte Einschränkungen.
- [`docs/TESTPLAN.md`](docs/TESTPLAN.md): welche Prüfungen vor einer neuen Version anstehen.
- [`docs/QUELLEN_UND_LIZENZEN.md`](docs/QUELLEN_UND_LIZENZEN.md): GitHub-Referenzen und Hinweise zur Veröffentlichung.

## Aktueller Entwicklungsstand

Der funktionierende Quellcode liegt unter `firmware/CoolLED1248_Web_v0_1_0.ino`. Zusätzlich ist der davor getestete Bluetooth-Textsketch archiviert. Die aktuelle v0.1.0 läuft als WLAN-Access-Point und Webserver auf dem ESP32-C3, mit Textfeld, einfarbiger Engschrift, 7 Farben, Helligkeitsregler und BLE-Upload.

**Bewusste v0.1.0-Einschränkungen:** nur Großbuchstaben (Kleinbuchstaben werden umgewandelt), Textlimit 40 Zeichen, Lauftext automatisch ab einer bestimmten Pixelbreite statt manuell gewählt, Vorschau dehnt sich bei Langtexten statt ein festes Displayfenster darzustellen. Diese Einschränkungen sind im Themenspeicher erfasst und **keine gewollten Eigenschaften des Endprodukts**.

## Firmware bauen

Arduino IDE 2.3.10; Espressif Arduino-ESP32 3.3.12; Board `ESP32C3 Dev Module`; `USB CDC On Boot: Enabled`; Partitionierung `No OTA (2MB APP/2MB SPIFFS)`; Upload über USB-C. Die Auswahl des Partitionsschemas ist relevant, da v0.1.0 unter der vorherigen 1,25-MiB-App-Partition nicht kompilierte.

Das Web-WLAN ist passwortgeschützt, aber **das aktuelle Passwort steht noch im Quellcode**. Vor einer öffentlichen Veröffentlichung oder Weitergabe unbedingt individuell ersetzen. Keine zusätzlichen Web-Logins geplant.

## Fortführung und Versionierung

1. Vor Codeänderungen den zuletzt funktionierenden Sketch **unverändert** archivieren.
2. Pro sachlicher Änderung die betreffende Themen-ID im Commit, Änderungsprotokoll oder in einer Notiz nennen (z. B. `UI-04`, `DIA-01`).
3. Versionsstand und Testergebnis in `CHANGELOG.md` festhalten: `Geändert`, `Getestet`, `Bekannte Fehler`.
4. Neue Ideen in `docs/THEMENSPEICHER.md` ergänzen, mit Status und Priorität.
5. Optional lokal Git verwenden, **ohne** schon etwas auf GitHub zu veröffentlichen. Eine öffentliche Lizenz erst nach dem Quellcode-Lizenzaudit auswählen.

## Woher stammt dieser Themenbestand?

Konsolidiert aus den vorhandenen Projektgesprächen (u. a. *Alternative Apps für LED-Tafel*, *Screenshots analysieren*, *Originalanzeigen sammeln* und dem Inbetriebnahme-/Firmwarechat) und den hier vorhandenen Testsketchen. Keine Behauptung, dass jede alte Bildreferenz schon vollständig übernommen wurde. Originalanzeigen und Fonts bleiben im dafür vorgesehenen separaten Chat als Referenzen.
