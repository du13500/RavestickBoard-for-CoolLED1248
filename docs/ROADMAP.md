> Stand v0.6.1.0-dev (3.11.2026): Der Hardwaretest des Animationstests (0.6.0.2) ist bestanden: `0x04` wird angenommen, Farben/Nummern/Richtung stimmen, 1–8 ms laufen flüssig, die Animation bleibt nach dem Ausschalten, ein Text ersetzt sie sauber; Anzeigemodus und Tempo-Regler wirken nicht auf Animationen. Darauf aufbauend ist **0.6.1.0** gebaut: Animationen als eigener Bereich mit Import (GIF, Bildstreifen, Text-Raster), Anpassen, Speichern als Dateien im ESP32, Liste und Senden aus der Datei; Arduino-Compile und Hardwaretest offen, siehe [`TESTPLAN_v0.6.1.0.md`](TESTPLAN_v0.6.1.0.md). Nächste Schritte: 0.6.1.1 Animationen als Kurzwahl, 0.6.2.0 Zeichen-Editor, 0.6.3.0 Komfort (Abschnitt „Animationen – Plan“ weiter unten). Die darunter stehenden älteren „Stand“-Hinweise sind Verlauf und nicht mehr maßgeblich.
>
> Stand v0.5.0.1-dev: wie 0.5.0.0 ohne Kopfzeilen-Chip; 0.5.0.0 wurde getestet und für gut befunden („funktioniert wie vorgeschlagen“). Tafelverwaltung gebaut (Meine Tafel im ESP32, Suche mit Signalstärke, fremde Tafel mit Rückweg; keine Adresse mehr im Code), dazu Zoom-Sperre, Palettentaste in der Farbzeile und Listenhöhen (Hardwaretest offen). Offen und möglicher Inhalt der nächsten Stände: Hardwaretest der Tafelverwaltung (insbesondere Adresstyp bei fremden Tafeln, Namensfilter), Aufräumen der Bedienung (Rot nur für endgültiges Löschen, alte Tafelsteuerungs-Reste und alte Überschriftensymbole entfernen), schnellere Bibliotheksliste, ACK-Auswertung (F-03), Maßnahmen gegen den seltenen Verbindungshänger (Verbindungsaufbau mit eigener Zeitüberwachung, NimBLE), Tafeln mit anderer Pixelgröße als 64×16 (Hinweis in der Oberfläche; Anpassung nur, wenn das Protokoll die Maße liefert). Festlegung: kein gleichzeitiger Betrieb mehrerer Tafeln.
>
> Davor v0.4.6.0-dev: Zeichen-Editor für Blockseiten und Bluetooth-Zeichen „verbunden“ während der Übertragung (Hardwaretest bestanden; ein Verbindungshänger bei sichtbarer Tafel wurde vom Wächter nach 25 s behoben).
>
> Davor v0.4.5.1-dev: Vor-Scan der Tafel und Bluetooth-Zeichen (Hardwaretest des Vor-Scans: bestanden, siehe Rückmeldung zu 0.4.5.1).
>
> Stand v0.4.5.0-dev: Zeichen-Fenster mit Wischen nach Richtung, Doppeltipp-Wort, fester Vorschau und Farbleiste; Kurzwahl-Dialog mit Quellen-Schaltflächen; WLAN auf höchstens zwei Geräte begrenzt (Hardwaretest offen). Nächster Schritt ist **0.4.6.0: Zeichen-Editor für Blockseiten** (gleiches Fenster; Firmware-Seitendarstellung muss die Stilmarken kennen; was über 64 Pixel hinausragt, entfällt wie bisher) mit Rückgängig/Wiederherstellen im Blockeditor. Festlegung: Wir bleiben bei der bisherigen BLE-Architektur; NimBLE ist zurückgestellt. Offene Tests: Wiederanlauf (mit/ohne Abmelden), Zwei-Geräte-Lauf (jetzt mit Obergrenze 2), Gesten auf dem iPhone.
>
> Davor v0.4.4.2-dev: schlankes Startbildschirm-Bedienfeld, sofortiges Sortieren und weniger Anfragen an den ESP32 (Weboberfläche; Hardwaretest offen). Entscheidung offen: Bibliotheksliste in der Firmware beschleunigen (RAM-Zwischenspeicher der Listenfelder oder nur Kopfdaten lesen; derzeit rund 0,9 s je Abruf, Webserver währenddessen blockiert) und der BLE-Hänger `ble_regNotify` bei niedrigem Heap (NimBLE, weniger gleichzeitige Clients, weniger Last je Anfrage).
> 
> Davor v0.4.4.1-dev: Feinschliff des Lauftext-Zeichen-Editors, sortierbare Programme, größeres Eingabefeld (alles Hardwaretest offen). Nächster Schritt ist **0.4.5.0: Zeichen-Editor für Blockseiten** (gleiches Fenster; Firmware-Seitendarstellung muss die Stilmarken kennen; was über 64 Pixel hinausragt, entfällt wie bisher) mit Rückgängig/Wiederherstellen im Blockeditor. Danach: Bedienung aufräumen (Rot nur für endgültiges Löschen, alte Tafelsteuerungs-Reste und alte Überschriftensymbole entfernen, Kopf von Bibliothek/Editor), schnellere Bibliotheksliste, ACK-Auswertung (F-03). Offen aus 0.4.3.4: Wiederanlauftest (mit/ohne Abmelden) und Zwei-Geräte-Test; NimBLE und Scroll-Sperre/App-Shell (iOS) zurückgestellt.

