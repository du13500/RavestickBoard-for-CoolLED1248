# Entscheidungen und Designprinzipien

Format: ID · Datum · Entscheidung · Begründung · Status.

| ID | Datum | Entscheidung | Grund / Auswirkung |
| --- | --- | --- | --- |
| ADR-001 | 2026-10 | **Original-LED-Tafel bleibt unangetastet**; ESP32-C3 steuert über BLE | Kein Löten/Umbau an der Anzeige nötig. |
| ADR-002 | 2026-10-22 | **Eigenes ESP32-WLAN**, kein notwendiges Heimnetz/Internet | Festivalbetrieb mit dem Smartphone möglich. |
| ADR-003 | 2026-10-22 | **Firmware nur per USB-C**, keine OTA-Funktion | Einfache Wartung, geringere Angriffsfläche und mehr Flash-Spielraum. |
| ADR-004 | 2026-10-22 | Passwortgeschütztes WLAN, **kein zweites Webinterface-Login** | Möglichst unkomplizierte Bedienung; direkte Fremdsteuerung der Originaltafel über BLE wird dadurch ohnehin nicht verhindert. |
| ADR-005 | 2026-10-22 | **Textlänge bestimmt nicht den Wiedergabemodus** | Lauftext oder statische Seiten werden immer explizit ausgewählt. Die v0.1.0-Längenautomatik ist nur ein Prototyp. |
| ADR-006 | 2026-10-22 | **Statische Anzeigen als individuell gestaltbare Seiten**, nicht über Leerzeichen simuliert | Pro Seite eigene Inhalte, Farben, Schriftwahl, Dauer; manuelle oder automatische Navigation. |
| ADR-007 | 2026-10-22 | **Zeichenweise bzw. bereichsweise Farben und Schriftarten** | Texteditor soll nicht an ein einheitliches Format gebunden sein. |
| ADR-008 | 2026-10-22 | **Vorschau zeigt echte LED-Punkte in festem 64×16-Raster** | Kein Schrumpfen langer Texte; Animation nur im festen Anzeigefenster. |
| ADR-009 | 2026-10-22 | **Vorschau mit Fast Forward** | Lange Laufschriften schnell prüfen; 1×/2×/4×/8× als Vorschlag zur ersten Umsetzung, genaue Stufen noch offen. |
| ADR-010 | 2026-10-22 | **Normal und Bold als Originalschrift separat rekonstruieren** | Pixelgenaue Aufnahme der beiden Original-Modi, nicht versehentlich vermischen. |
| ADR-011 | 2026-10-22 | **Hersteller-Unregelmäßigkeiten einzeln diskutieren** | Originalreferenz bewahren; nicht pauschal übernehmen oder automatisch „verbessern“. |
| ADR-012 | 2026-10-22 | **Mini-Font selbst entwickeln**, Engschrift behalten | Mini ist kein Originalfont; Engschrift bereits erfolgreich ausprobiert. |
| ADR-013 | 2026-10-22 | **Zweizeilig ohne künstlich erzwungenen Leerabstand** | 16 verfügbare Pixel sinnvoll nutzen, Lesbarkeit praktisch prüfen. |
| ADR-014 | 2026-10-22 | **Versionierte Dokumentation + Diagnose von Anfang an** | Aus SmallTV-Erfahrungen lernen; sporadische Probleme nachvollziehbar machen. |
| ADR-015 | 2026-10 | **Webinterface gestalterisch von SmallTV unterscheiden** | An vertraute CoolLED1248-App angelehnt, aber moderner und besser bedienbar; nicht SmallTV-grau/gelb. |
| ADR-016 | 2026-10-22 | **Keine automatische Überschreibung beim Booten** | Bereits gespeicherte Anzeige unangetastet lassen, bis Nutzer aktiv sendet oder ein bewusst aktiviertes Programm läuft. |
| ADR-017 | 2026-10-22 | **Bei öffentlichem GitHub-Release vorher Lizenz-/Provenienzprüfung** | Community-Projekte sind Referenzen; ungeprüftes Übernehmen fremden Quellcodes vermeiden. |

## Noch nicht entschieden

- Wie genau der mehrseitige Editor einzelne Seiten speichert und ob Programme ausschließlich im ESP32 verwaltet werden.
- Ob Speed-Werte der Originaltafel 1:1 abbildbar sind und wie Vorschaugeschwindigkeit kalibriert wird.
- Wie viele Seiten, Grafiken und Animationen der ESP32 speichern soll und in welchem Dateiformat.
- Verhalten bei konkurrierenden Original-App-BLE-Verbindungen: Verbindungsreihenfolge, Reconnect-Zeiten, Rückmeldung.
- Exakte Mini-Font-Pixelhöhe und Layoutregeln für zwei Zeilen.
- Ob iOS-App/Watch/Widget später noch separat sinnvoll sind, obwohl der Webcontroller funktioniert.
