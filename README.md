<p align="center"><img src="docs/assets/board-gif.gif" alt="CoolLEDX mit LED-Motiven" /></p>
<p align="center"><img src="docs/assets/totem.jpg" width="420" alt="RavestickBoard Festival-Totem mit Neonbanane" /></p>
<h1 align="center">🍌 RavestickBoard for CoolLEDX / CoolLED1248</h1>
<p align="center"><strong>Offline-Webcontroller für die 64 × 16-Pixel-CoolLEDX-Tafel mit ESP32-C3.</strong><br>
Entstanden aus einer Festival-Banane und dem Wunsch, mehr aus der ursprünglichen CoolLED1248-App herauszuholen.</p>
<p align="center">
<img alt="Version" src="https://img.shields.io/badge/version-v0.4.2.3--dev-7056d8">
<img alt="Hardware" src="https://img.shields.io/badge/hardware-ESP32--C3-007c91">
<img alt="Status" src="https://img.shields.io/badge/status-hardware%20test%20pending-f3b653">
<img alt="Lizenz" src="https://img.shields.io/badge/license-MIT-brightgreen">
<img alt="Mission" src="https://img.shields.io/badge/mission-Hugs%20and%20Love-ff69b4">
</p>

## Was ist RavestickBoard?

Das iPhone oder ein anderer Browser verbindet sich mit dem WLAN `CoolLED1248` des **ESP32-C3 Super Mini**. Das lokale Webinterface unter **http://192.168.4.1** überträgt die Darstellung via Bluetooth Low Energy (FFF0/FFF1) an die **CoolLEDX-Tafel**. Die LED-Tafel benötigt keine modifizierte Herstellerfirmware und kann einmal hochgeladene Inhalte autonom wiederholen. Die Bedienoberfläche läuft ohne Internetzugang; USB bleibt der einzige Firmware-Updateweg, **kein OTA**.

## Aktuell: v0.4.2.3-dev

**Aufbauend auf unseren erfolgreichen Hardwaretests mit v0.4.2.2-dev.** Wir
haben bestätigt, dass sich gemischte Programme jetzt tatsächlich zwischen
Lauftexten und stehenden Blockanzeigen bewegen. Die Übergänge sind bislang
noch nicht exakt synchron; auch den Geschwindigkeitsregler prüfen wir weiter.
Unsere Blockmessung bei Tempo 80 ergab für vier Seiten 6,33 Sekunden, sehr
nahe an der Prognose von 6,31 Sekunden.

- **Neue Programmwiedergabe „Alles als Laufband“:** In einem Programm können
  gespeicherte Texte und alle Seiten gespeicherter Blockfolgen als **ein
  horizontal scrollendes Pixelband** dargestellt werden. 16 leere Pixelspalten
  trennen die Teilinhalte, Farben und Schriftarten bleiben erhalten. Eine
  homogene Gruppe passt oft in **einen einzigen BLE-Upload** und wiederholt
  sich dann autonom. Bei Überschreiten des vorsichtigen 30-KiB-Budgets wird
  an vollständigen Text-/Seitengrenzen geteilt. Der bislang bewährte Modus
  mit stehenden Blöcken bleibt für existierende Programme Standard.
- **Keine Behauptung perfekter Synchronisation:** Geteilte Laufbänder und
  Mischprogramme mit stehenden Blöcken benötigen weiterhin zeitgesteuerte
  Uploadwechsel. Für Blockgruppen ist die Formel nur bei Tempo 60–90
  kalibriert; für Lauftexte fehlt uns die Messkurve. Unsere Ersatzzeit bleibt
  deshalb vorläufig im Programmeditor sichtbar.
- **Editor:** Nach dem Speichern erscheint ein direkter Button **„＋ Neu“**.
  Beim Verlassen ungespeicherter Arbeit wird weiterhin gefragt; bei
  unveränderten Inhalten geht es ohne Zwischenfrage zurück zur Bibliothek.
- **Kurzwahl:** Eine leere Kachel öffnet beim Antippen direkt den
  Einrichtungsdialog; ein gespeichertes, unvollständiges Programm ist mit
  rötlicher Kachel und Reparaturhinweis erkennbar.
- **Backup zusammenführen:** Neue Importlogik vergleicht **Name und Inhalt**,
  verwendet bereits identische Bibliothekseinträge/Programme wieder und
  ordnet abhängige IDs neu zu. Ein echtes, überschreibendes
  Wiederherstellen ist weiterhin bewusst nicht enthalten. Vor Testimporten
  eine separate Sicherung behalten; Teilimporte können bei Fehlern bestehen bleiben.
- **Offline-Lucide-Icons:** Unser ausgewählter Iconplan wurde auf die
  bestehenden Schaltflächen übertragen. Acht kleine 4×2-Symbole am
  Seitenende senden jeden erzwungenen BLE-Modusbefehl **bei jedem Klick**,
  ohne einen vermeintlichen aktiven Modus zu markieren. Die SVGs sind lokal
  eingebettet. Die Lucide-/Feather-Lizenzhinweise stehen unter
  [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) und sind in der
  [LICENSE](LICENSE) ausdrücklich referenziert.
