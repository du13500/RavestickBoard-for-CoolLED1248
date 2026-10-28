> Historische Archivrevision. Aktuelle Lizenz: MIT für eigenen Code, Rechte fremder Werke gesondert. Siehe `docs/QUELLEN_UND_LIZENZEN.md`.

# RavestickBoard · Dokumentation des Veröffentlichungsaudits

**Prüfstand: 23.10.2026.** Dieses Protokoll dokumentiert die Grenzen einer technischen Sichtung; es ist **keine rechtliche Freigabebescheinigung**.

## 1. Warum diese Prüfung durchgeführt wurde


## 2. Umfang und konkrete Ergebnisse

| Thema | Dokumentierter Befund / Grenze |
| --- | --- |
| Fremde Codepassagen | **Keine konkrete unlizenzierte Quellcodeübernahme festgestellt.** Protokoll-Paketwerte und Algorithmen teilen sich notwendigerweise Geräteanforderungen. Nicht sämtliche abstrakten Bearbeitungs-/Rechtefragen lassen sich automatisch ausschließen. |
| Geheimnisse | In den überprüften Text-/Projektpaketen keine erkannten API-Keys/Private-Keys. Der **bekannte** WLAN-AP-Schlüssel ist bewusst öffentlich. Neue Commits sind jeweils separat zu prüfen. |
| Geräteadresse | BLE-Testadresse ist gerätespezifisch, soll aber nach Nutzerentscheidung vorerst nicht entfernt werden. |
| Foto | Nutzer bestätigt, dass das Totem-Foto keine unerwünschten persönlichen Angaben zeigt. iPhone-13-Pro-Modell/Geotags vom Festivalgelände dürfen laut Nutzer bleiben; Originaldatei/EXIF wurden in dieser Prüfung nicht unabhängig ausgelesen. |
| Git-Historie | Laut Nutzer ausschließlich von ChatGPT bereitgestellte Projektdateien, keine fremden Forks oder Zusatzdaten. **Historie war für den Assistenten nicht vollständig direkt zugänglich**; die Aussage ist daher eine dokumentierte Nutzerangabe, kein eigener lückenloser Git-Historiennachweis. |
| `LICENSE` | Eigenes Copyright-/Nutzungsdokument erzeugt; wirksam nur für Bestandteile, für die die Rechte tatsächlich beim erklärenden Rechteinhaber liegen. Lizenzen Dritter werden nicht überschrieben. |

## 3. Bewusst akzeptierte Rahmenbedingungen

- **Wi-Fi:** SSID `CoolLED1248`, Kennwort `Banane1248!`. Das Kennwort ist im Code erkennbar, Personen in Funkreichweite könnten sich damit zum AP verbinden. Kein zusätzliches Login im Webinterface. Künftiger Passwortwechsel per Webinterface ist eine optionale Weiterentwicklung.
- **BLE:** Bekannte fest codierte MAC gilt nur für die Referenztafel, keine generische Plug-and-play-Kompatibilität.
- **Foto:** Festival-Geotags und Gerätemodell im EXIF sind akzeptiert.
- **Publikationsmodell:** Eigenes Projekt öffentlich anschaubar, **nicht automatisch frei verwendbar**, und nur für eigene Projektbestandteile werden Rechte vorbehalten.

## 4. Was das Audit nicht behauptet

- Keine Garantie rechtlicher Unbedenklichkeit eines beliebigen späteren GitHub-Commits.
- Keine vollständige Prüfung von privater Git-Historie oder Originalfoto; diese Grenzen wurden offengelegt.
- Keine Aussage, dass v0.2.0 schon universell mit allen CoolLEDX-Geräten funktioniert.

## 5. Nächste Wartungsschritte (kein Veröffentlichungshindernis als Firmwarefeature)

Bei **neuen** externen Codebestandteilen, Glyphen, Grafiken oder Animationen erneut die Herkunft dokumentieren. Der Nutzer hat das feste AP-Passwort, die testgerätespezifische MAC sowie die Foto-EXIF-Daten ausdrücklich akzeptiert; diese sind **keine ausstehenden Pflichtkorrekturen**. Den Dokumentenstand bei Änderungen im Repository aktualisieren.

**Nachtrag 24.10.2026:** Das Repository ist inzwischen tatsächlich **Public**, die eigene Lizenz weiterhin **All Rights Reserved**. In v0.3.0-dev wurden Hersteller-Symbolmatrizen rekonstruiert und als Originalsymbole gekennzeichnet. Der Rechtehinweis beansprucht keine Rechte an Originalgrafiken; eine Quellenangabe allein ist keine etwaig nötige Genehmigung. Vor einer Veröffentlichung des v0.3.0-Quellcodes bleibt die rechtliche Abwägung der Symbolübernahme Aufgabe des Repository-Eigentümers.
