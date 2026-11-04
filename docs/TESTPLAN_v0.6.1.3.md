# RavestickBoard · Hardwaretest v0.6.1.3-dev (Aufräum-Build: Prüfsumme, Kurzwahl-Bereinigung, Stapelwert, Regression der Animationen)

**Stand: 4.11.2026 · vor dem Flashen geschrieben.** Ersetzt den Testplan von 0.6.1.2 (liegt in `archive/`; 0.6.1.2 lief auf der Hardware, GIF-Bildqualität begrenzt durch die Tafel). 0.6.1.3 bringt **keine neue Bedienfunktion**, sondern Absicherungen aus dem Code-Review: Prüfsumme (CRC-32) der Animationsdateien, Abbruch bei Lesefehler, Kurzwahlen beim Löschen leeren, sicheres Ersetzen, Stapelwert in der Diagnose. Die Abschnitte A bis I sind die Regression von 0.6.1.2 (Texte unverändert, wo nichts anderes steht), **neu ist Abschnitt J**.

## Vorbereitung

1. JSON-Backup der Bibliothek sichern (Einstellungen), bisherige Firmware (0.6.1.2) als Rückfallversion aufbewahren.
2. `firmware/RavestickBoard_v0_6_1_3/RavestickBoard_v0_6_1_3.ino` öffnen, **alle sieben Dateien** im Sketch-Ordner lassen (unverändert seit 0.6.1.0), Einstellungen unverändert (ESP32C3 Dev Module, Core 3.3.12, USB CDC On Boot: Enabled, No OTA 2MB APP/2MB SPIFFS, Erase All Flash aus). „Meine Tafel“ bleibt erhalten.
3. Original-App der Tafel vom Handy trennen. Tafel einschalten, in Reichweite aufstellen.
4. Testmaterial bereithalten: ein kleines GIF (wenige Bilder), ein größeres GIF (mehr als 79 Bilder, möglichst mit Transparenz), ein Bildstreifen, ein Text-Raster aus Excel (16 Zeilen × 64 Spalten, Buchstaben `.RGBCMYW`).
5. Animationsversand und Animationstest **ersetzen den Inhalt der Tafel**. Für jeden Test notieren, was die Tafel zeigt (nichts / Standbild / Animation / ERROR), und danach das Diagnoseprotokoll exportieren.

## A. Start und Regression

| Nr. | Bereich | Schritt | Erwartung | Ergebnis | Bemerkung |
|---|---|---|---|---|---|
| A1 | Build | Sketch kompiliert ohne Fehler (die `-Wvolatile`-Warnungen zu `++notifyCount` usw. sind alt und harmlos); in 0.6.1.0 war `rb42ProgramPlaying` in `RavestickBoard_Anim.h` der einzige Fehler. Danach können noch Meldungen des Linkers kommen; besonders auf `RavestickBoard_Anim.h`, `File`, `openNextFile`, `rename` achten | kompiliert | | |
| A2 | Start | WLAN `CoolLED1248`, Weboberfläche, Version | 0.6.1.3-dev; „Meine Tafel“ aktiv | | |
| A3 | Menü | Hauptmenü öffnen | Punkt „Animationen“ mit dem Symbol Bild mit Wiedergabepfeil (`image-play`), Reihenfolge der übrigen Punkte unverändert | | |
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
| B6 | Umbenennen, Löschen (Duplizieren gibt es in der Oberfläche nicht mehr) | wie angezeigt; Löschen entfernt den Eintrag; die Symbolleiste zeigt Importieren, Senden (mint), Umbenennen, Löschen | | |
| B7 | Mehrere Animationen anlegen, bis Speicher oder Limit (40) erreicht | klare Meldung statt Absturz; Reserve von 24 KiB bleibt frei | | |
| B8 | Während des Hochladens zweites Gerät im WLAN | Hochladen läuft durch; Heap-Minimum in der Diagnose notieren | | |

## C. Import und Anpassen (iPhone/iPad-Safari und Desktop)

| Nr. | Schritt | Erwartung | Ergebnis | Bemerkung |
|---|---|---|---|---|
| C1 | GIF mit Transparenz und Teilbildern | Vorschau sieht aus wie das Original (Farben auf sieben reduziert) | | |
| C2 | Großes GIF (> 79 Bilder) | Hinweis auf die Grenze; „Auf 79 Bilder verteilen“ wählt gleichmäßig; Zeit pro Bild notieren (Decoder-Dauer, Absturz?) | | |
| C3 | Voreinstellungen Einpassen, Füllen, Strecken, Original (setzen nur den Rahmen, Details in Abschnitt F) | Vorschau folgt | | |
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

## F. Ausschnitt-Rahmen (aus 0.6.1.2)

