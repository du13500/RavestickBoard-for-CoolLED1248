# Installation und Update – v0.4.2.1-dev

**Zuerst** im Webinterface unter Einstellungen ein vollständiges JSON-Backup erstellen und extern sichern. Mit Arduino IDE den Sketch `firmware/RavestickBoard_v0_4_2_1/RavestickBoard_v0_4_2_1.ino` samt fünf Headern öffnen. Hardware TENSTAR ROBOT ESP32-C3 Super Mini, `ESP32C3 Dev Module`, Arduino-ESP32 3.3.12, `USB CDC On Boot: Enabled`; bewährte Partitionierung `No OTA (2MB APP/2MB SPIFFS)` unverändert lassen. **Niemals `Erase All Flash` aktivieren.**

Die alten Firmwareordner im Repository nicht überschreiben. Der neue Editor startet mit einer leeren RAM-Arbeitsfläche; frühere Testseiten aus NVS werden nicht geladen und nicht gelöscht. LittleFS-Bibliothek und Kurzwahlen sollen erhalten bleiben. Bei Startfehlern nichts formatieren; seriellen Log bei 115200 Baud sichern.

**Wichtig:** Diese Version adressiert einen auf echter Hardware gefundenen Stackschutzfehler beim Speichern einer Blockfolge. Das neue Speichern ist noch auf dem ESP32 nachzutesten. Alte Hardware-Erfolge der Blockband-Übertragung bleiben gültig.


Siehe außerdem [Testplan](TESTPLAN_v0.4.2.1.md), [Entwicklungsstand](ENTWICKLUNGSSTAND_v0.4.2.1.md) und [README](../README.md).
