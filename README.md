<p align="center">
  <img src="docs/assets/board-gif.gif" alt="Animierte Vorschau der CoolLED1248-LED-Tafel" />
</p>

<p align="center">
  <img src="docs/assets/totem.jpg" alt="CoolLED1248-Board an einem Festival-Totem (Ravestick)" />
</p>

<p align="center"><sub>CoolLED1248-Board an einem Festival-Totem (Ravestick).</sub></p>

<h1 align="center">🍌 RavestickBoard for CoolLED1248</h1>

<p align="center">
  <strong>Ein eigener ESP32-C3-Webcontroller für eine 64 × 16-Pixel-CoolLEDX-Tafel.</strong><br>
  Entstanden aus einer LED-Banane, einem Festival-Totem und dem Wunsch nach einer besseren App.
</p>

<p align="center">
  <img alt="Version v0.3.3 Testkandidat" src="https://img.shields.io/badge/version-v0.3.3--testkandidat-7056d8">
  <img alt="ESP32-C3" src="https://img.shields.io/badge/hardware-ESP32--C3-007c91">
  <img alt="Hardware Testing" src="https://img.shields.io/badge/status-hardware%20testing-2e8b71">
  <img alt="Nichtkommerzielles Hobbyprojekt" src="https://img.shields.io/badge/project-non--commercial-f39c49">
  <img alt="All rights reserved" src="https://img.shields.io/badge/license-all%20rights%20reserved-666666">
  <img alt="Mission: Hugs and Love" src="https://img.shields.io/badge/mission-hugs%20and%20love-ff69b4">
</p>


---

## Was ist RavestickBoard?

**RavestickBoard** ersetzt für die tägliche Bedienung der LED-Tafel die begrenzte Original-App durch ein **lokales Webinterface**, das auf einem **ESP32-C3 Super Mini** läuft. Ein iPhone, Android-Handy oder Computer verbindet sich mit dem WLAN des ESP32. Dieser überträgt die fertigen Anzeigen per **Bluetooth Low Energy (BLE)** an die CoolLEDX-Tafel.

Die Hersteller-Firmware der LED-Tafel wird dabei **nicht verändert**. Die Tafel kann hochgeladene Inhalte anschließend eigenständig darstellen. Eine Internetverbindung ist für die Bedienung nicht erforderlich.

Dieses Projekt ist ein privates, **nichtkommerzielles** Experiment für ein Festival-Totem. Es befindet sich aktiv in Entwicklung und ist noch kein fertiges Produkt.

## Aktuell: v0.3.3 · Testkandidat vom 25.10.2026

**Neu:** Statt eines festen 600-Zeichen-Limits berechnet das Webinterface live die tatsächliche Pixelbreite und die für eine Übertragung benötigte Datenmenge. Ein **vorläufiges, experimentelles Budget von 30 KiB einschließlich Header** steuert die Sendbarkeit; überschrittene Werte werden **fett und rot** angezeigt, Text wird niemals automatisch gekürzt. Besonders schmale Lauftexte können damit länger als 600 Zeichen werden. Zusätzlich bietet **Mini Zweizeilig** beim Lauftext jetzt **eine eigene Farbe pro Zeile**.

**Status:** v0.3.3 ist **noch nicht in der Arduino IDE und nicht auf echter Hardware getestet**. Der letzte vom Nutzer erfolgreich auf dem ESP32-C3 getestete Stand bleibt **v0.3.2 Korrektur 2**. Die neue Datenobergrenze ist eine Hypothese aus den v0.3.2-Versuchen, keine bestätigte Gerätespezifikation. BLE-OK bedeutet nicht zwangsläufig eine erfolgreiche Darstellung auf dem LED-Board. Eine Bibliothek und verkettete Lauftext-Programme sind **nur für spätere Versionen geplant**.

📋 **[Entwicklungsstand v0.3.3, Versuchsdaten und Testplan](docs/ENTWICKLUNGSSTAND_v0.3.3_2026-10-25.md)**

