"""v0.6.1.0: Dateispeicher der Animationen (RavestickBoard_Anim.h) mit dem ECHTEN Sketch-Code gegen ein Dateisystem im
Speicher: Upload in Stuecken, Liste, Abruf, Senden aus Datei, Umbenennen, Duplizieren, Loeschen, Fehler- und Grenzfaelle.
Die Gegenseite (Nutzlast, Base64, Kopf) ist hier unabhaengig in Python umgesetzt. Hardware (echtes LittleFS, echter
Webserver, Tafel) wird NICHT geprueft."""
from pathlib import Path
import base64, json, random, subprocess, sys, tempfile, zlib

ROOT = Path(__file__).resolve().parents[1]
fw = next((ROOT / 'firmware').glob('RavestickBoard_v*'))
ino = next(fw.glob('*.ino')).read_text(encoding='utf8')
anim_h = (fw / 'RavestickBoard_Anim.h').read_text(encoding='utf8')

a = ino.index('// ---- v0.6.0.0: gespeicherte Animation'); a2 = ino.index('// (Die Funktionen rb60NumberPixel'); a2 = ino.index('\n', a2) + 1
c = ino.index('// ---- v0.6.0.2: Funktionen der Animation'); b = ino.index('// ---- Ende v0.6.0.0 Animation')
# Seit 0.6.1.3 echte Teile aus Library.h/v042.h: Pruefsumme (FNV), Kurzwahl-Struktur und rb42Write (fuer das Leeren der Kurzwahlen beim Loeschen)
lib_h = (fw / 'RavestickBoard_Library.h').read_text(encoding='utf8')
v42_h = (fw / 'RavestickBoard_v042.h').read_text(encoding='utf8')
lib_part = lib_h[lib_h.index('uint32_t libraryChecksum('):lib_h.index('String libraryPath(')]
sc_a = v42_h.index('static constexpr uint32_t RB42_SHORT_MAGIC'); sc_b = v42_h.index('\n', sc_a) + 1
st_a = v42_h.index('struct RB42Shortcuts {'); st_b = v42_h.index('};', st_a) + 3
cn_a = v42_h.index('static RB42Shortcuts rb42Shortcuts'); cn_b = v42_h.index('static PageRecord rb42Band')
wr_a = v42_h.index('template <typename T> bool rb42Write'); wr_b = v42_h.index('bool rb42ReadBlock')
v42_part = v42_h[sc_a:sc_b] + v42_h[st_a:st_b] + v42_h[cn_a:cn_b] + v42_h[wr_a:wr_b]
src = ((ROOT / 'tests/host/animfs_stub_0613.h').read_text() + ino[a:a2] + ino[c:b] + lib_part + v42_part + anim_h + '\n'
       + (ROOT / 'tests/host/animfs_main_0613.cpp').read_text())
tmp = tempfile.mkdtemp()
(Path(tmp) / 't.cpp').write_text(src)
subprocess.run(['g++', '-std=c++17', '-Wall', '-Wextra', '-Wno-unused', '-Wno-volatile', '-Wno-unused-parameter', '-Wno-unused-function',
                '-o', f'{tmp}/t', f'{tmp}/t.cpp'], check=True)

class Sim:
    def __init__(self):
        self.p = subprocess.Popen([f'{tmp}/t'], stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True, bufsize=1)
    SILENT = ('STATE', 'MOUNT', 'TOTAL', 'FAILWRITES', 'TMPFILE', 'POKE', 'TRUNC', 'RENAMEOVERWRITE', 'FAILRENAME', 'FAILREAD', 'COPY', 'DEL', 'SETSC', 'DAMAGED')
    def cmd(self, *parts, lines=None):
        if lines is None: lines = 0 if parts[0] in self.SILENT else 1
        self.p.stdin.write('\t'.join(str(x) for x in parts) + '\n'); self.p.stdin.flush()
        return [self.p.stdout.readline().rstrip('\n') for _ in range(lines)]
    def until_end(self, *parts):
        self.p.stdin.write('\t'.join(parts) + '\n'); self.p.stdin.flush()
        out = []
        while True:
            l = self.p.stdout.readline().rstrip('\n')
            if l == 'END': return out
            out.append(l)
    def req(self, route, **kw):
        self.p.stdin.write('\t'.join(['REQ', route] + [f'{k}={v}' for k, v in kw.items()]) + '\n'); self.p.stdin.flush()
        st = self.p.stdout.readline().strip(); body = self.p.stdout.readline().rstrip('\n')
        assert st.startswith('STATUS '), st
        return int(st.split()[1]), (json.loads(body) if body.startswith('{') else body)

def b64u(raw): return base64.urlsafe_b64encode(raw).decode().rstrip('=')
def payload(n, ms, seed=1):
    r = random.Random(seed)
    return bytes(24) + bytes([n, ms >> 8, ms & 255]) + bytes(r.randrange(256) for _ in range(n * 384))
def thumb(pl, n):
    body = pl[27:]
    return body[0:128] + body[n * 128:n * 128 + 128] + body[2 * n * 128:2 * n * 128 + 128]
def upload(sim, name, n, ms, seed=1, pl=None, over=None):
    over = over or {}
    pl = pl if pl is not None else payload(n, ms, seed)
    last = None
    for off in range(0, len(pl), 1200):
        piece = pl[off:off + 1200]
        kw = dict(off=off, data=b64u(piece))
        if off == 0: kw.update(name=name, frames=n, ms=ms, total=len(pl), crc=zlib.crc32(pl), thumb=b64u(thumb(pl, n) if len(pl) >= 27 + n * 384 else bytes(384)))
        kw.update(over if off == 0 else {})
        last = sim.req('put', **kw)
        if last[0] != 200: return last
    return last

