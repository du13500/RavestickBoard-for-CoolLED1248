<p align="center">
  <img src="docs/assets/board-gif.gif" alt="CoolLEDX-Tafel mit wechselnden LED-Motiven" />
</p>

<p align="center">
  <img src="docs/assets/totem.jpg" width="420" alt="Unsere Ravestick-Banane mit CoolLEDX-Tafel im Festival-Totem" />
</p>

<h1 align="center">🍌 RavestickBoard for CoolLED1248</h1>

<p align="center">
  <strong>Unsere freie, lokal betriebene Websteuerung für eine 64 × 16-CoolLEDX-Tafel.</strong><br>
  Entstanden aus einer Festival-Banane, einer ungewöhnlich liebgewonnenen Original-App und dem Wunsch, deutlich mehr aus 1.024 LEDs herauszuholen.
</p>

<p align="center">
  <img alt="Version" src="https://img.shields.io/badge/version-v0.4.0--dev-7056d8">
  <img alt="ESP32-C3" src="https://img.shields.io/badge/hardware-ESP32--C3-007c91">
  <img alt="Hardwarestatus" src="https://img.shields.io/badge/status-v0.4.0%20Testkandidat-f3b653">
  <img alt="Lizenz" src="https://img.shields.io/badge/code%20license-MIT-2e8b71">
  <img alt="Mission" src="https://img.shields.io/badge/mission-Hugs%20and%20Love-ff69b4">
</p>

<p align="center">
  <img src="docs/assets/webux-v0.4.0.png" width="340" alt="Mobile Vorschau des v0.4.0-Startbildschirms mit sechs Kurzwahltasten" />
</p>

> **Entwicklungsstand vom 26. Oktober 2026:** Wir bauen v0.4.0 als **neues WebUX-Fundament** auf der zuletzt erfolgreich hardwaregetesteten **v0.3.5.1**. Der v0.4.0-Code ist ein **Testkandidat**: Browserseitige Tests sind durchgeführt, Arduino-Kompilierung und Hardwaretest stehen aus. Die Bibliothek und die Verkettung sind vorbereitete Ansichten, noch keine neue Datenbank oder autonome Wiedergabe.

## Worum geht es?

Wir ersetzen die umständliche tägliche Bedienung der CoolLED1248-App durch eine eigene, mobilefreundliche Webanwendung. Unser **ESP32-C3 Super Mini** stellt ein WLAN bereit und überträgt LED-Inhalte über **Bluetooth Low Energy (BLE)** an eine vorhandene **CoolLEDX-Tafel mit 64 × 16 Pixeln**.

```text
Smartphone / Notebook
        │ WLAN „CoolLED1248“
        ▼
ESP32-C3 mit RavestickBoard-Webinterface
        │ BLE: FFF0/FFF1
        ▼
CoolLEDX LED-Tafel (64 × 16, sieben LED-Farben)
```

Die Hersteller-Firmware der Tafel bleibt unverändert. Für das lokale Steuern brauchen wir weder Cloud noch Internet. Das Projekt ist aus unserem Festival-Totem entstanden; wir entwickeln es offen weiter, dokumentieren auch Sackgassen und testen die Änderungen möglichst auf echter Hardware.

## Was funktioniert schon?

Die **zuletzt hardwarebestätigte Basis v0.3.5.1** bietet:

- **Texte übertragen:** linkslaufender Text mit datenbasierter Längenprüfung; die experimentell gesetzte Obergrenze beträgt 30 KiB Bilddaten. Im Hardwaretest der Vorgängerfassung wurden 650 Zeichen bei 30.689 Byte erfolgreich übertragen und dargestellt.
- **Schriften:** Original Normal, Original Bold, Engschrift, Mini Einzeilig und Mini Zweizeilig. Im zweizeiligen Lauftext können die beiden Zeilen verschiedene Farben haben.
- **Pixelgrafiken:** 36 rekonstruierte und selbst entwickelte 16×16-Symbole; die RGB-Bilder sind Teil des LED-Renders. Die Herzvarianten unterstützen sieben Farben.
- **Blockseiten:** Einzelne Seiten gestalten, speichern und manuell anzeigen; bis zu zwölf Seiten in der bestehenden NVS-Struktur. Eine einfache ESP32-gesteuerte Wiedergabe ist vorhanden und erfordert weiterhin neue BLE-Bildübertragungen zwischen den Seiten.
- **Live-Steuerung:** Helligkeit, Tempo, acht Anzeigeeffekte sowie Ein/Aus per BLE, ohne dafür jeweils ein neues Bild hochzuladen. Die tatsächlich sichtbare Wirkung wurde im Projekt getestet; BLE-Quittungen allein sind kein Bildnachweis.
- **Diagnose:** Status, BLE-Zähler, Fehlermeldungen und Ereignisse; TXT-Export ohne gespeicherte Texte, Passwörter oder Geräteadressen.