### v0.3.3 installieren und testen

1. Das vollständige Direktupdate in den **Stammordner des lokalen Repositories** entpacken und vorhandene Dateien überschreiben. Der bisherige Firmwareordner `firmware/RavestickBoard_v0_3_2/` bleibt unangetastet.
2. `firmware/RavestickBoard_v0_3_3/RavestickBoard_v0_3_3.ino` in der Arduino IDE öffnen und mit den bisherigen ESP32-C3-Einstellungen kompilieren/flashen. Die Header liegen direkt daneben, **kein Generator erforderlich**.
3. Mit dem WLAN `CoolLED1248` verbinden und `http://192.168.4.1` öffnen. Den neuen Testplan befolgen, besonders WLAN/RAM, Darstellung langer Texte und zwei Farben im Mini-Zeilenlauftext.
4. Erst nach eigenem Test den Stand über GitHub Desktop committen/pushen; dieses Paket verändert GitHub nicht.

## Rückblick: v0.3.2 · Testkandidat vom 25.10.2026

**Aktueller auf echter Hardware getesteter Stand: v0.3.2 Korrektur 2 (25.10.2026).** Nach der RAM-Reparatur starten WLAN und Webinterface wieder, bestehende NVS-Blockseiten bleiben erhalten. Die neue iPhone-Symbolpalette, fünf Lauftextschriften und 600 normale Lauftextzeichen wurden erfolgreich ausprobiert. **Lange Symbolketten bleiben ein Experiment:** 300 gemischte Symbole funktionieren, 320 gemischte nicht; das tatsächliche Datenlimit der Tafel ist noch ungeklärt. **Die erfolgreiche BLE-Übertragung ist nicht automatisch eine bestätigte Bildanzeige.**

📋 **[Hardwaretest-Bericht zu v0.3.2 mit Messungen und damaligen Entwicklungsideen](docs/ENTWICKLUNGSSTAND_v0.3.2_2026-10-25.md)** · Stand 25.10.2026, ca. 11:45 Uhr MEZ. Historische Versionsabschnitte weiter unten dokumentieren ihren **damaligen** Stand.

### Neu in v0.3.2

- **Symbolauswahl auf Mobilgeräten:** Das bisher zu schmale Overlay erhält eine viewportbreite, scrollbare Darstellung; es bleibt beim Einfügen weiterer Symbole geöffnet. Die Textfelder werden beim Einfügen nicht automatisch fokussiert, damit die iPhone-Tastatur nicht sofort wieder erscheint.
- **Fünf einheitliche Schriftbezeichnungen** für Lauftext und Blockseiten: **Standard**, **Standard Bold**, **Engschrift**, **Mini Einzeilig**, **Mini Zweizeilig**. Interne Layoutkennungen bleiben kompatibel.
- **Mini Zweizeilig im Lauftext:** Zwei separate Eingabefelder werden auf zwei 7-Pixel-Zeilen gezeichnet und scrollen synchron. Derselbe Farbwert gilt derzeit für beide Zeilen; eine individuelle Zeilenfarbe wird nicht neu eingeführt.
- **Neue Texte beginnen leer:** Kein „Grüße“ im neuen Lauftextfeld und kein „UMARMUNG“ auf neu angelegten Blockseiten. Bereits gespeicherte ESP32-NVS-Seiten und vorhandene browserseitige Entwürfe werden **nicht** überschrieben. Ein in einem alten Browserentwurf gespeicherter Demotext kann deshalb noch erscheinen.
- **Experimentelle Grenze 600 Zeichen** statt 80 für Lauftexte. Die Gesamtzahl beider Zeilen zählt gemeinsam; der Zähler wird **erst ab 601 rot und fett**, ohne Popup. Überlange Eingaben bleiben erhalten, werden aber nicht übertragen und auch nicht automatisch abgeschnitten.

