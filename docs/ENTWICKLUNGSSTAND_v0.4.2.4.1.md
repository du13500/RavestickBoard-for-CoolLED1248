# Entwicklungsstand v0.4.2.4.1-dev

**Gemeinsamer Projektstand 29.10.2026:** v0.4.2.4 wurde auf ESP32-C3 kompiliert und ohne Panic gestartet. Bibliothek und sechs Kurzwahlen blieben erhalten. Kleine gemischte Laufbänder (inklusive Farben, Blockseiten und 16 Leer-Pixelspalten) sind hardwarebestätigt. Größere geteilte Laufbänder, BLE-Wiederholungen, Speicherverlauf und die restlichen Regressionstests sind noch nicht abgeschlossen.

**Hotfix:** Tafelsteuerung im Dokumentfluss statt am unteren Browserrand festgehalten; kompakte Kurzwahl-Icon-Aktionen und Bearbeitungsmodus ohne Meldung; Programmlöschung entfernt `.bin` und `.bak` (F-06). Keine Änderung des Datenformats oder der BLE-Sendelogik. **Diese Version ist noch nicht auf Hardware bestätigt.**

**Qualität:** Aus Claudes unabhängigem statischen Review bleiben F-01–F-05, F-07–F-12 und S-01–S-06 offen. Insbesondere die Paketbestätigungen (F-03) sind noch nicht empirisch geklärt. Teststufen getrennt führen: JS-Syntax, Mock-Browser, Host-Verhaltenstest, ESP32-Kompilierung, CoolLEDX-Hardware.