> Stand v0.4.4.0-dev: Lauftext-Editor „Zeichen gestalten“ gebaut (Hardwaretest offen). Als Nächstes: Zeicheneditor für Blockseiten (Frage der 64-Pixel-Prüfung in `renderBlockPage` bei Überlagerungen klären), dann Aufräumen der Bedienung. Die Tests zu v0.4.3.4 (Wiederanlauf, zwei Geräte) stehen weiter aus; NimBLE bleibt zurückgestellt.

> Stand v0.4.3.4-dev: Wiederanlauf nach Wächter-Neustart gezielt prüfen (Test-Auslöser `/api/test/hang`, mit/ohne Abmelden der Geräte); danach Lauf mit zwei Geräten gleichzeitig. NimBLE-Umstieg zurückgestellt, bis der Zwei-Geräte-Lauf zeigt, ob der Speicher wirklich knapp wird. Der BLE-Hänger selbst ist selten und nicht reproduzierbar (94 Verbindungen ohne Hänger).

> Stand v0.4.3.3-dev: Der ESP32 hing nach einem Tafel-„ERROR“ im Blockband-Upload (BLE-Bibliothek wartet ohne Zeitlimit). Als Nächstes: Hänger nachstellen, Absturzspur `[VORLAUF]` auslesen, die betroffene Wartestelle gezielt absichern (z. B. BLE-Schritte mit Zeitlimit oder in eigenem Task). Danach die Messreihen unten.
Messreihen: Messreihen A–C auf 0.4.3.2 liegen vor (Einbruch beim Beitritt des zweiten Clients bis 192 B; Diagnose-Handler konstant ~300 ms). Als Nächstes: Lauf A, C und D (BLE-Übertragung mit einem und zwei Clients) auf v0.4.3.3 mit RAM-Export und seriellem Log nach Reset; danach entscheiden über schnellere Bibliotheksliste (Kopfzeilen statt voller Einträge, Frage der defekten Einträge) und verzögerten BLE-Start; danach BLE-Bestätigungen auswerten (F-03). Einsatzfall: mehrere Bediener, also zwei Clients müssen stabil laufen.

> Stand v0.4.3.2-dev: Programme-Ansicht im Bibliotheksstil (nur UI).

> Stand v0.4.2.5.3-dev: Suche und manuellen Reload aus Bibliothek entfernt; acht Kurzwahlen bleiben. Kritische Heap-Minima weiter offen.

> Stand v0.4.2.5.2-dev: Acht Kurzwahlen, adaptive Startseite und Lucide-Bootscreen implementiert, Hardwareprüfung offen. **Heap-Stabilität weiterhin offen.**

## Als Nächstes nach v0.4.2.5-dev (Hardwarestatus offen)

- Speicher-Hardening: Extremwerte unter 1 KiB in WLAN- und HTTP-Testläufen, weiter eingrenzen; keine voreilige Entwarnung.
- API-/UI-Optimierung v0.4.2.5 am ESP32 verifizieren; keine neue Datenmigration.
- Danach: BLE-ACK-Decoding und native Opcode-0x04-Animationen nach Adam Schulz / OpenSign untersuchen (MIT-Attribution bei späterer Codeübernahme). **Stand 3.11.2026:** Antwortcodes werden seit 0.6.0.0 gelesen und gezählt; `0x04` ist als Animationstest gebaut, Hardwaretest offen.

# RavestickBoard Roadmap