**Korrektur 1:** Der `drawInline()`-Compilerfehler wurde behoben; der Nutzer konnte die Firmware danach kompilieren und aufspielen. **Dabei scheiterte der WLAN-Start aufgrund einer RAM-Allokation des WLAN-Treibers.**

**Korrektur 2 (WLAN/RAM, anschließend hardwaregetestet):** Zwei fest reservierte Bildpuffer von zusammen 122.507 Bytes wurden entfernt. Beim Senden wird nun ein einzelner, passend großer temporärer Payload-Puffer verwendet, der nach dem Auftrag freigegeben wird. Der serielle Monitor zeigt Heap-Größe und größten freien Block. Die maximale Textlänge bleibt vorerst experimentell bei 600 Zeichen. Bereits gespeicherte Blockseiten bleiben unangetastet. **Bei WLAN- oder RAM-Problemen nicht die NVS löschen**, sondern bei Bedarf v0.3.1 über USB flashen.

### Bitte vor dem ersten Test wissen

Die **600 Zeichen sind keine bestätigte BLE-Grenze**. Der bisher rekonstruierte Textheader enthält 80 Format-Slots, und der Quellcode setzt den dortigen Zählwert bei längeren Texten weiterhin auf höchstens 80, während er die umfangreichere Pixelmatrix zu senden versucht. Das ist **ein riskanter Protokollversuch**, kein garantierter Weg für 600 darstellbare Zeichen. Längere Inhalte, insbesondere breite Symbole, können viel RAM, längere Sendezeiten und deutlich mehr BLE-Pakete benötigen. Der Versuch mit **600 normalen Zeichen** verlief erfolgreich. Für die anschließende Grenzerforschung sind reine Texte und breite Symbole getrennt zu testen; serielles Log und sichtbares Ergebnis immer getrennt notieren. Für die Zukunft wird ein **datenbasiertes Pixel-/Byte-Limit** statt der aktuellen Zeichenbegrenzung untersucht.

Das NVS-Schema der Blockseiten bleibt unverändert. Die älteren Firmwareordner werden nicht überschrieben. Eine Rückkehr zu v0.3.1 per USB bleibt möglich.

### Installation des direkt entpackbaren v0.3.2-Pakets

1. ZIP **in den Stammordner des lokalen Git-Repositories** entpacken. Nur gleichnamige Dokumente und `web/index.html` überschreiben; bestehende Firmwareordner und `.git` behalten.
2. `firmware/RavestickBoard_v0_3_2/RavestickBoard_v0_3_2.ino` in der Arduino IDE öffnen. `RavestickBoard_WebUI.h` und `RavestickBoard_Assets.h` liegen direkt daneben, ohne Generatorlauf.
3. Board **ESP32C3 Dev Module**, **USB CDC On Boot: Enabled**, **No OTA (2MB APP/2MB SPIFFS)**, Espressif-Boardpaket **3.3.12** auswählen; kompilieren und per USB aufspielen.
4. Mit WLAN **CoolLED1248** verbinden und `http://192.168.4.1` öffnen. Den Testplan unter `docs/TESTPLAN.md` verwenden.
5. Nach Kontrolle deines bereits erfolgten Hardwaretests über **GitHub Desktop** committen/pushen. Dieses ZIP verändert GitHub nicht direkt. `docs/REPO_AUFRÄUMEN_v0.3.2.md` enthält separate Vorschläge für spätere Bereinigungen.

## Rückblick: v0.3.0-dev (Praxistest am 24.10.2026)

**Praxistest 24.10.2026:** Die v0.3.0-dev wurde vom Nutzer in der Arduino IDE kompiliert, auf dem ESP32-C3 gestartet und praktisch an der CoolLEDX-Tafel überprüft. Die zentralen Funktionen waren erfolgreich; **bekannte Einschränkungen und offene Tests bleiben ausdrücklich bestehen**. Die ältere, ebenfalls getestete v0.2.0 Korrektur 1 bleibt als Rückfallversion im Repository. v0.3.1 und v0.3.2 bauen darauf auf; v0.3.1 wurde inzwischen ebenfalls real getestet.

