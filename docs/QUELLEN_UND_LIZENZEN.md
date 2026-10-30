# RavestickBoard · Quellen, Lizenzumfang und Rechte

**Aktueller Stand: 26.10.2026 · Projektcode unter MIT.** Diese Übersicht trennt von uns lizenzierbare Bestandteile, fremde Werke und technische Gerätefakten. Sie ersetzt keine individuelle rechtliche Prüfung.

## MIT ab v0.4.0-dev

Wir geben den **eigenständig erstellten Programmcode für Firmware und Webinterface** und unsere eigenen Dokumentationstexte unter der [MIT-Lizenz](../LICENSE) frei. Die vormalige Einstellung **„All Rights Reserved“** war eine frühere, bewusst gewählte Projektphase und ist für die nun unter MIT veröffentlichten eigenen Codebestandteile nicht mehr der geltende Lizenzstatus.

MIT gestattet ausdrücklich auch kommerzielle Nutzung. Dass wir RavestickBoard als privates und überwiegend nichtkommerzielles Festivalprojekt betreiben, ist **keine weitere Lizenzbedingung**.

## Nicht von unserer MIT-Erlaubnis umfasst

Wir können nur Rechte vergeben, die wir selbst besitzen oder wirksam weiterlizenzieren dürfen. Unsere MIT-Lizenz verleiht daher keine Rechte an:

- **Hersteller-App und daraus rekonstruierten Symbolgrafiken oder Schriftzeichen.** Einzelne 16×16-Matrizen wurden nach Screenshots der Original-App rekonstruiert und liegen als Daten in `RavestickBoard_Assets.h` sowie den Webassets. Wir beanspruchen daran keine originären Herstellerrechte und sagen keine MIT-Lizenz für diese fremden Gestaltungen zu.
- **Herstellerlogos, Namen und Marken.** Die Nennung von CoolLED1248/CoolLEDX dient der Kompatibilitätsbeschreibung und bedeutet keine Zusammenarbeit oder Freigabe durch den Hersteller.
- **Fremdcode**, für den uns keine passende Lizenz oder ausdrückliche Erlaubnis vorliegt.
- **Fotos, GIFs und andere externe Materialien**, soweit ihre Rechte nicht ausdrücklich uns gehören. Ein öffentlich sichtbares Bild wird dadurch nicht gemeinfrei.
- **Plattform-Abhängigkeiten** (Arduino-ESP32, BLE, WebServer usw.); deren eigene Lizenzen bleiben wirksam.

Die von uns selbst entwickelten Pixelmotive sind eigenständig erstellte Inhalte; wir führen die Herkunft der Sammlungen getrennt von rekonstruierten Herstellermotiven nach. Dass Web-Buttons die Herkunft der Motive **nicht mehr unterscheiden**, ändert an diesen Rechtevorbehalten nichts.

## Externe technische Referenzen

| Projekt | Bekannter Lizenzstatus / Nutzung | Umgang |
| --- | --- | --- |
| [UpDryTwist/coolledx-driver](https://github.com/UpDryTwist/coolledx-driver) | MIT, Copyright © 2025 Greg Tatham; technische BLE-Protokollreferenz | Bei konkreter Übernahme urheberrechtlich geschützter Codeanteile Attribution und Lizenz erhalten |
| [ohtaras/led](https://github.com/ohtaras/led) | Zum früheren Prüfzeitpunkt keine allgemeine Wiederverwendungslizenz gesichert | Nur technische Fakten untersuchen, keinen unlizenzierbaren Code übernehmen |
| [schulzad/coolled-sign-controller (OpenSign)](https://github.com/schulzad/coolled-sign-controller) | Zum früheren Prüfzeitpunkt „All rights reserved until one is added“ | Keine Quellcodeübernahme/Portierung ohne geeignete Erlaubnis |

Wir haben Adam Schulz am 23.10.2026 um eine Erlaubnis zur Einbeziehung geeigneter OpenSign-Teile gebeten. **Bis zum hier dokumentierten Stand liegt keine bestätigte Erlaubnis vor.** Unsere künftige Recherche zum Animationsbefehl `0x04` trennt ungeschützte Geräteprotokollfakten von konkreter externer Implementierung.

**Historische Einordnung:** Ältere Entwicklungsberichte in `docs/` können weiterhin den früheren Lizenzstand „All Rights Reserved“ nennen. Sie dokumentieren ihren damaligen Zustand; für neu veröffentlichte eigene Codebestandteile gilt die aktuelle [`LICENSE`](../LICENSE) mit diesen Grenzen.

## Rechteprüfung bei neuen Beiträgen

Wir dokumentieren für zusätzliche Quellen künftig URL/Urheber/Lizenzstand, konkrete Verwendung und ggf. abweichende Nutzungsbedingungen. Externe Beiträge nehmen wir erst dann unter MIT an, wenn deren Weiterlizenzierung geklärt ist. Bei eigenen Copyright-Hinweisen und der Verteilung der Software erhalten wir die MIT-Hinweise.

Unsere Diagnoseexports enthalten bewusst keine persönlichen Texte, WLAN-Zugangsdaten oder Bluetooth-Geräteadressen. Das derzeit im Quelltext enthaltene Standard-WLAN-Passwort gilt als öffentliche Beispielkonfiguration und sollte vor einem breiteren Einsatz angepasst werden.
