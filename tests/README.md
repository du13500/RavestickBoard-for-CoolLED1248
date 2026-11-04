# Offline-Tests

**Aktuell: v0.6.1.2-dev** (Abschnitt „v0.6.0.0-dev bis v0.6.1.2-dev“ unten). Die weiteren Abschnitte beschreiben ältere Stände mit den damaligen Dateinamen; `test_build_4253.py` bis `browser_8_shortcuts.py` am Anfang gehören zum Stand 0.4.2.5.4.

- `python tests/test_build_4253.py`: prüft Version, Schema-Konstanten, gzip/ETag, JavaScript/C++-Syntax und JSON-Chunks. Enthält Regressionen auf die sieben `<span>`-Menülabels, bereits gerenderte Kurzwahl-SVGs und den initialen Home-Zustand.
- `python tests/browser_hotfix_smoke.py`: prüft mit einem headless Chromium und simulierten lokalen APIs die sichtbare Startansicht, den Ladezustand, das Hauptmenü und den Wechsel Home ↔ Texteditor.

Der Browser-Test erfordert Playwright und Chromium; Arduino-Boardpaket und reale Hardware werden dadurch nicht ersetzt. Insbesondere wird die kritische RAM-Untergrenze hier **nicht** getestet oder als behoben behauptet.

- `python tests/browser_8_shortcuts.py`: prüft alle acht Kurzwahlen, Bibliothek ohne Favoritensterne, Geschwindigkeitsregler sowie adaptive Höhe auf sechs Bildschirmgrößen.

## Aktuelle v0.4.2.5.4-Prüfungen

`python tests/test_build_4254.py`: statische Verträge, JavaScript-Syntax, gzip, ETag, C++-Header und JSON-Writer-Stub.

`python tests/test_browser_save_as_4254.py`: Simulierter Browser prüft Text- und Blockkopie gegen vorgetäuschte API, dass Originale unverändert bleiben, Varianten neue IDs erhalten, ein Schriftartwechsel die Vorlage nicht ablöst und Änderungen an Kopien nicht das Original überschreiben.

`python tests/browser_8_shortcuts.py`: responsives Layout mit acht Kurzwahlen und Reglern.

**Nicht enthalten:** Arduino-Compile oder ESP32-Hardwarelasttest.

## v0.4.3.1-dev (30.10.2026)

`python tests/test_0431.py` prüft die JavaScript-Syntax, HTML/gzip-/ETag-Konsistenz, die korrigierte Versionskennung, den entkoppelten Diagnose-TXT-Download und die interaktive Darstellung in drei Chromium-Mobilgrößen. Der Stream-Puffer wurde außerdem separat mit einem Host-C++-Test kompiliert. Dies ersetzt **keinen Arduino-Gesamtcompile** und keinen Heap-/BLE-Test auf dem ESP32.

## v0.6.0.0-dev bis v0.6.1.2-dev (3.11.2026)

Die Tests tragen jetzt die Endung `0612` (aus `0611`/`0610`/`0602`/`0601`/`0600`, davor `0500`/`0501`, umbenannt; die Rubrik gilt seit 0.6.0.0). Ältere Abschnitte unten nennen die Dateien unter ihrem damaligen Namen. Aufruf aus dem Repo-Wurzelverzeichnis:

