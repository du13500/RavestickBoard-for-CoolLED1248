<p align="center"><img src="docs/assets/board-gif.gif" alt="CoolLEDX mit LED-Motiven" /></p>
<p align="center"><img src="docs/assets/totem.jpg" width="420" alt="RavestickBoard Festival-Totem mit Neonbanane" /></p>
<h1 align="center">🍌 RavestickBoard for CoolLEDX / CoolLED1248</h1>
<p align="center"><strong>Offline-Webcontroller für die 64 × 16-Pixel-CoolLEDX-Tafel mit ESP32-C3.</strong><br>
Entstanden aus einer Festival-Banane und dem Wunsch, mehr aus der ursprünglichen CoolLED1248-App herauszuholen.</p>
<p align="center">
<img alt="Version" src="https://img.shields.io/badge/version-v0.4.5.1--dev-7056d8">
<img alt="Hardware" src="https://img.shields.io/badge/hardware-ESP32--C3-007c91">
<img alt="Status" src="https://img.shields.io/badge/status-dev%20hardware%20test%20pending-f3b653">
<img alt="Lizenz" src="https://img.shields.io/badge/license-MIT-brightgreen">
<img alt="Mission" src="https://img.shields.io/badge/mission-hugs%20and%20love-ff69b4">
</p>

> **Aktueller Entwicklungskandidat: v0.4.5.1-dev (3.11.2026).** Vor jedem neuen Verbindungsaufbau prüft die Firmware per kurzem Scan, ob die Tafel überhaupt zu hören ist (behebt den Hänger bei ausgeschalteter Tafel, Hardwaretest steht aus); kleines Bluetooth-Zeichen im Seitenkopf. Darunter unverändert 0.4.5.0: Zeichen-Editor für Lauftexte mit Wischen nach Richtung und Doppeltipp-Wort, Kurzwahl-Dialog mit Quellen-Schaltflächen, höchstens zwei WLAN-Geräte. Hardware-/Heap-Abnahme steht aus. Vollständige Versionsgeschichte: [`CHANGELOG.md`](CHANGELOG.md), Kurzüberblick: [`VERSIONSGESCHICHTE.md`](VERSIONSGESCHICHTE.md).

## Was ist RavestickBoard?

Das iPhone oder ein anderer Browser verbindet sich mit dem WLAN `CoolLED1248` des **ESP32-C3 Super Mini**. Das lokale Webinterface unter **http://192.168.4.1** überträgt die Darstellung via Bluetooth Low Energy (FFF0/FFF1) an die **CoolLEDX-Tafel**. Die LED-Tafel benötigt keine modifizierte Herstellerfirmware und kann einmal hochgeladene Inhalte autonom wiederholen. Die Bedienoberfläche läuft ohne Internetzugang; USB bleibt der einzige Firmware-Updateweg, **kein OTA**.

**Was die Oberfläche kann:**

- **Kurzwahl:** acht frei belegbare Kacheln (Bibliothekseinträge, Programme, Ambiente) für den schnellen Zugriff auf der Veranstaltung.
- **Lauftext:** Original-Schriften (Normal, Fett), Engschrift und Mini (ein- oder zweizeilig), 24 Herstellersymbole plus eigene Motive, Live-Vorschau und Datenbudget. Farbe und Schrift lassen sich pro Zeichen gestalten.
- **Blockseiten:** feste Seiten mit Ausrichtung und Anzeigedauer, als Blockfolge sendbar.
- **Bibliothek und Programme:** Einträge speichern, benennen, duplizieren, sortieren; Programme spielen Einträge nacheinander ab (Original oder als gemeinsames Laufband).
- **Tafelsteuerung:** Helligkeit, Tempo und Anzeige an/aus stehen unten auf jeder Seite; die Tasten zum Erzwingen eines Anzeigeverhaltens auf allen Seiten außer der Kurzwahl, wo nur Linkslauf, Standbild und der Stift zum Bearbeiten bleiben.
- **Diagnose:** RAM-Messung, HTTP-Spur, Absturzspur nach Hänger, Export als Textdatei.

