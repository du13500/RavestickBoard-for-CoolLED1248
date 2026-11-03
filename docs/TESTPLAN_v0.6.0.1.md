# RavestickBoard · Hardwaretest v0.6.0.1-dev (Animationstest, Befehl 0x04)

**Stand: 3.11.2026 · vor dem Flashen geschrieben.** Er ersetzt den Testplan der nie geflashten Fassung 0.6.0.0; neu in 0.6.0.1 ist die Frame-Nummer in allen Testmustern. Dieser Plan beschreibt, was geprüft werden soll; die Spalten „Ergebnis“ und „Bemerkung“ sind leer. Am Rechner geprüft sind nur Nutzdaten, Paketierung und Oberfläche (siehe `tests/README.md`). **Nicht belegt:** Arduino-Compile, ob die Tafel `0x04` annimmt, wie sie abspielt, was `0x06` danach bewirkt und was nach dem Ausschalten bleibt. Genau das soll dieser Test klären; die Ergebnisse entscheiden über 0.6.1.0 (Import und Animationsliste).

## Vorbereitung

1. JSON-Backup der Bibliothek aus der laufenden Firmware sichern (Einstellungen), bisherige Firmware als Rückfallversion aufbewahren (0.5.0.1).
2. `firmware/RavestickBoard_v0_6_0_1/RavestickBoard_v0_6_0_1.ino` öffnen, alle sechs Dateien im Sketch-Ordner lassen, Einstellungen unverändert (ESP32C3 Dev Module, Core 3.3.12, USB CDC On Boot: Enabled, No OTA 2MB APP/2MB SPIFFS, Erase All Flash aus). „Meine Tafel“ bleibt erhalten (NVS unverändert).
3. Original-App der Tafel vom Handy trennen (BLE-Konkurrenz). Tafel einschalten, in Reichweite aufstellen.
4. **Jedes Frame zeigt seine Nummer** (01, 02, …, zweistellig in der Mitte). Zum Lesen sind etwa 300 ms je Frame oder mehr nötig; bei 60 ms oder weniger ist die Nummer nur zu erahnen. Notiere bei jedem Lauf, bis zu welcher Nummer die Tafel kommt und welche Nummer nach der höchsten folgt.
5. Der Animationstest **ersetzt den Inhalt der Tafel**; danach ist zum Herstellen des Normalzustands ein normaler Textversand nötig. Für jeden Test sichtbar notieren, was die Tafel zeigt (nichts / Standbild / Animation / ERROR) und das Diagnoseprotokoll exportieren.

## A. Start und Regression

| Nr. | Bereich | Schritt | Erwartung | Ergebnis | Bemerkung |
|---|---|---|---|---|---|
| A1 | Build | Sketch kompiliert ohne Fehler und Warnungen zu `rb60*` | kompiliert | | |
| A2 | Start | WLAN `CoolLED1248`, Weboberfläche, Version | 0.6.0.1-dev; „Meine Tafel“ aktiv (Einstellungen) | | |
| A3 | Einstellungen | ganz nach unten scrollen | Abschnitt „Animationstest“ mit vier Tasten, Feld „Zeit je Frame“, Auswahl „Anzeigemodus danach“ | | |
| A4 | Regression | kurzen Text senden (Lauftext), danach eine Blockseite, Helligkeit, Tempo | wie bei 0.5.0.1; Diagnose „Antwortcodes“ zählt hoch, kein Auftrag bricht ab | | |

## B. Animation annehmen und abspielen