- `python tests/test_build_0612.py`: Version, gzip/ETag, JavaScript-Syntax, Firmware-Invarianten (u. a. Tafeladresse nicht im Code, Animation: Route, Konstanten 79/80 Frames, kein Tempo- und Moduswechsel vor den Daten im Animationsauftrag, strikte Antwortauswertung nur dort) und Dokumentationsversion (README-Banner unter der Titelzeile, Versionsangaben in README, CHANGELOG, VERSIONSGESCHICHTE, Installationspfad).
- `python tests/test_browser_0612.py`: Browser-Test mit simuliertem ESP (Playwright/Chromium), u. a. neuer Abschnitt „Animationstest“ in den Einstellungen (vier Tasten, gesendete Parameter, Zeit je Frame, Modus danach, Fehlermeldung), Tafelverwaltung, Zoom-Sperre.
- `python tests/test_render_parity_0612.py`: Firmware-Seitendarstellung gegen die Browser-Vorschau.
- `python tests/host_sign_0612.py`, `host_scan_0612.py`, `host_program_order_0612.py`: Logik von Tafelverwaltung, Vor-Scan und Programm-Reihenfolge mit dem echten Sketch-Code am Rechner (g++, Attrappen in `tests/host/`).
- **Neu:** `python tests/host_anim_0612.py`: erzeugt Nutzdaten und Chunks der Animation (`0x04`) mit dem echten Sketch-Code (Marken im Sketch, Attrappe `tests/host/anim_stub_0612.h`, Hauptprogramm `anim_main_0612.cpp`) und vergleicht sie mit einer unabhängigen Python-Rechnung (Kopf, Ebenenreihenfolge, Spaltenbytes, Chunk-Kopf, XOR, Rahmen mit Escape, Größen für 7/16/79/80 Frames; seit 0.6.0.1 auch die Frame-Nummer, deren Ziffern in der Rechnung als Textvorlage stehen); prüft den Antwort-Parser (Status 0–6, falsche Länge/Rahmen). Mit `OPENSIGN_SRC=<Pfad zu schulzad/coolled-sign-controller/src>` wird zusätzlich gegen OpenSigns Kodierer und echte aufgezeichnete Antworten abgeglichen; ohne diese Variable entfällt dieser Teil (Hinweis in der Ausgabe). `tests/host/build.py` setzt die Hilfsprogramme aus den Sketch-Marken zusammen.

- **Neu in 0.6.1.2:** `host_animfs_0612.py` prüft zusätzlich die temporäre Datei für „Nur senden“ (Anlegen über `temp=1`, Senden, Aufräumen durch `rb61DropTemp`, kein 40er-Limit, Altlast) und `rb61AnimExists`/`rb61QueueAnim`; `test_anim_core_0612.py` den Ausschnitt (`cropFor`, `cropRect`, `clampCrop`, Gleichheit mit den alten Modi); `test_browser_anim_0612.py` Rahmen (Mausziehen, Knöpfe, Begrenzung, Nutzlast), Nur senden, Senden-Farbe, Kurzwahl-Quelle „Animationen“, Bearbeiten (Überschreiben mit `replace`, Kopie, Limit 40, Zurücksetzen der Importansicht); `test_build_0612.py` Version, neue und entfernte Elemente, Vorwärtsdeklarationen. **Nicht abgedeckt:** Touch-Ereignisse (Zwei-Finger-Geste, Pencil) – Chromium-Test nutzt die Maus; der Sketch-Handler für Kurzwahl Typ 4.
- **In 0.6.1.1:** `python tests/decl_order.py <Pfad zur .ino>` (von `test_build_0612.py` aufgerufen): setzt den Sketch mit seinen `.h`-Dateien in Include-Reihenfolge zusammen und meldet jeden Namen, der vor seiner ersten Deklaration benutzt wird. Findet den Compile-Fehler von 0.6.1.0 (`rb42ProgramPlaying`), nichts in 0.6.1.1. Näherung ohne Präprozessor, kein Ersatz für den Compile.
- **Neu in 0.6.1.0:**
  - `python tests/host_animfs_0612.py`: übersetzt `RavestickBoard_Anim.h` und die Dateizweige des Sketchs mit g++ (Attrappen `tests/host/animfs_stub_0612.h` mit einem Dateisystem im Speicher und `animfs_main_0612.cpp`) und prüft, mit unabhängiger Python-Gegenseite: Upload in Stücken (inkl. Abbruch, falsche Reihenfolge, zu viel Daten), Liste (Vorschaubild, `broken`), Abruf in Stücken, Senden aus der Datei (Chunk-Folge gleich der Referenz), Umbenennen, Duplizieren, Löschen, Grenzen (40 Animationen, 79 Bilder, Speicher voll, Schreibfehler, nicht eingerichtetes Dateisystem, Namenssäuberung UTF-8).
  - `python tests/test_anim_core_0612.py`: Kernfunktionen der Weboberfläche (Block `RbAnim`) in Node: Nutzlast gegen die Referenz aus `host_anim_0612.py`, Base64, GIF-Decoder gegen Pillow (Interlace, Teilbilder, Transparenz, Entsorgungsarten, 12-Bit-Codes) und gegen den Decoder von Chromium (über `gif_ref_chromium_0612.py`; Pillow weicht nur bei Entsorgungsart 3 ab), Raster, Streifen, Farbumsetzung, Planung.
  - `python tests/test_browser_anim_0612.py [Screenshot-Ordner]`: Bereich „Animationen“ in Chromium mit simulierter API (Dateispeicher im Mock): Menü, Liste, drei Importwege, Anpassen, 79-Bilder-Grenze, Speichern in Stücken, Senden, Umbenennen, Duplizieren, Löschen, beschädigte Einträge, gesperrter Tempo-Regler, Layout bei 390 px ohne Zoom.
  - Angepasst: `test_build_0612.py` (Routen, Einbindung hinter `RavestickBoard_Library.h`, `animActive`, Dokumentation 0.6.1.0, Test auf entfernte Modusauswahl), `test_browser_0612.py`, Attrappe `File` in `tests/host/stubs.h`, `tests/host/build.py`.
  - **Nicht geprüft:** Arduino-Compile (Dateifunktionen des Core 3.3.12), echtes LittleFS und Webserver, BLE-Senden, Safari auf iPhone und iPad, Apple Pencil (kommt erst mit 0.6.2.0).
