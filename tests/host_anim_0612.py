"""v0.6.0.0: Animationsnutzlast (Opcode 0x04) mit dem echten Sketch-Code (Block 'gespeicherte Animation', framePacket,
makeChunk) gegen eine unabhaengige Python-Referenz, dazu die Antwortcode-Auswertung gegen echte Antworten der Tafel.

Die Referenz ist eine eigene Umsetzung der in Adams OpenSign-Protokollbuch beschriebenen Kodierung (Bitebenen,
24 Byte reserviert + Frames + ms, 128-Byte-Chunks mit XOR, Escape). Ist OPENSIGN_SRC gesetzt (Pfad zu
<opensign>/src), wird zusaetzlich sein Codec importiert und mit derselben Eingabe verglichen; sonst entfaellt das."""
from pathlib import Path
import os, subprocess, sys, tempfile

ROOT = Path(__file__).resolve().parents[1]
ino = next((ROOT / 'firmware').glob('RavestickBoard_v*/*.ino')).read_text(encoding='utf8')

def func(name, ret='size_t'):
    a = ino.index(f'{ret} {name}(')
    i = ino.index('{', a); depth = 0
    for j in range(i, len(ino)):
        depth += ino[j] == '{'; depth -= ino[j] == '}'
        if depth == 0: return ino[a:j + 1] + '\n'

# ab 0.6.0.2: Konstanten/Variablen oben, Funktionen weiter unten (hinter RamStage); beide Teile werden zusammengesetzt
a = ino.index('// ---- v0.6.0.0: gespeicherte Animation'); a2 = ino.index('// (Die Funktionen rb60NumberPixel')
a2 = ino.index('\n', a2) + 1
c = ino.index('// ---- v0.6.0.2: Funktionen der Animation'); b = ino.index('// ---- Ende v0.6.0.0 Animation')
src = ((ROOT / 'tests/host/anim_stub_0612.h').read_text() + (ino[a:a2] + ino[c:b]) + 'static bool dummy60=false;\n'
       + '// Nachbildung von copyPayloadSlice(): nur der Animationszweig; der echte Zweig wird im Quelltext geprueft.\nvoid copyPayloadSlice(size_t offset, size_t length, uint8_t *dest) {\n  if (rb60AnimActive) { rb60CopyAnimSlice(offset, length, dest); return; }\n  assert(!"nur Animationszweig");\n}\n' + func('framePacket') + func('makeChunk') + (ROOT / 'tests/host/anim_main_0612.cpp').read_text())
tmp = tempfile.mkdtemp()
(Path(tmp) / 't.cpp').write_text(src)
subprocess.run(['g++', '-std=c++17', '-Wall', '-Wextra', '-Wno-unused', '-Wno-volatile', '-Wno-unused-parameter', '-o', f'{tmp}/t', f'{tmp}/t.cpp'], check=True)
def run(*a): return subprocess.run([f'{tmp}/t', *map(str, a)], capture_output=True, text=True, check=True).stdout.split('\n')

# ---------- unabhaengige Referenz -------------------------------------------------------------
# Ziffern 3x5 als Text (unabhaengig von der Bit-Tabelle im Sketch); Nummer = f+1, zweistellig, doppelt gross
GLYPH = {
 '0': ['###', '# #', '# #', '# #', '###'], '1': [' # ', '## ', ' # ', ' # ', '###'],
 '2': ['###', '  #', '###', '#  ', '###'], '3': ['###', '  #', '###', '  #', '###'],
 '4': ['# #', '# #', '###', '  #', '  #'], '5': ['###', '#  ', '###', '  #', '###'],
 '6': ['###', '#  ', '###', '# #', '###'], '7': ['###', '  #', '  #', ' # ', ' # '],
 '8': ['###', '# #', '###', '# #', '###'], '9': ['###', '# #', '###', '  #', '###']}
def number_rows(num):
    """10 Zeilen zu je 14 Zeichen ('#' = Pixel)."""
    t = '%02d' % num
    rows = []
    for gy in range(5):
        line = ''.join(''.join(2 * c for c in GLYPH[d][gy]) + '  ' for d in t)[:-2]
        rows += [line, line]
    return rows
def number_pixel(num, x, y):
    if not (25 <= x < 39 and 3 <= y < 13): return False
    return number_rows(num)[y - 3][x - 25] == '#'
