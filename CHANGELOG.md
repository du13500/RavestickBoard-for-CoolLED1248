> **v0.4.3.1-dev · Korrektur 1 (30.10.2026):** Arduino-Compile-Fehler durch automatischen Prototyp fuer `rbAppendJsonText(RbDiagnosticStream&)` korrigiert. Die JSON-Escaping-Funktion ist nun eine Klassenmethode. Vollstaendiger Arduino-Compile auf Zielumgebung steht noch aus.

# RavestickBoard · Versionsgeschichte / Changelog

## v0.5.0.0-dev – Tafelverwaltung, Oberflächen-Korrekturen (3.11.2026)

- **Anlass:** Test von 0.4.6.0 („funktioniert wie gewünscht“) und die abgesprochene Tafelverwaltung. Rückmeldungen: Die Palettentaste der Blockanzeige steht unter den Farben statt in derselben Zeile; Zoom per Doppeltipp auf dem iPhone soll global gesperrt werden; die Programme-Ansicht ist zu lang (Scrollen bis zum Bedienfeld nötig).
- **Auswertung der Protokolle von 0.4.6.0:** (1) Ein echter Verbindungshänger: Nach „Verbindungsversuch zu CoolLEDX“ (Vor-Scan: Tafel nach 54 ms gesehen, −80 dBm, 170,4 s nach dem Start) kehrte `connect()` nicht zurück; der Wächter hat nach 25 s mit `abort()` neu gestartet (Spur `ble_connect`, Auftrag 1, 45 KB frei, nicht speicherbedingt). Das ist der seltene ursprüngliche Hänger, nicht der Fall „Tafel aus“. Nach dem Neustart war das WLAN-Gerät nach 3,6 s wieder angemeldet, alle folgenden acht Aufträge liefen fehlerfrei (Vor-Scan 15 bis 144 ms, −78 bis −89 dBm). Der Vor-Scan verhindert diesen Hänger nicht. (2) Beim Beitritt des zweiten WLAN-Geräts (287,6 s) fiel das System-Minimum von 21420 auf 9540 B, etwa eine Sekunde nach „IP vergeben“, ohne Folgen; ein Gerät ging später wieder. (3) Der Reset-Grund „Ausnahmefehler (Panic)“ im Diagnose-Export stammt vom Wächter (`abort()`), nicht von einem Absturz im Betrieb.
- **Firmware:** Die feste Tafeladresse (`SIGN_ADDRESS`) ist entfernt, auch aus dem Kopfkommentar. Neu: „Meine Tafel“ im NVS (Namespace `rbsign`, Schlüssel `addr`, `name`), aktive Tafel im RAM (`signActive`, `signKind` 0 keine/1 Meine/2 Fremde). Vor-Scan und Verbindungsaufbau nutzen die aktive Adresse; ohne Tafel meldet ein Auftrag „Keine Tafel eingerichtet“ und sendet nichts. Neue Schnittstellen: `GET /api/sign`, `POST /api/sign/scan` (blockiert etwa 4 s, aktiver Scan, bis zu zwölf Treffer, Parameter `all=1` blendet Geräte ohne Tafelnamen ein), `POST /api/sign/save` (Adresse und Name, ersetzt Meine Tafel, macht sie aktiv), `/api/sign/use` (vorübergehend, nur RAM), `/api/sign/back`. Die Statusantwort enthält `signKind` und `signName`. Die Adresse steht nie in der Diagnose; sie erscheint nur in der Suchantwort an die Weboberfläche. Wechsel und Suche werden nur abgewiesen, wenn ein Auftrag läuft (409); eine bestehende BLE-Verbindung wird vor dem Wechsel getrennt.
- **Weboberfläche:** Einstellungen, Abschnitt LED-Tafel: Anzeige der aktiven Tafel, „Tafel suchen“, Trefferliste mit Signalsymbol, Name, letzten zwei Adressbytes und dBm, Schaltflächen „Als meine Tafel speichern“ (beim Ersetzen zweiter Tipp nötig) und „Jetzt nutzen“, „Zurück zu meiner Tafel“, Schalter „Alle Bluetooth-Geräte zeigen“. Kopfzeile: Chip „Meine Tafel“/„Fremde Tafel“/„Keine Tafel“ (führt zu den Einstellungen; bei älterer Firmware ohne `signKind` unsichtbar). Sechs neue Lucide-Symbole (`antenna`, `signal`, `signal-high`, `signal-medium`, `signal-low`, `signal-zero`). Palettentaste der Blockanzeige in der Farbzeile (acht Spalten wie beim Lauftext). Zoom: Viewport `maximum-scale=1, user-scalable=no`, `touch-action: manipulation` für alles (gezielte Regeln wie `pan-y` bleiben), `gesturestart/-change/-end` unterdrückt, Felder mit mindestens 16 px. Höhe der Listen von Programmen und Bibliothek: `100dvh − 255px` (vorher 200 px; mit dem Bedienfeld unten wurde die Seite 45 px zu hoch und scrollte).
- **Abweichungen/Entscheidungen:** (1) Nach dem Update ist keine Tafel eingerichtet, auch wenn bisher die feste Adresse benutzt wurde; Meine Tafel muss einmal neu gespeichert werden (so abgesprochen, die Adresse soll nicht mehr mitgeliefert werden). (2) Welche Geräte als „Tafel“ gelten (Name enthält „cool“, Dienst FFF0 oder gespeicherte Tafel), ist eine Annahme, weil unbekannt ist, ob und wie jede Tafel ihren Namen sendet; deshalb der Schalter „Alle Bluetooth-Geräte zeigen“. (3) Der Adresstyp wird nicht gespeichert; die Verbindung läuft wie bisher mit unbekanntem Typ (0xFF). Das ist für die eigene Tafel bewährt, bei fremden Tafeln mit öffentlicher Adresse ungetestet. (4) Während der Suche (etwa 4 s) antwortet die Oberfläche nicht, wie bei jedem BLE-Auftrag; die Statusabfrage pausiert so lange. (5) Eine Tafel mit anderer Pixelgröße als 64 × 16 wird nicht unterschieden; die Darstellung bleibt 64 × 16. (6) Die gespeicherte Tafel überlebt Neustart und erneutes Flashen, solange der Flash nicht komplett gelöscht wird („Erase All Flash“ bleibt aus). (7) Ersetzen braucht einen zweiten Tipp statt eines Bestätigungsfensters; Löschen gibt es nicht. (8) Rot wird dafür nicht verwendet.
- **Geprüft (ohne Arduino/Hardware):** `tests/test_build_0500.py` (u. a.: keine Tafeladresse mehr in Firmware und Weboberfläche, Routen, `SignRef` vor den automatischen Prototypen), `tests/test_browser_0500.py` (Einstellungen mit simuliertem ESP: Suche, Filter, Signalsymbole je Stärke, Speichern, Ersetzen mit zweitem Tipp, fremde Tafel, Zurück ohne neue Suche, Kopfzeile; Zoom-Sperre; Palettentaste; Programme und Bibliothek mit gefüllten Listen scrollen nicht), `tests/host_sign_0500.py` (echter Sketch-Code gegen BLE-Attrappen: Adresse und Name prüfen, Treffer sammeln, Reihenfolge, Überlauf), `tests/host_scan_0500.py`, `tests/test_render_parity_0500.py`, `tests/host_program_order_0500.py`. Der Test der Statusabfrage (`poll()` nach `rbBusy`) war vom Takt abhängig und erlaubt jetzt einen zusätzlichen Takt. **Nicht geprüft:** Arduino-Compile der neuen Aufrufe, echte Suche und echter Wechsel an der Hardware, iPhone.
- **Bekannt/offen:** Der Verbindungshänger bei sichtbarer Tafel bleibt selten möglich; der Wächter behebt ihn nach 25 s. Mögliche Maßnahmen später: Verbindungsaufbau in einem eigenen Task mit Zeitüberwachung, NimBLE. `RB_TEST_HANG` bleibt in diesem Entwicklungsstand 1.

## v0.4.6.0-dev – Zeichen-Editor für Blockseiten, Bluetooth-Zeichen bei Übertragung (3.11.2026)

- **Anlass:** Die Reihenfolge war nach 0.4.5.0 so abgesprochen; der Hardwaretest von 0.4.5.1 lief laut Rückmeldung gut („Der Test lief super!“). Dazu die Frage, warum das Bluetooth-Zeichen bei einer Übertragung (Prozentanzeige) nicht „verbunden“ zeigt.
- **Antwort auf die Beobachtung:** Das war so gebaut, nicht übersehen. Während einer Übertragung blockiert der Auftrag die Schleife des Webservers, die Statusabfragen kommen erst danach zurück; angezeigt blieb deshalb der alte Zustand bzw. „sucht“.
- **Firmware:** `renderBlockPage` versteht die Stilmarken (neue Hilfen `rbLineHasMarks`, `rbPageLineWidth`, `rbPageLineChars`, `rbPageDrawLine`). Zeilen ohne Marken laufen über den bisherigen Weg (`decodeText`/`drawInline`), Zeilen mit Marken über den Maler des Lauftexts (`measureText`/`drawInlineUtf8`). Schriftmarken werden bei Mini zweizeilig ignoriert, Mini zeigt nur Großbuchstaben, die Oberkante folgt der Schrift der Marke. Die Breitenprüfung (`onProgramSet`, `renderBlockPage`, Kachel-Renderer `rb42RenderPageTile`) misst mit der Darstellungsbreite statt mit der Byte-Länge. Das Seitenformat (`PageRecord`, RBB2) bleibt unverändert; die Marken liegen im Text und belegen je 2 der 47 Bytes pro Zeile.
- **Weboberfläche:** Dasselbe Fenster „Zeichen gestalten“ für Blockseiten (Palettentaste unter „Farbe · Zeile 1“). Das Stilmodell ist je Bereich getrennt (`RBSX.scroll`/`RBSX.page`, `RBS.a/b` zeigen auf den sichtbaren Bereich), ebenso der Verlauf (`rbHX`). Seiten werden mit Marken gespeichert (`rbPageStore`) und beim Laden zerlegt (`rbPageLoad`); das Eingabefeld zeigt nur den Text. Vorschau, Seitenleiste und Namensvorschlag zeichnen bzw. zeigen die Marken passend (Zeichnen mit Marken über den Maler des Lauftexts, ohne Marken über den bisherigen Weg). „Senden“ im Fenster zeigt die Seite (`pageShow`). Die Prüfung `validatePage` zählt Breite aus der Darstellung und Bytes samt Marken.
- **Bluetooth-Zeichen:** Die Seite merkt sich den Zeitpunkt, an dem ein Auftrag angenommen wurde (HTTP 202). 0 bis 3,5 s: `bluetooth-searching`; danach `bluetooth-connected` („Übertragung läuft“), weil der Vor-Scan höchstens 3 s dauert und der ESP dann nur noch schweigt, wenn er überträgt. Die erste Antwort danach (`idle`/`error`) liefert den echten Zustand; Sicherheitsablauf nach 180 s. Das ist ein Schluss aus dem Schweigen, keine Messung des Verbindungszustands. Schlägt die Verbindung nach dem Vor-Scan fehl (Connect-Timeout bis 10 s), wird kurz „verbunden“ gezeigt, bevor „nicht erreicht“ erscheint.
- **Abweichungen/Entscheidungen:** (1) Wird die Zeilenfarbe der Seite gewählt, werden die Einzelfarben dieser Zeile zurückgesetzt (wie bei „Farbe für alles“ im Lauftext); rückgängig machbar. (2) Ein Wechsel des Layouts löscht die Schriftmarken der Seite (rückgängig machbar), weil sich die Schrift sonst unbemerkt ändert. (3) Rückgängig/Wiederherstellen gibt es nur im Fenster und je Seitenauswahl; beim Wechsel der Seite beginnt der Verlauf neu. (4) Das Fenster arbeitet auf Zeile 1 und bei Mini zweizeilig auch auf Zeile 2; die Palettentaste für Zeile 2 gibt es nicht gesondert. (5) Die Anzeige „verbunden“ während der Übertragung gilt auch für Aufträge, bei denen die Tafel nachher als nicht erreicht gemeldet wird (siehe oben).
- **Geprüft (ohne Arduino/Hardware):** `tests/test_build_0460.py` (Invarianten), `tests/test_browser_0460.py` (u. a. Zeichen-Fenster für Seiten: Markieren, Farbe, Rückgängig/Wiederherstellen, Rundlauf Seite→Eingabefeld, Prüfung; Bluetooth-Zeichen bei simulierter Übertragung mit HTTP 202), `tests/test_render_parity_0460.py` (Seiten: Firmware-Maler `page` am Rechner gegen `drawPage` im Browser, 15 Fälle über alle Schriften, Ausrichtungen und Zweizeilig; zu breite Seiten werden abgelehnt), host-Modus `pagecmp` (Seiten ohne Marken: alter Weg gleich Lauftext-Maler, 378 Fälle), `tests/host_program_order_0460.py`, `tests/host_scan_0460.py`. **Nicht geprüft:** Arduino-Compile, Hardware, Übertragung gestylter Seiten an die echte Tafel, Speichern/Laden gestylter Blockfolgen in der Bibliothek (die Marken liegen im Zeilentext, das Bibliotheksformat ist unverändert), Verhalten auf dem iPhone.
- **Bekannt/offen:** Bei Zeilen mit vielen Wechseln kann das 47-Byte-Limit vor der 64-Pixel-Grenze erreichen (Beispiel: 16 Zeichen mit je einer Farbmarke = 48 Bytes). `RB_TEST_HANG` bleibt in diesem Entwicklungsstand 1. Als Nächstes: 0.5.0.0 Tafelverwaltung.

## v0.4.5.1-dev – Vor-Scan der Tafel, Bluetooth-Zeichen, genauere Heap-Zuordnung (3.11.2026)

