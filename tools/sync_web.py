#!/usr/bin/env python3
"""Reproducible WebUI gzip/ETag; run from repo root or anywhere."""
from pathlib import Path
import gzip, hashlib, json
ROOT=Path(__file__).resolve().parents[1]
html=(ROOT/'web/index.html').read_text(encoding='utf8').rstrip('\n')
raw=('\n'+html+'\n').encode('utf8')
gz=gzip.compress(raw,compresslevel=9,mtime=0)
lines=['  '+', '.join(f'0x{b:02x}' for b in gz[i:i+16])+',' for i in range(0,len(gz),16)]
header='#pragma once\n// v0.4.3.2: gzip-9 HTML in PROGMEM, generated reproducibly from web/index.html.\n// Do not edit this header directly; regenerate from the checked-in HTML source.\nstatic const uint8_t PAGE_HTML_GZIP[] PROGMEM = {\n'+'\n'.join(lines)+'\n};\nstatic constexpr size_t PAGE_HTML_GZIP_LEN = sizeof(PAGE_HTML_GZIP);\n'
header+='static constexpr char PAGE_HTML_ETAG[] = '+json.dumps('"'+hashlib.sha256(raw).hexdigest()[:24]+'"')+';\n'
p=ROOT/'firmware/RavestickBoard_v0_4_3_2/RavestickBoard_WebUI.h'
p.write_text(header,encoding='utf8')
print(f'HTML={len(raw)} B gzip={len(gz)} B ETag={hashlib.sha256(raw).hexdigest()[:24]}')
