> **v0.4.3.1-dev · Korrektur 1 (30.10.2026):** Arduino-Compile-Fehler durch automatischen Prototyp fuer `rbAppendJsonText(RbDiagnosticStream&)` korrigiert. Die JSON-Escaping-Funktion ist nun eine Klassenmethode. Vollstaendiger Arduino-Compile auf Zielumgebung steht noch aus.

# RavestickBoard · Versionsgeschichte / Changelog

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
- **Prüfstand:** JavaScript-Syntax, Kompression/ETag und simulierte Browserabläufe auf drei Smartphonegrößen erfolgreich. **Nicht** mit Arduino/ESP32 kompiliert, **nicht** auf realer Hardware getestet. Historisches Heap-Minimum von 224 B ist noch nicht erklärt oder nachweislich behoben.

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
