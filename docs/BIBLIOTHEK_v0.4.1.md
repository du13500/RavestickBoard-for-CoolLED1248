# Bibliothek v0.4.1-dev: technisches Datenmodell

## Speicherung

- Die Bibliothek liegt im LittleFS-Dateisystembereich (Pfad `/rb-lib/`). Bestehende NVS-Schlüssel für die 12 Blockseiten werden **nicht gelesen, geändert oder migriert**.
- Ein Eintrag ist eine geprüfte, versionierte Binärstruktur mit Magic `RBL1`, Schema 1, stabiler uint32-ID, Titel, bis zu zwei UTF-8-Zeilen, Fontlayout, zwei Farben, Textmodus und Favoritflag. 64 Einträge sind das feste Limit. Scratch-Record im RAM ist statisch auf eine Struktur begrenzt.
- Reihenfolge, nächste ID und sechs Kurzwahlen stehen in einer Metadatenstruktur `RBM1`. Zwei Journaldateien `meta-a.bin` und `meta-b.bin` werden abwechselnd geschrieben. Beim Start wird die gültige Datei mit der neuesten Sequenznummer gewählt. Prüfsummen sind **Integritätsprüfungen**, keine Signaturen.
- Die aktuelle Textdatei wird vor Ersetzen als `.bak` aufbewahrt. Bei einem Stromverlust zwischen Umbenennung und Austausch wird bei Start aus der Rückfallkopie wiederhergestellt. Wenn sowohl neue `.bin` als auch Backup existieren, bleibt die neue Datei maßgeblich. Manuelle Fehlerbehebung kann bei einem defekten aktuellen Datensatz erforderlich sein.
- Flash-Speicherung nur bei bewussten Aktionen; keine periodischen Autosaves und keine vollständige Bibliothek im Heap.

## HTTP-Schnittstelle (lokaler AP)

| Methode | Pfad | Wirkung |
|---|---|---|
| GET | `/api/library` | Journalstatus, Flashbelegung, Text-Metadaten, Reihenfolge, Kurzwahlen |
| GET | `/api/library/get?id=N` | Einzelnen Text als JSON lesen |
| POST | `/api/library/init` | Ersteinrichtung, nur mit `confirm=INIT_LIBRARY_FS` und nur bei noch nicht montiertem FS |
| POST | `/api/library/save` | Neuen Text anlegen; mit `id=N` vorhandenen Text überschreiben |
| POST | `/api/library/delete` | Text entfernen; bei Kurzwahlreferenzen HTTP 409 |
| POST | `/api/library/move` | Reihenfolge ändern (`id`, `to`: nullbasierter Index) |
| POST | `/api/library/shortcut` | Kurzwahl referenziert Text-ID (`slot` 1–6, `id` oder 0) |
| POST | `/api/library/send` | Gespeicherten Text über die bestehende BLE-Auftragswarteschlange senden |

`/api/library/save` erwartet `name`, `text`, `line2`, `layout` 0–4, `color1`, `color2` 1–7, `mode` 1=statisch / 2=Lauftext und `favorite` 0/1. Die bestehenden HTTP- und BLE-Endpunkte wurden nicht umbenannt.

## Export und Import

Das Backup wird **im Browser** durch sequenzielles Lesen der Einzeltexte gebildet, nicht als großer JSON-String im RAM des ESP32. Das JSON enthält `format=RavestickBoard-library`, `schema=1`, `items` und `shortcuts`. Beim Import werden neue IDs vergeben; vorhandene Einträge bleiben unverändert. Kurzwahlreferenzen werden auf diese neuen IDs übersetzt und nur dann übertragen, wenn der jeweilige Slot noch frei ist. Import ist **nicht transaktional**: Im Fehlerfall bleiben bereits importierte Einträge stehen.

## Sicherheit, Grenzen, offene Punkte

- LittleFS wird mit `begin(false)` eingebunden. Ein unformatiertes Dateisystem erfordert eine gesonderte Benutzerbestätigung. Metadatenfehler lösen **keine** automatische Formatierung aus.
- Eine beschädigte Eintragsdatei wird als solche angezeigt und lässt sich nicht blind bearbeiten. Ein beschädigtes Journal muss untersucht werden.
- Der Webserver ist im lokalen Access-Point-Netz ohne separaten Benutzer-Login erreichbar: Wer die WLAN-Zugangsdaten hat, kann auch Bibliothekseinträge ändern. Zugriffsschutz steht noch auf der Roadmap.
- Unter 1.600 Bytes pro Zeile bleiben lange Textzeichenfolgen möglich; für größere Datenvolumen wäre eine künftig alternative Speicherung denkbar. Die 30-KiB-Grenze der übertragenen Bilddaten bleibt separat bestehen.
- Gespeichert wird vorläufig der Modus Standbild/Lauftext. Die Blockseiten nutzen weiterhin ihre alte Persistenz. Verkettete Programme mit Referenzen folgen erst in v0.4.2.
- Statische Hosttests ersetzen weder eine echte Arduino-ESP32-Kompilierung noch einen Hardware- und Stromausfalltest.
