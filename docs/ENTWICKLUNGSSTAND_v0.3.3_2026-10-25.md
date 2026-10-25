> **Abschlussnachtrag vom 25.10.2026:** Die ursprüngliche v0.3.3 wurde danach vom Nutzer erfolgreich kompiliert und hardwaregetestet (650 normale Zeichen / 30.689 B; 301 gemischte Zeichen / 30.695 B; separate Mini-Zeilenfarben; sieben erhaltene NVS-Seiten). Die im ursprünglichen Testkandidaten-Protokoll genannten offenen Hardwaretests sind damit historisch. Die anschließend vorgeschlagene Moduserweiterung erscheint **als eigene v0.3.4**, nicht als Bugfix für v0.3.3. Details: [`ENTWICKLUNGSSTAND_v0.3.4_2026-10-25.md`](ENTWICKLUNGSSTAND_v0.3.4_2026-10-25.md).

> **Nachtrag 25.10.2026 – v0.3.3 Korrektur 1:** Ursprüngliche v0.3.3 auf Hardware erfolgreich getestet. Im seriellen Log wurden nach WLAN-Start 93.352 B freier Heap und 81.908 B größter Block gemeldet; bei 30.695 B Bilddaten 240 BLE-Pakete, `JOB OK` nach 52.937 ms. 650 normale Textzeichen mit 30.689 B erfolgreich dargestellt; ein zweiter gemischter Text mit 301 Zeichen / 30.695 B ebenfalls. Sieben gespeicherte Seiten erhalten. Separater Mini-Zweizeilen-Farbwechsel, mobile Symbolpalette und Datenzähler bestätigt. Korrektur 1 ergänzt sechs ungetestete Anzeigemoduswerte `0x03`–`0x08` zum BLE-Befehl `0x06` ins Dropdown. Siehe `TESTPLAN.md` und `TECHNIK.md`. Korrektur 1 selbst noch nicht kompiliert/hardwaregetestet. Frühere Testkandidat-Aussagen unten sind historisch.

# RavestickBoard v0.3.3 – Testkandidat vom 25.10.2026

> **Status:** Quellcode und Webinterface erstellt; **keine Arduino-IDE-Kompilierung und kein Hardwaretest von v0.3.3 bestätigt**. Letzter tatsächlich an der CoolLEDX-Tafel getesteter Stand: **v0.3.2 Korrektur 2**. Die Aussagen zu diesem Stand stammen aus dem manuellen Test am 25.10.2026.

## Ziel und Umfang

v0.3.3 ist ein bewusst kleines Update auf Basis der hardwaregetesteten v0.3.2 Korrektur 2. Es betrifft **nur Lauftext**:

1. **Kein festes 600-Zeichen-Limit:** Die Eingabe bleibt vollständig erhalten. Der Controller liest die UTF-8-Zeichen bei der Berechnung und beim Rendern schrittweise ein, statt zwei globale 600-Zeichen-Puffer zu belegen. Die Länge ist faktisch durch die resultierende Pixelbreite, den freien zusammenhängenden RAM und den BLE-/Tafel-Datenrahmen begrenzt.
2. **Live-Auslastung im Webinterface:** Originalschriftbreiten, Mini/Engschrift und die tatsächliche beschnittene Breite der Icons werden verwendet. Anzeige: Zeichenanzahl **ohne Obergrenze**, Pixelspalten, berechnete Bytes und Prozent eines Datenbudgets. Ab Überschreitung **rot und fett**; eingegebener Text wird nicht gekürzt; Senden wird deaktiviert. Keine modalen Fehlermeldungen im Normalfall.
3. **Vorerst 30 KiB Datenbudget:** 30.720 Bytes einschließlich 107-Byte-Header, maximal 5.102 Spalten bei 6 Bytes pro Spalte. **Dies ist eine experimentelle Arbeitshypothese, keine nachgewiesene Obergrenze der CoolLEDX-Tafel.** Der Controller prüft denselben Wert erneut auf Serverseite. Zu große Aufträge werden vor BLE abgewiesen.
4. **Mini Zweizeilig als Lauftext:** Zwei separate Farben wie bei Blockseiten; beide Zeilen laufen weiter synchron. Bestehende einzeilige Farbwahl, Fonts, Entwürfe und die NVS-Seiten bleiben kompatibel. Hersteller-Symbole behalten ihre festen Pixel-Farben; die sieben Herzen sind wie zuvor eigene Farbvarianten.