### Neu in v0.4.0-dev

| Bereich | Stand dieser Version | Was folgt später? |
| --- | --- | --- |
| Startseite | Sechs große Kurzwahl-Kacheln; Bearbeiten-Modus visuell vorbereitet; Helligkeit und Ein/Aus bedienbar | Echte Kachelbelegung aus der Bibliothek |
| Navigation | Bananen-Button öffnet das Menü; sieben getrennte Bereiche; Header auf jeder Seite | – |
| Bibliothek | Layout mit fester Aktionsleiste, Liste, globalen Reglern und Modusauswahl | Anlegen, Auswahl, Drag-and-drop, Löschen, Übertragen (**v0.4.1**) |
| Programme | Eigenständige Ansicht und Bedienstruktur | Referenzbasierte Verkettung und Größenprüfung (**v0.4.2**) |
| Texteditor | Vorhandene Lauftext- und Blockseitenfunktionen bleiben erreichbar | Individuelle Farbgebung innerhalb eines Textes (**v0.4.3**, geplant) |
| Symbol-Picker | Eine thematisch sortierte Palette; **echte 16×16-Bildvorschauen** statt Emoji-Platzhaltern; Öffnen mit Herz ♥ | Zusätzliche Motive aus der erweiterten Excel-Sammlung |
| Ambiente | Sieben große Farbtasten; jedes Tippen sendet ein vollständig einfarbiges 64×16-Bild | Hardwaretest der neuen Funktion |
| Diagnose | Eigene Seite statt Fußbereich im Editor | Weitere Diagnosewerkzeuge |
| Einstellungen | Eigene Seite, aktueller Geräteanschluss beschrieben | BLE-Suche und Wechsel zwischen Tafeln |

**Bewusst noch nicht implementiert:** Persistente Bibliothek, echte gespeicherte Kurzwahlbelegung, Drag-and-drop und neue Mehrseiten-Programme. Die vorhandene **NVS-Blockseitenverwaltung im Editor bleibt bestehen**; wir nennen sie nicht vorzeitig die neue Bibliothek.

## Unsere Bedienlogik

### Startbildschirm: unmittelbar steuern

Die sechs Kacheln sind in einem 2×3-Raster angeordnet. Die Seite ist für einen Smartphonebildschirm ohne vertikales Scrollen ausgelegt. Helligkeit und Ein/Aus sind direkt erreichbar. Im derzeitigen Entwicklungsstand sind die sechs Kacheln bewusst *nicht belegt*; nach dem Aktivieren des Bearbeiten-Modus verweist die Oberfläche auf die noch ausstehende Bibliothek.

### Bibliothek und Programme: unterschiedliche Aufgaben

In der **Bibliothek** speichern wir künftig einzelne Texte mit Schrift, Farbe, Symbolen und ihrem jeweiligen Anzeigemodus. Die **Programme** verknüpfen diese Texte anhand stabiler Referenzen. Ändern wir später einen Bibliothekstext, wirken sich die Änderungen automatisch auf die Programme aus, die diesen Text verwenden. Beim Löschen werden wir referenzierende Programme berücksichtigen und vor Schäden warnen.

Sortieren soll per **Drag-and-drop** funktionieren, auch auf dem Smartphone. Die Bibliotheksansicht bekommt oben die Aktionen *Editor, Löschen, Sortieren, Übertragen*; die Programmverwaltung *Neu, Löschen, Sortieren, Übertragen*. Unten befinden sich die globalen Regler und der aktuelle Modus-Override.

### Globales Tempo und globale Helligkeit

Helligkeit und Geschwindigkeit gehören **zur Tafel, nicht zu einem gespeicherten Text**. Wir führen sie über alle relevanten Ansichten synchron. Ein Textwechsel darf diese beiden Werte nicht ungefragt ändern. In v0.4.0 lassen wir den gesamten Geschwindigkeitsbereich **1–100** zu (Standard 80); die Roh-BLE-Kennlinie bleibt vorerst unverändert und ist nicht mit der Hersteller-App kalibriert.

Der **Anzeigemodus** ist dagegen in unserer künftigen Bibliothek ein Bestandteil jedes Textes. Der Modus-Override unten in Bibliothek beziehungsweise Programme steuert die **aktuelle** Anzeige. Ein neu gestarteter Text setzt wieder seinen eigenen Modus. Die alten Blockseiten bleiben Standbilder.

### Pixelgrafiken statt Unicode-Vorschaubildern