## Aktuell: v0.4.5.1-dev – Vor-Scan der Tafel, Bluetooth-Zeichen (aufbauend auf 0.4.5.0: Zeichen-Fenster, Kurzwahl-Dialog, zwei WLAN-Geräte)

- **Zeichen gestalten (seit 0.4.4.0):** Im Lauftext-Editor öffnet die Palettentaste ein Fenster. Zeichen einzeln antippen oder per Wischen auswählen, dann Farbe und Schrift (Normal, Fett, Engschrift, Mini) für die Auswahl setzen. „Farbwechsel“ färbt reihum pro Zeichen oder pro Wort. Bei Zweizeilig Mini gehen Farben pro Zeichen und Zeile.
- **Neu in 0.4.5.1** (Auswertung des Serial-Monitors von 0.4.5.0):
  - **Vor-Scan:** Vor jedem *neuen* Verbindungsaufbau lauscht der ESP bis zu 3 s passiv nach der Tafel und bricht beim Treffer sofort ab. Wird die Tafel nicht gehört, aber andere Bluetooth-Geräte, gibt es keinen Verbindungsversuch und die Meldung „LED-Tafel nicht gefunden“. Hört der Scan gar nichts, verbindet die Firmware wie bisher. Hintergrund: Mit ausgeschalteter Tafel kehrte `connect()` nicht rechtzeitig zurück, der Hang-Wächter hat nach 25 s neu gestartet. Abschaltbar mit `RB_PRESCAN 0`. Ist die Tafel mit der Original-App verbunden, wirbt sie nicht und wird nicht gefunden.
  - **Bluetooth-Zeichen im Seitenkopf:** `bluetooth` (kein Verbindungsversuch bzw. zuletzt erreicht, dann grün), `bluetooth-searching` (Auftrag läuft), `bluetooth-connected` (Verbindung steht), `bluetooth-off` (zuletzt nicht erreicht). Während einer Übertragung antwortet der ESP nicht; es bleibt der zuletzt gemeldete Zustand stehen.
  - **Diagnose:** Zähler und Dauer der Vor-Scans (mit Signalstärke); ein Heap-Tiefstand außerhalb von Anfragen nennt jetzt die zuletzt beendete Anfrage, den Abstand dazu und das Minimum davor.
  - **Übergang:** Die Adresse der Tafel steht noch fest im Code. Geplant (0.5.0.0): „Meine Tafel“ wird im ESP gespeichert und per Suche eingerichtet, es wird keine Adresse mehr mitgeliefert; zusätzlich Wechsel auf eine fremde Tafel mit Rückweg.
- **Neu in 0.4.5.0** (Rückmeldungen aus dem Test von 0.4.4.2):
  - **Höchstens zwei WLAN-Geräte:** `RB_AP_MAX_CLIENTS` steht jetzt auf 2 (zuvor 4). Das ist festgelegt, mehr Geräte sind nicht vorgesehen; ein drittes Gerät wird vom Access Point abgewiesen. Grund: Das dritte Gerät hat in 0.4.4.2 das System-Minimum des Heaps auf 3288 B gedrückt. Die Tafel zählt nicht mit, sie hängt per BLE.
  - **Zeichen-Fenster (Lauftext):** Waagerecht wischen markiert (auf Markiertem entmarkiert), senkrecht wischen scrollt. Der Umschalter Einzeln/Wischen entfällt. Doppeltipp markiert ein ganzes Wort (Satzzeichen am Rand bleiben außen), nochmal entmarkiert es. Vorschau und Werkzeugleiste stehen oben fest und werden beim Scrollen kleiner. Unten hält eine schmale Farbleiste, solange der große Farbblock nicht im Bild ist; die Schriften stehen nur im großen Block.
  - **Vorschau im Editor** (Lauftext und Blockanzeige) wird beim Scrollen kleiner; die Vorschau-Werkzeuge des Lauftexts erscheinen nur ganz oben.
  - **Kurzwahl-Dialog:** Quelle als Schaltflächen mit den Icons der Menüführung (Bibliothek, Programme, Ambiente; Animationen ausgegraut). Die Beschriftung wird mit dem Namen des Inhalts vorbelegt; bleibt sie unverändert, folgt die Kachel weiter dem Namen.
  - **Kleinigkeiten:** Startbildschirm-Bedienfeld mit fünf gleichmäßig verteilten Tasten (an, aus, Linkslauf, Standbild, Bearbeiten); Einstellungen und Diagnose: Backup- und Download-Knöpfe in der Designlinie mit Icons; ein zweiter Tipp auf einen gewählten Eintrag in Bibliothek und Programmen hebt die Auswahl auf.
