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

| ADR-018 | 2026-10-22 | **Projektname „RavestickBoard for CoolLED1248“**, zurzeit private GitHub-Entwicklung | Unterscheidung vom Original-App-Namen; markenrechtliche und Quellcode-Lizenzprüfung vor öffentlichem Release offen. |
| ADR-019 | 2026-10-22 | **Diagnose-Ringpuffer nur im RAM, TXT-Export auf Anfrage** | Weniger Flash-Verschleiß, keine zusätzliche Anmeldung, keine gespeicherten Textinhalte oder Kennwörter im Log. |
| ADR-020 | 2026-10-22 | **Firmwareversion im Header sichtbar und im Diagnosebericht** | Direkt erkennbare Softwareversion bei Screenshots und Support; Versionsstand passt zum Git-Commit. |

## Noch nicht entschieden

- Wie genau der mehrseitige Editor einzelne Seiten speichert und ob Programme ausschließlich im ESP32 verwaltet werden.
- Ob Speed-Werte der Originaltafel 1:1 abbildbar sind und wie Vorschaugeschwindigkeit kalibriert wird.
- Wie viele Seiten, Grafiken und Animationen der ESP32 speichern soll und in welchem Dateiformat.
- Verhalten bei konkurrierenden Original-App-BLE-Verbindungen: Verbindungsreihenfolge, Reconnect-Zeiten, Rückmeldung.
- Exakte Mini-Font-Pixelhöhe und Layoutregeln für zwei Zeilen.
- Ob iOS-App/Watch/Widget später noch separat sinnvoll sind, obwohl der Webcontroller funktioniert.

## 22.10.2026 · v0.1.2: deutsche Sprache, Pixelvorschau und Modi

- Die Benutzeroberfläche schreibt Deutsch mit **Ä, Ö, Ü, ä, ö, ü und ß**. Keine pauschale Umschrift in `ae/oe/ue/ss`.
- Der Engschrift-Font bekommt echte Kleinbuchstaben; der Browser erhält seine Glyphen aus der ESP32-Tabelle über `/api/font`. Keine zweite manuell gepflegte Fonttabelle in JavaScript.
- Statischer Text und Lauftext werden **manuell gewählt**, unabhängig von Länge. Vorerst kein automatischer Seitenwechsel und kein generischer Auto-Umbruch. Zu breite Standbilder werden ausdrücklich gekennzeichnet.
- Die LED-Vorschau benutzt einzelne **runde** Punkte in fester 64×16-Matrix und bietet schnellen Vorlauf 1×/2×/4×/8×.
- Der Geschwindigkeitsregler für die LED-Tafel und die Vorschau-Fast-Forward-Funktion sind unabhängig voneinander. Geschwindigkeit bleibt bis zur Kalibrierung experimentell.
- Kopieren in die Zwischenablage wird zum bevorzugten Diagnose-Export, TXT bleibt als Alternative.

## Entscheidung 22.10.2026: v0.2.0-Blockeditor
- Seiten sind eigenständige Design-Objekte, nicht automatisch abgeschnittene Textstücke.
- Keine erzwungenen Zeilenumbrüche und kein automatisches Nachrücken von Seiten.
- Jede Seite hat eigenes Layout, Texte, Zeilenfarben, Ausrichtung und Anzeigedauer.
- 12 Seiten zunächst als pragmatisches Limit. NVS wird nur bei Benutzeraktionen beschrieben.
- Seitenplayer sendet vollständig über BLE; das ist ausdrücklich ein Experiment bis zur Klärung der Board-internen Mehrseitenfähigkeit.
- Firmware bleibt USB-only ohne OTA. Webinterface benötigt kein gesondertes Login.
- Typografische Detailkorrekturen g/p/q/y/Ä/Ö/Ü bleiben separate Entscheidungen anhand der Referenzen.
