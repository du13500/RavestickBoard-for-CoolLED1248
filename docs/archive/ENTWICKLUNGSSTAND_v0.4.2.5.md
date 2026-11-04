# Entwicklungsstand v0.4.2.5-dev · 29.10.2026

**Status:** Firmware-Quelländerungen und Offline-Prüfungen fertig; Arduino-Build und realer Hardware-/Langzeittest noch ausstehend.

## Änderungen gegenüber v0.4.2.4.2-dev-B

- gzip/ETag/304 und 20-ms-HTTP/RAMTRACE übernommen; exakte Firmwareversionskennung in Status, Diagnose und Downloadnamen.
- `JsonChunkWriter` mit begrenztem 1024-Byte-Puffer bündelt Antwortstücke für `/api/library` und `/api/collections`; alte LittleFS-Formate, IDs, Reihenfolgen, Meldungen, API-Namen und Logik von Einträgen/Programmen unverändert.
- Statische Lucide-SVGs direkt in HTML vorgerendert, auch Steuerungsleiste und Menü. JavaScript kann weiterhin dynamische Symbole (Play/Pause, Kurzwahlbearbeitung usw.) austauschen. Alte Canvas-Zeichenroutine ist tolerant gegenüber den vorgerenderten SVG-Containern.
- Lizenztext ist reines MIT; Drittmaterialien und Autorenhinweise bleiben in `THIRD_PARTY_NOTICES.md`.
- Adam Schulz / OpenSign für seine freundliche Protokollhilfe zu `0x04` und BLE-Bestätigungen in README erwähnt, ohne fremden Code zu kopieren.

## Risiko und nächste Schritte

Der sehr niedrige Free-Heap-Tiefstwert kann auch außerhalb der beiden optimierten Endpunkte auftreten; das Netzwerk-/WLAN-Subsystem ist nicht isoliert. Die Verbesserungen sind deshalb **Hypothesen und Code-Optimierungen, keine bestätigte Stabilitätsfreigabe**. Kurzer Hardware-Abnahmetest: `docs/TESTPLAN_v0.4.2.5.md`. Falls Heap-Einbrüche weiter auftreten, Ursache gezielter ermitteln statt weitere pauschale A/B-Testmarathons durchführen.
