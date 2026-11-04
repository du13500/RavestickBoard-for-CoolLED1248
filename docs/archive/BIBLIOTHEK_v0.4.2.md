# Bibliothek, Programme und Kurzwahlen: v0.4.2.1-dev

## Abhängigkeiten seit v0.4.2.2

Eine Bibliotheksvorlage darf auch gelöscht werden, wenn sie in Programmen oder Kurzwahlen vorkommt. Die Löschbestätigung nennt bekannte Verwendungen. Direkt zugewiesene Kurzwahlen werden beim Löschvorgang samt Label freigegeben. Programme behalten fehlende IDs und werden weiter in der Programmliste angezeigt, farbig als reparaturbedürftig markiert und nicht gestartet. Reparatur erfolgt durch Ersetzen oder Entfernen des betroffenen Schritts, anschließend Programm unter derselben ID speichern. **Keine automatische Programmbereinigung und kein unbemerkter Skip**. Das Speicherschema bleibt unverändert.


## Kompatibilitätskonzept

Vorhandene Textdateien mit Magic `RBL1` und Schema 1 bleiben **bytegenau unverändert**. Textdaten, Layout, Farben, Modus, Name und Favorit werden weiterhin in diesen Dateien gespeichert. Das `RBM1`-Metadatenjournal liest sowohl die alte 64-ID-Struktur von v0.4.1 als auch die erweiterte Struktur mit 192 möglichen IDs. Die Erweiterung wird erst beim nächsten regulären Metadatenschreibvorgang dauerhaft gespeichert. Freier LittleFS-Platz wird geprüft; mindestens 64 KiB bleiben als Reserve. Ein dynamisch unbegrenzter Index ist nicht implementiert.

**Neue Blockfolge:** Ein Bibliothekseintrag ist weiterhin ein `RBL1`-Record, aber `layout=5` markiert einen Blockeintrag; seine vollständigen editierbaren Seiten stehen in einer separaten, versionierten und mit FNV-1a geprüften Datei `/rb-lib/b<ID>.bin` (Magic `RBB2`). Das Dokument enthält 1–25 vollständige `PageRecord`-Strukturen mit zwei Textfeldern, Layout, Ausrichtung, zwei Farben und historischer Dauer. Das farbige Pixelband selbst wird nicht persistent gespeichert, sondern beim Senden aus diesen Originaldaten gerendert. Ab v0.4.2.1 erfolgen Änderungen ausschließlich in der RAM-Arbeitsfläche und erst nach ausdrücklichem Speichern in LittleFS. Das alte NVS-Schema bleibt unangetastet.

## Technische Endpunkte

- `GET /api/library`, `/api/library/get?id=…`, `POST /api/library/save`, `/api/library/delete`, `/api/library/move`, `/api/library/send`: bestehende Schnittstellen, nun mit der zusätzlichen Anzeige `kind=block/text` und optionaler Seitenzahl. `/api/library/save` nimmt keine direkten Blockupdates an.
- `POST /api/block/reset`: 25-Seiten-Arbeitsfläche auf eine leere Seite zurücksetzen.
- `POST /api/block/save` mit `name` und optional `id`: Arbeitsfolge speichern bzw. aktualisieren.
- `GET /api/block/get?id=…`, `POST /api/block/load` mit `id`: Originalseiten lesen und in Editor laden.
- `POST /api/block/show`: aktuelle Arbeitsfolge als ein Band senden; `POST /api/block/send` mit `id`: gespeicherte Folge senden.
- `POST /api/block/meta` mit `id`, `name` oder `favorite`, `/api/block/clone` mit `id` und `name`: Metadaten verändern bzw. komplette gespeicherte Folge kopieren.
- `GET /api/collections`, `GET /api/collections/get?id=…`, `POST /api/collections/save` (Name, Sekunden, kommagetrennte Bibliotheks-IDs), `POST /api/collections/run`, `POST /api/collections/stop`, `POST /api/collections/delete`: bis zu 24 Programme mit je bis zu 24 Schritten (Magic `RBP2`).
- `GET /api/shortcuts`, `POST /api/shortcuts/set`, `POST /api/shortcuts/run`: sechs typisierte Kurzwahlen in `/rb-lib/shortcuts2.bin` (Magic `RBS2`). Typen 0 leer, 1 Bibliothek, 2 Programm, 3 Ambiente. Typ 4 Animation ist nur vorbereitet und nicht auswählbar.

Die neuen Dateien nutzen temporäre Dateien, Prüfsummen und Backup-Renames. Seit v0.4.2.1 geschieht die Dateiverifikation ohne zweite große Datensatzkopie auf dem Stack, sondern mit 128-Byte-Puffer. Fehler während eines mehrstufigen Speichervorgangs sind **nicht vollständig transaktional**. Keine automatische LittleFS-Formatierung bei Fehlern. Die Kurzwahl-Seitenbelegung aus v0.4.1 wird bei der ersten Initialisierung übernommen.

## Übertragungsmodell

- Farbplanes: je 2 Byte pro Spalte in Rot-, Grün- und Blau-Ebene. Ein fertiges 64×16-Bild = 384 Datenbytes. Verbundene Seiten werden farbebenweise kopiert, nicht Zeichenfolgen verkettet.
- Maximalbudget `30*1024` Byte inklusive 107-Byte-Header. Maximal **79 komplette 64-Pixel-Seiten** in einem BLE-Upload (rechnerische Obergrenze, keine bestätigte Gerätespeichergrenze).
- Ein Blockeditor-Eintrag enthält bis zu 25 Seiten. Benachbarte Blockeinträge desselben Programms werden bis zum Budget verknüpft; an der nächsten vollständigen Seitengrenze wird gesplittet.
- Lauftexte desselben Layouts, der Farben und des Scrollmodus können mit Leerzeichen zu einem Banner kombiniert werden; andere Gruppen erfordern neue BLE-Uploads.
- Nur eine einzelne Gruppe wird **einmal** an die Tafel gesendet und läuft dort autonom weiter. Bei Programmen aus mehreren Gruppen übernimmt der ESP32 die Wechsel (5–180 s je Gruppe), und die Prozentanzeige kann erneut erscheinen.

## Export und Import

JSON-Sicherung `format=RavestickBoard-library`, `schema=2` enthält `items` (Blockdaten mit `pages`), `collections` und `shortcuts` inklusive `type`, `id` und `label`. Alte `schema=1`-Exporte werden weiterhin importiert. Import vergibt neue IDs und stellt Bezüge auf diese neuen IDs her; belegte Kurzwahlen bleiben unangetastet. Ein Import ist **nicht atomar**, daher bei Fehlern immer den aktuellen Bibliotheksbestand prüfen. Die NVS-Editor-Arbeitsfläche wird vorübergehend zum Aufbau importierter Blockseiten verwendet und anschließend möglichst wiederhergestellt.

**Offene Risiken:** Arduino-Core 3.3.12 sowie reale BLE-Schnittstellen noch nicht getestet; Szenarien NVS-Arbeitskopie, Bibliotheks-Konsistenz bei Stromverlust, freier Flashraum und lange Programme müssen auf Hardware geprüft werden.
