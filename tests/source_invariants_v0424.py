#!/usr/bin/env python3
"""Static v0.4.2.4 invariants only; not Arduino or hardware verification."""
from pathlib import Path
import re
root=Path(__file__).resolve().parents[1]
f=root/'firmware/RavestickBoard_v0_4_2_4'
ino=(f/'RavestickBoard_v0_4_2_4.ino').read_text()
h=(f/'RavestickBoard_v042.h').read_text()
ui=(f/'RavestickBoard_WebUI.h').read_text()
web=(root/'web/index.html').read_text().rstrip('\n')
license=(root/'LICENSE').read_text()
notices=(root/'THIRD_PARTY_NOTICES.md').read_text()
assert 'FW_VERSION[] = "0.4.2.4-dev"' in ino
assert 'v0.4.2.4-dev' in web
assert 'void loop() {\n  web.handleClient();\n  v042Tick();' in ino
assert 'v042AfterJob(job, ok);' in ino
assert 'rb43StreamBand' in ino and 'rb43CopyMixedSlice(offset,length,dest)' in ino
assert 'sizeof(streamTile)' in h and 'rb43RenderMixedTile' in h
assert 'memcpy(streamTile' in h
assert 'RB43_GAP=16;' in h
assert 'uint32_t duration=max(4000UL' in h
assert 'rb42ProgramSeconds*1000' not in h
assert 'rb42NextMs=millis()+duration;' in h
assert 'rb42BandCount*795UL*(100-rb42ProgramSpeed)/10UL' in h
assert 'Vollstaendiger Wiederholungsversuch (1/1)' in ino
assert 'currentMessage.indexOf("nicht bestaetigt")>=0' in ino
assert '<aside id="rbDock"' in web
assert web.count('id="rbDock"')==1
assert web.count('id="dockBrightness"')==1
assert web.count('id="dockSpeed"')==1
assert web.count('id="homePowerOn"')==1
assert web.count('class="rb-dock-modes v43-force-modes"')==1
assert 'RB43_FORCE_MODES' in web
assert 'data-rb-icon="bug"' not in web # menu icon resolved dynamically
assert "'spray-can','bug','wrench'" in web
assert "previewRate===1?'snail':previewRate===2?'bird':'rabbit'" in web
assert 'Ersatzlaufzeit für Gruppenwechsel (5–180 s)' not in web
assert '<div class="menu-title">' not in web
assert 'WLAN: CoolLED1248 · http://192.168.4.1' not in web
assert web in ui
assert license.startswith('MIT License\n\nCopyright (c) 2026 du13500')
assert 'THIRD_PARTY_NOTICES.md' in license
assert 'Copyright (c) 2026 Lucide Icons and Contributors' in notices
assert 'Copyright (c) 2013-present Cole Bemis' in notices
assert (root/'docs/TESTPLAN_v0.4.2.4.md').exists()
assert (root/'CONTRIBUTING.md').exists() and (root/'SECURITY.md').exists()
assert (root/'.github/ISSUE_TEMPLATE/bug_report.yml').exists()
print('PASS: v0.4.2.4 source, live dock, licenses, firmware guards and documentation')