## Aktuell · 29.10.2026 – v0.4.2.4.2-dev-A/B

**Zwei getrennte Diagnose-/Optimierungsbuilds, noch nicht Arduino-kompiliert oder hardwaregeprüft.**

- [x] **v0.4.2.4.1 auf Hardware:** kurzer gemischter Laufband-Upload inkl. 16 Leer-Pixelspalten; großes geteiltes Laufband ohne 20-s-Abschneiden; gemischtes Standardprogramm funktioniert näherungsweise synchron; Kurzwahlen, Modi, Preview, Bibliothek und Backup getestet; etwa drei Stunden Laufzeit und mehrere WLAN-Clients. F-06 `.bak`-Löschfehler im Hotfix behoben.
- [ ] **RAM-Sicherheitsreserve:** Bei etwa 2:55 h wurde ein historischer System-Heap-Tiefstwert von **1.076 Byte** bei zwei WLAN-Clients beobachtet. Kein kontinuierliches Speicherleck belegt; Ursache offen. BLE-Paketfehler konnte nicht reproduziert werden.
- [ ] **Build A hardwaretesten:** HTTP-Endpunkt-/404-Profiling, temporäre 20-ms-Speicherminima; Webseite weiter unkomprimiert. Beim Vergleich jeweils vor Messreihe neu starten.
- [ ] **Build B hardwaretesten:** identischer Profiler, gzip-9, Inhalts-ETag und 304; 200- und 304-Antworten getrennt auswerten. A/B mit einem und zwei WLAN-Clients vergleichen.
- [ ] **Weiter offen:** F-01/02/03 (Schema, Webserver während BLE und echte Paketantwortprüfung; F-03: Antwortcodes werden seit 0.6.0.0 gelesen und gezählt, strikt ausgewertet nur im Animationstest), F-04/05/07/08/09, S-01–S-05, Lauftextkalibrierung und Animationen. Der neue HTTP-Profiler ersetzt keine Hardwareabnahme.
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

## Animationen – Plan (3.11.2026)

Entschieden (Absprache vom 3.11.2026): Animationen sind GIFs und „gezeichnete“ Bildfolgen und bilden einen **eigenen Bereich**, losgelöst von den bisherigen Funktionen (Lauftext, Seiten, Programme, Effekte, Diashow). Die Tafel speichert und spielt die Animation selbst (Befehl `0x04`); ein Inhalt zugleich auf der Tafel. Grenzen aus dem Protokoll: höchstens etwa 30 KiB, das sind 79 Frames zu 384 Byte, ein globales Tempo je Animation (Millisekunden je Frame), kein Zeitwert pro Frame (Halten durch wiederholte Frames).

| Version | Inhalt | Voraussetzung |
|---|---|---|
| **0.6.0.0 bis 0.6.0.2** | Hardwaretest: `0x04` mit Testmustern (ab 0.6.0.1 mit Frame-Nummer), Antwortcodes, Streaming-Grundlage (**gebaut und auf der Tafel bestanden**) | Test auf der Tafel |
| **0.6.1.0** (**gebaut, Compile und Hardwaretest offen**) | Import und Animationsliste: GIF (im Browser zerlegt, auf sieben LED-Farben reduziert), Bildstreifen, Text-Raster mit Buchstaben `.RGBCMYW` (für den Excel-Weg), Anpassungswerkzeuge (Zuschnitt/Skalierung auf 64×16, Frames auswählen/entfernen, Tempo, Farbreduktion, Vorschau); Ablage als Dateien auf LittleFS (je höchstens 30 KB); Senden an die Tafel | Test 0.6.0.0 bestanden; Größe der LittleFS-Partition aus dem Partitionsschema prüfen |
| **0.6.1.1** | Animationen als Kurzwahl (Kachel sendet eine gespeicherte Animation; Quelle „Animationen“ im Kurzwahl-Dialog, bisher ausgegraut) | 0.6.1.0 getestet |
| **0.6.2.0** | Zeichen-Editor: Stift, Frames, Füllen, Linie, Rechteck, Pipette, Rückgängig; Eingabe per Zeigerereignisse (iPad mit Apple Pencil), eigenes Zoomen/Verschieben, Bildstreifen der Frames | 0.6.1.0 |
| **0.6.3.0** | Komfort: Auswahl verschieben, Spiegeln, Zwiebelschicht (Vorbild-Frame), lokale Ablage im Browser | 0.6.2.0 |

