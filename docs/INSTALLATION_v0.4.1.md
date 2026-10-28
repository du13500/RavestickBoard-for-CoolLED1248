# Direktupdate auf RavestickBoard v0.4.1-dev

**Stand:** 27.10.2026 · experimenteller Build; Arduino-Kompilierung und CoolLEDX-Hardwaretest noch offen.

1. **Vorbereitung:** Die bisherigen zwölf Seiten bleiben in NVS. Falls du sie zusätzlich sichern möchtest, dokumentiere sie vor dem Update (ein dedizierter NVS-Seitenexport ist noch nicht implementiert). Alte Bibliotheksbestände existieren in v0.4.0.3 noch nicht.
2. Das Verzeichnis `firmware/RavestickBoard_v0_4_1/` vollständig in einen Arduino-Sketch-Ordner übernehmen. `RavestickBoard_v0_4_1.ino`, `RavestickBoard_WebUI.h`, `RavestickBoard_Assets.h` und **`RavestickBoard_Library.h`** müssen nebeneinander liegen.
3. Arduino-ESP32-Core **3.3.12**, Board **ESP32C3 Dev Module** wie bisher, **USB CDC On Boot: Enabled**. **Die bisherige Partitionstabelle unbedingt beibehalten** (4 MB Flash; App-Partition in Diagnose etwa 2 MB). LittleFS braucht eine Dateisystempartition; die Größe vor dem ersten Speichern prüfen.
4. **Erase All Flash deaktiviert lassen**. Nach Flashen und Neustart wie gewohnt mit `CoolLED1248` verbinden und `http://192.168.4.1` öffnen.
5. **Erstinstallation LittleFS:** Wenn noch kein Dateisystem existiert, zeigt die Bibliothek einen Button zur ausdrücklich bestätigten Ersteinrichtung. Dabei wird ausschließlich die Dateisystempartition formatiert, **nicht NVS**. Nur verwenden, wenn dort keine anderen wichtigen Dateien gespeichert sind. Bei einer Fehlermeldung über beschädigte Metadaten **nicht formatieren**; zunächst Daten sichern und Fehler untersuchen.
6. Im Texteditor einen Testlauftext erstellen → **In Bibliothek** → Name eingeben. Danach über das Navigationsmenü **Bibliothek** öffnen, Eintrag auswählen und über **Öffnen** oder **Senden** testen. Kurzwahl 1–6 auf der Bibliotheksseite belegen und über die Startseite senden.
7. Einen Text ändern, **Änderungen speichern**, ESP32 neu starten und vom zweiten Telefon aus aufrufen: Name, Text, beide Farben, Schrift, Favorit, Sortierung und Kurzwahl müssen erhalten bleiben.
8. **Backup JSON exportieren** und außerhalb des Controllers speichern. Diese Datei ist für die Wiederherstellung nach vollständigem Flash-Löschen wichtig. Das JSON-Importieren fügt Einträge hinzu und überschreibt existierende nicht.

**Nicht umsetzen:** Eine andere Partitionsaufteilung, ein komplettes Löschen des Flashs oder ein automatisches LittleFS-Format würden unnötig Bestandsdaten riskieren.

**Bekannte Grenzen:** Bis 64 Vorlagen; bis 1.599 UTF-8-Bytes pro Zeile, 47 UTF-8-Bytes Name und max. 30 KiB Bilddaten. Eingebauter Scrolltext-Renderer sowie aktuelle RGB-Sprite-IDs bleiben erhalten. Bibliothek ist nicht mit dem bestehenden NVS-Blockseitenprogramm identisch. Vollständige Programme als Textverkettung sind v0.4.2 vorbehalten.
