# RavestickBoard v0.3.5.1 – Pixelart-Korrektur

**Status:** Quellcode-Paket erstellt. Arduino-IDE-Build und Hardwaretest ausstehend.

## Lieferumfang

- Basis: v0.3.5; Ein/Aus, BLE, Lauftext, alle Modi und NVS-Struktur bleiben unverändert.
- `docs/Pixel-Art.xlsx`: unveränderte Benutzerdatei als editierbare Quelle für die sieben 16×16-Motive.
- `docs/PIXELART_16x16_v0.3.5.1.json`: Palette + sämtliche 36 Motiv-Pixelmatrizen in verbindlicher Reihenfolge.
- Huhn überschreibt das bisherige Motiv Nr. 0; Küken = Nr. 34 und Frosch = Nr. 35. Banane Nr. 24, Ente Nr. 28, UFO Nr. 31, UFO+Strahl Nr. 32.
- Original-App-Symbole Nr. 25 (Kussmund), 26 (Blume), 27 (Biene), 29 (Kuh) direkt aus den vier Screenshots erneut aufgenommen. Keine verlorene rechte Pixelspalte mehr.
- Pixelart-Farben: `.`=aus, B=blau, G=grün, C=cyan, R=rot, M=magenta, Y=gelb, W=weiß. Beide Excel-Grüntöne ergeben G; das als W beschriftete Theme-Feld wird als weißes LED-Pixel interpretiert.
- Die bereits hardwarefreigegebenen Kaktus-, Alien- und Stern-Matrizen sind byte-identisch zum Vorgänger.
- NVS-Datenformat ist unverändert. Bereits gespeicherte Texte verwenden weiterhin Unicode-Symbole; vorhandenes 🐓 bekommt das neue Excel-Huhn.

## Bekannter offener Punkt aus v0.3.5

- Der Geschwindigkeitsregler zeigt 60..100; die vom Nutzer beobachtete niedrige reale Geschwindigkeit ist nicht kalibriert. Es wird in 0.3.5.1 **keine** ungetestete Änderung der BLE-Geschwindigkeitskennlinie vorgenommen.

## Testprotokoll

| Nr. | Test | Ergebnis |
|---|---|---|
| 1 | Arduino-IDE-Kompilierung und Flash | |
| 2 | Version 0.3.5.1 und gespeicherte NVS-Seiten | |
| 3 | Excel: Küken und Frosch auswählbar und darstellbar | |
| 4 | Excel: Huhn/Banane/Ente/UFO/UFO+Strahl sehen aus wie die LED-Palettenvorschau | |
| 5 | Originalsymbole: Kussmund/Blume/Biene/Kuh rechts vollständig | |
| 6 | Seitliche Symbolabstände zwischen `U U [Symbol] U` kontrollieren | |
| 7 | Kaktus, Alien und Stern unverändert | |
| 8 | Mini-Zweizeilig und mögliche Symbolüberlagerung unverändert | |
| 9 | Power 0x09, Modus 0x06, Geschwindigkeit 0x07 weiterhin steuerbar | |
| 10 | Mehrfache Textuploads und Seitenprogramm ohne Fehler | |
