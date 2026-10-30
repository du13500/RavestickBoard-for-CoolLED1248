> Stand v0.4.3.2-dev: Programme-Ansicht im Bibliotheksstil (nur UI). Als Nächstes: Heap-Messreihen A–D (ein/zwei Clients, mit/ohne Diagnose-Tab, mit BLE-Übertragung) gegen die neuen HTTPTRACE-Worst-Cases auswerten; danach BLE-Bestätigungen auswerten (F-03).

> Stand v0.4.2.5.3-dev: Suche und manuellen Reload aus Bibliothek entfernt; acht Kurzwahlen bleiben. Kritische Heap-Minima weiter offen.

> Stand v0.4.2.5.2-dev: Acht Kurzwahlen, adaptive Startseite und Lucide-Bootscreen implementiert, Hardwareprüfung offen. **Heap-Stabilität weiterhin offen.**

## Als Nächstes nach v0.4.2.5-dev (Hardwarestatus offen)

- Speicher-Hardening: Extremwerte unter 1 KiB in WLAN- und HTTP-Testläufen, weiter eingrenzen; keine voreilige Entwarnung.
- API-/UI-Optimierung v0.4.2.5 am ESP32 verifizieren; keine neue Datenmigration.
- Danach: BLE-ACK-Decoding und native Opcode-0x04-Animationen nach Adam Schulz / OpenSign untersuchen (MIT-Attribution bei späterer Codeübernahme).

# RavestickBoard Roadmap

## Aktuell · 29.10.2026 – v0.4.2.4.2-dev-A/B

**Zwei getrennte Diagnose-/Optimierungsbuilds, noch nicht Arduino-kompiliert oder hardwaregeprüft.**

- [x] **v0.4.2.4.1 auf Hardware:** kurzer gemischter Laufband-Upload inkl. 16 Leer-Pixelspalten; großes geteiltes Laufband ohne 20-s-Abschneiden; gemischtes Standardprogramm funktioniert näherungsweise synchron; Kurzwahlen, Modi, Preview, Bibliothek und Backup getestet; etwa drei Stunden Laufzeit und mehrere WLAN-Clients. F-06 `.bak`-Löschfehler im Hotfix behoben.
- [ ] **RAM-Sicherheitsreserve:** Bei etwa 2:55 h wurde ein historischer System-Heap-Tiefstwert von **1.076 Byte** bei zwei WLAN-Clients beobachtet. Kein kontinuierliches Speicherleck belegt; Ursache offen. BLE-Paketfehler konnte nicht reproduziert werden.
- [ ] **Build A hardwaretesten:** HTTP-Endpunkt-/404-Profiling, temporäre 20-ms-Speicherminima; Webseite weiter unkomprimiert. Beim Vergleich jeweils vor Messreihe neu starten.
- [ ] **Build B hardwaretesten:** identischer Profiler, gzip-9, Inhalts-ETag und 304; 200- und 304-Antworten getrennt auswerten. A/B mit einem und zwei WLAN-Clients vergleichen.
- [ ] **Weiter offen:** F-01/02/03 (Schema, Webserver während BLE und echte Paketantwortprüfung), F-04/05/07/08/09, S-01–S-05, Lauftextkalibrierung und Animationen. Der neue HTTP-Profiler ersetzt keine Hardwareabnahme.
- [ ] **Bedienungswünsche nach dem Speichervergleich:** Kapazitätsanzeige für 30-KiB-Gruppen im Programmeditor; „Als neuen Eintrag speichern“ bei Bibliotheksbearbeitung; kein kurzes Emoji-Logo beim Neuladen.

Siehe [Vergleichstest](VERGLEICHSTEST_v0.4.2.4.2.md). Historischer Status der vorherigen Versionen folgt unverändert.

---


**Lesart:** „Programmiert“ ist nicht dasselbe wie „auf CoolLEDX hardwarebestätigt“. Alte Entwicklungsentscheidungen sind im Changelog und in den Versionsberichten archiviert. Die folgenden Themen sind der aktuelle Plan.

## Historischer Stand aus v0.4.2.4.1 · 29.10.2026

**Hardware-Ergebnisse v0.4.2.4 (29.10.2026):** Kompilierung/Boot, Bestandserhalt und kurzes gemischtes Laufband mit 16 Leer-Pixelspalten bestätigt. Lange geteilte Laufbänder, RAMTRACE und BLE-Recovery noch offen. Die schwebende Steuerleiste war im Praxistest hinderlich; v0.4.2.4.1 legt sie in den Seitenfluss. Kurzwahl-Icons und sichere Löschung der Programm-`.bak` sind im Hotfix implementiert, aber noch nicht hardwareabgenommen.

