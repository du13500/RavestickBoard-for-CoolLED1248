# RavestickBoard for CoolLED1248

**Firmware:** v0.1.2, Testkandidat vom 22.10.2026. **Noch nicht auf der realen Hardware getestet.**  
**Letzte bestätigte Hardwareversion:** v0.1.1, inklusive 5 erfolgreichen BLE-Textaufträgen, Diagnoselog und Nutzung parallel zur Original-App (nach manueller Trennung ihrer BLE-Verbindung).

Eigenständige Offline-Websteuerung für die CoolLED1248 / CoolLEDX LED-Tafel mit einem TENSTAR ROBOT ESP32-C3 Super Mini. Keine Änderung der Firmware der LED-Tafel. Kein OTA; Firmwareaktualisierung ausschließlich per USB-C.

## Neue Funktionen in v0.1.2

- **Deutsche Schreibweise:** Die Benutzeroberfläche verwendet echte Umlaute und `ß`. Der eigenständige Engschrift-Font enthält jetzt erstmals Groß- **und** Kleinbuchstaben einschließlich `Ä Ö Ü ä ö ü ß`.
- **LED-Punktvorschau:** Festes 64×16-Dot-Matrix-Fenster mit ausgeschalteten und leuchtenden runden LEDs. Längere Texte werden in der Vorschau **nicht** verkleinert.
- **Preview-Player:** Play/Pause, Neustart, Beschleunigung 1× / 2× / 4× / 8×. Beschleunigung wirkt nur auf die Webvorschau.
- **Manuelle Anzeigemodi:** `Standbild` oder `Lauftext` unabhängig von der Textlänge. Ein zu breites Standbild zeigt einen sichtbaren Hinweis und den linken Ausschnitt, **noch keine automatischen Seiten**.
- **Laufgeschwindigkeit:** Schieberegler 1–100, auf BLE-Rohwerte 0–255 umgerechnet, über CoolLEDX-Befehl `0x07`. **Echte Geschwindigkeit noch nicht kalibriert.** Die Browser-Vorschau ist nur eine Annäherung.
- **Diagnose:** `Log kopieren` mit Clipboard-API, älterem Kopierweg und manuell markierbarem Text als Fallback. TXT-Download bleibt möglich.
- **Systemanzeige:** Verständliche Resetgründe inklusive `USB-Reset`. Tatsächlich verbleibender Platz der aktiven Firmwarepartition wird berechnet, nicht mehr die falsche Größe einer Updatepartition angezeigt.
- **Konkurrierende BLE-Verbindungen:** Bei Verbindungsfehler wird auf eine eventuell noch verbundene Hersteller-App hingewiesen; kürzerer Verbindungs-Timeout als Test.
- **Eine Schriftquelle:** `/api/font` liefert Glyphen aus **derselben C++-Tabelle**, die für die LED-Übertragung verwendet wird. Die Webvorschau hält keine zweite getrennte Fontkopie.

## Installation

1. Paket entpacken und die **Ordner/Dateien aus diesem ZIP** in deinen bestehenden Git-Projektordner `RavestickBoard-for-CoolLED1248` kopieren. Vorhandene README-, CHANGELOG- und docs-Dateien ersetzen; ältere `.ino`-Versionen erhalten.
2. In Arduino IDE `firmware/RavestickBoard_v0_1_2.ino` öffnen. Wenn Arduino einen gleichnamigen Sketch-Ordner anlegen möchte, diesen innerhalb des Projektordners erzeugen, damit Änderungen weiterhin von Git erfasst werden.
3. Board: `ESP32C3 Dev Module`; Port: der aktuelle COM-Port; `USB CDC On Boot: Enabled`; Partition: **`No OTA (2MB APP/2MB SPIFFS)`**; Espressif-Boardpaket 3.3.12.
4. Über **USB-C** flashen. Nach erfolgreichem Start mit dem WLAN **`CoolLED1248`** verbinden, Browser: **http://192.168.4.1**.
5. **Zunächst einen kurzen Text prüfen**, danach Kleinbuchstaben und `ÄÖÜäöüß`, Standbild und Lauftext, Geschwindigkeit, Diagnose und Speicherwerte. Siehe [Testplan](docs/TESTPLAN.md).

**Das WLAN und sein Testpasswort bleiben gegenüber v0.1.1 unverändert.** Standardpasswort im Quellcode vor einem Fremdeinsatz ändern. Kein zusätzlicher Weblogin, kein OTA.

## Bestand und Dokumentation

- [Themenspeicher](docs/THEMENSPEICHER.md): Aufgabenregister und offene Anforderungen.
- [Roadmap](docs/ROADMAP.md): Entwicklungsreihenfolge, kein verbindlicher Terminplan.
- [Changelog](CHANGELOG.md): nachweisbare Änderungen, Hardwaretest-Status und offene Punkte.
- [Diagnose](docs/DIAGNOSE.md): Diagnoseexport, Fehlerzähler, Grenzen des RAM-Logs.
- [Entscheidungen](docs/ENTSCHEIDUNGEN.md): Architektur und Bedienprinzipien.
- [Technik](docs/TECHNIK.md): CoolLEDX BLE und ESP32.
- [Quellen und Lizenzen](docs/QUELLEN_UND_LIZENZEN.md): vor öffentlichem Release unbedingt prüfen.

## Aufbau

```text
RavestickBoard-for-CoolLED1248/
├── README.md
├── CHANGELOG.md
├── firmware/
│   ├── ... ältere Sketche (nicht löschen)
│   └── RavestickBoard_v0_1_2.ino
├── web/
│   └── index.html
├── tools/
│   └── sync_web.py
└── docs/
```

**Weboberfläche verändern:** `web/index.html` editieren, anschließend `python tools/sync_web.py` ausführen. Das Skript kopiert die Webseite in den vollständigen `.ino`-Sketch. Dadurch stimmen die Webquelle und der Firmwarestand überein. Der ESP32 benötigt **kein Dateisystem** für das Webinterface.

## Weiterhin offen

- Seiteneditor mit individuellen Seiten, Seitenwechseln und Anzeigedauern.
- Farb- und Schriftauswahl je Zeichen; Originalschriften Normal/Bold, Mini-Zweizeilenfont.
- Exakter Geschwindigkeitsabgleich zwischen Browser-Vorschau und tatsächlicher LED-Tafel.
- Schrift-/Sonderzeichenvollständigkeit, mehrere Textblöcke, Bibliothek und Schnellzugriffe.
- WLAN-/BLE-Langzeittest, Heapüberwachung, Flash-Dateisystem und Wiederverbindung bei fremder Original-App.
- Copyright-/Lizenzprüfung vor einem möglichen öffentlichen GitHub-Release.

**Kein Code gilt als hardwaregetestet, nur weil er syntaktisch geprüft wurde.** v0.1.1 bleibt bis zum erfolgreichen v0.1.2-Gerätetest unsere verlässlichere Rückfallversion.
