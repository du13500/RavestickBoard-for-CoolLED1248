# Installation · RavestickBoard v0.4.0-dev

Wir haben das Update als **Direktupdate für das bestehende GitHub-Repository** vorbereitet. Es ist ein **Testkandidat** und wurde noch nicht auf dem ESP32-C3 kompiliert oder auf der CoolLEDX-Tafel getestet.

## Dateien übernehmen

1. Im lokalen Repository eine Sicherung oder einen Git-Commit des funktionierenden Stands **v0.3.5.1** anlegen.
2. `RavestickBoard_v0.4.0-dev_Direktupdate.zip` in die **Wurzel des Repositories** entpacken und gleichnamige Dokumentationsdateien überschreiben.
3. Der neue Sketch befindet sich unter `firmware/RavestickBoard_v0_4_0/`. **Die alten Sketchordner bleiben unangetastet.**
4. `web/index.html` und `firmware/RavestickBoard_v0_4_0/RavestickBoard_WebUI.h` gehören exakt zusammen und sind im ZIP synchron.
5. **Wichtig:** Die im Repository bereits vorhandene Datei **`docs/assets/Pixel-Art.xlsx`** (unsere neueste Sammlung) bleibt unverändert. Dieses Paket enthält bewusst keine Kopie davon, um sie nicht durch eine ältere Fassung zu ersetzen.

## Flashen

In Arduino IDE `firmware/RavestickBoard_v0_4_0/RavestickBoard_v0_4_0.ino` öffnen. Board: ESP32C3 Dev Module; die Einstellungen wie bei v0.3.5.1 beibehalten (USB CDC On Boot, No OTA 2MB App / 2MB SPIFFS). **Kein Erase All Flash** – die vorhandenen NVS-Blockseiten sollen erhalten bleiben.

Nach dem Flashen mit WLAN `CoolLED1248` verbinden und `http://192.168.4.1` öffnen. Zuerst die sieben Ansichten und vorhandenen Blockseiten prüfen. Danach Geschwindigkeit 1–100, globales Verhalten, Moduswechsel, Pixelpicker und die neuen Ambiente-Farben anhand des [Testplans](TESTPLAN_v0.4.0.md) testen.

## GitHub-Veröffentlichung

README, Changelog, Quellenhinweise und Root-`LICENSE` wurden auf **MIT für unsere eigenen Codebestandteile** umgestellt. Der bisherige `LICENSE`-Text war *All Rights Reserved*: Diese Lizenzänderung erst nach Prüfung bewusst als **eigenen Git-Commit** auf GitHub veröffentlichen. Ursprüngliche Herstellerpixel, Marken und andere fremde Werke sind nicht automatisch mitlizenziert.

Wir haben **keinen GitHub-Commit und keinen Push** durchgeführt. Das ZIP dient zur lokalen Übernahme. Bei Problemen können wir `firmware/RavestickBoard_v0_3_5_1/` erneut flashen, solange der frühere Stand lokal vorhanden ist.