**Review-Aufgaben:** F-06 (`.bak` beim Löschen entfernen) in v0.4.2.4.1 implementiert. F-01–F-05, F-07–F-12 und Aufräumbefunde S-01–S-06 bleiben als unabhängige Prüfpunkte bestehen (Datenschema, HTTP/BLE, ACK-Status, echte Verhaltenstests, Refactoring und Doku-Konsolidierung).

**Bereits auf Hardware bestätigt** (nicht automatisch für v0.4.2.4 regressionsgetestet):

- ✅ BLE-Senden, WLAN-Webinterface, Originalfonts/Pixelart, Mini-Schriften, Farben, Blockseiten, Live-Brightness/Tempo/Modus und Diagnose-Download (v0.1–v0.4.2).
- ✅ Bibliothek (LittleFS), sechs Kurzwahlen, sichere Aktualisierung vorhandener Einträge, Löschung mit referenziertem Programm, Reparatur des Programms und automatische Freigabe direkter Kurzwahlreferenzen (v0.4.2.2).
- ✅ Reine Lauftext- und Blockprogramme mit nur einem Upload und autonomer Wiederholung; gemischter Originalprogramm-Wechsel funktioniert grundsätzlich (v0.4.2.2), aber noch nicht lückenlos synchron.
- ✅ Blockseiten-Laufzeit bei Tempo 60/70/80/90 empirisch bestimmt, 4 Seiten bei Tempo 80 zuletzt **6,33 s** (v0.4.2.2).
- ✅ Neues **„Alles als Laufband“** mit Texten, 4 Blockseiten, Farben und Symbolen auf der Tafel sichtbar (v0.4.2.3). Problemfälle bei geteilten Gruppen dokumentiert.
- ✅ v0.4.2.3: Leere Kurzwahl öffnet Einrichtung; Lucide-Direktsteuerung und visueller Editor wurden im echten Browser geprüft. Beim Flashen half **115200 Baud** bei einem COM-Port-Fehler.

**In v0.4.2.4-dev implementiert, Hardwaretest ausstehend:**

- 🛠️ Tile-Streaming bei gemischten Laufbändern zur Reduktion des dynamischen Bildpufferbedarfs.
- 🛠️ Maximal **ein** vollständiger Neuversuch nach BLE-Paketbestätigungsfehler bei Bildübertragungen.
- 🛠️ Keine 20-s-Ersatzlaufzeit für geteilte Laufbänder; jetzt breite-/tempoabhängige **vorläufige** Schätzung. Lauftextkalibrierung weiter offen.
- 🛠️ Gemeinsame kompakte Tafelsteuerung, kleinere Kurzwahlen, Preview-Icons Schnecke/Vogel/Hase, `bug` für Diagnose, Menü ohne Zusatzüberschrift.
- 🛠️ Community-Dateien sowie synchronisierte Lizenz-/Drittanbieterhinweise.

**Nächste Abnahmeschritte:**

1. Firmware kompilieren und auf ESP32-C3 flashen, Altbestand nicht formatieren.
2. Gemischtes kurzes Laufband einmalig übertragen, Pixel-/Farbtreue prüfen.
3. Sehr großes Laufband (>30 KiB) teilen und mindestens **einen vollständigen Zyklus pro Gruppe** beobachten; keine vorzeitige Abschneidung tolerieren.
4. Bei absichtlich provoziertem BLE-Abbruch korrekte Erkennung, genau einen Retry und nachvollziehbaren Fehlerzustand nach erneutem Fehlschlag prüfen.
5. Alle Seiten bei 390 px Breite auf Scrollbarkeit, einheitliche Tafelsteuerung, Shortcut-Größen und unverdeckte Buttons prüfen.
6. RAMTRACE nach großen Übertragungen, mehrere WLAN-Clients und Wiederholungen beobachten. Ohne Test nicht als langzeitstabil markieren.

## Offene Produktfragen

