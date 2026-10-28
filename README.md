<p align="center"><img src="docs/assets/board-gif.gif" alt="CoolLEDX mit LED-Motiven" /></p>
<p align="center"><img src="docs/assets/totem.jpg" width="420" alt="RavestickBoard Festival-Totem mit Neonbanane" /></p>
<h1 align="center">🍌 RavestickBoard for CoolLEDX / CoolLED1248</h1>
<p align="center"><strong>Offline-Webcontroller für die 64 × 16-Pixel-CoolLEDX-Tafel mit ESP32-C3.</strong><br>
Entstanden aus einer Festival-Banane und dem Wunsch, mehr aus der ursprünglichen CoolLED1248-App herauszuholen.</p>
<p align="center">
<img alt="Version" src="https://img.shields.io/badge/version-v0.4.2.2--dev-7056d8">
<img alt="Hardware" src="https://img.shields.io/badge/hardware-ESP32--C3-007c91">
<img alt="Status" src="https://img.shields.io/badge/status-hardware%20validation%20pending-f3b653">
<img alt="Lizenz" src="https://img.shields.io/badge/code%20license-MIT-2e8b71">
<img alt="Mission" src="https://img.shields.io/badge/mission-hugs%20and%20love-ff69b4">
</p>

## Was ist RavestickBoard?

Das iPhone oder ein anderer Browser verbindet sich mit dem WLAN `CoolLED1248` des **ESP32-C3 Super Mini**. Das lokale Webinterface unter **http://192.168.4.1** überträgt die Darstellung via Bluetooth Low Energy (FFF0/FFF1) an die **CoolLEDX-Tafel**. Die LED-Tafel benötigt keine modifizierte Herstellerfirmware und kann einmal hochgeladene Inhalte autonom wiederholen. Die Bedienoberfläche läuft ohne Internetzugang; USB bleibt der einzige Firmware-Updateweg, **kein OTA**.

## Aktuell: v0.4.2.2-dev

**Nächste Korrekturversion nach erfolgreichen Hardwaretests der v0.4.2.1.** Der Nutzer hat bestätigt, dass sowohl ein gespeichertes Programm mit Lauftexten als auch gespeicherte Blockfolgen tatsächlich auf der LED-Tafel laufen. **Gemischte Programme hatten dagegen einen echten Fehler:** Die bereits vorhandene Funktion für den Wechsel zur nächsten Gruppe wurde in `loop()` nie aufgerufen. Diese Version bindet sie ein.

- **Gemischter Programmplayer:** Lauftexte und Blockgruppen können nacheinander laufen. Benachbarte kompatible Lauftexte werden gebündelt, benachbarte Blockfolgen zu einem Pixelband verkettet. **Reine Programme mit nur einer Übertragungsgruppe laufen ohne weitere ESP32-Uploads endlos auf der Tafel.** Für verschiedene Gruppen sind weitere Uploads einschließlich der boardseitigen Prozentanzeige technisch erforderlich.
- **Zeitsteuerung:** Bei Blockgruppen und festem Tempo **60 bis 90** wird nach dem abgeschlossenen Upload die Hardwarekalibrierung `Zeit ≈ Seiten × 0,0795 s × (100 − Tempo)` eingesetzt. Gemessen: vier Seiten bei Tempo 60/70/80/90 und sechs Seiten bei Tempo 80. Außerhalb des geprüften Bereichs und für Lauftextgruppen gilt noch die einstellbare Ersatzdauer von **5–180 Sekunden**. **Die Tafel meldet uns keinen exakten Beginn bzw. Abschluss eines Anzeigezyklus**; Übergänge sind Annäherungen, keine garantierte Bildsynchronisation. Das Tempo wird während eines laufenden gemischten Programms gegen Änderungen gesperrt.
- **Editor:** Beim Bearbeiten eines Bibliothekseintrags erscheint ein optisch eigenständiger Modus; der Tabwechsel ist gesperrt, und Änderungen werden unter der vorhandenen **ID aktualisiert**, nicht als neue Vorlage angelegt. Der direkte Button **„← Bibliothek“** fragt nur bei tatsächlich ungespeicherten Änderungen nach. Die angeheftete LED-Vorschau bleibt unterhalb des Headers.
- **Bibliothek und Abhängigkeiten:** Verwendete Einträge dürfen nach Warnung gelöscht werden. Direkte Kurzwahlen werden vollständig geleert; Programme mit fehlenden Einträgen **bleiben gespeichert und sichtbar**, sind rot markiert, können im Editor repariert werden und sind bis dahin nicht startbar. Ein laufendes betroffendes Programm wird gestoppt.
- **Statusmeldungen:** Wiederholte unzutreffende „ESP32 nicht erreichbar“-Toasts und normale Status-Poll-Meldungen entfallen. Ein dezenter Verbindungsstatus erscheint erst nach wiederholten Fehlern über mehr als 30 Sekunden. Fehler- und Erfolgsmeldungen verschwinden automatisch; Diagnoseereignisse bleiben erhalten.
- **Kompatibilität:** Bestehende Bibliothekseinträge (`RBL1`), Blockfolgen (`RBB2`), Programme (`RBP2`) und Kurzwahlen (`RBS2`) werden **ohne Formatmigration** weiterverwendet. Die bewährte BLE-Blockband- und Streaming-Übertragung ist erhalten geblieben.

