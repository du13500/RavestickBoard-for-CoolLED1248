# Pixelarchiv als Datenquelle

Die Firmware v0.3.0-dev enthält bereits die aus den zwei JSON-Archiven erzeugten Rasterdaten in `RavestickBoard_Assets.h` und `web/index.html`. Die Original-JSONs sind **nicht** nochmals Bestandteil dieses Pakets; sie liegen im separaten CoolLED1248-Archiv der Projektarbeit.

Zum erneuten Erzeugen die vollständigen Dateien `CoolLED1248_Originalglyphen_v02.json` (164 Zeichen) und `CoolLED1248_Originalsymbole_v01.json` (24 Motive) nach `assets/source/` kopieren und im Repository-Stammverzeichnis ausführen:

```bash
python tools/generate_assets.py --glyphs assets/source/CoolLED1248_Originalglyphen_v02.json --symbols assets/source/CoolLED1248_Originalsymbole_v01.json
python tools/sync_web.py
```

Die Hersteller-App-Symbole werden separat gekennzeichnet und nicht als eigenes Design beansprucht. Die Ravestick-Banane ist eine eigene Pixelgrafik.