| Nr. | Schritt | Erwartung | Ergebnis | Bemerkung |
|---|---|---|---|---|
| B1 | „Farben (7 Frames)“, Modus „unverändert lassen“, Zeit leer (700 ms) | Meldung „Animationstest gesendet“; die Tafel zeigt sieben Farbflächen mit schwarzer Nummer 01 bis 07 in der Reihenfolge **blau, grün, cyan, rot, magenta, gelb, weiß** und beginnt danach wieder bei 01. Abweichende Farben deuten auf vertauschte Farbebenen. Notieren: Farben, Nummern, Zeit je Frame, Wiederholung | | |
| B2 | „Balken (16 Frames)“, Zeit auf 300 ms stellen (Standard 60 ms ist zu schnell zum Lesen der Nummer) | grüne Nummer 01 bis 16; weißer Balken wandert von links nach rechts; oben links ein roter, unten rechts ein blauer Eckpixel. Spiegelung oder vertauschte Farben notieren (Ebenenreihenfolge, Spaltenrichtung); Nummern in falscher Reihenfolge oder fehlende Nummern notieren | | |
| B3 | Balken mit 200 ms, danach 60 ms, 20 ms und **8 ms** | kleinerer Wert = schneller. Notieren, ab wann es ruckelt oder Frames übersprungen werden (Nummern!). 8 ms entspricht Adams Versuchen mit Flackertrick für mehr Farben | | |
| B4 | Balken, Modus danach „statisch (0x01)“ | notieren: spielt weiter / bleibt auf Frame stehen / ERROR | | |
| B5 | Balken, Modus danach „Lauftext (0x02)“ | notieren: Animation läuft weiter / wird als Lauftext bewegt / nichts | | |
| B6 | Balken, Modus „unverändert“ direkt nach einem Lauftext-Versand | notieren: spielt die Animation oder bleibt der alte Modus wirksam | | |

## C. Grenzen und Fehler

| Nr. | Schritt | Erwartung | Ergebnis | Bemerkung |
|---|---|---|---|---|
| C1 | „Grenze: 79 Frames“ (Standard 300 ms, ein Durchlauf etwa 24 s) | wird angenommen (30 363 B); Dauer der Übertragung notieren (Adam: rund 55 s von einem Mac). Die weiße Nummer läuft von 01 bis 79 und beginnt dann wieder bei 01. Letzte sichtbare Nummer und Nummer nach 79 notieren | | |
| C2 | „80 Frames (Fehler erwartet)“ | Laut Adam lehnt die Tafel **schon das erste Paket** ab: Meldung „Tafel lehnt Paket **1** von m ab: Code 0x04 (Datenlänge)“ (Paketnummer und Code notieren); Diagnose „Fehlercodes“ steigt. Abweichung (anderer Code, spätere Paketnummer, ERROR auf der Tafel, Hänger) genau notieren. Zeigt die Tafel danach noch die vorherige Animation? | | |
| C3 | Direkt nach C2 wieder „Farben“ senden | Verbindung und Tafel ohne Neustart nutzbar | | |
| C4 | Diagnose exportieren | Zähler `ackOk/ackNak/ackFatal/ackOther` plausibel; `ackNak` > 0 nur bei echten Prüfsummenfehlern | | |

## D. Persistenz und Zusammenspiel

| Nr. | Schritt | Erwartung | Ergebnis | Bemerkung |
|---|---|---|---|---|
| D1 | Nach B2 die Tafel aus- und wieder einschalten | notieren: Animation läuft weiter / Tafel zeigt Text / leer. Das entscheidet, ob Animationen dauerhaft auf der Tafel liegen | | |
| D2 | Nach B2 einen normalen Text senden | Text ersetzt die Animation; notieren, ob Reste sichtbar bleiben | | |
| D3 | Nach einem Text eine Animation senden, dann Helligkeit und Tempo ändern | Helligkeit wirkt; Tempo-Regler (`0x07`) wirkt auf die Animation? notieren | | |
| D4 | Tafel ausschalten, Animation senden | Fehlermeldung „LED-Tafel nicht gefunden“ (Vor-Scan), kein Hänger | | |

## E. Stabilität

| Nr. | Schritt | Erwartung | Ergebnis | Bemerkung |
|---|---|---|---|---|
| E1 | Während der 79-Frame-Übertragung zweites Gerät im WLAN | Weboberfläche antwortet erst nach dem Auftrag (wie bei Textaufträgen); kein Neustart | | |
| E2 | Heap-Tiefstwert in der Diagnose nach den Tests | nicht schlechter als bei 0.5.0.1 (Vergleichswert notieren) | | |
| E3 | Hänger mit Wächter-Neustart (Phase in der Diagnose) | falls aufgetreten: Phase und Paketnummer notieren | | |

## Was zurückmelden

Pro Punkt „ok“, „nicht ok“ oder die Beobachtung in Worten; besonders B1–B6, C1, C2 und D1. Dazu den Diagnose-Export (Text) nach dem Durchlauf. Aus den Ergebnissen folgt: Nimmt die Tafel `0x04` an, wird 0.6.1.0 (Import, Animationsliste) gebaut; nimmt sie ihn nicht an oder verhält sich anders als beschrieben, wird zuerst die Ursache geklärt (siehe `docs/ROADMAP.md`).