sim = Sim()
fails = []
def check(cond, msg):
    if not cond: fails.append(msg); print('FAIL', msg)

# Base64-Rundreise gegen Python
for n in (1, 2, 3, 4, 5, 383, 384, 385, 1200):
    raw = bytes(random.Random(n).randrange(256) for _ in range(n))
    out = sim.cmd('B64', raw.hex() or '')[0].split()
    check(out and out[0] == b64u(raw) and int(out[1]) == n, f'Base64 n={n}: {out}')

check(sim.cmd('BEGIN')[0] == 'ready=1', 'BEGIN')
st, j = sim.req('list'); check(st == 200 and j['ready'] and j['items'] == [] and j['max'] == 79 and j['limit'] == 40, f'leere Liste {j}')

# ---- 16 Frames hochladen ----------------------------------------------------------------------
st, j = upload(sim, 'Balken Test', 16, 60, seed=5)
check(st == 200 and j['ok'] and j['id'] == 1, f'Upload 16 Frames: {st} {j}')
pl16 = payload(16, 60, 5)
st, j = sim.req('list'); it = j['items'][0] if j['items'] else {}
check(len(j['items']) == 1 and it['id'] == 1 and it['broken'] is False and it['name'] == 'Balken Test' and it['frames'] == 16 and it['ms'] == 60
      and it['bytes'] == 448 + len(pl16) and it['thumb'] == b64u(thumb(pl16, 16)), f'Liste nach Upload: {it}')
check(j['free'] > 0 and j['total'] == 1500000, 'Speicherangaben')
st, j = sim.req('get', id=1)
check(st == 200 and j['name'] == 'Balken Test' and j['frames'] == 16 and j['ms'] == 60
      and base64.urlsafe_b64decode(j['data'] + '=' * (-len(j['data']) % 4)) == pl16, 'Abruf = hochgeladene Nutzlast')
# Senden aus der Datei: Nutzlast in 128-Byte-Scheiben, wie makeChunk sie liest
r = sim.cmd('PREP', 1)[0].split()
check(r[:5] == ['LEN', str(len(pl16)), '1', '16', '60'] and ' '.join(r[5:]) == 'Balken Test', f'PREP {r}')
got = b''
for off in range(0, len(pl16), 128):
    n = min(128, len(pl16) - off); got += bytes.fromhex(sim.cmd('SLICE', off, n)[0])
check(got == pl16, 'Scheiben der Datei = Nutzlast')
check(bytes.fromhex(sim.cmd('SLICE', 5000, 700)[0]) == pl16[5000:5700], 'Scheibe ueber Seitengrenze')
check(sim.cmd('CLEAN')[0] == 'closed=1', 'Datei nach Auftrag geschlossen')

# ---- Grenzen / Fehler beim Upload ---------------------------------------------------------------------
st, j = upload(sim, 'Max', 79, 8, seed=9)
check(st == 200 and j['id'] == 2, f'79 Frames: {st} {j}')
pl79 = payload(79, 8, 9); check(len(pl79) == 30363, 'Nutzlast 79 Frames = 30363')
for label, kw in [('80 Frames', dict(frames=80, total=27 + 80 * 384)), ('1 Frame', dict(frames=1, total=27 + 384)), ('ms 0', dict(ms=0)),
                  ('ms 2001', dict(ms=2001)), ('Summe falsch', dict(total=27 + 16 * 384 + 1)), ('Name leer', dict(name='  ')),
                  ('Vorschau fehlt', dict(thumb='')), ('Vorschau kurz', dict(thumb=b64u(bytes(100))))]:
    n = kw.get('frames', 16); st, j = upload(sim, 'X', max(n, 2), 50, seed=2, pl=payload(max(n, 2), 50, 2), over=kw)
    check(st == 400 and not j['ok'], f'{label} abgelehnt: {st} {j}')
files = sim.until_end('FS'); check(not any('up.tmp' in f for f in files), f'keine tmp-Datei nach Ablehnung: {files}')
# schlechtes Base64
st, j = sim.req('put', off=0, name='B', frames=2, ms=5, total=27 + 768, crc=0, thumb=b64u(bytes(384)), data='a*b'); check(st == 400, 'Base64 ungueltig')
st, j = sim.req('put', off=0, name='B', frames=2, ms=5, total=27 + 768, crc=0, thumb=b64u(bytes(384)), data=''); check(st == 400, 'Daten leer')
# Reihenfolge
plx = payload(8, 40, 3)
st, j = sim.req('put', off=0, name='R', frames=8, ms=40, total=len(plx), crc=zlib.crc32(plx), thumb=b64u(thumb(plx, 8)), data=b64u(plx[:1200])); check(st == 200, 'Reihenfolge: Stueck 0')
st, j = sim.req('put', off=2400, data=b64u(plx[2400:2400 + 1200])); check(st == 409, f'Luecke abgelehnt {st}')
st, j = sim.req('put', off=1200, data=b64u(plx[1200:2400])); check(st == 200 and j['id'] == 0, 'Stueck 1 nach Fehler weiter moeglich')
st, j = sim.req('put', abort=1); check(st == 200, 'abort')
st, j = sim.req('put', off=2400, data=b64u(plx[2400:])); check(st == 409, 'nach abort kein Weiter')
# falscher Nutzlastkopf (Frame-Byte) -> beim Abschluss abgelehnt, nichts gespeichert, tmp weg
bad = bytearray(payload(4, 70, 4)); bad[24] = 3
st, j = upload(sim, 'Falsch', 4, 70, pl=bytes(bad)); check(st == 400, f'Kopf passt nicht: {st} {j}')
bad = bytearray(payload(4, 70, 4)); bad[3] = 1
st, j = upload(sim, 'Falsch2', 4, 70, pl=bytes(bad)); check(st == 400, 'reservierte Bytes muessen 0 sein')
check(not any('up.tmp' in f for f in sim.until_end('FS')), 'tmp weg nach Abschlussfehler')
st, j = sim.req('list'); check([i['id'] for i in j['items']] == [1, 2], f'nur gueltige Eintraege: {[i["id"] for i in j["items"]]}')

