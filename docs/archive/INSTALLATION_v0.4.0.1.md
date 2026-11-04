# RavestickBoard v0.4.0.1-dev · Installation und Rückfall

1. Direktupdate-ZIP in die Wurzel unseres lokalen GitHub-Repositories entpacken. Der neue Ordner `firmware/RavestickBoard_v0_4_0_1/` liegt neben den bisherigen Ständen. Bestehende Dateien in `web/` und die geänderte Projektdokumentation sind für diese Version vorgesehen.
2. Arduino IDE: `firmware/RavestickBoard_v0_4_0_1/RavestickBoard_v0_4_0_1.ino` öffnen. `RavestickBoard_Assets.h` und `RavestickBoard_WebUI.h` müssen daneben liegen.
3. Boardprofil wie erfolgreich bei v0.4.0: **ESP32C3 Dev Module**, USB CDC On Boot, **No OTA (2MB APP / 2MB SPIFFS)**; Port korrekt wählen. **„Erase All Flash“ NICHT einschalten**, um die neun vorhandenen NVS-Blockseiten zu bewahren.
4. Sketch kompilieren und aufspielen, dann über WLAN `CoolLED1248` und Browser `http://192.168.4.1` verbinden. Bei Browser-Cacheproblemen die Seite neu laden.
5. Zuerst Menüreihenfolge, kompakte Regler und gespeicherte neun NVS-Seiten überprüfen, anschließend Text-/BLE-Sendungen und neue Pixelgrafiken testen. Diagnose vor und nach der Testrunde als TXT speichern.
6. Bei Problemen Rückfall auf den hardwarebestätigten Ordner `firmware/RavestickBoard_v0_4_0/` möglich. Kein NVS-Schemawechsel in v0.4.0.1.

**Teststand vor Auslieferung:** Chromium-/JavaScript- und statische Datenprüfung; Arduino-Kompilierung und reale Boardfunktion von v0.4.0.1 sind noch nicht bestätigt.