- **Neu in 0.4.4.2** (Rückmeldung aus dem Test von 0.4.4.1, nur Weboberfläche, Firmware unverändert):
  - **Startbildschirm:** Das untere Bedienfeld zeigt nur noch *an, aus*, *Linkslauf* (Chevron links), *Standbild/Blocktext* (Achteck) und *Bearbeiten* (Stift), mit etwas Platz zwischen den Gruppen. Alle anderen Seiten behalten die volle Steuerung.
  - **Sortieren sofort:** In Bibliothek und Programmen springt der Eintrag beim Tippen auf den Pfeil sofort an die neue Stelle; die Befehle gehen der Reihe nach im Hintergrund an den ESP32. Nur bei einem Fehler wird die Liste vom ESP32 neu geholt.
  - **Weniger Last auf dem ESP32:** Seitenwechsel holen Bibliothek und Programme nur noch neu, wenn die Daten älter als 60 s sind (sonst Anzeige aus dem Speicher). Beim Start erscheint die Oberfläche zuerst, die Listen laden danach im Hintergrund. Die Statusabfrage pausiert, solange der ESP32 mit einer Anfrage beschäftigt ist oder der Tab im Hintergrund liegt.
- **Neu in 0.4.4.1:**
  - **Wischen entmarkiert:** Beginnt der Wisch auf einem markierten Zeichen, entmarkiert er alles, was er überstreicht; sonst markiert er.
  - **Schrift direkt wählen:** Die vier Schriften sind einzeln antippbar, der große Durchschalter entfällt.
  - **Rückgängig und Wiederherstellen** (nur Icons, neben „Text leeren“) für den ganzen Lauftext-Editor: Text, Zeichenstile und Zeilenfarben, mehrstufig. Der Knopf „Stil rückgängig“ entfällt.
  - **Größeres Eingabefeld (nur Lauftext):** wächst mit dem Text mit und lässt sich am Griff darunter nach unten größer ziehen (Doppelklick auf den Griff stellt wieder auf automatisch; auf dem iPhone noch ungeprüft). Die Eingabetaste schließt die Tastatur, statt einen Umbruch einzufügen.
  - **Programme sortieren:** Pfeile hoch/runter wie in der Bibliothek. Die Reihenfolge liegt auf dem ESP32 und gilt für alle Geräte (neue Firmware-Route `/api/collections/move`, Datei `/rb-lib/porder.bin`).
  - **Startbildschirm:** zeigt wieder dieselbe Tafelsteuerung wie alle anderen Seiten, inklusive der Tasten zum Erzwingen eines Anzeigeverhaltens und dem Stift für die Kurzwahlen.
