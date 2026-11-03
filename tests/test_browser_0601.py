"""v0.4.6.0 (aufbauend auf 0.4.5.x): Editor „Zeichen gestalten“ im echten (headless) Chromium mit simulierter API.
Prüft Stilmodell, Fenster (Einzeln/Wischen), Farbe, Schrift, Farbwechsel, Herz-Tausch, Rückgängig,
Transportformat (Senden/Bibliothek), globale Farbe/Schrift, zweizeilig, Sendefarbe, Regler.
Zusätzlich 0.4.5.0: Wischen-Entmarkieren, Schriftkästchen, mehrstufiges Rückgängig/Wiederherstellen,
Startbildschirm mit voller Tafelsteuerung, wachsendes Eingabefeld mit Ziehgriff, Programme sortieren.
Aufruf: python tests/test_browser_0601.py [Screenshot-Ordner]"""
import re, sys, json
from pathlib import Path
from playwright.sync_api import sync_playwright
ROOT=Path(__file__).resolve().parents[1]
SHOTS=Path(sys.argv[1]) if len(sys.argv)>1 else None
html=(ROOT/'web/index.html').read_text(encoding='utf8')
open_html=html
ino=next((ROOT/'firmware').glob('RavestickBoard_v*/*.ino')).read_text(encoding='utf8')
blk=ino[ino.index('static const Glyph FONT[] = {'):]; blk=blk[:blk.index('};')]
font={}
for m in re.finditer(r"\{\s*(?:'(\\?.)'|(0x[0-9A-Fa-f]+|\d+))\s*,\s*\{([^}]*)\}\s*\}",blk):
    ch,num,rows=m.groups(); font[ord(ch[-1]) if ch else int(num,0)]=[int(x) for x in rows.split(',')]
assert len(font)>=80,len(font)
MOCK=r'''<script>
window.__mock={entries:[],requests:[],nextId:1};
window.fetch=async function(path,opt={}){
 const full=String(path),p=full.split('?')[0];const arg=new URLSearchParams(opt.body||'');
 window.__mock.requests.push({path:p,method:opt.method||'GET',data:Object.fromEntries(arg.entries())});
 const m=window.__mock;let r={ok:true,message:'ok'};
 if(m.listDelay&&(p==='/api/library'||p==='/api/collections'))await new Promise(f=>setTimeout(f,m.listDelay));
 if(p==='/api/library/move'||p==='/api/collections/move'){
  if(m.moveDelay)await new Promise(f=>setTimeout(f,m.moveDelay));
  if(m.moveFail)return {ok:false,status:500,json:async()=>({ok:false,message:'Schreibfehler'}),text:async()=>'{}'};
  const list=p==='/api/library/move'?m.entries:m.programs,i=list.findIndex(x=>x.id===Number(arg.get('id')));
  if(i>=0){const [it]=list.splice(i,1);list.splice(Number(arg.get('to')),0,it);}
 }
 else if(p==='/api/font')r=%FONT%;
 else if(p==='/api/library')r={ready:true,items:m.entries.map(e=>({id:e.id,name:e.name,kind:e.kind,mode:e.mode,favorite:0,pages:0})),shortcuts:[0,0,0,0,0,0],used:200,total:100000};
 else if(p==='/api/library/get'){const id=Number(new URLSearchParams(full.split('?')[1]).get('id'));r=JSON.parse(JSON.stringify(m.entries.find(e=>e.id===id)));}
 else if(p==='/api/library/save'){const id=arg.has('id')?Number(arg.get('id')):m.nextId++;const prev=m.entries.find(e=>e.id===id);
  const row={id,name:arg.get('name'),text:arg.get('text'),line2:arg.get('line2'),layout:Number(arg.get('layout')),color1:Number(arg.get('color1')),color2:Number(arg.get('color2')),mode:Number(arg.get('mode')),kind:'text',favorite:0};
  if(prev)Object.assign(prev,row);else m.entries.push(row);r={ok:true,id};}
 else if(p==='/api/program')r={ok:true,pages:[{line1:'',line2:'',layout:3,align:1,color1:6,color2:7,duration:7}],current:0};
 else if(p==='/api/collections')r={ok:true,items:m.programs||[]};
 else if(p==='/api/shortcuts')r={ok:true,items:Array.from({length:8},()=>({type:0,id:0,label:''}))};
 else if(p==='/api/sign'){m.sign=m.sign||{kind:0,mine:{set:false,name:'',tail:''},active:{name:'',tail:''}};r=Object.assign({ok:true},m.sign);}
 else if(p==='/api/sign/scan'){r={ok:true,heard:9,all:arg.get('all')==='1',items:(m.signItems||[]).filter(x=>arg.get('all')==='1'||x.cool)};}
 else if(p==='/api/sign/save'||p==='/api/sign/use'||p==='/api/sign/back'){
  m.sign=m.sign||{kind:0,mine:{set:false,name:'',tail:''},active:{name:'',tail:''}};
  const tail=(arg.get('addr')||'').slice(12),name=arg.get('name')||'CoolLEDX';
  if(p==='/api/sign/save'){m.sign.mine={set:true,name,tail};m.sign.active={name,tail};m.sign.kind=1;}
  else if(p==='/api/sign/use'){const mine=m.sign.mine.set&&m.sign.mine.tail===tail;m.sign.active={name,tail};m.sign.kind=mine?1:2;}
  else{m.sign.active={name:m.sign.mine.name,tail:m.sign.mine.tail};m.sign.kind=1;}
  r={ok:true,message:'ok '+p};}
 else if(p==='/api/status')r=Object.assign({ok:true,state:'idle',message:'Bereit',brightness:80,speed:80,signKind:(m.sign||{kind:0}).kind,signName:(m.sign||{active:{name:''}}).active.name},m.status||{});
 const snap=JSON.stringify(r);   // wie echtes HTTP: die Seite bekommt eine Kopie, nie den Speicher des Mock-ESP
 return {ok:true,status:(m.force202&&(p==='/api/text'||p==='/api/program/show'))?202:200,json:async()=>JSON.parse(snap),text:async()=>snap};
};
</script>'''.replace('%FONT%',json.dumps(font))
HEART='❤️'
def U(ch): return chr(ch)
M=lambda n: chr(0x90+n)           # Farbmarke 1..7, 0 = Zeilenfarbe
F=lambda n: chr(0x98+n)           # Schriftmarke 0..3
FR=chr(0x9c)                      # Schrift der Zeile
errors=[]
def shot(page,name):
    if SHOTS: page.screenshot(path=str(SHOTS/name))
def st(page,k='a'): return page.evaluate(f"({{c:RBS.{k}.c,f:RBS.{k}.f,chars:RBS.{k}.chars}})")
def tiles(page,line='a'): return page.locator(f'#rbzGrids .rbz-tile[data-line="{line}"]')
def swatch(page,name,root='#rbzColors'): return page.locator(f'{root} .rbz-sw[aria-label^="{name}"]')
def open_win(page):
    page.locator('#rbzOpen').click(); page.wait_for_function('!document.getElementById("rbz").classList.contains("hidden")')
def close_win(page):
    page.locator('#rbzDone').click(); page.wait_for_function('document.getElementById("rbz").classList.contains("hidden")')

