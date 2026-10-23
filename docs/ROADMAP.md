# Roadmap RavestickBoard

## Fertig / in Hardware getestet
- [x] v0.1.0: WLAN und BLE-Textübertragung (64×16, sieben Farben).
- [x] v0.1.1: Diagnose, Log-Export, Versionsanzeige.
- [x] v0.1.2: Firmware auf ESP32 installiert; Zeichensatz- und UX-Probleme im Praxistest dokumentiert.

## Aktuelle Version: v0.2.0 (Testkandidat)
- [x] Erste Seite-zu-Seite-Bearbeitung mit unabhängigen Inhalten und Einstellungen im Code implementiert.
- [x] Zwei Textzeilen (Mini) oder eine Zeile (Engschrift), sieben Farben je Zeile.
- [x] Seiten als Programm in NVS speichern, nicht beim Boot abspielen.
- [x] Individuelle Vorschau und 1×/2×/4×.
- [ ] Mit realer Arduino-ESP32-Toolchain kompilieren.
- [ ] Einzelne Seite auf physischer LED-Tafel bestätigen.
- [ ] Seiten nach Powercycle im Editor wiederfinden.
- [ ] Automatische zwei- oder dreiseitige Wiedergabe mit BLE-Log testen.
- [ ] 1-Stunden-Heap und Stabilität testen.

## Danach: Blockeditor ausbauen
- [ ] Mehrere Textobjekte je Seite frei positionieren.
- [ ] Farbwechsel pro Zeichen / Textabschnitt; Schriftmix innerhalb einer Seite.
- [ ] Originalschrift Normal / Bold aus Referenzscreenshots rekonstruieren.
- [ ] Engschrift g, p, q, y, Ä, Ö, Ü überarbeiten; letzte Pixelreihe für Unterlängen.
- [ ] Live-Steuerung Helligkeit und Geschwindigkeit; normaler Bereich ~80, fein abgestuft.
- [ ] Boardnative Programm-/Mehrseitenfähigkeit untersuchen, ohne undokumentierte gefährliche Befehle zu senden.
- [ ] Schnellzugriffe, Bibliothek, gespeicherte Programme, Rotation.
- [ ] Scoreboard, Countdown, Stoppuhr, Licht und Animationsimport.
- [ ] Speicher- und BLE-Langzeitdiagnose mit logischer Fehlerhistorie.

## Öffentliches Release
- [ ] Herkunftsaudit und unabhängige Neuimplementierung potenziell fremder Teile.
- [ ] Lizenz- und Markenrechte, Datenschutzhinweise, Tests, README.
- [ ] Erst danach Sichtbarkeit des privaten GitHub-Repositories prüfen.
