#!/usr/bin/env python3
"""Synchronisiert web/index.html mit dem im Arduino-Sketch eingebetteten HTML.

Ausführen im Repository-Verzeichnis: python tools/sync_web.py
Arduino benötigt weiterhin ausschließlich die .ino-Datei.
"""
from pathlib import Path

root = Path(__file__).resolve().parent.parent
sketch = root / 'firmware' / 'RavestickBoard_v0_1_2.ino'
html = root / 'web' / 'index.html'
source = sketch.read_text(encoding='utf-8')
start_token = 'static const char PAGE[] PROGMEM = R"HTML('
end_token = ')HTML";'
assert source.count(start_token) == 1 and source.count(end_token) == 1
markup = html.read_text(encoding='utf-8')
assert end_token not in markup, 'HTML kollidiert mit Raw-String-Delimiter.'
begin = source.index(start_token) + len(start_token)
end = source.index(end_token, begin)
patched = source[:begin] + '\n' + markup + '\n' + source[end:]
sketch.write_text(patched, encoding='utf-8')
print(f'Synchronisiert: {html.relative_to(root)} -> {sketch.relative_to(root)}')
