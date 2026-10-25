# RavestickBoard – Entwicklungsstand v0.3.4

**Stand:** 25. Oktober 2026  
**Build:** v0.3.4 – erweiterte direkte Anzeigemodi  
**Status:** Neuer Releasekandidat; **noch keine vollständige Arduino-Kompilierung oder Hardwareprüfung dieser Version**.  
**Rückfall:** Original-v0.3.3, am 25.10.2026 erfolgreich auf ESP32-C3 und CoolLEDX 64×16 geprüft.

## Warum eine eigene Version?

v0.3.3 läuft im realen Test stabil und hat das Datenbudget statt des Zeichenlimits sowie verschiedene Farben im Mini-Zweizeiler erfolgreich eingeführt. Die sechs zusätzlichen Anzeigemodi sind **ein neues Feature**, keine Fehlerkorrektur. Der interne Arbeitstitel „v0.3.3 Korrektur 1“ wurde daher **vor Veröffentlichung** in **v0.3.4** überführt; der alte Sketch bleibt unangetastet.

## Was v0.3.4 hinzufügt

Das vorhandene Dropdown „Anzeigemodus ohne neuen Upload“ löst einen direkten BLE-Steuerbefehl **`0x06` mit variierendem Modusparameter** aus. Es sind also acht *Werte desselben Befehls*, nicht acht verschiedene Kommandos. Die Referenzbezeichnungen stammen aus [coolledx-driver](https://github.com/UpDryTwist/coolledx-driver/blob/main/src/coolledx/__init__.py).

| Parameter | Referenzbezeichnung | Eigene 64×16-Tafel | LED-Reaktion | Prozentanzeige |
| --- | --- | --- | --- | --- |
| `0x01` | Standbild | Bereits bestätigt | | |
| `0x02` | Linkslauf | Bereits bestätigt | | |
| `0x03` | Rechtslauf | **Noch zu testen** | | |
| `0x04` | Aufwärts | **Noch zu testen** | | |
| `0x05` | Abwärts | **Noch zu testen** | | |
| `0x06` | Schneeflocke | **Noch zu testen** | | |
| `0x07` | Bild / Picture | **Noch zu testen** | | |
| `0x08` | Laser | **Noch zu testen** | | |

Zusätzlich: HTTP-Startseite mit `Cache-Control: no-store` gegen alte Safari-Inhalte; Moduswert als Hex im Diagnose-Log; getrennte Startmeldungen und `Auftrag:`-Labels für Helligkeit, Tempo und Modus; der Downloadname des Diagnose-TXT wird aus `FW_VERSION` gebildet.

**Beachten:** `0x04` als *Moduswert* des Steuerbefehls `0x06` (vermutlich „Aufwärts“) ist **nicht** gleichbedeutend mit einem eigenständigen Kommando `0x04` für gespeicherte Animationen. Dessen Erforschung ist ein anderes, nicht implementiertes Thema.

## Bereits an v0.3.3 bestätigte Grundlage (25.10.2026)

- Arduino-Kompilierung und Flash erfolgreich; WLAN `CoolLED1248`, Webinterface und bestehende sieben NVS-Blockseiten erhalten.
- Originalglyphen-/Symbolbreiten werden für eine live berechnete Pixel-/Byteanzeige verwendet; der sichtbare Zeichenzähler hat keine feste Grenze.
- **650 normale Zeichen:** 5.097 Spalten, **30.689 B**, erfolgreich angezeigt.
- **301 gemischte Zeichen und Symbole:** 5.098 Spalten, **30.695 B**, **240 BLE-Pakete**, `JOB OK` nach **52.937 ms**, tatsächlich angezeigt.
- Unterschiedliche Farben für beide synchron laufenden Mini-Zeilen, fünf Schriftarten, mobile Symbolpalette und wiederholte BLE-Verbindungen erfolgreich ausprobiert.
- Nach WLAN-Start im seriellen Log **93.352 B freier Heap, größter freier Block 81.908 B**; während des 30.695-Byte-Puffers **42.508 B frei**. Kein beobachteter `JOB FEHLER` im ausgewerteten Log.
- Bei **301 × ⚠️** (30.803 B) färbt sich die Datenanzeige rot und deaktiviert den Sendeknopf; Text bleibt erhalten. Eine erfolgreiche Darstellung oberhalb 30.720 B **wurde damit nicht geprüft**.

Das 30-KiB-Limit ist **unsere experimentelle Softwaregrenze, keine gemessene Obergrenze der Tafel**. Frühere Übertragungen von 31.853 B und mehr wurden teilweise zwar per BLE quittiert, aber nicht dargestellt; laut Nutzer gab es bei diesen Fällen **keine sichtbare Prozentanzeige am Board**. Ob Headerfeld, Flash-/RAM-Limit oder Bildformat dafür verantwortlich ist, ist offen. BLE-Quittung ≠ bestätigte Anzeige.

## Änderungen, die v0.3.4 ausdrücklich nicht vornimmt

- Kein Eingriff in Originalfirmware, Schutzfolie, Verkabelung oder Holzmontage der Tafel.
- Keine Änderung am NVS-Datenschema, kein Löschen gespeicherter Blockseiten, kein `Erase All Flash`.
- Keine neuen Symbole, kein Bibliothekseditor, keine verketteten Lauftexte, keine autonome Blockseitenplaylist.
- Kein allgemeinerer Modus bei Textupload; neue Modi werden nur als separater Steuerauftrag auf bereits hochgeladene Inhalte angewendet. Textupload wählt weiterhin Standard-Linkslauf bzw. Standbild.

## Kurzer Testablauf für v0.3.4

1. `firmware/RavestickBoard_v0_3_4/RavestickBoard_v0_3_4.ino` mit Arduino-ESP32 3.3.12 kompilieren und ohne Vollflash-Löschung hochladen.
2. Serienausgabe: **v0.3.4**, keine `wifi:alloc pp wdev funcs fail`-Meldung, bestehende Seiten geladen, Weboberfläche zeigt v0.3.4.
3. Einen kurzen Testtext senden und sichtbar bestätigen.
4. Dropdown nacheinander `0x01` bis `0x08` testen. Für jeden Wert protokollieren: **BLE-Job OK? Tatsächlicher Anzeigeeffekt? Prozentanzeige?**
5. `0x01` und `0x02` erneut auswählen; anschließend normale Blockseite anzeigen und NVS-Persistenz überprüfen.
6. Bei Auffälligkeiten serielles Log sichern; vor weiterer Änderung auf v0.3.3 zurückgehen. Kein NVS-Reset.

## Weiterführende Themen nach dem Modustest

- **Exakte Display-Kapazität:** Technisch kontrollierter Test oberhalb des vorläufigen 30-KiB-Budgets, ggf. Testbuild.
- **Echte autonome Animationen:** Separates Protokollformat untersuchen, nicht aus Modus `0x04` herleiten.
- **NVS-Datensicherheit:** Export, versionierte Migration vor Einführung einer Bibliothek.
- **Bibliothek:** Gespeicherte Einzelnachrichten; später kompatible Lauftexte in eine einzige Pixelmatrix verketten und nur einmal übertragen.
- **Neues Symbolpaket:** Drei aus Herstellerreferenzen nachzubauende Motive (Kussmund, Blume, Biene), sieben eigene beziehungsweise zu überarbeitende (Banane, Ente, Tropfen, Katze, UFO, UFO-Strahl, Alien), plus Stern-/Abstandskorrekturen.
- **Webserver während langer BLE-Uploads:** Nicht blockierende Auftragsverwaltung; Statusmeldungen statt Scheinerfolg.

## GitHub-Paket und Prüfstatus

Die drei Dateien im neuen Firmwareordner (`.ino`, WebUI-Header, Asset-Header), `web/index.html` und die aktualisierten Markdown-Dokumente bilden **ein vollständig direkt entpackbares Paket**. Alte Firmwareordner bleiben erhalten. Auslieferung über ZIP; GitHub wird **nicht direkt verändert**. Vollständige Arduino-Kompilierung und echter Hardwaretest von v0.3.4 stehen aus.
