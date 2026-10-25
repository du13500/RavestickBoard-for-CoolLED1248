# Repository aufräumen: geprüfte Empfehlungen · 25.10.2026

**Diese Liste ist eine Empfehlung, keine automatische Löschung.** Grundlage ist die tatsächliche `main`-Dateiliste auf GitHub am 25.10.2026 und die inhaltliche Sichtung der genannten Textdateien. Im Firmware-ZIP wird nichts gelöscht oder verschoben. Am besten nach dem v0.3.2-Test als **separater Commit** bereinigen.

## Kann aus meiner Sicht weg (nach eigener Sichtkontrolle)

| Datei | Prüfung | Empfehlung |
|---|---|---|
| `TEST_HINWEIS.txt` | Bezieht sich ausschließlich auf den nicht mehr aktuellen v0.1.1-Testkandidaten und nennt v0.1.0 als letzte funktionierende Version. | **Löschen**: damalige Historie bleibt über `CHANGELOG.md` und Git erhalten. |
| `UPDATE_ANLEITUNG.txt` | Beschreibt das Einspielen von v0.1.1, nennt eine alte Sketch-Dateiposition und bezeichnet das Repo noch als PRIVATE. | **Löschen**: aktuelle Update-Anleitung steht im README. |
| `RELEASE_NOTES.txt` | Bezieht sich ausschließlich auf v0.2.0 Korrektur 1; Teile des damaligen Status sind überholt. Die relevanten Änderungen und der Arduino-Hotfix stehen im Changelog. | **Löschen oder in `docs/archive/` verschieben**. |

## Aufbewahren oder bewusst archivieren

| Datei | Prüfung | Empfehlung |
|---|---|---|
| `ENTWICKLUNGSSTAND_v0.3.0.md` | Wertvolle technische Momentaufnahme mit originalem Prüfstand *vor* dem späteren Hardwaretest; beschreibt unter anderem Pixeldaten und vorhandene Risiken. | **Nach `docs/archive/` verschieben**, nicht ersatzlos löschen. Historisches „ungetestet“ als damalige Momentaufnahme stehenlassen. |
| `docs/VEROEFFENTLICHUNGS_AUDIT_2026-10-23.md` | Dokumentiert Lizenz-, Herkunfts-, WLAN-/BLE- und Veröffentlichungsentscheidungen; nicht mit einem normalen Changelog austauschbar. | **Behalten**, gern unter `docs/archive/` als datiertes Protokoll; nicht löschen. |
| `docs/preview_v0.3.0-dev.png` | Momentan keine interne Textreferenz in der hochgeladenen Repositorykopie festgestellt. | Optional archivieren oder löschen, sofern das Bild als Entwurfsbeleg nicht mehr gebraucht wird. |
| `docs/ravestick_banana_v0.3.0-dev.png` | Derzeit keine interne Textreferenz festgestellt; dokumentiert aber die erste Bananenfassung. | Für Motivhistorie **archivieren** oder bewusst behalten. |

## Sollte bleiben

- **Kern:** `README.md`, `CHANGELOG.md`, `LICENSE`, `.gitignore`, `.gitattributes`.
- **Laufende Dokumentation:** `docs/THEMENSPEICHER.md`, `docs/ROADMAP.md`, `docs/ENTSCHEIDUNGEN.md`, `docs/TECHNIK.md`, `docs/TESTPLAN.md`, `docs/DIAGNOSE.md`, `docs/QUELLEN_UND_LIZENZEN.md`.
- **Quelle & Darstellung:** `assets/README.md` erläutert die getrennt verwahrten Original-JSON-Quellen; `docs/assets/totem.jpg` wird im README benutzt.
- **Firmware:** Alle bisherigen Versionsordner und frühen `.ino`-Experimente als reproduzierbare Entwicklungsstände. Bei den zwei losen Sketchen ist später eine Verschiebung nach `firmware/archive/` möglich, aber nicht nötig.
- **Werkzeuge & Browser:** `tools/generate_assets.py`, `tools/sync_web.py`, `web/index.html` (letztere soll mit dem aktuellen Firmware-WebUI-Header übereinstimmen).

## Wichtig beim Aufräumen

Verschobene Dateien müssen in `README.md` und anderen Querverweisen berücksichtigt werden, falls vorhanden. **Nicht den versteckten `.git`-Ordner bearbeiten.** Erst v0.3.2 testen, danach ein separater GitHub-Desktop-Commit für die Löschungen/Verschiebungen. Der Auditbericht ist ein historischer Beleg, keine aktuelle Freigabe; er sollte als solcher gekennzeichnet bleiben.
