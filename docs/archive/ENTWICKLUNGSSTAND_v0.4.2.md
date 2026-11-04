# RavestickBoard v0.4.2-dev: Entwicklungskandidat

## Implementiert

1. Aus v0.4.1 übernommene Bibliothek mit Legacy-`RBL1`-Textdateien, nun erweitertes Metadatenjournal (64er-Legacyformat lesbar, maximal 192 Indexplätze plus Flashreserve).
2. 25-Seiten-Blockeditor mit bewusst leerer Arbeitsfläche und eigener Löschen-/Leeren-Aktion. Die zwölf NVS-Testseiten wurden beim Strukturwechsel nicht migriert.
3. Bearbeitbare Blockfolgen als eigene LittleFS-Dateien und ein 64×16-Pixel-Seitenband pro Übertragung mit `BAND`-Job.
4. Programmverwaltung mit Referenzen auf gespeicherte Bibliothekseinträge, Zusammenfassen kompatibler Gruppen, gemischte Programmläufe über ESP32-Gruppenwechsel.
5. Kurzwahlen mit Quelle, Referenz und individueller Beschriftung; alte Text-IDs werden übernommen. Ambiente als unmittelbare Quelle; Animation noch deaktiviert.
6. Aufgeräumte mobile UI, Suche hinter Lupe, Backup in Einstellungen, Refresh für Standalone-Web-App, Apple-Touch-Icon aus Projektbild.
7. Neue Tests und Dokumentation, historische Firmwares unangetastet.

## Vorläufige Prüfung

- JavaScript-Syntax und Konsistenz der eingebetteten HTML-Datei überprüft.
- Chromium-Browser-Smoketest mit simulierten ESP32-Endpunkten für Bibliotheksspeicherung, Blockfolge, Programm und Kurzwahl.
- Separate C++-Hosttests der Pixelband-Verkettung (nicht mit echter Arduino-BLE-Firmware gleichzusetzen).

## Unbestätigt / bewusst nicht im Release

- **Arduino-ESP32-3.3.12-Kompilierung: nicht durchgeführt.** Die tatsächliche Firmwaregröße, Linkerbelegung, NimBLE/ESP32-API und RAM-Auslastung müssen an der vorgesehenen Arduino-Installation überprüft werden.
- **BLE-Gerätetest: nicht durchgeführt.** Darstellung eines verketteten Bandes, ACK-Verhalten und Pausenzeiten müssen mit der echten Tafel abgeglichen werden.
- Native Animationsübertragung `0x04`, GIF-Import, mehr als 16 Pixel breite Inline-Symbole, Passwortschutz und dynamische Boardwahl bleiben Entwicklungsthemen.
- Bei einem gemischten Programm kann ein nicht bestätigter Upload die Programmkette abbrechen. Vollständige automatische Neuübertragung nach ACK-Timeout ist noch offen.
- Im Webinterface und Firmware weiterhin identifizierbare alte NVS-Seitenplayer-Funktion, nur für Rückfalltests.

**Erprobungsregel:** Erst sichern, dann mit kurzen Texten und wenigen Seiten beginnen; echte Hardwaretesttabellen nutzen. `Erase All Flash` deaktiviert lassen.
