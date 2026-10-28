# Quellen, Credits und Rechte

## Projektcode

Die eigenständig erstellte RavestickBoard-Firmware, das Webinterface und unsere eigenen Dokumentationstexte werden unter der [MIT-Lizenz](../LICENSE) veröffentlicht. Das ursprüngliche Repository war früher nicht freigegeben; diese Entscheidung wurde inzwischen durch MIT ersetzt. **Die MIT-Lizenz bezieht sich nur auf Inhalte, an denen die Projektbeteiligten die entsprechenden Rechte halten.**

## Technische Referenzen, keine automatisch übernommene Codebasis

| Referenz | Rolle | Lizenz-/Verwendungshinweis |
|---|---|---|
| [UpDryTwist/coolledx-driver](https://github.com/UpDryTwist/coolledx-driver) | Frühe öffentliche Dokumentation des CoolLEDX-BLE-Protokolls | MIT; als technische Referenz genannt |
| [ohtaras/led](https://github.com/ohtaras/led) | Zusätzliche frühere Protokollvergleiche | Keine Quellcodeübernahme; Rechte vor möglicher Übernahme separat prüfen |
| [jean-santos/coolled1248-rs](https://github.com/jean-santos/coolled1248-rs) | Referenz für `0x04`, Frameformat und animierte Pixelbilder | MIT/Apache-2.0, konkrete Lizenz vor Übernahme prüfen |
| [zackmcmurrin-dev/coolledux-linux-uploader](https://github.com/zackmcmurrin-dev/coolledux-linux-uploader) | Referenz für native GIF-Programme, RGB444/LZSS und CRC32 | MIT, Hardwarekompatibilität offen |

Bekannte BLE-Befehle, Paketstrukturen und technische Tatsachen sind nicht identisch mit konkretem fremden Quellcode. Die aktuelle v0.4.2 verwendet die bisherige eigene C++-Textübertragung und zusätzliche eigene Blockband-/Programmverwaltung. **Es wurde kein fremdes Animationsverfahren portiert.**

## Hersteller- und Drittassets

- Aus der CoolLED1248-App rekonstruierte Originalschriften und 16×16-Pixelmotive können fremde Schutzrechte berühren. Ihre Dokumentation, Wiedergabe auf der eigenen Tafel und Veröffentlichung sind rechtlich getrennt zu bewerten.
- Unsere selbst gezeichneten Pixelmotive sind separat als eigene Projektmotive gekennzeichnet. Die Daten liegen in den Assets und unter `docs/assets` bzw. den Pixelart-Dateien.
- CoolLED1248 und CoolLEDX sind Hersteller-/Produktkennzeichen und keine Projektmarken. Das eigene Totem-Foto und die selbst gestaltete Homescreen-Banane stammen aus den Projektassets.
- Derzeitiger Einsatz ist privat und nichtkommerziell. Das ersetzt keine möglicherweise erforderlichen Nutzungsrechte an fremden Originalgrafiken.

**Hinweis:** Ältere Versionsdateien dürfen als technische historische Dokumente abweichende Entwicklungsstände beschreiben. Der aktuelle Lizenzstatus richtet sich nach der Root-`LICENSE`.