- **0.6.0.2:** `tests/arduino_prototypes.py` (von `test_build_0612.py` aufgerufen; einzeln: `python tests/arduino_prototypes.py <Pfad zur .ino>`): bildet nach, dass die Arduino-IDE ihre Prototypen vor die erste Funktion setzt, und meldet jeden selbstdefinierten Typ in einer Funktionssignatur, der erst danach definiert wird. Das ist ein Näherungstest (Funktionsköpfe in einer Zeile, Typen am Zeilenanfang), kein Ersatz für den Compile.

Weiterhin gilt: kein Arduino-Compile, keine Hardware. Der Test belegt nicht, dass die Tafel `0x04` annimmt.

## v0.5.0.0-dev (3.11.2026)

Die Tests tragen jetzt die Endung `0500`. Neu: `python tests/host_sign_0500.py` übersetzt den Logikblock der Tafelverwaltung (Marken `Tafelverwaltung (Logik)` bis `(Ende Logik)` im Sketch) mit g++ gegen Attrappen der BLE-Klassen (`tests/host/scan_stub_0500.h`, `sign_main_0500.cpp`): Adresse prüfen und normalisieren, Namen säubern, Treffer sammeln (Name, Dienst, eigene Tafel ohne Namen, Dubletten, ungültige Adresse), Reihenfolge und Filter der JSON-Antwort, Überlauf bei zwölf Einträgen. Im Browser-Test: Mock-ESP mit `/api/sign*`, Einstellungen (Suche, „Alle zeigen“, Signalsymbole je Stärke, Speichern, Ersetzen mit zweitem Tipp, fremde Tafel, Zurück ohne Suche, Kopfzeilen-Chip), Zoom-Sperre (Viewport, `touch-action`, unterdrückte Spreizgeste, 16-px-Felder), Palettentaste in der Farbzeile und Programme/Bibliothek mit gefüllten Listen ohne Seitenscrollen. Als Testadresse dient `c0:ff:ee:00:00:01`; keine echte Tafeladresse mehr in den Tests. Die Statusabfrage-Prüfung toleriert einen zusätzlichen Takt (war zeitabhängig).

## v0.4.6.0-dev (3.11.2026)

Die Tests tragen jetzt die Endung `0460`. Neu: Seiten-Parität in `test_render_parity_0460.py` (Firmware-Modus `page` am Rechner gegen `drawPage` im Browser, 15 Fälle, plus Ablehnung zu breiter Seiten), Host-Modus `pagecmp` in `tests/host/drv.h` (Seiten ohne Marken: alter Weg gleich Lauftext-Maler), im Browser-Test das Zeichen-Fenster für Blockseiten und das Bluetooth-Zeichen bei einer Übertragung (Mock liefert HTTP 202 über `__mock.force202`; das Schweigen des ESP wird über die Wartezeit nachgestellt), in `test_build_0460.py` Invarianten zu den neuen Hilfen. Die Scan-Attrappen heißen jetzt `scan_stub_0460.h`/`scan_main_0460.cpp`.

## v0.4.5.1-dev (3.11.2026)