### In v0.3.0 implementiert und im Nutzer-Test teilweise bestätigt

- Originalschriftarten **Normal** und **Bold** mit je 82 aus der Hersteller-App rekonstruierten Zeichen als eigene Auswahl im Blockeditor; im Praxistest wurde die Darstellung gegenüber der Original-App als übereinstimmend bewertet.
- Unsere **Minischrift, eine Zeile**: sieben Pixel hoch, horizontal **und** vertikal zentriert; zweizeilige Minischrift bleibt erhalten, in Mini werden kleine Buchstaben auf der Tafel als Großbuchstaben dargestellt.
- **Inline-Symbole:** 24 rekonstruierte Herstellerpiktogramme und eine selbst entworfene, vertikal gespiegelte RavestickBoard-Banane. Symbole lassen sich wie Emojis an der Cursorposition in den Text einfügen; die Vorschau zeigt das gemischte Pixelraster.
- **Herz in allen sieben LED-Farben** über sieben Emoji-Varianten.
- Unterschiedliche tatsächlich beleuchtete Symbolbreiten werden bei der Platzierung berücksichtigt. **Abstände vorerst experimentell**, ein systematischer Abgleich mit der Original-App steht aus.
- Direkte Befehle für Helligkeit, Tempo und Modus ohne erneuten Bild-Upload, mit zeitlich befristeter BLE-Verbindung. **Im kurzen Nutzer-Praxistest funktionierend; Langzeitverhalten und Geschwindigkeitskalibrierung noch offen.**
- Lauftextentwurf und zuletzt geöffneter Editor-Reiter sollten lokal zwischengespeichert werden. **Im Praxistest sprang der Lauftext nach Neuladen dennoch auf „Grüße!“ zurück**; das muss korrigiert werden.

### Beobachtungen aus dem v0.3.0-Praxistest

| Bereich | Ergebnis und weiterer Bedarf |
| --- | --- |
| Start, BLE, Speicherung | ESP32-C3 startete; bisherige v0.2.0-Blockseiten blieben erhalten; wiederholte Übertragungen und Diagnose funktionierten. |
| Schriftarten | Normal/Bold entsprachen im Test der Herstellerdarstellung. Mini einzeilig ist horizontal und vertikal zentriert; zweizeilig funktioniert. |
| Symbole und Farben | Inline-Symbole im Text und sieben Herzfarben funktionieren. Die selbst entworfene Ravestick-Banane ist jedoch noch nicht gut erkennbar. |
| Fernsteuerung | Helligkeit, Tempo und Anzeigemodus konnten ohne erneuten Upload gesteuert werden. Der zusätzliche Helligkeits-Sendebutton ist überflüssig. |
| Lauftext | **40-Zeichen-Begrenzung** ist zu eng; lange Symbolfolgen werden teils bereits in der HTTP-Validierung abgewiesen. Das tatsächliche Hardwarelimit ist noch unbekannt. |
| Entwurf | Blockseiten bleiben nach Reload gespeichert; im Lauftext erschien erneut der Demotext **„Grüße!“** statt des zuletzt bearbeiteten Entwurfs. |
| Bedienung | Symbol-Picker steht zu weit unten; LED-Vorschau soll beim Scrollen sichtbar bleiben. Symbolbreiten/-abstände benötigen später weitere Original-App-Vergleiche. |

Die älteren Aussagen „v0.3.0 noch nicht hardwaregetestet“ in historischen Release-Dokumenten beziehen sich auf den **Zeitpunkt vor dem Test am 24.10.2026**. Die neuen Ideen in `docs/THEMENSPEICHER.md` sind **noch nicht Bestandteil der Firmware**.

### Frühere bestätigte Basis: v0.2.0


