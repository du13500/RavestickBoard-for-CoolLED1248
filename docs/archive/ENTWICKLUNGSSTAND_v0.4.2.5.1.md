# Entwicklungsstand v0.4.2.5.2-dev · 30.10.2026

Nach Benutzer-Hardwaretest von v0.4.2.5-dev wurden zwei UI-Fehler bestätigt:
7 defekte `<span>` im Menü infolge fehlerhafter SVG-Inline-Transformation;
Sichtbares Nachladen und Nachzeichnen der Startseiten-Icons sowie kurzzeitig
zusätzlicher Speed-Regler, weil `data-rb-view` erst durch JavaScript gesetzt wurde.

Behebung: Tags repariert; Kurzwahl- und Reload-SVG direkt in HTML; Home-Status
initial gesetzt; Ladeansicht bis async `init()` abgeschlossen ist (auch Fehlerzweig).
Änderungen sind auf WebUI/Versionskennzeichnung begrenzt; sonstiger Firmwarecode
identisch mit 0.4.2.5-dev. gzip/ETag neu aus der HTML-Quelle erzeugt.

Abnahme: realer iOS-Browser-/BLE-Test und kompletter Arduino-Compile offen.
Critical system min heap (<1 KiB) bleibt offener Befund. Nicht als stabil freigeben.
