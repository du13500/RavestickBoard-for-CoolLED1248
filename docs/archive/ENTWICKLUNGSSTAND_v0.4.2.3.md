# Entwicklungsstand v0.4.2.3-dev

## Was wir bereits bestätigt haben

Beim Hardwaretest von v0.4.2.2-dev funktioniert die gemischte Abfolge Lauftext → Blockseiten → Lauftext erstmals; reine Programme laufen autonom. Bei Tempo 80 haben wir vier Blockseiten mit 6,33 s beobachtet (Prognose 6,31 s). Die Übergänge in gemischten Programmen sind trotzdem noch nicht synchron, die Geschwindigkeitsregler-Sperre verhielt sich einmal unerwartet. Bibliothekseinträge können wir aktualisieren, löschen, reparieren und über bestehende IDs erhalten.

## Was v0.4.2.3-dev hinzufügt

Zweite Wiedergabeart „Alles als Laufband“, offline verfügbare Lucide-SVG-Steuerelemente inklusive acht Modus-Direkttasten, editorseitiger Neuanfang und direkte Einrichtung leerer Kurzwahlen. Beim Backup-Zusammenführen werden identische Inhalte anhand der Nutzdaten und des Namens erkannt und wiederverwendet; abhängige IDs werden zugeordnet. Dieselbe Programmbinärstruktur (RBP2, reserviertes Byte jetzt Modus) wird weiterbenutzt. Lizenz- und Quellenangaben sind in LICENSE und THIRD_PARTY_NOTICES.md dokumentiert.

## Unsicherheit

Für diese neue Version liegen noch keine aktuellen Tests des ESP32-C3-Builds, Heap-Maximums, realen Bluetooth-Uploads eines gemischten Scrollbands und der zeitlichen Übergänge vor. Das 30-KiB-Limit ist vorsichtig gewählt. Hardwarebestätigung für v0.4.2.2 gilt nicht automatisch für diese neue Renderingvariante.