Die Tests tragen jetzt die Endung `0451`. Neu: `python tests/host_scan_0451.py` übersetzt den Vor-Scan-Block des Sketches mit g++ gegen Attrappen der BLE-Klassen (`tests/host/scan_stub_0451.h`, `scan_main_0451.cpp`) und prüft Treffer (Abbruch, Signalstärke), nur fremde Geräte (volle Zeit, „nicht gesehen“) und nichts gehört („ohne Aussage“). Das ist eine Prüfung der Entscheidungslogik, keine der echten Bibliothek. `test_build_0451.py`: Invarianten zum Vor-Scan (Aufruf nur einmal und nur vor neuem Verbindungsaufbau, vor dem Zählen der Versuche, nur „nicht gesehen“ bricht ab), Statusfelder und Zeichen im Kopf. `test_browser_0451.py`: Bluetooth-Zeichen in den Zuständen idle, ok, bad, link und busy.

## v0.4.5.0-dev (2.11.2026)

Die vier Tests tragen jetzt die Endung `0450`. Neu im Browser-Test: echte Touch-Ereignisse über das Chrome-Protokoll (`Input.dispatchTouchEvent`) für waagerechtes Markieren und senkrechtes Scrollen im Zeichen-Fenster, Doppeltipp-Wort (mit/ohne Satzzeichen), feste Kopfzone und deren Schrumpfen, Farbleiste (erscheint/verschwindet, wendet Farbe an), schrumpfende Vorschau im Editor (mit künstlich verlängerter Seite), Kurzwahl-Dialog (Quellen, Sperre des Speicherns, vorbelegte Beschriftung, leer gespeichert wenn unverändert), Auswahl aufheben in Bibliothek und Programmen, Knöpfe in Einstellungen und Diagnose, gleichmäßige Abstände im Startbildschirm-Bedienfeld. Der Build-Test prüft `RB_AP_MAX_CLIENTS` = 2. Nicht abgedeckt: das echte Verhalten auf iOS-Safari (Touch-Gesten, Doppeltipp-Zoom) und der ESP32 selbst.

## v0.4.4.2-dev (2.11.2026)

Die vier Tests des vorherigen Stands tragen jetzt die Endung `0442` (`test_build_0442.py`, `test_browser_0442.py`, `test_render_parity_0442.py`, `host_program_order_0442.py`). Neu im Browser-Test: der Mock-ESP verzögert Sortierbefehle absichtlich (700 ms), liefert Kopien statt seines Speichers und kann Sortierfehler melden; geprüft werden sofortige Anzeige, Reihenfolge und Anzahl der Befehle, keine Neuabfrage nach Erfolg, Rückfall bei Fehler, kein erneutes Laden frischer Listen beim Seitenwechsel (veraltete werden im Hintergrund geholt), pausierte Statusabfrage und das reduzierte Startbildschirm-Bedienfeld (Tasten, Reihenfolge, Abstände, eine Zeile; übrige Seiten zeigen alle acht Tasten). Nicht abgedeckt: reale Zeiten auf dem ESP32, iPhone-Safari.

## v0.4.4.1-dev (2.11.2026)

`python tests/test_build_0441.py`: wie `test_build_0440.py` (jetzt umbenannt), zusätzlich Invarianten zur Programm-Reihenfolge (Route, Datei, Funktionsreihenfolge im Header), zu Textfeldern und Verlauf, zum Startbildschirm und zu den entfallenen Elementen. `python tests/test_browser_0441.py [Screenshot-Ordner]`: ersetzt `test_browser_0440.py`; Playwright-Test Ende-zu-Ende, neu: Wischen-Entmarkieren, antippbare Schriftkästchen, mehrstufiges Rückgängig/Wiederherstellen (inkl. „Text leeren“ und Stile), Eingabefeld (Wachsen, Enter, Ziehgriff, Merken, Zeile 2), Programme sortieren (Pfeilzustände, Anfragen), Startbildschirm mit Erzwingen-Tasten. `python tests/test_render_parity_0441.py`: Zeichenfunktionen aus dem Sketch im Host-Build gegen Browser-Vorschau (unverändert gegenüber 0.4.4.0). `python tests/host_program_order_0441.py`: echter Firmware-Code der Programm-Reihenfolge (`rb44ProgramOrder`, `v044MoveCollection`) mit Stubs für String, LittleFS und Webserver (Standardreihenfolge, Verschieben, ungültige Anfragen, neu/gelöscht/`.bak`, beschädigte Datei, Schreibfehler, 24 Programme). Benötigt Playwright, Chromium und g++. Ersetzt **keinen Arduino-Gesamtcompile** und keinen Test auf dem ESP32. Ältere Testdateien beziehen sich auf ihre alten Ordner.

