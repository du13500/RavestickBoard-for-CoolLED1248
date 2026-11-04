# Testplan RavestickBoard v0.4.2-dev

**Noch keine Ergebnisse als „hardwarebestätigt“ markieren.** Liste als TSV direkt nach Excel kopierbar.

```tsv
ID	Prüffall	Erwartung	Ergebnis	Bemerkung
T42-01	Arduino-ESP32 3.3.12 kompilieren	Keine Fehlermeldungen; App-Partition passt	OFFEN	No OTA; USB CDC On Boot
T42-02	Flash ohne Erase All	Bestehende LittleFS-Texte bleiben erhalten	OFFEN	Vorab JSON-Backup
T42-03	Alte v0.4.1-Kurzwahl	Gleiche Bibliotheksvorlage per Kurzwahl erreichbar	OFFEN	Migrationsprüfung
T42-04	Leeren Blockeditor aufrufen	Nur eine leere Seite, NVS-Testseiten dürfen entfallen	OFFEN	Bewusste Ausnahme
T42-05	25 Blockseiten erstellen	25 editierbar; 26. Seite abgelehnt	OFFEN	Speicher prüfen
T42-06	Seite leeren	Nur aktuelle Seite wird nach Sicherheitsabfrage geleert	OFFEN	Andere unverändert
T42-07	Lauftext leeren	Text inkl. Symbole erst nach Bestätigung gelöscht	OFFEN	Globalregler unverändert
T42-08	Blockfolge speichern & Browser neu laden	Seiten weiterhin in LittleFS vorhanden	OFFEN	NVS-Arbeitskopie unterscheiden
T42-09	Blockfolge bearbeiten	Einzelne Schriften, Farben, Symbole und Reihenfolge vollständig rekonstruiert	OFFEN	Zum Vergleich Screenshot
T42-10	Drei verschiedenfarbige Blockseiten senden	Ein BLE-Upload, Tafel blättert exakt je 64px und endlos	OFFEN	Prozentanzeige nur zu Beginn
T42-11	Geschwindigkeitsregler	Bestimmt auch die Block-Blätterrate	OFFEN	Direkter Modus 0x07
T42-12	Kurzwahl zu Bibliothek	Ein Tipp lädt gespeicherten Inhalt	OFFEN	Nach Neustart prüfen
T42-13	Kurzwahl Ambiente	Ein Tipp zeigt Farbe ohne Bibliothek	OFFEN	Alle 7 Farben
T42-14	Kurzwahl benennen	Beschriftung persistiert, 47 UTF-8-Bytes sichtbar begrenzt	OFFEN	Emoji/Umlaut prüfen
T42-15	Programm mit mehreren Blockfolgen	Nach Möglichkeit ein einziger Upload	OFFEN	Laufzeit/Loop prüfen
T42-16	Gemischtes Programm Text+Block	Gruppenwechsel via ESP32; Upload nur je Gruppe	OFFEN	Prozentanzeige akzeptiert
T42-17	Überlanges Programm	An vollständiger 64px-Seitengrenze gesplittet	OFFEN	<=30KiB pro Upload
T42-18	Alte v0.4.1-Backupdatei importieren	Neue Text-IDs, vorhandene Einträge bleiben erhalten	OFFEN	Import ist additiv
T42-19	Backup Schema 2 & Restore	Blockseiten/Programme/Kurzwahlbezüge rekonstruierbar	OFFEN	Vorsicht beim NVS-Editor
T42-20	Homepage-Icon auf iPhone	180×180-PNG und Name RavestickBoard	OFFEN	Homescreen-Verknüpfung ggf. erneuern
T42-21	Menü: Webinterface neu laden	Nur Browser wird neu geladen, ESP32 bleibt aktiv	OFFEN	Cache no-store
T42-22	BLE-Timeout und Wiederanlauf	Fehler protokolliert; MTU-Reconnect funktioniert	OFFEN	Kein automatischer Komplettretry
T42-23	RAMTRACE nach 20 Übertragungen	Kein stetiger RAM-Verlust, keine Reboots	OFFEN	Ein bis drei WLAN-Clients
T42-24	Sicherung & Regression	v0.4.1 erneut flashbar, keine LittleFS-Beschädigung	OFFEN	Ohne Erase All
```

**Vor dem Rückmelden:** Diagnose (`/api/diagnostics.txt`) und RAMTRACE (`/api/ram.txt`) sichern, sowie bei Darstellungsfehlern ein LED-Foto anfertigen. Ein Test kann „teilweise“ sein; nicht automatisch auf „OK“ setzen.
