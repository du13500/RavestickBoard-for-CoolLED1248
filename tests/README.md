# Offline-Tests

Aktuell: `python tests/test_build_425.py`. Der Test validiert Firmwareversion, Datenformat-Konstanten, gzip-/ETag-Generator, Prä-Rendering der statischen Lucide-Icons, JavaScript-Syntax und einen kompilierten C++-Stub für den 1024-Byte-JSON-Puffer.

Diese Prüfungen sind **kein vollständiger Arduino-Build**, kein Test der ESP32-WebServer-API zur Laufzeit und kein Nachweis der Heap- oder BLE-Stabilität. Historische Testprotokolle unter docs/ bleiben Referenzen.