with sync_playwright() as pw:
    browser=pw.chromium.launch(headless=True,args=['--no-sandbox','--disable-dev-shm-usage'])
    ctx=browser.new_context(viewport={'width':390,'height':844},is_mobile=True,has_touch=True)
    page=ctx.new_page(); page.on('pageerror',lambda e:errors.append(str(e)))
    page.on('console',lambda m: errors.append('console:'+m.text) if m.type=='error' else None)
    page_html=html.replace('<script>',MOCK+'<script>',1)
    page.route('http://rb.test/**',lambda r:r.fulfill(status=200,body=page_html,content_type='text/html; charset=utf-8'))
    page.goto('http://rb.test/',wait_until='domcontentloaded')
    page.wait_for_function('!document.documentElement.classList.contains("rb-booting")',timeout=15000)
    page.evaluate("navigate('editor')")

    # 0) Sendefarbe und größere Regler
    for sel in ['#sendScroll','#libSend','#v42SendDraftBand','#v42RunProgram','#pageShow']:
        col=page.evaluate(f"getComputedStyle(document.querySelector('{sel}')).backgroundColor")
        assert col=='rgb(61, 220, 151)',(sel,col)
    assert page.evaluate("document.getElementById('rbzOpen')!==null"),'Palettenknopf fehlt'
    assert page.evaluate("getComputedStyle(document.getElementById('dockBrightness')).appearance")=='none'
    assert '::-webkit-slider-thumb{-webkit-appearance:none;appearance:none;width:26px;height:26px' in open_html
    h0=page.evaluate("document.getElementById('dockBrightness').getBoundingClientRect().height")
    assert h0>=28,h0

    # 1) Text tippen; Zeichen erben Stil
    page.fill('#scrollText','Hi Raver, Party!')
    assert st(page)['chars'].__len__()==16
    assert page.locator('#rbzOpen').is_visible()
    # Leerer Text öffnet kein Fenster
    page.fill('#scrollText',''); page.locator('#rbzOpen').click(); assert page.locator('#rbz.hidden').count()==1
    page.fill('#scrollText','Hi Raver, Party!')

    # 2) Einzeln: Tippen auf R-a-v-e-r, Cyan
    open_win(page); shot(page,'01_fenster_leer.png')
    assert page.locator('#rbzCount').inner_text().startswith('0 von 16')
    for i in range(3,8): tiles(page).nth(i).click()
    assert page.locator('#rbzCount').inner_text().startswith('5 von 16 gewählt')
    swatch(page,'Cyan').click()
    s=st(page); assert s['c']==[0]*3+[3]*5+[0]*8,s['c']
    shot(page,'02_fenster_einzeln.png')
    # abwählen durch erneutes Tippen
    tiles(page).nth(3).click(); assert page.locator('#rbzCount').inner_text().startswith('4 von 16')
    # Rückgängig
    page.locator('#rbzUndo').click(); assert st(page)['c']==[0]*16
    assert page.locator('#rbzRedo').is_enabled()                       # Wiederherstellen jetzt möglich
    page.locator('#rbzRedo').click(); assert st(page)['c']==[0]*3+[3]*5+[0]*8,st(page)['c']
    page.locator('#rbzUndo').click(); assert st(page)['c']==[0]*16
    # 3) Wischen
    assert page.locator('#rbzModeSwipe').count()==0 and page.locator('#rbzSeg').count()==0       # 0.4.5.0: kein Umschalter mehr
    assert page.evaluate("getComputedStyle(document.querySelector('.rbz-tiles')).touchAction")=='pan-y'   # senkrecht scrollt, waagerecht markiert
    assert 'waagerecht wischen markiert' in page.locator('#rbzHint').inner_text()
    page.locator('#rbzNone').click()
    b0=tiles(page).nth(8).bounding_box(); b1=tiles(page).nth(13).bounding_box()
    page.mouse.move(b0['x']+b0['width']/2,b0['y']+b0['height']/2); page.mouse.down()
    page.mouse.move(b1['x']+b1['width']/2,b1['y']+b1['height']/2,steps=12); page.mouse.up()
    sel=page.evaluate("[...rbz.sel.a].sort((a,b)=>a-b)"); assert sel==[8,9,10,11,12,13],sel
    shot(page,'03_fenster_wischen.png')
    # Zu viel markiert: einzeln abwählen (Tippen schaltet um)
    page.wait_for_timeout(450); tiles(page).nth(13).click(); assert page.evaluate("[...rbz.sel.a].sort((a,b)=>a-b)")==[8,9,10,11,12]
    swatch(page,'Gelb').click(); assert st(page)['c'][8:13]==[6]*5
    # Wischen auf bereits Markiertem entmarkiert (0.4.5.0): von 9 nach 11
    b0=tiles(page).nth(9).bounding_box(); b1=tiles(page).nth(11).bounding_box()
    page.mouse.move(b0['x']+b0['width']/2,b0['y']+b0['height']/2); page.mouse.down()
    page.mouse.move(b1['x']+b1['width']/2,b1['y']+b1['height']/2,steps=8); page.mouse.up()
    assert page.evaluate("[...rbz.sel.a].sort((a,b)=>a-b)")==[8,12],page.evaluate("[...rbz.sel.a]")
    # Wischen auf Unmarkiertem markiert wieder (von 9 nach 11)
    page.mouse.move(b0['x']+b0['width']/2,b0['y']+b0['height']/2); page.mouse.down()
    page.mouse.move(b1['x']+b1['width']/2,b1['y']+b1['height']/2,steps=8); page.mouse.up()
    assert page.evaluate("[...rbz.sel.a].sort((a,b)=>a-b)")==[8,9,10,11,12]
    # Einmal antippen bleibt ein Umschalter
    page.wait_for_timeout(450); tiles(page).nth(9).click(); assert 9 not in page.evaluate("[...rbz.sel.a]")
    # 4) Schrift: vier Kästchen sind direkt wählbar (kein großer Durchschalter mehr)
    assert page.locator('#rbzFont').count()==0
    page.locator('#rbzNone').click()
    chip=lambda n: page.locator(f'#rbzChips .chip[data-font="{n}"]')
    assert all(chip(n).is_disabled() for n in range(4))           # ohne Auswahl gesperrt
    for i in (0,1): tiles(page).nth(i).click()
    assert 'on' in chip(0).get_attribute('class').split() and chip(0).is_enabled()
    seq=[]
    for n in (1,2,3,0):
        chip(n).click(); seq.append(st(page)['f'][:2])
    assert seq==[[1,1],[2,2],[3,3],[-1,-1]],seq               # Normal = Zeilenschrift (-1)
    chip(1).click(); cls=lambda n: chip(n).get_attribute('class').split()
    assert 'on' in cls(1) and 'on' not in cls(0) and chip(1).get_attribute('aria-pressed')=='true'
    chip(3).click(); shot(page,'04_fenster_schrift.png')          # Mini
    tiles(page).nth(2).click()                                    # Auswahl gemischt (Mini, Mini, Normal)
    assert not any('on' in cls(n) for n in range(4)),'bei gemischter Auswahl leuchtet kein Kästchen'
    chip(0).click(); assert st(page)['f'][:3]==[-1,-1,-1]
    page.locator('#rbzUndo').click(); assert st(page)['f'][:2]==[3,3]
    # 5) Farbwechsel pro Zeichen (Leerzeichen zählen nicht mit)
    page.evaluate("RBS.a.f.fill(-1);RBS.a.c.fill(0);rbzRefresh()")
    page.locator('#rbzAll').click(); page.locator('#rbzFlowHead').click()
    assert page.locator('#rbzHint').is_hidden() and page.locator('#rbzColorBlock').is_hidden() and page.locator('#rbzFontBox').is_hidden()
    for n in ['Gelb','Magenta','Rot','Grün']: swatch(page,n,'#rbzFlowColors').click()
    assert swatch(page,'Magenta','#rbzFlowColors').inner_text().strip()=='2'
    assert 'Auf 16 Zeichen anwenden' in page.locator('#rbzFlowApplyText').inner_text()
    shot(page,'05_fenster_farbwechsel.png')
    page.locator('#rbzFlowApply').click()
    cc=st(page)['c']; txt='Hi Raver, Party!'; seqcol=[6,5,4,2]; n=0; exp=[]
    for ch in txt:
        if ch==' ': exp.append(0)
        else: exp.append(seqcol[n%4]); n+=1
    assert cc==exp,(cc,exp)
    # Pro Wort
    page.evaluate("RBS.a.c.fill(0);rbzRefresh()"); page.locator('#rbzAll').click()
    page.locator('#rbzFlowWord').click(); assert 'Auf 3 Wörter anwenden' in page.locator('#rbzFlowApplyText').inner_text()
    page.locator('#rbzFlowApply').click(); cc=st(page)['c']
    assert cc[:2]==[6,6] and cc[3:9]==[5]*6 and cc[10:16]==[4]*6,cc
    page.locator('#rbzFlowHead').click()
    close_win(page)

    # 6) Transport: senden
    page.fill('#scrollText','Hi Raver, Party!')                       # Text identisch -> Stil bleibt (kein Eingabe-Diff)
    page.evaluate("RBS.a.c.fill(0);RBS.a.f.fill(-1)")
    page.evaluate("for(const i of [3,4,5,6,7])RBS.a.c[i]=3;for(const i of [0,1])RBS.a.f[i]=1;")
    enc=page.evaluate("rbEncode('a')")
    exp=F(1)+'Hi'+FR+' '+M(3)+'Raver'+chr(0x90)+', Party!'
    assert enc==exp,(enc.encode('unicode_escape'),exp.encode('unicode_escape'))
    page.locator('#sendScroll').click(); page.wait_for_function('__mock.requests.some(r=>r.path==="/api/text")')
    req=[r for r in page.evaluate('__mock.requests') if r['path']=='/api/text'][-1]['data']
    assert req['text']==exp and req['layout']=='3' and req['line2']=='' and req['color']=='7',req
    # 7) Bibliothek: speichern, neu laden, wieder öffnen
    page.locator('#libSaveAs').click()
    page.locator('.v42-dialog:not(.hidden) input').fill('Stiltest'); page.locator('.v42-dialog:not(.hidden) .primary').click()
    page.wait_for_function('__mock.entries.length===1')
    saved=page.evaluate('__mock.entries[0]'); assert saved['text']==exp and saved['color1']==7,saved
    page.fill('#scrollText','Etwas anderes')
    page.evaluate("navigate('library')"); row=page.locator('#libList .library-row').filter(has_text='Stiltest')
    if 'selected' not in (row.get_attribute('class') or ''): row.locator('.lib-pick').click()      # ab 0.4.5.0 hebt ein zweiter Tipp die Auswahl auf
    page.locator('#libOpen').click()
    assert page.input_value('#scrollText')=='Hi Raver, Party!'
    s=st(page); assert s['c']==[0,0,0,3,3,3,3,3]+[0]*8 and s['f'][:2]==[1,1] and s['f'][2:]==[-1]*14,s
    assert page.evaluate("rbEncode('a')")==exp, 'Rundreise Speichern/Öffnen ist nicht verlustfrei'
    assert page.evaluate('v42EditorDirty()') is False
    # Alter Eintrag ohne Marken
    page.evaluate("__mock.entries.push({id:9,name:'Alt',text:'ALTER TEXT',line2:'',layout:2,color1:4,color2:7,mode:2,kind:'text',favorite:0})")
    page.evaluate("refreshLibrary()"); page.wait_for_timeout(100)
    page.evaluate("libraryOpenEditor(9)"); page.wait_for_function("document.getElementById('scrollText').value==='ALTER TEXT'")
    s=st(page); assert s['c']==[0]*10 and s['f']==[-1]*10
    assert page.evaluate("rbEncode('a')")=='ALTER TEXT'
    # 8) Neue Zeichen erben den Stil links; Löschen schiebt Stile mit
    page.evaluate("libraryOpenEditor(1)"); page.wait_for_function("document.getElementById('scrollText').value==='Hi Raver, Party!'")
    page.locator('#scrollText').focus(); page.evaluate("const e=document.getElementById('scrollText');e.setSelectionRange(8,8)")
    page.keyboard.type('X')            # hinter 'Raver' (Cyan) -> erbt Cyan
    s=st(page); assert page.input_value('#scrollText')=='Hi RaverX, Party!' and s['c'][8]==3 and s['c'][9]==0,(page.input_value('#scrollText'),s['c'])
    page.keyboard.press('Backspace'); s=st(page); assert page.input_value('#scrollText')=='Hi Raver, Party!' and s['c']==[0,0,0,3,3,3,3,3]+[0]*8
    page.evaluate("const e=document.getElementById('scrollText');e.setSelectionRange(3,3)")
    page.keyboard.press('Delete'); s=st(page)     # 'R' gelöscht
    assert page.input_value('#scrollText')=='Hi aver, Party!' and s['c'][:7]==[0,0,0,3,3,3,3],s['c']
    # 9) Globale Farbe überschreibt Einzelfarben (rückgängig möglich); globale Schrift setzt Schriften zurück
    page.locator('#scrollColors .swatch[aria-label="Rot"]').click()
    assert st(page)['c']==[0]*15 and page.evaluate('scrollColor')==4
    assert page.locator('#rbStyleUndo').count()==0,'„Stil rückgängig“ ist entfallen'
    page.locator('#rbUndo').click()
    assert st(page)['c'][3:7]==[3,3,3,3] and page.evaluate('scrollColor')==7
    page.locator('#scrollLayout').select_option('4'); assert st(page)['f']==[-1]*15 and st(page)['c'][3]==3
    page.locator('#rbUndo').click(); assert st(page)['f'][:2]==[1,1] and page.evaluate("Number(document.getElementById('scrollLayout').value)")==4   # Schriften zurück; Auswahlfeld bleibt auf der Zeilenschrift

    # 10) Herzen tauschen die Farbe
    page.locator('#scrollLayout').select_option('3'); page.fill('#scrollText','Du '+HEART+'!')
    open_win(page); tiles(page).nth(3).click(); swatch(page,'Cyan').click()
    assert page.input_value('#scrollText')=='Du \U0001FA75!',repr(page.input_value('#scrollText'))
    swatch(page,'Rot').click(); assert page.input_value('#scrollText')=='Du ❤️!'
    page.locator('#rbzUndo').click(); assert page.input_value('#scrollText')=='Du \U0001FA75!'
    close_win(page)

    # 11) Zweizeilig Mini: Schrift fest, Farbe pro Zeichen und Zeile
    page.locator('#scrollLayout').select_option('1'); page.fill('#scrollText','RAVESTICK'); page.fill('#scrollLine2','BAR OPEN')
    open_win(page)
    assert page.locator('#rbzGrids .rbz-line-label').all_text_contents()==['Zeile 1','Zeile 2']
    tiles(page,'b').nth(4).click(); tiles(page,'b').nth(5).click(); assert page.locator('#rbzChips').is_hidden()
    assert 'mini zweizeilig' in page.locator('#rbzFontLabel').inner_text().lower()
    swatch(page,'Cyan').click(); assert st(page,'b')['c'][4:6]==[3,3] and st(page,'a')['c']==[0]*9
    shot(page,'06_fenster_zweizeilig.png')
    close_win(page)
    page.locator('#sendScroll').click(); page.wait_for_timeout(100)
    req=[r for r in page.evaluate('__mock.requests') if r['path']=='/api/text'][-1]['data']
    assert req['text']=='RAVESTICK' and req['line2']=='BAR '+M(3)+'OP'+chr(0x90)+'EN' and req['layout']=='1',req
    # Schriftmarken werden in Zweizeilig nie geschrieben
    page.evaluate("RBS.a.f.fill(2)"); assert F(2) not in page.evaluate("rbEncode('a')") and chr(0x9c) not in page.evaluate("rbEncode('a')")
    page.evaluate("RBS.a.f.fill(-1)")
    # 12) Entwurf übersteht Neuladen (Felder werden nur übernommen, wenn die Länge zum Text passt)
    page.locator('#scrollLayout').select_option('3'); page.fill('#scrollText','Hallo Welt')
    page.evaluate("RBS.a.c.fill(0);RBS.a.f.fill(-1);RBS.a.c[0]=4;RBS.a.c[1]=4;RBS.a.f[2]=2;rbSaveDraftStyle()")
    draft=page.evaluate("localStorage.getItem('rb440-style')"); assert '"c":[4,4,0' in draft,draft
    page.evaluate("RBS.a.c.fill(0);RBS.a.f.fill(-1);rbRestoreDraft()")
    s=st(page); assert s['c'][:3]==[4,4,0] and s['f'][2]==2 and len(s['c'])==10,s
    page.evaluate("document.getElementById('scrollText').value='Kurz';rbRestoreDraft()")
    s=st(page); assert s['c']==[0]*4 and s['f']==[-1]*4,s            # Länge passt nicht: sauber zurückgesetzt
    page.evaluate("localStorage.setItem('rb440-style','{kaputt');document.getElementById('scrollText').value='Hallo Welt';rbRestoreDraft()")
    assert st(page)['c']==[0]*10
    page.fill('#scrollText','Hallo Welt')
    # 13) Hauptansicht: Vorschau und Kapazität berücksichtigen Schriftwechsel
    w0=page.evaluate("rbLineWidth('a')")
    page.evaluate("RBS.a.f[0]=3;RBS.a.f[1]=3"); w1=page.evaluate("rbLineWidth('a')")
    assert w1<w0,(w0,w1)           # Mini ist schmaler als Original Normal
    assert page.evaluate("scrollCapacity().cols")==64
    shot(page,'07_editor_haupt.png')
    # 14) Steuerzeichen im Eingabefeld werden entfernt
    page.fill('#scrollText','A\u0091B'); assert page.input_value('#scrollText')=='AB'
    page.fill('#scrollText','A\nB'); assert page.input_value('#scrollText')=='A B','Zeilenumbrüche werden zu Leerzeichen'
    # 15) Länge Bibliothek
    page.fill('#scrollText','x'*1700); n=page.evaluate("__mock.entries.length"); page.evaluate("v43SaveScrollNew()"); page.wait_for_timeout(300)
    assert page.evaluate("__mock.entries.length")==n,'zu langer Text darf nicht gespeichert werden'
    assert 'zu lang für die Bibliothek' in page.locator('#status').inner_text()
    assert page.locator('.v42-dialog:not(.hidden)').count()==0
    # Toast liegt bei offenem Fenster über der Fußleiste
    page.fill('#scrollText','Hi'); open_win(page); page.evaluate("notify('Test-Meldung')")
    tb=page.locator('#status').bounding_box(); fb=page.locator('.rbz-foot').bounding_box(); assert tb['y']+tb['height']<=fb['y']+2,(tb,fb)
    close_win(page)
    # 16) Eingabefeld (nur Lauftext): wächst mit, Enter schließt statt umzubrechen, Griff zieht größer
    page.fill('#scrollText',''); page.evaluate("rbGrowAll()")
    assert page.evaluate("document.getElementById('scrollText').tagName")=='TEXTAREA'
    assert page.evaluate("document.getElementById('line1').tagName")=='INPUT','Blockseiten behalten ihr Feld'
    h1=page.evaluate("document.getElementById('scrollText').getBoundingClientRect().height")
    page.fill('#scrollText','Das ist ein deutlich längerer Lauftext, der im schmalen Feld über mehrere Zeilen umbrechen muss, damit man alles sieht. '*3)
    h2=page.evaluate("document.getElementById('scrollText').getBoundingClientRect().height")
    assert h2>h1+60,(h1,h2)
    assert page.evaluate("getComputedStyle(document.getElementById('scrollText')).fontSize")=='16px'    # kein iOS-Zoom
    page.fill('#scrollText','Kurz'); h3=page.evaluate("document.getElementById('scrollText').getBoundingClientRect().height"); assert abs(h3-h1)<2,(h1,h3)
    page.locator('#scrollText').focus(); page.keyboard.press('End'); page.keyboard.press('Enter')
    assert page.input_value('#scrollText')=='Kurz' and page.evaluate("document.activeElement.id")!='scrollText'
    g=page.locator('#scrollText').locator('xpath=..').locator('.rb-grip').bounding_box()
    page.mouse.move(g['x']+g['width']/2,g['y']+g['height']/2); page.mouse.down()
    page.mouse.move(g['x']+g['width']/2,g['y']+g['height']/2+130,steps=10); page.mouse.up()
    h4=page.evaluate("document.getElementById('scrollText').getBoundingClientRect().height")
    assert h4>=h1+120,(h1,h4)
    assert int(page.evaluate("localStorage.getItem('rb441-fieldh-scrollText')"))>=h1+120
    page.fill('#scrollText','Kurz'); assert page.evaluate("document.getElementById('scrollText').getBoundingClientRect().height")>=h1+120   # gezogene Höhe bleibt
    page.locator('#scrollText').locator('xpath=..').locator('.rb-grip').dblclick()                 # Doppeltippen: zurück auf automatisch
    assert abs(page.evaluate("document.getElementById('scrollText').getBoundingClientRect().height")-h1)<2
    # zweizeilig: Zeile 2 hat dasselbe
    page.locator('#scrollLayout').select_option('1'); assert page.evaluate("document.getElementById('scrollLine2').tagName")=='TEXTAREA'
    page.fill('#scrollLine2','Zeile zwei mit sehr vielen Wörtern, damit das Feld in der Höhe wachsen muss und nicht abschneidet. '*2)
    assert page.evaluate("document.getElementById('scrollLine2').getBoundingClientRect().height")>h1+40
    page.fill('#scrollLine2',''); page.locator('#scrollLayout').select_option('3')
    shot(page,'08_editor_feld.png')

    # 17) Rückgängig / Wiederherstellen (mehrstufig, nur Icons neben „Text leeren“)
    page.evaluate("navigate('editor')")
    for sel in ('#rbUndo','#rbRedo'):
        assert page.evaluate(f"document.querySelector('{sel}').textContent.trim()")=='','nur Icon, keine Schrift'
    page.on('dialog',lambda d:d.accept())
    page.fill('#scrollText',''); page.evaluate("rbHReset()")
    assert page.locator('#rbUndo').is_disabled() and page.locator('#rbRedo').is_disabled()
    page.fill('#scrollText','eins'); page.wait_for_timeout(1100)
    page.fill('#scrollText','eins zwei'); page.wait_for_timeout(1100)
    page.fill('#scrollText','eins zwei drei'); page.wait_for_timeout(1100)
    assert page.locator('#rbUndo').is_enabled() and page.locator('#rbRedo').is_disabled()
    page.locator('#rbUndo').click(); assert page.input_value('#scrollText')=='eins zwei'
    page.locator('#rbUndo').click(); assert page.input_value('#scrollText')=='eins'
    assert page.locator('#rbRedo').is_enabled()
    page.locator('#rbRedo').click(); assert page.input_value('#scrollText')=='eins zwei'
    page.locator('#rbRedo').click(); assert page.input_value('#scrollText')=='eins zwei drei'
    assert page.locator('#rbRedo').is_disabled()
    # schnelles Tippen = ein Schritt
    page.locator('#scrollText').focus(); page.keyboard.press('End'); page.keyboard.type(' vier fünf'); page.wait_for_timeout(1100)
    page.locator('#rbUndo').click(); assert page.input_value('#scrollText')=='eins zwei drei',page.input_value('#scrollText')
    # Neue Änderung nach Rückgängig verwirft die Wiederherstellen-Kette
    page.fill('#scrollText','eins zwei drei X'); page.wait_for_timeout(1100); assert page.locator('#rbRedo').is_disabled()
    # Stile gehören zum Schritt; „Text leeren“ ist rückgängig machbar und stellt Stile wieder her
    page.fill('#scrollText','Hallo'); page.wait_for_timeout(1100)
    open_win(page); tiles(page).nth(0).click(); swatch(page,'Rot').click(); close_win(page)
    assert st(page)['c'][0]==4
    page.locator('#v42ClearText').click(); assert page.input_value('#scrollText')==''
    page.locator('#rbUndo').click(); assert page.input_value('#scrollText')=='Hallo' and st(page)['c'][0]==4,st(page)
    page.locator('#rbUndo').click(); assert st(page)['c'][0]==0 and page.input_value('#scrollText')=='Hallo'    # Farbe zurück
    page.locator('#rbRedo').click(); page.locator('#rbRedo').click(); assert page.input_value('#scrollText')=='' and page.locator('#rbRedo').is_disabled()
    # Bibliothek öffnen setzt die Historie zurück
    page.evaluate("libraryOpenEditor(1)"); page.wait_for_function("document.getElementById('scrollText').value==='Hi Raver, Party!'")
    assert page.locator('#rbUndo').is_disabled()
    shot(page,'09_editor_undo.png')

    # 18) Programme: Sortieren erscheint sofort (der Mock-ESP antwortet absichtlich erst nach 700 ms)
    page.evaluate("__mock.moveDelay=700;__mock.programs=[{id:3,name:'Alpha',count:2,missing:0,seconds:20,playback:0,ids:[1,2]},{id:5,name:'Beta',count:1,missing:0,seconds:20,playback:1,ids:[1]},{id:7,name:'Gamma',count:3,missing:0,seconds:20,playback:0,ids:[1,2,1]}]")
    page.evaluate("rbFresh.prog=0;navigate('programs')"); page.wait_for_function("document.querySelectorAll('#v42Programs .library-row').length===3")
    pnames=lambda: page.evaluate("[...document.querySelectorAll('#v42Programs .lib-name')].map(x=>x.textContent)")
    cnt=lambda path: len([r for r in page.evaluate('__mock.requests') if r['path']==path])
    rows=page.locator('#v42Programs .library-row')
    assert rows.nth(0).locator('button[title="Nach oben"]').is_disabled() and rows.nth(2).locator('button[title="Nach unten"]').is_disabled()
    assert rows.nth(1).locator('button[title="Nach oben"]').is_enabled() and rows.nth(1).locator('button[title="Nach unten"]').is_enabled()
    lists0=cnt('/api/collections')
    rows.nth(1).locator('button[title="Nach unten"]').click()
    assert pnames()==['Alpha','Gamma','Beta'],pnames()                       # sofort, ohne die Antwort abzuwarten
    page.locator('#v42Programs .library-row').nth(2).locator('button[title="Nach oben"]').click()
    assert pnames()==['Alpha','Beta','Gamma'],pnames()                       # zweiter Tipp ebenfalls sofort
    assert page.evaluate('rbMovePending')>=1                                 # der ESP ist noch nicht fertig
    page.wait_for_function('rbMovePending===0',timeout=8000)
    mvs=[r['data'] for r in page.evaluate('__mock.requests') if r['path']=='/api/collections/move']
    assert mvs==[{'id':'5','to':'2'},{'id':'5','to':'1'}],mvs               # der Reihe nach, in dieser Reihenfolge
    page.wait_for_timeout(300); assert cnt('/api/collections')==lists0,'nach erfolgreichem Sortieren keine Neuabfrage'
    assert pnames()==['Alpha','Beta','Gamma']
    shot(page,'10_programme_sortieren.png')
    # Fehlerfall: Anzeige springt nach dem Fehler auf den Stand des Controllers zurück
    page.evaluate("__mock.moveFail=true;__mock.moveDelay=50")
    page.locator('#v42Programs .library-row').nth(0).locator('button[title="Nach unten"]').click()
    assert pnames()==['Beta','Alpha','Gamma'],pnames()
    page.wait_for_function("[...document.querySelectorAll('#v42Programs .lib-name')].map(x=>x.textContent).join()==='Alpha,Beta,Gamma'",timeout=8000)
    page.evaluate("__mock.moveFail=false")

    # 18b) Bibliothek: ebenfalls sofort; Wechsel der Ansicht holt die Listen nicht bei jedem Mal neu
    page.evaluate("__mock.moveDelay=500;__mock.entries=[{id:11,name:'Erster',kind:'text',mode:2},{id:12,name:'Zweiter',kind:'text',mode:2},{id:13,name:'Dritter',kind:'text',mode:2}]")
    page.evaluate("rbFresh.lib=0;navigate('library')"); page.wait_for_function("document.querySelectorAll('#libList .library-row').length===3")
    lnames=lambda: page.evaluate("[...document.querySelectorAll('#libList .lib-name')].map(x=>x.textContent)")
    page.locator('#libList .library-row').nth(0).locator('button[title="Nach unten"]').click()
    assert lnames()==['Zweiter','Erster','Dritter'],lnames()
    page.wait_for_function('rbMovePending===0',timeout=8000)
    assert [r['data'] for r in page.evaluate('__mock.requests') if r['path']=='/api/library/move'][-1]=={'id':'11','to':'1'}
    n_lib=cnt('/api/library'); n_prog=cnt('/api/collections')
    page.evaluate("navigate('programs');navigate('library');navigate('programs');navigate('library')"); page.wait_for_timeout(300)
    assert cnt('/api/library')==n_lib and cnt('/api/collections')==n_prog,'frische Listen werden nicht erneut geholt'
    assert page.evaluate("document.querySelectorAll('#libList .library-row').length")==3     # sofort aus dem Speicher da
    page.evaluate("rbFresh.lib=0;rbFresh.prog=0;navigate('programs');navigate('library')"); page.wait_for_timeout(500)
    assert cnt('/api/library')==n_lib+1 and cnt('/api/collections')==n_prog+1,'veraltete Listen werden im Hintergrund aufgefrischt'
    # Abfrage des Status pausiert, solange der ESP mit Listen/Speichern beschäftigt ist
    n_st=cnt('/api/status')
    page.evaluate("rbBusy=1;rbBusySince=Date.now();poll()"); page.wait_for_timeout(150); assert cnt('/api/status')==n_st
    page.evaluate("rbBusy=0;poll()"); page.wait_for_timeout(300); assert n_st+1<=cnt('/api/status')<=n_st+2   # der Takt kann dazwischen einmal feuern
    page.evaluate("__mock.moveDelay=0")

    # 19) Startbildschirm: nur an, aus | Linkslauf, Standbild (Blocktext) | Bearbeiten; alle anderen Seiten behalten die volle Steuerung
    page.evaluate("navigate('home')")
    seen=page.evaluate("[...document.querySelectorAll('#rbDock .rb-dock-actions button')].filter(b=>{const r=b.getBoundingClientRect();return r.width>0&&r.height>0}).map(b=>b.id||b.dataset.mode)")
    assert seen==['homePowerOn','homePowerOff','scroll','static','editShortcuts'],seen
    rects=page.evaluate("[...document.querySelectorAll('#rbDock .rb-dock-actions button')].filter(b=>b.getBoundingClientRect().width>0).map(b=>{const r=b.getBoundingClientRect();return [r.left,r.right,r.top]})")
    assert len({round(r[2]) for r in rects})==1,'eine Zeile'
    g=[rects[i+1][0]-rects[i][1] for i in range(4)]       # Abstände zwischen den fünf Tasten
    assert max(g)-min(g)<=1.5 and max(g)<=8,g                # 0.4.5.0: fuenf Tasten gleichmaessig nebeneinander, keine Gruppenluecken
    for view in ('editor','library','programs'):
        page.evaluate(f"navigate('{view}')")
        n=page.evaluate("[...document.querySelectorAll('#rbDock .rb-dock-modes button')].filter(b=>b.getBoundingClientRect().width>0).length")
        assert n==8,(view,n)
    page.evaluate("navigate('home')"); assert page.locator('#editShortcuts').is_visible()
    page.evaluate("navigate('ambience')")
    assert not page.evaluate("(()=>{const e=document.querySelector('#rbDock .rb-dock-modes');return getComputedStyle(e).display!=='none'})()"),'Ambiente behält sein eigenes Bedienfeld'
    page.evaluate("navigate('home')"); shot(page,'11_start.png')

    # ================= v0.4.5.0 =================
    # 20) Auswahl per zweitem Tipp aufheben (Programme, Bibliothek)
    page.evaluate("navigate('programs')"); page.wait_for_function("document.querySelectorAll('#v42Programs .library-row').length===3")
    pick=lambda sel,n: page.locator(f'{sel} .library-row').nth(n).locator('.lib-pick')
    pick('#v42Programs',1).click(); page.wait_for_function('v42ProgramId===5')
    assert 'selected' in page.locator('#v42Programs .library-row').nth(1).get_attribute('class')
    pick('#v42Programs',1).click(); assert page.evaluate('v42ProgramId')==0 and page.locator('#v42Programs .library-row.selected').count()==0
    assert page.locator('#v43ProgEdit').is_disabled()
    page.evaluate("navigate('library')"); page.wait_for_function("document.querySelectorAll('#libList .library-row').length===3")
    assert page.evaluate('librarySelected')!=0 and page.locator('#libOpen').is_enabled()      # Vorbelegung wie bisher
    sid=page.evaluate('librarySelected'); idx=page.evaluate("libraryItems.findIndex(x=>x.id===librarySelected)")
    pick('#libList',idx).click(); assert page.evaluate('librarySelected')==0 and page.locator('#libOpen').is_disabled()
    page.evaluate("rbFresh.lib=0;refreshLibrary()"); page.wait_for_timeout(400); assert page.evaluate('librarySelected')==0,'bewusst aufgehoben bleibt aufgehoben'
    pick('#libList',0).click(); assert page.evaluate('librarySelected')!=0 and page.locator('#libOpen').is_enabled()

    # 21) Kurzwahl-Dialog: Quellen als Schaltflächen, Beschriftung vorbelegt
    page.evaluate("navigate('home');v42OpenShortcut(1)"); page.wait_for_function("!document.getElementById('v42ShortcutDialog').classList.contains('hidden')")
    srcs=page.locator('#v42Sources button'); assert srcs.count()==4
    assert [x.inner_text().strip() for x in srcs.all()]==['Bibliothek','Programme','Ambiente','Animationen']
    assert srcs.nth(3).is_disabled() and all(srcs.nth(i).get_attribute('aria-pressed')=='false' for i in range(3))
    assert page.locator('#v42ShortcutTargetWrap').is_hidden() and page.locator('#v42ShortcutSave').is_disabled()
    assert page.evaluate("[...document.querySelectorAll('#v42Sources button')].every(b=>b.querySelector('svg'))"),'Icons aus der Menüführung'
    srcs.nth(0).click(); assert srcs.nth(0).get_attribute('aria-pressed')=='true' and page.locator('#v42ShortcutTargetWrap').is_visible()
    assert page.locator('#v42ShortcutSave').is_disabled()                                          # erst Inhalt wählen
    page.select_option('#v42ShortcutTarget','12'); assert page.input_value('#v42ShortcutLabel')=='Zweiter' and page.locator('#v42ShortcutSave').is_enabled()
    page.locator('#v42ShortcutSave').click(); page.wait_for_function('__mock.requests.some(r=>r.path==="/api/shortcuts/set")')
    d=[r for r in page.evaluate('__mock.requests') if r['path']=='/api/shortcuts/set'][-1]['data']
    assert d=={'slot':'1','type':'1','id':'12','label':''},d                                         # unverändert vorbelegt = Name bleibt dynamisch
    page.evaluate("v42OpenShortcut(2)"); page.wait_for_timeout(200); srcs.nth(1).click(); page.select_option('#v42ShortcutTarget','5')
    page.fill('#v42ShortcutLabel','Mein Name'); page.locator('#v42ShortcutSave').click(); page.wait_for_timeout(200)
    d=[r for r in page.evaluate('__mock.requests') if r['path']=='/api/shortcuts/set'][-1]['data']; assert d['label']=='Mein Name' and d['type']=='2'
    shot(page,'12_kurzwahl_dialog.png')

    # 22) Einstellungen/Diagnose: Knöpfe in der Designlinie, Icons, keine Überschriften-Zeichen
    fw=lambda sel: page.evaluate(f"(()=>{{const e=document.querySelector('{sel}'),c=getComputedStyle(e);return [c.fontFamily,c.fontWeight,c.fontSize,!!e.querySelector('svg.rb-lucide')]}})()")
    a,b=fw('#libExport'),fw('label[for=libImport]'); assert a[:2]==b[:2] and b[3] and a[3],(a,b)
    assert page.evaluate("document.querySelector('label[for=libImport]').dataset.rbIcon")=='database-arrow-up' and 'Backup zusammenführen'==page.locator('label[for=libImport]').inner_text().strip()
    d1,d2,ref=fw('a[href="/api/diagnostics.txt"]'),fw('a[href="/api/ram.txt"]'),fw('#diagReload')
    assert d1==d2 and d1[:3]==ref[:3] and d1[3],(d1,d2,ref)
    assert page.evaluate("document.querySelector('a[href=\"/api/ram.txt\"]').dataset.rbIcon")=='database-arrow-down'
    heads=page.evaluate("[...document.querySelectorAll('#view-settings .settings-section > .label')].map(x=>x.textContent.trim())")
    assert heads==['Bibliothek & Datensicherung','LED-Tafel','Animationstest'],heads

    # 23) Erweiterter Editor: feste Kopfzone, waagerecht markiert / senkrecht scrollt (echte Touch-Ereignisse), Doppeltipp, Farbleiste
    long_text=('Hallo, Welt! Das ist ein langer Test mit vielen Wörtern. '*5).strip()
    page.evaluate("navigate('editor')"); page.fill('#scrollText',long_text); page.wait_for_timeout(100)
    open_win(page)
    cdp=ctx.new_cdp_session(page)
    def touch(path):
        cdp.send('Input.dispatchTouchEvent',{'type':'touchStart','touchPoints':[{'x':path[0][0],'y':path[0][1]}]})
        for x,y in path[1:]: cdp.send('Input.dispatchTouchEvent',{'type':'touchMove','touchPoints':[{'x':x,'y':y}]})
        cdp.send('Input.dispatchTouchEvent',{'type':'touchEnd','touchPoints':[]})
        page.wait_for_timeout(250)
    ctr=lambda i: (lambda b:(b['x']+b['width']/2,b['y']+b['height']/2))(tiles(page).nth(i).bounding_box())
    sc=lambda: page.evaluate("document.getElementById('rbzScroll').scrollTop")
    top0=page.locator('#rbzTop').bounding_box(); assert page.locator('#rbzTop').evaluate("e=>!e.classList.contains('compact')")
    x1,y1=ctr(1); x4,_=ctr(4)
    touch([(x1,y1)]+[(x1+(x4-x1)*k/6,y1) for k in range(1,7)])
    assert page.evaluate("[...rbz.sel.a].sort((a,b)=>a-b)")==[1,2,3,4] and sc()==0,(page.evaluate("[...rbz.sel.a]"),sc())    # waagerecht: markiert, scrollt nicht
    touch([(x1,y1)]+[(x1+(x4-x1)*k/6,y1) for k in range(1,7)])                                      # auf Markiertem: entmarkiert
    assert page.evaluate("rbz.sel.a.size")==0,page.evaluate("[...rbz.sel.a]")
    page.locator('#rbzNone').click()
    x2,y2=ctr(2)
    touch([(x2,y2)]+[(x2,y2-k*30) for k in range(1,8)])                                             # senkrecht: scrollt
    assert sc()>60 and page.evaluate("rbz.sel.a.size")==0,(sc(),page.evaluate("rbz.sel.a.size"))
    top1=page.locator('#rbzTop').bounding_box(); assert abs(top1['y']-top0['y'])<1,'Vorschau bleibt oben stehen'
    assert page.locator('#rbzMatrix').is_visible()
    page.wait_for_timeout(300); mw=page.locator('#rbzMatrix').bounding_box()['width']; assert mw<0.7*(top0['width']-30),(mw,top0['width'])   # verkleinert
    shot(page,'13_fenster_gescrollt.png')
    page.evaluate("document.getElementById('rbzScroll').scrollTop=0"); page.wait_for_timeout(300)
    assert page.locator('#rbzTop').evaluate("e=>!e.classList.contains('compact')") and page.locator('#rbzMatrix').bounding_box()['width']>0.9*(top0['width']-30)
    # Doppeltipp markiert das Wort (ohne Satzzeichen am Rand), nochmal entmarkiert es
    page.locator('#rbzNone').click(); page.wait_for_timeout(450)
    tiles(page).nth(2).dblclick(); assert page.evaluate("[...rbz.sel.a].sort((a,b)=>a-b)")==[0,1,2,3,4],page.evaluate("[...rbz.sel.a]")
    page.wait_for_timeout(450); tiles(page).nth(2).dblclick(); assert page.evaluate("rbz.sel.a.size")==0
    page.wait_for_timeout(450); tiles(page).nth(8).dblclick(); assert page.evaluate("[...rbz.sel.a].sort((a,b)=>a-b)")==[7,8,9,10],page.evaluate("[...rbz.sel.a]")   # „Welt“ ohne „!“
    page.wait_for_timeout(450); page.locator('#rbzNone').click()
    # Farbleiste: erscheint, solange der große Farbblock nicht sichtbar ist
    assert page.evaluate("document.getElementById('rbzColorBar').classList.contains('show')"),'lange Texte: Leiste unten sichtbar'
    tiles(page).nth(0).click(); swatch(page,'Rot','#rbzBarColors').click(); assert st(page)['c'][0]==4
    assert swatch(page,'Rot','#rbzBarColors').evaluate("e=>e.classList.contains('on')")
    page.evaluate("document.getElementById('rbzScroll').scrollTop=999999"); page.wait_for_timeout(400)
    assert not page.evaluate("document.getElementById('rbzColorBar').classList.contains('show')") and page.locator('#rbzColorBlock').is_visible(),'unten: großer Block statt Leiste'
    big=swatch(page,'Rot','#rbzColors').bounding_box(); small=swatch(page,'Rot','#rbzBarColors').evaluate("e=>e.getBoundingClientRect().width")
    assert big['width']>small+8,(big['width'],small)
    assert page.locator('#rbzFontBox').is_visible()
    page.evaluate("document.getElementById('rbzScroll').scrollTop=0"); page.wait_for_timeout(400)
    assert page.evaluate("document.getElementById('rbzColorBar').classList.contains('show')")
    page.locator('#rbzFlowHead').click(); assert not page.evaluate("document.getElementById('rbzColorBar').classList.contains('show')"),'im Farbwechsel keine Leiste'
    page.locator('#rbzFlowHead').click()
    # kurzer Text: kein Scrollen, keine Leiste
    close_win(page); page.fill('#scrollText','Kurz'); open_win(page); page.wait_for_timeout(300)
    assert not page.evaluate("document.getElementById('rbzColorBar').classList.contains('show')")
    close_win(page)

    # 24) Vorschau im Editor schrumpft beim Scrollen (Lauftext-Werkzeuge nur ganz oben)
    page.fill('#scrollText',long_text); page.evaluate("navigate('editor');document.body.style.paddingBottom='1400px'"); page.wait_for_timeout(200)
    pv=page.locator('#view-editor .preview-sticky'); w0=page.locator('#matrix').bounding_box()['width']
    assert page.locator('#previewTools').is_visible()
    page.evaluate("window.scrollTo(0,200)"); page.wait_for_timeout(400)
    assert 'rb-compact' in pv.get_attribute('class') and page.locator('#previewTools').is_hidden() and page.locator('#matrix').bounding_box()['width']<0.7*w0
    page.evaluate("window.scrollTo(0,0)"); page.wait_for_timeout(400)
    assert 'rb-compact' not in pv.get_attribute('class') and page.locator('#previewTools').is_visible()
    page.evaluate("document.body.style.paddingBottom=''")
    # ================= v0.4.5.1: Bluetooth-Zeichen im Kopf =================
    def ble(status):
        page.evaluate("s=>{window.__mock.status=s;rbPollPending=false;rbBusy=0;poll()}",status); page.wait_for_timeout(250)
        el=page.locator('#rbBle'); return el.get_attribute('data-rb-icon'),el.get_attribute('data-state'),el.get_attribute('title'),el.locator('svg.rb-lucide').count()
    assert ble({})[0:2]==('bluetooth','idle')
    assert ble({'bleLast':1})[0:2]==('bluetooth','ok')
    assert ble({'bleLast':2})[0:2]==('bluetooth-off','bad')
    assert 'nicht erreicht' in ble({'bleLast':2})[2]
    assert ble({'bleLast':2,'bleLink':True})[0:2]==('bluetooth-connected','ok')
    assert ble({'bleLast':2,'state':'sending'})[0:2]==('bluetooth-searching','busy')
    r=ble({'bleLast':1}); assert r[0:2]==('bluetooth','ok') and r[3]==1       # genau ein Symbol, kein Doppeln beim Wechsel
    assert page.locator('#rbBle').bounding_box()['width']<24
    page.evaluate("window.__mock.status=null;rbPollPending=false;poll()")

    # ================= v0.4.6.0: Zeichen-Editor für Blockseiten =================
    page.evaluate("window.__mock.status=null;rbPollPending=false;poll()")
    page.evaluate("navigate('editor');setTab('pages')"); page.wait_for_timeout(300)
    assert page.evaluate("pages.length")>=1 and page.locator('#rbzOpenPage').count()==1,'Palettenknopf der Seite fehlt'
    page.evaluate("pages[selected].layout=3;pages[selected].align=1;pages[selected].color1=7;renderSelected()")
    page.fill('#line1','Hi Party')
    assert page.evaluate("pages[selected].line1")=='Hi Party' and page.evaluate("RBS.a.chars.length")==8
    page.locator('#rbzOpenPage').click(); page.wait_for_function('!document.getElementById("rbz").classList.contains("hidden")')
    for i in range(3,8): tiles(page).nth(i).click()
    swatch(page,'Cyan').click()
    p1=page.evaluate("pages[selected].line1")
    assert re.search(r'[\u0090-\u009c]',p1) and page.evaluate("rbPlain(pages[selected].line1)")=='Hi Party',repr(p1)
    assert st(page)['c']==[0]*3+[3]*5,st(page)['c']
    assert page.evaluate("validatePage(pages[selected])")=='',page.evaluate("validatePage(pages[selected])")
    # Rückgängig/Wiederherstellen gelten im Fenster auch für die Seite
    page.locator('#rbzUndo').click(); assert st(page)['c']==[0]*8 and not re.search(r'[\u0090-\u009c]',page.evaluate("pages[selected].line1"))
    page.locator('#rbzRedo').click(); assert st(page)['c']==[0]*3+[3]*5 and re.search(r'[\u0090-\u009c]',page.evaluate("pages[selected].line1"))
    # Schrift pro Zeichen
    assert page.locator('#rbzFontBox').is_visible()
    close_win(page) if page.locator('#rbz.hidden').count()==0 else None
    # Der Lauftext bleibt unberührt (eigener Zustand und eigener Verlauf)
    assert page.evaluate("RBSX.scroll.a.c.length")==page.evaluate("codepoints(document.getElementById('scrollText').value).length")
    assert page.evaluate("document.getElementById('scrollText').value")!='Hi Party'
    # Seitenwechsel lädt den Stil zurück: Speichern/Laden-Rundlauf
    page.evaluate("renderSelected()")
    assert page.evaluate("document.getElementById('line1').value")=='Hi Party' and st(page)['c']==[0]*3+[3]*5,st(page)['c']
    # Zeilenfarbe setzt Einzelfarben zurück
    page.evaluate("rbPageBaseColor('a',2)")
    assert st(page)['c']==[0]*8 and page.evaluate("pages[selected].color1")==2 and not re.search(r'[\u0090-\u009c]',page.evaluate("pages[selected].line1").replace(chr(0x92),'',1)) or page.evaluate("pages[selected].line1").count(chr(0x92))<=1
    # Prüfung: Marken kosten Bytes und werden nicht als Breite gezählt
    msg=page.evaluate("validatePage({layout:2,align:1,color1:7,color2:7,line1:('\\u0091A').repeat(16),line2:''})")
    assert '47' in msg or 'maximal' in msg or 'breit' in msg,msg
    ok=page.evaluate("validatePage({layout:3,align:1,color1:7,color2:7,line1:'\\u0093Hi',line2:''})")
    assert ok=='',ok
    wd=page.evaluate("[rbPageLinePx('Hi',3),rbPageLinePx('\\u0093Hi',3)]"); assert wd[0]==wd[1],wd
    # Name-Vorschlag und Seitenleiste ohne Marken
    assert page.evaluate("rbPlain('\\u0093Hi')")=='Hi'
    # ================= v0.4.6.0: Bluetooth-Zeichen während der Übertragung =================
    page.evaluate("navigate('editor');setTab('scroll')")
    def bleicon(): return page.locator('#rbBle').get_attribute('data-rb-icon')
    page.evaluate("window.__mock.force202=true;window.__mock.status={state:'sending',bleLast:1}")
    page.fill('#scrollText','Test'); page.locator('#sendScroll').click(); page.wait_for_timeout(700)
    assert bleicon()=='bluetooth-searching',bleicon()          # zuerst: sucht/verbindet
    page.wait_for_timeout(3300)
    assert bleicon()=='bluetooth-connected',bleicon()          # ESP schweigt > 3,5 s: Verbindung steht, Übertragung läuft
    assert page.locator('#rbBle').get_attribute('title')=='LED-Tafel: Übertragung läuft'
    page.evaluate("window.__mock.status={state:'idle',bleLast:1};window.__mock.force202=false;rbPollPending=false;rbBusy=0;poll()"); page.wait_for_timeout(500)
    assert bleicon()=='bluetooth',bleicon()                     # erste Antwort räumt auf
    assert page.evaluate("rbBleJobAt")==0
    # Fehlerfall: Auftrag nicht erreicht -> bluetooth-off, nie „verbunden“
    page.evaluate("window.__mock.force202=true;window.__mock.status={state:'sending'}")
    page.locator('#sendScroll').click(); page.wait_for_timeout(4200)
    assert bleicon()=='bluetooth-connected'
    page.evaluate("window.__mock.status={state:'error',bleLast:2};window.__mock.force202=false;rbPollPending=false;rbBusy=0;poll()"); page.wait_for_timeout(500)
    assert bleicon()=='bluetooth-off',bleicon()
    page.evaluate("window.__mock.status=null;rbPollPending=false;poll()")

    # ================= v0.5.0.0: Oberflächen-Korrekturen und Tafelverwaltung =================
    page.evaluate("window.__mock.status=null;window.__mock.force202=false;rbPollPending=false;poll()")
    # Zoom gesperrt
    vp=page.evaluate("document.querySelector('meta[name=viewport]').content"); assert 'user-scalable=no' in vp and 'maximum-scale=1' in vp,vp
    for sel in ['body','#sendScroll','#rbSignScan','#scrollText']:
        ta=page.evaluate(f"getComputedStyle(document.querySelector('{sel}')).touchAction"); assert ta=='manipulation',(sel,ta)
    assert page.evaluate("getComputedStyle(document.querySelector('.rbz-tiles')||document.body).touchAction") in ('pan-y','manipulation')
    assert page.evaluate("(()=>{const e=new Event('gesturestart',{cancelable:true});document.dispatchEvent(e);return e.defaultPrevented;})()"),'Spreizgeste wird nicht unterdrückt'
    for sel in ['#scrollText','#scrollLine2','#line1','#line2','#libSearch']:
        if page.locator(sel).count():
            fs=page.evaluate(f"parseFloat(getComputedStyle(document.querySelector('{sel}')).fontSize)"); assert fs>=16,(sel,fs)
    # Palettentaste der Blockseite in derselben Zeile wie die Farben
    page.evaluate("navigate('editor');setTab('pages')"); page.wait_for_timeout(300)
    ty=page.evaluate("document.getElementById('rbzOpenPage').getBoundingClientRect().top"); sy=page.evaluate("document.querySelector('#pageColors1 .sw, #pageColors1 button').getBoundingClientRect().top")
    sw0=page.evaluate("document.querySelector('#pageColors1').children[0].getBoundingClientRect().top")
    assert abs(ty-sw0)<3,(ty,sw0)
    # Programme: Liste und Bedienfeld ohne Scrollen sichtbar
    page.evaluate("window.__mock.programs=Array.from({length:9},(_,i)=>({id:i+1,name:'Programm '+(i+1),steps:[],mode:0}));window.__mock.entries=Array.from({length:12},(_,i)=>({id:100+i,name:'Text '+(i+1),kind:'text',mode:0,text:'x',line2:'',layout:3,color1:7,color2:7}));")
    page.evaluate("refreshLibrary&&refreshLibrary()"); page.wait_for_timeout(300)
    page.evaluate("v42RefreshPrograms&&v42RefreshPrograms()"); page.evaluate("navigate('programs')"); page.wait_for_timeout(500)
    sh=page.evaluate("[document.documentElement.scrollHeight,window.innerHeight]"); assert sh[0]<=sh[1]+2,('Programme scrollt',sh)
    dock=page.evaluate("document.getElementById('rbDock').getBoundingClientRect().top"); lst=page.evaluate("document.getElementById('v42Programs').getBoundingClientRect().bottom")
    assert lst<=dock+2,('Liste ragt unter das Bedienfeld',lst,dock)
    page.evaluate("navigate('library')"); page.wait_for_timeout(600)
    assert page.evaluate("document.querySelectorAll('#libList .entry, #libList [role=listitem], #libList > *').length")>=6,'Bibliothek-Mock leer'
    sh=page.evaluate("[document.documentElement.scrollHeight,window.innerHeight]"); assert sh[0]<=sh[1]+2,('Bibliothek scrollt',sh)
    dock=page.evaluate("document.getElementById('rbDock').getBoundingClientRect().top"); lst=page.evaluate("document.getElementById('libList').getBoundingClientRect().bottom")
    assert lst<=dock+2,('Bibliotheksliste ragt unter das Bedienfeld',lst,dock)
    # ---- Tafelverwaltung ----
    page.evaluate("""window.__mock.signItems=[
      {addr:'c0:ff:ee:00:00:01',name:'CoolLED1248',rssi:-78,cool:true,mine:false,active:false},
      {addr:'aa:bb:cc:dd:ee:01',name:'CoolLEDX-AB',rssi:-55,cool:true,mine:false,active:false},
      {addr:'11:22:33:44:55:66',name:'',rssi:-93,cool:false,mine:false,active:false}];window.__mock.sign=null;rbPollPending=false;poll()""")
    page.wait_for_timeout(400)
    assert page.locator('#rbSignChip').count()==0 and page.locator('.rb-sign-chip').count()==0,'Chip in der Kopfzeile entfällt'
    page.evaluate("navigate('settings')"); page.wait_for_timeout(400)
    assert 'Noch keine Tafel' in page.locator('#rbSignName').inner_text() and page.locator('#rbSignBack').is_hidden()
    page.locator('#rbSignScan').click(); page.wait_for_selector('#rbSignList .rb-sign-row'); 
    rows=page.locator('#rbSignList .rb-sign-row'); assert rows.count()==2,rows.count()      # ohne „alle“: nur Tafeln
    assert rows.nth(0).locator('.rb-sign-sig').get_attribute('data-rb-icon') in ('signal-medium','signal-high','signal')
    icons=[rows.nth(i).locator('.rb-sign-sig').get_attribute('data-rb-icon') for i in range(2)]; assert set(icons)=={'signal-medium','signal'},icons   # -78 dBm mittel, -55 dBm voll
    assert rows.nth(0).locator('svg.rb-lucide').count()==1
    page.locator('#rbSignAll').check(); page.locator('#rbSignScan').click(); page.wait_for_function("document.querySelectorAll('#rbSignList .rb-sign-row').length===3")
    ic=page.evaluate("[...document.querySelectorAll('#rbSignList .rb-sign-sig')].map(e=>e.dataset.rbIcon)"); assert 'signal-zero' in ic,ic         # -93 dBm: sehr schwach
    assert ic.count('signal-zero')==1
    page.locator('#rbSignAll').uncheck(); page.locator('#rbSignScan').click(); page.wait_for_function("document.querySelectorAll('#rbSignList .rb-sign-row').length===2")
    # speichern (erste Tafel: kein zweiter Tipp nötig)
    row=page.locator('#rbSignList .rb-sign-row').filter(has_text='CoolLED1248')
    row.locator('button[data-act=save]').click(); page.wait_for_function("document.getElementById('rbSignNow').dataset.kind==='1'")
    assert 'Meine Tafel' in page.locator('#rbSignSub').inner_text() and '00:01' in page.locator('#rbSignSub').inner_text()
    assert page.locator('#rbSignList .rb-sign-row').filter(has_text='CoolLED1248').locator('.rb-sign-tag.mine').count()==1
    assert page.locator('#rbSignList .rb-sign-row').filter(has_text='CoolLED1248').locator('button[data-act=save]').count()==0
    reqs=page.evaluate("window.__mock.requests.filter(r=>r.path.startsWith('/api/sign')).map(r=>r.path+':'+JSON.stringify(r.data))")
    assert any(r.startswith('/api/sign/save:') and 'c0:ff:ee:00:00:01' in r for r in reqs),reqs
    # fremde Tafel nutzen, Chip + „Zurück“ erscheinen
    other=page.locator('#rbSignList .rb-sign-row').filter(has_text='CoolLEDX-AB')
    other.locator('button[data-act=use]').click(); page.wait_for_function("document.getElementById('rbSignNow').dataset.kind==='2'")
    assert 'Fremde Tafel' in page.locator('#rbSignSub').inner_text() and page.locator('#rbSignBack').is_visible()
    assert 'bis zum Neustart' in page.locator('#rbSignSub').inner_text()
    # Ersetzen von „Meine Tafel“ braucht zwei Tipps
    other.locator('button[data-act=save]').click(); page.wait_for_timeout(200)
    assert 'Nochmal tippen' in other.locator('button[data-act=save]').inner_text()
    n0=page.evaluate("window.__mock.requests.filter(r=>r.path==='/api/sign/save').length"); assert n0==1,n0
    other.locator('button[data-act=save]').click(); page.wait_for_function("document.getElementById('rbSignNow').dataset.kind==='1'")
    assert page.evaluate("window.__mock.requests.filter(r=>r.path==='/api/sign/save').length")==2
    # nochmals fremd nutzen und ohne neue Suche zurück
    page.locator('#rbSignList .rb-sign-row').filter(has_text='CoolLED1248').locator('button[data-act=use]').click(); page.wait_for_function("document.getElementById('rbSignNow').dataset.kind==='2'")
    scans=page.evaluate("window.__mock.requests.filter(r=>r.path==='/api/sign/scan').length")
    page.locator('#rbSignBack').click(); page.wait_for_function("document.getElementById('rbSignNow').dataset.kind==='1'")
    assert page.evaluate("window.__mock.requests.filter(r=>r.path==='/api/sign/scan').length")==scans,'Zurück darf nicht neu suchen'
    assert page.locator('#rbSignBack').is_hidden()
    page.evaluate("window.__mock.status=null;rbPollPending=false;poll()")
    shot(page,'05_tafelverwaltung.png')
    # 0.6.0.0: Animationstest in den Einstellungen
    page.evaluate("navigate('settings')"); page.wait_for_timeout(200)
    assert page.locator('#rbAnimTestSection button[data-anim]').count()==4
    page.evaluate("window.__mock.requests.length=0")
    page.locator('#rbAnimTestSection button[data-anim=colors]').click(); page.wait_for_timeout(150)
    page.fill('#rbAnimMs','50'); page.select_option('#rbAnimMode','static')
    page.locator('#rbAnimTestSection button[data-anim=bar]').click(); page.wait_for_timeout(150)
    page.locator('#rbAnimTestSection button[data-anim=limit][data-n="80"]').click(); page.wait_for_timeout(150)
    page.select_option('#rbAnimMode','scroll'); page.fill('#rbAnimMs','')
    page.locator('#rbAnimTestSection button[data-anim=limit][data-n="79"]').click(); page.wait_for_timeout(150)
    ar=page.evaluate("window.__mock.requests.filter(r=>r.path==='/api/anim/test').map(r=>[r.method,r.data])")
    assert ar==[['POST',{'pattern':'colors','mode':'keep'}],['POST',{'pattern':'bar','mode':'static','n':'16','ms':'50'}],
                ['POST',{'pattern':'limit','mode':'static','n':'80','ms':'50'}],['POST',{'pattern':'limit','mode':'scroll','n':'79'}]],ar
    fs=page.evaluate("['rbAnimMs','rbAnimMode'].map(i=>parseFloat(getComputedStyle(document.getElementById(i)).fontSize))")
    assert min(fs)>=16,fs   # sonst zoomt iOS
    assert page.evaluate("document.documentElement.scrollWidth<=window.innerWidth"),'Testbereich laeuft seitlich ueber'
    shot(page,'06_animationstest.png')
    assert not errors,errors
    ctx.close(); browser.close()
print('PASS Browser 0.6.0.0 (Animationstest-Bereich: vier Tasten, Zeit/Modus, Anfragen) / 0.5.0.0 (Tafelverwaltung, Zoom-Sperre, Palettentaste, Programme ohne Scrollen) / 0.4.6.0 (Zeichen-Editor für Blockseiten, Bluetooth-Zeichen bei Übertragung) / 0.4.5.1 / 0.4.5.0: Einzeln/Wischen (Richtungserkennung per Touch, Doppeltipp-Wort, feste Kopfzone, Farbleiste), Auswahl aufheben, Kurzwahl-Dialog, Einstellungen/Diagnose-Knöpfe, schrumpfende Vorschau; früher: Entmarkieren, Farbe, Schriftkästchen, Farbwechsel, Herz-Tausch, mehrstufiges Rückgängig/Wiederherstellen, Transport, Bibliothek, zweizeilig, Eingabefeld, Programme/Bibliothek sofort sortieren (inkl. Fehlerfall), Listen nicht doppelt holen, Startbildschirm reduziert, Sendefarbe, Regler.')
