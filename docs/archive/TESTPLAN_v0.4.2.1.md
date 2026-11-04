# Testplan v0.4.2.1-dev

Vorher Bibliotheks-Backup anlegen, Flash/NVS nicht formatieren. Bei Fehler seriellen Log (115200 Baud) sichern.

| Nr. | Test | Erwartetes Verhalten | Ergebnis |
|---:|---|---|---|
| 1 | Firmware kompilieren und starten | Kein Stack-/Reset-Fehler, WLAN erreichbar, LittleFS bereit | Offen |
| 2 | Alte Texte anzeigen und senden | Inhalte, Farben und Symbole unverändert | Offen |
| 3 | Drei Blockseiten gestalten, wechseln, senden | Änderungen bleiben im RAM, eine BLE-Übertragung, Tafel blättert | Früher in v0.4.2 bestanden; erneut prüfen |
| 4 | Blockfolge in Bibliothek speichern | Kein `Stack protection fault`, Name und Anzahl sichtbar | **Kritischer Nachtest** |
| 5 | ESP32 neu starten, gespeicherte Folge bearbeiten | Gespeicherte Inhalte korrekt, neuer Entwurf leer | Offen |
| 6 | Neues Programm aus gespeicherten Texten erzeugen | Scrollen und Speichern auf iPhone vollständig erreichbar; kein Reset | Offen |
| 7 | Kurzwahl belegen und löschen | Quelle/Ziel vor Label; Löschen leert Label und Quelle | Offen |
| 8 | „Alles löschen“ und „Seite löschen“ | Jeweils Sicherheitsabfrage; gespeicherte Bibliothek bleibt unangetastet | Offen |
| 9 | Zwischen Lauftext und Blockanzeige wechseln | Vorschau korrekt, kein Zurücksetzen, keine Seitenzahl im Lauftext | Offen |
| 10 | Einstellungen und Backup exportieren | Flaches Layout, JSON lädt herunter | Offen |
| 11 | Backup importieren (nur Testdaten) | Vorhandene Einträge nicht überschrieben | Offen |
| 12 | RAM-Diagnose bei mehreren Clients | Kein erneuter Stackschutzfehler; Speicherreserven beobachtbar | Offen |
