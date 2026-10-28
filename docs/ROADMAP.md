# RavestickBoard Roadmap

**Lesart:** „Programmiert“ ist nicht dasselbe wie „auf CoolLEDX hardwarebestätigt“. Alte Entwicklungsentscheidungen sind im Changelog und in den Versionsberichten archiviert. Die folgenden Themen sind der aktuelle Plan.

## Bereits bestätigt

- ✅ v0.1.x: Eigenes ESP32-C3-WLAN, Webinterface und direkte CoolLEDX-BLE-Textübertragung.
- ✅ v0.2.0 Korrektur 1: Mehrere NVS-Blockseiten, Einzelübertragung, Basis-Seitenplayer; bei jedem automatischen Wechsel noch kompletter Neu-Upload.
- ✅ v0.3.x: Herstellerfonts und Pixelart, zusätzliche Inline-Symbole, Mini-Zeilen, Live-Steuerbefehle. Einzelne Pixel- und Abstandsfragen bleiben offen.
- ✅ v0.4.0.x: Modus- und Ambiente-Steuerung, RAMTRACE, Streaming-Lauftext mit 384-Byte-Kacheln. Mehrfach große BLE-Übertragungen bestätigt, achtstündiger Stabilitätstest. Ein einmaliger 436-Byte-Heap-Tiefstwert bleibt ein Untersuchungsfall.
- ✅ v0.4.1: LittleFS-Textbibliothek und sechs Kurzwahlen in ersten Nutzertests grundsätzlich erfolgreich. Umfangreiche Stromausfall-/Langzeittests fehlen weiterhin.

## v0.4.2.3-dev · Aktueller Entwicklungsstand

- ✅ **Auf Hardware bestätigt (v0.4.2.2):** Gemischte Programmwechsel grundsätzlich möglich, reine Lauftext- und Blockfolgen autonom, Bibliotheksbearbeitung und -reparatur funktionieren. Aber Übergänge noch nicht exakt synchron.
- 🛠️ **Alternative Laufbandwiedergabe:** Neu: Texte und Blockseiten in einem fortlaufenden scrollenden Bild, mit 16 Spalten Abstand. Hardwaredarstellung und Teilung an der Payloadgrenze noch nicht bestätigt.
- 🛠️ **Blockzyklus-Kalibrierung:** Fünf reale Messungen stützen `T ≈ N × 0,0795 × (100−V)` Sekunden für V=60–90. Automatische Gruppenlaufzeit nur für Blöcke in diesem Bereich; außerhalb und bei Lauftexten Ersatzzeit 5–180 s. Kalibrierung der Pixelgeschwindigkeit von Lauftexten bleibt offen.
- 🛠️ **Datenkonsistenz:** Löschen referenzierter Vorlagen, Kurzwahlen automatisch freigeben, unvollständige Programme anzeigen und Reparatur verlangen. Echte Stromausfall- und Mehrfachlöschtests offen.
- 🛠️ **Editor/UX:** visuell eigener Bearbeitungsmodus und Aktualisieren derselben ID, direkte Bibliotheksrückkehr, Header/Preview-Fix, reduzierte Statusmeldungen.
- 🔬 **Abnahme:** Arduino-ESP32-Kompilierung und Flash; gemischtes Programm TEXT → BAND → TEXT auf der echten LED-Tafel; automatischer 6-Seiten-Zyklus bei Tempo 80; Bibliothekseintrag löschen (auch wenn Programm/Kurzwahl ihn verwendet); Neustart und Backup-Kontrolle.

## Offene Produktfragen

- 🔬 **Lauftext-Zykluszeit:** Pixelbreite und Geschwindigkeit auf echter Tafel messen und eine zuverlässige Kurvenformel bestimmen. Bisher ist die 5–180-s-Gruppe ein bewusst dokumentierter Kompromiss.
- 🔬 **Prozentanzeige:** Nur notwendige Uploads beim Wechsel zwischen Lauftext- und Blockgruppen oder bei Überschreitung der sicheren Datenmenge; ein unsichtbares Vorladen ist nicht bestätigt und wird nicht vorausgesetzt.
- 🔬 **Grenzen der Blockkalibrierung:** Messpunkte für Tempo unter 60 und über 90 und exakten Start nach Upload; keine Garantie auf präzise Bildwechsel ohne Tafel-Rückmeldesignal.
- 🔬 **Datenbudget 30 KiB:** Vorsichtiger Softwarewert, keine bestätigte Hardwaregrenze. Technische Prüfung fortsetzen; Zähler im Webinterface bleibt sichtbar.

## Nächste Schritte, nach Hardwareabnahme

- 🔬 **BLE-Recovery:** Nach fehlender Paketbestätigung maximal einen kontrollierten vollständigen Neuversuch der ganzen Übertragung mit neuem Verbindungsaufbau. Ein in v0.4.0.3 beobachteter `MTU=0`-Reconnect war erfolgreich; ein automatischer Komplettretry existiert noch nicht.
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

Referenzen und externe Forschung stehen in [QUELLEN_UND_LIZENZEN.md](QUELLEN_UND_LIZENZEN.md), laufende Detailideen im [THEMENSPEICHER.md](THEMENSPEICHER.md) und die Hardwaretestliste in [TESTPLAN_v0.4.2.3.md](TESTPLAN_v0.4.2.3.md).