**Nicht Bestandteil von v0.3.3:** Bibliothek, verkettete Lauftext-Programme, blockübergreifende zeitliche Programmsteuerung, neues BLE-Protokoll, dynamische Auto-Reconnect-Strategie oder OTA. Das kommt erst nach erfolgreichem Stabilitätstest in Frage.

## Bisherige Hardwarebeobachtungen aus v0.3.2 Korrektur 2

| Anzeigeinhalt | Effektive Datenmenge | BLE-Pakete | Ergebnis an LED-Tafel |
|---|---:|---:|---|
| 600 normale Textzeichen | 27.461 B | 215 | funktioniert |
| 270 gemischte Original-Symbole | 27.317 B | 214 | funktioniert |
| 300 gemischte Original-Symbole | 29.837 B | 234 | funktioniert |
| 320 × ⚡ | 25.061 B | 196 | funktioniert |
| 320 gemischte Original-Symbole | 31.853 B | 249 | keine Anzeige trotz BLE-OK |
| 320 × ⚠️ | 32.741 B | 256 | keine Anzeige trotz BLE-OK |
| 450 gemischte Symbole | 43.817 B | 343 | keine Anzeige trotz BLE-OK |
| 500 gemischte Symbole | 48.683 B | 381 | keine Anzeige trotz BLE-OK |
| 514–600 sehr breite Symbole | etwa 50–58 KB benötigt | – | RAM-Pufferallokation auf ESP32 gescheitert |

**Interpretation:** Nicht die Zeichenanzahl, sondern die tatsächliche gerenderte Breite beziehungsweise die daraus resultierende Datenmenge ist maßgeblich. Das exakte Tafel-Limit ist **weiter ungeklärt**. Möglich sind eine Grenze um 30 KiB oder um 5.120 Spalten; 30 KiB im neuen Editor sind daher ein vorsichtiger, experimenteller Arbeitswert. Werte knapp unter 30 KiB sind **nicht automatisch garantiert**.

Der bisherige CoolLEDX-Pixelaufbau besteht aus 107 Bytes Header und 6 Bytes je Spalte (16 Pixel hoch, 3 Farbkanäle mit je 2 Bytes). Die 80 bekannten Format-Slots des ursprünglichen Headers werden unverändert weiterverwendet. Der Erfolg einer BLE-Paketübertragung beweist **nicht**, dass die Tafel die Anzeige angenommen hat. Die Prozentanzeige erscheint weiterhin bei jedem neuen Upload; die Firmware der Tafel wird nicht verändert.

## Status 25.10.2026

- Repository-Hauptzweig geprüft: v0.3.2 Korrektur 2, GIF `docs/assets/board-gif.gif`, Totemfoto und neue README-Bildunterschrift vorhanden; historische Berichte archiviert.
- v0.3.2 kompiliert und auf echter ESP32-C3/CoolLEDX-Hardware getestet, WLAN/RAM-Reparatur erfolgreich; vorhandene NVS-Blockseiten erhalten.
- v0.3.3 ist ein **Testkandidat**. JavaScript-Syntax/Breitentabellen und simulierte Browser-Eingaben wurden überprüft. Arduino-Kompilierung, BLE-Übertragung und Board-Darstellung müssen noch erfolgen.
- Alte Browserentwürfe (`rb03-scroll-draft`, `rb032-scroll-line2`) bleiben bestehen; keine Änderung am NVS-Datenschema.