- **Anlass:** Serial-Monitor, HTTP-, RAM- und Diagnose-Export zum Test von 0.4.5.0. Beim Senden an die ausgeschaltete Tafel startete der Hang-Wächter neu.
- **Auswertung:** Das Log endet nach `[BLE] Verbindungsversuch zu CoolLEDX` ohne weitere Zeile (auch keine `/api/status`-Abfragen, der Webserver stand), danach `abort()` durch den Wächter (Spur `ble_connect`, Auftrag 1). Der Connect-Timeout von 10 s hat nicht gegriffen. Beim Hänger waren 42 KB Heap frei, größter Block 28 KB: Speicher ist hier nicht die Ursache. Das System-Minimum von 2844 B stammt nicht vom Hänger, sondern vom ersten Seitenaufruf nach dem Flashen mit nur einem Gerät (zwischen `/api/font` und `/api/program`, Minimum davor 25148 B, danach wieder rund 47 KB frei). Vermutung, im Log nicht beweisbar: die vollständige Seite (200 statt 304 nach neuem Flashen, rund 66 KB gzip) zusammen mit den parallelen Verbindungen des Browsers. Mit gecachter Seite und zwei Geräten blieb das Minimum bei 13848 B. Nach dem Wächter-Neustart waren beide Geräte binnen etwa 6 s wieder im WLAN.
- **Firmware:** Vor-Scan (`bleScanForSign()`): vor jedem neuen Verbindungsaufbau passiver Scan, Fenster 99 von 100, höchstens `RB_PRESCAN_S` = 3 s, `stop()` beim Treffer der Tafeladresse. Ergebnis 1 = gesehen, 0 = nicht gesehen, aber andere Geräte gehört (kein Verbindungsversuch, Meldung „LED-Tafel nicht gefunden“, Zustand „nicht erreicht“), −1 = nichts gehört (ohne Aussage, es wird wie bisher verbunden; das fängt auch eine unpassende Rückruf-Signatur der Bibliothek ab). Nicht bei bestehender Verbindung und nicht bei der MTU-Neuverbindung. Fehlscans zählen nicht als Verbindungsversuche. `RB_PRESCAN 0` schaltet alles ab. Neue Wächter-Phase `ble_scan`. `/api/status` liefert zusätzlich `bleLink` und `bleLast` (0 unbekannt, 1 erreicht, 2 nicht erreicht). Diagnose-Export: Vor-Scan-Zähler, letzte Dauer und Signalstärke. HTTP-Export: Heap-Tiefstand außerhalb von Anfragen mit zuletzt beendeter Anfrage, Abstand und Minimum davor.
- **Weboberfläche:** kleines Bluetooth-Zeichen hinter „for CoolLEDX · ESP32-C3“ (`bluetooth`, `bluetooth-searching` mit Pulsieren, `bluetooth-connected`, `bluetooth-off`), mit Hinweistext. Die Seite wird alle 1,7 s abgefragt; während einer Übertragung antwortet der ESP nicht.
- **Absprache für später (nicht in diesem Stand):** „Meine Tafel“ wird im ESP gespeichert statt fest im Code, ohne mitgelieferte Adresse; Tafel suchen, vorübergehend eine fremde Tafel nutzen und per Knopf zurück zu „Meine Tafel“; nach einem Neustart ist wieder „Meine Tafel“ aktiv. Für die Signalstärke in der Trefferliste sind die Lucide-Icons `antenna`, `signal-zero`, `signal-low`, `signal-medium`, `signal-high` und `signal` vorgesehen. Die Adresse steht in diesem Stand noch fest im Code (im Quelltext als Übergang vermerkt).
- **Abweichungen von der Absprache:** Das Bluetooth-Zeichen im Kopf war nicht ausdrücklich verlangt, sondern aus dem Hinweis auf die Lucide-Bluetooth-Icons abgeleitet; es lässt sich leicht entfernen. Die Pfade der drei neuen Icons (`bluetooth-connected`, `-off`, `-searching`) sind aus dem Gedächtnis eingetragen, weil das Lucide-Paket hier nicht abrufbar war; bitte im Browser prüfen, ob sie wie die Originale aussehen.
- **Geprüft (ohne Arduino/Hardware):** `tests/test_build_0451.py` (neue Invarianten zum Vor-Scan), `tests/test_browser_0451.py` (u. a. Bluetooth-Zeichen in allen fünf Zuständen), `tests/host_scan_0451.py` (Entscheidungslogik mit dem echten Sketch-Code gegen Attrappen: Treffer nach 900 ms, nur fremde Geräte nach 3000 ms, nichts gehört), `tests/test_render_parity_0451.py`, `tests/host_program_order_0451.py`. **Nicht geprüft:** Arduino-Compile; ob die BLE-Klassen des Core 3.3.x die angenommenen Signaturen haben (`onResult(BLEAdvertisedDevice)`, `start(Sekunden)`, `clearResults()`); ob die Tafel im Scan mit ihrer Adresse auftaucht; die Scan-Dauer auf der Hardware.
- **Bekannt/offen:** Ist der Scan wegen unpassender Signatur wirkungslos, meldet die Diagnose „ohne Aussage“ und es ändert sich nichts gegenüber 0.4.5.0. Der Hänger bei ausgeschalteter Tafel in einer Umgebung ganz ohne andere Bluetooth-Geräte bleibt dann möglich. Hardwaretest: Tafel aus, 2–3 Mal senden, dann mit Tafel an. `RB_TEST_HANG` bleibt in diesem Entwicklungsstand 1.

## v0.4.5.0-dev – Zeichen-Fenster bedienbarer, Kurzwahl-Dialog, höchstens zwei WLAN-Geräte (2.11.2026)

- **Anlass:** Rückmeldungen nach dem Test von 0.4.4.2 („läuft viel sauberer“) und eine Sammlung von Bedienwünschen. Der Zeichen-Editor für Blockseiten, bisher als 0.4.5.0 geplant, wandert nach 0.4.6.0.
- **Firmware:** `RB_AP_MAX_CLIENTS` Standard 2 statt 4 (Festlegung: höchstens zwei WLAN-Geräte, ein zweites ist die Ausnahme). Grund: In 0.4.4.2 fiel das System-Minimum beim Beitritt des dritten Geräts auf 3288 B (12016 → 7720 → 3288 innerhalb von etwa 6 s). Die BLE-Verbindung zur Tafel zählt nicht mit. Sonst keine Firmwareänderung (Version und Webseite).
- **Zeichen-Fenster (Lauftext):** Umschalter „Einzeln/Wischen“ entfällt. Waagerechtes Wischen markiert (auf Markiertem entmarkiert, wie bisher), senkrechtes scrollt (`touch-action: pan-y`, Richtung nach 8 px Bewegung; mit der Maus markiert jede Richtung). Doppeltipp auf dasselbe Zeichen innerhalb 400 ms überträgt den Zustand des ersten Tipps auf das ganze Wort (Leerzeichen-getrennt, Satzzeichen am Rand außen vor, nur Satzzeichen: ganzes Wort). Kopfzone (Titel, Vorschau, Zähler, Alle/Keine/Rückgängig/Wiederherstellen) steht oben fest und wird ab 60 px Scrollen auf 62 % der Breite kleiner. Schmale Farbleiste unten, solange der große Farbblock nicht sichtbar ist (IntersectionObserver); im Farbwechsel keine Leiste. Der große Farbblock hat jetzt größere Felder (bis 44 px).
- **Vorschau im Editor (Lauftext und Blockanzeige):** wird beim Scrollen auf 62 % der Breite kleiner, Beschriftung und Vorschau-Werkzeuge werden dann ausgeblendet. Das Schrumpfen setzt nur ein, wenn die Seite danach noch genug zum Scrollen hat (kein Selbstaufheben des Scrollens).
- **Kurzwahl-Dialog:** Quelle als Schaltflächen mit Menü-Icons (Bibliothek `library`, Programme `list-video`, Ambiente `spray-can`; Animationen `play` ausgegraut); die Inhaltsliste erscheint erst nach der Quellenwahl; „Speichern“ bleibt gesperrt, bis ein Inhalt gewählt ist. Beschriftung wird mit dem Namen des Inhalts vorbelegt und leer gespeichert, wenn sie unverändert bleibt (die Kachel folgt dann weiter dem Namen).
- **Kleinigkeiten:** Startbildschirm-Bedienfeld: fünf Tasten gleichmäßig nebeneinander (an, aus, Linkslauf, Standbild, Bearbeiten). Einstellungen: „Backup zusammenführen“ in der Schrift von „Backup exportieren“, Icon `database-arrow-up`; Überschriften ohne vorangestelltes Zeichen (auch „LED-Tafel“, über die Anfrage hinaus). Diagnose: „TXT herunterladen“ und „RAM-Verlauf TXT“ in der Designlinie mit `database-arrow-down`. Zweiter Tipp auf einen gewählten Eintrag in Bibliothek und Programmen hebt die Auswahl auf (die Vorauswahl des ersten Eintrags beim Öffnen bleibt; eine bewusst aufgehobene Auswahl bleibt aufgehoben).
- **Abweichungen von der Absprache:** Das Icon für „Animationen“ ist `play` (kein Menü-Icon vorhanden). Der Umschalter „Einzeln/Wischen“ ist ersatzlos entfallen, statt ihn zu behalten.
- **Geprüft (ohne Arduino/Hardware):** `tests/test_browser_0450.py` (u. a. echte Touch-Ereignisse über das Chrome-Protokoll: waagerecht markiert, senkrecht scrollt, Doppeltipp-Wort, feste Kopfzone, Farbleiste, schrumpfende Vorschau, Kurzwahl-Dialog, Auswahl aufheben, Knöpfe in Einstellungen/Diagnose), `tests/test_build_0450.py`, `tests/test_render_parity_0450.py`, `tests/host_program_order_0450.py`.
- **Bekannt/offen:** Gesten und Doppeltipp ohne Zoom sind nicht auf dem iPhone geprüft. Die Bibliotheksliste braucht weiter ca. 0,9 s auf dem ESP32. `RB_TEST_HANG` bleibt in diesem Entwicklungsstand 1.

## v0.4.4.2-dev – Schlankes Startbildschirm-Bedienfeld, Sortieren sofort, weniger Last auf dem ESP32 (2.11.2026)

- **Anlass:** Rückmeldung aus dem Hardware-/Weboberflächentest von 0.4.4.1: Das Bedienfeld unten im Startbildschirm war mit allen Tasten übervoll; die Oberfläche wirkte träge (Sortieren nicht sofort, Tasten ohne sichtbare Wirkung, „Webinterface neu laden“ brauchte rund 10 s bis zum Ladebildschirm); eine Übertragung an die Tafel endete mit Fehler.
- **Startbildschirm:** nur noch *an*, *aus*, Linkslauf (Chevron links), Standbild/Blocktext (Achteck) und *Bearbeiten* (Stift), mit Abstand zwischen den drei Gruppen. Reihenfolge an/aus vertauscht (zuvor aus/an). Alle anderen Seiten behalten die volle Steuerung mit acht Erzwingen-Tasten.
- **Sortieren sofort (Bibliothek und Programme, Pfeile und Ziehen):** Die Liste wird lokal sofort umgeordnet, die Befehle `/api/library/move` bzw. `/api/collections/move` laufen der Reihe nach im Hintergrund. Keine Neuabfrage nach Erfolg. Schlägt ein Befehl fehl, erscheint eine Meldung und die Liste wird vom ESP32 neu geholt. Der verschobene Eintrag leuchtet kurz auf.
- **Weniger Anfragen an den ESP32 (der Arduino-Webserver bedient nur eine Anfrage zugleich):** (a) Seitenwechsel in Bibliothek/Programme zeigen die Liste aus dem Speicher und holen sie nur neu, wenn sie älter als 60 s ist; Änderungen im Editor, Import usw. holen wie bisher sofort neu. (b) Beim Start erscheint die Oberfläche nach Schrift und Seiten, Kurzwahlen, Programme und Bibliothek laden danach im Hintergrund. (c) Die Statusabfrage (alle 1,7 s) pausiert, solange eine andere Anfrage läuft (höchstens 6 s) oder der Tab im Hintergrund liegt.
- **Firmware:** unverändert gegenüber 0.4.4.1 (nur Versionsstring und neue Webseite im Header). Die Bibliotheksliste benötigt weiterhin rund 0,9 s, weil jeder Eintrag vollständig gelesen und geprüft wird (`onLibraryList`); eine Beschleunigung ist bewusst nicht Teil dieses Standes.
- **Auswertung des Logs von 0.4.4.1 (Serial, RAM, HTTP, Diagnose):** Der Hänger der Übertragung ist der bekannte BLE-Hänger (Phase `ble_regNotify`, Auftrag 3, System-Minimum 7320 B), der Wächter hat nach 25 s neu gestartet, die Geräte wurden vorher abgemeldet, das Handy war rund 1 s nach dem Neustart wieder verbunden (ein einzelner Messpunkt, kein Vergleich ohne Abmelden). Keine Behebung in diesem Stand.
- **Geprüft (ohne Arduino/Hardware):** `tests/test_browser_0442.py` (Sortieren sofort bei absichtlich langsamem Mock-ESP, Reihenfolge der Befehle, keine Neuabfrage, Fehlerfall mit Rückfall auf den ESP-Stand, keine doppelten Listenabrufe, Statusabfrage pausiert, Startbildschirm: Tasten, Reihenfolge, Abstände, eine Zeile; übrige Seiten voll), `tests/test_build_0442.py`, `tests/test_render_parity_0442.py` und `tests/host_program_order_0442.py` unverändert grün. Beim Schreiben des Fehlerfall-Tests wurde ein Selbstblockieren der Nachladung nach fehlgeschlagenem Sortieren gefunden und behoben.
- **Bekannt/offen:** Sortier- und Ladezeiten auf der echten Hardware sind nicht gemessen. Sendet man direkt nach dem Sortieren, laufen die Sortierbefehle zuerst. `RB_TEST_HANG` bleibt in diesem Entwicklungsstand 1.

## v0.4.4.1-dev – Feinschliff Zeichen-Editor, Programme sortieren, größeres Eingabefeld (2.11.2026)

- **Anlass:** Rückmeldungen aus dem Weboberflächen-Test von 0.4.4.0: Wischen konnte nur markieren, der große Schriftschalter war neben den vier Schriftkästchen überflüssig, „Stil rückgängig“ passte nicht zur Bedienung, Programme ließen sich nicht sortieren, das einzeilige Eingabefeld war bei langen Texten auf dem iPhone mühsam, und der Startbildschirm zeigte nur Regler statt der gemeinsamen Tafelsteuerung.
- **Zeichen-Fenster:** Wischen auf einem bereits markierten Zeichen **entmarkiert** (der erste berührte Kasten bestimmt den Modus). Der große Schriftschalter entfällt; die vier Kästchen Normal/Fett/Engschrift/Mini setzen die Schrift direkt und leuchten nur, wenn alle gewählten Zeichen sie tragen. Neben Rückgängig gibt es im Fenster auch Wiederherstellen.
- **Rückgängig/Wiederherstellen (nur Lauftext-Editor):** Zwei reine Icon-Knöpfe neben „Text leeren“ ersetzen „Stil rückgängig“. Ein Schritt umfasst Text beider Zeilen, Zeichenstile und Zeilenfarben; bis zu 60 Schritte. Tippen wird nach einer kurzen Pause (ca. 0,6 s) zu einem Schritt zusammengefasst, Aktionen im Fenster, globale Farbe und „Text leeren“ sind eigene Schritte. Das Öffnen eines Eintrags und „Neu“ beginnen eine frische Historie. Die Schriftart (Layout) gehört nicht zum Verlauf.
- **Eingabefeld (nur Lauftext, Zeile 1 und 2):** mehrzeiliges Feld, das mit dem Text wächst (bis 60 % der Bildschirmhöhe, danach Scrollen im Feld), mit Ziehgriff darunter zum Vergrößern; die Höhe wird je Feld im Browser gemerkt, Doppelklick auf den Griff stellt auf automatisch. Die Eingabetaste schließt die Tastatur; Zeilenumbrüche beim Einfügen werden zu Leerzeichen. Der Text bleibt ein einziger Lauftext. Blockseiten behalten ihre Felder.
- **Programme sortieren:** Pfeile hoch/runter in der Programmliste wie in der Bibliothek. **Firmware:** neue Route `POST /api/collections/move` (`id`, `to`), Reihenfolge in `/rb-lib/porder.bin` (höchstens 24 Byte, Liste der Programm-IDs). `GET /api/collections` liefert in dieser Reihenfolge; fehlt die Datei oder ist sie beschädigt, gilt aufsteigende ID, neue Programme stehen am Ende, gelöschte verschwinden von selbst. Programmdateien und Bibliotheksformat bleiben unverändert. Die Reihenfolge ist nicht Teil des JSON-Backups.
- **Startbildschirm:** Die Tafelsteuerung unten zeigt dieselben Tasten zum Erzwingen eines Anzeigeverhaltens wie auf allen anderen Seiten (zuvor ausgeblendet), zusätzlich den Stift für die Kurzwahlen. Auf der Ambiente-Seite bleibt die eigene Bedienung.
- **Geprüft (ohne Arduino/Hardware):** `tests/test_browser_0441.py` (Chromium, Oberfläche Ende-zu-Ende), `tests/test_build_0441.py` (Invarianten), `tests/test_render_parity_0441.py` (Zeichnung Firmware ↔ Browser, unverändert zu 0.4.3.4 ohne Marken), `tests/host_program_order_0441.py` (echter Firmware-Code der Programm-Reihenfolge mit Stubs). **Kein Arduino-Gesamtcompile, kein Test auf ESP32 und Tafel.**
- **Bekannt/offen:** Doppelklick auf den Ziehgriff ist auf dem iPhone ungeprüft. Zeichen-Editor für Blockseiten (mit Rückgängig/Wiederherstellen dort) folgt in 0.4.5.0. `RB_TEST_HANG` bleibt in diesem Entwicklungsstand 1.

