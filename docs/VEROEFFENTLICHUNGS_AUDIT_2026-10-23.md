# Veröffentlichungsaudit, Stand 23.10.2026

**Ergebnis:** In den verfügbaren Firmwarepaketen wurde keine konkret nachweisbare unlizenzierte Codeübernahme festgestellt. Die öffentliche Sichtbarkeit unter `LICENSE` (All Rights Reserved) ist eine bewusste Entscheidung des Eigentümers.

**Prüfumfang:** Lokale Arduino-Sketches und Versionspakete von erstem BLE-Test bis v0.2.0 Korrektur 1; projektspezifische Weboberfläche, Dokumentation und benannte Referenzen (`UpDryTwist`, `ohtaras`, `OpenSign`). Keine vollständige Einsicht in die private GitHub-Historie oder das lokale Originalfoto. Die Angaben des Eigentümers zur Git-Historie und zu akzeptierten EXIF-Daten sind erfasst.

**Abgrenzung:** Die technische Übereinstimmung mit dem dokumentierten BLE-Protokoll ist kein Beleg für kopierten Code. Das Ergebnis ersetzt keine rechtsverbindliche Aussage über sämtliches geistiges Eigentum Dritter.

**Erkannte Risikohinweise:** Der AP-Zugangsschlüssel `Banane1248!` ist im öffentlichen Sketch bekannt und damit bewusst kein Geheimnis. Der BLE-Gerätebezeichner und die fest hinterlegte Geräteadresse sind spezifisch für die verwendete Hardware. Zukünftige Änderungen können neue Lizenzfragen eröffnen.

**Vor Public:** `LICENSE` ins Repository übernehmen; README und `docs/QUELLEN_UND_LIZENZEN.md` auf den aktuellen Status bringen. Anschließend kann der Eigentümer in GitHub unter Settings → Danger Zone → Change repository visibility selbst auf Public umstellen.
