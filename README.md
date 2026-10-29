<p align="center"><img src="docs/assets/board-gif.gif" alt="CoolLEDX mit LED-Motiven" /></p>
<p align="center"><img src="docs/assets/totem.jpg" width="420" alt="RavestickBoard Festival-Totem mit Neonbanane" /></p>
<h1 align="center">🍌 RavestickBoard for CoolLEDX / CoolLED1248</h1>
<p align="center"><strong>Offline-Webcontroller für die 64 × 16-Pixel-CoolLEDX-Tafel mit ESP32-C3.</strong><br>
Entstanden aus einer Festival-Banane und dem Wunsch, mehr aus der ursprünglichen CoolLED1248-App herauszuholen.</p>
<p align="center">
<img alt="Version" src="https://img.shields.io/badge/version-v0.4.2.4--dev-7056d8">
<img alt="Hardware" src="https://img.shields.io/badge/hardware-ESP32--C3-007c91">
<img alt="Status" src="https://img.shields.io/badge/status-dev%20hardware%20test%20pending-f3b653">
<img alt="Lizenz" src="https://img.shields.io/badge/license-MIT-brightgreen">
<img alt="Mission" src="https://img.shields.io/badge/mission-hugs%20and%20love-ff69b4">
</p>

## Was ist RavestickBoard?

Das iPhone oder ein anderer Browser verbindet sich mit dem WLAN `CoolLED1248` des **ESP32-C3 Super Mini**. Das lokale Webinterface unter **http://192.168.4.1** überträgt die Darstellung via Bluetooth Low Energy (FFF0/FFF1) an die **CoolLEDX-Tafel**. Die LED-Tafel benötigt keine modifizierte Herstellerfirmware und kann einmal hochgeladene Inhalte autonom wiederholen. Die Bedienoberfläche läuft ohne Internetzugang; USB bleibt der einzige Firmware-Updateweg, **kein OTA**.

## Aktuell: v0.4.2.4-dev

**Unsere Hardwaretests vom 28.10.2026:** Die v0.4.2.3 wurde erfolgreich auf dem ESP32-C3 kompiliert und geflasht. Lauftexte, Symbole, verschiedenfarbige Schriften und Blockseiten werden im Modus **„Alles als Laufband“** korrekt verkettet und angezeigt. Die Tafel scrollt ein passendes, vollständiges Band nach einem Upload autonom. Größere Laufbänder wurden in mehrere Gruppen zerlegt; die alte **20-Sekunden-Ersatzdauer** schnitt Inhalte vorzeitig ab. Bei einem späteren Upload wurde **Paket 70/106 nicht bestätigt** und die Tafel blieb im Hersteller-Fehlerbildschirm. Der ESP32-Heap war dabei knapp, aber ein RAM-Absturz wurde nicht nachgewiesen. Siehe [Testbefund](docs/TESTBEFUNDE_v0.4.2.3.md).

Für **v0.4.2.4-dev** haben wir gezielt nachgebessert:

- **Stabilität:** Auch gemischte Laufbänder verwenden beim Senden einen kleinen **384-Byte-Tile-Puffer**, keinen vollständigen 30-KiB-Bildpuffer. Nach einer nicht bestätigten Bildpaketübertragung wird die ganze Gruppe höchstens einmal nach BLE-Neuverbindung neu gesendet. Scheitert auch der Versuch, stoppt der Programmablauf mit Fehlerstatus.
- **Zeitsteuerung:** Die alte einstellbare Gruppendauer (5–180 s) entfällt aus dem Programmeditor und bestimmt die Wiedergabe nicht länger. Die Hardwareformel für Blockseiten bei Tempo 60–90 bleibt bestehen. Geteilte Laufbänder erhalten eine **vorsichtige Schätzung** aus Spaltenzahl und Tempo. Das ist noch keine kalibrierte Laufzeit und kann bewusst zu längeren Wiederholungen führen – vollständige Wiedergabe hat Vorrang vor einem vorzeitigen Gruppenwechsel.
- **Bedienung:** Eine gemeinsame, am unteren Rand angeheftete Tafelsteuerung statt mehrfacher Regler. Sie folgt in Größe und Anordnung unseren MS-Paint-Mockups; Kurzwahlkacheln sind kompakter. Acht Lucide-Direkttasten senden bei **jedem** Antippen den BLE-Befehl erneut, ohne vermeintlich aktiven Modus.
- **LED-Vorschau:** Kleine Lucide-Buttons ohne Textbeschriftung: Anfang, Play/Pause, Vorschaugeschwindigkeit **Schnecke (1×) → Vogel (2×) → Hase (4×)**. Die dynamisch wechselnden Icons bleiben erhalten, die unnötige Beschriftung „Lauftextvorschau“ entfällt.
- **Menü und Editor:** Menüüberschrift „Navigation“ entfällt, Diagnose verwendet Lucide `bug`. Technische WLAN-/Speicherhinweise werden nicht mehr im Editor angezeigt, sondern bleiben in Diagnose/Dokumentation.
- **Repository:** Roadmap und Themenspeicher wurden mit den bestätigten Tests abgeglichen. Neue Community-Dateien (`CONTRIBUTING.md`, `SECURITY.md`, Issue-/PR-Vorlagen, Verhaltenskodex) erleichtern Rückmeldungen. Unser Quellcode bleibt unter **MIT**; die getrennten Lucide/Feather-Rechte stehen in `THIRD_PARTY_NOTICES.md` und sind in `LICENSE` referenziert.

