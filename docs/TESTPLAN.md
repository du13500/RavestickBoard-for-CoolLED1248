# Prüfplan für neue Firmwarestände

## Testprotokoll je Release

**Datum:** ____  **Firmware:** ____  **Boardpaket:** ____  **Partitionsschema:** ____  
**Freier Heap vor/nach Test:** ____ / ____  **größter Block:** ____  
**Ergebnis:** bestanden / teilweise / fehlgeschlagen  **Fehler-IDs:** ____

### Grundtests
- [ ] WLAN-AP erscheint, Verbindungsaufbau per iPhone funktioniert ohne Internet.
- [ ] Webseite und API `/api/status` sind erreichbar.
- [ ] Ein kurzer Text kann auf die CoolLEDX übertragen werden.
- [ ] Textfarbe entspricht der Wahl.
- [ ] Nur Helligkeit verändern überschreibt den Anzeigetext nicht.
- [ ] Einmal hochgeladener Inhalt läuft nach BLE-Trennung weiter.
- [ ] Gespeicherter Tafelinhalt bleibt nach Stromtrennung bestehen.
- [ ] ESP32-Neustart überschreibt den Inhalt der Anzeige nicht.

### Künftige Vorschau- und Editor-Tests
- [ ] Die Vorschau besteht aus 64×16 getrennten, sichtbar runden Punkten mit unbeleuchteten LEDs.
- [ ] Die Vorschau besitzt einen festen Ausschnitt, egal wie lang der Text ist.
- [ ] Lange Textfolgen laufen durch das Fenster und werden nicht verkleinert.
- [ ] Fast Forward (z. B. 2×/4×/8×), Pause, Reset/Neustart der Vorschau funktionieren.
- [ ] Preview-Zeit und reale Laufrichtung/Geschwindigkeit sind plausibel abgeglichen.
- [ ] Statischer Modus kann auch für Text >64 Pixel gewählt werden; Überlauf wird sichtbar angezeigt, nicht automatisch umgebrochen.
- [ ] Lauftext kann auch für einen kurzen Text gewählt werden.
- [ ] Jede Seite kann separat formatiert und manuell oder per Dauer gewechselt werden.
- [ ] Unterschiedliche Farben und Schriften innerhalb eines Textes entsprechen der Tafelanzeige.
- [ ] Echte Kleinbuchstaben, Umlaute, Sonderzeichen und Herz getestet.
- [ ] Zweizeilige Mini-Schrift ist auf echter Tafel aus typischer Festivalentfernung lesbar.

### Künftige Diagnose- und Stabilitätstests
- [ ] Log-Export lädt eine lesbare TXT-Datei mit Firmwarestand, Resetgrund und Uptime.
- [ ] Heap-Minimum, freier Heap und größter freier Block sind nachvollziehbar.
- [ ] Fehlversuch ohne eingeschaltete Tafel wird protokolliert und UI bleibt bedienbar.
- [ ] Fremde BLE-Verbindung über Original-App: Erkennung/Retry/Wiederaufnahme getestet.
- [ ] Mehrere hintereinander gesendete Aufträge führen weder zu Absturz noch zu vermischten Texten.
- [ ] Langzeitbetrieb des AP und BLE in einem Testfenster von mehreren Stunden geprüft.
- [ ] Auf das notwendige Maß begrenzte RAM-/Flashnutzung; Dateisystemfüllstand prüfen.

### Mobile Oberfläche
- [ ] Farbpalette mit 7 Farben ohne unnötigen Umbruch.
- [ ] `100 %` bleibt in einer Zeile.
- [ ] Safari-Symbolleiste überdeckt weder Schaltflächen noch Meldungen.
- [ ] Große Schaltflächen und gute Lesbarkeit, Touchbedienung funktioniert.
