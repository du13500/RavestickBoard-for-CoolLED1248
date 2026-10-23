# Testplan v0.2.0: Blockeditor (ESP32-C3 / CoolLEDX)

**Erwartung:** Gerät bleibt zunächst beim bisherigen Anzeigetext. Nach dem Flashen soll **kein automatischer Seitenwechsel** starten.

## Kompilierung und Start
- [ ] Arduino IDE: ESP32C3 Dev Module, USB CDC Enabled, Partition No OTA (2MB APP / 2MB SPIFFS).
- [ ] Kein C++-Compilerfehler wie in der ersten v0.1.2 (`Job`).
- [ ] Serieller Monitor meldet v0.2.0, NVS-Status, AP bereit, HTTP bereit.
- [ ] Smartphone öffnet die Webseite unter http://192.168.4.1 und sieht v0.2.0.

## Editor ohne Upload
- [ ] Reiter „Blockanzeige“ öffnen, **noch kein BLE-Senden**.
- [ ] Neue Seite hinzufügen. Seite 1 und 2 unterschiedlich beschriften.
- [ ] Einzelne Seite speichern, wechseln, erneut aufrufen. Text, Farben, Dauer erhalten.
- [ ] Duplizieren, Pfeile für Reihenfolge, Löschen (mit Sicherheitsfrage).
- [ ] Mehr als zehn Zeichen in einer Zeile: Warnung, kein Abschneiden und kein Upload.
- [ ] Umstellen auf Mini: zwei Zeilen sichtbar. In der Vorschau sind 64×16 Punkte fix.
- [ ] 1×, 2×, 4×, Pause und Neuanfang im Browser.

## Manueller BLE-Test
- [ ] Seite 1 mit gelbem „SUCHE:“ als eine große Zeile anzeigen.
- [ ] Seite 2 als Mini zweizeilig: „Grüße“ (rot), „ÄÖÜ ẞ“ (grün).
- [ ] Beide Zeilen komplett sichtbar, keine schrägen abgeschnittenen Zeichen.
- [ ] Danach Test mit rechtsbündiger Ausrichtung.
- [ ] Kein Reset notwendig zwischen zwei Übertragungen.

## Dauerhafter Programmspeicher
- [ ] Seiten speichern. ESP32 USB abziehen und erneut einschalten.
- [ ] Browser neu öffnen: Seiten und Werte sind noch gespeichert.
- [ ] Led-Tafel zeigt weiterhin den zuletzt hochgeladenen Inhalt, **kein Autostart**.

## Automatisches Programm: Erst nach bestandenen manuellen Tests
- [ ] Zwei Seiten, Anzeigedauer z. B. je 7 Sekunden.
- [ ] „Programm starten“, zwei Wechsel beobachten, tatsächliche Gesamtdauer notieren.
- [ ] „Stoppen“: Tafel behält letztes Bild.
- [ ] Hersteller-App während der Übertragung verbinden: Fehlermeldung und sauberer Abbruch, danach erneuter Start möglich.
- [ ] Diagnose mit „Log kopieren“ abrufen und Laufzeit, freien Heap, BLE-/Page-Zähler prüfen.

## Offene bekannte Grenzen
- [ ] Leere Seite nicht erlaubt.
- [ ] Pro Zeile noch keine per-Zeichen-Farbe und nur ein gemeinsamer Font.
- [ ] 5 Sekunden Sichtbarkeit beginnt erst nach dem Upload, dadurch sind Seitenwechsel nicht nahtlos.
- [ ] Keine automatische Programmausführung nach Stromverlust.

**Abnahme erst nach realem Boardtest und RAM-Auswertung; die lokale Simulation allein genügt nicht.**
