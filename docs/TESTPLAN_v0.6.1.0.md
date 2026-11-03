# RavestickBoard · Hardwaretest v0.6.1.0-dev (Animationen: Import, Liste, Senden)

**Stand: 3.11.2026 · vor dem Flashen geschrieben.** Ersetzt den Testplan von 0.6.0.2 (dessen Ergebnisse stehen im README und im Changelog). Dieser Plan beschreibt, was geprüft werden soll; die Spalten „Ergebnis“ und „Bemerkung“ sind leer. Am Rechner geprüft sind Dateispeicher (gegen ein Dateisystem im Speicher), Nutzlast, Importfunktionen und Oberfläche (siehe `tests/README.md`). **Nicht belegt:** Arduino-Compile der neuen Datei `RavestickBoard_Anim.h`, echtes LittleFS und Webserver, Senden aus der Datei über BLE, Heap, Safari auf iPhone und iPad, Wirkung der Helligkeit auf Animationen.

## Vorbereitung

1. JSON-Backup der Bibliothek sichern (Einstellungen), bisherige Firmware (0.6.0.2) als Rückfallversion aufbewahren.
2. `firmware/RavestickBoard_v0_6_1_0/RavestickBoard_v0_6_1_0.ino` öffnen, **alle sieben Dateien** im Sketch-Ordner lassen (neu: `RavestickBoard_Anim.h`), Einstellungen unverändert (ESP32C3 Dev Module, Core 3.3.12, USB CDC On Boot: Enabled, No OTA 2MB APP/2MB SPIFFS, Erase All Flash aus). „Meine Tafel“ bleibt erhalten.
3. Original-App der Tafel vom Handy trennen. Tafel einschalten, in Reichweite aufstellen.
4. Testmaterial bereithalten: ein kleines GIF (wenige Bilder), ein größeres GIF (mehr als 79 Bilder, möglichst mit Transparenz), ein Bildstreifen, ein Text-Raster aus Excel (16 Zeilen × 64 Spalten, Buchstaben `.RGBCMYW`).
5. Animationsversand und Animationstest **ersetzen den Inhalt der Tafel**. Für jeden Test notieren, was die Tafel zeigt (nichts / Standbild / Animation / ERROR), und danach das Diagnoseprotokoll exportieren.

## A. Start und Regression

| Nr. | Bereich | Schritt | Erwartung | Ergebnis | Bemerkung |
|---|---|---|---|---|---|
| A1 | Build | Sketch kompiliert ohne Fehler (die `-Wvolatile`-Warnungen zu `++notifyCount` sind alt und harmlos); besonders Meldungen zu `RavestickBoard_Anim.h`, `File`, `openNextFile`, `rename` | kompiliert | | |
| A2 | Start | WLAN `CoolLED1248`, Weboberfläche, Version | 0.6.1.0-dev; „Meine Tafel“ aktiv | | |
| A3 | Menü | Hauptmenü öffnen | neuer Punkt „Animationen“ (Film-Symbol), Reihenfolge der übrigen Punkte unverändert | | |
| A4 | Regression | Lauftext senden, Blockseite senden, Helligkeit, Tempo, Programm abspielen | wie bei 0.6.0.2; kein Auftrag bricht ab | | |
| A5 | Einstellungen | Abschnitt „Animationstest“ | vier Tasten und „Zeit je Frame“; **keine** Auswahl „Anzeigemodus danach“ | | |

## B. Speichern und Liste

| Nr. | Schritt | Erwartung | Ergebnis | Bemerkung |
|---|---|---|---|---|
| B1 | Animationen öffnen (Dateisystem eingerichtet) | leere Liste mit Hinweis; kein Fehler | | |
| B2 | Dateisystem nicht eingerichtet (falls es sich nachstellen lässt) | verständliche Meldung statt Fehler | | |
| B3 | Kleines GIF importieren, Standardwerte, speichern | Vorschau läuft, Speichern meldet Erfolg, Eintrag erscheint mit Vorschaubild, Name, Bildzahl, Zeit | | |
| B4 | Seite neu laden (Menü), Liste erneut öffnen | Eintrag bleibt; Vorschau stimmt | | |
| B5 | ESP32 neu starten (Strom), Liste öffnen | Eintrag bleibt (LittleFS) | | |
| B6 | Umbenennen, Duplizieren, Löschen | wie angezeigt; Duplikat hat eigene ID und denselben Inhalt; Löschen entfernt den Eintrag | | |
| B7 | Mehrere Animationen anlegen, bis Speicher oder Limit (40) erreicht | klare Meldung statt Absturz; Reserve von 24 KiB bleibt frei | | |
| B8 | Während des Hochladens zweites Gerät im WLAN | Hochladen läuft durch; Heap-Minimum in der Diagnose notieren | | |