# ---- Namen ---------------------------------------------------------------------------------------------
st, j = upload(sim, ' \x07A\x01b  \x7f', 2, 10, seed=1); check(st == 200, 'Name mit Steuerzeichen')
st, jl = sim.req('list'); check(jl['items'][-1]['name'] == 'Ab', f'Steuerzeichen entfernt: {jl["items"][-1]["name"]!r}')
st, j = upload(sim, 'ä' * 30, 2, 10, seed=1); check(st == 200, 'langer Name')
st, jl = sim.req('list'); nm = jl['items'][-1]['name']; check(nm == 'ä' * 20 and len(nm.encode()) == 40, f'UTF-8 sauber bei 40 Byte gekappt: {nm!r}')
id_long = jl['items'][-1]['id']

# ---- Umbenennen / Duplizieren / Loeschen ---------------------------------------------------------------
st, j = sim.req('rename', id=1, name='Neuer Name'); check(st == 200 and j['ok'], 'rename')
st, jl = sim.req('list'); check(jl['items'][0]['name'] == 'Neuer Name' and jl['items'][0]['thumb'] == b64u(thumb(pl16, 16)) and jl['items'][0]['frames'] == 16, 'Name neu, Rest unveraendert')
st, j = sim.req('get', id=1); check(base64.urlsafe_b64decode(j['data'] + '=' * (-len(j['data']) % 4)) == pl16, 'Nutzlast nach rename unveraendert')
check(sim.req('rename', id=1, name='')[0] == 400 and sim.req('rename', id=99, name='x')[0] == 404 and sim.req('rename', name='x')[0] == 400, 'rename Fehler')
sim.cmd('STATE', 'sending')
check(sim.req('rename', id=1, name='x')[0] == 409 and sim.req('delete', id=1)[0] == 409, 'waehrend BLE-Auftrag gesperrt')
check(upload(sim, 'x', 2, 10)[0] == 409, 'Upload waehrend Auftrag gesperrt')
sim.cmd('STATE', 'idle')
check('onAnimDuplicate' not in anim_h and 'httpRoute("/api/anim/duplicate"' not in ino, 'Duplizieren-Route und -Handler sind entfernt (0.6.1.3)')
dup = id_long + 1
st, j = upload(sim, 'Kopie fuer Loeschtest', 16, 60, seed=5); check(st == 200 and j['id'] == dup, f'Upload statt Duplizieren {j}')
check(sim.req('delete', id=dup)[0] == 200 and sim.req('delete', id=dup)[0] == 404 and sim.req('delete')[0] == 404, 'delete')
st, jl = sim.req('list'); check(dup not in [i['id'] for i in jl['items']], 'geloescht')

# ---- Senden ----------------------------------------------------------------------------------------------
sim.cmd('STATE', 'queued')
check(sim.req('send', id=1)[0] == 409, 'send bei Auftrag: 409')
sim.cmd('STATE', 'idle')
check(sim.req('send', id=99)[0] == 404, 'send unbekannt')
st, j = sim.req('send', id=1); check(st == 202 and j['ok'] and j['id'] == 1, f'send {st} {j}')
v = sim.cmd('VARS')[0]; check('job=9' in v and 'pending=0' in v and 'autoProgram=0' in v and 'prog=0' in v and 'id=1' in v and 'state=queued' in v, f'Auftrag ANIM eingereiht: {v}')

# ---- beschaedigte Dateien ---------------------------------------------------------------------------------
sim.cmd('STATE', 'idle')
sim.cmd('POKE', '/rb-anim/a2.rba', 0, 88)
def item(i): return [x for x in sim.req('list')[1]['items'] if x['id'] == i][0]
check(item(2)['broken'] is True and item(2)['name'] == '' and sim.req('get', id=2)[0] == 404 and sim.req('send', id=2)[0] == 404
      and sim.req('rename', id=2, name='x')[0] == 404, 'Magic kaputt -> als broken gemeldet, sonst 404')
r = sim.cmd('PREP', 2)[0].split(); check(r[1] == '0', 'PREP lehnt kaputte Datei ab')
sim.cmd('TRUNC', '/rb-anim/a1.rba', 5000)
check(item(1)['broken'] is True and item(1)['bytes'] == 5000 and sim.req('send', id=1)[0] == 404, 'abgeschnittene Datei -> broken/404')
sim.cmd('POKE', '/rb-anim/a2.rba', 0, 82)   # Magic wieder 'R'
check(item(2)['broken'] is False and item(2)['frames'] == 79, 'Magic repariert -> wieder gueltig')
check(sim.req('delete', id=1)[0] == 200 and 1 not in [i['id'] for i in sim.req('list')[1]['items']], 'beschaedigte Datei laesst sich loeschen')

