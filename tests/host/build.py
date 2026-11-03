import sys,re
from pathlib import Path
# usage: build.py <repo root> <out.cpp>   (Rechnertest der Text-Renderfunktionen aus dem Sketch)
stage=Path(sys.argv[1]); out=Path(sys.argv[2])
fw=sorted((stage/'firmware').glob('RavestickBoard_v*'))[0]
ino=next(fw.glob('*.ino')).read_text(encoding='utf8').split('\n')
def find(prefix,start=0):
    for i in range(start,len(ino)):
        if ino[i].startswith(prefix): return i
    raise SystemExit('missing '+prefix)
g0=find('struct Glyph'); g1=find('static const uint8_t UNKNOWN_GLYPH')
c0=find('// Decode short NVS block-page fields only'); c1=find('size_t renderBlockPage')
a0=find('// ---- v0.6.0.0: gespeicherte Animation'); a1=find('// ---- Ende v0.6.0.0 Animation')
anim='\n'.join(ino[a0:a1])   # ab 0.6.0.0 liest copyPayloadSlice() den Animationszweig
body=anim+'\n'+'\n'.join(ino[g0:g1+1])+'\n'+'\n'.join(ino[c0:c1])
assets=(fw/'RavestickBoard_Assets.h').read_text(encoding='utf8')
stub=Path(__file__).with_name('stubs.h').read_text()
out.write_text(stub+'\n'+assets+'\n'+body+'\n#include "drv.h"\n',encoding='utf8')
