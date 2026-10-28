# Installation v0.4.2-dev

**Ungetesteter Entwicklungskandidat, nicht für den Festival-Echtbetrieb freigegeben.**

1. In v0.4.1 unter Einstellungen/Backup (in der alten Fassung noch Bibliothek) eine **JSON-Sicherung** anlegen. Zusätzlich ältere Firmwareversionen im Repository aufbewahren.
2. Die ZIP-Dateien **in einen temporären Ordner** entpacken, nur den Inhalt mit seiner relativen Ordnerstruktur in das lokale Repository kopieren. **`.git` nicht ersetzen**. Keine GitHub-Änderung erfolgt automatisch.
3. In Arduino IDE `firmware/RavestickBoard_v0_4_2/RavestickBoard_v0_4_2.ino` öffnen; die fünf zugehörigen `.h`-Dateien müssen im selben Ordner sein.
4. `ESP32C3 Dev Module`, `USB CDC On Boot: Enabled`, Arduino-ESP32 `3.3.12`, `No OTA (2MB APP/2MB SPIFFS)`. Die Partitionstabelle und den USB-Port prüfen. **Erase All Flash: Disabled**.
5. Kompilieren und Flashen. Fehlermeldungen sichern; dieser Build wurde nicht am echten Board geprüft. LittleFS darf nicht automatisch formatiert werden.
6. Auf dem iPhone WLAN `CoolLED1248` verbinden und `http://192.168.4.1` öffnen. Versionsnummer `v0.4.2-dev` prüfen. Die gespeicherten Texte und v0.4.1-Kurzwahlen müssen erhalten sein.
7. Kurzen Text senden, einzelne Blockseite testen, dann zweifarbige dreiseitige Blockfolge als **ein Upload**. Kurzwahlen, Programme, JSON-Sicherung und Wiederherstellung nur mit Testdaten prüfen.
8. Bei Problemen v0.4.1 erneut per USB aufspielen. Keinesfalls LittleFS formatieren oder Partitionen wechseln. Nicht bestandene Punkte in der Testtabelle dokumentieren.

**Datenmodell:** Die bisherigen zwölf NVS-Testseiten können bei Änderung auf 25 Seiten zurückgesetzt werden. Bestätigt wurde, dass sie nur Testentwürfe waren. Die LittleFS-Bibliothek soll dagegen kompatibel bleiben. Auf der Tafel gespeicherter letzter Inhalt ist von beiden Speichern unabhängig.

**Homescreen-Icon:** Quelldatei `docs/assets/ravestick-icon.png`; die Firmware liefert ein verkleinertes PNG über `/apple-touch-icon.png`. Wenn iOS ein altes Symbol im Cache hält, die Homescreen-Verknüpfung entfernen und über Safari erneut hinzufügen.
