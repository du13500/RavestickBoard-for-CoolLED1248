# Entwicklungsstand v0.4.2.4-dev

## Was wir bereits bestätigt haben

Beim Hardwaretest von v0.4.2.2-dev funktioniert die gemischte Abfolge Lauftext → Blockseiten → Lauftext erstmals; reine Programme laufen autonom. Bei Tempo 80 haben wir vier Blockseiten mit 6,33 s beobachtet (Prognose 6,31 s). Die Übergänge in gemischten Programmen sind trotzdem noch nicht synchron, die Geschwindigkeitsregler-Sperre verhielt sich einmal unerwartet. Bibliothekseinträge können wir aktualisieren, löschen, reparieren und über bestehende IDs erhalten.

## Was bereits in v0.4.2.3-dev hinzukam

Die alternative Programmwiedergabe „Alles als Laufband“, offline eingebundene Lucide-SVGs, acht Modus-Direktbefehle, der direkt einrichtbare leere Kurzwahlplatz und der Editor-Neuanfang waren in v0.4.2.3 bereits enthalten. Diese Version hat auf der echten Tafel ein **kurzes** gemischtes Laufband mit Schrift-/Farberhaltung erfolgreich gezeigt, aber bei mehreren Übertragungsgruppen fehlerhafte 20-s-Wechsel und einen BLE-Bestätigungsabbruch verursacht.

## Was v0.4.2.4-dev tatsächlich neu bringt

Gemischte Laufbänder werden ohne vollständigen Bildpuffer im 384-B-Tile-Streaming gerendert; fehlerhafte Bild-Paketübertragung erhält maximal einen vollständigen BLE-Neuversuch. Die alte manuelle Gruppendauer wird nur noch aus Gründen der Datenkompatibilität mitgeführt. Das Webinterface bekommt die einheitliche untere Tafelsteuerung, kompaktere Kurzwahlen, reine Vorschau-Icons (Schnecke, Vogel, Hase), `bug` für Diagnose und eine reduzierte Menünavigation. Roadmap/Themenspeicher und Community-Unterlagen wurden abgeglichen. Unsere MIT-`LICENSE` und separate Lucide-/Feather-Lizenzen bleiben erhalten.

## Unsicherheit

v0.4.2.3-dev wurde auf der Hardware erfolgreich kompiliert und bei kurzen gemischten Laufbändern getestet. Beim geteilten Band traten abgeschnittene Gruppen und ein BLE-Paketabbruch auf. v0.4.2.4-dev enthält einen erneuerten Tile-Renderer, einen einmaligen BLE-Vollretry und eine konservative Zeitschätzung; diese Korrekturen sind noch nicht auf der Hardware bestätigt. Das 30-KiB-Limit ist vorsichtig gewählt. Hardwarebestätigung für v0.4.2.2 gilt nicht automatisch für diese neue Renderingvariante.