**Stand 0.6.1.0:** Dateien liegen als `/rb-anim/a<ID>.rba` im Dateisystem der Bibliothek (LittleFS); die Größe der Partition wurde nicht aus dem Partitionsschema gelesen, der Sketch prüft den freien Platz zur Laufzeit (Reserve 24 KiB, höchstens 40 Animationen). Das Warten auf die Bestätigung je Paket ist unverändert (feste Pause plus Antwortprüfung); eine Beschleunigung bleibt Kandidat nach der Messung der Übertragungsdauer. Weitere Ideen (nicht scrappen): die Testbilder (Nummer, Vollbildfarbe, Balken) als Grundlage für Zähler oder Uhrzeit, siehe Themenspeicher.

Der Editor dient zum Nachbessern importierter Animationen, muss aber auch eine Animation vollständig erzeugen können. Vor jeder Stufe: Ergebnis des vorherigen Hardwaretests abwarten; was die Tafel nicht annimmt, wird nicht gebaut. Aus Adams Mail (3.11.2026) zusätzlich vorgemerkt: Der Upload dauert bei ihm für 79 Frames etwa 55 s, vor allem wegen des Wartens auf die Bestätigung jedes Chunks; ein echtes Warten auf die Antwort statt fester Pause könnte ihn beschleunigen (Kandidat für 0.6.1.0, nach Messung in Test C1). Der Flackertrick (ein Bild mit mehr wahrgenommenen Farben durch sehr kurze Frames, etwa 8 ms) und „Seitenprogramm als Animation ohne Prozentanzeige“ sind spätere Ideen, die an die Trennung „Animation als eigener Bereich“ rühren und erst nach dem Test entschieden werden. Offen: Verhalten nach dem Ausschalten, Zusammenspiel mit `0x06` und mit einem späteren Text-Upload, Zeit für 30 KB über BLE (Dauer der Übertragung, Prozentanzeige), RAM-Bedarf beim Zerlegen großer GIFs (im Browser, nicht im ESP32), Flashbedarf je Animationsdatei.

## Offene Produktfragen

- 🔬 **Lauftext-Zykluszeit:** Pixelbreite und Geschwindigkeit auf echter Tafel messen und eine zuverlässige Kurvenformel bestimmen. Die alte 5–180-s-Ersatzgruppe wurde in v0.4.2.4 aus der Wiedergabe entfernt; ein genaues Lauftextzeitmodell fehlt weiterhin.
- 🔬 **Prozentanzeige:** Nur notwendige Uploads beim Wechsel zwischen Lauftext- und Blockgruppen oder bei Überschreitung der sicheren Datenmenge; ein unsichtbares Vorladen ist nicht bestätigt und wird nicht vorausgesetzt.
- 🔬 **Grenzen der Blockkalibrierung:** Messpunkte für Tempo unter 60 und über 90 und exakten Start nach Upload; keine Garantie auf präzise Bildwechsel ohne Tafel-Rückmeldesignal.
- 🔬 **Datenbudget 30 KiB:** Vorsichtiger Softwarewert, keine bestätigte Hardwaregrenze. Technische Prüfung fortsetzen; Zähler im Webinterface bleibt sichtbar.

## Weitere Entwicklung nach Hardwareabnahme

- 🔬 **BLE-Recovery:** Nach fehlender Paketbestätigung maximal einen kontrollierten vollständigen Neuversuch der ganzen Übertragung mit neuem Verbindungsaufbau. Ein in v0.4.0.3 beobachteter `MTU=0`-Reconnect war erfolgreich; ein automatischer Komplettretry ist in v0.4.2.4 implementiert, aber noch nicht hardwarebestätigt.
- 🔬 **Speichergrenzen:** Untersuchung des kurzzeitig gemessenen historischen Heap-Minimums bei mehreren WLAN-Clients, sichere Browser-HTTP-Statusabfragen während langen BLE-Übertragungen.
- 🔬 **Bibliothekskapazität:** Der v0.4.2-Index hat zunächst 192 Slots und prüft Flashreserve. Langfristig variable Datensatzlängen, echte kapazitätsabhängige Indexstruktur und Stromausfalltests.
- 🔬 **Animationen (überholt durch den Plan unten):** ursprünglich ein nur seriell angesteuerter Test des Opcode `0x04`. Umgesetzt als Animationstest in den Einstellungen (0.6.0.0). Das Python-RGB444/LZSS-Programmformat bleibt ohne Hardwaretest als nicht kompatibel anzunehmen.
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
