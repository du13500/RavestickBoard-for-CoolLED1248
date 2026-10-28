# Entwicklungsstand v0.4.2.1-dev

## Hardware-Testbefunde (v0.4.2-dev)

- **Bestätigt:** Mehrere Blockseiten wurden erfolgreich als ein breites Bildband mit einem Upload übertragen, einschließlich automatischer Seitenfolge und verschiedener Anzeigemodi.
- **Bestätigter Fehler:** Beim Speichern einer Blockfolge trat `Guru Meditation Error ... Stack protection fault` in `loopTask` auf; nach Neustart meldete LittleFS weiterhin drei vorhandene Texte. Der PC lag außerhalb des registrierten Task-Stackbereichs. Unnötige große temporäre Stackobjekte in der Schreib-/Leseroutine sind die technisch naheliegende Ursache.
- **Weitere Testbefunde:** Der alte Editor schrieb Seiten ungefragt in NVS; „Neue Blockfolge“ konnte ohne Rückfrage Inhalte löschen; Programm-Editor auf dem iPhone nicht scrollbar; verwirrende Kurzwahl-Beschriftung ohne Quelle; überflüssige Vorschau-Playersteuerung.

## Korrektur v0.4.2.1

- `rb42Read` und `rb42Write` lesen/verifizieren ohne lokal kopierten 25-Seiten-Datensatz; `rb42Write` vergleicht die Dateibytes in einem 128-Byte-Puffer. Das LittleFS-Journal behält `.bak`-Wiederherstellung.
- Bibliotheksmetadaten-Puffer im Programmstapel reduziert.
- Block-Editor ausschließlich volatile Arbeitsdaten im ESP32-RAM; alte NVS-Seiten bleiben unberührt. Explizites Speichern in der Bibliothek persistiert eine vollständige editierbare Folge.
- Programmseite scrolling, Kurzwahl löschen, separate Vorschau je Editor, Sicherheitsabfragen, überarbeitete Einstellungen.

## Nicht als bestanden behauptet

Erneute Arduino-ESP32-Kompilierung und vollständiger realer LittleFS-Speichertest stehen aus. Tests in Browser/Host geben keine Hardwaregarantie. Das aktuelle JSON-Backup sollte vor einem Flash als eigenständige Datei bereitliegen.