# ---- Speicher / Limits ------------------------------------------------------------------------------------
sim.cmd('TOTAL', 60000)
st, j = upload(sim, 'Gross', 79, 8, seed=2); check(st == 507, f'zu wenig Platz: {st} {j}')
sim.cmd('TOTAL', 1500000)
sim.cmd('FAILWRITES', 1)
st, j = upload(sim, 'Fehlschlag', 4, 10, seed=2); check(st == 500, f'Schreibfehler: {st} {j}')
sim.cmd('FAILWRITES', 0)
check(not any('up.tmp' in f for f in sim.until_end('FS')), 'tmp weg nach Schreibfehler')
n_now = len(sim.req('list')[1]['items'])
i = 0
while n_now < 40:
    st, j = upload(sim, f'F{i}', 2, 10, seed=i); check(st == 200, f'Fuellen {i}: {st} {j}'); n_now += 1; i += 1
st, j = upload(sim, 'Zuviel', 2, 10); check(st == 409, f'41. Animation abgelehnt: {st} {j}')
check(len(sim.req('list')[1]['items']) == 40, 'Liste zeigt 40')
ids = [i['id'] for i in sim.req('list')[1]['items']]; check(ids == sorted(ids), 'Liste aufsteigend nach ID')

# ---- v0.6.1.2: "nur senden" (voruebergehende Datei) und Kurzwahl-Hilfen -----------------------------------
# Liste ist hier bei 40 Eintraegen: eine voruebergehende Animation zaehlt nicht mit und erscheint nicht in der Liste.
plt = payload(8, 33, 21)
st, j = upload(sim, 'Nur senden', 8, 33, pl=plt, over=dict(temp=1))
check(st == 200 and j['ok'] and j['id'] == 60000, f'temp-Upload trotz 40 Eintraegen: {st} {j}')
fs_now = sim.until_end('FS')
check(any(l.startswith('/rb-anim/send.rba ') for l in fs_now) and not any('up.tmp' in l for l in fs_now), f'send.rba vorhanden, up.tmp weg: {fs_now[-4:]}')
check(len(sim.req('list')[1]['items']) == 40 and 60000 not in [i['id'] for i in sim.req('list')[1]['items']], 'temp nicht in der Liste, Zaehlung unveraendert')
r = sim.cmd('PREP', 60000)[0].split(); check(r[:5] == ['LEN', str(len(plt)), '1', '8', '33'], f'PREP temp {r}')
got = b''.join(bytes.fromhex(sim.cmd('SLICE', o2, min(128, len(plt) - o2))[0]) for o2 in range(0, len(plt), 128)); check(got == plt, 'temp-Nutzlast aus der Datei = hochgeladen')
sim.cmd('CLEAN')
# zweiter temp-Upload ersetzt den ersten
plt2 = payload(4, 20, 22)
st, j = upload(sim, 'Nur senden 2', 4, 20, pl=plt2, over=dict(temp=1)); check(st == 200 and j['id'] == 60000, 'zweiter temp-Upload ersetzt')
r = sim.cmd('PREP', 60000)[0].split(); check(r[1] == str(len(plt2)) and r[3] == '4', f'PREP temp 2 {r}'); sim.cmd('CLEAN')
# Senden mit temp=1
sim.cmd('STATE', 'idle')
st, j = sim.req('send', temp=1); check(st == 202 and j['ok'] and j['id'] == 60000, f'send temp {st} {j}')
v = sim.cmd('VARS')[0]; check('job=9' in v and 'id=60000' in v and 'state=queued' in v and 'pending=0' in v, f'ANIM-Auftrag fuer temp: {v}')
# nach dem Auftrag: loeschen und Kennung zuruecksetzen
check(sim.cmd('DROP')[0] == 'pending=0', 'DROP setzt Kennung zurueck')
check(not any(l.startswith('/rb-anim/send.rba') for l in sim.until_end('FS')), 'send.rba nach Auftrag geloescht')
check(sim.cmd('DROP')[0] == 'pending=0', 'DROP ohne temp ist harmlos')
# DROP laesst normale Animationen in Ruhe
sim.cmd('STATE', 'idle'); sim.req('send', id=2); check(sim.cmd('DROP')[0] == 'pending=2' and any('a2.rba' in l for l in sim.until_end('FS')), 'DROP beruehrt gespeicherte Animationen nicht')
sim.cmd('STATE', 'idle')
# send temp ohne Datei: 404; temp waehrend Auftrag gesperrt
check(sim.req('send', temp=1)[0] == 404, 'send temp ohne Datei: 404')
sim.cmd('STATE', 'sending'); check(upload(sim, 'x', 2, 10, over=dict(temp=1))[0] == 409, 'temp-Upload waehrend Auftrag gesperrt'); sim.cmd('STATE', 'idle')
# temp-Upload mit Abschlussfehler hinterlaesst nichts
bad = bytearray(payload(4, 70, 4)); bad[24] = 3
st, j = upload(sim, 'Falsch', 4, 70, pl=bytes(bad), over=dict(temp=1)); check(st == 400, 'temp: Kopf passt nicht')
fs_now = sim.until_end('FS'); check(not any('send.rba' in l or 'up.tmp' in l for l in fs_now), 'temp: nichts uebrig nach Fehler')
# Rest eines "nur senden" wird beim Start geloescht
st, j = upload(sim, 'Rest', 2, 10, over=dict(temp=1)); check(st == 200, 'temp fuer Startpruefung')
sim.cmd('BEGIN'); check(not any('send.rba' in l for l in sim.until_end('FS')), 'BEGIN loescht voruebergehende Datei')
# Kurzwahl-Hilfen
check([sim.cmd('EXISTS', n)[0] for n in (2, 1, 0, 10000, 99)] == ['1', '0', '0', '0', '0'], 'rb61AnimExists: vorhanden / geloescht / 0 / zu gross / unbekannt')
sim.cmd('STATE', 'idle')
q = sim.cmd('QUEUE', 2)[0]; check(q.startswith('1 job=9 pending=2 state=queued'), f'rb61QueueAnim: {q}')
sim.cmd('STATE', 'idle'); check(sim.cmd('QUEUE', 1)[0].startswith('0 '), 'rb61QueueAnim: unbekannt -> false')
sim.cmd('STATE', 'idle')

