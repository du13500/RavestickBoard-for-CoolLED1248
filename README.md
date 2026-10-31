> **Aktueller Entwicklungskandidat: v0.4.3.3-dev (30.10.2026).** RAM-Messung, Hang-Wächter und Absturzspur auf Basis von v0.4.3.2. Vollständige Versionsgeschichte: [`CHANGELOG.md`](CHANGELOG.md). Hardware-/Heap-Abnahme steht noch aus.

<p align="center"><img src="docs/assets/board-gif.gif" alt="CoolLEDX mit LED-Motiven" /></p>
<p align="center"><img src="docs/assets/totem.jpg" width="420" alt="RavestickBoard Festival-Totem mit Neonbanane" /></p>
<h1 align="center">🍌 RavestickBoard for CoolLEDX / CoolLED1248</h1>
<p align="center"><strong>Offline-Webcontroller für die 64 × 16-Pixel-CoolLEDX-Tafel mit ESP32-C3.</strong><br>
Entstanden aus einer Festival-Banane und dem Wunsch, mehr aus der ursprünglichen CoolLED1248-App herauszuholen.</p>
<p align="center">
<img alt="Version" src="https://img.shields.io/badge/version-v0.4.3.3--dev-7056d8">
<img alt="Hardware" src="https://img.shields.io/badge/hardware-ESP32--C3-007c91">
<img alt="Status" src="https://img.shields.io/badge/status-dev%20hardware%20test%20pending-f3b653">
<img alt="Lizenz" src="https://img.shields.io/badge/license-MIT-brightgreen">
<img alt="Mission" src="https://img.shields.io/badge/mission-hugs%20and%20love-ff69b4">
</p>

## Was ist RavestickBoard?

Das iPhone oder ein anderer Browser verbindet sich mit dem WLAN `CoolLED1248` des **ESP32-C3 Super Mini**. Das lokale Webinterface unter **http://192.168.4.1** überträgt die Darstellung via Bluetooth Low Energy (FFF0/FFF1) an die **CoolLEDX-Tafel**. Die LED-Tafel benötigt keine modifizierte Herstellerfirmware und kann einmal hochgeladene Inhalte autonom wiederholen. Die Bedienoberfläche läuft ohne Internetzugang; USB bleibt der einzige Firmware-Updateweg, **kein OTA**.

## Aktuell: v0.4.3.3-dev – RAM-Messung, Hang-Wächter und Absturzspur

- **Hintergrund:** Bei einem Blockband-Upload zeigte die Tafel „ERROR“, und der ESP32 antwortete danach nicht mehr. Die BLE-Bibliothek des Cores wartet an mehreren Stellen ohne Zeitlimit; bricht die Gegenstelle ab, kann die Hauptschleife stehen bleiben (plausible, noch nicht bewiesene Erklärung).
- **Neu:** Ein Wächter startet das Board nach 25 s ohne Lebenszeichen neu. Eine Absturzspur im RTC-Speicher nennt beim nächsten Start die Phase, in der es stand (Serial `[VORLAUF]`, Diagnose, RAM-Export). Die Spur überlebt Reset, nicht das Ausstecken.
- **RAM-Messung:** Messreihen auf v0.4.3.2 zeigten einen Einbruch des Heap-Minimums beim Beitritt eines (zweiten) WLAN-Clients (mit einem Client etwa 16 KB, mit zwei Clients einmal 192 B). Neu: getrennte `[RAM]`-Werte beim Start (BLE, NVS, LittleFS, Programme, WLAN-AP), WLAN-Ereignisse mit Zeitstempel im RAM-Export, Sketch-Größe nur einmal beim Start (vermutete Ursache der ca. 300 ms langen Diagnose-Aufrufe). Schalter im Sketch `RB_AP_MAX_CLIENTS` und `RB_LAZY_BLE_INIT`, beide im Standard aus.
- **Weboberfläche unverändert** außer der Versionskennung. Hardware-Messkandidat, kein nachgewiesener Fix.

## Vorher: v0.4.3.2-dev – Programme im Bibliotheksstil

