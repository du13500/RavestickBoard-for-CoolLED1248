# Quellen, Fremdcode und GitHub-Veröffentlichung

## Genutzte technische Referenzen (22.10.2026 überprüft)

| Repository | Zweck | Erkannter Lizenzstatus | Konsequenz |
| --- | --- | --- | --- |
| [UpDryTwist/coolledx-driver](https://github.com/UpDryTwist/coolledx-driver) | BLE-Protokoll, Datenrahmung, Pixel- und Kommandomodelle | **MIT-Lizenz** | Übernommene, urheberrechtlich geschützte Implementierungsanteile müssten die MIT-Lizenzhinweise erhalten. |
| [ohtaras/led](https://github.com/ohtaras/led) | Web-Bluetooth-Ansatz, Protokoll- und RGB-Rendering-Beispiele | **Keine Lizenzdatei / keine erkannte GitHub-Lizenz** | Nicht einfach Quellcode kopieren oder als frei lizenziert annehmen. |
| [schulzad/coolled-sign-controller](https://github.com/schulzad/coolled-sign-controller) | Weitere Analyse/Referenz für CoolLEDX | **Keine Lizenzdatei / keine erkannte GitHub-Lizenz** | Vor Codeübernahme Zustimmung oder eindeutige Lizenzlage klären. |

**Wichtig:** Im aktuellen Arduino-Sketch steht ausdrücklich, dass die Protokollimplementierung *auf den dokumentierten Ansätzen* dieser Projekte beruht. Die aktuell genutzten eigenen Glyphendefinitionen sind nicht als Original-App-Font oder als originale UpDryTwist-Schrift auszugeben.

## Veröffentlichungsstrategie

1. **Git lokal** für Historie und Dateisicherung sofort möglich, auch ohne GitHub-Account oder Online-Repository.
2. Erst den Arduino-Quellcode und insbesondere framing/chunking/rendering auf Herkunft prüfen. Die Arbeit am gemeinsamen technischen Protokoll ist nicht automatisch identisch mit urheberrechtlich geschütztem Fremdcode; nahe Codeübernahmen können aber lizenzrelevant sein.
3. Für jede Datei Herkunft dokumentieren. Code ohne klare Freigabe nicht aus unlizenzierter GitHub-Quelle übernehmen.
4. Bei MIT-Codeübernahmen Lizenztext/Urheberhinweis beilegen. Zusätzliche Nutzungsbedingungen anderer Bestandteile prüfen.
5. Original-App-Screenshots, fremde Icons und Originalfont-Pixelvorlagen **nicht ungeprüft** ins öffentliche Repo legen.
6. Auch harte Zugangsdaten und gerätespezifische Konfiguration (z. B. WLAN-Passwort, BLE-Adresse) vor Veröffentlichung entfernen oder neutral vorbelegen.
7. Erst danach eigene Projektlizenz festlegen und bei Bedarf **öffentlich** machen. Bis dahin lokal oder privat halten.

**Keine Aussage einer Rechtsprüfung:** GitHub-Metadaten und Wurzeldateien geben nur einen ersten Lizenzhinweis. Fehlende Lizenzdatei heißt nicht „kostenlos verwendbar“.

## Zu prüfende Dateien im aktuellen Stand

- [ ] `firmware/CoolLED1248_FirstText.ino`: Protokoll-/Rendering-Codeherkunft dokumentieren.
- [ ] `firmware/CoolLED1248_Web_v0_1_0.ino`: Protokoll-/Rendering-Codeherkunft dokumentieren.
- [ ] Schriftdefinitionen und GUI-Elemente aus eigenen Entwürfen vs. Original-App/Referenzen abgrenzen.
- [ ] Keine übernommenen Codeabschnitte aus `ohtaras/led` ohne klare Erlaubnis verwenden.
- [ ] Drittlizenzen und Quellen in einer späteren `NOTICE`-Datei erfassen, sofern relevant.