def ref_color(pattern, n, f, x, y):
    if number_pixel(f + 1, x, y): return {1: 0, 2: 2, 3: 7}[pattern]
    if pattern == 1: return f + 1 if f < 7 else 0
    x0 = f * 64 // n
    if x0 <= x < x0 + 4: return 7 if pattern == 2 else (f // 8) % 6 + 1
    if (x, y) == (0, 0): return 4
    if (x, y) == (63, 15): return 1
    return 0
def planes(pattern, n, f):
    out = [bytearray(), bytearray(), bytearray()]
    for x in range(64):
        for half in range(2):
            acc = [0, 0, 0]
            for k in range(8):
                c = ref_color(pattern, n, f, x, half * 8 + k)
                for p, bit in enumerate((4, 2, 1)): acc[p] = (acc[p] << 1) | (1 if c & bit else 0)
            for p in range(3): out[p].append(acc[p])
    return out
def esc(d):
    o = bytearray()
    for v in d:
        if 1 <= v <= 3: o += bytes([2, v + 4])
        else: o.append(v)
    return bytes(o)
def ref_chunks(pattern, n, ms, opcode=4):
    fr = [planes(pattern, n, f) for f in range(n)]
    pix = b''.join(bytes(fr[f][p]) for p in range(3) for f in range(n))
    raw = bytes(24) + bytes([n]) + ms.to_bytes(2, 'big') + pix
    chunks = []
    for cid, st in enumerate(range(0, len(raw), 128)):
        part = raw[st:st + 128]
        fmt = bytearray(b'\x00' + len(raw).to_bytes(2, 'big') + cid.to_bytes(2, 'big') + bytes([len(part)]) + part)
        x = 0
        for v in fmt: x ^= v
        fmt.append(x)
        payload = bytes([opcode]) + bytes(fmt)
        chunks.append(b'\x01' + esc(len(payload).to_bytes(2, 'big') + payload) + b'\x03')
    return raw, chunks

cases = [(1, 7, 700), (2, 16, 60), (2, 2, 5), (2, 79, 1), (3, 79, 300), (3, 80, 300), (3, 20, 2000), (1, 7, 255), (2, 33, 256)]
for pat, n, ms in cases:
    out = run('anim', pat, n, ms)
    ln = int(out[0].split()[1]); got = [bytes.fromhex(x) for x in out[1:] if x]
    raw, exp = ref_chunks(pat, n, ms)
    assert ln == len(raw) == 27 + n * 384, (pat, n, ln, len(raw))
    assert got == exp, f'Nutzlast weicht ab: Muster {pat}, {n} Frames, {ms} ms'
# Datenlimit: 79 Frames passen in 30 KiB, 80 nicht (Messung von Adam, 15.08.2026)
assert 27 + 79 * 384 == 30363 <= 30 * 1024 < 27 + 80 * 384 == 30747
assert run('rejects', 1)[0].split() == ['0', '0', '0', '0', '0', '30747'], run('rejects', 1)

# ---------- Antwortcodes ------------------------------------------------------------------------
real = ['0100050206000002070003', '0100050206000002060003', '0100050206000002050003', '010005020600000000' '03',
        '010005020600000600' '03', '0100050206000009 00 03'.replace(' ', '')]
exp_idx = [3, 2, 1, 0, 6, 9]
got = run('ack', *real)[:len(real)]
for g, i in zip(got, exp_idx): assert g == f'OK 2 {i} 0', (g, i)
def ack(cmd, idx, status):
    pay = bytes([cmd, 0]) + idx.to_bytes(2, 'big') + bytes([status])
    return (b'\x01' + esc(len(pay).to_bytes(2, 'big') + pay) + b'\x03').hex()
for st in (0, 1, 2, 3, 4, 5, 6, 7, 255):
    for idx in (0, 1, 2, 3, 4, 5, 255, 256, 513, 240):
        assert run('ack', ack(4, idx, st))[0] == f'OK 4 {idx} {st}', (st, idx)
for bad in ('', '01', '0100050206000003', '0200050206000002070003', '0100060206000002070003', '01000502060000020003',
            '0100050209000002070003', '010005020006000002070003', '0100050204000002070003'):
    assert run('ack', bad)[0] == 'BAD', bad

# ---------- optional: Adams Codec -----------------------------------------------------------------
extra = ''
op = os.environ.get('OPENSIGN_SRC')
if op:
    sys.path.insert(0, op)
    from opensign.protocol.codecs import coolledx as cx
    for pat, n, ms in cases:
        raw, exp = ref_chunks(pat, n, ms)
        assert [bytes(c) for c in cx._chop_into_chunks(raw, cx.OPCODE_ANIMATION)] == exp
    for r in real: assert cx.decode_coolledx_ack(bytes.fromhex(r))['status'] == 0
    for st in (0, 4, 6): assert cx.decode_coolledx_ack(bytes.fromhex(ack(4, 7, st)))['status'] == st
    extra = ' + Chunk-Kodierung und Antwortdekodierung gleich zu Adams Codec'
print(f'PASS Animationsnutzlast (echter Sketch-Code) = unabhaengige Referenz: {len(cases)} Faelle inkl. 79/80 Frames; '
      f'Antwortcodes gegen echte Tafelantworten und Fehlerfaelle{extra}')
