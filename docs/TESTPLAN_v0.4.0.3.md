# 🍌 v0.4.0.3-dev — Hardwaretest (noch offen)

**Vorbereitung:** Aktuelle 12 NVS-Blockseiten prüfen, Diagnoselog von v0.4.0.2 sichern. ESP32-C3 und CoolLEDX ausreichend versorgen. Arduino ESP32 Board-Package wie im bisherigen Projekt einstellen. *Erase All Flash = Disabled*.

- [ ] Arduino-Kompilierung erfolgreich; keine fehlenden Bibliotheken, keine Überschreitung der App-Partition
- [ ] USB-Flash mit erhaltenen zwölf Blockseiten; Firmware/Diagnose zeigen `v0.4.0.3-dev`
- [ ] Text normal/kompakt/Bold/Mini einzeilig/Mini zweizeilig, mehrere Farbwerte und Symbole wie 🍌🦆❤️
- [ ] Alte 12-Seiten-Folge, Speichern, Duplizieren und manuelle Seitenübertragung funktionieren
- [ ] Kurzer Lauftext (unter 8.192 Byte) sieht unverändert aus
- [ ] Langer Lauftext (über 8.192 Byte) ist auf der CoolLEDX sichtbar und enthält keine verschobenen Pixel
- [ ] Gleicher nahezu 30,6-KiB-Lauftext wie in v0.4.0.2: RAMTRACE vor/nach/during, Upload-Dauer vergleichen
- [ ] Drei WLAN-Clients verbunden, langer Lauftext wiederholen; größter freier Block vergleichen
- [ ] Bei wiederholten BLE-Verbindungsaufbauten kein MTU=0; bei MTU=0 einmaliges Wiederverbinden nachvollziehbar
- [ ] Bei Fehler wird die Übertragung sauber abgebrochen; nach einem Fehler wieder eine normale Übertragung möglich
- [ ] Helligkeit, Tempo, Ambiente und Moduswechsel weiterhin nutzbar
- [ ] RAM-Verlauf TXT und allgemeiner Diagnoseexport funktionieren, Job-Minima erscheinen
- [ ] 30 Minuten Betrieb mit automatischen Seitenwechseln und mindestens drei verbundenen Clients ohne Absturz

**Abbruch:** Falls der lange Text verzerrt erscheint, die Paketübertragung extrem langsamer wird, sich der C3 aufhängt oder die 12 Seiten fehlen: Test beenden und v0.4.0.2 zurückflashen, Flash **nicht** löschen. RAM-/Seriellog vor Reset sichern.