# ---- Bearbeiten: vorhandene Animation unter derselben ID ersetzen (0.6.1.2) -----------------------------------
n_before = len(sim.req('list')[1]['items']); ids_before = [i['id'] for i in sim.req('list')[1]['items']]
plr = payload(5, 77, seed=42)
st, j = upload(sim, 'Bearbeitet', 5, 77, pl=plr, over=dict(replace=2))
check(st == 200 and j['ok'] and j['id'] == 2, f'replace=2 trotz voller Liste (40): {st} {j}')
L = sim.req('list')[1]['items']
check(len(L) == n_before and [i['id'] for i in L] == ids_before, 'replace: Anzahl und IDs unveraendert')
it = [i for i in L if i['id'] == 2][0]; check(it['name'] == 'Bearbeitet' and it['frames'] == 5 and it['ms'] == 77 and not it['broken'], f'replace: Kopf neu {it}')
r = sim.cmd('PREP', 2)[0].split(); check(r[:5] == ['LEN', str(len(plr)), '1', '5', '77'], f'replace: PREP {r}')
got = b''.join(bytes.fromhex(sim.cmd('SLICE', o2, min(128, len(plr) - o2))[0]) for o2 in range(0, len(plr), 128)); sim.cmd('CLEAN')
check(got == plr, 'replace: Nutzlast = hochgeladen')
check(not any('up.tmp' in l for l in sim.until_end('FS')), 'replace: keine Zwischendatei uebrig')
check(upload(sim, 'x', 2, 10, over=dict(replace=9999))[0] == 404 and upload(sim, 'x', 2, 10, over=dict(replace=99))[0] == 404, 'replace unbekannte ID: 404')
check(upload(sim, 'x', 2, 10, over=dict(replace=2, temp=1))[0] == 400, 'replace + temp: 400')
check(upload(sim, 'x', 2, 10, over=dict(replace=-3))[0] == 400, 'replace negativ: 400')
sim.cmd('STATE', 'sending'); check(upload(sim, 'x', 2, 10, over=dict(replace=2))[0] == 409, 'replace waehrend Auftrag gesperrt'); sim.cmd('STATE', 'idle')
st, j = upload(sim, 'Nochmal', 3, 15, seed=7, over=dict(replace=2)); check(st == 200 and j['id'] == 2, 'replace zweites Mal')
bad2 = bytearray(payload(4, 70)); bad2[3] = 9
st, j = upload(sim, 'Falsch', 4, 70, pl=bytes(bad2), over=dict(replace=2)); check(st == 400, 'replace: Kopf passt nicht -> 400')
it = [i for i in sim.req('list')[1]['items'] if i['id'] == 2][0]; check(it['frames'] == 3 and it['name'] == 'Nochmal' and not it['broken'], f'Fehler beim Ersetzen laesst das Original unberuehrt {it}')
check(sim.cmd('EXISTS', 2)[0] == '1', 'Original nach fehlgeschlagenem Ersetzen vorhanden')


# ======================================================================================================
# ---- v0.6.1.3: Kurzwahlen beim Loeschen leeren (M2), Pruefsumme (M1), Lesefehler (M1), sicheres Ersetzen (M3), Altlasten (N1)
# ======================================================================================================
def scs(): return sim.cmd('SC')[0]
def scf(): return sim.cmd('SCFILE')[0]
def content(i):
    st, j = sim.req('get', id=i)
    return base64.urlsafe_b64decode(j['data'] + '=' * (-len(j['data']) % 4)) if st == 200 else None