**Wichtig:** v0.4.2.4-dev ist hier noch **nicht auf der echten Tafel abgenommen**; lokale Browser-/Quelltests ersetzen weder Arduino-Kompilierung noch Hardware-, RAM- und BLE-Tests. Speichergrößen und Runtime-Timing für geteilte Gruppen müssen wir erneut prüfen.

## Installation und Datensicherheit

1. Vorab ein JSON-Backup aus der aktuell laufenden **v0.4.2.3-dev** erstellen und lokal aufbewahren. Die bisherige Firmware als Rückfallversion sichern.
2. `firmware/RavestickBoard_v0_4_2_4/RavestickBoard_v0_4_2_4.ino` öffnen. **Alle sechs Dateien** im selben Sketch-Ordner belassen: `.ino`, `RavestickBoard_WebUI.h`, `RavestickBoard_Assets.h`, `RavestickBoard_Library.h`, `RavestickBoard_v042.h` und `RavestickBoard_Icon.h` (insgesamt **sechs Dateien**).
3. Arduino IDE: **ESP32C3 Dev Module**, Espressif **3.3.12**, **USB CDC On Boot: Enabled**, **No OTA (2MB APP/2MB SPIFFS)**. Bestehendes Partitionsschema **nicht ändern**, **Erase All Flash deaktiviert** lassen.
4. Nach dem Flashen mit WLAN `CoolLED1248` verbinden, `http://192.168.4.1` aufrufen. Bibliothek nicht neu formatieren. Testplan schrittweise abarbeiten. Falls das Homescreen-Icon nicht übernommen wird, die Verknüpfung über Safari erneut hinzufügen.

Siehe [ausführliche Installation](docs/INSTALLATION_v0.4.2.4.md), [Testplan](docs/TESTPLAN_v0.4.2.4.md), [Bibliotheksformat](docs/BIBLIOTHEK_v0.4.2.md), [Changelog](CHANGELOG.md), [Roadmap](docs/ROADMAP.md) und [Themenspeicher](docs/THEMENSPEICHER.md).

## Mitmachen und Sicherheit

Siehe [CONTRIBUTING.md](CONTRIBUTING.md), [SECURITY.md](SECURITY.md), [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md) und unsere [Issue-Vorlagen](.github/ISSUE_TEMPLATE/).

## Wie wir entwickeln

Wir entwickeln RavestickBoard gemeinsam: Wir entwerfen, programmieren, dokumentieren und testen die Software im engen Austausch. Die realen ESP32-/CoolLEDX-Hardwaretests führen wir vor Ort durch; dabei unterstützen KI-gestützte Werkzeuge wie **ChatGPT und Claude** die Konzeption, Fehlersuche, Implementierung und Dokumentation. Wir unterscheiden ausdrücklich zwischen lokal geprüftem Code und hardwarebestätigten Funktionen.

## Entwicklung und Quellen

RavestickBoard ist eine eigenständige C++-/HTML-/JavaScript-Implementierung. Die beiden aktuellen Referenzen für zukünftige Animationsforschung sind [jean-santos/coolled1248-rs](https://github.com/jean-santos/coolled1248-rs) und [zackmcmurrin-dev/coolledux-linux-uploader](https://github.com/zackmcmurrin-dev/coolledux-linux-uploader). Ergänzend dokumentieren wir den früh untersuchten [UpDryTwist/coolledx-driver](https://github.com/UpDryTwist/coolledx-driver) und [ohtaras/led](https://github.com/ohtaras/led). Die technischen Protokollfakten wurden für unsere Firmware eigenständig umgesetzt. Ob eine Datenformatvariante des anderen Panels auf unserer eigenen CoolLEDX funktioniert, ist separat zu prüfen.

**Lizenz und Drittmaterialien:** Unser selbst entwickelter RavestickBoard-Code steht unter der **[MIT-Lizenz](LICENSE)** (Copyright © 2026 du13500). Die Lizenz erlaubt ausdrücklich auch kommerzielle Nutzung, Bearbeitung und Weitergabe unter ihren Bedingungen. Das Projekt wird derzeit als privates Hobbyprojekt entwickelt; das schränkt die MIT-Rechte nicht ein. Eingebettete **Lucide-Icons** stehen unter ISC, ausgewählte von Feather abgeleitete Icons zusätzlich unter MIT. Ihre erforderlichen Copyright- und Lizenzhinweise stehen in [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md). Fremde Originalmotive, Marken und sonstige Drittmaterialien werden durch unsere MIT-Lizenz nicht automatisch mitlizenziert.

<p align="center"><strong>Made for the rave, powered by a banana. 🍌</strong><br><sub>Independent project, not affiliated with the CoolLED1248/CoolLEDX manufacturer.</sub></p>