- **Programmliste:** Gleiche Bauweise wie die Bibliothek: Icon-Leiste (neu, bearbeiten, starten, stoppen, löschen) und Zeilen mit Untertitel (Anzahl Einträge, Wiedergabeart). Defekte Programme sind rot markiert.
- **Bearbeiten:** Eigene Ansicht mit Zurück-Pfeil. Schritte erscheinen als Bibliothekszeilen mit Typ („Lauftext“, „Blockfolge · 4 Seiten“). Speichern führt zur Liste zurück, bei ungespeicherten Änderungen fragt „Zurück“ nach.
- **Firmware unverändert:** Nur Versionskennung und Weboberfläche. Läuft nach Rückmeldung auf dem ESP32-C3, die Ansicht wurde freigegeben.

## Vorher: v0.4.3.1-dev – Diagnoseentlastung und UI-Feinschliff

- **Diagnose:** Statusübersicht fordert nur kompaktes JSON an. Der vollständige TXT-Bericht entsteht ausschließlich bei Download bzw. Kopieren, speichersparend in kleinen HTTP-Stücken statt als 11-KB-String.
- **HTTPTRACE:** Speichert zusätzlich worst-case-Pfade (längste Anfrage, Heap-Minimum, HTTP-Zeitpunkt eines System-Min-Abfalls) auch über das 32-Anfragen-Fenster hinaus. Ein außerhalb von Handlern beobachteter Tiefstwert bleibt ausdrücklich *nicht zugeordnet*.
- **UI:** Ambiente wieder einspaltig, Blockseiten-Werkzeuge mit klaren deaktivierten Zuständen, originale Lucide-SVGs, gleichmäßige Senden-/Speichern-Abstände.
- **Wichtig:** Keine belegte Behebung des kritischen historischen Heap-Tiefstwerts; v0.4.3.1 ist ein Hardware-Testkandidat, keine stabile Freigabe.

## Vorher: v0.4.3-dev – UI-Konsolidierung

- **App-Symbol:** Die gleiche hellgrauweiße Lucide-Banane wie auf der Menütaste, auf deren dunklem Hintergrund, als Offline-iPhone-Icon und Browserfavicon. Die alte Emoji-Banane wird dort nicht mehr verwendet.
- **Lauftext:** Nur der Lauftextmodus wird neu erzeugt. Zwei große Aktionen „Lauftext senden“ und „Speichern“. Bei vorhandenen Bibliothekseinträgen bietet Speichern eine Auswahl zwischen Aktualisieren und neuer Variante.
- **Blockanzeige:** Neue Seiten verwenden standardmäßig **Original - Normal**. Drei Ausrichtungs-Icons und kompakte Seitenwerkzeuge mit Radierer; bei einer einzigen Seite steht `trash-off`, „Alles löschen“ ist bei einer Seite grau und deaktiviert, ab zwei Seiten rot hervorgehoben. Große Aktionen: „Diese Seite senden“, „Alle Seiten senden“, „Speichern“.
- **Bibliothek und Dialoge:** Tooltips ohne alte Symbolzeichen; Umbenennen mit `text-cursor-input`, Duplizieren-Dialog mit passenden Icon-Aktionen. Keine Favoriten, Suche oder manuelle Aktualisieren-Taste.
- **Ambiente und Tafelsteuerung:** Farbkacheln nutzen die Bildschirmhöhe. Im gemeinsamen Steuerbereich verwendet Standbild das `octagon`-Symbol. Die acht Kurzwahlen bleiben erhalten.
- **Editorführung:** „Zur Bibliothek“ nur bei geöffneter Bibliotheksvorlage. „Neu“ bleibt verfügbar; Änderungen werden nicht ohne Bestätigung verworfen.

### Speichern und Mehrgerätebetrieb

Beim Bearbeiten eines vorhandenen Bibliothekseintrags wählt man nach **Speichern**, ob er **aktualisiert** oder als **neuer Eintrag** mit eigener ID gesichert wird. Die Vorlage und darauf verweisende Kurzwahlen und Programme bleiben bei der zweiten Option unverändert.

Bearbeitungen werden auf dem jeweiligen Gerät direkt in der Bibliothek angezeigt. Hat ein anderes Smartphone Änderungen vorgenommen, aktualisiert **Menü → Webinterface neu laden** die Ansicht; gleichzeitige Bearbeitung desselben Eintrags kann vorherige Änderungen überschreiben.

