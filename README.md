<p align="center"><img src="docs/assets/board-gif.gif" alt="CoolLEDX mit LED-Motiven" /></p>
<p align="center"><img src="docs/assets/totem.jpg" width="420" alt="RavestickBoard Festival-Totem mit Neonbanane" /></p>
<h1 align="center">🍌 RavestickBoard for CoolLEDX / CoolLED1248</h1>
<p align="center"><strong>Offline-Webcontroller für die 64 × 16-Pixel-CoolLEDX-Tafel mit ESP32-C3.</strong><br>
Entstanden aus einer Festival-Banane und dem Wunsch, mehr aus der ursprünglichen CoolLED1248-App herauszuholen.</p>
<p align="center">
<img alt="Version" src="https://img.shields.io/badge/version-v0.4.2--dev-7056d8">
<img alt="Hardware" src="https://img.shields.io/badge/hardware-ESP32--C3-007c91">
<img alt="Status" src="https://img.shields.io/badge/status-hardware%20test%20pending-f3b653">
<img alt="Lizenz" src="https://img.shields.io/badge/code%20license-MIT-2e8b71">
<img alt="Mission" src="https://img.shields.io/badge/mission-Hugs%20and%20Love-ff69b4">
</p>

## Was ist RavestickBoard?

Das iPhone oder ein anderer Browser verbindet sich mit dem WLAN `CoolLED1248` des **ESP32-C3 Super Mini**. Das lokale Webinterface unter **http://192.168.4.1** überträgt die Darstellung via Bluetooth Low Energy (FFF0/FFF1) an die **CoolLEDX-Tafel**. Die LED-Tafel benötigt keine modifizierte Herstellerfirmware und kann einmal hochgeladene Inhalte autonom wiederholen. Die Bedienoberfläche läuft ohne Internetzugang; USB bleibt der einzige Firmware-Updateweg, **kein OTA**.

## Aktuell: v0.4.2-dev

**Entwicklungs- und Testkandidat, noch nicht mit Arduino-ESP32 3.3.12 auf echter Hardware kompiliert oder abgenommen.** Die vorherige v0.4.1 wurde vom Nutzer auf dem ESP32 installiert; LittleFS, gespeicherte Texte und Kurzwahlen funktionierten in ersten Praxistests. Der 384-Byte-Tile-Renderer aus v0.4.0.3 wurde zuvor mehrfach mit langen Übertragungen und einem achtstündigen Betrieb überprüft. Ein einmalig gemessener extrem niedriger historischer Heap-Wert bleibt zur Untersuchung offen.

### Neue Funktionen

- **Blockeditor mit 25 Seiten.** Neue Blockfolgen beginnen leer. Jede 64 × 16-Pixel-Seite erhält ihre eigene Schrift, Farben und Ausrichtung. Einzelne Seiten können geleert, kopiert, sortiert und gelöscht werden. Lauftexte haben eine eigene Aktion „Text leeren“, jeweils mit Bestätigung.
- **Blockfolgen in LittleFS.** Ganze Seitenfolgen werden als **ein Bibliothekseintrag** gespeichert, bleiben mit ihren ursprünglichen Seitendaten editierbar und werden erst beim Senden als Pixelstreifen gerendert. Der bereits vom Nutzer beobachtete Standbildmodus (`0x06 0x01`) blättert entlang der 64-Pixel-Grenzen, passt die Blättergeschwindigkeit an und wiederholt die Folge auf der Tafel. Das pixelgenaue Ergebnis unseres neuen Renderers muss auf Hardware geprüft werden.
- **Programme.** Bis zu 24 gespeicherte Programme mit bis zu 24 Bibliotheksschritten. Benachbarte Blockfolgen werden bis zum Datenbudget (höchstens 79 Seiten pro Upload) zusammengefügt; benachbarte kompatible Lauftexte können ein gemeinsames Banner bilden. Bei gemischten oder überlangen Gruppen sind erneute BLE-Uploads und deren Prozentanzeige weiterhin vorgesehen. Gruppenlaufzeit 5 bis 180 Sekunden.
- **Kurzwahlen.** Sechs direkt auf der Startseite bearbeitbare Plätze mit frei wählbarer Beschriftung. Quelle: Bibliothek, gespeichertes Programm oder eine der sieben Ambiente-Farben. Animationen sind bewusst nur als Zukunftsoption vorgemerkt. v0.4.1-Zuordnungen werden beim ersten Start übernommen.
- **Bibliotheksverwaltung.** Bestehende `RBL1`-Textdateien bleiben lesbar. Der v0.4.1-Metadatenindex (64 Plätze) wird beim Lesen kompatibel in einen Index mit **bis zu 192 Plätzen** überführt. Tatsächliche weitere Einträge hängen zusätzlich vom freien LittleFS-Platz ab; mindestens 64 KiB sollen frei bleiben. Dies ist noch kein unbegrenzter dynamischer Index.
- **Kompakteres Webinterface.** Bibliothekssuche hinter der Lupe, „Bearbeiten“ statt „Öffnen“, Kurzwahlverwaltung aus der Bibliothek entfernt, Backup/Import unter Einstellungen, Menüaktion zum Neuladen der iPhone-Homescreen-Web-App. `docs/assets/ravestick-icon.png` wird als Apple-Touch-Icon (180 × 180) mit der Firmware ausgeliefert.
- **JSON-Backup Schema 2** enthält Texte, strukturierte Blockfolgen, gespeicherte Programme und Typ/Beschriftung der Kurzwahlen. Schema-1-Import aus v0.4.1 wird weiterhin angeboten. Import ist ergänzend, nicht transaktional; vorhandene Einträge werden nicht ersetzt.

