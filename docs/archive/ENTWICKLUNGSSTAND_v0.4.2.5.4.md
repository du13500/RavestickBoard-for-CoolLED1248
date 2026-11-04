# Entwicklungsstand v0.4.2.5.4-dev

Im bestehenden Lauftext- und Blockeditor ist bei geöffneten Bibliotheksvorlagen nun beides möglich: aktualisieren unter derselben ID oder **als neuen Eintrag speichern** (neue ID, altes Original unangetastet). Das neue Namensfeld wird mit dem bisherigen Titel plus ` (Variante)` vorbelegt. Nach dem Anlegen bearbeiten wir im Editor die neue ID.

Beim Font-Wechsel im Lauftext bleibt die Bindung zur Bibliotheksvorlage jetzt erhalten. Die vorherige automatische Aufhebung der Bindung war ein Fehler, außerdem wurde dadurch der Änderungsstatus unabsichtlich als sauber markiert.

Dokumentiert: Bei Änderungen auf anderen verbundenen Smartphones **Menü → Webinterface neu laden** verwenden; parallele Bearbeitung desselben Eintrags kann zum Überschreiben führen. Keine separate Reload-Taste in der Bibliothek nötig.

Unverändert: 8 Kurzwahlen, keine Suche/kein Favoritenstern, gzip/ETag, JSON-Puffer, BLE und Speicherformat. Kritische Heap-Tiefstwerte weiterhin offen. Offline mit Browser-Mocks geprüft, jedoch weder durch Arduino kompiliert noch auf ESP32-Hardware getestet.
