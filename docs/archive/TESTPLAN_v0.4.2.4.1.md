# Hardwaretestplan v0.4.2.4.1-dev

**Stand 29.10.2026.** Ergebnisse aus v0.4.2.4 sind **keine automatische Freigabe** für v0.4.2.4.1. Vor dem Update JSON-Backup erstellen, Partitionsschema unverändert lassen, `Erase All Flash` AUS, bei Uploadfehler 115200 Baud.

| Nr. | Test | Bisherige Beobachtung aus v0.4.2.4 | v0.4.2.4.1 neu prüfen |
|---|---|---|---|
| 1 | Sketch kompiliert / Boot ohne Panic; LittleFS + Kurzwahlen erhalten | **ok** | offen |
| 2 | Kurzes Laufband Text → 4 Blockseiten → Text; ein Upload, Farbe/Reihenfolge korrekt | **ok** | offen |
| 3 | 16 echte Leer-Pixelspalten zwischen Einträgen | **ok** | offen |
| 4 | Geteiltes Band über 30 KiB: vollständige Gruppen ohne Abschneiden | offen | offen |
| 5 | Große Sendungen wiederholt: RAMTRACE, größter Block, freier Heap | offen | offen |
| 6 | BLE-Paketfehler: maximal ein Vollretry; nachvollziehbarer Stop beim zweiten Fehler | offen | offen |
| 7 | Originalmodus gemischt, Standblöcke 4× bei Tempo 80 (~6,3 s) | nicht reproduzierbar | offen |
| 8 | Home: sechs kompakte Kurzwahlen, leere Kachel öffnet Setup | offen | offen |
| 9 | Tafelsteuerung Helligkeit/Tempo: **am Seitenende im normalen Fluss**, kein schwebendes Overlay, bei Bedarf Scrollen | schwebendes Dock als UX-Mangel erkannt | offen |
| 10 | Acht Modus-Icons: jeder Druck ein eigener BLE-Befehl | offen | offen |
| 11 | LED-Vorschau Lucide Anfang/Play/Pause, Schnecke/Vogel/Hase | offen | offen |
| 12 | Menü `bug`, keine Menüüberschrift, keine technische Editorfußnote | offen | offen |
| 13 | Bibliothek aktualisieren, Abhängigkeiten und Kurzwahlbereinigung | offen | offen |
| 14 | Zweimaliger Backup-Zusammenführungsimport, keine unnötigen Duplikate | offen | offen |
| 15 | Mehrere WLAN-Clients, 10+ Minuten Laufzeit, Neustart, Diagnose sichern | offen | offen |
| 16 | Kurzwahl-Dialog nur `trash`, `undo-dot`, `star-check` und per Tooltip/Screener verständlich | neu | offen |
| 17 | Bearbeitungsmodus zeigt `pencil-off` / Normalmodus `pencil`, **keine** Modusmeldungen | neu | offen |
| 18 | Programm wirklich löschen; nach Neustart **keine Wiederherstellung aus `.bak`** | Review F-06 | offen |

**Review:** F-01–F-12 (technische Befunde) sowie S-01–S-06 (Aufräum- und Testqualität) behalten wir als offenen Arbeitsvorrat. Im Hotfix behoben: F-06. Andere Punkte sind damit **nicht** automatisch als erledigt markiert. Die vorläufige Schätzung zur Lauftextdauer ist weiterhin **nicht hardwarekalibriert**.
