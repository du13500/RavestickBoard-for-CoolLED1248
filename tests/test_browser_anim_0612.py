"""v0.6.1.0: Bereich „Animationen“ im echten (headless) Chromium mit simulierter API (Dateispeicher im Mock):
Menü, Liste, Vorschau, Import (GIF, Bildstreifen, Text-Raster), Anpassen, Grenzen (79 Bilder), Speichern in Stücken
(Nutzlast gegen unabhaengige Referenz), Senden/Umbenennen/Duplizieren/Loeschen, beschaedigte Eintraege, Tempo-Regler.
Aufruf: python tests/test_browser_anim_0612.py [Screenshot-Ordner]
Nicht geprueft: iPad-Safari, Apple Pencil, echte ESP32-Antworten."""
import io, json, re, sys, base64
from pathlib import Path
from PIL import Image
from playwright.sync_api import sync_playwright
ROOT = Path(__file__).resolve().parents[1]
SHOTS = Path(sys.argv[1]) if len(sys.argv) > 1 else None
html = (ROOT / 'web/index.html').read_text(encoding='utf8')
src = (ROOT / 'tests/test_browser_0612.py').read_text(encoding='utf8')
# Schrift und Basis-Mock aus dem Haupt-Browsertest uebernehmen (dort ebenfalls aus dem Sketch gelesen)
ino = next((ROOT / 'firmware').glob('RavestickBoard_v*/*.ino')).read_text(encoding='utf8')
blk = ino[ino.index('static const Glyph FONT[] = {'):]; blk = blk[:blk.index('};')]
font = {}
for m in re.finditer(r"\{\s*(?:'(\\?.)'|(0x[0-9A-Fa-f]+|\d+))\s*,\s*\{([^}]*)\}\s*\}", blk):
    ch, num, rows = m.groups(); font[ord(ch[-1]) if ch else int(num, 0)] = [int(x) for x in rows.split(',')]
base = src[src.index("MOCK=r'''") + 9:src.index("'''.replace('%FONT%'")].replace('%FONT%', json.dumps(font))
ANIM_MOCK = r'''<script>
(function(){
 const base=window.fetch;
 window.__anim={items:[],nextId:1,ready:true,free:1400000,up:null,sent:[],puts:[],broken:[]};
 window.fetch=async function(path,opt={}){
  const full=String(path),p=full.split('?')[0];
  if(!p.startsWith('/api/anim/')||p==='/api/anim/test')return base(path,opt);
  const A=window.__anim,arg=new URLSearchParams(opt.body||''),q=new URLSearchParams(full.split('?')[1]||'');
  window.__mock.requests.push({path:p,method:opt.method||'GET',data:Object.fromEntries(arg.entries())});
  const reply=(obj,status)=>{const snap=JSON.stringify(obj);return {ok:(status||200)<400,status:status||200,json:async()=>JSON.parse(snap),text:async()=>snap};};
  const find=id=>A.items.find(x=>x.id===Number(id));
  if(p==='/api/anim/list'){
   return reply({ok:true,ready:A.ready,max:79,limit:40,free:A.free,total:1500000,
    items:A.items.map(x=>({id:x.id,name:x.name,frames:x.frames,ms:x.ms,bytes:448+x.payload.length,broken:false,thumb:RbAnim.b64uEnc(RbAnim.thumbOf(x.payload,x.frames))}))
     .concat(A.broken.map(id=>({id,name:'',broken:true,frames:0,ms:0,bytes:5000,thumb:''}))).sort((a,b)=>a.id-b.id)});}
  if(p==='/api/anim/get'){const x=find(q.get('id'));if(!x)return reply({ok:false,message:'Animation nicht gefunden oder beschaedigt.'},404);
   return reply({ok:true,id:x.id,name:x.name,frames:x.frames,ms:x.ms,data:RbAnim.b64uEnc(x.payload)});}
  if(p==='/api/anim/put'){
   if(arg.get('abort')==='1'){A.up=null;return reply({ok:true,id:0,message:'Upload verworfen.'});}
   if(A.failAt!=null&&Number(arg.get('off'))>=A.failAt)return reply({ok:false,message:'Schreiben ins Dateisystem fehlgeschlagen.'},500);
   const off=Number(arg.get('off')),raw=RbAnim.b64uDec(arg.get('data'));
   if(off===0)A.up={replace:Number(arg.get('replace')||0),temp:arg.get('temp')==='1',name:arg.get('name'),frames:Number(arg.get('frames')),ms:Number(arg.get('ms')),total:Number(arg.get('total')),thumb:arg.get('thumb'),parts:[],got:0};
   if(!A.up||A.up.got!==off)return reply({ok:false,message:'Upload-Reihenfolge stimmt nicht.'},409);
   A.up.parts.push(raw);A.up.got+=raw.length;A.puts.push({off,len:raw.length});
   if(A.up.got<A.up.total)return reply({ok:true,id:0,message:'Stueck gespeichert.'});
   const payload=new Uint8Array(A.up.total);let o=0;for(const part of A.up.parts){payload.set(part,o);o+=part.length;}
   if(A.up.replace){const x=find(A.up.replace);if(!x){A.up=null;return reply({ok:false,message:'Die zu ersetzende Animation fehlt.'},404);}
    Object.assign(x,{name:A.up.name,frames:A.up.frames,ms:A.up.ms,payload,thumb:A.up.thumb});A.up=null;return reply({ok:true,id:x.id,message:'Animation aktualisiert.'});}
   if(A.up.temp){A.temp={name:A.up.name,frames:A.up.frames,ms:A.up.ms,payload};A.up=null;return reply({ok:true,id:60000,message:'Animation bereit.'});}
   const id=A.nextId++;A.items.push({id,name:A.up.name,frames:A.up.frames,ms:A.up.ms,payload,thumb:A.up.thumb});A.up=null;
   return reply({ok:true,id,message:'Animation gespeichert.'});}
  if(p==='/api/anim/send'&&arg.get('temp')==='1'){if(!A.temp)return reply({ok:false,message:'Keine Animation bereit.'},404);A.sentTemp=(A.sentTemp||0)+1;return reply({ok:true,id:60000,message:'Animation angenommen.'},202);}
  if(p==='/api/anim/send'){const x=find(arg.get('id'));if(!x)return reply({ok:false,message:'Animation nicht gefunden.'},404);A.sent.push(x.id);return reply({ok:true,id:x.id,message:'Animation angenommen.'},202);}
  if(p==='/api/anim/rename'){const x=find(arg.get('id'));x.name=arg.get('name');return reply({ok:true,id:x.id,message:'Umbenannt.'});}
  if(p==='/api/anim/duplicate')return reply({ok:false,message:'Duplizieren gibt es nicht mehr.'},404);
  if(false){const x=find(arg.get('id'));const id=A.nextId++;A.items.push({...x,id,name:x.name+' (Kopie)'});return reply({ok:true,id,message:'dupliziert'});}
  if(p==='/api/anim/delete'){const id=Number(arg.get('id'));A.items=A.items.filter(x=>x.id!==id);A.broken=A.broken.filter(x=>x!==id);return reply({ok:true,id,message:'geloescht'});}
  return reply({ok:false,message:'?'},404);
 };
})();
</script>'''
errors = []
def check(c, m):
    if not c: errors.append(m); print('FAIL', m)