- 🔬 **Lauftext-Zykluszeit:** Pixelbreite und Geschwindigkeit auf echter Tafel messen und eine zuverlässige Kurvenformel bestimmen. Die alte 5–180-s-Ersatzgruppe wurde in v0.4.2.4 aus der Wiedergabe entfernt; ein genaues Lauftextzeitmodell fehlt weiterhin.
- 🔬 **Prozentanzeige:** Nur notwendige Uploads beim Wechsel zwischen Lauftext- und Blockgruppen oder bei Überschreitung der sicheren Datenmenge; ein unsichtbares Vorladen ist nicht bestätigt und wird nicht vorausgesetzt.
- 🔬 **Grenzen der Blockkalibrierung:** Messpunkte für Tempo unter 60 und über 90 und exakten Start nach Upload; keine Garantie auf präzise Bildwechsel ohne Tafel-Rückmeldesignal.
- 🔬 **Datenbudget 30 KiB:** Vorsichtiger Softwarewert, keine bestätigte Hardwaregrenze. Technische Prüfung fortsetzen; Zähler im Webinterface bleibt sichtbar.

## Weitere Entwicklung nach Hardwareabnahme

- 🔬 **BLE-Recovery:** Nach fehlender Paketbestätigung maximal einen kontrollierten vollständigen Neuversuch der ganzen Übertragung mit neuem Verbindungsaufbau. Ein in v0.4.0.3 beobachteter `MTU=0`-Reconnect war erfolgreich; ein automatischer Komplettretry ist in v0.4.2.4 implementiert, aber noch nicht hardwarebestätigt.
- 🔬 **Speichergrenzen:** Untersuchung des kurzzeitig gemessenen historischen Heap-Minimums bei mehreren WLAN-Clients, sichere Browser-HTTP-Statusabfragen während langen BLE-Übertragungen.
- 🔬 **Bibliothekskapazität:** Der v0.4.2-Index hat zunächst 192 Slots und prüft Flashreserve. Langfristig variable Datensatzlängen, echte kapazitätsabhängige Indexstruktur und Stromausfalltests.
- 🔬 **Animationen v0.4.3 (Vorschlag):** isolierter, nur seriell angesteuerter Test des Opcode `0x04` mit vorhandenen sieben Ambiente-Farben als Frames. Danach optional GIF-Konvertierung; das Python-RGB444/LZSS-Programmformat ist nicht ohne Hardwaretest als kompatibel anzunehmen.
- 🎨 **Pixelart:** Variable Symbolbreiten bei 16-Pixel-Höhe (z. B. Flugzeug 40×16, Fahrzeuge/Szenen); vorher Renderer, Browser-Vorschau, Flashformat und BLE-Datenbudget anpassen.
- 🕒 **Programme:** Countdown, Stoppuhr, Scoreboard/Punktetafel, frei platzierbare Grafiken, gemischte Schrift-/Farbsegmente, zusätzliche Überblendeffekte und acht statt sechs Schnellzugriffe nur falls sinnvoll.
- 🔐 **Sicherheit/Ergonomie:** Individuelles WLAN-Passwort und Passwortschutz, dynamische BLE-Gerätesuche, Bedienung mehrerer Boards, Export/Import-Robustheit.

## Nicht im aktuellen Scope

- Keine Hersteller-Firmwareänderung oder Zugriff auf unbekannte Board-Speicherbereiche.
- Keine Rechtebehauptung an Herstellerfonts/-grafiken; die gesonderten Lucide/Feather-Icons behalten ihre ISC-/MIT-Lizenzhinweise.
- Kein automatisches Öffnen/Formatieren unbekannter LittleFS-Dateisysteme.

Referenzen und externe Forschung stehen in [QUELLEN_UND_LIZENZEN.md](QUELLEN_UND_LIZENZEN.md), laufende Detailideen im [THEMENSPEICHER.md](THEMENSPEICHER.md) und die Hardwaretestliste in [TESTPLAN_v0.4.2.4.md](TESTPLAN_v0.4.2.4.md).

## Release-Überlegung für v1.0 (30.10.2026)

- Erst die kritischen Heap-Minima unter WLAN-/HTTP-/BLE-Last beheben und am Gerät verifizieren.
- Für Release-Builds 20-ms-RAMTRACE und ausführliches HTTPTRACE entfernen bzw. über ein separates Entwickler-Build verfügbar lassen.
- Einen kompakten Fehlerbericht mit Firmwareversion, WLAN-/BLE-Status, Resetgrund und letzten Fehlern behalten.
- Programmverwaltung optisch/inhaltlich erst nach den noch ausstehenden Praxishinweisen überarbeiten.