- **Bestehende Daten:** Wir behalten die RBL1/RBB2/RBP2/RBS2-Dateien und
  deren bisheriges Layout; das bislang reservierte Byte der Programmdaten
  speichert die neue Abspielart. Es erfolgt **keine LittleFS-Formatierung**.

**Reifegrad:** Lokale Quell- und Browsertests sowie frühere Renderer-Vergleiche
ersetzen keinen Arduino-Build und keinen echten Test des neuen Laufbandrenders
auf der CoolLEDX. Diese neue Version ist deshalb ausdrücklich `-dev`.

## Installation und Datensicherheit

1. Vorab ein JSON-Backup aus der aktuell laufenden **v0.4.2.2-dev** erstellen und lokal aufbewahren. Die bisherige Firmware als Rückfallversion sichern.
2. `firmware/RavestickBoard_v0_4_2_3/RavestickBoard_v0_4_2_3.ino` öffnen. **Alle sechs Dateien** im selben Sketch-Ordner belassen: `.ino`, `RavestickBoard_WebUI.h`, `RavestickBoard_Assets.h`, `RavestickBoard_Library.h`, `RavestickBoard_v042.h` und `RavestickBoard_Icon.h` (insgesamt **sechs Dateien**).
3. Arduino IDE: **ESP32C3 Dev Module**, Espressif **3.3.12**, **USB CDC On Boot: Enabled**, **No OTA (2MB APP/2MB SPIFFS)**. Bestehendes Partitionsschema **nicht ändern**, **Erase All Flash deaktiviert** lassen.
4. Nach dem Flashen mit WLAN `CoolLED1248` verbinden, `http://192.168.4.1` aufrufen. Bibliothek nicht neu formatieren. Testplan schrittweise abarbeiten. Falls das Homescreen-Icon nicht übernommen wird, die Verknüpfung über Safari erneut hinzufügen.

Siehe [ausführliche Installation](docs/INSTALLATION_v0.4.2.3.md), [Testplan](docs/TESTPLAN_v0.4.2.3.md), [Bibliotheksformat](docs/BIBLIOTHEK_v0.4.2.md), [Changelog](CHANGELOG.md), [Roadmap](docs/ROADMAP.md) und [Themenspeicher](docs/THEMENSPEICHER.md).

## Wie wir entwickeln

Wir entwickeln RavestickBoard gemeinsam: Wir entwerfen, programmieren, dokumentieren und testen die Software im engen Austausch. Die realen ESP32-/CoolLEDX-Hardwaretests führen wir vor Ort durch; dabei unterstützen KI-gestützte Werkzeuge wie **ChatGPT und Claude** die Konzeption, Fehlersuche, Implementierung und Dokumentation. Wir unterscheiden ausdrücklich zwischen lokal geprüftem Code und hardwarebestätigten Funktionen.

## Entwicklung und Quellen

RavestickBoard ist eine eigenständige C++-/HTML-/JavaScript-Implementierung. Die beiden aktuellen Referenzen für zukünftige Animationsforschung sind [jean-santos/coolled1248-rs](https://github.com/jean-santos/coolled1248-rs) und [zackmcmurrin-dev/coolledux-linux-uploader](https://github.com/zackmcmurrin-dev/coolledux-linux-uploader). Ergänzend dokumentieren wir den früh untersuchten [UpDryTwist/coolledx-driver](https://github.com/UpDryTwist/coolledx-driver) und [ohtaras/led](https://github.com/ohtaras/led). Die technischen Protokollfakten wurden für unsere Firmware eigenständig umgesetzt. Ob eine Datenformatvariante des anderen Panels auf unserer eigenen CoolLEDX funktioniert, ist separat zu prüfen.

**Lizenz und Drittmaterialien:** Unser selbst entwickelter RavestickBoard-Code steht unter der **[MIT-Lizenz](LICENSE)** (Copyright © 2026 du13500). Die Lizenz erlaubt auch kommerzielle Nutzung, Bearbeitung und Weitergabe unter ihren Bedingungen. RavestickBoard ist derzeit ein privates Hobbyprojekt, ohne die MIT-Rechte einzuschränken. Für eingebettete Lucide-Icons gelten ISC und für ausgewählte Feather-Ableitungen zusätzlich MIT; siehe [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md). Fremde Originalmotive, Marken und sonstige Drittmaterialien werden durch unsere MIT-Lizenz nicht automatisch mitlizenziert.

<p align="center"><strong>Made for the rave, powered by a banana. 🍌</strong><br><sub>Independent project, not affiliated with the CoolLED1248/CoolLEDX manufacturer.</sub></p>
