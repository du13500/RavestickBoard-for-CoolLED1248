# Testplan RavestickBoard v0.1.2

**Status:** Vorbereitung vom 22.10.2026. Diese Version ist bis zum realen Test ausdrücklich **Testkandidat**.

## A. Flash und Webstart

- [ ] Board ESP32C3 Dev Module, USB CDC On Boot Enabled, `No OTA (2MB APP/2MB SPIFFS)` in Arduino IDE 2.3.10 / ESP32 3.3.12.
- [ ] Kompilierung erfolgreich; Sketchgröße, RAM-Angaben notieren.
- [ ] Per USB-C hochladen. Serieller Monitor 115200 Baud; WLAN `CoolLED1248` wird bereitgestellt.
- [ ] Browser lädt unter `http://192.168.4.1`; sichtbare Version **v0.1.2**; keine JavaScript-Fehler.

## B. Vorschau / Schrift

- [ ] Vorschau zeigt **64 Spalten × 16 Zeilen** mit erkennbaren dunklen Einzelpunkten und leuchtenden Einzelpunkten.
- [ ] `Grüße!` wird mit echten Kleinbuchstaben angezeigt.
- [ ] `ÄÖÜ äöü ß` wird in Vorschau und auf der echten Tafel korrekt dargestellt. Zeichensatzvarianten vergleichen.
- [ ] Langer Text bleibt auf Vorschaugröße und läuft im gewählten Lauftext-Modus; kein Schrumpfen des gesamten Inhalts.
- [ ] Play/Pause, Neustart, 1× → 2× → 4× → 8× → 1× funktionieren. Fast Forward verändert **nur** die Vorschau.
- [ ] Kurzer Text kann bewusst als Lauftext gesendet werden; langer Text kann bewusst als Standbild gewählt werden (sichtbare Überlaufwarnung). Nicht automatisch Modus wechseln.
- [ ] Linker Ausschnitt von überlangem Standbild in Vorschau und LED-Tafel vergleichen; ggf. Abweichung dokumentieren.

## C. Bluetooth / Einstellungen

- [ ] Kurzen Text per WLAN-Webinterface senden und auf Tafel prüfen.
- [ ] Rot, Grün, Blau, Weiß erneut prüfen; bestehende Farbauswahl darf nicht kaputtgehen.
- [ ] Lauftext-Modus mit Speed 10, 50, 90 testen; reale Laufrichtung und Tempo notieren. Vergleich mit Preview nur grobe Annäherung.
- [ ] Helligkeit 10 % und 100 % separat senden; bisheriger Text bleibt auf der Tafel.
- [ ] Während die Original-App mit der Tafel verbunden ist, eigenen Auftrag versuchen. Fehlermeldung verständlich? Wie viele Sekunden bis Abbruch? Original-App danach trennen und ohne ESP-Neustart erneut senden.
- [ ] BLE-Notifications und Auftragserfolge im Diagnoseprotokoll prüfen.

## D. Diagnose / iPhone

- [ ] `Diagnose und Ereignisprotokoll` öffnen und aktualisieren.
- [ ] `Log kopieren` direkt in die Zwischenablage. Den kopierten Bericht in eine Notiz einfügen.
- [ ] Falls automatisches Kopieren auf `http://192.168.4.1` blockiert: manuelles Kopierfeld nutzbar?
- [ ] TXT-Download weiterhin möglich.
- [ ] Resetgrund „USB-Reset“ statt Zahlencode 11, falls per USB neu gestartet.
- [ ] App-Partition ca. 2048 KiB; freie Bytes = Partition gesamt minus verwendete Firmware. **Nicht** irreführend „2048 KiB frei“.
- [ ] Freier/minimal freier Heap und größter Block vor/nach mehreren BLE-Aufträgen und nach 30 Minuten Laufzeit vergleichen.

## E. GitHub

- [ ] Nach erfolgreichem Test Änderungen im lokalen Projektordner committen, z. B. `v0.1.2: Dot preview, German glyphs, modes and diagnostics copy`.
- [ ] `CHANGELOG.md` an konkreten Hardwaretest anpassen: erfolgreicher und gescheiterter Test, ggf. Heap/Buildwert.
- [ ] `Push origin` in GitHub Desktop. Sicherstellen, dass das Repository **Private** bleibt.
- [ ] Wenn kritische Fehler auftreten: Firmware v0.1.1 erneut über USB flashen; v0.1.2 noch nicht als stabil markieren.
