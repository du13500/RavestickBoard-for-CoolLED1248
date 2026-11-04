## v0.4.3-dev – Konsolidierung der Bedienoberfläche (30.10.2026)
- Neues iOS-App-Icon und Favicon: originale Lucide-Banane der Menütaste, gleiche hellgrauweiße Strichfarbe (#f3f5fa) und dunkler Tastengrund (#303744), offline aus PROGMEM.
- Lauftexteditor: überflüssige eigene Standbild-Option entfernt; einheitlicher Vollbreiten-Senden-/Speichernbereich, neue Speichern-Auswahl „Änderungen speichern“ / „Als neuen Eintrag speichern“ für Bibliothekstexte.
- Blockeditor: Standardschrift Original - Normal für neue Seiten; Dreier-Icongruppe für Ausrichtung; ikonbasierte Seitenwerkzeuge inkl. Radierer, trash-off bei Einzelseite, shredder/Alles löschen erst ab zwei Seiten; dreiteilige Vollbreiten-Senden-/Speichernaktionen.
- Bibliothek: vereinheitlichte Tooltips ohne alte Unicode-Zeichen; Umbenennen mit text-cursor-input; Name-/Duplizieren-Dialog wie bei Kurzwahl mit Lucide-Icons.
- Globale Tafelsteuerung: Standbild-Icon „octagon“. Ambiente: Raster füllt die verfügbare Höhe aus.
- Editor: Bibliothek-Rückweg nur bei Bearbeitung einer Bibliotheksvorlage eingeblendet. Bereits vorhandene BLE- und LittleFS-/NVS-Formate bleiben unverändert.
- ESP32-Compile, Hardware-/Heap-Tests stehen weiterhin aus. Alte Standbildvorlagen bleiben im Speicher; Wiedergabe über Programme kann sie weiterhin verwenden, der Lauftexteditor legt keine neuen an.

Geplant: USB/BLE-Firmware und Heap-Stabilität weiterhin untersuchen; native Animationen erst nach Stabilitätsprüfung.
