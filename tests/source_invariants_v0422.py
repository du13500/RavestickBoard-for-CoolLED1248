#!/usr/bin/env python3
"""Static source checks for regressions that are not covered by browser mocks.
These do not replace Arduino compilation or actual ESP32 execution.
"""
from pathlib import Path
root=Path(__file__).resolve().parents[1];f=root/'firmware/RavestickBoard_v0_4_2_2'
ino=(f/'RavestickBoard_v0_4_2_2.ino').read_text()
program=(f/'RavestickBoard_v042.h').read_text()
lib=(f/'RavestickBoard_Library.h').read_text()
ui=(f/'RavestickBoard_WebUI.h').read_text()
html=(root/'web/index.html').read_text()
assert 'void loop() {\n  web.handleClient();\n  v042Tick();' in ino
assert 'v042AfterJob(job, ok);' in ino
assert 'rb42NextMs=millis()+duration;' in program
assert 'rb42ProgramSpeed>=60 && rb42ProgramSpeed<=90' in program
assert 'rb42BandCount*795UL*(100-rb42ProgramSpeed)/10UL' in program
assert 'if(rb42MissingCount())' in program
assert '"missing"' in program or r'\"missing\"' in program
assert 'if (v042ProtectLibrary(id))' not in lib
assert 'v042DetachLibraryShortcuts(id,next)' in lib
assert 'nextShortcuts.labels[i][0]=0;' in program
assert 'rb42ReadCollection' in program
assert 'programPlaying' in ino
assert 'v0.4.2.2-dev' in ino and 'v0.4.2.2-dev' in html
assert html in ui
print('PASS: source invariants, program tick, block timing, missing refs, shortcut cleanup, synced WebUI')
