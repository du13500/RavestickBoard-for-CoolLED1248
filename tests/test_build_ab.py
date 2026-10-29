#!/usr/bin/env python3
"""Offline source consistency checks for v0.4.2.4.2 A/B; NOT an Arduino hardware test."""
from pathlib import Path
import gzip, hashlib, re, subprocess, tempfile
ROOT = Path(__file__).resolve().parents[1]
a = ROOT / 'firmware/RavestickBoard_v0_4_2_4_2_A'
b = ROOT / 'firmware/RavestickBoard_v0_4_2_4_2_B'
html = (ROOT/'web/index.html').read_text(encoding='utf-8').rstrip('\n')
payload = ('\n'+html+'\n').encode('utf-8')
ha = (a/'RavestickBoard_WebUI.h').read_text(encoding='utf-8')
hb = (b/'RavestickBoard_WebUI.h').read_text(encoding='utf-8')
assert 'R"RBHTML(\n'+html+'\n)RBHTML";' in ha
codes = re.findall(r'0x([0-9a-f]{2})', hb)
compressed = bytes(int(x,16) for x in codes)
assert gzip.decompress(compressed) == payload
assert len(compressed) < len(payload)//3
assert f'"\\"{hashlib.sha256(payload).hexdigest()[:24]}\\""' in hb
for name, folder in [('A',a),('B',b)]:
    ino = (folder/f'RavestickBoard_v0_4_2_4_2_{name}.ino').read_text(encoding='utf-8')
    assert f'0.4.2.4.2-dev-{name}' in ino
    assert 'HTTP_TRACE_CAP = 32' in ino
    assert 'esp_timer_start_periodic(httpTimer, 20000)' in ino
    assert 'web.on(path, method, [handler]()' in ino
    assert 'httpRoute(path, method, [handler]()' not in ino
    assert 'httpRoute("/api/http.txt", HTTP_GET, onHttpTraceDownload);' in ino
    assert 'web.onNotFound([]() { httpBegin(); respond(false, "Nicht gefunden.", 404); httpEnd(); });' in ino
    assert 'httpSetStatus((uint16_t)status)' in ino
    assert 'Seiten persistent in NVS' not in ino
    assert 'ZÄHLER SEIT BOOT' in ino
    assert 'httpRoute("/api/status", HTTP_GET, onStatus);' in ino
    assert 'httpRoute("/", HTTP_GET, onHomePage);' in ino
    assert ino.count('web.on(')==1
    assert ino.count('httpRoute(')>35
    if name=='A':
        assert 'web.send_P(200, "text/html; charset=utf-8", PAGE_HTML)' in ino
        assert 'no-store, no-cache, must-revalidate' in ino
        assert 'Content-Encoding' not in ino
    else:
        for part in ['web.collectHeaders(requestHeaders, 1)',
                     'web.header("If-None-Match") == PAGE_HTML_ETAG',
                     'web.send(304, "text/html; charset=utf-8", "")',
                     'web.sendHeader("Content-Encoding", "gzip")',
                     'PAGE_HTML_GZIP, PAGE_HTML_GZIP_LEN']:
            assert part in ino, part
    assert (folder/'RavestickBoard_Assets.h').read_bytes()==(a/'RavestickBoard_Assets.h').read_bytes()
    assert (folder/'RavestickBoard_Library.h').read_bytes()==(a/'RavestickBoard_Library.h').read_bytes()
    assert (folder/'RavestickBoard_Icon.h').read_bytes()==(a/'RavestickBoard_Icon.h').read_bytes()
# Test regenerating WebUI files is deterministic.
originals=[ha,hb]
subprocess.run(['python', str(ROOT/'tools/sync_web.py')],cwd=ROOT,check=True)
assert [(a/'RavestickBoard_WebUI.h').read_text(encoding='utf-8'),
        (b/'RavestickBoard_WebUI.h').read_text(encoding='utf-8')]==originals
license = (ROOT/'LICENSE').read_text(encoding='utf-8')
assert 'MIT License' in license and 'Lucide' in license
assert 'ISC' in (ROOT/'THIRD_PARTY_NOTICES.md').read_text(encoding='utf-8')
print(f'PASS: A/B source, 404 and all HTTP routes, binary gzip={len(compressed)} B, ETag, MIT+Lucide. No firmware compilation performed.')
