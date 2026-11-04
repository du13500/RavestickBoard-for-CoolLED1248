# Hardware-Testplan v0.4.1-dev

Vorher: `Erase All Flash` deaktiviert, **Partitionstabelle unverändert** und wenn möglich 12 NVS-Seiten dokumentiert.

- [ ] Sketch kompiliert mit ESP32-Arduino 3.3.12 und passender LittleFS-Bibliothek, USB CDC bleibt aktiv.
- [ ] Nach Flash: alle bisherigen NVS-Blockseiten vorhanden, Original-BLE-Textauftrag und RAMTRACE funktional.
- [ ] Frischer LittleFS-Bereich: ohne automatische Formatierung, einmaliger manueller Einrichtungsbutton funktioniert. Neustart danach ohne weitere Formatierung.
- [ ] iPhone: „HELLO 🍌“ als Text in Bibliothek speichern, Liste, Titel, Farben und Vorschau im Editor kontrollieren.
- [ ] Zweites Gerät: denselben Eintrag über WLAN finden, öffnen und via BLE senden. Beide Geräte sehen dieselbe Bibliothek.
- [ ] Vorlage aktualisieren und umbenennen; beim Belegen einer Kurzwahl muss die gespeicherte ID gleich bleiben. Kurzwahl-Taste sendet den Text.
- [ ] Löschen einer belegten Vorlage scheitert mit Referenzschutz. Nach Freigeben der Kurzwahl ist das Löschen möglich.
- [ ] Favoriten, Duplizieren, Suchen und Sortieren (mobil hoch/runter, Desktop Drag/Drop) prüfen.
- [ ] Export als JSON, neuen Text hinzufügen, Import des Backups: alte Texte bleiben erhalten; Kurzwahlen belegen nur bisher freie Slots.
- [ ] ESP neu starten, alle Einträge und Kurzwahlen weiterhin vorhanden.
- [ ] Großen Text (~30 KB Bildpayload) aus Bibliothek übertragen, RAMTRACE prüfen; keine 30-KB-Bildpufferregression.
- [ ] 1–3 WLAN-Clients, automatische NVS-Seitenfolge und Bibliotheksaufrufe abwechselnd testen.
- [ ] Fehlerpfad: Bei absichtlich gestörter LittleFS-Partition darf **keine** automatische Formatierung stattfinden (nur sicher simuliert, keine produktiven Daten beschädigen).

Statische Tests im Paket: `tests/library_host_test.cpp`, `tests/browser_library_test.py` und `tests/stream_renderer_host_test.cpp`. Test-Ergebnisse in `tests/README.md`; Hardwarechecks bleiben offen.
