# Sicherheitsrichtlinie

RavestickBoard ist ein experimenteller, offline nutzbarer Webcontroller mit WLAN-Access-Point und Bluetooth Low Energy. **Wir geben keine Zusicherung, dass die Steuerung sicherheitsgehärtet, für öffentliche Netze geeignet oder für kritische Einsätze zertifiziert ist.**

## Unterstützung

| Version | Status |
|---|---|
| v0.4.2.4-dev | Entwicklungsstand, keine zugesagte Sicherheitswartung |
| frühere Entwicklungsstände | archiviert, keine zugesagte Sicherheitswartung |

## Sicherheitslücke melden

Für vertrauliche Hinweise nutze die Funktion **„Report a vulnerability“** unter dem GitHub-Tab **Security**, **sofern der Repositoryinhaber private Vulnerability Reports aktiviert hat**. Falls diese Funktion nicht verfügbar ist, bitte **keine Zugangsdaten oder ausnutzbaren Details in einem öffentlichen Issue** veröffentlichen; zunächst über einen verfügbaren privaten Kanal an den Repositoryinhaber wenden. Es gibt derzeit keine garantierte Bearbeitungsfrist.

Sinnvolle Angaben sind Firmwareversion, Bedrohungsszenario, reproduzierbare Schritte, Auswirkungen und eine möglichst datenarme Diagnose. Keine Zugangsdaten, privaten BLE-Adressen oder fremden personenbezogenen Daten senden.

## Sicherheitsgrenzen

- Der voreingestellte WLAN-AP und seine Zugangsdaten sind derzeit **nicht** als individuelle, starke Absicherung gedacht. Eine individuell konfigurierbare Absicherung steht auf der Roadmap.
- Websteuerung und BLE-Befehle sind nicht Ende-zu-Ende-authentifiziert. Der ESP32 ist **nicht** als Netzwerk-Gateway für andere Geräte vorgesehen.
- Firmwareupdates werden nur über USB durchgeführt. **Kein OTA**, kein Ferneingriff in die Herstellerfirmware.
- Neue Abhängigkeiten und Drittanbieterdaten müssen getrennt geprüft werden.
