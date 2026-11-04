# RavestickBoard v0.4.0.2-dev – Direktupdate

Das ZIP in den Stammordner des lokalen GitHub-Repositories entpacken.

**Arduino IDE:** `firmware/RavestickBoard_v0_4_0_2/RavestickBoard_v0_4_0_2.ino` öffnen. Die beiden Header-Dateien müssen daneben liegen.

- ESP32C3 Dev Module / ESP32-C3 Super Mini; USB CDC On Boot aktiviert; bewährte Partitionierung **No OTA (2 MB APP / 2 MB SPIFFS)** beibehalten.
- **„Erase All Flash“ nicht aktivieren.** Unsere zwölf NVS-Blockseiten sollen erhalten bleiben.
- Verbinden: WLAN `CoolLED1248`, Browser `http://192.168.4.1`.
- Diagnose → **RAM-Verlauf TXT** (`/api/ram.txt`) und **TXT herunterladen** (`/api/diagnostics.txt`).
- Vor und nach dem großen Texttest jeweils exportieren; die Werte stehen nur bis zum nächsten Neustart bereit.
- Neue Symbolauswahl enthält **Ziggi I / Ziggi II**; die sechs verworfenen bunten Warnvarianten sind ausgeblendet, ihre internen IDs bleiben für vorhandene Inhalte erhalten.
- Die aktuelle **`docs/assets/Pixel-Art.xlsx` bleibt aus deinem bereits aktualisierten GitHub-Repository bestehen**; das Update enthält keine ältere Arbeitsmappe, die diese Datei überschreiben könnte.
- Bei Problemen Rückfall auf `firmware/RavestickBoard_v0_4_0_1/` möglich. Diese Version ändert das NVS-Schema nicht.

**Status:** Testbuild; Arduino-Kompilierung und Hardwaretest von v0.4.0.2 sind noch nicht erfolgt.
