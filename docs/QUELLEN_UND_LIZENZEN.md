# RavestickBoard · Quellen, Codeherkunft und Nutzungsrechte

**Stand: 23.10.2026.** Technische Herkunftsdokumentation, **keine anwaltliche Rechtsberatung**. Für die ursprünglichen Quellen und den Prüfumfang siehe auch [`VEROEFFENTLICHUNGS_AUDIT_2026-10-23.md`](VEROEFFENTLICHUNGS_AUDIT_2026-10-23.md).

## Projekt- und Lizenzentscheidung

Die Repository-Sichtbarkeit und eine Open-Source-Lizenz sind unterschiedliche Entscheidungen. Für **eigene, vom Rechteinhaber kontrollierte Projektbestandteile** gilt die [`LICENSE`](../LICENSE): **All Rights Reserved**, keine allgemeine Erlaubnis zum Kopieren, Bearbeiten oder Weiterverbreiten. GitHub-Forking-/Viewing-Rechte, geltendes Recht und **fremde Lizenzen** bleiben unberührt.

Der Nutzer möchte das Projekt bei Bedarf öffentlich präsentieren, u. a. damit OpenSign-Entwickler einen Einblick erhalten. **Der tatsächliche aktuelle GitHub-Sichtbarkeitsstatus war in diesem Dokumentenreview nicht unabhängig abrufbar** und wird nicht als technisch bestätigte Tatsache behauptet. Eine Entscheidung, Code offen zu lizenzieren, wurde **nicht** getroffen.

## Dokumentierte Fremdprojekte

| Quelle | Lizenzstand beim Prüfen 23.10.2026 | Nutzung / Grenze |
| --- | --- | --- |
| [UpDryTwist/coolledx-driver](https://github.com/UpDryTwist/coolledx-driver) | **MIT-Lizenz**, Copyright © 2025 Greg Tatham | Technische BLE-Protokollreferenz. Bei **tatsächlicher** Übernahme geschützter Codebestandteile MIT-Lizenz- und Copyright-Hinweise mitführen. |
| [ohtaras/led](https://github.com/ohtaras/led) | Keine allgemeine Code-Wiederverwendungslizenz festgestellt | Protokoll-/Rendererreferenz; unlizenzierte Quellcodepassagen nicht kopieren oder nahezu wortgetreu portieren. |
| [schulzad/coolled-sign-controller (OpenSign)](https://github.com/schulzad/coolled-sign-controller) | README erklärt ausdrücklich „all rights reserved until one is added“, **keine Lizenzdatei** | Wertvolle Referenz zur autonomen Animation, aber keine Nutzungserlaubnis für dessen konkreten Python-Code. |

**Stand OpenSign-Kontakt:** Der Nutzer hat im Browser eine englische GitHub-Lizenzanfrage an Adam vorbereitet und in das Issue-Formular eingetragen. **Das tatsächliche Absenden und eine Antwort/Erlaubnis sind aus dem bisherigen Gespräch nicht bestätigt.** Bis zur ausdrücklichen Erlaubnis: kein Kopieren/Portieren von OpenSign-Implementierung. Ein öffentliches Repository oder nichtkommerzielle Nutzung schaffen keine automatische Weiterverwendungslizenz.

## Technische Protokollkenntnisse gegenüber Fremdcode

In den bei der ursprünglichen Prüfung verfügbaren Firmwarepaketen (FirstText, v0.1.0, v0.1.1, v0.1.2, v0.2.0 Korrektur 1) wurde **keine konkret nachweisbare Übernahme unlizenzierter Quellcodepassagen** festgestellt. Die Funktionen für ESC-Byte-Framing, 128-Byte-Chunks, Checksummen, RGB-Bitebenen und BLE-Kommandos implementieren gemeinsame technische Geräteanforderungen. Gleiche Paketwerte allein belegen keine Codekopie.

**Abgrenzung:** Diese Feststellung ist ein technischer, begrenzter Prüfbefund und **keine rechtssichere Garantie**. Eine automatische Textsuche kann auch sinngemäße Codebearbeitungen nicht vollständig ausschließen. **Die tatsächliche private Git-Historie wurde nicht unabhängig ausgelesen**; laut Nutzer enthält sie nur die zuvor hier gemeinsam erzeugten Projektdateien.

Die aktuelle v0.2.0-Firmware ist in C++ für ESP32-C3 geschrieben. Protokollreferenz ist nicht identisch mit der Übernahme eines geschützten Python- oder JavaScript-Implementierungstexts. Eigenständig erstellte Glyphen sind nicht als originaler Herstellerfont auszugeben.

## OpenSign-Animation: externer Forschungsbefund

OpenSign dokumentiert auf **seiner** 64×16-CoolLEDX-Referenztafel (Firmwarekennung 6) gespeicherte Animationen per `0x04`, ungefähr **30 KiB / 80 Frames** sowie eine **gemeinsame Haltezeit pro Frame**. Es beschreibt „store-and-loop“ und keinen bestätigten dauerhaften Frame-Stream.

**Diese Angaben sind nicht auf unsere Tafel übertragen oder verifiziert.** Ein eigenes Testpaket und reale Messungen (Frames, ACKs, Timeraster, Übertragung, Stromausfall, sichtbare Anzeige) sind in [`THEMENSPEICHER.md`](THEMENSPEICHER.md) unter `ANM-01` bis `ANM-14` vorgemerkt. Die kompatible eigene Implementierung wird anhand eigener Tests und technischer Fakten erstellt oder nach eindeutig erteilter Erlaubnis.

## Weitere Rechte und bewusst akzeptierte Veröffentlichungspunkte

- **WLAN-AP-Schlüssel:** `Banane1248!` ist vom Nutzer **bewusst zur Dokumentation freigegeben**; spätere UI-Konfiguration ist optional. Für Nutzer in Funkreichweite ist der AP damit nicht geheim.
- **BLE-Testadresse:** `ff:00:00:03:7e:c3` identifiziert nur die aktuell getestete Tafel und ist kein Geheimnis; spätere Gerätesuche geplant.
- **Eigenes Totem-Foto:** EXIF-Angaben zum iPhone-Modell und Festival-Geotags sind dem Nutzer bewusst und akzeptiert.
- **Original-App-Screenshots, Fonts, Herstellerlogos:** Vor Einbindung in ein öffentlich sichtbares Repository gesondert auf Rechte prüfen. Referenzaufnahmen im Chat sind nicht automatisch zur öffentlichen Wiederverwendung freigegeben.
- **Plattformbibliotheken:** Arduino-ESP32-Core, WiFi, WebServer, BLE, Preferences werden über ihre jeweiligen Paket-/Abhängigkeitslizenzen bereitgestellt. Eine eigene `LICENSE` im Repository ändert diese Rechte nicht.

## Laufende Pflege

1. Jede neue externe Quelle mit URL, Lizenzstand, Abrufdatum und konkret verwendetem Inhalt dokumentieren.
2. Zwischen protokolltechnischen Fakten, **eigener** Implementierung und tatsächlich übernommener Codepassage unterscheiden.
3. Bei MIT-Codeübernahme Copyright und erforderliche Lizenzhinweise mitliefern; bei unlizenzierter Quelle erst schriftliche Freigabe erhalten.
4. Änderungen an einer tatsächlichen späteren OpenSign-Erlaubnis **wörtlich und mit Datum dokumentieren**; nicht aus einer freundlichen Antwort pauschale Rechte ableiten.
5. Nach Import neuer Fonts/Bilder/Animationen Provenienz erneut prüfen.
