# RavestickBoard v0.3.0-dev · Entwicklungspaket (24.10.2026)

**Status:** Quelltext- und Browser-Testkandidat. **Nicht** mit der Arduino IDE / `esp32` 3.3.12 gebaut und **nicht** auf ESP32-C3 oder CoolLEDX getestet. Letzter funktionierender Hardwarestand ist v0.2.0 Korrektur 1. Die vorhandene v0.2.0 bleibt als Rückfallversion enthalten.

## Enthalten

- 82 rekonstruierte Glyphen je **Normal** und **Bold** als separate Fonttabellen.
- **Mini eine Zeile**, 7 Pixel, horizontal und vertikal zentriert. **Mini zwei Zeilen** bleibt erhalten und zeigt Kleinbuchstaben als Versalien.
- 24 rekonstruierte CoolLED1248-Originalsymbole als farbige 16×16-Raster, wie Emojis mitten in Lauf- und Blocktexte einfügbar.
- Original-Banane und separate selbst entworfene **vertikal gespiegelte** RavestickBoard-Banane.
- Herz in **allen sieben LED-Farben** via Emoji-Varianten, unabhängig von der Textfarbe.
- Individuelle Symbolbreite gemäß beleuchteten Spalten, derzeit **1 Pixel vorläufiger Abstand**. Die tatsächliche App-Abstandsmessung steht noch aus.
- Direktbefehle für Helligkeit, Geschwindigkeit, Modus (`0x08`, `0x07`, `0x06`) mit kurzzeitig wiederverwendeter BLE-Verbindung.
- Browserseitiges Wiederherstellen von Lauftextentwurf und letztem Reiter (`localStorage`); keine neue LittleFS-Bibliothek.
- Überarbeitete Dokumente (README, CHANGELOG, ROADMAP, THEMENSPEICHER, ENTSCHEIDUNGEN, TECHNIK, TESTPLAN, DIAGNOSE, QUELLEN_UND_LIZENZEN, Veröffentlichungsaudit). LICENSE inhaltlich unverändert.

## Prüfstand

- **JavaScript:** Syntax geprüft und browserseitig ohne Ausnahme ausgeführt; Originalfonts 82/82, Symbole 25, sieben Herzfarben geprüft.
- **C++:** Textrenderer mit Host-C++-Compiler und Platzhalter-Stubs übersetzt und getestet: Normal/Bold, Minis, Herzfarben, Bananen, 64-Spalten-Begrenzung. **Dies ist keine vollständige Arduino-Kompilierung.**
- **ESP32 / BLE:** Ausstehend. Vor echtem Einsatz insbesondere die Originalfont-Positionen, Icon-Abstände, NVS-Migration und den kurzzeitig gehaltenen BLE-Kanal kontrollieren.
- **Bekannte Grenze:** Ein 16-Pixel-Symbol im zweizeiligen 7-Pixel-Mini-Layout kann beide Textzeilen überlagern. Dafür existiert noch kein automatischer Kollisionsschutz.
- **Nicht enthalten:** Animation `0x04`, nahtlose native Mehrseiten-Programme, LittleFS-Programmbibliothek, Symbolkombination mit individuell formatierbaren Textfragmenten.

## Arduino-Installation (nur zum bewusst experimentellen Test)

Alle drei Dateien im selben Ordner belassen:

```text
firmware/RavestickBoard_v0_3_0/
  RavestickBoard_v0_3_0.ino
  RavestickBoard_WebUI.h
  RavestickBoard_Assets.h
```

Arduino IDE: **ESP32C3 Dev Module**, **USB CDC On Boot: Enabled**, **No OTA (2MB APP/2MB SPIFFS)**, Espressif `esp32` v3.3.12. Bei Kompilierfehlern und BLE-Problemen weiterhin v0.2.0 Korrektur 1 verwenden. NVS-Seitenschema bleibt v1; unbekannte Layoutwerte dürfen die bestehenden Seiten nicht beschädigen.

## Dateien und Merge ins Repository

Das Archiv ist als **Update** zu behandeln. Die vom Nutzer selbst eingefügte Datei `docs/assets/totem.jpg` liegt im aktuellen GitHub-Repository und wird vom Paket **nicht** ersetzt oder entfernt. Alte Firmwares bleiben erhalten. Das Repository wird durch dieses Paket **nicht automatisch geändert**.

Die Original-JSON-Dateien bleiben im separaten CoolLED1248-Archiv; die benötigten Pixelraster sind bereits verlustfrei in die jeweiligen Header- und Browserdaten gepackt. Das Werkzeug `tools/generate_assets.py` kann nach separater Bereitstellung beider JSONs die Headerdaten erneut generieren.

## Quellenkennzeichnung

Die Rekonstruktionen der 24 Herstellersymbole bleiben ausdrücklich als **CoolLED1248-Originalsymbole** gekennzeichnet. Eigenmotive erhalten die Kennzeichnung **RavestickBoard-Eigensymbole**. Einzelheiten siehe `docs/QUELLEN_UND_LIZENZEN.md`. Eine Quellenangabe allein ersetzt eine gegebenenfalls erforderliche Nutzungserlaubnis nicht. Die eigene `LICENSE` beansprucht keine zusätzlichen Rechte an fremden Originalgestaltungen.

OpenSign-Autor Adam Schulz wurde am 23.10.2026 **per E-Mail** angeschrieben. Eine Antwort bzw. Lizenzfreigabe ist noch nicht bestätigt. Kein OpenSign-Quellcode übernommen.