Unser Symbol-Picker verwendet die vorhandenen Pixelmatrizen direkt. Das sichtbare 16×16-Motiv entspricht damit der Grundlage des späteren LED-Renders. Die Herkunft (Hersteller-Screenshot oder eigene Pixelart) spielt für die Bedienung keine Rolle; die Quellen dokumentieren wir separat. Die Symbole sind thematisch sortiert, nicht nach Herkunft.

Wir pflegen weitere Motive in der Excel-Sammlung `docs/assets/Pixel-Art.xlsx` im Repository. **Die neu ergänzten Excel-Motive sind in v0.4.0 noch nicht automatisch importiert.** Die von uns neu gezeichnete Krabbe soll die alte Krabbe ersetzen, während die zusätzliche Schneeflocke als eigene Variante erhalten bleibt. Der Frosch wartet noch auf eine Überarbeitung.

## Installation / erster Test

### Voraussetzungen

- ESP32-C3 Super Mini / ESP32C3 Dev Module und passende USB-Datenverbindung.
- Arduino IDE mit ESP32-Boardpaket und den durch Arduino-ESP32 bereitgestellten Bibliotheken `WiFi`, `WebServer`, `Preferences` und BLE-Unterstützung.
- CoolLEDX-Tafel mit 64 × 16 Pixeln; die Firmware ist derzeit auf die **bekannte BLE-Adresse einer Testtafel** ausgerichtet und scannt noch nicht selbst nach anderen Boards.
- Zum Flashen: Boardprofil **ESP32C3 Dev Module**, USB CDC On Boot aktiviert und Partitionierung **No OTA (2MB APP / 2MB SPIFFS)**; mit dieser Einstellung haben wir den bisherigen Sketch getestet. Für v0.4.0 steht der Kompiliertest noch aus.

### Flashen

1. Das Versionspaket in die **Wurzel des lokalen Git-Repositories** entpacken. Bestehende Versionsordner bleiben als Rückfalloption erhalten.
2. In der Arduino IDE `firmware/RavestickBoard_v0_4_0/RavestickBoard_v0_4_0.ino` öffnen. Die beiden `.h`-Dateien müssen **im gleichen Ordner** liegen.
3. Board-/Port-Einstellungen wie bei v0.3.5.1 wählen. **„Erase All Flash“ nicht aktivieren.** Wir behalten das bestehende NVS-Schema bei, damit gespeicherte Blockseiten erhalten bleiben.
4. Flashen, das Smartphone mit **`CoolLED1248`** verbinden und **`http://192.168.4.1`** öffnen. Das vorhandene Demo-WLAN-Passwort steht im Firmwarequelltext; vor dem Einsatz in einer fremden Umgebung bitte anpassen.
5. Erst Navigation, gespeicherte Blockseiten und die vertrauten BLE-Funktionen testen; anschließend Ambiente und globale Regler prüfen. Nach jedem Hardwaretest den Diagnoselog sichern.

**Wichtig:** Den Testkandidaten nicht allein aufgrund bestandener JavaScript-Tests als freigegeben betrachten. Bei Problemen können wir v0.3.5.1 aus ihrem eigenen Sketchordner flashen. Das Update führt keine NVS-Migration durch.

## Projektstruktur

```text
RavestickBoard-for-CoolLED1248/
├── README.md                         Projektübersicht, Aufbau und Bedienung
├── LICENSE                           MIT für unsere lizenzierbaren Codebestandteile
├── CHANGELOG.md                      Versionen und nachprüfbarer Teststatus
├── docs/
│   ├── ENTWICKLUNGSSTAND_v0.4.0_2026-10-26.md
│   ├── TESTPLAN_v0.4.0.md
│   ├── QUELLEN_UND_LIZENZEN.md       Rechte und externe Referenzen
│   ├── ROADMAP.md                    Entwicklungsschritte
│   ├── THEMENSPEICHER.md             Ideen, auch ohne Umsetzungszusage
│   └── assets/                       Totemfotos, GIF, UI-Bild, Pixelart-Excel
├── firmware/
│   ├── RavestickBoard_v0_3_5_1/     Hardwaregetesteter Rückfallstand
│   └── RavestickBoard_v0_4_0/       Neuer Sketch + WebUI + Pixelassets
└── web/index.html                    Bearbeitbare Weboberfläche (Quelle)
```

Die Datei `firmware/RavestickBoard_v0_4_0/RavestickBoard_WebUI.h` bettet die aktuelle `web/index.html` für den ESP32 ein. **Beide Versionen müssen synchron bleiben.** Die vielen alten Sketchordner sind gewollt: Wir archivieren Meilensteine, statt stabile Stände kommentarlos zu überschreiben.

## Protokoll und bekannte Grenzen