L = [i['id'] for i in sim.req('list')[1]['items']]; A, B = L[0], L[1]
sim.cmd('SETSC', 0, 4, A, 'AnimA'); sim.cmd('SETSC', 1, 4, B, 'AnimB'); sim.cmd('SETSC', 2, 1, A, 'Bibliothek'); sim.cmd('SETSC', 3, 3, A, 'Ambiente')
# beschaedigte Kurzwahldatei: Loeschen verweigert, Animation bleibt
sim.cmd('DAMAGED', 1)
st, j = sim.req('delete', id=A); check(st == 500 and 'Kurzwahl' in j['message'], f'Loeschen bei beschaedigter Kurzwahldatei verweigert: {st} {j}')
check(sim.cmd('EXISTS', A)[0] == '1', 'Animation bleibt, wenn die Kurzwahlen nicht gesichert werden koennen')
sim.cmd('DAMAGED', 0)
# Schreibfehler: dasselbe, und die Kurzwahlen im Speicher bleiben unveraendert
sim.cmd('FAILWRITES', 1)
st, j = sim.req('delete', id=A); check(st == 500, f'Loeschen bei Schreibfehler der Kurzwahlen: {st}')
check(sim.cmd('EXISTS', A)[0] == '1' and scs().startswith(f'4:{A}:AnimA '), f'nichts geaendert: {scs()}')
# Animation ohne Kurzwahl darf auch bei Schreibfehler geloescht werden (es wird dann nichts geschrieben)
Z = L[-1]; check(f'4:{Z}:' not in scs(), 'Z hat keine Kurzwahl')
check(sim.req('delete', id=Z)[0] == 200, 'Loeschen ohne Kurzwahl schreibt keine Kurzwahldatei')
sim.cmd('FAILWRITES', 0)
# regulaer: nur Typ 4 mit dieser ID wird geleert; Typ 1/3 mit gleicher Zahl und andere Animationen bleiben
st, j = sim.req('delete', id=A); check(st == 200, f'Loeschen mit Kurzwahl: {st} {j}')
sc = scs().split(' ')
check(sc[0] == '0:0:' and sc[1] == f'4:{B}:AnimB' and sc[2] == f'1:{A}:Bibliothek' and sc[3] == f'3:{A}:Ambiente', f'nur Typ 4 mit ID {A} geleert: {sc}')
check(scf().split()[:4] == ['0:0', f'4:{B}', f'1:{A}', f'3:{A}'], f'Kurzwahldatei geschrieben: {scf()}')
check(sim.cmd('EXISTS', A)[0] == '0', 'Animation geloescht')
# zwei Kurzwahlen auf dieselbe Animation: beide leer
sim.cmd('SETSC', 4, 4, B, 'Zweite'); check(sim.req('delete', id=B)[0] == 200, 'delete B')
sc = scs().split(' '); check(sc[1] == '0:0:' and sc[4] == '0:0:', f'beide Kurzwahlen auf B geleert: {sc}')
sim.cmd('SETSC', 2, 0, 0, ''); sim.cmd('SETSC', 3, 0, 0, '')

# ---- Pruefsumme ------------------------------------------------------------------------------------------
check(sim.cmd('CRC', b'123456789'.hex())[0] == str(0xCBF43926), 'CRC-32 "123456789" = 0xCBF43926')
for n in (0, 1, 27, 255, 256, 257, 30363):
    raw = bytes(random.Random(n).randrange(256) for _ in range(n)); check(sim.cmd('CRC', raw.hex() or '')[0] == str(zlib.crc32(raw)), f'CRC gleich zlib, n={n}')
plc = payload(6, 90, seed=31)
def put0(pl, **over):
    kw = dict(off=0, name='C', frames=6, ms=90, total=len(pl), crc=zlib.crc32(pl), thumb=b64u(thumb(pl, 6)), data=b64u(pl[:1200])); kw.update(over)
    return sim.req('put', **kw)
nlist = len(sim.req('list')[1]['items'])
for label, over in [('crc fehlt', dict(crc='')), ('crc Buchstaben', dict(crc='abc')), ('crc mit Rest', dict(crc='12x')), ('crc zu lang', dict(crc='99999999999')), ('crc negativ', dict(crc='-5')), ('crc ueber 2^32', dict(crc='4294967296'))]:
    st, j = put0(plc, **over); check(st == 400 and not j['ok'], f'{label}: 400 ({st})')
