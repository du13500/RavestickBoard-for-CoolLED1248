# Testplan v0.4.2.3-dev – Hardwareabnahme

**Vorbereitung:** Backup sichern, vor Update „Erase All Flash“ ausgeschaltet lassen. ESP32-C3 / Arduino-ESP32 3.3.12, USB-Uploadgeschwindigkeit bei Verbindungsfehlern auf **115200 Baud** reduzieren. Partition unverändert.

| Nr. | Test | Ergebnis / Beobachtung |
|---|---|---|
| 1 | Kompiliert, Bootlog ohne Fehler, altes LittleFS und sechs Kurzwahlen noch da | |
| 2 | Bestehendes Programm hat weiterhin Standard „Original/Standbilder“ | |
| 3 | Neues Programm „Alles als Laufband“: Lauftext → 4 Blockseiten → Lauftext; nur ein Upload, alle Motive scrollen | |
| 4 | Zwei unterschiedlich gefärbte Schriften, Symbole und Blockfarben bleiben korrekt | |
| 5 | Ein Laufband mit 16 Pixel Leerspalten zwischen Inhalten | |
| 6 | Langes Laufband überschreitet 30 KiB: Split an vollständiger Seiten-/Textgrenze, spätere Gruppe wird gesendet | |
| 7 | Mehrere Blockseiten (64×16) im Scrollband werden nicht abgeschnitten oder vertauscht | |
| 8 | Altes Originalprogramm: stehende Blockseiten und Lauftexte wechseln wie in v0.4.2.2 | |
| 9 | Gemischte Programme stoppen; reines Programm neu starten; Temposlider wird wieder freigegeben | |
| 10 | Leere Kurzwahl antippen, Einrichtungsdialog öffnet sich auf dem richtigen Slot | |
| 11 | Kurzwahl auf unvollständiges Programm: roter Zustand und Reparaturhinweis | |
| 12 | Editor: aus gespeichertem Text „＋ Neu“ → leere Vorlage; vorher ungespeicherter Entwurf fragt | |
| 13 | Acht BLE-Direktsymbole in 4×2: pro Klick neuer Modusbefehl, insbesondere zweimal gleiche Taste | |
| 14 | Lucide-Icons im iPhone offline sichtbar, darunter `square-off` und `laser` | |
| 15 | Schema-2-Backup zweimal zusammenführen: zweites Mal null zusätzliche Einträge/Programme | |
| 16 | Backup mit neuem Text unter gleichem Namen: zusätzliche Vorlage statt Überschreiben | |
| 17 | Neustart nach Programmspeicherung: neuer Wiedergabemodus bleibt erhalten | |
| 18 | 10-minütiger Betrieb mit mehrfachen Uploads: RAMTRACE, BLE- und WLAN-Status prüfen | |

**Achtung:** Für Split-Gruppen mit Lauftexten verwenden wir vorläufig die ersatzweise Sekundenzeit, keine exakte Synchronisierung. Kein automatisches Löschen von Bestandsdaten.
