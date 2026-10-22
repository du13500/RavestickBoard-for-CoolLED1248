# Entwicklungs-Roadmap

**Status:** Planungsvorschlag vom 22.10.2026, flexibel. Keine Zusage oder fixe Termine. `THEMENSPEICHER.md` ist die maßgebliche abhakhbare Aufgabenliste.

| Zielversion | Schwerpunkt | Fertig, wenn … |
| --- | --- | --- |
| **0.1.0** ✅ | Erster WLAN-/BLE-Webcontroller | iPhone kann Text und Farbe auf die Tafel übertragen. |
| **0.1.1** ✅ | Diagnose + iPhone-UI-Korrekturen (Gerätetest erfolgt) | Status, Events, Heap und TXT-Export funktionieren; mobile Layoutfehler behoben. |
| **0.1.2** 🧪 | Punktvorschau, deutsche Schrift, Modus und Geschwindigkeit, Log-Kopieren | Code bereit; Hardwaretests und Safari-Kopieren offen. |
| **0.2.0** ⬜ | Seiteneditor | Seiten/Blöcke unabhängig gestalten, einstellbare Anzeigedauer, manueller und automatischer Wechsel. Statisch/Lauftext ab v0.1.2 als Test vorhanden. |
| **0.2.x** ⬜ | Seiteneditor | Jede Seite separat gestaltbar, manuelles und automatisches Durchblättern, Seitenliste. |
| **0.3.0** ⬜ | Zeichensätze + Zeichenformatierung | Engschrift-Kleinbuchstaben ab v0.1.2 getestet werden; Original Normal/Bold und Mini, Farbe/Font je Zeichen oder Bereich. |
| **0.4.0** ⬜ | Bibliothek + Programme | Vorlagen, Favoriten, 8 Shortcuts, Reihenfolge, Dauer, Wiederholung, Profile, Rotation. |
| **0.5.0** ⬜ | Spezialanzeigen | Countdown, Stoppuhr, manuelles Scoreboard ❤️/💋, Lichtmodi weiß/rot. |
| **0.6.0** ⬜ | Grafiken + Animationen | Dot-Editor, Import, Platzlimits, Speicher und Vorschau in echter Auflösung. |
| **0.7.x** ⬜ | Robustheit + Release-Vorbereitung | Wiederverbindung, konkurrierende BLE-Steuerung, Tests, Lizenzen, Dokumentation. |

## Übergreifende Grundsätze

- Lieber eine zuverlässige kleine Funktion als ein halbfertiges Großfeature.
- Die originale Anzeige soll selbstständig abspielen, wo möglich; der ESP32 lädt neue Inhalte nur bei Bedarf oder zur bewussten Programmsteuerung.
- Funktionen wie Countdown/Scoreboard erfordern möglicherweise häufige BLE-Updates. Machbarkeit und nutzbare Frequenz gesondert prüfen.
- Keine OTA-Updates, kein Heimnetz-Zwang, keine doppelte Anmeldung, kein automatisch verkleinerter Text auf statischen Seiten.
- Keine Änderung der Originalfont-Zeichen ohne Einzelentscheidung.

## Warum die Roadmap keine Feature-Versprechen sind

Die vollständige Machbarkeit aller Spezialfunktionen hängt von RAM, Flash, BLE-Übertragungszeit und dem Verhalten der Tafel ab. Prüfaufgaben bleiben so lange offen, bis sie am Gerät getestet wurden.
