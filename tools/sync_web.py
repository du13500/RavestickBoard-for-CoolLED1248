#!/usr/bin/env python3
"""Synchronize ONE web/index.html into two test firmware builds.
Build A: plain PROGMEM HTML. Build B: reproducible gzip-9, mtime=0 + ETag SHA-256.
Do not flash only a regenerated header; always flash its matching sketch.
"""
from pathlib import Path
import gzip, hashlib

ROOT = Path(__file__).resolve().parents[1]
html = (ROOT / "web/index.html").read_text(encoding="utf-8").rstrip("\n")
if ')RBHTML"' in html:
    raise SystemExit("Unsupported Arduino raw literal delimiter in HTML")
# Keep the existing raw-literal byte envelope in A, and gzip precisely those bytes in B.
payload = ("\n" + html + "\n").encode("utf-8")
raw = '#pragma once\n// Build A: flash-resident HTML (no compression). Source: web/index.html\nstatic const char PAGE_HTML[] PROGMEM = R"RBHTML(\n'+html+'\n)RBHTML";\n'
a = ROOT / 'firmware/RavestickBoard_v0_4_2_4_2_A/RavestickBoard_WebUI.h'
a.write_text(raw, encoding='utf-8')
encoded = gzip.compress(payload, compresslevel=9, mtime=0)
etag = '"'+hashlib.sha256(payload).hexdigest()[:24]+'"'
hex_lines = []
for i in range(0, len(encoded), 16):
    hex_lines.append('  '+', '.join('0x%02x'%x for x in encoded[i:i+16])+',')
header = '#pragma once\n// Build B: compressed flash HTML (gzip-9); source web/index.html.\n'
header += 'static const uint8_t PAGE_HTML_GZIP[] PROGMEM = {\n'+'\n'.join(hex_lines)+'\n};\n'
header += 'static constexpr size_t PAGE_HTML_GZIP_LEN = sizeof(PAGE_HTML_GZIP);\n'
header += "static constexpr char PAGE_HTML_ETAG[] = "+__import__("json").dumps(etag)+";\n"
b = ROOT / 'firmware/RavestickBoard_v0_4_2_4_2_B/RavestickBoard_WebUI.h'
b.write_text(header, encoding='utf-8')
print(f'HTML raw {len(payload)} B; gzip {len(encoded)} B; ETag {etag}; A/B headers written')
