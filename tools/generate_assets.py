#!/usr/bin/env python3
"""Rebuild v0.3.0 flash assets from the two reconstructed original-app JSONs.

Use files produced in the separate CoolLED1248 glyph/symbol archive:
  python tools/generate_assets.py --glyphs assets/source/CoolLED1248_Originalglyphen_v02.json \
       --symbols assets/source/CoolLED1248_Originalsymbole_v01.json
  python tools/sync_web.py

This script does not download assets or copy other people's source code.
Manufacturer symbols remain credited in docs/QUELLEN_UND_LIZENZEN.md.
"""
from __future__ import annotations
import argparse, base64, json, re
from pathlib import Path

ROOT=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--glyphs', type=Path, required=True)
p.add_argument('--symbols', type=Path, required=True)
a=p.parse_args()
glyphs=json.loads(a.glyphs.read_text(encoding='utf-8'))['glyphen']
icons=json.loads(a.symbols.read_text(encoding='utf-8'))['symbole']
fonts={}
for face in ('Normal','Bold'):
 matches=[g for g in glyphs if g['schrift']==face]
 if len(matches)!=82:raise SystemExit(f'{face}: expected 82 records, got {len(matches)}')
 bb=bytearray()
 for g in matches:
  cp=ord(g['zeichen']); width=int(g['breite_px'])
  rows=g['zeilen_bin']
  if not(0<=cp<65536 and 1<=width<=16 and len(rows)==16):raise SystemExit(f'invalid glyph {g}')
  bb += cp.to_bytes(2,'little')+bytes([width])
  for row in rows:
   if len(row)!=width or not set(row)<={'0','1'}:raise SystemExit('Bad row: '+str(g))
   bb+=int(row,2).to_bytes(2,'little')
 fonts[face.lower()]=bb
if len(icons)!=24:raise SystemExit('24 original symbols expected')
pal={'K':0,'R':4,'Y':6,'B':1,'W':7,'G':2,'C':3,'M':5}
all_symbols=bytearray()
for icon in icons:
 rows=icon['pixel_codes_16x16']
 if len(rows)!=16 or any(len(row)!=16 for row in rows):raise SystemExit('not 16x16')
 for row in rows:
  for x in range(0,16,2):all_symbols.append((pal[row[x]]<<4)|pal[row[x+1]])
# Independently drawn RavestickBoard banana; mirrored vertically for festival totem.
banana=[
'KKKKKKKKKKKKKKKK','KKKKKKKKKKKKKKKK','KGGKKKKKKKKKGGKK','KYGKKKKKKKKKYGKK',
'KYYKKKKKKKKYYGKK','KYYKKKKKKKKYYKKK','KKYYYKKKKKYYYKKK','KKYYYKKKKYYYKKKK',
'KKKYYYYYYYYYKKKK','KKKYYYYYYYYKKKKK','KKKKYYYYYYKKKKKK','KKKKKWWYYKKKKKKK',
'KKKKKWWWYKKKKKKK','KKKKKKWYYKKKKKKK','KKKKKKKKKKKKKKKK','KKKKKKKKKKKKKKKK']
assert len(banana)==16 and all(len(row)==16 for row in banana)
for row in banana[::-1]:
 for x in range(0,16,2):all_symbols.append((pal[row[x]]<<4)|pal[row[x+1]])
assert len(all_symbols)==25*128
out='const RB_ASSET_DATA = '+json.dumps({**{k:base64.b64encode(v).decode() for k,v in fonts.items()},'symbols':base64.b64encode(all_symbols).decode()},ensure_ascii=False)+';'
web=ROOT/'web'/'index.html'
s=web.read_text(encoding='utf-8')
updated,n=re.subn(r'const RB_ASSET_DATA = \{[^\n]+?\};',lambda _:out,s,count=1)
if n!=1:raise SystemExit('RB_ASSET_DATA not present in web/index.html')
web.write_text(updated,encoding='utf-8')
lines=['#pragma once','// Auto-generated from CoolLED1248_Originalglyphen_v02.json, CoolLED1248_Originalsymbole_v01.json',
'// 164 reconstructed manufacturer glyphs; 24 reconstructed manufacturer symbols.',
'// The last (25th) icon is an independent RavestickBoard design.',
'// Original symbols remain credited to CoolLED1248; see docs/QUELLEN_UND_LIZENZEN.md.',
'#include <stdint.h>','#include <pgmspace.h>']
for name,data in [('RB_FONT_NORMAL',fonts['normal']),('RB_FONT_BOLD',fonts['bold']),('RB_SYMBOLS_PACKED',all_symbols)]:
 lines+=['static const uint8_t '+name+'[] PROGMEM = {']
 for idx in range(0,len(data),24):lines+=['  '+', '.join('0x%02X'%byte for byte in data[idx:idx+24])+',']
 lines+=['};','']
lines+=['static constexpr uint8_t RB_ORIG_GLYPHS_PER_FACE = 82;','static constexpr uint8_t RB_SYMBOL_COUNT = 25;',
'static constexpr uint8_t RB_ORIGINAL_SYMBOL_COUNT = 24;']
header=ROOT/'firmware'/'RavestickBoard_v0_3_0'/'RavestickBoard_Assets.h'
header.write_text('\n'.join(lines)+'\n',encoding='utf-8')
print('Rebuilt',header,', web/index.html. Run python tools/sync_web.py next.')
