# Installation v0.4.2.3-dev

1. Deine aktuell laufende Version **v0.4.2.1-dev** und ein externes **JSON-Backup** als Rückfallmöglichkeit sichern.
2. Das Update-ZIP im Root deines lokalen Repositories entpacken. Alte Firmwareordner **nicht** überschreiben.
3. `firmware/RavestickBoard_v0_4_2_3/RavestickBoard_v0_4_2_3.ino` in Arduino IDE öffnen; alle dazugehörigen fünf Headerdateien müssen im selben Ordner liegen.
4. Einstellungen: **ESP32C3 Dev Module**, Arduino-ESP32 **3.3.12**, **USB CDC On Boot: Enabled**, **No OTA (2MB APP/2MB SPIFFS)**; `Erase All Flash` bleibt **deaktiviert**. Keinen Partitionswechsel durchführen.
5. Kompilieren und flashen, über WLAN `CoolLED1248` mit `http://192.168.4.1` verbinden. Den Homescreen-Link auf iPhone bei Bedarf entfernen und neu anlegen, um Cache und Icon zu aktualisieren.
6. Zuerst bestehende Library/Programme/Kurzwahlen prüfen, danach den [Hardware-Testplan](TESTPLAN_v0.4.2.3.md) abarbeiten. **LittleFS niemals neu initialisieren, wenn bereits Einträge vorhanden sind.**

Das Update ändert keine Datenformate und löscht keine bestehenden Texte oder Programme. Bei unvollständigen Programmen bewusst die roten Fehlstellen im Programmeditor reparieren.


**Hinweis aus dem Flash-Test am 28.10.2026:** Bei `Cannot configure port` nach Wechsel auf 921600 Baud half 115200 Baud. Die Datenpartition nicht formatieren.