## v0.4.4.0-dev – Zeichen gestalten: Farbe und Schrift pro Zeichen (31.10.2026)

- **Neu im Lauftext-Editor:** Fenster „Zeichen gestalten“ (Knopf unter dem Textfeld). Zeichen einzeln antippen oder per Wischen auswählen, dann Farbe und/oder Schrift (Normal → Fett → Engschrift → Mini) für die Auswahl setzen. Alle/Keine/Rückgängig, LED-Vorschau im Fenster. **Farbwechsel:** Farben der Reihe nach antippen, dann „Pro Zeichen“ oder „Pro Wort“ anwenden (Leerzeichen zählen nicht, Symbole zählen, bleiben aber unverändert; je Zeile neu begonnen). Zweizeilig Mini: Farbe pro Zeichen und Zeile, Schrift fest.
- **Übertragungsformat:** unsichtbare Steuerzeichen im Text (UTF-8, je 2 Byte): U+0090 = Zeilenfarbe, U+0091..U+0097 = Farbe 1..7, U+0098..U+009B = Schrift Original Normal/Fett/Engschrift/Mini (einzeilig), U+009C = Schrift der Zeile. Marken haben keine Breite und gelten bis zur nächsten Marke derselben Zeile. Symbole behalten ihre eigenen Farben; im Browser werden die sieben Herz-Emojis gegen die Farbe getauscht (❤️💛💚🩵💙💜🤍 ↔ 4,6,2,3,1,5,7). Einträge ohne Marken bleiben unverändert gültig.
- **Firmware:** `measureText`, `drawInlineUtf8` und `drawInlineUtf8Window` werten die Marken aus (neue `struct TextStyle`, `styleInit/styleApply/styleTop`, gemeinsames `drawToken`). Dadurch wirken sie überall gleich: Lauftext senden, Bibliothek, Programme, Gruppen, Streaming-Kacheln. `decodeText` (Blockseiten) überspringt die Marken. Programm-Verkettung setzt zwischen Einträgen Farbe und Schrift zurück. Ohne Marken ist die Ausgabe pixelgleich zu 0.4.3.4 (Host-Vergleich mit 35 Fällen). Der Wächter-/Test-Auslöser-Code aus 0.4.3.4 ist unverändert (`RB_TEST_HANG` weiterhin Standard 1, für eine Freigabe auf 0 setzen).
- **Weboberfläche:** Zeilenmodell mit Farbe/Schrift je Zeichen, mitgeführt beim Tippen (neue Zeichen erben vom linken Nachbarn); Entwurf bleibt im Browser erhalten; Bibliothek speichert und öffnet die Marken; Hinweis, wenn der Text für den Bibliotheksspeicher zu lang wird (über 1599 Byte). Globale Farbwahl löscht Einzelfarben, globale Schriftwahl setzt Einzelschriften zurück (jeweils rückgängig machbar über „Stil rückgängig“).
- **Senden-Farbe:** Alle Senden-Knöpfe (Lauftext, Bibliothek, Entwurf, Programm, Blockseiten, Fenster) in einheitlichem Mint (#3ddc97). Regler im unteren Bereich etwas größer und mit mehr Abstand (Bereich nur ca. 27 px höher). Palettenknopf als achte Zelle neben den Farben.
- **Geprüft (ohne Arduino/Hardware):** `tests/test_build_0440.py` (Invarianten), `tests/test_browser_0440.py` (Playwright, Editor Ende-zu-Ende, drei Mobilgrößen), `tests/test_render_parity_0440.py` (Firmware-Renderfunktionen im Host-Build, Pixelvergleich Firmware ↔ Browser in 10 Fällen, Altausgabe pixelgleich). **Kein Arduino-Gesamtcompile, kein Test auf dem ESP32 und auf der Tafel.**
- **Bekannte Grenzen:** Blockseiten haben noch keinen Zeicheneditor. Fenster-Vorschau zeigt die ersten 64 Spalten und springt zum ersten gewählten Zeichen. Bibliotheks-Textlimit 1600 Byte (Marken zählen mit, 2 Byte je Marke).

## v0.4.3.4-dev – Geräte vor dem Wächter-Neustart abmelden, Test-Auslöser (31.10.2026)

- **Anlass:** Der Hang-Wächter aus 0.4.3.3 hat im Hardwaretest funktioniert (Lauf B: Hänger in `ble_connect`, Neustart nach 25 s, Spur `[VORLAUF]` lesbar). Danach war die Weboberfläche aber nicht erreichbar, obwohl das Handy „WLAN verbunden“ zeigte; im Serial-Log fehlte `Station verbunden`. Der Hänger selbst ließ sich im Wiederholungslauf nicht erneut auslösen (22 min, 103 Aufträge, 94 BLE-Verbindungen, 0 Hänger, 0 Write-Fehler, Heap im Leerlauf stabil bei ca. 40 KB, System-Minimum 13.504 B einmalig beim ersten Seitenaufbau).
- **Vermutung (nicht bewiesen):** `abort()` startet hart neu, ohne die Geräte am Access Point abzumelden. Das Handy hält die alte Verbindung für gültig, der neue AP kennt es nicht.
- **Neu:** Vor `abort()` sendet der Wächter `esp_wifi_deauth_sta(0)` (alle Geräte abmelden) und wartet 250 ms. Die Spur im RTC-Speicher erhält dafür ein Flag; beim nächsten Start steht in Serial, Diagnose und Absturzspur „Geräte vorher abgemeldet“ bzw. „NICHT abgemeldet“. Schalter `RB_DEAUTH_BEFORE_RESTART` (Standard 1).
- **Test-Auslöser (nur Entwicklungsbuild):** `http://192.168.4.1/api/test/hang` (mit `?deauth=0` ohne Abmelden, zum Vergleich). Die Seite antwortet, `loop()` bleibt nach etwa 2 s absichtlich stehen, der Wächter startet nach 25 s neu. So lässt sich der Wiederanlauf prüfen, ohne auf einen zufälligen BLE-Hänger zu warten. Schalter `RB_TEST_HANG` (hier Standard 1; für eine Freigabe auf 0 setzen, weil jeder Client im WLAN das Gerät damit anhalten könnte). Kein Knopf in der Oberfläche.
- **Diagnose/JSON:** neue Zeilen „Abmelden vor Waechter-Neustart“ und „Test-Ausloeser /api/test/hang“; JSON-Felder `hangDeauth`, `testHang`. Puffer für die Absturzspur von 150 auf 200 Byte vergrößert.
- **Geändert:** nur Sketch (neuer Block hinter `rbBeat`, Wächter im 20-ms-Timer, Test-Handler vor `onHttpTraceDownload`, Route in `setup()`, Prüfung in `loop()`, Diagnose-/JSON-Ausgabe) und Versionskennung der Weboberfläche. BLE-Protokoll, API und Speicherformate unverändert.
- **Prüfstand:** `tests/test_build_0434.py` und `tests/test_0434.py` laufen durch; Wächter-, Abmelde- und Test-Handler-Code wurde in einem Host-Stubtest (g++) auf Syntax und Ablauf geprüft (Abmeldung vor Abbruch, Flag, Spurtext, Test-Handler mit und ohne `deauth=0`). **Kein Arduino-Gesamtcompile, keine Hardwareprüfung.** Ob `esp_wifi_deauth_sta(0)` aus dem Timer-Task unter Arduino-ESP32 3.3.x wie erwartet funktioniert, zeigt erst der Test.
- **Messplan:** siehe README (Test 1: Wiederanlauf mit und ohne Abmelden, Test 2: zwei Geräte gleichzeitig).

## v0.4.3.3-dev – RAM-Messung, Hang-Wächter und Absturzspur (30.10.2026)

- **Anlass:** In einer Sitzung auf v0.4.3.2 (15 min, ein bis zwei Clients, 21 BLE-Aufträge in Ordnung) zeigte die Tafel bei einem Blockband-Upload während der %-Anzeige „ERROR“, fiel auf den Herstellerscreen zurück, und der ESP32 antwortete nicht mehr (kein Web, keine Serial-Ausgabe, kein Neustart). Das Serial-Log endete in diesem Auftrag kurz nach „Verbindung hergestellt“; der RAM-Verlauf davor war unauffällig (ca. 39,9 KB frei, größter Block 29,7 KB).
- **Weiterer Anlass:** Die Messreihen A–C auf v0.4.3.2-dev zeigten, dass das System-Minimum des Heaps beim Beitritt eines (zweiten) WLAN-Clients einbricht (Lauf C: 192 B bei 42,7 s, bevor die Diagnose zum ersten Mal geöffnet wurde). v0.4.3.3 ändert noch nichts am Verhalten der Weboberfläche, sondern macht die Ursache messbar und entfernt eine vermutete Zeitfalle.
- **Befund im Quelltext des ESP32-Cores 3.3.0:** `BLERemoteCharacteristic::writeValue()` wartet auch bei Schreiben ohne Antwort am Ende unbegrenzt auf den BLE-Stack; `BLEClient::getServices()` und `registerForNotify()` ebenfalls. Bricht die Gegenstelle dabei ab, kann `loop()` dauerhaft stehen bleiben. Der Task-Watchdog überwacht `loop()` in diesem Sketch nicht. **Das ist eine plausible, noch nicht bewiesene Erklärung**; die Lage des Log-Endes passt nicht sauber dazu (siehe Messplan).
- **Hang-Wächter:** `loop()` und die BLE-Schritte melden Lebenszeichen. Der bestehende 20-ms-Timer (eigener Task) startet das Board per `abort()` neu, wenn länger als `RB_HANG_RESTART_MS` (25 s, 0 = aus) kein Lebenszeichen kam. Legitime Wartezeiten sind kürzer (BLE-Verbindung höchstens 10 s, Pakete melden einzeln). `abort()` statt `esp_restart()`, damit hängende Stacks nicht über Shutdown-Handler selbst blockieren.
- **Absturzspur:** Vor jedem Schritt (RAM-Marken, HTTP-Handler, BLE-Verbindung, Service-Suche, Notify-Anmeldung, MTU, Paket schreiben, auf Bestätigung warten, trennen) werden Phase, Zeit, Auftrag und Heap in den RTC-Speicher geschrieben. Er überlebt Panic, Watchdog und Software-Reset, **nicht** das Ausstecken. Beim nächsten Start: Serial-Zeile `[VORLAUF]`, Ereignis im Diagnose-Log, Zeile „Absturzspur Vorlauf“ in Diagnose und RAM-Export, JSON-Feld `prevRun`.
- **Sketch-Größe nur einmal:** `ESP.getSketchSize()` wird nach Quelltext des ESP32-Cores (3.3.0) bei jedem Aufruf über `esp_image_verify` ermittelt, prüft also das Flash-Image. Die Diagnose rief es zweimal pro Anfrage auf; die Diagnose-Handler dauerten konstant 300–322 ms. Jetzt einmal in `setup()` ermittelt (`sketchSizeCached`), die Dauer wird als `sketchSizeMs` protokolliert. **Ob dies die 300 ms vollständig erklärt, zeigt erst die Messung.**
- **Start-Aufteilung:** `[RAM]`-Zeilen (seriell, im RAM-Export unter „START-AUFTEILUNG“) nach Start, Sketch-Größe, BLE-Init, NVS-Seiten, LittleFS-Bibliothek, Programmen und WLAN-AP, jeweils mit freiem Heap, größtem Block, System-Minimum und Zeit. Damit wird der BLE-Anteil an den bisher gemessenen ca. 75 KB Startverbrauch sichtbar (vorher als Summe).
- **WLAN-Ereignisse:** AP gestartet/gestoppt, Station verbunden/getrennt, IP vergeben, mit eigenem Zeitstempel und Heap-Werten im Moment des Ereignisses (Ring mit 24 Einträgen, Export „WLAN-EREIGNISSE“ in `/api/ram.txt`; zusätzlich Zeilen `WLAN-EVT` im Diagnose-Log). Der Event-Task schreibt nur in den Ring; ausgegeben wird in `loop()`.
- **Versuchsschalter (Standard = Verhalten wie 0.4.3.2):** `RB_AP_MAX_CLIENTS` (Standard 4; auf 2 setzen begrenzt die Geräte am AP) und `RB_LAZY_BLE_INIT` (Standard 0; 1 startet BLE erst beim ersten Auftrag). Der zweite Schalter ist ein reiner Messversuch: Dann ist das WLAN schon aktiv, der Speicherblock für BLE ist kleiner, ein Fehlschlag ist denkbar.
- **Diagnose/JSON:** neue Zeilen „Sketch-Größe einmal beim Start ermittelt, Dauer“, „AP max. Clients“, „BLE-Start“; JSON-Felder `sketchSizeMs`, `apMaxClients`.
- **Nebenwirkung:** Hängt der BLE-Stack, startet das Board nach etwa 25 s selbst neu und ist dann wieder erreichbar. Die Tafel bleibt eventuell im eigenen Fehlerzustand und muss gegebenenfalls aus- und eingeschaltet werden.
- **Geändert:** nur der Sketch (neue Blöcke vor `ramStageLabel` und nach `bleClient`, Hooks in `ramTraceMark`, Timer, HTTP-Anfang/-Ende, BLE-Pfad, `sendPacket`, `loop`, `setup`, Diagnose-/RAM-Export). Weboberfläche nur Versionskennung; BLE-Protokoll, API und Speicherformate unverändert.
- **Prüfstand:** `tests/test_build_0433.py` und `tests/test_0433.py` laufen durch; Wächter- und Spurcode wurde mit Stubs auf Syntax geprüft und das Auslöseverhalten simuliert (kein Abbruch bei 20 s, Abbruch bei 31 s). **Kein Arduino-Gesamtcompile, keine Hardwareprüfung.**
- **Nachtrag (31.10.2026, Hardwaretest):** Start-Aufteilung gemessen (Start 195.152 B frei → BLE-Init −72,9 KB → LittleFS −2,5 KB → WLAN-AP −57,4 KB → 62.248 B frei); Diagnose-Handler jetzt ca. 28 ms statt 300 ms. Lauf B: Hänger in `ble_connect` (18. Verbindung), Wächter-Neustart nach 25 s, danach Web nicht erreichbar (siehe 0.4.3.4). Wiederholungslauf: kein Hänger in 94 Verbindungen.
- **Messplan:** (1) Fehler nachstellen (mehrere Blockband-Uploads kurz nacheinander) und beim Hänger abwarten: Nach etwa 25 s sollte das Board neu starten und `[VORLAUF]` die Phase nennen, an der es stand. (2) Danach Messreihen A, C und D wie in v0.4.3.3 beschrieben, mit RAM-Datei, HTTP-Datei, Diagnose und seriellem Log. (3) Je nach Phase gezielt gegen die unbegrenzte Wartestelle absichern.

## v0.4.3.2-dev – Programme im Bibliotheksstil (30.10.2026)
- **Programmliste wie die Bibliothek:** Icon-Leiste (neu, bearbeiten, starten, stoppen, löschen), Zeilen mit Name und Untertitel („5 Einträge · Original“ / „Gemeinsames Laufband“), gelber Rand für die Auswahl. Programme mit fehlenden Einträgen sind rot markiert („1 Eintrag fehlt“); Starten ist dafür gesperrt.
- **Eigene Bearbeitungsansicht:** Stift oder „+“ öffnen sie, der Pfeil oben links führt zurück. Name, Wiedergabeart (Kurznamen „Original (Blockseiten stehen)“ / „Alles als Laufband“), Schritte, Auswahl zum Hinzufügen und „Programm speichern“. Die Schrittliste scrollt nicht mehr in sich, die Seite scrollt.
- **Schritte als Bibliothekszeilen:** mit Typ-Untertitel („Lauftext“, „Blockfolge · 4 Seiten“), Pfeilen und Entfernen-Symbol. Fehlende Einträge lassen sich weiterhin über „Ersetzen durch …“ reparieren.
- **Ablauf:** Speichern kehrt zur Liste zurück. „Zurück“ fragt bei ungespeicherten Änderungen nach und lädt sonst den gespeicherten Stand neu. Nach dem Löschen ist nichts ausgewählt.
- **Firmware:** nur `FW_VERSION` = `0.4.3.2-dev` und der Serial-Banner (nutzt jetzt `FW_VERSION`). Keine Änderung an BLE, API, LittleFS-/NVS-Formaten oder Speicherlogik. Die eingebettete Webseite wächst um rund 0,9 KB gzip (50.398 → 51.264 Byte).
- **Geändert in der Weboberfläche:** `view-programs` (Markup, CSS, `v42ShowPrograms`, `v42RenderProgramSteps`, neue Hilfsfunktionen `v43Prog*`). Alle bisherigen Element-IDs und API-Aufrufe der Programme bleiben bestehen.
- **Prüfstand:** `tests/test_0432.py` (Browser mit simulierter API auf 360/390/430 px: Liste, Auswahl, Bearbeiten, Abbrechen/Verwerfen, Speichern, Neu, Löschen, defektes Programm, Werkzeugleiste einzeilig, kein horizontaler Überlauf) und `tests/test_build_0432.py` (Version, gzip/ETag, JavaScript-Syntax, Diagnose-Verträge) laufen durch. `browser_8_shortcuts` und `browser_hotfix_smoke` laufen durch. `tests/test_browser_save_as_4254.py` schlägt bereits gegen den unveränderten Stand v0.4.3.1 fehl (veralteter Test zur Speichern-Auswahl) und wurde nicht angefasst. **Kein Arduino-Compile dieses Standes, keine Prüfung der neuen Ansicht auf iPhone/ESP32, Heap-Frage weiter offen.**
- **Auf dem iPhone zu prüfen:** Werkzeugleiste einzeilig, Tippziele, Zurück-Rückfrage, Speichern → Liste, Starten/Stoppen eines Programms, Reparatur eines defekten Programms, Bearbeiten mit zwei Geräten.
- **Nachtrag (30.10., Rückmeldung des Entwicklers):** kompiliert, geflasht, neue Ansicht optisch freigegeben. Messreihen A–C auf diesem Stand: Ein Client, 12 min, System-Minimum tiefster Wert 16.176 B, kein Leck (frei im Leerlauf 41–45 KB). Zwei Clients: System-Minimum 8.016 B (Lauf B) bzw. **192 B (Lauf C, 42,7 s, vor der ersten Diagnoseanfrage)**; allein der Beitritt zweier Geräte ohne Seitenaufruf senkte es auf 16.320 B (Lauf B2). Schwerster Handler ist `/api/library` (bis 809 ms; 11.516 B Tiefstwert mit zwei Clients). Ursachen sind teils gemessen (Beitritt), teils Vermutung (lange Handler, BLE-Anteil am Startverbrauch).

## v0.4.3.1-dev – Diagnoseentlastung, HTTP-Eingrenzung, UI-Korrekturen (30.10.2026)
- **Firmwareversion:** Zentrale Versionskennung von fälschlich `0.4.2.5.1-dev` auf `0.4.3.1-dev` korrigiert. Diagnosedateinamen und Diagnose-JSON folgen wieder dem tatsächlichen Sketch.
- **Diagnose-Speicher:** Große `String.reserve(11000)`- und `reserve(5000)`-Puffer entfernt. Diagnosebericht und JSON-Ausgabe werden in kleinen Blöcken als HTTP-Antwort gestreamt; der Ereignisring wird ohne große Zwischentextkopie ausgegeben. Die TXT-Datei wird erst bei Download oder Kopieren erzeugt.
- **Diagnose-Tab:** Beim Wechsel zur Ansicht genau eine kompakte JSON-Anfrage statt parallelem JSON- und TXT-Abruf. Mehrfachanfragen per `toggle` entfernt; erneutes Aktualisieren bleibt über die vorhandene Taste möglich.
- **HTTPTRACE:** Bei Ringüberschreibung bleiben die längste Anfrage, die Anfrage mit dem niedrigsten gemessenen Heap und der schwerste beobachtete System-Min-Abfall inklusive Requestpfad erhalten. Separate Beobachtung eines Min-Abfalls außerhalb eines HTTP-Handlers, damit nicht jeder Tiefstwert pauschal HTTP zugeschrieben wird. Keine Queryparameter, Textinhalte oder Passwörter in Logs.
- **Kleinere APIs:** Statusantwort mit begrenztem `snprintf`-Puffer statt umfangreicher `String`-Verkettungen; `respond()` vermeidet die mehrfache String-Kettenbildung.
- **Ambiente:** Sieben Farben wieder **einspaltig**, von oben nach unten Rot, Gelb, Grün, Cyan, Blau, Magenta, Weiß; verfügbaren Platz weiterhin ausnutzen.
- **Blockeditor:** Nachfrage „Seite # leeren?“; bei einer Seite `trash-off` und „Alles löschen“ deaktiviert und grau, ab zwei Seiten aktiviert. Icon für „Alles löschen“ jetzt originales Lucide `flame-kindling`. Drei Senden-/Speichern-Tasten in einem gemeinsamen Raster mit gleichem Abstand.
- **Bibliothek:** Umbenennen verwendet nun die unveränderten SVG-Pfade von Lucide `text-cursor-input` statt der fehlerhaften Nachzeichnung.
- **Nicht geändert:** BLE-Protokoll, LittleFS-/NVS-Schema, gespeicherte Bibliothek, Programme, acht Kurzwahlen und Firmwarepartition. Größere Neugestaltung der Programme-Ansicht steht aus.
- **Prüfstand:** JavaScript-Syntax, Kompression/ETag und simulierte Browserabläufe auf drei Smartphonegrößen erfolgreich. Nachtrag (30.10., Rückmeldung des Entwicklers): kompiliert, läuft auf dem ESP32-C3, BLE-Übertragungen erfolgten; die Heap-Messreihen stehen aus. Historisches Heap-Minimum von 224 B ist noch nicht erklärt oder nachweislich behoben.

## v0.4.3-dev – Konsolidierung der Bedienoberfläche (30.10.2026)
- Neues iOS-App-Icon und Favicon: originale Lucide-Banane der Menütaste, gleiche hellgrauweiße Strichfarbe (#f3f5fa) und dunkler Tastengrund (#303744), offline aus PROGMEM.
- Lauftexteditor: überflüssige eigene Standbild-Option entfernt; einheitlicher Vollbreiten-Senden-/Speichernbereich, neue Speichern-Auswahl „Änderungen speichern“ / „Als neuen Eintrag speichern“ für Bibliothekstexte.
- Blockeditor: Standardschrift Original - Normal für neue Seiten; Dreier-Icongruppe für Ausrichtung; ikonbasierte Seitenwerkzeuge inkl. Radierer, trash-off bei Einzelseite, shredder/Alles löschen erst ab zwei Seiten; dreiteilige Vollbreiten-Senden-/Speichernaktionen.
- Bibliothek: vereinheitlichte Tooltips ohne alte Unicode-Zeichen; Umbenennen mit text-cursor-input; Name-/Duplizieren-Dialog wie bei Kurzwahl mit Lucide-Icons.
- Globale Tafelsteuerung: Standbild-Icon „octagon“. Ambiente: Raster füllt die verfügbare Höhe aus.
- Editor: Bibliothek-Rückweg nur bei Bearbeitung einer Bibliotheksvorlage eingeblendet. Bereits vorhandene BLE- und LittleFS-/NVS-Formate bleiben unverändert.
- ESP32-Compile, Hardware-/Heap-Tests stehen weiterhin aus. Alte Standbildvorlagen bleiben im Speicher; Wiedergabe über Programme kann sie weiterhin verwenden, der Lauftexteditor legt keine neuen an.

## v0.4.2.5.4-dev – Vorlagen im Editor auch als neuen Eintrag speichern (30.10.2026)
- **Lauftexte und Blockfolgen:** Bibliothekseintrag im Editor bearbeiten und zwischen „Änderungen speichern“ (bestehenden Eintrag überschreiben) und „Als neuen Eintrag speichern“ (neue ID, neues Namensfeld, Original unverändert) wählen.
- Nach erfolgreichem „Als neuen Eintrag speichern“ bearbeitet der Editor die neue Kopie. Original, bereits vergebene Kurzwahlen und Programmschritte bleiben auf der ursprünglichen ID.
- **Fehler behoben:** Wechsel der Lauftext-Schriftart entfernt nicht mehr die Bindung zum geöffneten Bibliothekseintrag und setzt nicht länger unbeabsichtigt den Änderungs-Snapshot zurück.
- Nur Web-UI/Version/Dokumentation geändert; LittleFS-/NVS-Formate, BLE und Partitionierung unverändert. ESP32-Hardware-, Arduino-Compile- und Heap-Abnahmetests offen.

## v0.4.2.5.3-dev – Bibliothekssuche und manuelle Aktualisierung entfernt (30.10.2026)
- Fernglas, Suchfeld und manuelle Aktualisieren-Schaltfläche aus der Bibliothek entfernt.
- Sämtliche dazugehörigen DOM-IDs, Eventhandler, Titel-Filterlogik, Filter-CSS und exklusives Fernglas-SVG gestrichen, statt nur die Buttons auszublenden.
- Ungefilterte Bibliotheksliste; Sortierpfeile funktionieren ohne Suchzustand.
- Bestehende interne `refreshLibrary()`-Aufrufe für alle Datenänderungen, Navigation und Initialisierung ausdrücklich erhalten.
- Gespeicherte Inhalte, acht Kurzwahlen, Programme, BLE und Partitionslayout unverändert; Speicherproblem weiter offen.

## v0.4.2.5.2-dev – Acht Kurzwahlen, Favoriten aus Bibliothek entfernt (30.10.2026)
- RBS3-Kurzwahlliste mit 8 Slots (neue Belegung); altes 6-Slot-Format bleibt unberührt.
- Startseite mit 2 Spalten × 4 Reihen und flexiblem Raster und Helligkeits-/Geschwindigkeitssteuerung.
- Ladescreen mit lokalem Lucide-Bananen-SVG.
- Mini-ß/ẞ: einen unteren Pixel entfernt.
- UI-Änderungen; Heap-Stabilität separat weiterhin offen.

## v0.4.2.5.1-dev – 30.10.2026 · UI-Hotfix (Hardwaretest ausstehend)

- **MENÜ-01:** Sieben versehentlich verstümmelte `<span>`-Starttags nach inline-SVG im Hauptmenü repariert. Ursache: fehlerhafte HTML-Transformation in v0.4.2.5-dev. Hauptmenütexte standen als `span>Kurzwahl` usw. im Browser.
- **UI-01:** Statische Kurzwahl-/Reload-SVGs jetzt schon im HTML. Initiales `data-rb-view=home` verhindert den kurzzeitig sichtbaren zweiten Geschwindigkeitsregler. Startansicht wird erst sichtbar, nachdem die erste Dateninitialisierung abgeschlossen ist; bei Initialisierungsfehlern wird sie trotzdem freigegeben.
- **Unverändert:** v0.4.2.5-HTTP-/gzip-/ETag-/JSON-Puffer-Optimierungen, BLE, LittleFS, Programme, NVS und Partitionierung.
- **Tests:** HTML/JS- und Renderregressionen offline; **vollständiger Arduino-Build, iPhone-Test und RAM-/BLE-Stabilität nicht verifiziert**. Die beobachteten historischen Free-Heap-Tiefstwerte sind weiterhin offen.

## v0.4.2.5-dev – 29.10.2026 · HTTP-/UI-Stabilisierung (Hardware-Abnahme offen)

- **Basis:** v0.4.2.4.2-dev-B, reproduzierbar gzip-komprimierte Webseite, inhaltsbasierter ETag und HTTP-304; 20-ms-HTTP/RAM-Diagnose bleibt aktiv, damit neue Tests direkt vergleichbar sind.
- **Erkenntnis:** A/B-Test auf ESP32-C3 zeigte kritische historische Heap-Tiefstwerte bis 592 B bei B – bereits bei einem verbundenen WLAN-Client. gzip verbessert Transferdauer/304, **löst den extremen Heap-Tiefstwert nicht nachweislich**. Wir markieren das ausdrücklich nicht als gelöst. Keine BLE-Uploads während dieser Messungen.
- **API:** `/api/library` und `/api/collections` senden Listen-JSON mit einem auf 1024 B begrenzten Sammelpuffer statt vielen Mini-HTTP-Sendungen. Schemas, Reihenfolge, fehlende/defekte Datensätze und gespeicherte LittleFS-Binärformate bleiben unverändert; schnelleres und RAM-schonenderes Verhalten ist noch am Gerät zu messen.
- **UI-01:** Statische Menü-, Steuerungs- und Regler-Icons als fertige Lucide-SVG direkt im HTML statt sichtbarem Fallback-Symbol und spätem SVG-Austausch. Dynamische Icons weiterhin per JS. Alte Canvas-Malroutine überspringt nun nicht-Canvas-Elemente.
- **Lizenz:** `LICENSE` wieder reiner Standard-MIT-Text, Lucide/Feather-Material bleibt separat in `THIRD_PARTY_NOTICES.md` und README ausgewiesen. Adam Schulz/OpenSign als Community-Protokollquelle gewürdigt; **kein Code aus seinem Repo in dieser Version kopiert**.
- **Grenze:** Keine Arduino-IDE/Hardwarekompilierung in der Entwicklungsumgebung; kein belegter Heap-Fix. Mehrere WLAN-Clients und BLE weiterhin später unter realer Last testen.

## v0.4.2.4.2-dev-A/B – 29.10.2026 · Kontrollierte HTTP-/RAM-Messung

- **Build A:** Unveränderte, unkomprimierte Webseitenauslieferung wie v0.4.2.4.1-dev; neue HTTP-/RAM-Messung für alle registrierten Routen und unbekannte 404-Pfade. `/api/http.txt` exportiert die letzten 32 Anfragen, Zeit, WLAN-Clientzahl, 20-ms-Speicherminima und historische System-Minima vor/nach der Anfrage. Identische Instrumentierung in Build B.
- **Build B:** Dieselbe Webseite komprimiert (gzip-9, 44.911 statt 157.222 Byte HTML). Auslieferung aus `PROGMEM` mittels `send_P(..., length)`; `Content-Encoding: gzip`; inhaltsbasierter ETag (`SHA-256`-Präfix) aus der Webquelle; registriertes `If-None-Match`, `Cache-Control: no-cache` und 304 für unveränderte Version. API weiterhin `no-store`.
- **Messplan:** A und B jeweils nach Neustart mit einem und zwei WLAN-Clients, Seitenaufrufen und passivem zweiten Client vergleichen. HTTP 200 und 304 unterscheiden. Kein neuer BLE-/Renderingpfad.
- **Diagnosekorrekturen:** Veraltete Behauptung, dass Editorseiten in NVS persistieren, berichtigt; Überschrift „ZÄHLER“ korrigiert. Frühere Bibliotheksdatensätze und Partitionierung unverändert.
- **Status:** Keine Arduino-Kompilierung und kein Hardwaretest für A/B durchgeführt. Timing und Speicherwirkung des Timers sind selbst zu messen. 20-ms-Abtastung kann kürzere Spitzen verpassen.

## v0.4.2.4.1-dev – 29.10.2026 · Bedienungs-Hotfix und überprüfter Hardwarestand

- **Hardware v0.4.2.4 bestätigt:** Sketch kompiliert, Boot ohne Panic, Bibliothek/Kurzwahlen erhalten; kleines gemischtes Laufband mit einem Upload und korrekten Farben/Reihenfolge; 16 leere Pixelspalten zwischen Einträgen. Die Langzeit-, Retry- und >30-KiB-Prüfungen bleiben ausdrücklich offen; Original-Mischprogramm in dieser Runde nicht reproduzierbar.
- **Tafelsteuerung:** Ein einzelner gemeinsamer Abschnitt bleibt funktional erhalten, jedoch nicht mehr `position:fixed`: als normale Box nach dem jeweiligen Seiteninhalt. Dokument scrollt; keine Überlagerung von Speicherbuttons, Listen oder Kurzwahlen. Home 2×3 in kompakten Reihen.
- **Kurzwahlen:** Einrichtungsdialog mit ausschließlich Icon-Aktionen (`trash`, `undo-dot`, `star-check`), verständlichen `title`-/`aria-label`-Attributen. Bearbeitungsmodus wechselt zwischen `pencil` und `pencil-off`, ohne überflüssige Toasts.
- **Speicherkonsistenz:** `v042DeleteCollection()` entfernt beim bewussten Löschen auch `.bak` (Review F-06), die bestehende Rückfalllogik für **nicht gelöschte** Programme bleibt unangetastet.
- **Grenzen:** Keine Änderung der laufenden BLE-/Streamingzeitsteuerung; keine neue ABI oder Datenmigration; Arduino-Build und ESP32-Hardwaretest v0.4.2.4.1 weiterhin ausstehend. Review-IDs F-01–F-12 / S-01–S-06 werden separat verfolgt.

## v0.4.2.4-dev – 28.10.2026 (Implementierung, Hardwaretest offen)

- **Befund aus v0.4.2.3 auf echter CoolLEDX:** Kurzes gemischtes Scrollband funktioniert; bei größerem, aufgeteiltem Band schnitt die alte 20-s-Gruppenzeit ab. Der letzte BLE-Upload brach bei Paket 70/106 ohne Bestätigung ab. Kein nachgewiesener RAM-Panic (RAMTRACE Heap-Minimum 17 KB, später 31 KB frei).
- **Firmware:** Gemischte Laufbänder werden tilebasiert mit einem 384-B-Puffer generiert; 20-s-Ersatzzeit beeinflusst Programme nicht mehr. Stattdessen vorläufige Breite/Tempo-Schätzung, die eine volle Wiedergabe bevorzugt. Maximal ein kompletter Neu-Upload mit neuem BLE-Verbindungsaufbau nach Paketbestätigungsfehler. Nicht hardwareabgenommen.
- **Web:** Ein gemeinsames, kompaktes Tafelsteuer-Dock am unteren Rand; kleinere Kurzwahlkacheln (2×3), unveränderte direkte Kurzwahl-Einrichtung, acht BLE-Icon-Tasten. Preview nur mit Lucide (Anfang/Play/Pause/Schnecke/Vogel/Hase). Diagnose-Icon `bug`, Menüüberschrift weg, Editor-Fußinfo entfernt, Gruppensekunden-Eingabe entfernt. Mobile Anordnung an Paint-Skizzen orientiert.
- **Repository:** README, Roadmap, Themenspeicher und Testpläne auf Hardwarebefunde abgeglichen; Community-Unterlagen und Templates hinzugefügt. Unsere MIT-Lizenz ist unverändert, Drittanbieterrechte Lucide ISC/Feather MIT einschließlich `LICENSE`-Hinweis bleiben erhalten. Generische KI-„Nutzer“-Formulierungen werden vermieden; Entwicklung als gemeinsame Arbeit mit ChatGPT/Claude transparent benannt.

## Ältere Versionsprotokolle

## v0.4.2.3-dev – Alles als Laufband, Lucide und Bibliothekskomfort

- **Hardwarestand davor:** Unsere v0.4.2.2-dev-Tests bestätigen gemischte Programmwechsel, bestehende Bibliotheks-IDs, Rücknavigation, Programmlöschung/-reparatur. Blockgruppe mit vier Seiten bei Tempo 80: gemessene 6,33 s; vorausberechnet 6,31 s. Die Mischprogrammübergänge sind noch nicht synchron.
- **Zweite Programm-Wiedergabeart:** Programm kann alternativ alle Lauftext- und Blocksegmente horizontal verketten; 16 Pixel Zwischenraum, gleiche Farb-/Glyphenrenderer, möglichst ein Upload, Datenlimit 30 KiB; Splits an Seitengrenzen bzw. zwischen ganzen Texten. Originalmodus für vorhandene Programme bleibt der Standard. Programmschema nutzt bisher reserviertes Byte (RBP2 bleibt bestehen).
- **Bibliothek/Editor:** Neuer-Inhalt-Button direkt nach Speichern und bearbeitete Vorlagen weiter editierbar; leere Kurzwahl öffnet Einrichtung; unvollständige Programm-Kurzwahl rötlich.
- **Backup-Import:** Inhalts- und Namensvergleich vor Wiederverwendung sowie ID-Neuzuordnung für Programme/Kurzwahlen; Schema 1/2 lesbar. Kein unkontrolliertes Überschreiben, kein atomarer Import bei Fehlern.
- **Icon-Oberfläche:** Offline-Lucide-SVGs für Navigation, Schaltflächen, Regler und acht kompakte erzwungene BLE-Modi, die jeden Klick senden, ohne aktiven Zustand. Third-party-ISC- und Feather-MIT-Texte in THIRD_PARTY_NOTICES.md; RavestickBoard-eigener Code bleibt MIT-lizenziert. Hersteller-Pixelmotive bleiben von dieser Freigabe unberührt.
- **Dokumentation:** Gemeinsame Wir-Perspektive und KI-Werkzeuge ChatGPT/Claude wieder aufgenommen; MIT-Lizenz unserer eigenen Software im Nachtrag wiederhergestellt (die zwischenzeitliche restriktive Fassung war ein Dokumentationsfehler).
- **Noch offen:** Echter ESP32-Build, Betrieb mit gleichzeitigem WLAN/BLE und die neue Laufbanddarstellung sind hardwareseitig zu testen; Lauftextgeschwindigkeiten weiterhin unkalibriert.

## v0.4.2.2-dev – Gemischte Programme, sichere Bibliotheksabhängigkeiten, Editorführung

- **Gemischte Programme repariert (Code):** `v042Tick()` wird in der Arduino-Hauptschleife aufgerufen; bisher wurden nur erste Programmgruppen übertragen. Nach erfolgreichem BLE-Upload startet der Timer für den nächsten Gruppenwechsel. Reine Ein-Gruppen-Programme laufen auf der Tafel weiterhin autonom endlos.
- **Blockdauer kalibriert:** Hardwaremessungen 4 Seiten: Tempo 60 = 12,65 s; 70 = 9,42 s; 80 = 6,31 s; 90 = 3,09 s. 6 Seiten bei Tempo 80 = 9,50 s. Vorläufige Berechnung `T[s] ≈ Seiten × 0,0795 × (100 - Tempo)` für 60–90; sonst vorhandene Dauer. Lauftexte bleiben vorerst zeitbasiert, bis ihre Pixelgeschwindigkeit gemessen ist. Programmspeed während aktiver Wiedergabe gesperrt.
- **Abhängigkeiten:** Bibliothekseinträge trotz Verwendung löschen. Betroffene direkte Kurzwahlen werden geleert, unvollständige Programme inklusive fehlender IDs behalten und rot gekennzeichnet. Ein solches Programm kann erst nach Reparatur gestartet werden; laufende Programme mit betroffenem Eintrag werden gestoppt.
- **Editor UX:** Neue und bestehende Vorlagen sichtbar getrennt; beim Bearbeiten nur die betreffende Anzeigeart. Direkte Rückkehr zur Bibliothek, Bestätigung nur bei ungesicherten Änderungen, Speichern aktualisiert vorhandene ID. Vorschau bleibt beim Scrollen unter dem Hauptheader sichtbar.
- **Meldungen:** Kein ständiges Toast „ESP32 nicht erreichbar“ während BLE-Verkehr; erst nach wiederholt fehlschlagenden Statusabfragen über mehr als 30 s dezent im Header. Normale Poll-Statuswerte ohne Toast; Meldungen enden selbstständig.
- **Web und Diagnose:** Browser-Mocktest inkl. Programmreparatur; Renderer-Regression (135 Fälle), v0.4.1-Bibliotheks-Hosttest und JS-Syntaxprüfung ausgeführt. Arduino-Kompilierung und Bestätigung auf ESP32-C3 noch ausstehend. Keine automatische LittleFS-Formatierung und keine Änderung der bisherigen BLE-Bilddatenformate.

## v0.4.2.1-dev – Stabilisierung und Editorbereinigung

- **Hardwarebefund aus v0.4.2:** Blockfolge als ein durchgehendes Pixelband über BLE übertragen; autonomes Blättern und erzwungene Anzeigemodi funktionieren auf der CoolLEDX.
- **Kritischer Fehler behoben (Code):** `Stack protection fault` der Arduino-`loopTask` bei `/api/block/save`. Ursache waren große Stack-Kopien in `rb42Read`/`rb42Write`. Verifikation nun stückweise mit 128-Byte-Puffer; Rückfallkopien erhalten. Auch Metadaten-Journalpuffer werden statisch gehalten. Hardware-Nachtest ausstehend.
- Editor-Arbeitsfläche nur im RAM, keine NVS-Autospeicherung; bestehendes NVS bleibt unverändert. Alte Einzel-Seitenplayer-Bedienung und Aufrufpunkte entfernt.
- „Alles löschen“ mit Sicherheitsabfrage, neuer Seiten-/Reiterwechsel ohne implizites Zurücksetzen. Vorschau im Blockeditor zeigt nur die ausgewählte Seite; zusätzliche Wiedergabeknöpfe entfallen.
- Lauftext „Text leeren“ unter dem Textfeld, standardmäßige Undo-/Redo-Bedienelemente aus dem einfachen Editor entfernt; Datenbudget-Zähler bleibt.
- Programmseite auf dem iPhone vollständig scrollbar, Speichern dauerhaft erreichbar.
- Kurzwahl-Dropdown mit ungewählter Quelle; Beschriftung erst nach gültigem Ziel; kompletter Löschbutton mit Rückfrage, Label wird ebenfalls gelöscht.
- Einstellungsansicht flach gegliedert, Backup/Import weiterhin unter Einstellungen; veraltete Erklärungen und BLE-Hexcodes aus der normalen UI entfernt.
- Bestehende Bibliotheksformate und BLE-Blockbandprotokoll unverändert. Kein automatisches LittleFS-Formatieren.

**Status:** Codekorrektur und lokale Tests, **noch keine erneute ESP32-/CoolLEDX-Hardwarefreigabe**.

## [0.4.2-dev] · Blockfolgen, Bibliothek, Programme und Kurzwahlen

**Entwicklungsstand, noch keine echte Arduino-Kompilierung oder LED-Hardwareabnahme.**

### Neue Funktionen
- 25-Seiten-Blockeditor mit gezieltem Leeren, bearbeitbarer LittleFS-Blockfolge und einem zusammenhängenden BLE-Bildband (1–79 Seiten rechnerisch pro Upload).
- Bibliotheksbestand aus v0.4.1 weiterhin lesbar (`RBL1`, `RBM1`); alter 64er-Metadatenindex wird beim Lesen kompatibel erweitert, nun 192 Plätze und mindestens 64 KiB Flashreserve.
- 24 speicherbare Programme mit je bis zu 24 Bibliotheksschritten und 5–180 s Gruppenzeit; Zusammenfassen benachbarter Blöcke und kompatibler Lauftexte, erneuter Upload bei Gruppenwechsel.
- Sechs Kurzwahlen mit eigenem Quellentyp (Bibliothek/Programm/Ambiente) und eigenem Titel; Migration bestehender Bibliotheksreferenzen.
- UI: Bibliothekssuche hinter Lupe, Bearbeiten-Beschriftung, Backup unter Einstellungen, Live-Namenslängenprüfung, Menü-Refresh und eigenes iPhone-Icon.
- JSON-Backup Schema 2 inkl. Blockfolgen, Programmen und neuen Kurzwahlen; Import Schema 1 bleibt möglich.
- README neu aufgebaut, Roadmap, Quellen/Technik/Installation/Testplan überarbeitet, frühere Firmwareversionen erhalten.

### Prüfung / offen
- JavaScript Syntax- und Chromium-Mocktests. Separater C++-Hosttest des Bandformats.
- Echte Arduino-ESP32-3.3.12-Kompilierung, RAM-Regression und BLE-Bildtest ausstehend. Alte NVS-Seiten dürfen bei Schemawechsel entfallen; gespeicherte LittleFS-Texte nicht.
- Keine native `0x04`-Animation, kein GIF-Upload, keine variabel breiten Icons und noch kein kompletter automatischer Retry nach BLE-ACK-Timeout.

---

## v0.4.1-dev · 27.10.2026 · Persistente Flash-Bibliothek

**Status:** C++-Hosttests (Bibliothek/Journaling) und Browser-Interaktionstests bestanden; **Arduino-Kompilierung und echter Hardwaretest ausstehend**.

- Bis zu 64 Flash-basierte Textvorlagen in LittleFS, stabil nummeriert. Speichert beide Zeilen, Schrift, zwei Farben, Modus, Name und Favoritenflag. Indizes werden in zwei alternierenden, prüfsummengesicherten Kopien gespeichert. Beim Ersetzen wird eine Rückfallkopie erstellt und beim Neustart ein unterbrochener Austausch wiederhergestellt.
- Browser-Bibliothek mit Textauswahl, Öffnen, Speichern aus dem Editor, Überschreiben, Kopieren, Umbenennen, Löschen mit Kurzwahl-Referenzschutz, Suchen, Favorisieren und Reihenfolge ändern (Drag & Drop/Desktop und Pfeile/mobil).
- Sechs Kurzwahlen referenzieren stabile IDs und senden gespeicherte Texte direkt via vorhandener BLE-Warteschlange. Entwurf und aktiver LED-Inhalt bleiben separat.
- JSON-Backup und additiver Import mit neuer ID-Zuordnung, Kurzwahl-Migration nur in freie Felder. Kein automatisches Überschreiben bestehender Vorlagen.
- LittleFS-Ersteinrichtung per expliziter Bestätigung; kein automatisches Formatieren bei Mount-/Metadatenfehlern. Bestehendes NVS-Schema der 12 Blockseiten vollständig unverändert.
- Weitergeführt: v0.4.0.3-Streaming für große Texte, BLE-MTU-Neuverbindung, RAMTRACE und Pixelart-Excel-Stand (Ziggi II etc.).
- Statische JS-Syntaxprüfung, vollständige WebUI-Header-Synchronisierung. Hosttests prüfen unter anderem simulierten Neustart und unterbrochene Dateiumbenennung.
- Grenzen: max. 64 Texte und 1.599 UTF-8-Bytes pro Zeile, zunächst nur Stand/Lauf als gespeicherter Textmodus; keine Programmkette, kein Löschen referenzierter Texte, keine automatischen Anzeigen beim Boot. Schreibvorgänge verbrauchen Flash-Zyklen und sollten nicht an jeden Tastendruck gekoppelt werden.

---

## v0.4.0.3-dev · 27.10.2026 · Stream-Renderer / Speicheroptimierung (Testbuild)

**Status:** C++-Hosttests 135/135 bytegenau erfolgreich; Arduino-Kompilierung und Hardwaretest noch offen.

- Lange Lauftexte (Payload ≥8 KiB) jetzt mit 384-Byte-Kachelrenderer statt bis zu 30-KiB-Vollbildpuffer. Alle Farbkanäle, Symbole, Layouts und CoolLEDX-BLE-Paketbytes bleiben im Vergleichstest identisch.
- Nach MTU kleiner 160: kontrolliert einmalige BLE-Neuverbindung mit erneuter FFF0/FFF1-Abfrage und MTU-Prüfung vor Schreibbefehlen.
- Diagnoseversion korrigiert; RAMTRACE mit per-Job-Minima, auch nach Überschreiben des Ringpuffers.
- WebUX unverändert außer Versionsbezeichnung, aktualisierte Pixel-Art-Sammlung v0.4.0.2 bleibt vollständig erhalten, keine NVS-/LittleFS-Änderung.
- Testplan und Installationsanleitung für v0.4.0.3 ergänzt.

## v0.4.0.2-dev · 27.10.2026 · RAM-Analyse + aktuelle Pixelart

**Status: Diagnose-Testbuild, noch nicht mit Arduino IDE kompiliert und nicht auf ESP32-C3 getestet.** Letzter erfolgreich hardwaregetesteter Stand: **v0.4.0.1-dev**, 20/21 Testpunkte OK (Langzeitpunkt zum Zeitpunkt der Checkliste offen), BLE-Aufträge fehlerfrei. Historische Heap-Minima je nach Szenario: 324 B, 48.108 B und 11.052 B.

- Kleine, fest begrenzte RAMTRACE-Ringaufzeichnung (`64 × 24 B`) im RAM; keine Flash-/NVS-Schreibvorgänge.
- Messungen vor/nach Bildpufferreservierung, am Rendering-Ende, beim BLE-Verbindungsaufbau, vor/während/nach Übertragung (alle 32 Pakete und beobachtete neue Minima), vor/nach Pufferfreigabe und nach Jobende.
- Jede Probe enthält Zeit seit Boot, Job-ID/Typ, freie Heapbytes, historischen System-Minimumwert, maximal allokierbaren Block, Paket-/Payloadangabe und WLAN-Clientzahl. Drei niedrigste beobachtete Kenngrößen bleiben separat erhalten, auch wenn der Ring überschrieben wird.
- `/api/ram.txt` als zeilenweiser Export mit eigenem Diagnose-Button; regulärer Diagnoseexport enthält Zusammenfassungswerte. Der Export selbst verursacht keine große zusätzliche RAM-Kopie.
- Aktualisierte Excel-Sammlung mit 36 Quellmotiven: sechs bunte Warnsymbolvarianten aus dem Picker entfernt (bestehende Sprite-IDs zur Kompatibilität reserviert), neue Ziggi-II-Variante mit ID 63, aktualisierte Frosch-/Zigaretten-/Joint-Grafiken. Alle vorhandenen Schrift-/Symbolcodes außer bewusst geänderten Pixelgrafiken kompatibel. Excel-Banane ID 14 wird nicht ungefragt ersetzt.
- Keine Änderungen an BLE-Protokoll, NVS-Seitenschema, Dateisystem, gespeicherten Daten und bisheriger UI-Navigation.

**Wichtig:** Der systemweite Heap-Tiefststand kann zwischen zwei Messpunkten entstanden sein; er darf nicht ungeprüft der letzten beobachteten Funktion zugeschrieben werden. Hardware-RAMTEST und Funktionstests stehen aus.

---

## v0.4.0.1-dev · 26.10.2026 · WebUX-Feinschliff und Pixelart

**Status: Browser-/Quellcodeprüfung erfolgreich; Arduino-Kompilierung und Hardwaretest noch ausstehend.** Basis v0.4.0: **30/31 Hardwareprüfungen OK**, Dauerbetriebspunkt Nr. 30 noch offen.

- Navigation jetzt **Kurzwahl → Texteditor → Bibliothek → Programme → Ambiente → Diagnose → Einstellungen**; Kurzwahl immer sichtbar.
- Scrollflächen von Bibliothek und Programmen erheblich vergrößert; Aktionsleiste und globale Regler platzsparend angeordnet.
- Vier eigenständige monochrome 16×16-Pixel-Piktogramme an den Reglern: kleine und große Glühlampe, Schnecke und galoppierendes Pferd. Keine zusätzliche Reglerüberschrift.
- Status unten nicht länger permanent: normale Meldungen als 2,7-Sekunden-Toast, Fehler bis zur Bestätigung sichtbar; unbelegte Kurzwahl nur als kurzes Kachelsignal.
- **63 Symbole insgesamt** (36 bisherige einschließlich unverändert nummerierter Krabben-ID plus 27 zusätzliche). Neue Excel-Krabbe ersetzt alte Grafik; ursprüngliche und handgezeichnete Schneeflocke sind separat. Alien steht bei UFO/Weltraum. Herkunfts-Trennung im Picker entfällt.
- Alle neuen Pixelbilder identisch in Web-Canvas und C++-Flashsprites; 35 Excel-Motive und ihre Farbmatrizen im Register mit Hashes dokumentiert. Historische IDs abseits der Krabbe bleiben pixelidentisch.
- **Keine Änderung** an BLE-Befehlen, NVS-Layout oder Speicherformat. Keine neue Bibliotheks-Datenbank und keine autonome Programmverkettung. Der Frosch bleibt als offener Zeichenfeinschliff notiert.
- Kontrolle: Chromium 360×640, 390×844, 1366×768; Startseite ohne Scrollen, größere Listen, Menü, Pixelbuttons, Einfügen neuer Symbole und synchronisierte Daten; keine JavaScript-Fehler.

**Für den Hardwaretest:** Flash ohne vollständiges Löschen des Flashs; bestehende neun Seiten und BLE erneut prüfen. Minimaler Heap in v0.4.0 beim Diagnoselog 9.416 Byte, daher RAM-Reserve beobachten; 30 Minuten Dauerbetrieb separat durchführen.

---

## v0.4.0-dev · 26.10.2026 – WebUX-Neustrukturierung und MIT-Lizenz

**Status:** Browser- und Syntaxprüfung abgeschlossen, Arduino-Kompilierung und Hardwaretests offen. Letzter hardwarebestätigter Rückfallstand: **v0.3.5.1**.

- Neue Navigation über die Banane, fixierter Header auf allen sieben Seiten: Start, Bibliothek, Programme, Texteditor, Ambiente, Diagnose, Einstellungen.
- Startbildschirm ohne vertikales Scrollen: sechs Kurzwahl-Kacheln (vorerst unbelegt), visueller Belegungsmodus, Helligkeit sowie Ein/Aus.
- Bibliothek und Programme mit geplanten festen Aktionsleisten, scrollbarem Listenbereich, globalen Reglern und Modus-Override. **Noch Platzhalter:** Speicherung, Textauswahl, Drag-and-drop und Programmverkettung.
- Vorhandenen Text-/Blockseiteneditor und Diagnosefunktionen in eigene Seiten überführt; keine NVS-Migration, ältere Sketchordner unberührt.
- Symbol-Picker über ♥ geöffnet, thematisch geordnet und mit echten 16×16-Pixelbild-Kacheln statt Unicode-Emoji-Vorschaubildern. Vorhandene 36 Sprites unverändert, neuere Excel-Grafiken noch nicht importiert.
- Globale Helligkeits- und Tempowerte über alle Ansichten synchron; `/api/status` um `brightness`/`speed` ergänzt. Neue Textaufträge ändern diese Sollwerte nicht mehr. Geschwindigkeitsregler wieder **1–100**, Standard 80.
- Neues Ambiente: sieben große Farbtasten senden über `/api/ambience` einen normalen, einfarbigen 64×16-Pixel-Upload. **Hardwarewirkung noch nicht getestet.**
- README vollständig in gemeinschaftlicher Wir-Perspektive neu aufgebaut, tatsächliche Feature-Reife getrennt dokumentiert, selbst erstellter Code von All Rights Reserved auf MIT umgestellt; fremde Glyphen/Bilder bleiben rechtevorbehalten.

Siehe [Entwicklungsstand](docs/ENTWICKLUNGSSTAND_v0.4.0_2026-10-26.md) und [Testplan](docs/TESTPLAN_v0.4.0.md).

---

## v0.3.5.1 – 25.10.2026 – Pixelart-Update (Hardwaretest offen)

- Seven Excel-created 16×16 graphics implemented directly from the cells and the eight-state LED color legend (`Pixel-Art.xlsx`): Huhn, Ravestick-Banane, Gummiente, UFO, UFO+Traktorstrahl, new Küken and Frosch. Existing sprite IDs preserved.
- Original-app screenshot reconstructions Kussmund, Blume, Biene, Kuh re-extracted pixel-by-pixel from the screenshots; fixes one-column X shift and restores rightmost column.
- Font data, BLE commands, display-mode codes, NVS format and block-pages behavior unchanged from 0.3.5.
- Color mapping from the Excel legend: light green and dark green both map to hardware Green; `W` maps to LED White even where the Excel theme displays dark.
- v0.3.5 test feedback: speed-range semantics still require a separate hardware calibration; **not considered fixed** in this patch.
- Hardware / Arduino IDE compilation still to be confirmed by tester.

## v0.3.5 · LED-Steuerung und Pixelmotive (2026-10-25)

**Status: fertig gepackter Firmware-Testkandidat; Arduino-Kompilierung und Hardwaretest offen.**

- LED-Anzeige Ein/Aus mit `0x09 0x00/01` als kurzer BLE-Befehl ohne Pixelupload; physische Funktion und Ein-Schalten nach Aus sind **noch hardwareseitig zu bestätigen**.
- Laufgeschwindigkeit 60–100 statt 1–100; Startwert 80; unveränderte BLE-Skalierung.
- Hardwarebestätigte acht Anzeigemodi eindeutiger benannt, `0x07 = Aufdecken`; präzisierte Dropdown-Überschrift.
- Vier zusätzliche aus den bereitgestellten Original-App-Screenshots abgelesene 16×16-Piktogramme: Kussmund, Blume, Biene, Kuh; Rose-Emoji als abwärtskompatibles Alias für Blume.
- Fünf eigene neue Motive (Quietscheente, Kaktus, UFO, UFO mit Traktorstrahl, Alien) und überarbeitete vertikal gespiegelte Ravestick-Banane; Pixelpalette auf sieben LED-Farben begrenzt.
- Blume: rechter Abstand um 1 Pixel reduziert; Stern zentriert; alle alten Hersteller-Spriteindizes bleiben erhalten.
- Separate neue Firmwaremappe, keine Änderung der NVS-Seitenstruktur, kein neues Playlist-/Laufzeit-System.


## [0.3.4] · 25.10.2026 · Erweiterte BLE-Anzeigemodi (Hardwaretest ausstehend)

**Eigenständiges Feature-Release auf Basis der am 25.10.2026 hardwarebestätigten v0.3.3.** Die zunächst intern „v0.3.3 Korrektur 1“ genannte Erweiterung wird als **v0.3.4** ausgeliefert; kein Fehlerfix am bereits funktionsfähigen v0.3.3-Release.

### Neu und geändert
- Das bestehende Dropdown für direkte Moduswechsel über BLE-Kommando `0x06` führt zusätzlich `0x03` Rechtslauf, `0x04` Aufwärts, `0x05` Abwärts, `0x06` Schneeflocke, `0x07` Bild/Picture und `0x08` Laser. **Alle sechs neuen Werte sind am eigenen Board noch ungetestet.** Referenznamen stammen aus `UpDryTwist/coolledx-driver`; Anzeigeeffekte können je nach Board abweichen.
- Standbild (`0x01`) und Linkslauf (`0x02`) bleiben erhalten. Server validiert die erlaubten Modusnamen und protokolliert den Parameter als Hexwert. Moduswechsel selbst sind kurze BLE-Steueraufträge ohne neuen Bild-Upload.
- HTTP-Startseite liefert `Cache-Control: no-store` gegen veraltetes Safari-HTML. Laufzeitdiagnose unterscheidet Helligkeit, Tempo und Moduswechsel auch bei der `Auftrag:`-Ausgabe; der exportierte Diagnose-TXT-Dateiname wird aus `FW_VERSION` gebildet.
- Firmware/HTML sichtbar `v0.3.4`; neuer separater Sketchordner `firmware/RavestickBoard_v0_3_4/`. Aktualisierter `web/index.html`, Testplan, README, Roadmap, technische Dokumente und Entwicklungsstand.

### Unverändert
- Das in v0.3.3 erfolgreich hardwaregetestete datenbasierte Lauftextlimit (vorläufig 30 KiB), zwei Mini-Zeilenfarben, Glyphen/Symbole und gespeicherte NVS-Seiten. **Keine Migration und kein Datenverlust beabsichtigt.**
- Neue Textuploads wählen weiterhin ihre eigenen Textmodi; die zusätzlichen Werte werden nur über das separate Live-Dropdown getestet.
- Keine Bibliothek, keine neuen Symbole, kein Animationsupload und keine verketteten Programme.

### Prüfen auf echter Hardware
- Arduino-IDE-Kompilierung, WLAN-Start, bestehende sieben Blockseiten und Übertragung eines kurzen Texts.
- Modusparameter `0x01` bis `0x08` einzeln: Beobachtung auf der Tafel, BLE-Log, Prozentanzeige und Rückkehr zu bekannten Modi. Keine tatsächliche Effektunterstützung für `0x03`–`0x08` vorab behaupten.
- **Ein BLE-JOB-OK bestätigt nur die Übertragung/Notification, nicht die Bildanzeige.** Das vorige v0.3.3 ist die Rückfallversion.

---

## Historischer Zwischenstand · Arbeitstitel „v0.3.3 Korrektur 1“ (nicht eigenständig veröffentlicht)

- Live-Modus-Dropdown um Referenz-Moduswerte `0x03`–`0x08` erweitert: rechts, aufwärts, abwärts, Schneeflocke, Bild, Laser. Bereits auf unserer Tafel getestete Werte `0x01` (Standbild) und `0x02` (Linkslauf) bleiben erhalten. **Alle neuen Werte noch ungetestet.** Modus wird weiterhin ausschließlich über den eigenständigen BLE-Steuerbefehl `0x06` ohne Bildneuübertragung geändert; der neue Textupload bleibt wie gehabt mit Linkslauf.
- Firmware validiert Modusnamen serverseitig und protokolliert den tatsächlich gesendeten Hexwert. Startlog unterscheidet Tempo, Modus und Helligkeit. HTML-Startseite sendet `Cache-Control: no-store` gegen veraltete Safari-Oberflächen.
- **Hardwaretests der ursprünglichen v0.3.3 erfolgreich** (noch **nicht** der vorliegenden Korrektur 1): WLAN, NVS-Seiten (7), mobile Palette, fünf Fonts, zweizeiliger Mini-Lauftext mit zwei Farben, datenbasierter Zähler; 650 Zeichen / 5.097 Spalten / 30.689 Bytes real dargestellt. Gemischte 301-Zeichen-Folge bei 30.695 B erfolgreich. Grenzwert 301 × ⚠️ (30.803 B) wird im Editor blockiert. Im seriellen Log 240 quittierte BLE-Pakete bei 30.695 B, Dauer 52.937 ms; freier Heap nach WLAN 93.352 B, größter Block 81.908 B. In diesem Log keine `JOB FEHLER`-Zeilen.
- **Firmwarestatus zum Zeitpunkt dieser Notiz:** Moduserweiterung noch nicht mit Arduino kompiliert oder am Board getestet. **Anschließend als v0.3.4 veröffentlicht**, ohne das bestätigte v0.3.3 umzubenennen; NVS-Format unverändert.

## v0.3.3 — Ursprünglicher Testkandidat (25.10.2026)

**Nachträglich an realer Hardware erfolgreich getestet.** Die ursprünglich als ausstehend bezeichneten Tests wurden am 25.10.2026 durchgeführt (siehe Korrektur-1-Eintrag oben).

- Experimentelles **datenbasiertes Lauftextlimit** statt 600-Zeichen-Begrenzung; berechnete Original-Glyphen-/Symbolbreiten, Bytes einschließlich 107-Byte-Header und Prozentbalken live im Webeditor. Vorläufiges 30-KiB-Limit, keine automatische Textkürzung; Überschreitung rot/fett, Senden gesperrt.
- UTF-8-Lauftext wird vom ESP32 gestreamt und ohne globale 600-Zeichen-Arrays gerendert. Datenlimit auch in HTTP-Validierung und Bildrendering geprüft; blockseitenbezogene Datenstruktur und NVS-Schema unverändert.
- Mini Zweizeilig im Lauftext: unabhängige Schriftfarben für obere und untere Zeile, auch in der 64×16-Pixel-Vorschau.
- `web/index.html` und `RavestickBoard_WebUI.h` synchronisiert; aktualisierte README/Dokumentation und neuer reproduzierbarer Testplan.
- **Nicht enthalten:** Bibliothek, Lauftext-Programmeditor, Verkettung mehrerer Lauftexte in einer BLE-Übertragung. Diese Ideen sind für Folgereleases dokumentiert.

**Risikohinweis:** 30 KiB ist kein gesichertes CoolLEDX-Limit. Tests von v0.3.2 zeigten 29.837 B erfolgreich, 31.853 B ohne Anzeige trotz BLE-Erfolg. Umfangreichere Daten brauchen ggf. mehr zusammenhängenden RAM, als zum Sendezeitpunkt verfügbar ist.

## [0.3.2] · 25.10.2026 · Mobile-Bedienung und experimenteller Langtext

**Hardwaretest-Nachtrag · 25.10.2026, ca. 11:45 Uhr MEZ:** **Korrektur 2 wurde vom Nutzer erfolgreich kompiliert, geflasht und auf ESP32-C3 + CoolLEDX getestet.** Der zuvor fehlgeschlagene WLAN-AP startet wieder; NVS-Blockseiten blieben erhalten. Mobile Symbolpalette, Mehrfach-Einfügen ohne Tastatur, alle fünf Lauftextschriften, synchroner Mini-Zweizeilen-Lauftext, leere neue Eingaben und rot/fetter Zähler ab 601 bestätigt. **600 normale Zeichen werden auf der Tafel dargestellt** (27.461 B, 215 BLE-Pakete, 54,3 s). 300 gemischte Symbole funktionieren (29.837 B), 320 gemischte Symbole nicht (31.853 B); 320 Blitze funktionieren (25.061 B), 320 Warnsymbole nicht (32.741 B). Anzeigefehler werden trotz vollständig quittierter BLE-Pakete teilweise als `JOB OK` gemeldet. Bei 514 bis 600 sehr breiten Symbolen scheitert teils bereits die RAM-Allokation. **Keine feste Tafeldatengrenze bestätigt.** Zwei Zeilen in Mini-Lauftext haben noch keine getrennte Farbauswahl. **Ideen, nicht implementiert:** datenbasierte Live-Kapazität statt Zeichenlimit und ein aus Bibliothekseinträgen zusammengefügtes Laufband als einzelner BLE-Upload. Details: [`docs/ENTWICKLUNGSSTAND_v0.3.2_2026-10-25.md`](docs/ENTWICKLUNGSSTAND_v0.3.2_2026-10-25.md).

**Leseregel:** Die nachfolgenden Hinweise „noch nicht getestet“ kennzeichnen den damaligen *Auslieferungsstand vor diesem Hardwaretest*, nicht den heutigen Status von Korrektur 2.

**Korrektur 2 (25.10.2026, WLAN/RAM):** Nutzer hat Korrektur 1 erfolgreich kompiliert und geflasht, der WLAN-AP konnte jedoch wegen fehlgeschlagener WLAN-RAM-Allokation nicht starten (`wifi:alloc pp wdev funcs fail`, Folgefehler `0x3001`). Der Quelltext hatte zwei dauerhaft reservierte Bildpuffer von zusammen **122.507 Bytes** (`61.200 + 61.307`). Jetzt wird nur ein auf die tatsächlich gerenderte Bildbreite dimensionierter Payload-Puffer unmittelbar vor dem Versand angelegt (RGB-Daten liegen direkt ab Byte 107) und auf sämtlichen Erfolgs-/Fehlerpfaden wieder freigegeben. Kleinere Stackpuffer beim Blockseitenrendering und zusätzliche Heap-Diagnose beim Start. Der Webserver wird bei fehlendem AP nicht irreführend als bereit gemeldet. **Zum Zeitpunkt der Auslieferung von Korrektur 2 stand der Arduino-/Hardwaretest noch aus; dieser wurde am 25.10.2026 anschließend erfolgreich vom Nutzer durchgeführt (siehe Nachtrag oben).** Experimentelle 600-Codepoint-Grenze und NVS-Format bleiben unverändert. Bei sehr langen Symbolketten kann die dynamische Allokation dennoch scheitern; dann wird nicht gesendet.

**Korrektur 1 (25.10.2026):** Compilerfehler in `renderText()` behoben: `drawInline()` erhielt versehentlich einen zusätzlichen Skalierungsparameter. Die Skalierung wird bereits innerhalb von `drawInline()` aus `layout` bestimmt. Keine Änderung an NVS, BLE-Protokoll oder Webinterface. **Zum Zeitpunkt von Korrektur 1 stand die Arduino-Neukompilierung noch aus; sie erfolgte anschließend erfolgreich.**

**Historischer Auslieferungsstatus vor Hardwareprüfung:** JavaScript-Syntax und mobile Chromium-Browser-Smoketests geprüft (375px, 351px Dialog); die Arduino-Kompilierung und der LED-Hardwaretest waren zu diesem Zeitpunkt noch ausstehend. Grundlage: v0.3.1, am 25.10.2026 vom Nutzer erfolgreich kompiliert und praktisch getestet.

- Mobile Symbolpalette: **fehlendes schließendes HTML-`div` behoben**. Dadurch gehören Diagnose/Status nicht länger zum Overlay, die Palette hat wieder volle nutzbare Dialogbreite. Mehrere Symbole ohne erneutes Öffnen einfügen; beim Einfügen kein automatisches Fokussieren des Textfelds und kein Tastatur-Popup.
- Lauftext-Schriftauswahl und Blockseiten-Auswahl einheitlich: Standard, Standard Bold, Engschrift, Mini Einzeilig, Mini Zweizeilig. Layout-Codes und gespeicherte NVS-Struktur bleiben unverändert.
- Experimenteller synchron zweizeiliger Mini-Lauftext mit zwei Eingabefeldern und gemeinsamem horizontalen Scrollen.
- Neue Blockseiten und neue Browserentwürfe ohne Demo-Feldwerte; bestehende gespeicherte Seiten bleiben erhalten. Browserseitige lokale Entwürfe werden weiterhergestellt.
- Experimenteller Controller-Puffer für bis zu 600 Unicode-Codepoints, rote/fette Zählerdarstellung ab 601; Texte werden nicht gekürzt. Über 600 wird nicht gesendet, ohne Popup.
- **Protokoll-Risiko:** Die bekannten 80 Format-Slots und das Ein-Byte-Zeichenfeld des BLE-Headers wurden nicht reverse-engineert. Bei mehr als 80 Zeichen wird das Header-Feld auf 80 begrenzt, die vollständige Pixelmatrix aber übertragen. Ob die Tafel längere Inhalte annimmt, ist unbekannt. Lange Symbolketten können große BLE-Paketzahlen und lange Sendezeiten verursachen. Test bei 80, 100, 150, 200, 300, 400, 500, 600 schrittweise.
- Web-Mirror aktualisiert; bestehende Firmwareversionen und gespeicherte Blockseiten bleiben unangetastet.

**Offen:** Mobile Safari-Test, Arduino-Build, BLE-Langtext, iOS-Overlay, zweizeiliger Lauftext und Persistenztest.


## [0.3.1] · 24.10.2026 · begrenzter Korrektur- und Testkandidat

**Nachtrag 25.10.:** Vom Nutzer erfolgreich mit Arduino IDE kompiliert und auf Hardware getestet. 40/60/80 Zeichen und lange Symbolketten übertragen; bestehende Seiten erhalten. UI-Probleme der Symbolpalette, verbleibende Demotexte und fehlende Lauftext-Schriftwahl in v0.3.2 aufgenommen.

**Status:** Quelltext erstellt, JavaScript-Syntax und zentrale Browser-Bedienabläufe mit simulierten API-Antworten geprüft. **Noch keine vollständige Arduino-/ESP32-Kompilierung und kein Hardwaretest**; Version nicht als abgenommen markieren. Ausgangsbasis ist das auf echter Hardware bestätigte v0.3.0-dev.

### Neu / geändert
- `EDT-11`, `QA-02`: Experimentelles 80-Zeichen-Limit für Lauftext statt 40; Pixelpuffer auf bis zu 1.360 Spalten ausgelegt. Die 80 Slots des bisherigen Protokollkopfs werden nicht überschritten. Die Tafelkapazität muss real geprüft werden, insbesondere bei vielen 16×16-Piktogrammen.
- `LIB-11`, `EDT-22`: Demotext „Grüße!“ als voreingestellten Feldwert entfernt; bisheriger Lauftextentwurf bleibt im Browser gespeichert und wird wiederhergestellt.
- `UI-03`, `SYM-02`: Symbolauswahl als Overlay neben jedem Textfeld, mit Einfügen an der gespeicherten Cursorposition.
- `EDT-15`: Rückgängig/Wiederholen für **Textänderungen und Symbol-Einfügen** in Lauftext sowie Zeile 1/2, jeweils lokaler Verlauf von bis zu 100 Zuständen pro Feld. Farbwechsel/Seitenoperationen ausdrücklich noch nicht rückgängig.
- `FNT-19`: Leerzeichen bei Mini ein- und zweizeilig schmaler (2 + 1 statt 5 + 1 Pixel). Vorschau und Controller rechnen einheitlich.
- `PRE-07`, `UI-04`: Vorschau beim Scrollen sticky; redundanter Helligkeits-Button entfernt. Vier vorrangige Fontnamen aufgeräumt; bestehende Engschrift bleibt als Kompatibilitätsoption verfügbar.

### Unverändert / zurückgestellt
- Firmwareordner v0.3.0 und alle früheren Stände unberührt.
- NVS-Struktur der bis zu zwölf Blockseiten **nicht verändert**, bisherige Seiten bleiben erhalten.
- Keine individuelle Zeilenausrichtung, keine neuen Symbole, kein überarbeiteter Bananenentwurf, keine vollständige Bibliothek, kein erweitertes Zeichenformatierungs-UI.

### Prüfung
- JavaScript durch Node.js `--check` geprüft; Chromium-Smoketest im simulierten Webumfeld: Leerstarter, Textzähler, Undo/Redo, Symbol-Einfügung an Cursorposition und Undo, Blockseitentext-Undo. Kein echter BLE-/USB-Test.
- Offene Praxisprüfung: Arduino-Kompilierung, UI auf iPhone/Safari, 40/50/60/80-Zeichen-Übertragungen, viele Symbole, Helligkeit, Persistenz alter Seiten, Speicher und BLE-Diagnose.

---


## [0.3.0-dev] · 24.10.2026 · Entwicklungs- und Browser-Testkandidat

**Noch nicht mit Arduino-ESP32 3.3.12 kompiliert und nicht auf dem ESP32 oder der CoolLEDX-Tafel geprüft. Nicht als hardwarebestätigte Version kennzeichnen.**

- 82 vollständige Originalglyphen **Normal**, 82 **Bold** aus dem Originalglyphenarchiv v02 in Flash-Daten vorbereitet. Pixelvorschub/Abstände experimentell.
- 24 Originalsymbole aus dem Herstellerarchiv v01 (16×16 und sieben Farben), dazu eine unabhängige, vertikal gespiegelte RavestickBoard-Banane. Symbole in Texte einfügbar; für unterschiedliche effektive Breiten wird die tatsächlich beleuchtete Breite benutzt, plus vorläufig 1 Pixel Abstand.
- Herz in sieben Farben, durch Emoji-Farbvarianten auswählbar und unabhängig von der normalen Textfarbe.
- Mini, eine Zeile: 7 px hoch, horizontal und vertikal zentriert. Mini in zwei Zeilen bleibt erhalten; kleine Buchstaben werden in der Anzeige zu Versalien, ohne den gespeicherten Text zu verändern.
- Separate BLE-Kommandos für Helligkeit (`0x08`), Tempo (`0x07`) und Anzeigeart (`0x06`), kurzzeitige Verbindungswiederverwendung. Wirkung/Antwort noch nicht hardwaregetestet.
- Browserseitige Wiederherstellung von Lauftextentwurf und zuletzt verwendetem Editor-Reiter.
- NVS-Datensatzformat der bestehenden 12 Seiten bleibt unverändert; neue Layoutwerte 2–4 werden ergänzt.
- Kein `0x04`-Animationsupload, keine LittleFS-Programmbibliothek und keine vollständige Zeichen-für-Zeichen-Formatierung in diesem Kandidaten.
- Browser-Smoketest ohne JavaScript-Ausnahme; vollständige Arduino-/Hardwareprüfung ausstehend.

---


> **Dokumentationsnachtrag 23.10.2026:** Das frühere v0.2.0-Paket bezeichnete Hardwaretests noch als ausstehend. Diese wurden nach **Korrektur 1** vom Nutzer tatsächlich durchgeführt. Historische „zu testen“-Listen in älteren Versionsabschnitten bleiben als damalige Planung erhalten und sind keine aktuellen Offenstände.

### [0.2.0] Praxisabnahme · 23.10.2026

- **13/14** ursprüngliche Testpunkte vom Nutzer mit „Ja“ beantwortet; die einzeilige Darstellung blieb unbewertet.
- **Bestätigt:** Start, iPhone-Webinterface, Seite hinzufügen/duplizieren/verschieben, unterschiedliche Farben, Mini in zwei Zeilen, automatische Seitenfolge, individuelle Anzeigedauer, NVS-Persistenz nach Neustart und wiederholte Uploads.
- **Bekannter Mangel:** Der ESP32 lädt bei jedem Seitenwechsel den gesamten Inhalt erneut hoch. Die tafelseitige **Prozentanzeige** erscheint jedes Mal. Der Player wird deshalb als technische Vorstufe einer späteren **Playlist zwischen Programmen** betrachtet.
- **Schriftentscheidungen:** Zwei Zeilen in der aktuellen Großbuchstaben-Mini beibehalten; Kleinbuchstaben in Mini künftig automatisch als Versalien rendern; „Mini, eine Zeile“ ergänzen; `ß/ẞ` mit Entfernen des blau markierten Pixels; `g/p/q/y` an gemeinsame x-Höhe und Unterlängen anpassen.
- **Editor-Beobachtung:** Gespeicherte Blockseiten bleiben bei Browser-Reload erhalten; **Lauftextentwurf und zuletzt gewählter Reiter nicht**. Bibliothek/Entwurfswiederherstellung geplant.
- **Dokumentationsentscheidung:** Kurze Excel-kopierbare Testtabelle zu jedem späteren **Firmwarepaket direkt im Chat**, keine zusätzliche Download-Datei.

---

## [0.2.0] – 23.10.2026 (Korrektur 1, auf echter Hardware getestet)

### Korrektur 1 · 23.10.2026 (Kompilier-Hotfix)
- HTML und JavaScript aus dem `.ino` in `RavestickBoard_WebUI.h` ausgelagert, um fehlerhafte automatische Arduino-Funktionsprototypen (`function does not name a type`) zu verhindern.
- `tools/sync_web.py` aktualisiert jetzt die Headerdatei.
- Sketch und Header müssen gemeinsam im gleichen Arduino-Sketch-Ordner liegen.
- Sonst keine beabsichtigten Funktionsänderungen; reale ESP32-Kompilierung wurde anschließend vom Nutzer bestätigt (23.10.2026).


### Neu
- Getrennte Bereiche für Lauftext und Blockanzeige.
- Bis zu zwölf eigenständige Seiten mit je einer 14-Pixel-Zeile oder zwei 7-Pixel-Zeilen, eigenständigen Zeilenfarben, Ausrichtung und Anzeigedauer.
- Seiten hinzufügen, duplizieren, verschieben, löschen, speichern und einzeln hochladen.
- Nichtflüchtige Speicherung des Seitenprogramms im NVS, ohne automatische Wiedergabe nach dem Start.
- Experimenteller ESP32-Seitenplayer mit unabhängigen BLE-Uploads, manueller Start-/Stoppsteuerung und Fehlerabbruch.
- Vorschau mit festen 64×16 LED-Punkten, seitenweiser Abfolge und 1×/2×/4× Fast Forward.
- Großes ẞ zusätzlich zum kleinen ß, mit bewusst gleicher komprimierter Glyphenform.
- Diagnose-Zähler für Seitenübertragungen und Speichervorgänge.

### Korrekturen und Entscheidungen
- Im Seiteneditor keine gekappten Buchstaben und kein automatischer Seitenumbruch.
- Verschiedene Seiten werden unabhängig mit ihren eigenen Parametern gespeichert.
- Die zuvor gefundene Arduino-Typ-Prototyp-Falle (Job) bleibt behoben.
- Firmwarequellcode und `web/index.html` durch `tools/sync_web.py` synchronisierbar.

### Noch offen
- **Erledigt am 23.10.2026:** ESP32-Arduino-Kompilierung nach Korrektur 1 sowie BLE-/NVS-Grundtests. Offen bleiben spezielle Regressionen und Dauerbelastung.
- Farb- und Schriftwechsel pro Zeichen sowie Rekonstruktion der Herstellerfonts.
- Live-BLE-Regler für Helligkeit/Geschwindigkeit, ohne Verbindungsaufbau je Schritt.
- Prüfung nativer Mehrseitenfähigkeit der Tafel und verkürzter Umschaltzeiten.
- Schriftformen g, p, q, y, Ä, Ö, Ü gemäß Original-Referenzen verfeinern.

---

# Änderungs- und Testhistorie

**Prinzip:** Tatsächlich getestete Funktionen und geplante Änderungen getrennt führen. Eine neue Versionsnummer bezeichnet eine bewusst archivierte Firmware, nicht nur eine Idee.

## [0.1.2] · 22.10.2026 · Nach Compilerkorrektur auf ESP32 gestartet; detaillierte Glyphenprobleme später gesammelt

### Neu und verbessert (damals zunächst implementiert; teilweise später auf Hardware beobachtet)
- **Deutsche Oberfläche und Groß-/Kleinschreibung:** alle UI-Texte deutsch und mit Umlauten/ß; 26 echte Kleinbuchstaben und ä/ö/ü zusätzlich zum bisherigen Zeichensatz (`FNT-02`).
- **Font aus einer Quelle:** Das Webinterface bezieht dieselbe Engschrift-Glyphentabelle wie der ESP32 über `/api/font`.
- **LED-Vorschau als Punktmatrix:** stets genau 64×16 runde LEDs, auch bei langem Text; keine automatische Verkleinerung (`PRE-01`, `PRE-02`, `PRE-03`).
- **Vorschau-Player:** Play/Pause, Neustart, 1×/2×/4×/8× (`PRE-04`). Browsergeschwindigkeit vorläufig angenähert, noch kein echter Abgleich.
- **Anzeigemodus wählbar:** Standbild oder Lauftext unabhängig von Textlänge; lange Standbilder zeigen einen Ausschnitt und eine Überlaufwarnung (`EDT-01`, `EDT-09` teilweise). Seiteneditor bleibt offen.
- **Laufgeschwindigkeit:** UI-Regler (1–100), sendet dokumentierten CoolLEDX-Speed-Befehl `0x07` mit umgerechneten Rohwerten 0–255. Unkalibrierter Hardwaretest (`EDT-10` teilweise).
- **Diagnose kopieren:** direkter Clipboard-Versuch, Fallback und manuell auswählbarer Volltext; vorhandener TXT-Download bleibt (`DIA-10`).
- **Resetgrund / Speicher:** `USB-Reset` statt Code 11; freie App-Bytes über aktive Firmwarepartition statt der bisher irreführenden `getFreeSketchSpace()`-Angabe.
- **BLE-Fehlerfall:** Hinweise auf konkurrierende Hersteller-App, Verbindungsversuch mit auf 10 s gesetztem Timeout (muss real getestet werden).

### Intern geprüft
- JavaScript mit Node.js auf Syntax geprüft.
- C++ mit Arduino-API-Dummys syntaktisch geprüft; **keine echte Arduino-Kompilierung**.
- Webinterface bei 390 px Breite in Chromium mit simulierten API-Antworten geprüft: Umlaute/Zählung, Standbild/Lauftext, Player, Diagnose, Kopieren.
- Fontdefinition für Groß-/Kleinbuchstaben einschließlich `ÄÖÜäöüß` automatisiert überprüft.

### Zum damaligen Zeitpunkt noch nicht bestätigt
- ESP32-Kompilierung mit Espressif 3.3.12 und reale Flash-Belegung; **inzwischen nach Korrektur bestätigt**.
- Tatsächliche Zeichendarstellung von Groß-/Kleinbuchstaben und Umlauten auf der Tafel.
- Tatsächliches Lauftext-Tempo und Richtung; Einfluss der Rohwertskala.
- Clipboard-Fallback auf iPhone/Safari unter unverschlüsseltem `http://192.168.4.1`.
- Statischer Überlauf: tatsächlicher Ausschnitt auf dem Board kann noch von der Simulation abweichen.
- BLE-Timeout, Stabilität und Heap bei mehreren Aufträgen.

## [0.1.1] · 22.10.2026 · Auf Hardware bestätigt

### Nachträglich am Gerät geprüft
- WLAN-Webinterface und Header mit v0.1.1 auf iPhone sichtbar.
- Diagnoseansicht und TXT-Export funktionieren auf iPhone.
- Reale BLE-Diagnose: fünf Textaufträge, fünf erfolgreiche Verbindungen, fünf erfolgreiche Jobs, 20 Notifications und keine Timeout-/Schreibfehler in diesem Test.
- Erfolgreicher erneuter BLE-Auftrag nach Freigabe einer Bluetooth-Verbindung durch die Original-App. Während die Original-App verbunden blieb, scheiterte der Auftrag wie erwartet nach ca. 30 Sekunden.
- Von RavestickBoard hochgeladene Inhalte können über die Original-App anschließend in Wiedergabemodus und Helligkeit verändert werden.

### Noch nicht getestet
- Mehrstündige Speicherstabilität; Fehlerhistorie nach Stromverlust (RAM-Ringpuffer wird zurückgesetzt).


### Implementiert (Code erstellt; Gerätetest ausstehend)
- **Rebranding:** Name „RavestickBoard for CoolLED1248“ und sichtbare Versionsnummer `v0.1.1` im Header. WLAN-SSID `CoolLED1248` und bisheriger Verbindungsweg bleiben bewusst unverändert.
- **Diagnose:** aufklappbare Systemdaten im Webinterface, inklusive Uptime, Resetgrund, freiem/minimalem Heap, größtem Speicherblock, Sketchgröße und freiem App-Platz.
- **Log-Export:** TXT-Download unter `/api/diagnostics.txt`, JSON-Status unter `/api/diagnostics`, fester RAM-Ringpuffer für 56 Ereignisse; keine dauernden Flash-Schreibvorgänge.
- **BLE-Metriken:** Verbindungsversuche, Erfolge, Disconnects, lokale BLE-Write-Fehler, Notifications, Timeouts, Paket- und Bytezähler, letzter fehlgeschlagener Auftrag.
- **Mobile UI:** sieben Farben in einer Rasterzeile, Helligkeitswert mit `white-space:nowrap`, zusätzliche Scrollreserve für Safari-Toolbar und iPhone-Safe-Area.
- **Datensparsamkeit:** Keine Texte, Passwörter oder Geräteadressen im TXT-Bericht. Fehlerverlauf seit Systemstart, keine über Neustarts persistierte Historie.

### Intern geprüft
- JavaScript syntaktisch mit Node.js geprüft; HTML/CSS als mobile Chromium-Ansicht gerendert (keine ESP32-Verbindung).

### Ursprüngliche Testcheckliste (danach teilweise bestätigt)
- Arduino-Kompilierung und Flash unter ESP32 Core 3.3.12 mit `No OTA (2MB APP/2MB SPIFFS)`.
- WLAN-Webseite auf iPhone/Safari, v0.1.1 sichtbar, Diagnose und Download geprüft.
- Textübertragung, Farben und separate Helligkeit gegenüber v0.1.0 regressionsgeprüft.
- Erfolgreicher und fehlgeschlagener BLE-Auftrag erscheinen nachvollziehbar im Download.
- Heap und größte Blockgröße nach wiederholten BLE-Aufträgen; Reset löscht das Ereignislog.

### Unveränderte Einschränkungen
- Vorschau langer Texte noch nicht pixelgenau, keine Kleinbuchstaben, Auto-Lauftext noch aktiv.
- Diagnose im RAM verliert Ereignisse bei Neustart. Keine vollständige Ursache für Fremd-BLE-Steuerung erkennbar.

## [Historische Planung vor v0.2.0] · ursprüngliche Vorschläge ab 22.10.2026

### Geplant
- Nach realem v0.1.1-Gerätetest Diagnosefunktionen als bestätigt markieren; später Diagnose-Persistenz prüfen (`DIA-*`).
- Vorschau als echte 64×16-LED-Punktmatrix, Lauftext-Simulation, 1×/2×/4×/8× und Vorspulen (**8× später ausdrücklich verworfen; aktuell 1×/2×/4×**) (`PRE-*`).
- Mobile-UI-Korrekturen aus v0.1.1 auf realem iPhone/Safari verifizieren (`UI-*`).
- Explizite Wahl zwischen Lauftext und statischen Seiten, keine Längenautomatik (`EDT-*`).
- Ergänzen echter Kleinbuchstaben für Engschrift (`FNT-*`).

### Beobachtungen
- Aktuelles Webinterface skaliert bei langen Texten die ganze Darstellung statt auf feste 64×16 Pixel zu begrenzen.
- Aktuelle Engschrift besitzt keine echten Kleinbuchstaben; sie werden auf dem Controller in Großbuchstaben umgewandelt.

## [0.1.0] · 22.10.2026 · Erster funktionsfähiger Webcontroller

### Neu
- Eigenes WLAN `CoolLED1248` mit Offline-Webinterface unter `192.168.4.1`.
- Textfeld, 7 Einzelfarben, Helligkeitsregler, einfache Textvorschau.
- Bluetooth-Übertragung über CoolLEDX (`FFF0`/`FFF1`).
- Einzeilige 5×7-Engschrift, vertikal verdoppelt auf der 16-Pixel-Matrix.
- Automatischer Wechsel auf Laufschrift bei Textbreite über 64 Pixeln (temporäre Prototyp-Regel).
- Kein automatisches Überschreiben des Tafel-Inhalts beim Start.

### Geprüft am realen Gerät
- Smartphone verbindet sich mit ESP32-WLAN.
- Weboberfläche ist auf iPhone/Safari erreichbar.
- Text `TEST 123` gelangt vom Webinterface zur Tafel und erscheint passend zur Vorschau.
- Farbwechsel über das Webinterface funktioniert nach Nutzer-Rückmeldung.

### Bekannte Einschränkungen
- Bislang keine lokale Bibliothek, keine Textseiten, keine gemischten Fonts/Farben, keine Diagnose zum Herunterladen.
- Nur Großbuchstaben; 40 Zeichen Eingabelimit; fehlerhafte Vorschau-Skalierung langer Texte.
- Mobile Darstellung: Farbe Weiß in zweiter Zeile; Helligkeit `100 %` umgebrochen; Browserleiste überdeckt Bedienelemente.
- BLE-Adresse in der Firmware fest einprogrammiert; keine robuste Wiederverbindung nach fremder App-Steuerung getestet.

### Build-Konfiguration
- Arduino IDE 2.3.10; esp32 by Espressif 3.3.12; ESP32C3 Dev Module.
- Erste Kompilierung mit 1.310.720 B App-Maximum scheiterte: 1.312.457 B gebraucht, 1.737 B zu viel.
- Partitionierung auf `No OTA (2MB APP/2MB SPIFFS)` umgestellt; erfolgreicher Webbetrieb danach bestätigt.

## [Experiment: FirstText] · 22.10.2026 · BLE-Pixelübertragung
- Eigene mehrfarbige Anzeige `FUCK YES` erfolgreich auf die Tafel übertragen.
- Engschrift: 5×7-Pixelgrundform, auf 14 Pixel Höhe verdoppelt.
- Protokollbeispiele aus Community-GitHub-Projekten als technische Grundlage untersucht.

## [Experiment: BLE-Bring-up] · 22.10.2026 · Verbindung und Helligkeit
- ESP32-C3 erstmals per USB-C programmiert, Ausgabe im seriellen Monitor erfolgreich.
- Gerät `CoolLEDX` gefunden, Service `FFF0`, Characteristic `FFF1` ausgelesen.
- Schreibtyp `Write Without Response`; Benachrichtigungen verfügbar.
- Helligkeitsbefehl mit Maximalwert `0xFF` sichtbar erfolgreich getestet.
- LED-Tafel speichert bereits hochgeladene Inhalte über Stromverlust hinweg (Nutzerbeobachtung).

## Vorlage für die nächste Version

```markdown
## [x.y.z] · YYYY-MM-DD
### Neu / Geändert / Behoben
- ... (Themen-ID)
### Real getestet
- Gerät, Szenario, Ergebnis ...
### Noch offen / bekannte Fehler
- ...
### Build
- Arduino-/ESP32-Version, Partition, Speicherverbrauch, Besonderheiten
```