**Die Version v0.2.0 inklusive Kompilierkorrektur wurde am 23.10.2026 auf echter Hardware gestartet und praktisch getestet.** Der separate WebUI-Header ist Bestandteil der funktionierenden Fassung.

### Bereits umgesetzt

- **Lauftext- und Blockeditor** als getrennte Bereiche, ohne automatische Moduswahl anhand der Textlänge.
- **Bis zu zwölf individuell gestaltete Blockseiten**: hinzufügen, duplizieren, verschieben, löschen und dauerhaft speichern.
- **Pro Seite (v0.2.0):** eine große einzeilige Engschrift oder zwei Zeilen in unserer eigens entwickelten Minischrift; sieben LED-Farben, Ausrichtung und individuell einstellbare Anzeigezeit von 5–60 Sekunden.
- **Zweizeilige Darstellung mit unterschiedlichen Zeilenfarben**, auf der echten LED-Tafel erfolgreich getestet.
- **64 × 16-LED-Vorschau** mit einzelnen runden Punkten sowie Wiedergabesteuerung einschließlich 1×, 2× und 4×.
- **Permanente Speicherung der Blockseiten auf dem ESP32** mittels NVS, auch nach Neuladen des Webinterfaces beziehungsweise Neustart.
- **Experimenteller automatischer Seitenplayer**, Diagnosebereich und BLE-Übertragung.
- **Deutsche Sonderzeichen** und eigene Pixel-Glyphen; verschiedene Formen benötigen noch Feinschliff.

### Reale Hardwaretests vom 23.10.2026

Die folgende Zusammenfassung stammt aus dem ersten systematischen Nutzer-Testprotokoll für v0.2.0. **13 von 14 Punkten wurden mit „Ja“ bestätigt**, ein Punkt war noch nicht abschließend bewertet. „Ja“ bedeutet dabei, dass die Grundfunktion läuft, nicht unbedingt, dass Darstellung und Bedienung bereits ausgereift sind.

| Testbereich | Ergebnis | Beobachtung |
| --- | --- | --- |
| ESP32-Start und Webinterface auf dem iPhone | ✅ | Funktioniert |
| Blockseiten anlegen, duplizieren, sortieren | ✅ | Funktioniert |
| Unterschiedliche Farben | ✅ | Funktioniert |
| Zweizeilige Schrift | ✅ | Gut lesbar; soll in der derzeitigen Grundform erhalten bleiben |
| Einzeilige Schrift | ⏳ | Separater Testpunkt noch offen; gewünscht ist zusätzlich „Mini, eine Zeile“, vertikal zentriert |
| Kleine Buchstaben / Unterlängen | ✅ mit Änderungsbedarf | Die kleine Schrift soll künftig **ausschließlich Großbuchstaben darstellen** |
| Umlaute und Sonderzeichen | ✅ mit Änderungsbedarf | Beim **ß** soll ein konkret markierter Pixel ausgeschaltet werden |
| Automatischer Seitenwechsel | ✅ mit Einschränkung | **Zwischen den Seiten erscheint die Prozentanzeige**, weil jede Seite neu übertragen wird |
| Individuelle Anzeigedauer | ✅ mit Einschränkung | Zeitsteuerung funktioniert, aber mit erneuter BLE-Übertragung zwischen den Seiten |
| Seiten nach Neustart weiterhin vorhanden | ✅ | NVS-Speicherung bestätigt |
| Mehrfache Übertragungen / Programmabfolge | ✅ | Im ersten Praxistest funktionsfähig |

**Wichtig:** Der Seitenplayer von v0.2.0 lädt für jeden Wechsel eine komplette Seite per BLE hoch. Die Tafel zeigt dabei ihre eigene Übertragungsfortschrittsanzeige. Dieses Verhalten ist für einen zusammenhängenden Blocktext **noch nicht die gewünschte Lösung**. Der vorhandene Code ist jedoch eine gute Grundlage für eine spätere **Playlist aus mehreren eigenständigen Programmen**.

## So funktioniert es

