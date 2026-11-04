# Hardwaretest v0.4.2.3-dev – Befunde vom 28.10.2026

| Test / Beobachtung | Resultat |
|---|---|
| Firmware kompiliert und geflasht, altes LittleFS/6 Kurzwahlen erhalten | bestätigt |
| Alter Programmtyp „Original / Standbilder“ bleibt vorausgewählt | bestätigt |
| Lauftext → 4 Blockseiten → Lauftext, „Alles als Laufband“ | bestätigt: gemeinsames Scrollen, ein Upload bei kurzer Version |
| Verschiedene Schriften, Farben, Symbole und Blockfarben | bestätigt |
| 16 leere Pixelspalten zwischen den Inhalten | im gemeldeten Prüfschritt nicht erfolgreich abgenommen; Ursache der späteren Fehler nicht mit Abstand selbst gleichzusetzen |
| Sehr langes Laufband (>30 KiB) | drei Gruppen beobachtet (30.293 / 22.655 / 13.565 B); bisherige 20-s-Ersatzdauer schnitt Durchlauf ab |
| BLE-Versagen bei dritter Gruppe | Paket 70/106 nicht bestätigt; kein automatischer Komplettretry, Original-Herstellerfehlerbildschirm blieb |
| RAMTRACE vor Ausfall | zuletzt ca. 31 KB Heap frei, historisches Minimum 17 KB; **kein nachgewiesener ESP32-Panic** |
| Firmwareupdate über 921600 Baud fehlgeschlagen | Upload mit 115200 Baud erfolgreich |

Interpretation: **Zwei getrennte Defekte:** (1) unpassende gruppenweise Zeitsteuerung, (2) fehlende robuste Wiederherstellung nach bestätigungslosem BLE-Paket. Ein direkter Zusammenhang mit RAM-Mangel ist nicht nachgewiesen. Die Korrekturen in v0.4.2.4 sind bis zum erneuten Hardwaretest nur Implementierungen.