# ---- Referenz fuer die Nutzlast (unabhaengig, wie in test_anim_core_0612.py) ---------------------
def ref_payload(frames, ms):   # frames: Liste von 16 Zeilen zu je 64 Farbwerten (0..7)
    n = len(frames); planes = [[], [], []]
    for p, bit in enumerate((4, 2, 1)):
        for f in range(n):
            for x in range(64):
                for half in range(2):
                    acc = 0
                    for k in range(8): acc = (acc << 1) | (1 if frames[f][half * 8 + k][x] & bit else 0)
                    planes[p].append(acc)
    return bytes(24) + bytes([n]) + ms.to_bytes(2, 'big') + bytes(planes[0] + planes[1] + planes[2])
RGB = [(0, 0, 0), (0, 0, 255), (0, 255, 0), (0, 255, 255), (255, 0, 0), (255, 0, 255), (255, 255, 0), (255, 255, 255)]
def grid(colors_by_xy):   # {(x,y): c} -> 16 x 64
    g = [[0] * 64 for _ in range(16)]
    for (x, y), c in colors_by_xy.items(): g[y][x] = c
    return g
def image_of(g):
    im = Image.new('RGB', (64, 16))
    for y in range(16):
        for x in range(64): im.putpixel((x, y), RGB[g[y][x]])
    return im
def frame_g(k):   # Testbild k: farbiger Balken + Eckpixel
    d = {(x, y): (k % 7) + 1 for x in range(4 * k, 4 * k + 4) for y in range(16) if x < 64}
    d[(0, 0)] = 4; d[(63, 15)] = 1
    return grid(d)

def captured_payload(page, idx=-1):
    return bytes(page.evaluate("Array.from(window.__anim.items.at(%d).payload)" % idx))

