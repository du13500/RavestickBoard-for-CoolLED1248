"""v0.4.3.2-dev: Programme-Ansicht (Variante B) gegen eine simulierte API.
Aufruf: python tests/test_0432.py [Screenshot-Ordner]
Benötigt Playwright + Chromium. Ersetzt keinen Arduino-Compile und keinen Hardwaretest."""
import json, os, re, subprocess, sys, tempfile
from pathlib import Path
from playwright.sync_api import sync_playwright

root = Path(__file__).resolve().parents[1]
html = (root / 'web/index.html').read_text(encoding='utf-8')
shots = Path(sys.argv[1]) if len(sys.argv) > 1 else None
if shots: shots.mkdir(parents=True, exist_ok=True)

# JavaScript syntax
js = '\n'.join(re.findall(r'<script(?![^>]*src)[^>]*>(.*?)</script>', html, re.S))
with tempfile.NamedTemporaryFile('w', suffix='.js', delete=False) as f:
    f.write(js); tmp = f.name
subprocess.run(['node', '--check', tmp], check=True); os.unlink(tmp)

MOCK = r"""
window.__calls=[];
const lib=[{id:1,name:'Rave Intro',layout:0,color:1,mode:2,kind:'text',pages:0},
 {id:2,name:'Willkommen Gäste',layout:0,color:2,mode:2,kind:'text',pages:0},
 {id:3,name:'Pixel Banane',layout:5,color:1,mode:0,kind:'block',pages:4},
 {id:4,name:'Getränkekarte',layout:0,color:3,mode:2,kind:'text',pages:0},
 {id:5,name:'Countdown Show',layout:5,color:1,mode:0,kind:'block',pages:6}];
const cols={1:{id:1,name:'Abendprogramm',seconds:20,playback:0,ids:[1,3,2,5,4]},
 2:{id:2,name:'Warm-up',seconds:20,playback:1,ids:[1,2,4]},
 3:{id:3,name:'Kaputtes Programm',seconds:20,playback:0,ids:[1,99]}};
let nextId=4;
const miss=c=>c.ids.filter(i=>!lib.some(l=>l.id===i)).length;
const R=(o,ok=true)=>({ok,status:ok?200:400,json:async()=>JSON.parse(JSON.stringify(o)),text:async()=>JSON.stringify(o)});
window.fetch=async(u,opt={})=>{u=String(u);const [p,q]=u.split('?');const body=opt.body?Object.fromEntries(new URLSearchParams(String(opt.body))):{};
 window.__calls.push([p,q||'',body]);
 if(p==='/api/font')return R({"65":[14,17,17,31,17,17,17]});
 if(p==='/api/status')return R({state:'idle',message:'bereit',playing:false,current:0,programPlaying:false,powerKnown:true,powerOn:true,brightness:80,speed:50});
 if(p==='/api/library')return R({ok:true,ready:true,count:5,total:1000000,used:1000,shortcuts:[0,0,0,0,0,0,0,0],items:lib});
 if(p==='/api/collections')return R({ok:true,items:Object.values(cols).map(c=>({id:c.id,name:c.name,count:c.ids.length,missing:miss(c),seconds:c.seconds,playback:c.playback,ids:c.ids}))});
 if(p==='/api/collections/get'){const c=cols[new URLSearchParams(q).get('id')];return R({ok:true,...c});}
 if(p==='/api/collections/save'){const id=body.id?Number(body.id):nextId++;cols[id]={id,name:body.name,seconds:Number(body.seconds),playback:Number(body.playback),ids:body.ids.split(',').map(Number)};return R({ok:true,id,message:'Programm gespeichert.'});}
 if(p==='/api/collections/delete'){delete cols[body.id];return R({ok:true,message:'ok'});}
 if(p==='/api/collections/run')return R({ok:true,message:'Programm gestartet.'});
 if(p==='/api/shortcuts')return R({ok:true,items:[]});
 if(p==='/api/program')return R({ok:true,pages:[],current:0});
 return R({ok:true,message:'ok'});};
"""

def vis(page, sel): return page.locator(sel).is_visible()
def rows(page, sel): return page.locator(sel + ' .library-row').count()