- **Technik:** Die Gestaltung reist als unsichtbare Steuerzeichen im Text (U+0090..U+009C, Format in [`docs/TECHNIK.md`](docs/TECHNIK.md)). Die Firmware wertet sie beim Zeichnen aus, daher wirken sie auch in Bibliothek, Programmen und Gruppen. Einträge ohne Marken bleiben unverändert gültig; ältere Firmware kennt die Marken nicht (Darstellung dort ungeprüft).
- **Entwicklungsschalter:** `RB_TEST_HANG` steht in diesem Stand noch auf 1 (Test-Auslöser `/api/test/hang` für den Wiederanlauftest). Für eine Freigabe auf 0 setzen.
- **Bekannt:** Die Gesten im Zeichen-Fenster (Richtungserkennung, Doppeltipp ohne Zoom) sind nur im simulierten Chromium geprüft, nicht auf dem iPhone. Die Bibliotheksliste braucht auf dem ESP32 etwa 0,9 s, weil jeder Eintrag vollständig gelesen und geprüft wird; währenddessen bedient der Webserver nichts anderes. Eine Firmware-Beschleunigung ist offen. Der seltene BLE-Hänger (Diagnose: Phase `ble_regNotify`, Wächter-Neustart) und der niedrige Heap-Tiefstwert bleiben ungelöst.
- **Noch offen:** Zeichen-Editor für Blockseiten (geplant als 0.4.6.0), Arduino-Compile und Hardwaretest, Wiederanlauf- und Zwei-Geräte-Test (siehe [`docs/ROADMAP.md`](docs/ROADMAP.md)).

## Kurz zurückgeblickt

- **v0.4.4.2-dev:** Sortieren sofort, weniger Anfragen an den ESP32, schlankes Startbildschirm-Bedienfeld.
- **v0.4.4.1-dev:** Feinschliff des Zeichen-Editors, Rückgängig/Wiederherstellen, sortierbare Programme, wachsendes Eingabefeld.
- **v0.4.4.0-dev:** Erstfassung von „Zeichen gestalten“, einheitliche Sendefarbe (Mint) auf allen Senden-Knöpfen, etwas größere Regler.
- **v0.4.3.x-dev (Stabilitätsarbeit):** Nach einem Tafel-„ERROR“ im Blockband-Upload hing der ESP32 (die BLE-Bibliothek des Cores wartet an mehreren Stellen ohne Zeitlimit). Seitdem gibt es einen **Wächter**, der das Board nach 25 s ohne Lebenszeichen neu startet, eine **Absturzspur** im RTC-Speicher (Serial `[VORLAUF]`, Diagnose) und eine getrennte **RAM-Messung** beim Start. Der Wächter hat einen echten Hänger erkannt; danach war die Weboberfläche zunächst nicht erreichbar. Deshalb meldet der Wächter vor dem Neustart alle WLAN-Geräte ab (Vermutung, noch nicht bewiesen), und ein Test-Auslöser erlaubt es, den Wiederanlauf gezielt zu prüfen. Messreihen zeigten einen Einbruch des Heap-Minimums beim Beitritt eines zweiten WLAN-Clients (bis 192 B). Der Hänger selbst ist selten (94 Verbindungen am Stück ohne Fehler). **Keine belegte Behebung, nur Messkandidaten.**
- **v0.4.3-dev (UI-Konsolidierung):** Lauftext-Editor mit „Lauftext senden“ und „Speichern“ (Aktualisieren oder neue Variante), Blockanzeige mit Original-Normal als Standard und Seitenwerkzeugen, Programme im Bibliotheksstil mit eigener Bearbeitungsansicht, Lucide-Icons, App-Symbol und Favicon, Diagnose ohne Dauerlast (kompaktes JSON, TXT-Bericht nur auf Abruf).
- **v0.4.2.x-dev (HTTP- und RAM-Stabilisierung):** Weboberfläche per gzip, ETag und 304 ausgeliefert, JSON-Antworten in begrenzten Stücken, Lauftext-Gruppen mit kleinem 384-Byte-Tile-Puffer statt vollem 30-KiB-Bildpuffer, einmalige Wiederholung einer nicht bestätigten Gruppe nach BLE-Neuverbindung, Programme als Laufband mit vorsichtiger Laufzeitschätzung, gemeinsame untere Tafelsteuerung, acht Kurzwahlen, Bibliothek im LittleFS. Hardwaretests vom 28./29.10.2026 bestätigten Lauftexte, Symbole, Farben und kleine Laufbänder; ein späterer Upload scheiterte bei Paket 70/106 ([Testbefund](docs/TESTBEFUNDE_v0.4.2.3.md)). **Der historische Heap-Tiefstwert (zeitweise 592 Byte, auch mit nur einem Client) ist weiter nicht eindeutig geklärt.** Die A/B-Messungen dazu stehen in [`docs/VERGLEICHSTEST_v0.4.2.4.2.md`](docs/VERGLEICHSTEST_v0.4.2.4.2.md).

