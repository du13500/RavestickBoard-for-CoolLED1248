# RavestickBoard Roadmap

**Lesart:** „Programmiert“ ist nicht dasselbe wie „auf CoolLEDX hardwarebestätigt“. Alte Entwicklungsentscheidungen sind im Changelog und in den Versionsberichten archiviert. Die folgenden Themen sind der aktuelle Plan.

## Bereits bestätigt

- ✅ v0.1.x: Eigenes ESP32-C3-WLAN, Webinterface und direkte CoolLEDX-BLE-Textübertragung.
- ✅ v0.2.0 Korrektur 1: Mehrere NVS-Blockseiten, Einzelübertragung, Basis-Seitenplayer; bei jedem automatischen Wechsel noch kompletter Neu-Upload.
- ✅ v0.3.x: Herstellerfonts und Pixelart, zusätzliche Inline-Symbole, Mini-Zeilen, Live-Steuerbefehle. Einzelne Pixel- und Abstandsfragen bleiben offen.
- ✅ v0.4.0.x: Modus- und Ambiente-Steuerung, RAMTRACE, Streaming-Lauftext mit 384-Byte-Kacheln. Mehrfach große BLE-Übertragungen bestätigt, achtstündiger Stabilitätstest. Ein einmaliger 436-Byte-Heap-Tiefstwert bleibt ein Untersuchungsfall.
- ✅ v0.4.1: LittleFS-Textbibliothek und sechs Kurzwahlen in ersten Nutzertests grundsätzlich erfolgreich. Umfangreiche Stromausfall-/Langzeittests fehlen weiterhin.

## v0.4.2-dev · Zum Hardwaretest bereit

- 🟡 Blockeditor 25 Seiten, getrenntes Leeren, gespeicherte bearbeitbare Blockfolgen.
- 🟡 Mehrere fertige 64×16-Seiten als Pixelband in einem einzigen BLE-Auftrag; native automatische Seitenaufteilung, Wiederholung und Temporegelung sind vom Nutzer bereits mit langen Standbild-Lauftexten beobachtet worden. Unser neuer kombinierter Renderer selbst ist noch ungetestet.
- 🟡 Gespeicherte Programme mit mehreren Bibliothekseinträgen und Gruppierung kompatibler Bilder/Schriften.
- 🟡 Neue Kurzwahlquellen, eigene Beschriftungen, JSON-Schema 2 und Legacy-Bibliothek-Kompatibilität.
- 🟡 Apple-Touch-Icon, Menü-Refresh, verschlankte Bibliothek und überarbeitete Dokumente.
- 🔬 Vor Freigabe: Arduino-Kompilierung, Block-BLE-Test, Programmlauf, Legacy-Flashbestand, Export/Import, RAM-Diagnose und Langzeittest.

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
- Keine über die MIT-Lizenz hinausgehende Rechtebehauptung an Herstellerfonts/-grafiken.
- Kein automatisches Öffnen/Formatieren unbekannter LittleFS-Dateisysteme.

Referenzen und externe Forschung stehen in [QUELLEN_UND_LIZENZEN.md](QUELLEN_UND_LIZENZEN.md), laufende Detailideen im [THEMENSPEICHER.md](THEMENSPEICHER.md) und die Hardwaretestliste in [TESTPLAN_v0.4.2.md](TESTPLAN_v0.4.2.md).
