# Quellen, Codeherkunft und Nutzungsrechte

Stand: 23. Oktober 2026. Dieses Dokument ist eine technische Herkunftsprüfung, **keine anwaltliche Rechtsberatung**. Der derzeitige Status des GitHub-Repositories kann von der bei uns lokal geprüften Paketfassung abweichen.

## Veröffentlichungsentscheidung

Das Repository darf nach Entscheidung seines Eigentümers öffentlich sichtbar sein, bleibt jedoch **ohne Open-Source-Lizenz**. Der Umfang der erlaubten Nutzung des eigenen Quellcodes richtet sich nach der [`LICENSE`](../LICENSE). Für Drittmaterial gelten ausschließlich dessen tatsächliche Rechte und Lizenzbedingungen.

- Der bestehende WLAN-AP-Schlüssel ist **bewusst öffentlich** und darf in Code und Dokumentation verbleiben; er ist dann kein Zugangsschutz gegen Leser des Repositories. Ein konfigurierbarer Zugang wird für eine spätere Firmware erwogen.
- Die fest eingestellte BLE-Geräteadresse darf als Testwert im öffentlichen Quellcode verbleiben; sie ist gerätespezifisch und muss für fremde Geräte ohnehin angepasst werden.
- Das Totem-Foto ist ein eigenes Foto. Der Eigentümer akzeptiert die bestehenden EXIF-Angaben einschließlich iPhone-Modell und Festival-Geotags.
- Nach Angabe des Repository-Eigentümers enthält die Git-Historie nur die in dieser Zusammenarbeit erstellten Projektdateien, keine zusätzlichen Commits, Forks oder fremde Dateien. Die private Git-Historie konnte beim Audit nicht direkt über die verbundene GitHub-Schnittstelle eingesehen werden.

## Herkunft der Protokollkenntnisse

| Referenz | Status | Verwendung in RavestickBoard |
| --- | --- | --- |
| [UpDryTwist/coolledx-driver](https://github.com/UpDryTwist/coolledx-driver) | MIT-Lizenz, Copyright Greg Tatham 2025 | Technische BLE-Protokollreferenz und Kommandobedeutungen; keine eindeutig übernommenen Python-Quelltextpassagen festgestellt |
| [ohtaras/led](https://github.com/ohtaras/led) | Keine allgemeine Quellcode-Nutzungslizenz festgestellt | Protokollreferenz für Paketstruktur und Pixelreihenfolge; **keine nachgewiesene wortgleiche JavaScript-Codeübernahme** in die eigene ESP32-C++-Implementierung |
| [schulzad/coolled-sign-controller / OpenSign](https://github.com/schulzad/coolled-sign-controller) | README erklärt ausdrücklich „all-rights-reserved until one is added“ | Forschungsreferenz für künftige Animationen. Die bestehenden v0.2.0-Quellen enthalten **keinen festgestellten OpenSign-Code**. Lizenzanfrage ist vorgesehen bzw. in Vorbereitung. |

## Geprüfte Projektbereiche und Erkenntnisse

Geprüft wurden die im Projektgespräch verfügbaren Firmwarepakete und Quelltexte der Versionen erster BLE-Test, v0.1.0, v0.1.1, v0.1.2 und v0.2.0 samt Korrekturpaket. Die relevanten C++-Funktionen für Textdaten, Pixelpacking, BLE-Escaping, Chunking und Steuerbefehle wurden mit den dokumentierten Python-/JavaScript-Referenzen verglichen.

| Bereich | Einordnung |
| --- | --- |
| 64×16-Pixelmatrix, RGB-Bitebenen, Übertragungsrahmen, Escape-Regeln, XOR-Checksumme | Technische Eigenschaften des Geräteprotokolls, in Projektcode eigenständig als C++-Algorithmen realisiert; notwendigerweise gleiche Daten- und Befehlswerte |
| Eigene 5×7-Glyphen, zusätzliche Kleinschrift | In der geprüften Firmware als projektspezifische Glyphentabelle enthalten; **kein Import der Originalschriftdateien** festgestellt |
| Webinterface, Seiteneditor, Diagnose, NVS-Seitenverwaltung | Projektbezogene C++-/HTML-/JavaScript-Implementierung; keine unmittelbar übernommenen Fremdmodule oder verdächtigen Lizenzheader in den gelieferten Dateien gefunden |
| Arduino-ESP32-Core, WiFi, WebServer, BLE, Preferences | Verwendete Plattform-Bibliotheken über Arduino-/Espressif-Pakete; keine Quellkopien als Vendor-Dateien in den Projektpaketen erkannt |
| Git-Historie | Angabe des Nutzers berücksichtigt, jedoch **nicht unabhängig vollständig eingesehen** |

**Abgrenzung:** Die gleiche Protokollstruktur ist für kompatible Kommunikation erforderlich und für sich allein kein Beleg einer urheberrechtlich relevanten Codekopie. Umgekehrt kann eine rein automatische Textähnlichkeitsprüfung eine unzulässige Bearbeitung nicht rechtssicher ausschließen. Dieser Audit hat bei den geprüften Dateien **keine konkrete unlizenzierte Codeübernahme nachgewiesen**; das ist keine umfassende rechtliche Freigabe.

## Prüfpunkte bei künftigen Versionen

1. Fremden Quellcode nur übernehmen, wenn dessen Rechte und Nachweispflichten geklärt sind.
2. Bei MIT-lizenzierten **tatsächlich kopierten oder abgeleiteten** Bestandteilen den MIT-Lizenztext und Copyright-Hinweis mitführen. Die bloße Nennung eines Referenzprojekts löst keine automatische Lizenzübernahme aus.
3. Bei OpenSign: Lizenzänderung beziehungsweise ausdrücklich erlaubte Nutzung vor dem Portieren einzelner Codepassagen abwarten.
4. Original-App-Screenshots, Markenlogos und Originalschrift-Pixelvorlagen vor Aufnahme ins öffentliche Repository eigenständig auf Rechte prüfen.
5. Den technisch überprüften, nichtkommerzielle Hobbycharakter des Projekts nicht mit einer Freigabe von Drittcode verwechseln.