Die vollständige Geschichte steht in [`CHANGELOG.md`](CHANGELOG.md).

### Speichern und Mehrgerätebetrieb

Beim Bearbeiten eines vorhandenen Bibliothekseintrags wählt man nach **Speichern**, ob er **aktualisiert** oder als **neuer Eintrag** mit eigener ID gesichert wird. Die Vorlage und darauf verweisende Kurzwahlen und Programme bleiben bei der zweiten Option unverändert.

Bearbeitungen werden auf dem jeweiligen Gerät direkt in der Bibliothek angezeigt. Hat ein anderes Smartphone Änderungen vorgenommen, aktualisiert **Menü → Webinterface neu laden** die Ansicht (Bibliothek und Programme werden beim Seitenwechsel außerdem im Hintergrund aufgefrischt, wenn sie älter als 60 s sind); gleichzeitige Bearbeitung desselben Eintrags kann vorherige Änderungen überschreiben. Die Programmreihenfolge liegt auf dem ESP32 und ist damit für alle Geräte gleich.

## Installation und Datensicherheit

1. Vorab ein JSON-Backup aus der aktuell laufenden Firmware erstellen und lokal aufbewahren. Die bisherige Firmware als Rückfallversion sichern.
2. `firmware/RavestickBoard_v0_4_5_1/RavestickBoard_v0_4_5_1.ino` öffnen. **Alle sechs Dateien** im selben Sketch-Ordner belassen: `.ino`, `RavestickBoard_WebUI.h`, `RavestickBoard_Assets.h`, `RavestickBoard_Library.h`, `RavestickBoard_v042.h` und `RavestickBoard_Icon.h`.
3. Arduino IDE: **ESP32C3 Dev Module**, Espressif **3.3.12**, **USB CDC On Boot: Enabled**, **No OTA (2MB APP/2MB SPIFFS)**. Bestehendes Partitionsschema **nicht ändern**, **Erase All Flash deaktiviert** lassen.
4. Nach dem Flashen mit WLAN `CoolLED1248` verbinden, `http://192.168.4.1` aufrufen. Bibliothek nicht neu formatieren; das Bibliotheksformat ist unverändert. Falls das Homescreen-Icon nicht übernommen wird, die Verknüpfung über Safari erneut hinzufügen.
5. Nach Änderungen an `web/index.html`: `python tools/sync_web.py` erzeugt den gzip-Webheader neu.

Weiterführend: [Bibliotheksformat](docs/BIBLIOTHEK_v0.4.1.md), [letzte ausführliche Installationsanleitung](docs/INSTALLATION_v0.4.2.4.1.md), [Testplan](docs/TESTPLAN_v0.4.2.4.1.md), [Changelog](CHANGELOG.md), [Roadmap](docs/ROADMAP.md) und [Themenspeicher](docs/THEMENSPEICHER.md).

**Teststand v0.4.5.1:** zusätzlich zu den unten genannten Prüfungen: Entscheidungslogik des Vor-Scans mit dem echten Sketch-Code gegen Attrappen der BLE-Klassen (Treffer, nur fremde Geräte, nichts gehört) und Browser-Test des Bluetooth-Zeichens. Die echte BLE-Bibliothek des Arduino-Core ist hier nicht verfügbar; Signaturen von `BLEScan`/`BLEAdvertisedDeviceCallbacks` sind nicht gegen Core 3.3.x kompiliert.