## Testplan für v0.3.3

| Nr. | Test | Erwartung / Beobachtung |
|---|---|---|
| 1 | `.ino` kompiliert in Arduino IDE mit den bisherigen ESP32-C3-Einstellungen | fehlerfrei; sonst vollständiges Compilerlog sichern |
| 2 | WLAN `CoolLED1248` startet, Webseite zeigt v0.3.3 | WLAN erreichbar, keine RAM-Init-Fehler |
| 3 | Gespeicherte Blockseiten und vorhandene Browserentwürfe | unverändert |
| 4 | Normaltext mit 600 Zeichen erneut senden | funktioniert wie in v0.3.2 |
| 5 | Normaltext mit mehr als 600 Zeichen (z. B. 650, 700, 750) | kein künstliches Zeichenlimit; Datenanzeige bestimmt Sendbarkeit |
| 6 | Fontwechsel Standard → Bold → Engschrift → Mini | Pixelspalten und Bytes reagieren passend; Vorschau stimmt |
| 7 | 300 × ⚠️ eingeben | theoretisch 30.701 Byte, knapp unter dem vorläufigen Limit; **Boardergebnis offen** |
| 8 | 301 × ⚠️ eingeben | theoretisch 30.803 Byte; Zähler rot/fett, Senden deaktiviert, Text erhalten |
| 9 | Mini Zweizeilig: Zeile 1 rot, Zeile 2 blau | Vorschau und LED-Tafel verschiedene Farben; synchrones Scrollen |
| 10 | Original-Icons und sieben Herzfarben | feste Ikonenfarben bleiben erhalten; farbige Herzen korrekt |
| 11 | Langtext mehrmals, Neustart, danach Blockseite senden | WLAN, BLE und Speicher stabil |
| 12 | Grenz-/RAM-Fehler im seriellen Monitor | kontrollierte Ablehnung, kein Neustart und keine Datenverluste |

Nach dem Test bitte nicht nur die Meldung `JOB OK`, sondern ausdrücklich **die tatsächliche Anzeige auf der Tafel** dokumentieren. Maximalgröße und Laufzeit getrennt notieren. Einen Text kann man ohne Kürzung aus dem Eingabefeld wieder zurücknehmen.

## Ausblick: Bibliothek und verkettete Lauftextprogramme

In einer späteren Bibliothek werden einzelne Nachrichten **separat gespeichert und bearbeitet**. Der Programmeditor könnte mehrere kompatible **Lauftexte** mit frei bestimmbaren Leer-Spalten zu einem einzigen Pixelstreifen rendern und dann **einmal** via BLE übertragen. Damit gäbe es die boardseitige Prozentanzeige nur beim Start des Gesamtprogramms statt zwischen jeder Lauftextnachricht. Die gesamte Programmbreite muss innerhalb der empirisch ermittelten Boardkapazität bleiben. **Echte Blockseiten mit individuellen Standzeiten und gemischte Anzeige-Modi** können nicht ohne Weiteres in demselben Streifen simuliert werden und benötigen separate Protokollrecherche. Das ist noch nicht implementiert.

## Rückfall und GitHub

Die funktionierende Firmware v0.3.2 Korrektur 2 bleibt im eigenen Ordner. Das v0.3.3-Paket enthält wieder `.ino`, `RavestickBoard_WebUI.h`, `RavestickBoard_Assets.h`, `web/index.html` sowie aktualisierte Doku. Im Arduino IDE den **v0.3.3-Sketch** öffnen. **Nicht** „Erase All Flash“ aktivieren, um NVS-Seiten zu behalten. Ältere Versionsordner nicht löschen. GitHub-Commit und Push erfolgen ausschließlich lokal durch den Nutzer via GitHub Desktop; die Assistenz verändert das Remote-Repository nicht.