```text
Handy / Computer
       │
       │ WLAN: lokales Webinterface
       ▼
ESP32-C3 Super Mini
       │
       │ BLE: Steuerbefehle + Pixeldaten
       ▼
CoolLEDX / CoolLED1248
64 × 16 Pixel · 7 LED-Farben
```

Die Hardware unterscheidet zwischen **Inhaltsübertragungen** und kurzen **Steuerbefehlen**. Helligkeit (`0x08`), Laufgeschwindigkeit (`0x07`) und Anzeigemodus (`0x06`) lassen sich prinzipiell separat steuern. Die Direktbefehle sind in v0.3.0-dev im Nutzer-Praxistest bestätigt. Die Verbindungserhaltung, Grenzfälle und die exakte Geschwindigkeitskalibrierung bleiben offen.

### Geplante Architektur

- **Seite:** Individuell gestaltete Anzeige mit Text, Schrift, Farben, Positionierung und gegebenenfalls Dauer.
- **Programm:** Eine oder mehrere Seiten, die idealerweise **mit einer Übertragung** auf die Tafel gelangen und anschließend autonom ablaufen.
- **Playlist:** Eine Abfolge mehrerer Programme, deren Wechsel der ESP32 steuert. Dabei sind neue BLE-Uploads akzeptabel.

Ob unsere konkrete Tafel mehrseitige Inhalte oder eigene Animationen über den dokumentierten Animationsbefehl `0x04` selbstständig abspielen kann, ist **noch nicht auf unserer Hardware verifiziert**. Das ist ein wichtiges nächstes Forschungsthema.

## Installation und erste Verbindung

**Benötigt:** ESP32-C3 Super Mini, kompatible CoolLEDX-Tafel (hier 64 × 16 Pixel, sieben Farben), USB-Kabel und Arduino IDE mit dem Espressif-Boardpaket.