with sync_playwright() as pw:
    browser = pw.chromium.launch(args=['--no-sandbox', '--disable-dev-shm-usage'])
    for w, hgt in [(360, 740), (390, 844), (430, 932)]:
        ctx = browser.new_context(viewport={'width': w, 'height': hgt}, device_scale_factor=2, is_mobile=True, has_touch=True)
        page = ctx.new_page(); errors = []
        page.on('pageerror', lambda e: errors.append(str(e)))
        answer = {'mode': 'fail', 'seen': []}
        def on_dialog(d):
            answer['seen'].append(d.message)
            (d.accept if answer['mode'] == 'accept' else d.dismiss)()
        page.on('dialog', on_dialog)
        page.set_content(html.replace('<script>', '<script>' + MOCK + '</script><script>', 1), wait_until='domcontentloaded')
        page.wait_for_function('!document.documentElement.classList.contains("rb-booting")', timeout=15000)
        page.evaluate("navigate('programs')"); page.wait_for_timeout(300)

        # 1. list state
        assert vis(page, '#v43ProgListPane') and not vis(page, '#v43ProgEditPane')
        assert rows(page, '#v42Programs') == 3
        assert page.inner_text('#v43ProgCount') == '3 Programme'
        for i in ('v43ProgEdit', 'v42RunProgram', 'v42DeleteProgram'): assert page.locator('#' + i).is_disabled(), i
        assert page.locator('#v42StopProgram').is_enabled() and page.locator('#v42NewProgram').is_enabled()
        ys = [page.locator(f'#{i}').bounding_box()['y'] for i in ('v42NewProgram', 'v43ProgEdit', 'v42RunProgram', 'v42StopProgram', 'v42DeleteProgram')]
        assert max(ys) - min(ys) < 2, ('Werkzeugleiste umbricht', w, ys)
        assert 'Original' in page.inner_text('#v42Programs .library-row:nth-child(1) small')
        assert 'Gemeinsames Laufband' in page.inner_text('#v42Programs .library-row:nth-child(2) small')
        assert 'fehlt' in page.inner_text('#v42Programs .library-row:nth-child(3) small')
        assert page.locator('#v42Programs .library-row.v43-prog-invalid').count() == 1
        if shots: page.screenshot(path=str(shots / f'liste_{w}.png'), full_page=True)

        # 2. select + edit
        page.click('#v42Programs .library-row:nth-child(1) .lib-pick'); page.wait_for_timeout(200)
        assert page.locator('#v42Programs .library-row.selected').count() == 1
        for i in ('v43ProgEdit', 'v42RunProgram', 'v42DeleteProgram'): assert page.locator('#' + i).is_enabled(), i
        page.click('#v43ProgEdit')
        assert vis(page, '#v43ProgEditPane') and not vis(page, '#v43ProgListPane')
        assert page.inner_text('#v43ProgHeading') == 'Abendprogramm' and '5 Einträge' in page.inner_text('#v43ProgSub')
        assert rows(page, '#v42ProgramSteps') == 5
        assert 'Blockfolge · 4 Seiten' in page.inner_text('#v42ProgramSteps .library-row:nth-child(2) small')
        assert page.locator('#v42ProgramSteps .library-row:nth-child(1) button').nth(0).is_disabled()
        assert page.locator('#v42ProgramSteps .library-row:nth-child(5) button').nth(1).is_disabled()
        assert page.evaluate('document.documentElement.scrollWidth<=window.innerWidth'), 'horizontaler Überlauf'
        if shots: page.screenshot(path=str(shots / f'bearbeiten_{w}.png'), full_page=True)

        # 3. dirty -> back asks, cancel keeps, accept discards and reloads
        page.locator('#v42ProgramSteps .library-row:nth-child(2) button').nth(0).click()
        assert page.inner_text('#v42ProgramSteps .library-row:nth-child(1) .lib-name').endswith('Pixel Banane')
        answer['mode'] = 'dismiss'; answer['seen'].clear()
        page.click('#v43ProgBack'); page.wait_for_timeout(100)
        assert answer['seen'] and vis(page, '#v43ProgEditPane'), 'Abbrechen muss in der Bearbeitung bleiben'
        answer['mode'] = 'accept'; answer['seen'].clear()
        page.click('#v43ProgBack'); page.wait_for_timeout(300)
        assert answer['seen'], 'Rückfrage erwartet'
        assert vis(page, '#v43ProgListPane')
        page.click('#v43ProgEdit')
        assert page.inner_text('#v42ProgramSteps .library-row:nth-child(1) .lib-name').endswith('Rave Intro'), 'Verwerfen lädt Originalstand'
        # clean back: no dialog
        answer['seen'].clear()
        page.click('#v43ProgBack'); page.wait_for_timeout(200)
        assert vis(page, '#v43ProgListPane') and not answer['seen'], 'unerwarteter Dialog'

        # 4. edit + save returns to list
        page.click('#v43ProgEdit')
        page.select_option('#v42StepChoose', '2'); page.click('#v42StepAdd')
        assert rows(page, '#v42ProgramSteps') == 6
        page.fill('#v42ProgramName', 'Abendprogramm 2')
        page.click('#v42SaveProgram'); page.wait_for_timeout(400)
        assert vis(page, '#v43ProgListPane')
        saved = [c for c in page.evaluate('__calls') if c[0] == '/api/collections/save'][-1][2]
        assert saved['id'] == '1' and saved['name'] == 'Abendprogramm 2' and saved['ids'] == '1,3,2,5,4,2', saved
        assert 'Abendprogramm 2' in page.inner_text('#v42Programs')

        # 5. new program
        page.click('#v42NewProgram')
        assert vis(page, '#v43ProgEditPane') and page.inner_text('#v43ProgHeading') == 'Neues Programm'
        assert rows(page, '#v42ProgramSteps') == 0 and 'Noch keine Einträge' in page.inner_text('#v42ProgramSteps')
        page.fill('#v42ProgramName', 'Neu'); page.select_option('#v42StepChoose', '4'); page.click('#v42StepAdd')
        page.click('#v42SaveProgram'); page.wait_for_timeout(400)
        assert vis(page, '#v43ProgListPane') and rows(page, '#v42Programs') == 4
        # 6. delete selected (new one)
        answer['mode'] = 'accept'; answer['seen'].clear()
        page.click('#v42DeleteProgram'); page.wait_for_timeout(400)
        assert answer['seen'], 'Löschen fragt nach'
        assert rows(page, '#v42Programs') == 3 and page.locator('#v43ProgEdit').is_disabled()

        # 7. broken program: run disabled, replace select present
        page.click('#v42Programs .library-row:nth-child(3) .lib-pick'); page.wait_for_timeout(200)
        assert page.locator('#v42RunProgram').is_disabled()
        page.click('#v43ProgEdit')
        assert page.locator('#v42ProgramSteps .v42-program-invalid select').count() == 1
        assert 'fehlen' in page.inner_text('#v42ProgramError')
        if shots: page.screenshot(path=str(shots / f'defekt_{w}.png'), full_page=True)
        assert not errors, errors
        ctx.close()
    browser.close()
print('OK: Programme-Ansicht (Variante B) auf 360/390/430 px')
