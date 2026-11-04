# v0.4.2.4.2-dev-A/B – HTTP-/RAM-Vergleichstest

**Ziel:** Die beobachteten Heap-Einbrüche auf bis zu 1.076 Byte System-Minimum unter v0.4.2.4.1-dev bei zeitweise zwei WLAN-Clients eingrenzen. Die Ursache ist noch unbekannt. Ein unkomprimiertes A wird zuerst gemessen; B nutzt exakt dieselbe Instrumentierung, komprimiert aber die Startseite und bietet Browser-Cache-Revalidierung.

## Die beiden Builds

| Build | Sketch | HTTP-Startseite | Browsercache | Gemeinsame Messung |
| --- | --- | --- | --- | --- |
| A | `firmware/RavestickBoard_v0_4_2_4_2_A/RavestickBoard_v0_4_2_4_2_A.ino` | `send_P` aus Flash, unkomprimiert | `no-store` (Altverhalten) | 20-ms-Messung, `/api/http.txt`, 404 erfasst |
| B | `firmware/RavestickBoard_v0_4_2_4_2_B/RavestickBoard_v0_4_2_4_2_B.ino` | gzip-9 aus Flash, binäre Länge explizit | ETag (`If-None-Match` → 304), `no-cache` | exakt dieselbe Messung |

Das zentrale `web/index.html` ist in beiden Builds **inhaltlich identisch**. `python tools/sync_web.py` erzeugt beide Header. Die A/B-Sketches sind eigenständige Arduino-Projekte; nicht versehentlich nur einen Header austauschen. Das alte v0.4.2.4.1-dev als Rückfall behalten.

## Vor jedem Durchgang

1. **JSON-Bibliotheksbackup sichern** und den bisherigen Firmwarestand beibehalten. Im Arduino-Menü Board **ESP32C3 Dev Module**, Core 3.3.12, USB CDC an und **unveränderte 2MB-APP/2MB-SPIFFS-Partitionierung**. **Erase All Flash deaktivieren**, ggf. 115200 Baud.
2. **A flashen. ESP32 neu starten.** Neues Boot-Systemminimum wird nur durch Neustart zurückgesetzt. Startzeit und Version `v0.4.2.4.2-dev-A` im Diagnoseexport kontrollieren. Erst danach messen.
3. Browsercache der Geräte bewusst behandeln (bei A unwesentlich). **Ein Gerät** anmelden, 20-mal die Startseite neu laden. Danach `/api/http.txt`, `/api/ram.txt` und `/api/diagnostics.txt` speichern, **bevor** ein neuer Test die 32 Einträge überschreibt oder ein Neustart die Logs löscht.
4. ESP32 **neu starten**. **Zwei Geräte** anmelden, das zweite zunächst nur mit WLAN verbinden und mindestens 30 Sekunden untätig lassen. HTTP-/RAM- und Diagnoseexport sichern. Prüfen, ob unbekannte Prüfpfade wie `/hotspot-detect.html` oder `/generate_204` auftauchen. Ihr Fehlen schließt externe Verbindungsprüfungen nicht aus.
5. ESP32 erneut **neu starten**. Zwei Geräte anmelden, beide Seite laden / wiederholt neuladen (20 pro Gerät). Exporte sichern. Ein früheres System-Minimum aus einer anderen Messreihe nicht übernehmen.
6. **B flashen** und die drei Messreihen mit identischem Ablauf und frischem Neustart wiederholen. HTTP 200 und 304 ausdrücklich unterscheiden. Ein neues privates Browserfenster kann den ersten 200-Aufruf erleichtern, garantiert aber nicht, dass alle folgenden Aufrufe 200 sind. Für einen erzwungenen 200-Aufruf `If-None-Match` vermeiden (neue Browser-Session bzw. Cache gezielt leeren).

Für eine saubere Aussage zu HTTP-Kompression **die 200-Aufrufe** von A und B vergleichen; 304-Aufrufe separat als Cache-Effekt auswerten. Die Weboberfläche und BLE-Funktionen sollten dabei unverändert nutzbar sein. Bei einem Crash Resetgrund und Bootlog separat sichern.

## HTTP-Protokoll verstehen

`http://192.168.4.1/api/http.txt` liefert die letzten **32** bearbeiteten HTTP-Anfragen im RAM sowie **aggregierte Kennzahlen je Kategorie seit Boot** (Startseite, 404, APIs, Icons, sonstige). Für konkrete Endpunkt-Zuordnung trotzdem öfter exportieren. Die 20-ms-Stichproben zeigen Speicher-Minima **während des Handlers**. Die Aufgabe verwendet `esp_timer` statt eines eigenen FreeRTOS-Tasks, benötigt aber trotzdem etwas RAM und Rechenzeit. Einige Peaks sind kürzer als 20 ms; insbesondere TCP-/WLAN-Puffer können **auch außerhalb einer HTTP-Anfrage** Speicher belegen. Darum protokollieren wir zusätzlich das gemessene Idle-Minimum und die historische System-Minimum-Entwicklung.

`Heap vor/min/nach` und `Block vor/min/nach` sind frei gemessene Werte, **nicht der zusätzliche Verbrauch der Seite**. `SystemMin vor/nach` ist der historische niedrigste freie System-Heap seit Boot und kann nie steigen. Zwei verbundenen Clients bedeutet nicht automatisch zwei gleichzeitige HTTP-Anfragen. Der 404-Handler zeichnet unbekannte Pfade, aber **keine Query-Strings** auf.

**Interpretation vorsichtig halten:** Ein 404-Protokolleintrag beweist nur, dass eine Anfrage unseren HTTP-Server erreicht hat. Ein fehlender Eintrag beweist keine ausbleibende iOS-/Android-Konnektivitätsprüfung; DNS-Auflösung oder externes Routing können vorher scheitern. Keine Host-, Query-, Text-, Kennwort- oder BLE-Adressdaten exportieren.

## Erfolgskriterien

- Beide Builds kompilieren und starten ohne Panic; Bibliothek und Kurzwahlen erhalten.
- `GET /` = 200 in A und B, identischer sichtbarer Inhalt. B muss `Content-Encoding: gzip` und korrektes `Content-Length` senden; Header-`If-None-Match` mit ETag ergibt 304.
- 404 unbekannter Pfad im HTTP-Export; auch `/api/status` und `/api/http.txt` werden mit Pfad protokolliert.
- Unter gleicher Belastung deutlich mehr Speicherreserve bzw. weniger tiefe Minima in B (noch kein Ergebnis!). Ein möglicher Unterschied muss mit HTTP-Status und Clientzahl erklärt werden.
- Falls System-Minima wieder sehr niedrig ausfallen, Test abbrechen, Logs sichern; Ursache weiter untersuchen, statt daraus voreilig einen Erfolg abzuleiten.

**Nicht in diesen Testbuilds enthalten:** die offenen Claude-Reviewthemen F-01/F-02/F-03 (Schema-Migration, Webserver während BLE und Validierung der BLE-Bestätigungen) und die neuen UI-Wünsche (Bibliothek kopieren, Programmkapazitätsbalken, Banane beim Laden).
