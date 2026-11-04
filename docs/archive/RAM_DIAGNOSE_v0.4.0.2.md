# 🍌 RAMTRACE – Diagnosekonzept v0.4.0.2-dev

## Warum?

Unsere v0.4.0.1-Läufe zeigten historisch minimale Heap-Werte von 324 B, 48.108 B und 11.052 B. Ein großer Lauftext (30.713 Bilddatenbytes, 240 BLE-Pakete, 53,5 Sekunden Übertragung) funktionierte dennoch. Die bisherigen TXT-Exporte zeigen **nicht**, bei welchem Schritt das Minimum entstand.

## Was wird gemessen?

* `free B`: **aktuell** freier Heap beim Messpunkt.
* `System-Min B`: bisheriger systemweiter Tiefstwert seit Boot, **nicht** zwingend aktuell und unter Umständen zwischen Messpunkten entstanden.
* `maxBlock B`: derzeit größter zusammenhängend allokierbarer Speicherblock.
* `ms`: Zeit seit Boot; `job`: fortlaufende Jobnummer; `jobTyp`: TEXT/PAGE/AMBIENCE/BRIGHTNESS/SPEED/MODE/POWER; `WLAN-Clients`: aktuelle Anzahl.
* `daten`: bei BUFFER_ALLOC/RENDER_END/TX_BEGIN/TX_END Bilddatengröße in Bytes; bei TX_PROGRESS fortlaufende Paketnummer.

Gemessene Phasen:

`BOOT`, `WEB_BEGIN`, `WEB_END`, `STATUS_LOW`, `JOB_BEGIN`, `RENDER_BEGIN`, `BUFFER_ALLOC`, `RENDER_END`, `BLE_BEGIN`, `BLE_LINK`, `BLE_READY`, `TX_BEGIN`, `TX_PROGRESS`, `TX_END`, `RELEASE_BEGIN`, `RELEASE_END`, `JOB_END`.

Bei großen Übertragungen wird **jedes Paket beobachtet**, aber nur alle 32 Pakete und bei neuen beobachteten Tiefstwerten eine Probe im Ring abgelegt. Drei separate Spitzenwerte bleiben bis zum Reset gespeichert, auch wenn der Ring seine ältesten 64 Proben überschreibt. Die Messung verwendet einen statischen Puffer von ca. 1.536 Byte.

## Export und Datenschutz

1. Webinterface → **Diagnose** → **RAM-Verlauf TXT** (`http://192.168.4.1/api/ram.txt`).
2. Zusätzlich **TXT herunterladen** (`/api/diagnostics.txt`) für die üblichen Zähler/Ereignisse und die RAM-Minimumzusammenfassung.
3. Keine Textinhalte, Passwörter oder BLE-Hardwareadressen im RAMTRACE. Beide Logs enthalten nur Daten seit dem letzten Neustart; also **vor** einem Reset exportieren.

## Testablauf

1. Firmware wie gewohnt **ohne „Erase All Flash“** installieren. Bestehende zwölf NVS-Seiten kontrollieren.
2. ESP32 neu starten. Nur ein Smartphone verbinden. RAMTRACE als Baseline exportieren.
3. Kleinen Lauftext senden, danach RAMTRACE exportieren; Werte für `BUFFER_ALLOC`, `RENDER_END`, `BLE_READY`, `TX_END`, `RELEASE_END` vergleichen.
4. 30-KiB-Lauftext senden. **Vor dem Job**, unmittelbar danach und nach erneuter Diagnoseabfrage exportieren.
5. 2, 3 und 4 WLAN-Clients nacheinander verbinden, denselben Test mit kurzem Text wiederholen.
6. Gegebenenfalls mit automatischer 12-Seiten-Folge testen. Jedes Messszenario separat kennzeichnen.

**Hinweise:**
- Das Tracing verändert den RAM-Verbrauch geringfügig. Es ist eine Messsoftware, kein fertiger Stabilitätsfix.
- Das systemweite Minimum zeigt den jemals erreichten Tiefstwert, **nicht** den exakten Allokationsort. Für hundertprozentige Zuordnung wäre tiefergehende Heap-Tracing-Instrumentierung erforderlich.
- Es wurde kein LittleFS eingebunden und keine NVS-Migration durchgeführt. Der Webserver bleibt bei längeren BLE-Übertragungen weiterhin blockiert.
- Wenn `free B` nach vergleichbaren Jobs dauerhaft sinkt oder `maxBlock B` nicht wieder ansteigt, müssen wir Allokationen/Fragmentierung gezielt untersuchen.
