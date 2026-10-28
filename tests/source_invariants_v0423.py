#!/usr/bin/env python3
"""Static checks for v0.4.2.3; not an Arduino build or hardware verification."""
from pathlib import Path
root=Path(__file__).resolve().parents[1];f=root/'firmware/RavestickBoard_v0_4_2_3'
ino=(f/'RavestickBoard_v0_4_2_3.ino').read_text()
program=(f/'RavestickBoard_v042.h').read_text()
lib=(f/'RavestickBoard_Library.h').read_text()
ui=(f/'RavestickBoard_WebUI.h').read_text()
html=(root/'web/index.html').read_text()
assert 'void loop() {\n  web.handleClient();\n  v042Tick();' in ino
assert 'v042AfterJob(job, ok);' in ino
assert 'rb42NextMs=millis()+duration;' in program
assert 'rb42BandCount*795UL*(100-rb42ProgramSpeed)/10UL' in program
assert 'if(rb42MissingCount())' in program
assert 'if (v042ProtectLibrary(id))' not in lib
assert 'v042DetachLibraryShortcuts(id,next)' in lib
assert 'nextShortcuts.labels[i][0]=0;' in program
assert 'rb42ReadCollection' in program
assert 'rb43Playback=rb42Collection.reserved==1' in program
assert 'rb42Collection.reserved=playback' in program
assert 'rb43QueueScrollGroup' in program
assert 'rb43RenderGroup' in program
assert 'RB43_GAP=16;' in program
assert 'job == TEXT && rb43CurrentGroup' in ino
assert 'void drawInlineUtf8' in ino and 'uint16_t offset=0, uint16_t available=0' in ino
assert 'v0.4.2.3-dev' in ino and 'v0.4.2.3-dev' in html
assert html in ui
assert html.count('class="v43-force-modes"')==3
assert 'RB43_FORCE_MODES' in html and "for(const [mode,icon,label] of RB43_FORCE_MODES)" in html
assert 'rb43StaticIcons' in html and 'RB43_LUCIDE' in html
assert 'THIRD_PARTY_NOTICES.md' in (root/'LICENSE').read_text()
assert 'Copyright (c) 2026 Lucide Icons' in (root/'THIRD_PARTY_NOTICES.md').read_text()
assert 'Copyright (c) 2013-present Cole Bemis' in (root/'THIRD_PARTY_NOTICES.md').read_text()
print('PASS: v0.4.2.3 source invariants and legal notices')
