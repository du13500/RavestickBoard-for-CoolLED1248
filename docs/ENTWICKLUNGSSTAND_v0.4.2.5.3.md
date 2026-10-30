# Entwicklungsstand v0.4.2.5.3-dev

Bibliothekssuche und manueller Reload wurden aus der Oberfläche und dem dazugehörigen UI-Code entfernt. Die Bibliothek wird weiterhin automatisch aus der `/api/library`-API geladen: beim Öffnen, beim Start und nach jeder gespeicherten Änderung. Kein neuer Suchindex und keine Änderung an den persistenten Datensätzen.

Unverändert: acht Kurzwahlen, adaptive Startseite, Helligkeit/Tempo, BLE, LittleFS/NVS-Layout und Diagnosen. Der weiterhin ungeklärte historische Heap-Tiefstwert bleibt ein separates Stabilitätsthema.

Verifiziert: statische UI-Prüfungen, gzip/ETag-Konsistenz, JS-Syntax und Browsersimulation. Noch nicht verifiziert: Arduino-Build und echte Hardware.
