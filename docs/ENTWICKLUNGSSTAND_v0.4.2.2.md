# Entwicklungsstand v0.4.2.2-dev

## Neu in dieser Version

Die Hardwaretests der Vorversion bestätigen reine Lauftext- und Blockprogramme auf der CoolLEDX. Bei gemischten Programmen wurden dagegen nur die ersten Textgruppen abgespielt. Ursache in der Codebasis: `v042Tick()` war vorhanden, wurde aber nicht aus `loop()` aufgerufen. Das ist in dieser Version korrigiert, ebenso die fehlende Timer-Rücksetzung beim Programmstart/-stopp.

Erste Kalibrierung für Blockgruppen: `T[s] = Seiten × 0,0795 × (100−Tempo)` für Tempo 60–90, mit fünf Hardwaremesswerten; für alle anderen Inhalte/Tempi bleibt der 5–180-s-Fallback. Programmlaufzeit von Lauftexten und die genaue Zeit zwischen Ende BLE-Upload und sichtbarem Anzeigenstart sind nicht hinreichend geklärt.

Die Bibliothek erlaubt nun das Entfernen referenzierter Inhalte. Programme mit fehlenden Bibliotheks-IDs bleiben in LittleFS gespeichert, erscheinen in der Weboberfläche als reparaturbedürftig und sind vor einer Reparatur nicht ausführbar. Zugehörige direkte Kurzwahlen werden automatisch gelöscht. Der Editor unterscheidet neue und gespeicherte Inhalte, aktualisiert vorhandene IDs und bietet einen Ein-Klick-Rückweg mit Änderungserkennung. Die Statusmeldungen werden reduziert.

## Tests / Grenzen

Die lokale JS-Syntaxprüfung, browserseitige CRUD-/Programmeditor-Tests mit simulierten HTTP-Antworten, 135 hostseitige bytegenaue Renderer-Vergleiche und der Bibliotheks-Mocktest wurden erfolgreich durchgeführt. **Keine vollständige Arduino-ESP32-Kompilierung und kein neuer Hardwaretest:** Der auf dem ESP32 funktionierende BLE-Kern wurde unverändert kopiert, neue Controller-Abläufe bedürfen aber der Gerätebestätigung.

Keine automatische Dateisystemformatierung; vorhandene Bibliotheks-/Programmformate unverändert. Vor dem Flashen externes JSON-Backup aufheben und „Erase All Flash“ ausgeschaltet lassen.