Wir arbeiten mit der bestehenden Hersteller-Firmware und deren BLE-Protokoll. Zu den hardwaregeprüften Live-Steuerbefehlen zählen Helligkeit (`0x08`), Geschwindigkeit (`0x07`), Anzeigemodus (`0x06`) sowie Ein/Aus (`0x09`). Unsere neuen Ambiente-Bilder verwenden bewusst den bekannten **normalen Bildtransfer**, keinen spekulativen zusätzlichen Gerätebefehl.

Das Display zeigt während neuer Bildübertragungen eine prozentuale Fortschrittsanzeige. Für autonome Programme ohne Fortschrittsanzeige zwischen den Seiten erforschen wir künftig den Animationsweg über `0x04`. Eine kompatible Implementierung für unser konkretes Board haben wir **noch nicht bestätigt**. Programmlaufzeit-Schätzungen für Schneeflocke, Aufdecken, Laser und die anderen Spezialeffekte stehen nicht auf unserer Prioritätenliste.

Das v0.4.0-Webinterface ist mobil ausgerichtet, aber noch kein stabiler Release. Besonders offen sind Arduino-Flashgrößen, Speicherverhalten, das neue Ambiente und die Abläufe mit mehreren gleichzeitig verbundenen Smartphones.

## Dokumentation und Mitarbeit

Wir halten technische Entscheidungen, Beobachtungen und Messungen getrennt von Ideen fest. Was in der Praxis bestätigt ist, erhält einen anderen Status als eine Implementierung, die erst noch auf der Tafel ausprobiert werden muss.

- [CHANGELOG](CHANGELOG.md) – was sich tatsächlich geändert hat.
- [Entwicklungsstand v0.4.0](docs/ENTWICKLUNGSSTAND_v0.4.0_2026-10-26.md) – Architektur und Status der neuen Oberfläche.
- [Testplan v0.4.0](docs/TESTPLAN_v0.4.0.md) – die nächste Hardware-Prüfrunde.
- [Roadmap](docs/ROADMAP.md) und [Themenspeicher](docs/THEMENSPEICHER.md) – nächste Schritte und langfristige Ideen.
- [Quellen und Lizenzen](docs/QUELLEN_UND_LIZENZEN.md) – saubere Abgrenzung zu fremdem Code und Bildmaterial.

## Wie wir entwickeln

Wir entwerfen, programmieren, dokumentieren und testen RavestickBoard in enger Zusammenarbeit. Dabei nutzen wir auch **KI-gestützte Entwicklungswerkzeuge** wie ChatGPT und Claude – zum Strukturieren unserer Ideen, für Code, Fehlersuche und Dokumentation. Die Anwendung entsteht nicht durch abstrakte Aufträge eines „Benutzers“: Wir treffen unsere technischen und gestalterischen Entscheidungen als gemeinsames Projekt und halten sie auch in dieser Perspektive fest.

**Was wir noch nicht auf der Tafel geprüft haben, nennen wir ausdrücklich ungetestet.** Statische Codeanalyse, Browsertests und BLE-Quittungen ersetzen keinen realen Anzeigeversuch. Unser Ziel bleibt nachvollziehbare Entwicklung statt vollmundiger Versionsversprechen.

## Lizenz und Rechte

Wir stellen **unseren eigenen Firmware- und Webcode sowie selbst verfasste Dokumentation unter der [MIT-Lizenz](LICENSE)** bereit. MIT erlaubt ausdrücklich auch kommerzielle Weiterverwendung; **„privates, nichtkommerzielles Hobbyprojekt“ beschreibt unsere Motivation und ist keine zusätzliche Nutzungsbeschränkung**.

**Die MIT-Lizenz erteilt jedoch keine Rechte an fremdem Material.** Rekonstruierte Hersteller-Symbole und Schriftbilder, die Original-App, Herstellerlogos, fremde Fotos und fremde Codeimplementierungen bleiben bei ihren jeweiligen Rechteinhabern. Herkunft, Quellenlage und Ausschlüsse dokumentieren wir in [`docs/QUELLEN_UND_LIZENZEN.md`](docs/QUELLEN_UND_LIZENZEN.md). Aus dem noch nicht lizenzierten OpenSign-Projekt übernehmen wir **keinen fremden Implementierungscode**, solange keine entsprechende Erlaubnis vorliegt.

RavestickBoard ist **kein offizielles Produkt und keine von CoolLED1248 unterstützte Anwendung**. Wir geben keine Gewähr für Kompatibilität, Funktionsfähigkeit oder Schäden. Das bestehende Board wird über BLE bedient; eine Modifikation der Hersteller-Firmware ist nicht erforderlich.

<p align="center"><strong>🍌 Hugs and Love. ❤️</strong></p>