## C. Import und Anpassen (iPhone/iPad-Safari und Desktop)

| Nr. | Schritt | Erwartung | Ergebnis | Bemerkung |
|---|---|---|---|---|
| C1 | GIF mit Transparenz und Teilbildern | Vorschau sieht aus wie das Original (Farben auf sieben reduziert) | | |
| C2 | Großes GIF (> 79 Bilder) | Hinweis auf die Grenze; „Auf 79 Bilder verteilen“ wählt gleichmäßig; Zeit pro Bild notieren (Decoder-Dauer, Absturz?) | | |
| C3 | Einpassen (komplett, ausfüllen, strecken, Original) mit Verschieben | Vorschau folgt | | |
| C4 | Farbreduktion: Schwelle, Floyd-Steinberg, Bayer, Schwellwert, Kontrast | Vorschau folgt; Bayer/Floyd gut bei Verläufen | | |
| C5 | Bildstreifen importieren | Bilder in richtiger Reihenfolge | | |
| C6 | Text-Raster aus Excel einfügen (Tabulator-getrennt), mit zwei Bildern getrennt durch Leerzeile oder `---` | zwei Bilder, Farben passen zu den Buchstaben | | |
| C7 | Bildbereich, Schrittweite, Hin und Zurück, wiederholte Bilder | Bildzahl und Dauer in der Anzeige stimmen | | |
| C8 | Eingabefelder (iPhone) | kein Zoomen beim Antippen | | |

## D. Senden an die Tafel

| Nr. | Schritt | Erwartung | Ergebnis | Bemerkung |
|---|---|---|---|---|
| D1 | Animation mit 16 Bildern senden | Meldung „gesendet“; Tafel zeigt die Animation wie in der Vorschau (Farben, Richtung); Dauer notieren | | |
| D2 | Animation mit 79 Bildern senden | wird angenommen (30 363 B); **Dauer notieren** (erwartet etwa 55 s; Oberfläche zeigt eine Schätzung); Wächter-Neustart? Letzte Phase notieren | | |
| D3 | Tafel aus- und einschalten | Animation bleibt (bei 0.6.0.2 bestätigt) | | |
| D4 | Danach Lauftext senden | Text ersetzt die Animation sauber | | |
| D5 | Tempo-Regler nach dem Senden einer Animation | gesperrt mit Hinweis; nach einem Lauftext wieder frei | | |
| D6 | ESP32 neu starten, Tempo-Regler | bedienbar (Zustand unbekannt) | | |
| D7 | Helligkeit ändern, nachdem eine Animation läuft | **notieren, ob die Helligkeit wirkt** (offen aus 0.6.0.2) | | |
| D8 | Tafel ausschalten, Animation senden | Meldung „LED-Tafel nicht gefunden“, kein Hänger | | |
| D9 | Während des Sendens einen anderen Befehl (zweites Gerät) | Antwort „beschäftigt“ (409) oder Wartezeit; kein Neustart | | |
| D10 | 80 Bilder: Animationstest „80 Frames“ wiederholen | Tafel lehnt schon das erste Paket ab (Code 0x04); Oberfläche meldet es; danach ist alles weiter bedienbar | | |

## E. Diagnose

| Nr. | Schritt | Erwartung | Ergebnis | Bemerkung |
|---|---|---|---|---|
| E1 | Diagnose exportieren | Ereignisse `ANIM` plausibel; `ackOk` steigt mit den Paketen; `ackNak`/`ackFatal` nur bei echten Fehlern | | |
| E2 | Heap-Tiefstwert nach den Tests | nicht schlechter als bei 0.6.0.2 (Vergleichswert notieren) | | |
| E3 | Hänger mit Wächter-Neustart | falls aufgetreten: Phase und Paketnummer notieren | | |

## Was zurückmelden

Pro Punkt „ok“, „nicht ok“ oder die Beobachtung in Worten; besonders A1, B3–B5, D1–D2, D5, D7 und D10. Dazu den Diagnose-Export (Text). Aus den Ergebnissen folgt 0.6.1.1 (Animationen als Kurzwahl) oder zuerst eine Korrektur.
