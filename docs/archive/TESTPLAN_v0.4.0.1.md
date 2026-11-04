# Hardwaretest v0.4.0.1-dev · 26.10.2026

Wir testen gezielt die geänderten Bereiche und die wichtigsten Regressionen. Die Basis v0.4.0 hat bereits 30 von 31 Prüfpunkten erfolgreich bestanden.

| Nr. | Testbereich | Testschritt | Erwartetes Ergebnis | Ergebnis / Notiz |
| ---: | --- | --- | --- | --- |
| 1 | Build | `RavestickBoard_v0_4_0_1.ino` kompilieren | Erfolgreich im Boardprofil ESP32C3 Dev Module | ☐ |
| 2 | Boot/WLAN | Flash ohne Erase All Flash, USB-Reset | `v0.4.0.1-dev`, AP `CoolLED1248` | ☐ |
| 3 | NVS | Vorherige neun Seiten öffnen und senden | Inhalte und Reihenfolge unverändert | ☐ |
| 4 | Kurzwahl | iPhone-Startseite ohne Scrollen | Sechs 2×3-Tasten + Glühlampenregler + Ein/Aus | ☐ |
| 5 | Menü | Banane öffnen, sieben Menüpunkte prüfen | Kurzwahl immer auf Platz 1; danach Texteditor, Bibliothek, Programme, Ambiente, Diagnose, Einstellungen | ☐ |
| 6 | Bibliothek | Ansicht bei kleinem iPhone öffnen | Großer innerer Scrollbereich, kompakte Regler und Toolbar | ☐ |
| 7 | Programme | Programmansicht öffnen | Gleicher großer innerer Scrollbereich; noch keine echte Bibliothek | ☐ |
| 8 | Pixelregler | Helligkeit/Tempo in Bibliothek, Programme, Editor ansehen | Kleine/große Glühlampe, Schnecke/Pferd; keine Reglerüberschriften | ☐ |
| 9 | Globalwerte | Tempo/Helligkeit von zwei Ansichten ändern | Gleicher geräteweiter Wert überall; Textupload überschreibt ihn nicht | ☐ |
| 10 | Status | Kurzwahl unbelegt antippen, Seite neu laden | Kein permanenter Fußtext; dezente Kachelreaktion | ☐ |
| 11 | Toast | Regulären Auftrag senden | Erfolgsinfo verschwindet von selbst; echte Fehler bleiben bestätigbar | ☐ |
| 12 | Symbolsortierung | Herz-Symbolpicker öffnen | Themen statt Herkunft; Alien zwischen UFOs | ☐ |
| 13 | Schneeflocken | Natur/Wetter anzeigen | Zwei verschiedene Schneeflocken, beide auswählbar | ☐ |
| 14 | Krabbe | Krabbe in Lauftext einfügen und senden | Neues rotes Excel-Motiv statt bisheriger Grafik | ☐ |
| 15 | Neue Pixelart | Warnfarben, Pfeile, NO REC einzeln senden | Farbige 16×16-Motive entsprechen Picker-Vorschau | ☐ |
| 16 | Bestandssymbole | Herzfarben, Banane, UFO, Texte testen | Keine Regression | ☐ |
| 17 | Ambiente | Rot, Grün, Cyan, Weiß; Helligkeit ändern | Vollbild korrekt, globale Helligkeit erhalten | ☐ |
| 18 | Ein/Aus | Beide Befehle von Kurzwahl aus | Ohne unnötigen Bildtransfer | ☐ |
| 19 | Diagnose | Status/TXT exportieren | Version korrekt; keine Passwörter oder Nutzertexte | ☐ |
| 20 | Speicher | Minimalen Heap vor/nach intensiver Nutzung notieren | Kein Fehler; 9.416-B-v0.4.0-Minimum als Vergleich | ☐ |
| 21 | Dauerbetrieb | ≥ 30 Minuten, Browserreloads und zwei Geräte | Kein Freeze, BLE-Steuerung verfügbar | ☐ |

**Hinweis:** Die reale sichtbare LED-Wirkung prüfen; ein erfolgreich quittierter BLE-Job allein beweist keine Bildanzeige.
