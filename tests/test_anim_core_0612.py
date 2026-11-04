"""v0.6.1.0: Kernfunktionen der Animationsoberflaeche (web/index.html, Block RbAnim) in Node:
 - Nutzlast-Bau gegen die unabhaengige Referenz, die in host_anim_0612.py die Firmware bestaetigt (und die auf der echten Tafel richtig aussah)
 - Base64, Auspacken, Vorschaubild
 - GIF-Decoder gegen Pillow (Interlace, Teilbilder, Transparenz, Entsorgungsarten, Rauschen mit 12-Bit-Codes, Randgroessen)
 - Text-Raster, Farbumsetzung, Bildfolge-Planung, Fenster, Streifen
Nicht geprueft: echte Browser (iPad-Safari) und die Tafel."""
from pathlib import Path
import base64, json, random, subprocess, sys, tempfile
from PIL import Image

ROOT = Path(__file__).resolve().parents[1]
html = (ROOT / 'web/index.html').read_text(encoding='utf8')
a = html.index('const RbAnim=(()=>{'); b = html.index('/* ==== Ende reine Funktionen ==== */')
core = html[a:b]
tmp = Path(tempfile.mkdtemp())
(tmp / 'core.js').write_text(core + '\nmodule.exports=RbAnim;\n')
def node(script, data=None):
    (tmp / 'run.js').write_text("const R=require('./core.js');const D=process.argv[2]?JSON.parse(require('fs').readFileSync(process.argv[2],'utf8')):null;(async()=>{" + script + "})().catch(e=>{console.error(e.stack||e);process.exit(1)});")
    args = ['node', str(tmp / 'run.js')]
    if data is not None:
        (tmp / 'in.json').write_text(json.dumps(data)); args.append(str(tmp / 'in.json'))
    r = subprocess.run(args, capture_output=True, text=True)
    if r.returncode: raise SystemExit('Node-Fehler:\n' + r.stderr)
    return json.loads(r.stdout) if r.stdout.strip() else None
fails = []
def check(c, m):
    if not c: fails.append(m); print('FAIL', m)

# ---- Referenz aus host_anim_0612.py (Firmware-bestaetigt): Testmuster -> Pixelfelder -> Nutzlast ----------
GLYPH = {'0': ['###', '# #', '# #', '# #', '###'], '1': [' # ', '## ', ' # ', ' # ', '###'], '2': ['###', '  #', '###', '#  ', '###'],
         '3': ['###', '  #', '###', '  #', '###'], '4': ['# #', '# #', '###', '  #', '  #'], '5': ['###', '#  ', '###', '  #', '###'],
         '6': ['###', '#  ', '###', '# #', '###'], '7': ['###', '  #', '  #', ' # ', ' # '], '8': ['###', '# #', '###', '# #', '###'],
         '9': ['###', '# #', '###', '  #', '###']}
def number_rows(num):
    t = '%02d' % num; rows = []
    for gy in range(5):
        line = ''.join(''.join(2 * c for c in GLYPH[d][gy]) + '  ' for d in t)[:-2]; rows += [line, line]
    return rows
def number_pixel(num, x, y):
    return 25 <= x < 39 and 3 <= y < 13 and number_rows(num)[y - 3][x - 25] == '#'
