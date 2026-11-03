# Versionsgeschichte

Kurzüberblick über alle Versionen von RavestickBoard for CoolLED1248. Ausführliche Details stehen im [CHANGELOG](CHANGELOG.md), die zugehörigen Firmwarestände unter `firmware/RavestickBoard_v…`.

| Version | Datum | Wichtigste Änderungen | Anmerkungen |
|---|---|---|---|
| Experimente (BLE-Bring-up, FirstText) | 22.10.2026 | Erste BLE-Verbindung (`FFF0`/`FFF1`), Helligkeitsbefehl, erster eigener Pixeltext auf der Tafel | |
| v0.1.0 | 22.10.2026 | Erster Webcontroller: eigenes WLAN `CoolLED1248`, Weboberfläche, 7 Farben, Helligkeit, 5×7-Engschrift | |
| v0.1.1 | 22.10.2026 | Umbenennung zu „RavestickBoard“, Diagnose mit TXT-Export und BLE-Metriken, mobile UI-Korrekturen | |
| v0.1.2 | 22.10.2026 | Deutsche Oberfläche, Kleinbuchstaben und Umlaute, 64×16-LED-Vorschau mit Player, Standbild/Lauftext, Tempo-Regler | |
| v0.2.0 | 23.10.2026 | Bis zu 12 Blockseiten (NVS-Speicher), Seitenplayer, ß/ẞ; Korrektur 1: Web-UI in eigenen Header ausgelagert (Compile-Fix) | |
| v0.3.0-dev | 24.10.2026 | 82 Original-Glyphen (Normal/Bold), 24 Herstellersymbole plus Ravestick-Banane, Mini einzeilig, BLE-Befehle für Helligkeit/Tempo/Modus | |
| v0.3.1 | 24.10.2026 | Lauftext bis 80 Zeichen, Symbol-Overlay, Undo/Redo, Entwurf bleibt im Browser erhalten | |
| v0.3.2 | 25.10.2026 | Mobile Symbolpalette, zweizeiliger Mini-Lauftext, einheitliche Schriftauswahl; Korrekturen für Compiler und WLAN-RAM | |
| v0.3.3 | 25.10.2026 | Datenbasiertes Lauftextlimit (vorläufig 30 KiB) mit gestreamtem Rendering, zwei Zeilenfarben bei Mini zweizeilig | |
| v0.3.4 | 25.10.2026 | Live-Moduswahl über BLE (`0x01`–`0x08`), Cache-Schutz für die Startseite | |
| v0.3.5 | 25.10.2026 | LED Ein/Aus, Tempo 60–100, benannte Anzeigemodi, neue Piktogramme (Kussmund, Blume, Biene, Kuh, Ente, Kaktus, UFO, Alien) | Tempo-Skala unkalibriert |
| v0.3.5.1 | 25.10.2026 | Pixelart aus `Pixel-Art.xlsx` (Huhn, Banane, Gummiente, UFO, Küken, Frosch) | |
| v0.4.0-dev | 26.10.2026 | Neue Web-UX (Navigation über die Banane, 7 Seiten), Ambiente-Farben, globale Regler, Symbol-Picker, MIT-Lizenz | |
| v0.4.0.1-dev | 26.10.2026 | Navigation neu geordnet, 63 Symbole, Pixel-Piktogramme an den Reglern, Toast-Meldungen | |
| v0.4.0.2-dev | 27.10.2026 | RAM-Diagnose (RAMTRACE, `/api/ram.txt`), aktualisierte Pixelart-Sammlung | |
| v0.4.0.3-dev | 27.10.2026 | Stream-Renderer mit 384-Byte-Kacheln statt Vollbildpuffer, BLE-Neuverbindung bei kleiner MTU | |
| v0.4.1-dev | 27.10.2026 | Persistente Flash-Bibliothek (LittleFS, bis 64 Texte), 6 Kurzwahlen, JSON-Backup und -Import | |
| v0.4.2-dev | 28.10.2026 | 25-Seiten-Blockeditor als ein BLE-Bildband, 24 speicherbare Programme, Backup Schema 2 | Stack-Fehler beim Speichern |
| v0.4.2.1-dev | 28.10.2026 | Stack-Protection-Fault beim Speichern behoben, Editor- und Bedienungsbereinigung | |
| v0.4.2.2-dev | 28.10.2026 | Gemischte Programme laufen (Gruppenwechsel per Timer), Blockdauer kalibriert, Schutz bei gelöschten Bibliothekseinträgen | |
| v0.4.2.3-dev | 28.10.2026 | Modus „Alles als Laufband“, Lucide-Icons, besserer Backup-Import | Upload brach bei Paket 70/106 ab |
| v0.4.2.4-dev | 28.10.2026 | Kachelbasiertes Laufband, ein erneuter Upload bei fehlender Paketbestätigung, feste Gruppenzeit entfällt | |
| v0.4.2.4.1-dev | 29.10.2026 | Bedienungs-Hotfix: Tafelsteuerung nicht mehr schwebend, Kurzwahl-Icons, `.bak` wird beim Löschen mitentfernt | |
| v0.4.2.4.2-dev (A/B) | 29.10.2026 | Messbuilds für HTTP-/RAM-Protokoll; B zusätzlich mit gzip und ETag | Heap-Tiefstwert bis 592 B |
| v0.4.2.5-dev | 29.10.2026 | gzip/ETag/304 für die Web-UI, gepufferte JSON-APIs, Icons direkt im HTML, reiner MIT-Lizenztext | UI-Fehler, Heap offen |
| v0.4.2.5.1-dev | 30.10.2026 | UI-Hotfix: defekte `<span>`-Tags im Menü repariert, Startseite ohne Icon-Flackern | |
| v0.4.2.5.2-dev | 30.10.2026 | 8 Kurzwahlen (neues RBS3-Format), 4×2-Startseite mit Tempo-Regler, Favoriten-Oberfläche entfernt | |
| v0.4.2.5.3-dev | 30.10.2026 | Bibliothekssuche und manuelle Aktualisierung entfernt | |
| v0.4.2.5.4-dev | 30.10.2026 | Bibliothekseinträge im Editor überschreiben oder als neuen Eintrag speichern, Schriftwechsel-Fehler behoben | |
| v0.4.3-dev | 30.10.2026 | UI-Konsolidierung: Speichern, Blockwerkzeuge, Lucide-App-Icon und bildschirmfüllendes Ambiente | Diagnosedateien zeigten wegen veralteter zentraler Konstante noch v0.4.2.5.1-dev; historisches Heap-Minimum 224 B |
| v0.4.3.1-dev (Korrektur 1) | 30.10.2026 | Diagnoseausgabe streamen, Doppelaufrufe vermeiden, HTTPTRACE-Worst-Cases inkl. Pfad, Statusantwort optimieren, Ambiente wieder einspaltig, Blockwerkzeuge/Icon korrigieren | Läuft auf dem ESP32-C3, Übertragungen erfolgten; Heap-Messreihen offen |
| v0.4.3.2-dev | 30.10.2026 | Programme im Bibliotheksstil: Icon-Leiste, Zeilenliste, eigene Bearbeitungsansicht (nur Weboberfläche) | Kompiliert, läuft auf dem ESP32-C3, Ansicht freigegeben; Heap-Reihen A–C: Einbruch beim Beitritt eines zweiten Clients (192 B), Lauf D offen |
| v0.4.3.3-dev | 30.10.2026 | RAM-Messung (Start-Aufteilung, WLAN-Ereignisse, Sketch-Größe einmal, Schalter für Client-Limit und verzögerten BLE-Start, Standard aus), Hang-Wächter (Neustart nach 25 s ohne Lebenszeichen) und Absturzspur im RTC-Speicher; Anlass: ESP32 hing nach Tafel-„ERROR“ bei Blockband-Upload | Kandidat; Arduino-Compile und Hardwareprüfung offen |
| v0.4.3.4-dev | 31.10.2026 | Wächter meldet vor dem Neustart alle WLAN-Geräte ab; Test-Auslöser `/api/test/hang` für den Wiederanlauf (nur Entwicklungsbuild); Absturzspur nennt, ob abgemeldet wurde | Kandidat; Arduino-Compile und Hardwareprüfung offen |
| v0.4.4.0-dev | 31.10.2026 | Lauftext-Editor „Zeichen gestalten“ (Farbe und Schrift pro Zeichen, Farbwechsel, zweizeilig Mini), Steuerzeichen U+0090..U+009C im Text, einheitliche Sendefarbe, größere Regler | Kandidat; Arduino-Compile und Hardwareprüfung offen |
| v0.4.4.1-dev | 2.11.2026 | Wischen entmarkiert, Schrift direkt wählbar, Rückgängig/Wiederherstellen im Lauftext-Editor, Programme sortierbar (neue Firmware-Route), wachsendes Eingabefeld mit Ziehgriff, Startbildschirm mit voller Tafelsteuerung | Kandidat; Arduino-Compile und Hardwareprüfung offen |
| v0.4.4.2-dev | 2.11.2026 | Schlankes Startbildschirm-Bedienfeld (an, aus, Linkslauf, Standbild, Bearbeiten), Sortieren in Bibliothek/Programmen sofort mit Abgleich im Hintergrund, weniger Listenabrufe und pausierte Statusabfrage (nur Weboberfläche, Firmware sonst unverändert) | Kandidat; Arduino-Compile und Hardwareprüfung offen |
| v0.4.5.0-dev | 2.11.2026 | Höchstens zwei WLAN-Geräte (Firmware-Konstante), Zeichen-Fenster mit Wischen nach Richtung, Doppeltipp-Wort, fester Kopfzone und Farbleiste, schrumpfende Vorschau im Editor, Kurzwahl-Dialog mit Quellen-Schaltflächen, Knöpfe in Einstellungen/Diagnose angeglichen, Auswahl per zweitem Tipp aufheben | Kandidat; Arduino-Compile und Hardwareprüfung offen |
| v0.4.5.1-dev | 3.11.2026 | Vor-Scan der Tafel vor jedem neuen Verbindungsaufbau (gegen den Hänger bei ausgeschalteter Tafel), Bluetooth-Zeichen im Seitenkopf, genauere Zuordnung von Heap-Tiefständen in der Diagnose | Kandidat; Arduino-Compile und Hardwareprüfung offen |
| v0.4.6.0-dev | 3.11.2026 | Zeichen-Editor (Farbe und Schrift pro Zeichen) auch für Blockseiten, Firmware zeichnet Stilmarken auf Seiten, Bluetooth-Zeichen zeigt „verbunden“ während einer Übertragung | Kandidat; Arduino-Compile und Hardwareprüfung offen |
| v0.5.0.0-dev | 3.11.2026 | Tafelverwaltung (Meine Tafel im ESP32, Suche mit Signalstärke, fremde Tafel mit Rückweg, keine Adresse mehr in der Firmware), Zoom-Sperre, Palettentaste in der Farbzeile, Programme/Bibliothek ohne Scrollen | Kandidat; Arduino-Compile und Hardwareprüfung offen |
| v0.5.0.1-dev | 3.11.2026 | Chip „Meine Tafel/Fremde Tafel“ in der Kopfzeile entfällt (sonst gleich 0.5.0.0) | Hardwaretest bestanden („Perfekt.“) |
| v0.6.0.0-dev | 3.11.2026 | Animationstest: eingebaute Muster als gespeicherte Animation (Befehl `0x04`), Antwortcodes der Tafel in der Diagnose | durch 0.6.0.1 ersetzt (nicht geflasht) |
| v0.6.0.1-dev | 3.11.2026 | Testmuster zeigen die Frame-Nummer (zweistellig), Grenztest mit 300 ms, Balkenfarbe im Grenztest ohne Weiss | Kandidat; Arduino-Compile und Hardwaretest offen |

**Offene Punkte (Stand v0.6.0.1-dev):** Hardwaretest des Animationstests (nimmt die Tafel `0x04` an? Sind die Frame-Nummern lesbar und vollständig? Abspielgeschwindigkeit, Verhalten bei 79/80 Frames, Wirkung von `0x06` danach, bleibt die Animation nach dem Ausschalten, Antwortcodes in der Diagnose), danach Animationen als eigener Bereich (0.6.1.0 Import und Animationsliste, 0.6.2.0 Zeichen-Editor, 0.6.3.0 Komfort). Weiter offen: Hardwaretest gestylter Blockseiten und der Bibliothek, Zoom-Sperre auf dem iPhone, seltener Verbindungshänger bei sichtbarer Tafel (Wächter behebt ihn nach 25 s), Zwei-Geräte-Test, Arduino-Compile der neuen Teile.

Korrektur 1 (30.10.2026): Arduino-Prototyp-Fehler mit `RbDiagnosticStream` beseitigt. Hardware-Compile steht noch aus.