st, j = put0(plc); check(st == 200, 'gueltiger Anfang'); sim.req('put', abort=1)
bad = bytearray(plc); bad[2000] ^= 0x10
st, j = upload(sim, 'Verfaelscht', 6, 90, pl=bytes(bad), over=dict(crc=zlib.crc32(plc)))
check(st == 422 and not j['ok'] and 'Pruefsumme' in j['message'], f'verfaelschte Daten: {st} {j}')
check(len(sim.req('list')[1]['items']) == nlist and not any('up.tmp' in l for l in sim.until_end('FS')), 'nach Pruefsummenfehler: nichts gespeichert, tmp weg')
check(any('Pruefsumme' in e for e in sim.until_end('EVENTS')), 'Pruefsummenfehler im Ereignisprotokoll')
st, j = upload(sim, 'Gut', 6, 90, pl=plc); check(st == 200 and j['ok'], f'gueltige Pruefsumme: {st} {j}')
N = j['id']; PN = f'/rb-anim/a{N}.rba'
check(sim.cmd('BYTES', PN, 12, 1)[0] == '01' and sim.cmd('BYTES', PN, 55, 4)[0] == zlib.crc32(plc).to_bytes(4, 'little').hex(), 'Kopf: flags Bit 0 und CRC (little endian) bei Byte 55')
check(sim.cmd('BYTES', PN, 59, 5)[0] == '0000000000', 'Rest des Kopfes bleibt 0')
check(sim.cmd('PREP', N)[0].split()[1] == str(len(plc)), 'PREP: gueltige Datei'); sim.cmd('CLEAN')
st, j = sim.req('rename', id=N, name='Gut umbenannt'); check(st == 200, 'rename')
check(sim.cmd('BYTES', PN, 55, 4)[0] == zlib.crc32(plc).to_bytes(4, 'little').hex() and sim.cmd('PREP', N)[0].split()[1] == str(len(plc)), 'Umbenennen laesst die Pruefsumme unberuehrt'); sim.cmd('CLEAN')
# Datei kippt nachtraeglich (Flash-Fehler): Senden wird vor Bluetooth abgelehnt
sim.cmd('POKE', PN, 448 + 777, 0x5A)
r = sim.cmd('PREP', N)[0].split(); check(r[1] == '0' and r[2] == '0', f'PREP lehnt Datei mit falscher Pruefsumme ab: {r}')
check(sim.cmd('CLEAN')[0] == 'closed=1', 'Datei nach Ablehnung geschlossen')
check(any('Senden abgebrochen' in e and 'Pruefsumme' in e for e in sim.until_end('EVENTS')), 'Ablehnung im Protokoll')
check(item(N)['broken'] is False, 'Kopf ist noch in Ordnung (Liste zeigt sie weiter, der Fehler steckt in den Nutzdaten)')
# alte Datei ohne Pruefsumme (flags = 0) bleibt benutzbar, auch wenn die Nutzdaten nicht geprueft werden
sim.cmd('POKE', PN, 12, 0); check(sim.cmd('PREP', N)[0].split()[1] == str(len(plc)), 'flags=0: ohne Pruefung akzeptiert (Dateien aus 0.6.1.0 bis 0.6.1.2)'); sim.cmd('CLEAN')
sim.cmd('POKE', PN, 12, 2); check(item(N)['broken'] is True, 'unbekannte Flags: Datei gilt als beschaedigt')
sim.cmd('POKE', PN, 12, 1); sim.cmd('POKE', PN, 448 + 777, plc[777])
check(sim.cmd('PREP', N)[0].split()[1] == str(len(plc)), 'Datei wieder heil: PREP ok'); sim.cmd('CLEAN')

# ---- Lesefehler beim Senden (M1) -----------------------------------------------------------------------------
PNf = PN
check(sim.cmd('READFAILED')[0] == '0', 'Merker anfangs aus')
sim.cmd('FAILREAD', PNf, 0); r = sim.cmd('PREP', N)[0].split(); check(r[1] == '0', f'Lesefehler schon beim Pruefen: PREP lehnt ab {r}'); sim.cmd('CLEAN')
sim.cmd('FAILREAD', PNf, 4); r = sim.cmd('PREP', N)[0].split(); check(r[1] == '0', f'Lesefehler mitten in der Pruefsumme: PREP lehnt ab {r}'); sim.cmd('CLEAN')
sim.cmd('FAILREAD', PNf, -1)
check(sim.cmd('PREP', N)[0].split()[1] == str(len(plc)) and sim.cmd('READFAILED')[0] == '0', 'PREP ok, Merker aus')
got = b''.join(bytes.fromhex(sim.cmd('SLICE', o2, min(128, len(plc) - o2))[0]) for o2 in range(0, len(plc), 128))
check(got == plc and sim.cmd('READFAILED')[0] == '0', 'fehlerfreies Lesen setzt den Merker nicht')
sim.cmd('FAILREAD', PNf, 0); out = bytes.fromhex(sim.cmd('SLICE', 128, 128)[0])
check(sim.cmd('READFAILED')[0] == '1' and out == bytes(128), 'Lesefehler: Merker gesetzt (Nullen werden nicht als Daten ausgegeben)')
sim.cmd('FAILREAD', PNf, -1); sim.cmd('CLEAN')
check(sim.cmd('PREP', N)[0].split()[1] == str(len(plc)) and sim.cmd('READFAILED')[0] == '0', 'naechster Auftrag: PREP setzt den Merker zurueck'); sim.cmd('CLEAN')
# Lesen hinter dem Dateiende zaehlt ebenfalls als Fehler
sim.cmd('PREP', N); sim.cmd('SLICE', len(plc) - 10, 128); check(sim.cmd('READFAILED')[0] == '1', 'Lesen ueber das Ende hinaus = Fehler'); sim.cmd('CLEAN')