## v0.4.4.0-dev (31.10.2026)

`python tests/test_build_0440.py` (ab 0.4.4.1 umbenannt in `test_build_0441.py`): wie `test_build_0434.py`, zusätzlich Invarianten zum Stilformat (Marken U+0090..U+009C in Firmware und Weboberfläche, Programm-Verkettung, `decodeText`, Bibliotheksgrenze). `python tests/test_browser_0440.py [Screenshot-Ordner]`: Playwright-Test des Editors Ende-zu-Ende (Einzeln, Wischen, Farbe, Schrift, Farbwechsel, Herzen, Rückgängig, Senden-Nutzlast, Bibliothek hin und zurück, alte Einträge, zweizeilig, Entwurf, Längenhinweis, Mobilgrößen 375/390/430 px). `python tests/test_render_parity_0440.py`: baut die echten Zeichenfunktionen des Sketches mit Stubs als Host-Programm (`tests/host/build.py`), vergleicht Klartext mit der Ausgabe von 0.4.3.4 (`golden_plain_0434.txt`), prüft Stilfälle und vergleicht Firmware und Browser pixelweise. Benötigt Playwright, Chromium und g++. Ersetzt **keinen Arduino-Gesamtcompile** und keinen Test auf dem ESP32. Ältere Testdateien beziehen sich auf ihre alten Ordner.

## v0.4.3.4-dev (31.10.2026)

`python tests/test_build_0434.py`: wie `test_build_0433.py`, zusätzlich Invarianten zum Abmelden vor dem Wächter-Neustart (`esp_wifi_deauth_sta(0)` vor `abort()`, Spur vor Abmeldung, Flag in Spurtext), zum Test-Auslöser (Route, Blockade in `loop()`, Parameter `deauth`) und dazu, dass die Weboberfläche keinen Knopf dafür enthält. `python tests/test_0434.py [Screenshot-Ordner]`: Browser-Test der Programme-Ansicht (unverändert, nur Version). Der Wächter-/Abmeldecode wurde zusätzlich in einem einmaligen Host-Stubtest (g++) auf Ablauf geprüft; dieser Test liegt nicht im Repository. Kein Ersatz für Arduino-Compile und Hardwaretest.

## v0.4.3.3-dev (30.10.2026)

`python tests/test_build_0433.py`: Version, gzip/ETag, JavaScript-Syntax, Diagnose-Verträge, Mobilgrößen und Firmware-Invarianten zur RAM-Messung (Sketch-Größe nur ein Aufruf, `bleEnsureInit`, Standardwerte der Schalter, `softAP` mit Client-Limit, `[RAM]`-Marken, Export-Abschnitte) und zum Wächter (Wächter-Schwelle, RTC-Spur, Phasenmarken im BLE-Pfad, Anzeige des Vorlaufs). `python tests/test_0433.py [Screenshot-Ordner]`: Browser-Test der Programme-Ansicht (unverändert gegenüber 0.4.3.2, nur Version). Kein Ersatz für Arduino-Compile und Hardwaretest.

## v0.4.3.2-dev (30.10.2026)

`python tests/test_0432.py [Screenshot-Ordner]`: Browser-Test der Programme-Ansicht gegen eine simulierte API auf 360/390/430 px (Liste, Auswahl, Bearbeiten, Abbrechen/Verwerfen, Speichern, Neu, Löschen, defektes Programm, Werkzeugleiste einzeilig, kein horizontaler Überlauf). Benötigt Playwright und Chromium.

`python tests/test_build_0432.py`: Version, gzip/ETag, JavaScript-Syntax, Diagnose-Verträge und Mobilgrößen (Nachfolger von `test_0431.py`).

Hinweis: `tests/test_browser_save_as_4254.py` schlägt bereits gegen v0.4.3.1 fehl (Speichern-Auswahl wurde in v0.4.3 umgebaut) und ist nicht angepasst. Weiterhin gilt: kein Arduino-Compile, kein Hardware- oder Heap-Test.