**Reifegrad:** Der neue Stand wurde im Browser mit einem simulierten ESP32 getestet; die 135 alten Renderervergleiche und der LittleFS-Mocktest bestehen. **Ein vollständiger Arduino-ESP32-Build und der Hardwaretest von v0.4.2.2-dev stehen aus.** Details: [Testplan](docs/TESTPLAN_v0.4.2.2.md), [Entwicklungsstand](docs/ENTWICKLUNGSSTAND_v0.4.2.2.md), [Roadmap](docs/ROADMAP.md) und [Themenspeicher](docs/THEMENSPEICHER.md).

## Installation und Datensicherheit

1. Vorab ein JSON-Backup aus der aktuell laufenden **v0.4.2.1-dev** erstellen und lokal aufbewahren. Die bisherige Firmware als Rückfallversion sichern.
2. `firmware/RavestickBoard_v0_4_2_2/RavestickBoard_v0_4_2_2.ino` öffnen. **Alle sechs Dateien** im selben Sketch-Ordner belassen: `.ino`, `RavestickBoard_WebUI.h`, `RavestickBoard_Assets.h`, `RavestickBoard_Library.h`, `RavestickBoard_v042.h` und `RavestickBoard_Icon.h` (insgesamt **sechs Dateien**).
3. Arduino IDE: **ESP32C3 Dev Module**, Espressif **3.3.12**, **USB CDC On Boot: Enabled**, **No OTA (2MB APP/2MB SPIFFS)**. Bestehendes Partitionsschema **nicht ändern**, **Erase All Flash deaktiviert** lassen.
4. Nach dem Flashen mit WLAN `CoolLED1248` verbinden, `http://192.168.4.1` aufrufen. Bibliothek nicht neu formatieren. Testplan schrittweise abarbeiten. Falls das Homescreen-Icon nicht übernommen wird, die Verknüpfung über Safari erneut hinzufügen.

Siehe [ausführliche Installation](docs/INSTALLATION_v0.4.2.2.md), [Testplan](docs/TESTPLAN_v0.4.2.2.md), [Bibliotheksformat](docs/BIBLIOTHEK_v0.4.2.md), [Changelog](CHANGELOG.md), [Roadmap](docs/ROADMAP.md) und [Themenspeicher](docs/THEMENSPEICHER.md).

## Entwicklung und Quellen

RavestickBoard ist eine eigenständige C++-/HTML-/JavaScript-Implementierung. Die beiden aktuellen Referenzen für zukünftige Animationsforschung sind [jean-santos/coolled1248-rs](https://github.com/jean-santos/coolled1248-rs) und [zackmcmurrin-dev/coolledux-linux-uploader](https://github.com/zackmcmurrin-dev/coolledux-linux-uploader). Ergänzend dokumentieren wir den früh untersuchten [UpDryTwist/coolledx-driver](https://github.com/UpDryTwist/coolledx-driver) und [ohtaras/led](https://github.com/ohtaras/led). Die technischen Protokollfakten wurden für unsere Firmware eigenständig umgesetzt. Ob eine Datenformatvariante des anderen Panels auf unserer eigenen CoolLEDX funktioniert, ist separat zu prüfen.

**Lizenz:** Eigenständiger Programmcode und eigener Dokumentationstext stehen unter der [MIT-Lizenz](LICENSE). Herstellerschriften, Original-Pixelmotive, Marken und sonstige Drittmaterialien sind **nicht automatisch von dieser Lizenz erfasst**. Details: [Quellen und Lizenzen](docs/QUELLEN_UND_LIZENZEN.md). Privat genutztes, nichtkommerzielles Hobbyprojekt.

<p align="center"><strong>Made for the rave, powered by a banana. 🍌</strong><br><sub>Independent project, not affiliated with the CoolLED1248/CoolLEDX manufacturer.</sub></p>