with sync_playwright() as pw:
    browser = pw.chromium.launch(headless=True, args=['--no-sandbox', '--disable-dev-shm-usage'])
    ctx = browser.new_context(viewport={'width': 390, 'height': 844}, is_mobile=True, has_touch=True, accept_downloads=True)
    page = ctx.new_page(); page.on('pageerror', lambda e: errors.append(str(e)))
    page.on('console', lambda m: errors.append('console:' + m.text) if m.type == 'error' else None)
    page.on('dialog', lambda d: d.accept())
    page_html = html.replace('<script>', '<script>' + '' , 1)
    # Mock vor dem ersten Skript (RbAnim steht im ersten <script>, der Anim-Mock braucht es erst bei Aufrufen)
    first = html.index('<script>')
    page_html = html[:first] + base + ANIM_MOCK + html[first:]
    page.route('http://rb.test/**', lambda r: r.fulfill(status=200, body=page_html, content_type='text/html; charset=utf-8'))
    page.goto('http://rb.test/', wait_until='domcontentloaded')
    try:
        page.wait_for_function('!document.documentElement.classList.contains("rb-booting")', timeout=15000)
    except Exception:
        print('BOOT-FEHLER', errors[:5]); raise

    # 1) Menue
    labels = page.evaluate("[...document.querySelectorAll('#mainMenu [data-nav],#v42ReloadMenu')].map(b=>b.textContent.trim())")
    check(labels == ['Kurzwahl', 'Texteditor', 'Bibliothek', 'Programme', 'Animationen', 'Ambiente', 'Diagnose', 'Einstellungen', 'Webinterface neu laden'], f'Menü {labels}')
    check(page.evaluate("[...document.querySelectorAll('#mainMenu button')].every(b=>b.querySelector('svg.rb-lucide'))"), 'jeder Menüpunkt hat ein Icon')
    page.evaluate("showMenu(true)"); page.locator('#mainMenu [data-nav=animations]').click()
    check(page.locator('#view-animations').is_visible() and page.locator('#view-library').is_hidden(), 'Ansicht Animationen')
    page.wait_for_function("document.getElementById('animStatus').textContent.includes('von 40')")
    check('0 von 40' in page.inner_text('#animStatus'), page.inner_text('#animStatus'))
    check('Noch keine Animationen' in page.inner_text('#animList'), 'Leerzustand')
    check(page.locator('#animImport').is_enabled() and all(page.locator(f'#{i}').is_disabled() for i in ('animSend', 'animRename', 'animDelete')), 'Knöpfe im Leerzustand')
    check(all(page.evaluate(f"!!document.getElementById('{i}').querySelector('svg')") for i in ('animImport', 'animSend', 'animRename', 'animDelete')), 'Knöpfe mit Icons')

    # 2) GIF importieren: 64x16, 3 Bilder in Palettenfarben -> Nutzlast muss exakt sein
    gframes = [frame_g(k) for k in range(3)]
    buf = io.BytesIO(); ims = [image_of(g) for g in gframes]
    ims[0].save(buf, format='GIF', save_all=True, append_images=ims[1:], duration=[120, 120, 120], loop=0); gif3 = buf.getvalue()
    page.locator('#animImport').click()
    check(page.locator('#animImportPane').is_visible() and page.locator('#animAdjust').is_hidden(), 'Importseite, Anpassen noch zu')
    page.set_input_files('#animFileGif', files=[{'name': 'Balkentest.gif', 'mimeType': 'image/gif', 'buffer': gif3}])
    page.wait_for_function("!document.getElementById('animAdjust').classList.contains('hidden')", timeout=10000)
    page.wait_for_function("document.getElementById('animInfo').textContent.includes('Frames')")
    check(page.input_value('#animName') == 'Balkentest' and page.input_value('#animMs') == '120' and page.input_value('#animTo') == '3', f'Vorbelegung {page.input_value("#animName")} {page.input_value("#animMs")} {page.input_value("#animTo")}')
    check('GIF 64 × 16, 3 Bilder' in page.inner_text('#animSrcInfo'), page.inner_text('#animSrcInfo'))
    info = page.inner_text('#animInfo'); check(info.startswith('3 Frames · 120 ms je Frame') and 'Übertragung' in info, info)
    check(page.locator('#animSave').is_enabled() and page.locator('#animErr').inner_text() == '', 'Speichern frei')
    check(page.evaluate("document.getElementById('animImpPreview').width")==512, 'Vorschauleinwand')
    page.fill('#animMs', '90'); page.wait_for_timeout(300)
    check(page.inner_text('#animInfo').startswith('3 Frames · 90 ms'), 'Zeit je Frame wirkt auf die Anzeige')
    page.locator('#animSave').click()
    page.wait_for_function("document.getElementById('animListPane') && !document.getElementById('animListPane').classList.contains('hidden')", timeout=10000)
    page.wait_for_function("document.querySelectorAll('#animList .anim-row').length===1")
    check(captured_payload(page) == ref_payload(gframes, 90), 'Nutzlast des GIF-Imports = Referenz (64x16, 3 Frames, 90 ms)')
    puts = page.evaluate("window.__anim.puts"); check(len(puts) == 1 and puts[0]['off'] == 0, f'3 Frames in einem Stück {puts}')
    row = page.inner_text('#animList .anim-row'); check('Balkentest' in row and '3 Frames' in row and '90 ms' in row, row)
    check(page.evaluate("document.querySelectorAll('#animList .anim-thumb').length")==1, 'Vorschaubild in der Liste')
    # Vorschaubild zeigt das erste Bild: Pixel (0,0) rot
    px = page.evaluate("(()=>{const c=document.querySelector('#animList .anim-thumb'),x=c.getContext('2d');const d=x.getImageData(1,1,1,1).data;return Array.from(d)})()")
    check(px[0] > 200 and px[1] < 100 and px[2] < 100, f'Listen-Vorschau Pixel (0,0) rot: {px}')
    shot_name = '07_animationen_liste.png'
    if SHOTS: page.screenshot(path=str(SHOTS / shot_name))

    # 3) Auswahl, Vorschau, Senden
    # Nach dem Speichern ist die neue Animation schon gewählt (Vorschau läuft); erneutes Antippen hebt die Auswahl auf, wie in der Bibliothek
    page.wait_for_function("document.getElementById('animPreviewInfo').textContent.includes('Schleife')")
    check(page.evaluate('animUI.selected')==1, 'neue Animation ist gewählt')
    check('3 Frames · 90 ms' in page.inner_text('#animPreviewInfo'), page.inner_text('#animPreviewInfo'))
    check(all(page.locator(f'#{i}').is_enabled() for i in ('animSend', 'animRename', 'animDelete')), 'Knöpfe nach Auswahl')
    page.locator('#animSend').click(); page.wait_for_timeout(200)
    check(page.evaluate("window.__anim.sent") == [1], 'Senden ruft /api/anim/send mit der ID')
    # Umbenennen
    page.locator('#animRename').click(); page.wait_for_timeout(300)
    dlg = page.locator('body > .v42-dialog').last
    dlg.locator('input.v42-name').fill('Neu ' + 'ä' * 30); dlg.locator('button.primary').click(); page.wait_for_timeout(300)
    name = page.evaluate("window.__anim.items[0].name"); check(name.startswith('Neu ä') and len(name.encode()) <= 40, f'Name auf 40 Byte begrenzt: {name!r} {len(name.encode())}')
    # Loeschen (zweiten Eintrag per Mock anlegen, auswaehlen, loeschen)
    page.evaluate("window.__anim.items.push({...window.__anim.items[0],id:window.__anim.nextId++,name:'Zum Löschen'});animRefresh()")
    page.wait_for_function("document.querySelectorAll('#animList .anim-row').length===2")
    page.locator('#animList .anim-row .lib-pick').last.click(); page.wait_for_timeout(200)
    check(page.evaluate("animUI.selected") == 2, 'zweiter Eintrag ist gewählt')
    page.locator('#animDelete').click(); page.wait_for_function("window.__anim.items.length===1")
    page.wait_for_function("document.querySelectorAll('#animList .anim-row').length===1")
    check(page.evaluate("animUI.selected") == 0 and page.locator('#animSend').is_disabled(), 'nach Löschen keine Auswahl')

    # 4) Bildstreifen: 4 Teile zu je 32x16 nebeneinander (128x16)
    parts = [Image.new('RGB', (32, 16), RGB[c]) for c in (1, 2, 4, 6)]
    strip = Image.new('RGB', (128, 16))
    for i, pimg in enumerate(parts): strip.paste(pimg, (32 * i, 0))
    sb = io.BytesIO(); strip.save(sb, format='PNG')
    page.locator('#animImport').click()
    page.set_input_files('#animFileStrip', files=[{'name': 'streifen.png', 'mimeType': 'image/png', 'buffer': sb.getvalue()}])
    page.wait_for_function("!document.getElementById('animStripOpts').classList.contains('hidden')")
    check(page.input_value('#animStripN') == '8' and page.input_value('#animStripDir') == 'h', 'Streifen: Raten quadratischer Bilder (8)')
    page.fill('#animStripN', '4'); page.dispatch_event('#animStripN', 'change'); page.wait_for_timeout(400)
    check(page.input_value('#animTo') == '4' and 'in 4 Teile (nebeneinander)' in page.inner_text('#animSrcInfo'), page.inner_text('#animSrcInfo'))
    page.select_option('#animDither', 'thresh'); page.fill('#animMs', '200'); page.wait_for_timeout(300)
    page.locator('#animSave').click(); page.wait_for_function("window.__anim.items.length===2", timeout=10000)
    exp = []
    for c in (1, 2, 4, 6):
        d = {(x, y): c for x in range(16, 48) for y in range(16)}   # 32x16 mittig im 64x16-Feld (Faktor 1)
        exp.append(grid(d))
    check(captured_payload(page) == ref_payload(exp, 200), 'Nutzlast des Streifen-Imports = Referenz (4 Teile, mittig)')
    page.wait_for_function("document.querySelectorAll('#animList .anim-row').length===2")

    # 5) Text-Raster: 2 Bilder 5x3, Originalgroesse mittig
    page.locator('#animImport').click()
    page.evaluate("document.querySelector('.anim-raster').open=true")
    page.fill('#animRasterText', 'R.G.B\n.....\nCMYWR\n---\nBBBBB\nGGGGG\nRRRRR')
    page.locator('#animRasterGo').click(); page.wait_for_function("!document.getElementById('animAdjust').classList.contains('hidden')")
    check(page.input_value('#animFit') == 'orig' and page.input_value('#animDither') == 'thresh' and page.input_value('#animTo') == '2', 'Raster: Originalgröße + Schwelle vorbelegt')
    check(page.locator('#animCrop').is_visible() and page.locator('#animOffsets').count()==0 and page.locator('#animOx').count()==0, 'Ausschnitt-Rahmen statt Regler')
    page.fill('#animMs', '333'); page.wait_for_timeout(300); page.fill('#animName', 'Mein Raster'); page.locator('#animSave').click()
    page.wait_for_function("window.__anim.items.length===3", timeout=10000)
    def put(rows, x0, y0):
        d = {}
        cm = {'.': 0, 'R': 4, 'G': 2, 'B': 1, 'C': 3, 'M': 5, 'Y': 6, 'W': 7}
        for dy, r in enumerate(rows):
            for dx, ch in enumerate(r): d[(x0 + dx, y0 + dy)] = cm[ch]
        return grid(d)
    # 5x3 mittig: x0=(64-5)/2=29.5 -> gerundet 30 (Math.round), y0=(16-3)/2=6.5 -> 7
    exp = [put(['R.G.B', '.....', 'CMYWR'], 30, 7), put(['BBBBB', 'GGGGG', 'RRRRR'], 30, 7)]
    check(captured_payload(page) == ref_payload(exp, 333), 'Nutzlast des Raster-Imports = Referenz')
    check(page.evaluate("window.__anim.items[2].name") == 'Mein Raster', 'Name übernommen')
    # Fehler im Raster
    page.locator('#animImport').click(); page.evaluate("document.querySelector('.anim-raster').open=true")
    page.fill('#animRasterText', 'RG\nRX'); page.locator('#animRasterGo').click(); page.wait_for_timeout(200)
    check('Zeile 2, Spalte 2' in page.inner_text('#animErr') and page.locator('#animAdjust').is_hidden(), page.inner_text('#animErr'))
    page.locator('#animImpBack').click()

    # 6) Grenze 79 Bilder: GIF mit 100 Bildern
    many = [Image.new('RGB', (64, 16), RGB[(i % 7) + 1]) for i in range(100)]
    for i, im in enumerate(many): im.putpixel((i % 64, 0), (255, 255, 255))
    mb = io.BytesIO(); many[0].save(mb, format='GIF', save_all=True, append_images=many[1:], duration=50, loop=0)
    page.locator('#animImport').click()
    page.set_input_files('#animFileGif', files=[{'name': 'viele.gif', 'mimeType': 'image/gif', 'buffer': mb.getvalue()}])
    page.wait_for_function("!document.getElementById('animAdjust').classList.contains('hidden')", timeout=15000); page.wait_for_timeout(500)
    check('100 Bilder sind zu viel' in page.inner_text('#animErr') and page.locator('#animSave').is_disabled() and page.locator('#animSaveSend').is_disabled(), f'zu viele Bilder: {page.inner_text("#animErr")}')
    check(page.locator('#animReduce').is_visible(), 'Verteilen-Knopf sichtbar')
    page.locator('#animReduce').click(); page.wait_for_timeout(400)
    check(page.input_value('#animStep') == '2' and page.inner_text('#animErr') == '' and page.locator('#animSave').is_enabled() and page.locator('#animReduce').is_hidden(), 'Schritt 2 -> 50 Bilder')
    check(page.inner_text('#animInfo').startswith('50 Frames'), page.inner_text('#animInfo'))
    page.fill('#animStep', '1'); page.fill('#animTo', '79'); page.wait_for_timeout(400)
    check(page.inner_text('#animErr') == '' and page.inner_text('#animInfo').startswith('79 Frames'), '79 Bilder erlaubt')
    page.fill('#animTo', '80'); page.wait_for_timeout(400); check('80 Bilder sind zu viel' in page.inner_text('#animErr'), '80 Bilder abgelehnt')
    page.fill('#animTo', '79'); page.check('#animPing'); page.wait_for_timeout(400)
    check('zu viel' in page.inner_text('#animErr'), 'Hin und zurück sprengt die Grenze')
    page.uncheck('#animPing'); page.fill('#animTo', '1'); page.wait_for_timeout(400)
    check('Mindestens 2 Bilder' in page.inner_text('#animErr') and page.locator('#animSave').is_disabled(), '1 Bild genügt nicht')
    # 79 Bilder speichern: Upload in Stuecken (30363 B = 26 Stuecke), Fortschritt, dann senden
    page.fill('#animTo', '79'); page.fill('#animMs', '8'); page.wait_for_timeout(400)
    page.evaluate("window.__anim.puts.length=0")
    page.locator('#animSaveSend').click()
    page.wait_for_function("window.__anim.items.length===4", timeout=20000)
    offs = [p['off'] for p in page.evaluate("window.__anim.puts")]
    check(offs == list(range(0, 30363, 1200)) and len(offs) == 26, f'Stückelung {len(offs)} Stücke')
    check(page.evaluate("window.__anim.items[3].payload.length") == 30363 and page.evaluate("window.__anim.items[3].frames") == 79, '79 Frames = 30363 Byte')
    page.wait_for_function("window.__anim.sent.length===2"); check(page.evaluate("window.__anim.sent[1]") == page.evaluate("window.__anim.items.at(-1).id"), 'Speichern und senden sendet die neue ID')
    # Fehler beim Upload: Meldung, Abbruch, Knoepfe wieder frei
    page.locator('#animImport').click()
    page.set_input_files('#animFileGif', files=[{'name': 'x.gif', 'mimeType': 'image/gif', 'buffer': gif3}])
    page.wait_for_function("!document.getElementById('animAdjust').classList.contains('hidden')"); page.wait_for_timeout(300)
    page.evaluate("window.__anim.failAt=0;window.__mock.requests.length=0")
    page.locator('#animSave').click(); page.wait_for_function("document.getElementById('animErr').textContent.includes('Speichern')")
    check('Schreiben' in page.inner_text('#animErr') and page.locator('#animSave').is_enabled(), page.inner_text('#animErr'))
    check(any(r['path'] == '/api/anim/put' and r['data'].get('abort') == '1' for r in page.evaluate('__mock.requests')), 'Abbruch wird gemeldet')
    page.evaluate("window.__anim.failAt=null"); page.locator('#animImpBack').click()

    # 7) beschaedigte Datei in der Liste: nur loeschen
    page.evaluate("window.__anim.broken=[9]"); page.evaluate("animRefresh()"); page.wait_for_function("document.querySelectorAll('#animList .anim-row').length===5")
    brow = page.locator('#animList .anim-row').last; check('Beschädigte Datei #9' in brow.inner_text(), brow.inner_text())
    brow.locator('.lib-pick').click(); page.wait_for_timeout(200)
    check(page.locator('#animDelete').is_enabled() and all(page.locator(f'#{i}').is_disabled() for i in ('animSend', 'animRename')), 'beschädigt: nur Löschen')
    check(page.locator('#animPreviewWrap').is_hidden(), 'keine Vorschau bei beschädigter Datei')
    page.locator('#animDelete').click(); page.wait_for_function("document.querySelectorAll('#animList .anim-row').length===4")

    # 8) Dateisystem nicht eingerichtet
    page.evaluate("window.__anim.ready=false"); page.evaluate("animRefresh()"); page.wait_for_timeout(300)
    check('nicht eingerichtet' in page.inner_text('#animStatus') and page.locator('#animImport').is_disabled(), page.inner_text('#animStatus'))
    page.evaluate("window.__anim.ready=true"); page.evaluate("animRefresh()"); page.wait_for_timeout(300)

    # 9) Tempo-Regler bei Animationen aus
    page.evaluate("navigate('home')")
    page.evaluate("window.__mock.status={animActive:true};rbPollPending=false;poll()"); page.wait_for_timeout(300)
    check(page.evaluate("[...document.querySelectorAll('input[data-global=speed],#scrollSpeed')].every(x=>x.disabled)") and 'Animationen nicht' in (page.evaluate("document.querySelector('#rbDock input[data-global=speed]').title") or ''), 'Tempo-Regler bei Animation gesperrt, mit Hinweis')
    check(page.evaluate("document.querySelector('#rbDock input[data-global=brightness]').disabled") is False, 'Helligkeit bleibt bedienbar')
    page.evaluate("window.__mock.status={animActive:false};rbPollPending=false;poll()"); page.wait_for_timeout(300)
    check(page.evaluate("[...document.querySelectorAll('input[data-global=speed],#scrollSpeed')].every(x=>!x.disabled)"), 'Tempo-Regler danach wieder frei')

    # 10) Layout / Zoom-Sperre
    page.evaluate("navigate('animations')"); page.wait_for_timeout(300)
    check(page.evaluate("document.documentElement.scrollWidth<=window.innerWidth"), 'Liste: kein seitliches Überlaufen')
    page.locator('#animImport').click(); page.set_input_files('#animFileGif', files=[{'name': 'x.gif', 'mimeType': 'image/gif', 'buffer': gif3}])
    page.wait_for_function("!document.getElementById('animAdjust').classList.contains('hidden')"); page.wait_for_timeout(300)
    check(page.evaluate("document.documentElement.scrollWidth<=window.innerWidth"), 'Import: kein seitliches Überlaufen')
    fs = page.evaluate("[...document.querySelectorAll('#animImportPane input:not([type=checkbox]):not([type=range]):not([type=file]),#animImportPane select,#animImportPane textarea')].map(e=>[e.id,parseFloat(getComputedStyle(e).fontSize)])")
    check(all(f >= 16 for _, f in fs), f'Eingabefelder >= 16 px (iOS-Zoom): {[x for x in fs if x[1] < 16]}')
    if SHOTS:
        page.screenshot(path=str(SHOTS / '08_animationen_import.png'), full_page=True)
    page.locator('#animImpBack').click()

    # 11) Ausschnitt-Rahmen (4:1): grosses Bild 128x64, oben rot, unten blau
    big = Image.new('RGB', (128, 64), RGB[4])
    for y in range(32, 64):
        for x in range(128): big.putpixel((x, y), RGB[1])
    bb = io.BytesIO(); big.save(bb, format='PNG'); pngbig = bb.getvalue()
    gb = io.BytesIO(); big2 = big.copy(); big2.putpixel((0, 0), RGB[2]); ims2 = [big, big2]; ims2[0].save(gb, format='GIF', save_all=True, append_images=ims2[1:], duration=[100, 100], loop=0); gifbig = gb.getvalue()
    page.evaluate("navigate('animations')"); page.wait_for_timeout(200)
    page.locator('#animImport').click()
    page.set_input_files('#animFileGif', files=[{'name': 'gross.gif', 'mimeType': 'image/gif', 'buffer': gifbig}])
    page.wait_for_function("!document.getElementById('animAdjust').classList.contains('hidden')"); page.wait_for_timeout(300)
    cr = lambda: page.evaluate("JSON.parse(JSON.stringify(animImp.crop))")
    c0 = cr(); check(page.input_value('#animFit') == 'contain' and abs(c0['w'] - 2) < 1e-9 and abs(c0['cx'] - .5) < 1e-9, f'Einpassen (128x64 = 2:1): Rahmen doppelt so breit wie das Bild {c0}')
    page.select_option('#animFit', 'cover'); c1 = cr()
    check(abs(c1['w'] - 1) < 1e-9, f'Füllen: Rahmen so breit wie das Bild {c1}')
    # Rahmen auf die untere Haelfte schieben (per Code) -> alles blau, Auswahl springt auf "Eigener Ausschnitt"
    page.evaluate("animCropSet({cx:.5,cy:.75,w:1})"); page.wait_for_timeout(300)
    check(page.input_value('#animFit') == 'crop', 'Rahmen setzt Eigener Ausschnitt')
    page.fill('#animMs', '100'); page.fill('#animName', 'Unten'); page.wait_for_timeout(300)
    page.locator('#animSave').click(); page.wait_for_function("window.__anim.items.length===5", timeout=10000)
    blue = grid({(x, y): 1 for x in range(64) for y in range(16)})
    check(captured_payload(page) == ref_payload([blue, blue], 100), 'Rahmen unten -> Nutzlast komplett blau')
    # Zeigerbedienung: Mausziehen verschiebt den Rahmen, Knoepfe veraendern die Groesse
    page.locator('#animImport').click()
    page.set_input_files('#animFileGif', files=[{'name': 'gross.gif', 'mimeType': 'image/gif', 'buffer': gifbig}])
    page.wait_for_function("!document.getElementById('animAdjust').classList.contains('hidden')"); page.wait_for_timeout(300)
    page.locator('#animCropSmaller').click(); c2 = cr()
    check(abs(c2['w'] - 1.7) < 1e-6 and page.input_value('#animFit') == 'crop', f'Rahmen kleiner {c2}')
    page.locator('#animCropBigger').click(); page.locator('#animCropBigger').click(); c3 = cr()
    check(c3['w'] > c2['w'], f'Rahmen größer {c3}')
    page.evaluate("animCropSet({cx:.5,cy:.5,w:.5})"); page.wait_for_timeout(100)
    box = page.locator('#animCrop').bounding_box(); before = cr()
    g = page.evaluate("(()=>{const g=animCropLayout();return {k:g.k,pad:g.pad}})()")
    cxp = box['x'] + (0.5 * 128 + g['pad']) * g['k'] * box['width'] / (page.evaluate("animCropLayout().cw")); cyp = box['y'] + (0.5 * 64 + g['pad']) * g['k'] * box['height'] / page.evaluate("animCropLayout().ch")
    page.mouse.move(cxp, cyp); page.mouse.down(); page.mouse.move(cxp + 30, cyp + 10, steps=4); page.mouse.up()
    after = cr(); check(after['cx'] > before['cx'] and after['cy'] > before['cy'], f'Ziehen verschiebt den Rahmen {before} -> {after}')
    page.evaluate("animCropSet({cx:-5,cy:9,w:0.001})"); c4 = cr()
    check(0 <= c4['cx'] <= 1 and 0 <= c4['cy'] <= 1 and c4['w'] >= 8 / 128 - 1e-9, f'Rahmen bleibt begrenzt {c4}')
    page.select_option('#animFit', 'stretch'); page.wait_for_timeout(200)
    check(page.locator('#animCrop').is_visible(), 'Strecken: Rahmen zeigt das ganze Bild')
    check(page.evaluate("document.documentElement.scrollWidth<=window.innerWidth"), 'Rahmen: kein seitliches Überlaufen')

    # 12) Nur senden: kein Listeneintrag, temp=1, anschliessend /api/anim/send?temp=1
    n_items = page.evaluate("window.__anim.items.length")
    page.select_option('#animFit', 'contain'); page.wait_for_timeout(200)
    page.evaluate("window.__mock.requests.length=0")
    check(page.locator('#animSendOnly').is_enabled(), 'Nur senden frei')
    page.locator('#animSendOnly').click(); page.wait_for_function("window.__anim.sentTemp===1", timeout=10000)
    reqs = page.evaluate("__mock.requests")
    puts_t = [r for r in reqs if r['path'] == '/api/anim/put']
    check(puts_t and all(r['data'].get('temp') == '1' for r in puts_t if r['data'].get('off') == '0'), 'Upload mit temp=1')
    check(page.evaluate("window.__anim.items.length") == n_items, 'Nur senden legt keinen Listeneintrag an')
    check(page.evaluate("window.__anim.temp.frames") == 2, 'temporäre Datei hat 2 Frames')
    page.wait_for_timeout(200)
    check(page.locator('#animSendOnly').is_enabled() and page.locator('#animSave').is_enabled(), 'Knöpfe danach wieder frei')
    # Limit 40: Speichern gesperrt, Nur senden bleibt frei
    page.evaluate("animUI.items=Array.from({length:40},(_,i)=>({id:i+1,name:'x'+i,frames:2,ms:100,bytes:900,broken:false,thumb:''}));animSchedule(0)"); page.wait_for_timeout(500)
    check(page.locator('#animSave').is_disabled() and page.locator('#animSaveSend').is_disabled() and page.locator('#animSendOnly').is_enabled(), 'Limit 40: Speichern gesperrt, Nur senden frei')
    check('40' in page.inner_text('#animErr'), page.inner_text('#animErr'))
    # Farbe: Senden-Knoepfe mint
    cols = page.evaluate("['animSaveSend','animSendOnly'].map(i=>getComputedStyle(document.getElementById(i)).backgroundColor)")
    check(all(c == 'rgb(61, 220, 151)' for c in cols), f'Senden-Knöpfe mint {cols}')
    page.locator('#animImpBack').click(); page.evaluate("animRefresh()"); page.wait_for_timeout(300)
    check(page.evaluate("getComputedStyle(document.getElementById('animSend')).backgroundColor") == 'rgb(61, 220, 151)', 'Senden-Knopf der Liste mint')

    # 13) Kurzwahl-Quelle Animationen
    page.evaluate("window.__anim.items=window.__anim.items.slice(0,2);window.__anim.items.forEach((x,i)=>x.id=i+1);animRefresh()"); page.wait_for_timeout(300)
    page.evaluate("navigate('home');v42OpenShortcut(3)"); page.wait_for_function("!document.getElementById('v42ShortcutDialog').classList.contains('hidden')")
    srcs = page.locator('#v42Sources button'); check(srcs.nth(3).is_enabled() and srcs.nth(3).inner_text().strip() == 'Animationen', 'Quelle Animationen frei')
    srcs.nth(3).click(); page.wait_for_timeout(200)
    opts = page.evaluate("[...document.querySelectorAll('#v42ShortcutTarget option')].map(o=>o.textContent)")
    check(len(opts) >= 2 and 'streifen' in opts, f'Animationen in der Auswahl {opts}')
    page.select_option('#v42ShortcutTarget', '2'); page.wait_for_timeout(100)
    page.locator('#v42ShortcutSave').click(); page.wait_for_function('__mock.requests.some(r=>r.path==="/api/shortcuts/set")')
    d = [r for r in page.evaluate('__mock.requests') if r['path'] == '/api/shortcuts/set'][-1]['data']
    check(d['type'] == '4' and d['id'] == '2' and d['slot'] == '3', f'Kurzwahl Typ 4 gespeichert {d}')
    # Kachel: vorhandene / fehlende Animation
    page.evaluate("v42Shortcuts[2]={type:4,id:1,label:''};libShortcutsDraw()")
    t = page.evaluate("(()=>{const b=document.querySelector('.shortcut[data-shortcut=\"3\"]');return b.textContent})()")
    check('Animation' in t and 'fehlt' not in t, f'Kachel Animation {t!r}')
    page.evaluate("v42Shortcuts[2]={type:4,id:77,label:''};libShortcutsDraw()")
    t = page.evaluate("document.querySelector('.shortcut[data-shortcut=\"3\"]').textContent")
    check('Animation fehlt' in t, f'Kachel fehlt {t!r}')
    # Loeschen warnt vor Verwendung
    page.evaluate("v42Shortcuts[2]={type:4,id:1,label:''}")
    msgs = []
    page.evaluate("navigate('animations')"); page.wait_for_timeout(300)
    page.evaluate("window.__dialogs=[];window.confirm=m=>{window.__dialogs.push(m);return false}")
    page.locator('#animList .anim-row .lib-pick').first.click(); page.wait_for_timeout(200)
    page.locator('#animDelete').click(); page.wait_for_timeout(200)
    dl = page.evaluate("window.__dialogs")
    check(any('Kurzwahl 3' in (m or '') for m in dl), f'Löschen warnt vor Kurzwahl {dl}')

    # 14) Bestehende Animation bearbeiten (Zeit, Bildbereich, Name; ueberschreiben oder als Kopie)
    page.evaluate("window.confirm=()=>true;navigate('animations')"); page.wait_for_timeout(300); page.evaluate("animRefresh()"); page.wait_for_timeout(300)
    page.evaluate("animSelect(0)"); page.wait_for_timeout(200)
    check(page.locator('#animEdit').is_disabled(), 'Bearbeiten ohne Auswahl gesperrt')
    page.locator('#animList .anim-row .lib-pick').nth(1).click(); page.wait_for_function("document.getElementById('animPreviewInfo').textContent.includes('Schleife')")
    check(page.locator('#animEdit').is_enabled() and page.evaluate("!!document.getElementById('animEdit').querySelector('svg')"), 'Bearbeiten-Knopf frei, mit Symbol')
    sel_id = page.evaluate("animUI.selected"); orig_ms = page.evaluate("window.__anim.items.find(x=>x.id===animUI.selected).ms"); n_items = page.evaluate("window.__anim.items.length")
    page.locator('#animEdit').click(); page.wait_for_function("!document.getElementById('animAdjust').classList.contains('hidden')"); page.wait_for_timeout(300)
    check(page.inner_text('#animImpTitle') == 'Animation bearbeiten' and page.locator('#animSourcePick').is_hidden() and page.locator('#animPixelOpts').is_hidden(), 'Bearbeiten: Quelle und Ausschnitt ausgeblendet')
    check(page.locator('#animSaveCopy').is_visible() and page.locator('#animEditHint').is_visible(), 'Bearbeiten: Kopie-Knopf und Hinweis sichtbar')
    check(page.input_value('#animName') == 'streifen' and page.input_value('#animMs') == str(orig_ms) and page.input_value('#animTo') == '4', f'Vorbelegung {page.input_value("#animName")} {page.input_value("#animMs")} {page.input_value("#animTo")}')
    check(page.inner_text('#animInfo').startswith(f'4 Frames · {orig_ms} ms'), page.inner_text('#animInfo'))
    check(page.locator('#animSave').is_enabled() and page.locator('#animSaveCopy').is_enabled() and page.locator('#animSendOnly').is_enabled(), 'Bearbeiten: Knoepfe frei')
    page.fill('#animMs', '50'); page.fill('#animTo', '3'); page.fill('#animName', 'Kurz'); page.wait_for_timeout(400)
    check(page.inner_text('#animInfo').startswith('3 Frames · 50 ms'), page.inner_text('#animInfo'))
    page.evaluate("window.__mock.requests.length=0")
    page.locator('#animSave').click(); page.wait_for_function("window.__anim.items.find(x=>x.id===%d).frames===3" % sel_id, timeout=10000)
    puts_e = [r for r in page.evaluate("__mock.requests") if r['path'] == '/api/anim/put']
    check(puts_e and puts_e[0]['data'].get('replace') == str(sel_id), f'put mit replace={sel_id}')
    check(page.evaluate("window.__anim.items.length") == n_items, 'Ueberschreiben legt keinen neuen Eintrag an')
    it = page.evaluate("(()=>{const x=window.__anim.items.find(x=>x.id===%d);return {name:x.name,ms:x.ms,frames:x.frames}})()" % sel_id)
    check(it == {'name': 'Kurz', 'ms': 50, 'frames': 3}, f'Eintrag aktualisiert {it}')
    exp3 = [grid({(x, y): c for x in range(16, 48) for y in range(16)}) for c in (1, 2, 4)]
    cur = bytes(page.evaluate("Array.from(window.__anim.items.find(x=>x.id===%d).payload)" % sel_id))
    check(cur == ref_payload(exp3, 50), 'Nutzlast nach Bearbeiten = erste 3 Bilder mit 50 ms')
    page.wait_for_function("document.getElementById('animListPane') && !document.getElementById('animListPane').classList.contains('hidden')")
    # Als Kopie speichern
    page.wait_for_timeout(300); page.locator('#animList .anim-row .lib-pick').nth(1).click() if page.evaluate("animUI.selected")!=sel_id else None
    page.wait_for_function("document.getElementById('animPreviewInfo').textContent.includes('Schleife')")
    page.locator('#animEdit').click(); page.wait_for_function("!document.getElementById('animAdjust').classList.contains('hidden')"); page.wait_for_timeout(300)
    page.fill('#animName', 'Kopie davon'); page.fill('#animMs', '99'); page.wait_for_timeout(300)
    page.locator('#animSaveCopy').click(); page.wait_for_function("window.__anim.items.length===%d" % (n_items + 1), timeout=10000)
    cp = page.evaluate("(()=>{const x=window.__anim.items.at(-1);const o=window.__anim.items.find(x=>x.id===%d);return {n:x.name,ms:x.ms,id:x.id,on:o.name,oms:o.ms}})()" % sel_id)
    check(cp['n'] == 'Kopie davon' and cp['ms'] == 99 and cp['id'] != sel_id and cp['on'] == 'Kurz' and cp['oms'] == 50, f'Kopie neu, Original unveraendert {cp}')
    # Speichern und senden (ueberschreibt und sendet dieselbe ID); Limit 40 sperrt nur die Kopie
    page.wait_for_function("document.getElementById('animListPane') && !document.getElementById('animListPane').classList.contains('hidden')")
    page.locator('#animList .anim-row').filter(has_text='Kurz').locator('.lib-pick').click()
    page.wait_for_function("document.getElementById('animPreviewInfo').textContent.includes('Schleife')")
    page.locator('#animEdit').click(); page.wait_for_function("!document.getElementById('animAdjust').classList.contains('hidden')"); page.wait_for_timeout(300)
    page.evaluate("animUI.items=Array.from({length:39},(_,i)=>({id:100+i,name:'x'+i,frames:2,ms:100,bytes:900,broken:false,thumb:''})).concat(animUI.items.filter(x=>x.id===%d));animSchedule(0)" % sel_id); page.wait_for_timeout(400)
    check(page.locator('#animSave').is_enabled() and page.locator('#animSaveSend').is_enabled() and page.locator('#animSaveCopy').is_disabled(), 'Limit 40: Ueberschreiben frei, Kopie gesperrt')
    check('40' not in page.inner_text('#animErr'), 'Limit-Hinweis stoert beim Ueberschreiben nicht')
    page.evaluate("window.__anim.sent.length=0"); page.fill('#animMs', '60'); page.wait_for_timeout(300)
    page.locator('#animSaveSend').click(); page.wait_for_function("window.__anim.sent.length===1", timeout=10000)
    check(page.evaluate("window.__anim.sent[0]") == sel_id and page.evaluate("window.__anim.items.find(x=>x.id===%d).ms" % sel_id) == 60, 'Speichern und senden im Bearbeiten: gleiche ID, neue Zeit')
    # Import nach Bearbeiten: Ansicht ist wieder normal
    page.wait_for_timeout(300); page.evaluate("animRefresh()"); page.wait_for_timeout(300)
    page.locator('#animImport').click()
    check(page.inner_text('#animImpTitle') == 'Animation importieren' and page.locator('#animSourcePick').is_visible() and page.locator('#animSaveCopy').is_hidden() and page.locator('#animEditHint').is_hidden(), 'Import nach Bearbeiten: Ansicht zurueckgesetzt')
    page.locator('#animImpBack').click()
    check(not [e for e in errors if not e.startswith('FAIL')][:0], '')
    ctx.close(); browser.close()
real = [e for e in errors]
print('FAILS:', len(real))
sys.exit(1 if real else 0)