**Sketch:** `firmware/RavestickBoard_v0_4_3_3/RavestickBoard_v0_4_3_3.ino`. Die sechs zugehörigen Dateien im gemeinsamen Ordner belassen. `python tools/sync_web.py` erzeugt den gzip-Webheader nach HTML-Änderungen.

**Teststand v0.4.3.3:** JavaScript-Syntax, Browser-Simulation der Programme-Ansicht, Versionierung, gzip-/ETag-Konsistenz und Firmware-Invarianten geprüft; Wächter- und Spurcode mit Stubs auf Syntax geprüft, Auslöseverhalten simuliert. **Kein Arduino-Gesamtcompile dieses Standes, keine Hardwareprüfung, kein nachgewiesener Heap-Fix.** v0.4.3.2 läuft nach Rückmeldung auf dem ESP32-C3; die Heap-Messreihen A–C liegen vor (siehe Changelog).

Frühere Versionen, einschließlich v0.4.2.5.4-dev, sind vollständig in [`CHANGELOG.md`](CHANGELOG.md) und den bisherigen Release-Notes dokumentiert.

## Vorversion: v0.4.2.5-dev – HTTP-/RAM-Stabilisierung

**Neue Firmware:** `firmware/RavestickBoard_v0_4_2_5/RavestickBoard_v0_4_2_5.ino` (alle sechs Sketch-Dateien zusammenlassen). Grundlage ist Build B mit gzip/ETag/304 und `/api/http.txt`. Die JSON-Antworten der Bibliothek und Programmübersicht werden in gebündelten, begrenzten Sendestücken ausgegeben. Die Lucide-Icons stehen schon im HTML, statt sichtbar von alten Symbolen umgebaut zu werden. Das LittleFS-Bibliotheksformat und das BLE-Übertragungsprotokoll wurden nicht geändert.

**Noch nicht gelöst:** In den echten A/B-Messungen sank `ESP.getMinFreeHeap()` teilweise auf **592 Byte**, auch mit nur einem WLAN-Client. Die neue Version ist ein **gezielter Optimierungsversuch**, keine als stabil bestätigte Freigabe. Der ursprüngliche Tiefstwert kann aus WLAN-/Netzwerkkomponenten kommen; die Ursache ist noch nicht eindeutig geklärt. Test unter Last und BLE-Abnahme stehen aus. Die neue Diagnose bleibt zur Ursachenklärung erhalten.

