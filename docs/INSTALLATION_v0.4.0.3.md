# Installation v0.4.0.3-dev

1. ZIP entpacken; `firmware/RavestickBoard_v0_4_0_3/RavestickBoard_v0_4_0_3.ino` mit Arduino IDE öffnen. Die beiden `.h` müssen im **gleichen Verzeichnis** verbleiben.
2. Board-Einstellungen unverändert: ESP32C3 Dev Module, Arduino-ESP32 3.3.12, USB CDC On Boot Enabled, Flashgröße 4 MB und bisherige Partitionierung.
3. **Erase All Flash: Disabled.** Bestehende 12 NVS-Seiten bleiben erhalten.
4. Kompilieren, dann per USB hochladen.
5. Am Hotspot `CoolLED1248` anmelden, `http://192.168.4.1` öffnen, im Diagnoseexport `v0.4.0.3-dev` prüfen.
6. Erst kleinen Text, dann großen Lauftext testen; RAM- und Diagnose-TXT speichern.
7. Wenn Probleme auftreten, v0.4.0.2 wie bisher zurückflashen, **ohne Flash zu löschen**.

**Hinweis:** Firmware wurde durch Host-Differenztests und statische Prüfungen überprüft, aber hier nicht mit Arduino-ESP32 kompiliert und nicht real am Board getestet. Experimenteller Build, vor Festivals nicht ungeprüft einsetzen.