| Nr. | Schritt | Erwartung | Ergebnis | Bemerkung |
|---|---|---|---|---|
| F1 | GIF importieren, großes Bild (mindestens 2× so groß wie die Tafel) | über dem Bild liegt ein gelber Rahmen im Seitenverhältnis 4 : 1, außerhalb abgedunkelt; die Vorschau darunter zeigt genau den Rahmeninhalt | | |
| F2 | Rahmen mit dem Finger (iPhone/iPad), Stift (Apple Pencil) bzw. Maus verschieben | folgt dem Finger, die Vorschau folgt verzögert (bis etwa 0,2 s); die Seite scrollt dabei **nicht** mit | | |
| F3 | An einer Ecke ziehen | Rahmen wird größer/kleiner, die gegenüberliegende Ecke bleibt stehen, Verhältnis bleibt 4 : 1 | | |
| F4 | Mit zwei Fingern auf dem Bild spreizen/zusammenziehen (nur Touch), Mausrad (Desktop) | Rahmen wird kleiner/größer; Safari zoomt dabei nicht die ganze Seite | | |
| F5 | „Rahmen kleiner“ / „Rahmen größer“ | in Schritten; für Geräte ohne Zwei-Finger-Bedienung | | |
| F6 | Voreinstellung Einpassen, Füllen, Original, Strecken durchschalten | Rahmen springt passend (Einpassen: ganzes Bild, ggf. mit schwarzen Rändern; Füllen: Bild füllt die Tafel; Strecken: ganzes Bild, Rahmen gestrichelt); nach dem Verschieben steht „Eigener Ausschnitt“ | | |
| F7 | Rahmen teilweise über den Bildrand schieben | außerhalb des Bildes ist die Tafel schwarz | | |
| F8 | GIF mit mehreren Bildern, Feld „Von Bild“ ändern | die Rahmenansicht zeigt das gewählte Bild | | |
| F9 | Gespeicherte Animation senden | Tafel zeigt denselben Ausschnitt wie die Vorschau | | |

## G. Speichern, Speichern und senden, Nur senden (aus 0.6.1.2)

| Nr. | Schritt | Erwartung | Ergebnis | Bemerkung |
|---|---|---|---|---|
| G1 | „Speichern“ | Eintrag in der Liste, nichts wird gesendet | | |
| G2 | „Speichern und senden“ | Eintrag in der Liste und Tafel zeigt die Animation; beide Senden-Knöpfe mint | | |
| G3 | „Nur senden (nicht speichern)“ | Tafel zeigt die Animation; **kein** neuer Listeneintrag; Speicher (Liste, „frei“) danach wie vorher | | |
| G4 | Nach G3 ESP32 neu starten, Liste und Diagnose ansehen | keine Reste (die temporäre Datei `send.rba` wird nach dem Senden gelöscht, `up.tmp` beim Start) | | |
| G5 | Nur senden mit 40 gespeicherten Animationen | Speichern gesperrt mit Hinweis, „Nur senden“ geht | | |
| G6 | Nur senden während ein anderer Auftrag läuft | Meldung „beschäftigt“ statt Hänger | | |

## H. Animationen als Kurzwahl (aus 0.6.1.2)

| Nr. | Schritt | Erwartung | Ergebnis | Bemerkung |
|---|---|---|---|---|
| H1 | Kurzwahl belegen: Quelle „Animationen“ (Symbol `image-play`) | Liste der gespeicherten Animationen, Beschriftung vorbelegt | | |
| H2 | Kachel antippen | Tafel zeigt die Animation (Dauer wie beim Senden aus der Liste, bei 79 Bildern etwa 55 s) | | |
| H3 | Animation löschen, die einer Kurzwahl zugeordnet ist | Rückfrage nennt die Kurzwahl; danach zeigt die Kachel „Animation fehlt“ und sendet nichts (Meldung) | | |
| H4 | Neustart des ESP32 | Zuordnung bleibt (Kurzwahlen liegen im NVS) | | |
| H5 | Backup der Bibliothek (Einstellungen) | enthält die Animationen **nicht** (Dateien liegen im LittleFS, nicht im JSON-Backup) | | |

## I. Bestehende Animation bearbeiten (aus 0.6.1.2)