**Teststand v0.4.5.0:** JavaScript-Syntax, Browser-Test der Oberfläche (inkl. echter Touch-Wischereignisse, Doppeltipp, sofortigem Sortieren mit absichtlich langsamem Mock-ESP und Fehlerfall) (Playwright/Chromium, mehrere Mobilgrößen), Versionierung, gzip-/ETag-Konsistenz und Firmware-Invarianten geprüft. Die Firmware-Zeichenfunktionen und die neue Programm-Reihenfolge wurden im Host-Build (g++) mit dem echten Sketch-Code kompiliert und getestet; die Zeichnung wurde pixelweise mit der Browser-Vorschau verglichen. **Kein Arduino-Gesamtcompile dieses Standes und keine Hardwareprüfung auf ESP32-C3 und Tafel.** Details: [`tests/README.md`](tests/README.md).

## Mitmachen und Sicherheit

Siehe [CONTRIBUTING.md](CONTRIBUTING.md), [SECURITY.md](SECURITY.md), [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md) und unsere [Issue-Vorlagen](.github/ISSUE_TEMPLATE/).

## Wie wir entwickeln

Wir entwickeln RavestickBoard gemeinsam: Wir entwerfen, programmieren, dokumentieren und testen die Software im engen Austausch. Die realen ESP32-/CoolLEDX-Hardwaretests führen wir vor Ort durch; dabei unterstützen KI-gestützte Werkzeuge wie **ChatGPT und Claude** die Konzeption, Fehlersuche, Implementierung und Dokumentation. Wir unterscheiden ausdrücklich zwischen lokal geprüftem Code und hardwarebestätigten Funktionen.

## Entwicklung, Quellen und Lizenz

RavestickBoard ist eine eigenständige C++-/HTML-/JavaScript-Implementierung. Die beiden aktuellen Referenzen für zukünftige Animationsforschung sind [jean-santos/coolled1248-rs](https://github.com/jean-santos/coolled1248-rs) und [zackmcmurrin-dev/coolledux-linux-uploader](https://github.com/zackmcmurrin-dev/coolledux-linux-uploader). Ergänzend dokumentieren wir den früh untersuchten [UpDryTwist/coolledx-driver](https://github.com/UpDryTwist/coolledx-driver) und [ohtaras/led](https://github.com/ohtaras/led). Die technischen Protokollfakten wurden für unsere Firmware eigenständig umgesetzt. Ob eine Datenformatvariante des anderen Panels auf unserer eigenen CoolLEDX funktioniert, ist separat zu prüfen.

**Danksagung:** Besonderer Dank an **Adam Schulz**, [OpenSign](https://github.com/schulzad/coolled-sign-controller), für den freundlichen Austausch und die technischen Hinweise zu nativer Animation (`0x04`) und BLE-Bestätigungen. Diese Erkenntnisse stehen für eine spätere Version auf der Roadmap; hier wurde **kein OpenSign-Code übernommen**.

**Lizenz und Drittmaterialien:** Unser selbst entwickelter RavestickBoard-Code steht unter der **[MIT-Lizenz](LICENSE)** (Copyright © 2026 du13500). Die Lizenz erlaubt ausdrücklich auch kommerzielle Nutzung, Bearbeitung und Weitergabe unter ihren Bedingungen. Das Projekt wird derzeit als privates Hobbyprojekt entwickelt; das schränkt die MIT-Rechte nicht ein. Eingebettete **Lucide-Icons** stehen unter ISC, ausgewählte von Feather abgeleitete Icons zusätzlich unter MIT. Ihre erforderlichen Copyright- und Lizenzhinweise stehen in [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md). Fremde Originalmotive, Marken und sonstige Drittmaterialien werden durch unsere MIT-Lizenz nicht automatisch mitlizenziert.

<p align="center"><strong>Made for the rave, powered by a banana. 🍌</strong><br><sub>Independent project, not affiliated with the CoolLED1248/CoolLEDX manufacturer.</sub></p>