**Lizenz/Danksagung:** Eigener Quellcode unter MIT (reine `LICENSE`), eingebettete Lucide/Feather-Icons laut `THIRD_PARTY_NOTICES.md`. Besonderer Dank an **Adam Schulz**, [OpenSign](https://github.com/schulzad/coolled-sign-controller), für den freundlichen Austausch und die technischen Hinweise zu nativer Animation (`0x04`) und BLE-Bestätigungen. Diese Erkenntnisse stehen für eine spätere Version auf der Roadmap; hier wurde **kein OpenSign-Code übernommen**.

**Installation/Test:** siehe `UPDATE_HINWEISE_v0.4.2.5.txt`. Nach dem Flashen zunächst normal die Webseite öffnen, Bibliothek/Kurzwahlen überprüfen, dann einen kurzen WLAN-/HTTP-Test mit `/api/http.txt` durchführen. Kein Speichervorgang oder Partitionswechsel erforderlich.

### Vorherige A/B-Messungen (v0.4.2.4.2)


**A und B sind absichtlich zwei eigenständige Versuchsfirmwares, keine aufeinanderfolgenden Upgrades.**
A ergänzt unsere bewährte v0.4.2.4.1-dev ausschließlich um dieselbe HTTP-/RAM-Protokollierung, die auch in B enthalten ist. Die Startseite bleibt bei A unkomprimiert und mit `Cache-Control: no-store`. B verwendet für **dieselbe HTML-Quelle** gzip-Stufe 9, ETag und optional `304 Not Modified`. Die Geräteversion unterscheidet `-A` und `-B` auch in den Logdateinamen. Für die Messung wird jeweils ein Sketch geflasht, nicht beide.

**Teststatus:** Weder A noch B ist lokal mit Arduino kompiliert oder auf dem ESP32-C3 getestet worden. Unsere zuvor bestätigten Hardwaretests gelten für v0.4.2.4.1-dev und dürfen nicht auf die beiden Testbuilds übertragen werden. Die 20-ms-Stichproben können sehr kurze Speicherpeaks übersehen und verändern selbst die RAM-Grundlast geringfügig. Der historische `ESP.getMinFreeHeap()`-Wert wird erst durch ESP32-Neustart zurückgesetzt.

**Neuer Mess-Export:** `http://192.168.4.1/api/http.txt` protokolliert HTTP-Pfade (inklusive unbekannter 404-Pfade), Status, WLAN-Clientzahl, Laufzeit, Heap- und Maxblockwerte vor/nach der Anfrage, den niedrigsten in 20-ms-Schritten beobachteten Wert während der Anfrage und das historische System-Minimum vor/nach der Anfrage. Queryparameter, Textinhalte, Passwörter und BLE-Adressen werden nicht exportiert. Ältere Exporte `/api/ram.txt` und `/api/diagnostics.txt` funktionieren weiterhin.

**Testablauf und Grenzen:** [docs/VERGLEICHSTEST_v0.4.2.4.2.md](docs/VERGLEICHSTEST_v0.4.2.4.2.md). Historische A/B-Sketches waren `v0_4_2_4_2_A` und `v0_4_2_4_2_B`. Das Tool `python tools/sync_web.py` erzeugt inzwischen den Header des aktuellen v0.4.3-Builds. Das alte Build-B-HTML umfasste nach gzip 44.911 B gegenüber 157.222 B Rohdaten. Die neue HTML-Fassung hat durch vorgerenderte SVGs einen anderen Umfang.

### Historisch: v0.4.2.4.1-dev

**Hardwaretest vom 29.10.2026, Basis v0.4.2.4-dev:** Firmware kompiliert und startet ohne Panic; LittleFS-Bibliothek und sechs Kurzwahlen erhalten. Ein kleines Laufband (Lauftext → 4 Blockseiten → Lauftext) mit Originalfarben und Reihenfolge funktioniert als ein Upload; auch die 16 echten Leer-Pixelspalten wurden bestätigt. **Noch offen:** geteilte Laufbänder über 30 KiB, Langzeit-RAMTRACE, ACK-/Retry-Test, die weiteren UI- und Backupregressionen. Der normale gemischte Programmablauf mit stehenden Blockseiten war in dieser Runde nicht reproduzierbar; das ist weder ein Bestehen noch ein belegter Fehler.

**Hotfix v0.4.2.4.1-dev:** Die globale Tafelsteuerung liegt wieder als normaler Abschnitt am *Ende des Seiteninhalts*. Sie schwebt nicht über Kurzwahlen, Bibliothek oder Editoren; zum Erreichen wird bei Bedarf gescrollt. Der Kurzwahl-Einrichtungsdialog hat drei kompakte reine Lucide-Aktionen `trash` / `undo-dot` / `star-check`. Im Bearbeitungsmodus wechselt das Icon zu `pencil-off`, beim Verlassen zu `pencil`, ohne Meldung. Die Löschroutine für Programme entfernt jetzt auch die mögliche `.bak`-Rückfallkopie, damit ein bewusst gelöschtes Programm nicht unbeabsichtigt zurückkehrt (Review F-06).

**Der Streaming- und BLE-Kern wurde für den damaligen UI-Hotfix bewusst nicht umgebaut.** Die offenen Punkte aus unseren beiden Claude-Reviews (F-01–F-12, S-01–S-06) bleiben als technische Arbeitsliste erhalten; insbesondere ACK-Prüfung, Webserver während BLE, Laufzeitkalibrierung und echte Byte-Verhaltenstests sind weiterhin offen. Arduino-Kompilierung und Hardwaretest **dieser** Version stehen aus.

### Vorgängerversion v0.4.2.4-dev

**Unsere Hardwaretests vom 28.10.2026:** Die v0.4.2.3 wurde erfolgreich auf dem ESP32-C3 kompiliert und geflasht. Lauftexte, Symbole, verschiedenfarbige Schriften und Blockseiten werden im Modus **„Alles als Laufband“** korrekt verkettet und angezeigt. Die Tafel scrollt ein passendes, vollständiges Band nach einem Upload autonom. Größere Laufbänder wurden in mehrere Gruppen zerlegt; die alte **20-Sekunden-Ersatzdauer** schnitt Inhalte vorzeitig ab. Bei einem späteren Upload wurde **Paket 70/106 nicht bestätigt** und die Tafel blieb im Hersteller-Fehlerbildschirm. Der ESP32-Heap war dabei knapp, aber ein RAM-Absturz wurde nicht nachgewiesen. Siehe [Testbefund](docs/TESTBEFUNDE_v0.4.2.3.md).

Für **v0.4.2.4-dev** haben wir gezielt nachgebessert:

- **Stabilität:** Auch gemischte Laufbänder verwenden beim Senden einen kleinen **384-Byte-Tile-Puffer**, keinen vollständigen 30-KiB-Bildpuffer. Nach einer nicht bestätigten Bildpaketübertragung wird die ganze Gruppe höchstens einmal nach BLE-Neuverbindung neu gesendet. Scheitert auch der Versuch, stoppt der Programmablauf mit Fehlerstatus.
- **Zeitsteuerung:** Die alte einstellbare Gruppendauer (5–180 s) entfällt aus dem Programmeditor und bestimmt die Wiedergabe nicht länger. Die Hardwareformel für Blockseiten bei Tempo 60–90 bleibt bestehen. Geteilte Laufbänder erhalten eine **vorsichtige Schätzung** aus Spaltenzahl und Tempo. Das ist noch keine kalibrierte Laufzeit und kann bewusst zu längeren Wiederholungen führen – vollständige Wiedergabe hat Vorrang vor einem vorzeitigen Gruppenwechsel.
- **Bedienung:** Eine gemeinsame, am unteren Rand angeheftete Tafelsteuerung statt mehrfacher Regler. Sie folgt in Größe und Anordnung unseren MS-Paint-Mockups; Kurzwahlkacheln sind kompakter. Acht Lucide-Direkttasten senden bei **jedem** Antippen den BLE-Befehl erneut, ohne vermeintlich aktiven Modus.
- **LED-Vorschau:** Kleine Lucide-Buttons ohne Textbeschriftung: Anfang, Play/Pause, Vorschaugeschwindigkeit **Schnecke (1×) → Vogel (2×) → Hase (4×)**. Die dynamisch wechselnden Icons bleiben erhalten, die unnötige Beschriftung „Lauftextvorschau“ entfällt.
- **Menü und Editor:** Menüüberschrift „Navigation“ entfällt, Diagnose verwendet Lucide `bug`. Technische WLAN-/Speicherhinweise werden nicht mehr im Editor angezeigt, sondern bleiben in Diagnose/Dokumentation.
- **Repository:** Roadmap und Themenspeicher wurden mit den bestätigten Tests abgeglichen. Neue Community-Dateien (`CONTRIBUTING.md`, `SECURITY.md`, Issue-/PR-Vorlagen, Verhaltenskodex) erleichtern Rückmeldungen. Unser Quellcode bleibt unter **MIT**; die getrennten Lucide/Feather-Rechte stehen in `THIRD_PARTY_NOTICES.md` und sind in `LICENSE` referenziert.

**Wichtig:** Die damalige v0.4.2.4.1-dev wurde noch **nicht auf der echten Tafel abgenommen**; lokale Browser-/Quelltests ersetzen weder Arduino-Kompilierung noch Hardware-, RAM- und BLE-Tests. Speichergrößen und Runtime-Timing für geteilte Gruppen müssen wir erneut prüfen.

## Installation und Datensicherheit

1. Vorab ein JSON-Backup aus der aktuell laufenden Firmware erstellen und lokal aufbewahren. Die bisherige Firmware als Rückfallversion sichern.
2. `firmware/RavestickBoard_v0_4_3/RavestickBoard_v0_4_3.ino` öffnen. **Alle sechs Dateien** im selben Sketch-Ordner belassen: `.ino`, `RavestickBoard_WebUI.h`, `RavestickBoard_Assets.h`, `RavestickBoard_Library.h`, `RavestickBoard_v042.h` und `RavestickBoard_Icon.h` (insgesamt **sechs Dateien**).
3. Arduino IDE: **ESP32C3 Dev Module**, Espressif **3.3.12**, **USB CDC On Boot: Enabled**, **No OTA (2MB APP/2MB SPIFFS)**. Bestehendes Partitionsschema **nicht ändern**, **Erase All Flash deaktiviert** lassen.
4. Nach dem Flashen mit WLAN `CoolLED1248` verbinden, `http://192.168.4.1` aufrufen. Bibliothek nicht neu formatieren. Testplan schrittweise abarbeiten. Falls das Homescreen-Icon nicht übernommen wird, die Verknüpfung über Safari erneut hinzufügen.

Siehe [aktuelle Update-Hinweise](UPDATE_HINWEISE_v0.4.3.txt) oder [vorherige ausführliche Installation](docs/INSTALLATION_v0.4.2.4.1.md), [Testplan](docs/TESTPLAN_v0.4.2.4.1.md), [Bibliotheksformat](docs/BIBLIOTHEK_v0.4.1.md), [Changelog](CHANGELOG.md), [Roadmap](docs/ROADMAP.md) und [Themenspeicher](docs/THEMENSPEICHER.md).

## Mitmachen und Sicherheit

Siehe [CONTRIBUTING.md](CONTRIBUTING.md), [SECURITY.md](SECURITY.md), [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md) und unsere [Issue-Vorlagen](.github/ISSUE_TEMPLATE/).

## Wie wir entwickeln

Wir entwickeln RavestickBoard gemeinsam: Wir entwerfen, programmieren, dokumentieren und testen die Software im engen Austausch. Die realen ESP32-/CoolLEDX-Hardwaretests führen wir vor Ort durch; dabei unterstützen KI-gestützte Werkzeuge wie **ChatGPT und Claude** die Konzeption, Fehlersuche, Implementierung und Dokumentation. Wir unterscheiden ausdrücklich zwischen lokal geprüftem Code und hardwarebestätigten Funktionen.

## Entwicklung und Quellen

RavestickBoard ist eine eigenständige C++-/HTML-/JavaScript-Implementierung. Die beiden aktuellen Referenzen für zukünftige Animationsforschung sind [jean-santos/coolled1248-rs](https://github.com/jean-santos/coolled1248-rs) und [zackmcmurrin-dev/coolledux-linux-uploader](https://github.com/zackmcmurrin-dev/coolledux-linux-uploader). Ergänzend dokumentieren wir den früh untersuchten [UpDryTwist/coolledx-driver](https://github.com/UpDryTwist/coolledx-driver) und [ohtaras/led](https://github.com/ohtaras/led). Die technischen Protokollfakten wurden für unsere Firmware eigenständig umgesetzt. Ob eine Datenformatvariante des anderen Panels auf unserer eigenen CoolLEDX funktioniert, ist separat zu prüfen.

**Lizenz und Drittmaterialien:** Unser selbst entwickelter RavestickBoard-Code steht unter der **[MIT-Lizenz](LICENSE)** (Copyright © 2026 du13500). Die Lizenz erlaubt ausdrücklich auch kommerzielle Nutzung, Bearbeitung und Weitergabe unter ihren Bedingungen. Das Projekt wird derzeit als privates Hobbyprojekt entwickelt; das schränkt die MIT-Rechte nicht ein. Eingebettete **Lucide-Icons** stehen unter ISC, ausgewählte von Feather abgeleitete Icons zusätzlich unter MIT. Ihre erforderlichen Copyright- und Lizenzhinweise stehen in [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md). Fremde Originalmotive, Marken und sonstige Drittmaterialien werden durch unsere MIT-Lizenz nicht automatisch mitlizenziert.

<p align="center"><strong>Made for the rave, powered by a banana. 🍌</strong><br><sub>Independent project, not affiliated with the CoolLED1248/CoolLEDX manufacturer.</sub></p>