### Noch nicht gelöst

- Die native Animation über Opcode `0x04`, GIF-Upload und variable 16 Pixel hohe, über 16 Pixel breite Symbole sind **nicht** Teil von v0.4.2.
- Ein Gruppenwechsel zwischen unterschiedlichen Anzeigearten benötigt einen neuen Upload. Der alte ESP32-NVS-Seitenplayer bleibt als historischer Testmodus sichtbar, ist aber **nicht** der neue Blockprogramm-Player.
- Das Anlegen von 25 Seiten kann die früheren **12 NVS-Testseiten** zurücksetzen. Das war ausdrücklich vereinbart, ist jedoch **kein** Löschen vorhandener LittleFS-Bibliothekseinträge.
- Im Programm-Player kontrolliert eine feste Gruppenlaufzeit den ESP32-Wechsel. Eine Echtzeit-/Frame-genaue Zeitsteuerung zwischen verschiedenen Gruppen ist noch nicht umgesetzt.
- Browser- und Hosttests ersetzen keine echte Arduino-Kompilierung und keinen BLE-Gerätetest.

## Installation und Datensicherheit

1. Vorab ein JSON-Backup aus **v0.4.1** erstellen und lokal aufbewahren. Die v0.4.1-Firmware als Rückfallversion sichern.
2. `firmware/RavestickBoard_v0_4_2/RavestickBoard_v0_4_2.ino` öffnen. **Alle sechs Dateien** im selben Sketch-Ordner belassen: `.ino`, `RavestickBoard_WebUI.h`, `RavestickBoard_Assets.h`, `RavestickBoard_Library.h`, `RavestickBoard_v042.h` und `RavestickBoard_Icon.h` (insgesamt **sechs Dateien**).
3. Arduino IDE: **ESP32C3 Dev Module**, Espressif **3.3.12**, **USB CDC On Boot: Enabled**, **No OTA (2MB APP/2MB SPIFFS)**. Bestehendes Partitionsschema **nicht ändern**, **Erase All Flash deaktiviert** lassen.
4. Nach dem Flashen mit WLAN `CoolLED1248` verbinden, `http://192.168.4.1` aufrufen. Bibliothek nicht neu formatieren. Testplan schrittweise abarbeiten. Falls das Homescreen-Icon nicht übernommen wird, die Verknüpfung über Safari erneut hinzufügen.

Siehe [ausführliche Installation](docs/INSTALLATION_v0.4.2.md), [Testplan](docs/TESTPLAN_v0.4.2.md), [Bibliotheksformat](docs/BIBLIOTHEK_v0.4.2.md), [Changelog](CHANGELOG.md), [Roadmap](docs/ROADMAP.md) und [Themenspeicher](docs/THEMENSPEICHER.md).

## Entwicklung und Quellen

RavestickBoard ist eine eigenständige C++-/HTML-/JavaScript-Implementierung. Die beiden aktuellen Referenzen für zukünftige Animationsforschung sind [jean-santos/coolled1248-rs](https://github.com/jean-santos/coolled1248-rs) und [zackmcmurrin-dev/coolledux-linux-uploader](https://github.com/zackmcmurrin-dev/coolledux-linux-uploader). Ergänzend dokumentieren wir den früh untersuchten [UpDryTwist/coolledx-driver](https://github.com/UpDryTwist/coolledx-driver) und [ohtaras/led](https://github.com/ohtaras/led). Die technischen Protokollfakten wurden für unsere Firmware eigenständig umgesetzt. Ob eine Datenformatvariante des anderen Panels auf unserer eigenen CoolLEDX funktioniert, ist separat zu prüfen.

**Lizenz:** Eigenständiger Programmcode und eigener Dokumentationstext stehen unter der [MIT-Lizenz](LICENSE). Herstellerschriften, Original-Pixelmotive, Marken und sonstige Drittmaterialien sind **nicht automatisch von dieser Lizenz erfasst**. Details: [Quellen und Lizenzen](docs/QUELLEN_UND_LIZENZEN.md). Privat genutztes, nichtkommerzielles Hobbyprojekt.

<p align="center"><strong>Made for the rave, powered by a banana. 🍌</strong><br><sub>Independent project, not affiliated with the CoolLED1248/CoolLEDX manufacturer.</sub></p>