def ref_color(pattern, n, f, x, y):
    if number_pixel(f + 1, x, y): return {1: 0, 2: 2, 3: 7}[pattern]
    if pattern == 1: return f + 1 if f < 7 else 0
    x0 = f * 64 // n
    if x0 <= x < x0 + 4: return 7 if pattern == 2 else (f // 8) % 6 + 1
    if (x, y) == (0, 0): return 4
    if (x, y) == (63, 15): return 1
    return 0
def ref_payload(pattern, n, ms):
    planes = [[], [], []]
    for p, bit in enumerate((4, 2, 1)):
        for f in range(n):
            for x in range(64):
                for half in range(2):
                    acc = 0
                    for k in range(8): acc = (acc << 1) | (1 if ref_color(pattern, n, f, x, half * 8 + k) & bit else 0)
                    planes[p].append(acc)
    return bytes(24) + bytes([n]) + ms.to_bytes(2, 'big') + bytes(planes[0] + planes[1] + planes[2])
for pat, n, ms in [(1, 7, 700), (2, 16, 60), (3, 79, 300), (2, 2, 5), (2, 33, 256)]:
    frames = [[ref_color(pat, n, f, x, y) for y in range(16) for x in range(64)] for f in range(n)]
    out = node("process.stdout.write(JSON.stringify({p:Buffer.from(R.buildPayload(D.f.map(a=>Uint8Array.from(a)),D.ms)).toString('hex')}))", {'f': frames, 'ms': ms})
    check(bytes.fromhex(out['p']) == ref_payload(pat, n, ms), f'Nutzlast Muster {pat} n={n} ms={ms}')
    if pat == 3:   # Rundreise: auspacken ergibt dieselben Pixel; Vorschaubild = erstes Bild
        o2 = node("const u=R.unpackPayload(Uint8Array.from(Buffer.from(D.p,'hex')));const t=R.thumbOf(Uint8Array.from(Buffer.from(D.p,'hex')),D.n);process.stdout.write(JSON.stringify({ms:u.ms,f:u.frames.map(a=>Array.from(a)),tp:Array.from(R.thumbPixels(t)),t:Buffer.from(t).toString('hex')}))", {'p': out['p'], 'n': n})
        check(o2['ms'] == ms and o2['f'] == frames, 'auspacken = Eingabe')
        check(o2['tp'] == frames[0] and len(o2['t']) == 768, 'Vorschaubild = erstes Bild')

# ---- Base64 -----------------------------------------------------------------------------------
rb = random.Random(3)
for n in (0, 1, 2, 3, 4, 5, 383, 384, 1199, 1200, 30363):
    raw = bytes(rb.randrange(256) for _ in range(n))
    o = node("const u=Uint8Array.from(Buffer.from(D.h,'hex'));const e=R.b64uEnc(u);process.stdout.write(JSON.stringify({e,d:Buffer.from(R.b64uDec(e)).toString('hex')}))", {'h': raw.hex()})
    check(o['e'] == base64.urlsafe_b64encode(raw).decode().rstrip('=') and o['d'] == raw.hex(), f'Base64 n={n}')

# ---- GIF-Decoder gegen Pillow -------------------------------------------------------------------
def noise(w, h, seed, colors=256):
    r = random.Random(seed); im = Image.new('P', (w, h))
    pal = [v for _ in range(colors) for v in (r.randrange(256), r.randrange(256), r.randrange(256))]
    im.putpalette(pal + [0] * (768 - len(pal))); im.putdata([r.randrange(colors) for _ in range(w * h)]); return im
def sprite(w, h, k, bg=(0, 0, 0)):
    im = Image.new('RGB', (w, h), bg)
    for y in range(h):
        for x in range(w):
            if (x - 2 * k) ** 2 + (y - h // 2) ** 2 < (h // 3) ** 2: im.putpixel((x, y), (255, 40 * (k % 5), 60))
    return im
gifs = {}
gifs['noise256'] = ([noise(100, 80, s) for s in range(4)], dict(duration=[30, 100, 200, 0]))
gifs['noise_bigcodes'] = ([noise(200, 180, 9, 200)], dict())
gifs['sprite_opt'] = ([sprite(40, 24, k) for k in range(8)], dict(duration=60, loop=0))
gifs['sprite_disp2'] = ([sprite(40, 24, k) for k in range(6)], dict(duration=60, disposal=2, loop=0))
gifs['sprite_disp3'] = ([sprite(40, 24, k) for k in range(6)], dict(duration=60, disposal=3, loop=0))
gifs['tiny'] = ([sprite(7, 5, k) for k in range(3)], dict(duration=50))
gifs['one_by_one'] = ([Image.new('RGB', (1, 1), (10, 200, 30)), Image.new('RGB', (1, 1), (200, 20, 30))], dict(duration=40))
gifs['wide64x16'] = ([sprite(64, 16, k) for k in range(5)], dict(duration=80))
gifs['interlaced_odd'] = ([noise(37, 51, 4, 16)], dict(interlace=True))
gifs['plain'] = ([noise(30, 30, 5, 16)], dict(interlace=False))
paths = {}
for name, (frames, kw) in gifs.items():
    path = tmp / f'{name}.gif'
    if len(frames) == 1: frames[0].save(path, format='GIF', **kw)
    else: frames[0].save(path, format='GIF', save_all=True, append_images=frames[1:], **kw)
    paths[name] = path
# Referenz 1: Chromium (ImageDecoder, Browser-Decoder); Referenz 2: Pillow. Weichen beide ab, gilt der Browser (Pillow-Hinweis wird gezaehlt).
subprocess.run([sys.executable, str(ROOT / 'tests/gif_ref_chromium_0612.py'), str(tmp / 'chromium.json')] + [str(x) for x in paths.values()], check=True)
chromium = json.loads((tmp / 'chromium.json').read_text())
pillow_diff = []
def diffpix(mine, ref):
    bad = 0
    for j in range(0, len(mine), 4):
        ma, ra = mine[j + 3], ref[j + 3]
        if (ma == 255) != (ra == 255) or (ma == 255 and mine[j:j + 3] != ref[j:j + 3]): bad += 1
    return bad
for name, path in paths.items():
    im = Image.open(path); ref = []
    for i in range(getattr(im, 'n_frames', 1)):
        im.seek(i); ref.append((im.convert('RGBA').tobytes(), im.info.get('duration', 0)))
    out = node("const fs=require('fs');const buf=new Uint8Array(fs.readFileSync(D.p));const fr=[];const res=await R.gifDecode(buf,(rgba,w,h,d)=>fr.push({w,h,d,px:Buffer.from(rgba).toString('hex')}));process.stdout.write(JSON.stringify({res,fr}))", {'p': str(path)})
    cr = chromium[str(path)]
    check(out['res']['count'] == len(cr) == len(ref), f'{name}: Bildanzahl {out["res"]["count"]} / Chromium {len(cr)} / Pillow {len(ref)}')
    for i, f in enumerate(out['fr']):
        mine = bytes.fromhex(f['px'])
        check(diffpix(mine, bytes.fromhex(cr[i])) == 0, f'{name}: Bild {i}: {diffpix(mine, bytes.fromhex(cr[i]))} Pixel weichen vom Chromium-Decoder ab')
        if i < len(ref) and diffpix(mine, ref[i][0]): pillow_diff.append(f'{name}#{i}')
        want = ref[i][1] if ref[i][1] > 10 else 100
        check(f['d'] == want, f'{name}: Bild {i}: Zeit {f["d"]} != {want}')
if pillow_diff: print('Hinweis: Pillow weicht von Chromium/eigenem Decoder ab bei', ', '.join(pillow_diff[:8]), '(Browser gilt)')
# kaputte Dateien
for label, data in [('keine GIF', b'PNG....'), ('abgeschnitten', (tmp / 'noise256.gif').read_bytes()[:300]), ('leer', b'')]:
    (tmp / 'bad.bin').write_bytes(data)
    try:
        o = node("const fs=require('fs');const buf=new Uint8Array(fs.readFileSync(D.p));let c=0;try{const r=await R.gifDecode(buf,()=>c++);process.stdout.write(JSON.stringify({ok:true,c}))}catch(e){process.stdout.write(JSON.stringify({ok:false,m:e.message}))}", {'p': str(tmp / 'bad.bin')})
        check(o['ok'] is False or o['c'] >= 0, f'kaputt {label}')
        if label != 'abgeschnitten': check(o['ok'] is False, f'{label}: Fehlermeldung erwartet {o}')
    except SystemExit as e: check(False, f'{label}: Absturz {e}')
# mehr als 300 Bilder: Abbruch mit Hinweis
many = [Image.new('RGB', (4, 4), (i % 256, 0, 0)) for i in range(320)]
many[0].save(tmp / 'many.gif', save_all=True, append_images=many[1:], duration=20)
o = node("const fs=require('fs');const buf=new Uint8Array(fs.readFileSync(D.p));let c=0;const r=await R.gifDecode(buf,()=>c++);process.stdout.write(JSON.stringify({r,c}))", {'p': str(tmp / 'many.gif')})
check(o['c'] == 300 and o['r']['truncated'] is True, f'300-Bilder-Grenze: {o}')

# ---- Text-Raster ----------------------------------------------------------------------------------
def raster(t):
    return node("try{const f=R.parseRaster(D.t);process.stdout.write(JSON.stringify({ok:true,f:f.map(x=>({w:x.w,h:x.h,px:Array.from(x.px)}))}))}catch(e){process.stdout.write(JSON.stringify({ok:false,m:e.message}))}", {'t': t})
r = raster('.RGB\nCMYW')
check(r['ok'] and r['f'][0]['w'] == 4 and r['f'][0]['h'] == 2 and r['f'][0]['px'] == [0, 4, 2, 1, 3, 5, 6, 7], f'Raster einfach {r}')
r = raster('R\tG\t\tB\nW\t.\tc\tm\n')   # Excel: Tab, leere Zelle = aus, Kleinbuchstaben
check(r['ok'] and r['f'][0]['px'] == [4, 2, 0, 1, 7, 0, 3, 5], f'Raster Tab {r}')
r = raster('RR\nRR\n\nGG\nGG\n---\nBB\nBB')
check(r['ok'] and len(r['f']) == 3 and r['f'][1]['px'] == [2, 2, 2, 2], f'Raster 3 Bilder {r}')
r = raster('R\tR\nR\tR\n---\t\t\nB\tB\n')   # '---' in der ersten Zelle trennt (Excel-Zeile)
check(r['ok'] and len(r['f']) == 2, f'Raster Excel-Trenner {r}')
r = raster('R\t\t\n\t\t\nB\t\t')   # Zeile aus leeren Zellen ist eine AUS-Zeile, kein Trenner
check(r['ok'] and len(r['f']) == 1 and r['f'][0]['h'] == 3 and r['f'][0]['w'] == 3, f'leere Excel-Zeile = aus {r}')
r = raster('RG\r\nBC\r\n'); check(r['ok'] and r['f'][0]['px'] == [4, 2, 1, 3], 'Raster CRLF')
r = raster('R.\nRX'); check(not r['ok'] and 'Zeile 2' in r['m'] and 'Spalte 2' in r['m'], f'Fehlerort {r}')
r = raster('R\tGG'); check(not r['ok'] and 'mehr als ein Zeichen' in r['m'], f'Zelle zu lang {r}')
r = raster('   \n\n'); check(not r['ok'], 'nur Leerraum -> Fehler')
r = raster('R..\nR'); check(r['ok'] and r['f'][0]['w'] == 3 and r['f'][0]['px'] == [4, 0, 0, 4, 0, 0], f'kurze Zeile aufgefuellt {r}')
r = raster('.' * 513); check(not r['ok'], 'zu breit')

# ---- Farbumsetzung -------------------------------------------------------------------------------
rgbs = [(0, 0, 0), (0, 0, 255), (0, 255, 0), (0, 255, 255), (255, 0, 0), (255, 0, 255), (255, 255, 0), (255, 255, 255)]
rnd = random.Random(1); idx = [rnd.randrange(8) for _ in range(1024)]
rgba = []
for i in idx: rgba += list(rgbs[i]) + [255]
for m in ('thresh', 'fs', 'bayer'):
    o = node("process.stdout.write(JSON.stringify(Array.from(R.quantize(Uint8ClampedArray.from(D.a),{method:D.m,thr:128,contrast:0}))))", {'a': rgba, 'm': m})
    check(o == idx, f'Palettenfarben bleiben exakt ({m})')
grey = [v for g in [rnd.randrange(256) for _ in range(1024)] for v in (g, g, g, 255)]
o = node("process.stdout.write(JSON.stringify(Array.from(R.quantize(Uint8ClampedArray.from(D.a),{method:'thresh',thr:100,contrast:0}))))", {'a': grey})
check(o == [7 if grey[i * 4] >= 100 else 0 for i in range(1024)], 'Schwelle 100 auf Grauwerten')
mid = [128, 128, 128, 255] * 1024
for m, lo, hi in (('fs', 0.35, 0.65), ('bayer', 0.35, 0.65)):
    o = node("process.stdout.write(JSON.stringify(Array.from(R.quantize(Uint8ClampedArray.from(D.a),{method:D.m,thr:128,contrast:0}))))", {'a': mid, 'm': m})
    share = sum(1 for v in o if v == 7) / 1024
    check(lo <= share <= hi, f'Rasterung {m}: mittleres Grau ~50 % weiss, ist {share:.2f}')
    check(all(v in (0, 7) for v in o), f'Rasterung {m}: Grau bleibt grau (nur aus/weiss)')
o = node("process.stdout.write(JSON.stringify(Array.from(R.quantize(Uint8ClampedArray.from(D.a),{method:'thresh',thr:128,contrast:100}))))", {'a': [100, 100, 100, 255] * 1024})
check(set(o) == {0}, 'Kontrast + dunkles Grau -> aus'); o = node("process.stdout.write(JSON.stringify(Array.from(R.quantize(Uint8ClampedArray.from(D.a),{method:'thresh',thr:128,contrast:0}))))", {'a': [200, 0, 0, 255] * 1024}); check(set(o) == {4}, 'rot bleibt rot')

# ---- Planung -------------------------------------------------------------------------------------
def plan(delays, **opt): return node("process.stdout.write(JSON.stringify(R.plan(D.d,D.o)))", {'d': delays, 'o': opt})
p = plan([100] * 10, ms=80); check(p['order'] == list(range(10)) and p['ms'] == 80 and not p['tooMany'], f'plan einfach {p}')
p = plan([100] * 10, **{'from': 3, 'to': 9, 'step': 2, 'ms': 50}); check(p['order'] == [2, 4, 6, 8], f'Bereich+Schritt {p}')
p = plan([100] * 5, ping=True, ms=50); check(p['order'] == [0, 1, 2, 3, 4, 3, 2, 1], f'Hin und zurueck {p}')
p = plan([100] * 2, ping=True, ms=50); check(p['order'] == [0, 1], 'Pingpong bei 2 Bildern unveraendert')
p = plan([100] * 80, ms=50); check(p['tooMany'] and p['count'] == 80, 'zu viele')
p = plan([100] * 79, ms=50); check(not p['tooMany'] and p['count'] == 79, '79 ok')
p = plan([100] * 100, step=2, ms=50); check(p['count'] == 50 and not p['tooMany'], 'Schritt 2')
p = plan([100, 100, 200, 100], variable=True, ms=50); check(p['order'] == [0, 1, 2, 2, 3] and p['ms'] == 100, f'Zeiten nachbilden {p}')
p = plan([20, 20, 40, 60, 1000], variable=True, ms=50); check(p['ms'] == 20 and p['count'] == 2 + 2 + 3 + 50, f'Zeiten nachbilden 2 {p}')
p = plan([10] + [1000] * 3, variable=True, ms=50); check(p['count'] <= 79 and p['ms'] >= 10, f'Zeiten nachbilden passt in 79 {p}')
p = plan([5000, 10, 10], ms=50, variable=True); check(p['count'] <= 79 and p['ms'] <= 2000, 'Obergrenze 2000 ms')
p = plan([100] * 4, ms=-5); check(p['ms'] == 1, 'ms >= 1'); p = plan([100] * 4, ms=0); check(p['ms'] == 100, 'ms leer -> 100'); p = plan([100] * 4, ms=99999); check(p['ms'] == 2000, 'ms <= 2000')

# ---- Fenster, Streifen -----------------------------------------------------------------------------
def fit(sw, sh, mode, ox=0, oy=0): return node("process.stdout.write(JSON.stringify(R.fitRect(D.a,D.b,D.m,D.x,D.y)))", {'a': sw, 'b': sh, 'm': mode, 'x': ox, 'y': oy})
r = fit(128, 128, 'contain'); check(abs(r['w'] - 16) < 1e-9 and abs(r['h'] - 16) < 1e-9 and abs(r['x'] - 24) < 1e-9 and r['y'] == 0, f'contain {r}')
r = fit(128, 128, 'cover'); check(abs(r['w'] - 64) < 1e-9 and abs(r['h'] - 64) < 1e-9 and abs(r['y'] + 24) < 1e-9, f'cover mitte {r}')
r = fit(128, 128, 'cover', 0, -100); check(abs(r['y']) < 1e-9, f'cover oben {r}'); r = fit(128, 128, 'cover', 0, 100); check(abs(r['y'] + 48) < 1e-9, f'cover unten {r}')
r = fit(10, 10, 'orig'); check(r['x'] == 27 and r['y'] == 3 and r['w'] == 10, f'orig {r}')
r = fit(100, 100, 'stretch'); check(r['x'] == 0 and r['y'] == 0 and r['w'] == 64 and r['h'] == 16, f'stretch {r}')
r = fit(64, 16, 'contain'); check(r['scale'] == 1 and r['x'] == 0 and r['y'] == 0, 'exakt 64x16 ohne Skalierung')
# --- v0.6.1.2: Ausschnitt-Rahmen (cropFor / cropRect / clampCrop) -------------------------------------
def cropfit(sw, sh, mode):
    return node("const c=R.cropFor(D.a,D.b,D.m);process.stdout.write(JSON.stringify({c,r:R.fitRect(D.a,D.b,'crop',0,0,c),o:R.fitRect(D.a,D.b,D.m,0,0)}))", {'a': sw, 'b': sh, 'm': mode})
for (sw, sh) in [(128, 128), (128, 64), (300, 40), (10, 10), (64, 16), (7, 90), (640, 480)]:
    for mode in ('contain', 'cover', 'orig'):
        o = cropfit(sw, sh, mode)
        same = all(abs(o['r'][k] - o['o'][k]) < 1e-6 for k in ('x', 'y', 'w', 'h', 'scale'))
        check(same, f'Rahmen {mode} {sw}x{sh} = alter Modus: {o["r"]} gegen {o["o"]}')
o = node("process.stdout.write(JSON.stringify(R.cropRect(128,64,{cx:.5,cy:.75,w:1})))")
check(abs(o['x']) < 1e-9 and abs(o['y'] + 32 * 0.5) < 1e-9 and abs(o['scale'] - 0.5) < 1e-9, f'Rahmen auf der unteren Bildhaelfte {o}')
o = node("process.stdout.write(JSON.stringify([R.clampCrop({cx:-3,cy:9,w:0.0001},128,64),R.clampCrop({cx:.5,cy:.5,w:99},128,64),R.clampCrop({cx:.2,cy:.3,w:.5},128,64)]))")
check(o[0]['cx'] == 0 and o[0]['cy'] == 1 and abs(o[0]['w'] - 8 / 128) < 1e-9, f'clamp klein {o[0]}')
check(abs(o[1]['w'] - 2 * max(128, 64 * 4) / 128) < 1e-9, f'clamp gross {o[1]}')
check(o[2] == {'cx': .2, 'cy': .3, 'w': .5}, 'clamp laesst gueltige Werte unveraendert')

o = node("process.stdout.write(JSON.stringify({h:R.stripRects(300,100,3,'h'),v:R.stripRects(100,300,3,'v'),g1:R.stripGuess(640,64),g2:R.stripGuess(16,160),g3:R.stripGuess(50,50)}))")
check(o['h'] == [{'x': 0, 'y': 0, 'w': 100, 'h': 100}, {'x': 100, 'y': 0, 'w': 100, 'h': 100}, {'x': 200, 'y': 0, 'w': 100, 'h': 100}], f'Streifen waagerecht {o["h"]}')
check(o['v'][2] == {'x': 0, 'y': 200, 'w': 100, 'h': 100}, 'Streifen senkrecht')
check(o['g1'] == {'dir': 'h', 'n': 10} and o['g2'] == {'dir': 'v', 'n': 10} and o['g3']['n'] == 1, f'Streifen raten {o}')
o = node("process.stdout.write(JSON.stringify({a:R.uploadSeconds(79),b:R.uploadSeconds(16)}))")
check(53 < o['a'] < 57 and 10 < o['b'] < 14, f'Uebertragungsdauer-Schaetzung {o}')

print('FAILS:', len(fails))
sys.exit(1 if fails else 0)