1. Für den **v0.3.2-Testkandidaten**: `firmware/RavestickBoard_v0_3_2/RavestickBoard_v0_3_2.ino` öffnen. Die geprüfte v0.3.0-dev bleibt als direkte Rückfallversion bestehen.
2. Sicherstellen, dass **`RavestickBoard_WebUI.h` und `RavestickBoard_Assets.h`** zusammen mit der `.ino` im selben Sketch-Verzeichnis liegen. Beide sind im neuen v0.3.2-Ordner bereits enthalten.
3. Das Board **`ESP32C3 Dev Module`** auswählen und **`USB CDC On Boot: Enabled`** setzen.
4. Die Partitionierung **`No OTA (2MB APP/2MB SPIFFS)`** auswählen. Getestete Entwicklungsumgebung: Arduino IDE 2.3.10, `esp32` von Espressif 3.3.12.
5. Über USB auf den ESP32 flashen. **OTA-Firmwareupdates sind nicht vorgesehen.**
6. Handy oder Computer mit dem vom ESP32 erzeugten WLAN **`CoolLED1248`** verbinden (Standardpasswort **`Banane1248!`**, bewusst öffentlich dokumentiert). Es ist kein Geheimnis; Personen in Funkreichweite könnten damit das Geräte-WLAN verwenden. Eine individuelle Passwortkonfiguration im Webinterface ist für eine spätere Version vorgesehen.
7. Die lokale Seite **[http://192.168.4.1](http://192.168.4.1)** öffnen. Der Browser benötigt keine Internetverbindung.
8. Zunächst eine Blockseite **einzeln** übertragen; erst danach die experimentelle automatische Wiedergabe ausprobieren.

> **Hinweis:** Die BLE-Adresse der bisher verwendeten Tafel ist in der Firmware noch fest hinterlegt. Eine automatische Gerätesuche und Unterstützung mehrerer Boards sind geplant. Während die Original-App die BLE-Verbindung zur Tafel hält, kann RavestickBoard unter Umständen nicht gleichzeitig übertragen.

## Bedienkonzept und vorgemerkte Weiterentwicklung

Die Grundbedienung bleibt bewusst einfach: **Text schreiben, globale Schrift und Farbe auswählen, senden.** Bei der zweizeiligen Mini-Blockanzeige gibt es bereits **getrennte Farben pro Zeile**. Neu vorgemerkt sind **schmalere Leerzeichen in der Mini-Schrift** und eine **eigene horizontale Ausrichtung pro Zeile** (Links / Zentriert / Rechts), statt einer gemeinsamen Ausrichtung für beide Zeilen.

### Größere Erweiterungen, noch nicht umgesetzt

- **🎨 Erweiterter Formatierungseditor:** achte Schaltfläche neben den sieben Farben. Textbereiche markieren oder Buchstaben, Leerzeichen und Symbole als einzelne Kacheln mit Mehrfachauswahl bearbeiten. **Alles auswählen**, **Auswahl aufheben** und **Auswahl umkehren**.
- **Farbe und Font je Zeichen:** Globaler Standard bleibt erhalten, einzelne Wörter dürfen Original Normal, Original Bold, Engschrift und gegebenenfalls Mini Einzeilig verwenden. Einzelabweichungen lassen sich unabhängig für Farbe und Schrift festlegen oder zurück auf „Standard übernehmen“ setzen.
- **Globale Änderung mit Wahlmöglichkeit:** Bei vorhandenen Einzelabweichungen kompakt fragen, ob diese **beibehalten** oder für die geänderte Eigenschaft **alle angeglichen** werden sollen. Keine globalen Änderungen an der jeweils anderen Eigenschaft.
- **Rückgängig / Wiederholen (↶ / ↷):** Möglichst früh auch für normale Texteingaben und versehentliche Löschungen, später ebenso für Formatierungen und Seitenaktionen. Editor-Rückgängig sendet niemals ungefragt eine neue Anzeige an die Tafel.
- **Originalsymbole bewahren:** Festfarbige Hersteller-Piktogramme behalten ihre Originalfarben und reagieren stillschweigend nicht auf Farb- oder Schriftwechsel, selbst wenn ihre Kachel ausgewählt ist. **Keine Warnhinweise**. Herzvarianten bleiben eine bewusst unterstützte Ausnahme; weitere Varianten, etwa Warnzeichen in anderen Farben, werden später einzeln entschieden.
- **Neue Symbolreferenzen:** Kussmund/Lippen, Blume/Rose und Biene wurden in der Original-App nachgewiesen und fehlen noch in unserer bisherigen 24er-Herstellerliste. Die Blume benötigt weniger rechten Nachlauf, der Stern soll ohne Umzeichnung vertikal besser sitzen. Als **eigene** Ravestick-Symbole sind eine besser erkennbare Totem-Banane und ein gelbes Quietscheentchen vorgesehen.
- **Freie Text-/Symbolkomposition:** Große einzeilige Schriften, kleine zweizeilige Blöcke und Symbole gemeinsam auf 64×16 Pixel anordnen. **Nur Ideenspeicher**, noch keine Umsetzung: Erst die angekündigten Nutzerskizzen abwarten.
- **Bibliothek:** Leerer neuer Editor, vorhandene Programme bewusst aus der Bibliothek laden; bestehende gespeicherte Seiten dürfen bei einer späteren Migration nicht verloren gehen.

Alle konkreten Anforderungen, Aufgaben-IDs, Prioritäten und offenen technischen Fragen stehen in [`docs/THEMENSPEICHER.md`](docs/THEMENSPEICHER.md). Ein künftiger v0.4.0-Build wurde besprochen, ist durch diese Dokumentationspflege **weder erstellt noch freigegeben**.

## Bekannte Grenzen und nächste Schritte

| Thema | Stand |
| --- | --- |
| **Blockprogramm ohne Prozentanzeige** | Offen: Seiten sollen als **ein Programm** übertragen werden; Animations- und native Blockmodi untersuchen |
| **Minischrift** | Zweizeilig und einzeilig am 24.10.2026 erfolgreich getestet; **Leerzeichen zu breit**, **Ausrichtung pro Zeile** bei zweizeiligem Blocktext noch offen |
| **Schrift-Feinschliff** | `g`, `p`, `q`, `y` und `ß`/`ẞ` weiter optimieren; 82 Originalglyphen je Normal/Bold rekonstruiert und in v0.3.0-dev integriert, Vergleich im Praxistest positiv; weitere Einzelzeichen-Regressionsprüfung offen |
| **Live-Steuerung** | Direkte BLE-Kommandos für Helligkeit, Tempo und Modus im v0.3.0-Praxistest funktionierend; Kalibrierung und Langzeitprüfung offen |
| **Bibliothek** | Noch nicht umgesetzt; NVS-Blockseiten vorhanden, Lauftextentwurf nach Reload fehlerhaft; später Programme, Export/Import und Favoriten |
| **Individuelle Gestaltung** | Erweiterter 🎨-Editor mit Mehrfachauswahl, Farb-/Font-Abweichungen, Undo/Redo und gezieltem Reset: noch nicht umgesetzt |
| **Eigene Animationen** | `0x04` und Speichergrenzen auf dem konkreten Board experimentell überprüfen |
| **Weitere Programme** | Countdown, Stoppuhr, Scoreboard, Lichtfunktion, Playlist und acht Schnellzugriffe (Ideen, nicht in v0.3.0 vorhanden) |

Weitere Ideen, Entscheidungen und offene Aufgaben werden unter [`docs/THEMENSPEICHER.md`](docs/THEMENSPEICHER.md) sowie [`docs/ROADMAP.md`](docs/ROADMAP.md) gesammelt. Änderungen und Teststände stehen in [`CHANGELOG.md`](CHANGELOG.md).

## Referenzen, Credits und Lizenzsituation

RavestickBoard baut auf eigenen Hardwaretests und öffentlich dokumentierten Erkenntnissen zum CoolLEDX-BLE-Protokoll auf. Wertvolle Referenzen sind insbesondere:

- [UpDryTwist/coolledx-driver](https://github.com/UpDryTwist/coolledx-driver) (MIT-lizenziertes Referenzprojekt).
- [schulzad/coolled-sign-controller / OpenSign](https://github.com/schulzad/coolled-sign-controller) (wichtige Protokollforschung und Animationsexperimente; **derzeit ohne Freigabelizenz für die Übernahme des Quellcodes**).
- [ohtaras/led](https://github.com/ohtaras/led) (Protokollreferenz; gesonderte Lizenzprüfung vor einer etwaigen Codeübernahme).

**RavestickBoard ist öffentlich einsehbar, aber derzeit nicht unter einer Open-Source-Lizenz freigegeben.** Es gilt der Copyright- und Nutzungshinweis in [`LICENSE`](LICENSE) (All Rights Reserved). Sichtbarkeit, Dokumentation und technische Diskussion sind erwünscht; daraus entsteht keine allgemeine Erlaubnis zum Kopieren, Bearbeiten oder Weiterverbreiten des eigenen Projektcodes. GitHubs Nutzungsbedingungen und gesetzliche Rechte bleiben unberührt.

Nichtkommerzieller Einsatz allein ersetzt keine Nutzungsrechte an fremdem Code. Insbesondere übernehmen wir OpenSign-Code ohne Erlaubnis **nicht** und portieren ihn nicht einfach nach C++. Die Quellcodeherkunft und die Abgrenzung dokumentierter Protokolldaten von fremder Software dokumentiert [`docs/QUELLEN_UND_LIZENZEN.md`](docs/QUELLEN_UND_LIZENZEN.md).

---

<p align="center">
  <strong>Made for the rave, powered by a banana. 🍌</strong><br>
  <sub>RavestickBoard is an independent hobby project, not affiliated with the CoolLED1248 manufacturer.</sub>
</p>