| Nr. | Schritt | Erwartung | Ergebnis | Bemerkung |
|---|---|---|---|---|
| I1 | Animation in der Liste wählen, Knopf „Bearbeiten“ (Stift) | Ansicht „Animation bearbeiten“ ohne Quelle und Ausschnitt; Name, Zeit und Bildbereich vorbelegt; Vorschau läuft | | |
| I2 | Zeit je Frame ändern (z. B. halbieren), Bildbereich kürzen | Anzeige „n Frames · x ms“ und Vorschau folgen | | |
| I3 | „Speichern“ | Liste zeigt dieselbe Animation (gleiche Stelle, neuer Name/Zeit); kein zusätzlicher Eintrag | | |
| I4 | Animation senden | Tafel zeigt die geänderte Zeit/Bilder | | |
| I5 | Einer Kurzwahl zugeordnete Animation bearbeiten und speichern | Kurzwahl zeigt weiter auf sie und sendet die neue Fassung | | |
| I6 | „Als Kopie speichern“ | neuer Eintrag, Original unverändert | | |
| I7 | „Speichern und senden“ und „Nur senden“ im Bearbeiten | wie beschriftet; Nur senden legt nichts in der Liste an | | |
| I8 | Mit 40 Animationen: Bearbeiten, „Speichern“ | geht; „Als Kopie speichern“ gesperrt | | |
| I9 | ESP32 während des Überschreibens vom Strom trennen (nur wenn du es testen magst) | nach dem Start: Original oder neue Fassung vorhanden, keine halbe Datei in der Liste; Diagnose zeigt ggf. „Unfertiger Animations-Upload entfernt“ | | |
| I10 | Prüfen, ob das Überschreiben auch bei knappem Speicher klappt (ein sehr großes Original, danach Bearbeiten) | entweder erfolgreich oder verständliche Meldung „Nicht genug freier Speicher“ | | |

## J. Neu in 0.6.1.3: Prüfsumme, Kurzwahl-Bereinigung, Stapelwert

| Nr. | Schritt | Erwartung | Ergebnis | Bemerkung |
|---|---|---|---|---|
| J1 | Eine Animation normal speichern und senden | wie bisher; kein neuer Hinweis | | |
| J2 | Eine **alte** Animation aus 0.6.1.2 (ohne Prüfsumme) in der Liste wählen und senden | wird angezeigt und gesendet wie zuvor (keine Prüfsummenmeldung) | | |
| J3 | Diese alte Animation bearbeiten und speichern, danach senden | gespeichert (jetzt mit Prüfsumme), sendet; Liste zeigt weiter einen Eintrag | | |
| J4 | Eine Animation einer Kurzwahl zuordnen, dann in der Liste **löschen** | Rückfrage nennt die Kurzwahl („Diese Kurzwahl wird dabei geleert“); danach ist die Kachel leer, nicht „Animation fehlt“ | | |
| J5 | Neue Animation speichern, deren Nummer die gelöschte wiederverwendet (wenn es die höchste war) | die geleerte Kachel bleibt leer und spielt **nicht** die neue Animation | | |
| J6 | Animation mit Kurzwahl bearbeiten und speichern (Überschreiben) | Kurzwahl zeigt weiter auf sie (wie I5) | | |
| J7 | Mehrfach hintereinander bearbeiten und überschreiben (5×) | jedes Mal „gespeichert“, Liste unverändert groß, keine Meldung zu `.bak` im Diagnoseprotokoll | | |
| J8 | Diagnose: nach „Animation senden“ das Protokoll ansehen | Ereignis „Animation startet“ (beim Animationstest „Animationstest startet“) | | |
| J9 | Diagnose exportieren | Zeile „Kleinster freier Stapel (Loop-Task): … Bytes“ vorhanden; Wert **notieren** (nach einem Durchgang aus Speichern, Bearbeiten, Senden, Löschen, Liste laden, Animationstest) | | |
| J10 | Beim Arduino-Compile die Zeile „Globale Variablen verwenden … Bytes“ | Wert **notieren** (Befund M4) | | |
| J11 | Wiederholung der Grenzfälle: Animation mit **79** Bildern speichern und senden, danach **80** versuchen | 79 geht; bei 80 meldet die Oberfläche die Grenze | | |
| J12 | Helligkeit: Regler bewegen, Animation senden, Tafel aus und wieder ein | beobachten, ob die Helligkeit der Animation zum Regler passt (Befund N3); Beobachtung in Worten | | |
| J13 | Löschen-Rückfrage einer Animation ohne Kurzwahl | normaler Text ohne Hinweis auf Kurzwahlen | | |
| J14 | (nur wenn du magst) Nach dem Speichern einer Animation den Strom trennen, neu starten | Animation ist da oder nicht da, keine halbe Datei in der Liste; Diagnose ggf. „Unfertiger Animations-Upload entfernt“ oder „Alte Animation … wiederhergestellt“ | | |

*Nicht am Gerät provozierbar:* ein echter Prüfsummenfehler (Meldung „Pruefsumme stimmt nicht …“) und ein Lesefehler beim Senden lassen sich ohne beschädigten Speicher nicht auslösen; sie sind am Rechner mit Attrappen geprüft. Falls einer auftritt: Meldung, Diagnose-Export und was die Tafel zeigt zurückmelden.

## Was zurückmelden

Pro Punkt „ok“, „nicht ok“ oder die Beobachtung in Worten; besonders A1, A2, B3–B5, D1–D2, D5, D7, D10, F2–F4 (Touch-Bedienung), G3, H2–H3, I3–I5 und J2–J5, J7, J9, J10 und J12. Dazu den Diagnose-Export (Text). Aus den Ergebnissen folgt (mit deinem Go) der Zeichen-Editor (0.6.2.0) oder zuerst eine Korrektur.