# ---- Ersetzen (M3) ------------------------------------------------------------------------------------------
def fs_has(sub): return [l for l in sim.until_end('FS') if sub in l]
plA = payload(5, 77, seed=51); plB = payload(7, 66, seed=52); plC = payload(3, 55, seed=53)
st, j = upload(sim, 'Ersetzen', 5, 77, pl=plA, over=dict(replace=N)); check(st == 200 and j['id'] == N and content(N) == plA, 'Ersetzen normal (rename ueberschreibt)')
sim.cmd('RENAMEOVERWRITE', 0)
st, j = upload(sim, 'Ersetzen', 7, 66, pl=plB, over=dict(replace=N)); check(st == 200 and content(N) == plB, f'Ersetzen ohne Ueberschreiben: ueber .bak {st} {j}')
check(not fs_has('.bak') and not fs_has('up.tmp'), 'danach weder .bak noch up.tmp')
check(sim.cmd('BYTES', PN, 55, 4)[0] == zlib.crc32(plB).to_bytes(4, 'little').hex(), 'neue Pruefsumme im Kopf')
# 3. rename (Zwischendatei -> Ziel) scheitert, Rueckgabe der alten klappt: alte Animation unversehrt, Fehlermeldung
sim.cmd('FAILRENAME', 3, 3)
st, j = upload(sim, 'Ersetzen', 3, 55, pl=plC, over=dict(replace=N)); check(st == 500 and 'alte bleibt erhalten' in j['message'], f'Ersetzen scheitert: {st} {j}')
sim.cmd('FAILRENAME', 0, 0)
check(content(N) == plB and not fs_has('.bak') and not fs_has('up.tmp'), 'alte Animation unversehrt, keine Reste')
# schon das Beiseitelegen (2. rename) scheitert: alte bleibt am Platz
sim.cmd('FAILRENAME', 2, 2)
st, j = upload(sim, 'Ersetzen', 3, 55, pl=plC, over=dict(replace=N)); check(st == 500, 'Beiseitelegen scheitert')
sim.cmd('FAILRENAME', 0, 0); check(content(N) == plB and not fs_has('.bak') and not fs_has('up.tmp'), 'alte unversehrt (Beiseitelegen)')
# 3. UND 4. rename scheitern (Stromausfall-aehnlich): alte liegt als .bak da, wird beim Start zurueckgeholt
sim.cmd('FAILRENAME', 3, 4)
st, j = upload(sim, 'Ersetzen', 3, 55, pl=plC, over=dict(replace=N)); check(st == 500, 'Ersetzen und Rueckgabe scheitern')
sim.cmd('FAILRENAME', 0, 0)
check(sim.cmd('EXISTS', N)[0] == '0' and fs_has(f'a{N}.rba.bak'), 'alte Animation liegt als .bak da')
check(all(i['id'] != N for i in sim.req('list')[1]['items']), '.bak erscheint nicht in der Liste')
check(any('.bak' in e for e in sim.until_end('EVENTS')), 'Ereignis protokolliert')
sim.cmd('BEGIN'); check(sim.cmd('EXISTS', N)[0] == '1' and content(N) == plB and not fs_has('.bak'), 'BEGIN holt die alte Animation aus der Sicherung zurueck')
check(any('wiederhergestellt' in e for e in sim.until_end('EVENTS')), 'Wiederherstellung protokolliert')
# beide vorhanden: Sicherung wird verworfen, die eigentliche Datei bleibt
sim.cmd('COPY', PN, PN + '.bak'); sim.cmd('POKE', PN + '.bak', 100, 9)
sim.cmd('BEGIN'); check(not fs_has('.bak') and content(N) == plB, 'BEGIN: Sicherung neben vorhandener Datei wird entfernt')
sim.cmd('RENAMEOVERWRITE', 1)

# ---- Altlast: unfertiger Upload zaehlt nicht bei der Platzpruefung mit (N1) -------------------------------------------
X = [i['id'] for i in sim.req('list')[1]['items']][3]
pl16s = payload(16, 20, seed=61)
st, j = sim.req('put', off=0, name='Halb', frames=16, ms=20, total=len(pl16s), crc=zlib.crc32(pl16s), thumb=b64u(thumb(pl16s, 16)), data=b64u(pl16s[:1200]))
check(st == 200 and fs_has('up.tmp'), 'angefangener Upload liegt als up.tmp da')
info = sim.req('list')[1]; plbig = payload(79, 10, seed=62); need = len(plbig) + 448 + 24576
sim.cmd('TOTAL', info['total'] - info['free'] + need - 100)   # mit up.tmp zu wenig Platz, ohne passt es
st, j = upload(sim, 'Gross', 79, 10, pl=plbig, over=dict(replace=X))
check(st == 200 and content(X) == plbig, f'neuer Upload raeumt zuerst die Altlast weg, dann reicht der Platz: {st} {j}')
sim.cmd('TOTAL', 60000); st, j = upload(sim, 'Gross', 79, 10, pl=plbig, over=dict(replace=X)); check(st == 507, 'zu wenig Platz bleibt 507 (und raeumt trotzdem das tmp)')
check(not fs_has('up.tmp'), 'kein up.tmp nach 507')
sim.cmd('TOTAL', 1500000)
# alte voruebergehende Datei zaehlt ebenfalls nicht mit
st, j = upload(sim, 'T1', 79, 10, pl=plbig, over=dict(temp=1)); check(st == 200, 'temp 79 Frames')
info = sim.req('list')[1]; sim.cmd('TOTAL', info['total'] - info['free'] + need - 100)
st, j = upload(sim, 'T2', 79, 10, pl=plbig, over=dict(temp=1)); check(st == 200, f'zweites temp: altes send.rba zaehlt nicht mit {st} {j}')
sim.cmd('TOTAL', 1500000); sim.cmd('STATE', 'idle'); sim.cmd('DROP')

# ---- Start / Dateisystem --------------------------------------------------------------------------------
sim.cmd('TMPFILE', '/rb-anim/up.tmp')
sim.cmd('BEGIN'); check(not any('up.tmp' in f for f in sim.until_end('FS')), 'BEGIN raeumt unfertigen Upload weg')
ev = sim.until_end('EVENTS'); check(any('Unfertiger' in e for e in ev), 'Ereignis protokolliert')
sim.cmd('MOUNT', 0)
st, j = sim.req('list'); check(st == 200 and j['ready'] is False, 'nicht eingebunden: ready=false')
check(sim.req('put', off=0, data='AAAA')[0] == 503 and sim.req('send', id=1)[0] == 503 and sim.req('get', id=1)[0] == 503, 'nicht eingebunden: 503')

print('FAILS:', len(fails))
sys.exit(1 if fails else 0)
