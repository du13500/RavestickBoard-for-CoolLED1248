"""v0.4.4.0/0.4.5.0: Firmware-Renderer (g++ am Rechner, echter Sketch-Code) gegen Browser-Vorschau, Pixel für Pixel.
Prüft außerdem: Texte ohne Stilmarken werden unverändert wie in v0.4.3.4 gezeichnet (Golden-Datei),
Streaming und Vollpuffer liefern dasselbe Bild.   Aufruf: python tests/test_render_parity_0610.py"""
import re, sys, json, subprocess, tempfile, shutil
from pathlib import Path
from playwright.sync_api import sync_playwright
ROOT=Path(__file__).resolve().parents[1]; HOST=ROOT/'tests/host'
tmp=Path(tempfile.mkdtemp())
subprocess.run([sys.executable,str(HOST/'build.py'),str(ROOT),str(tmp/'gen.cpp')],check=True)
subprocess.run(['g++','-std=c++17','-O1','-w','-I',str(HOST),'-o',str(tmp/'host'),str(tmp/'gen.cpp')],check=True)
H=str(tmp/'host')
# 1) Texte ohne Marken: identisch zu v0.4.3.4
plain=subprocess.run([H,'plain'],capture_output=True,check=True).stdout
assert plain==(HOST/'golden_plain_0434.txt').read_bytes(),'Text ohne Stilmarken wird anders gezeichnet als in v0.4.3.4'
# 2) Stiltests am Rechner
out=subprocess.run([H,'styled'],capture_output=True,text=True).stdout
assert 'STYLED: ALL PASS' in out,out[-600:]
# 3) Parität Firmware <-> Browser
html=(ROOT/'web/index.html').read_text(encoding='utf8')
ino=next((ROOT/'firmware').glob('RavestickBoard_v*/*.ino')).read_text(encoding='utf8')
blk=ino[ino.index('static const Glyph FONT[] = {'):]; blk=blk[:blk.index('};')]
font={}
for m in re.finditer(r"\{\s*(?:'(\\?.)'|(0x[0-9A-Fa-f]+|\d+))\s*,\s*\{([^}]*)\}\s*\}",blk):
    ch,num,rows=m.groups(); font[ord(ch[-1]) if ch else int(num,0)]=[int(x) for x in rows.split(',')]
MOCK=r'''<script>window.fetch=async(p)=>{p=String(p).split('?')[0];let r={ok:true};if(p==='/api/font')r=%FONT%;
if(p==='/api/library')r={ready:true,items:[],shortcuts:[],used:0,total:100000};if(p==='/api/program')r={ok:true,pages:[],current:0};
if(p==='/api/collections')r={ok:true,items:[]};if(p==='/api/shortcuts')r={ok:true,items:[]};if(p==='/api/status')r={ok:true,state:'idle',message:'Bereit'};
return {ok:true,status:200,json:async()=>r,text:async()=>JSON.stringify(r)};};</script>'''.replace('%FONT%',json.dumps(font))
M=lambda n:chr(0x90+n); F=lambda n:chr(0x98+n); FR=chr(0x9c); R0=chr(0x90)
HEARTC='❤️'
cases=[
 ('farben normal',3,7,7,'Hi '+M(3)+'Raver'+R0+', '+M(6)+'Party!',''),
 ('farbwechsel',3,4,4,''.join(M([6,5,4,2][i%4])+c for i,c in enumerate('Hi Raver')),''),
 ('schriftmix',3,7,7,F(1)+'LIVE '+F(2)+'DJ '+F(3)+'22',''),
 ('engschrift basis',0,6,6,'AB'+F(0)+'cd'+F(3)+'ef'+FR+'GH',''),
 ('mini einzeilig basis',2,5,5,'abc'+F(1)+'Dd'+F(2)+'Ef'+FR+'xyz',''),
 ('bold basis',4,3,3,'Größe '+F(3)+'äöü ß'+FR+' Ok',''),
 ('symbole+fonts',3,4,4,'Du '+HEARTC+' '+F(3)+'\U0001F34C︎'+' ab'+FR+'\U0001F339 '+M(5)+'x',''),
 ('lang',3,7,7,'Ein langer Lauftext '+M(4)+'mit Farbe '+F(2)+'und Engschrift '+FR+M(3)+'bis weit '+R0+'ueber 64 Spalten hinaus',''),
 ('zweizeilig',1,4,2,'RAVE'+M(5)+'STICK','BAR '+M(3)+'OPEN'),
 ('zweizeilig +fontmarke',1,6,7,F(0)+'AB'+M(3)+'CD',M(2)+'xy'+F(3)+'z'),
]
with sync_playwright() as pw:
    b=pw.chromium.launch(headless=True,args=['--no-sandbox','--disable-dev-shm-usage'])
    ctx=b.new_context(viewport={'width':390,'height':844}); page=ctx.new_page(); errs=[]
    page.on('pageerror',lambda e:errs.append(str(e)))
    page_html=html.replace('<script>',MOCK+'<script>',1)
    page.route('http://rb.test/**',lambda r:r.fulfill(status=200,body=page_html,content_type='text/html; charset=utf-8'))
    page.goto('http://rb.test/',wait_until='domcontentloaded')
    page.wait_for_function('!document.documentElement.classList.contains("rb-booting")',timeout=15000)
    JS='''([layout,c1,c2,a,bt])=>{
      document.getElementById('scrollLayout').value=String(layout);scrollColor=c1;scrollColor2=c2;
      const da=rbDecode(a,layout),db=rbDecode(bt,layout);
      document.getElementById('scrollText').value=da.text;document.getElementById('scrollLine2').value=db.text;
      rbApplyDecoded('a',da);rbApplyDecoded('b',db);
      const out=[];const cv=document.createElement('canvas');const pitch=10;cv.width=W*pitch;cv.height=H*pitch;const g=cv.getContext('2d',{willReadFrequently:true});drawDots(g,pitch);
      const w=rbLineWidth('a');
      rbDrawStyled(g,'a',pitch,layout===1?0:(w<W?Math.floor((W-w)/2):0));
      if(layout===1)rbDrawStyled(g,'b',pitch,0);
      const hexes=Object.fromEntries(COLORS.map(c=>[c[2].toLowerCase(),c[1]]));
      for(let y=0;y<H;y++){let row='';for(let x=0;x<W;x++){const d=g.getImageData(Math.round((x+.5)*pitch),Math.round((y+.5)*pitch),1,1).data;const hx='#'+[d[0],d[1],d[2]].map(v=>v.toString(16).padStart(2,'0')).join('');row+=hexes[hx]!==undefined?String(hexes[hx]):'.';}out.push(row);}
      return {w,rows:out};}'''
    bad=0
    for name,layout,c1,c2,a,bt in cases:
        web=page.evaluate(JS,[layout,c1,c2,a,bt])
        res=subprocess.run([H,'render',str(layout),str(c1),str(c2),a.encode('utf8').hex(),bt.encode('utf8').hex()],capture_output=True,text=True)
        assert res.returncode==0,(name,res.stdout[:200])
        lines=res.stdout.strip().split('\n'); fw=lines[1:]
        assert any(c!='.' for row in fw for c in row),('leeres Bild',name)
        if fw!=web['rows']:
            bad+=1; print('MISMATCH',name)
            for y in range(16): print('FW ',fw[y]); print('WEB',web['rows'][y]) if fw[y]!=web['rows'][y] else None
    assert not bad,f'{bad} Fälle weichen ab'

    # ---- v0.4.6.0: Blockseiten (mit und ohne Stilmarken): Firmware renderBlockPage <-> Browser drawPage ----
    PJS=r"""([layout,align,c1,c2,l1,l2])=>{
      const p={layout,align,color1:c1,color2:c2,line1:l1,line2:l2};
      const cv=document.createElement('canvas');const pitch=10;cv.width=W*pitch;cv.height=H*pitch;const g=cv.getContext('2d',{willReadFrequently:true});
      drawPage(g,p,pitch);
      const hexes=Object.fromEntries(COLORS.map(c=>[c[2].toLowerCase(),c[1]]));const rows=[];
      for(let y=0;y<H;y++){let row='';for(let x=0;x<W;x++){const d=g.getImageData(Math.round((x+.5)*pitch),Math.round((y+.5)*pitch),1,1).data;const hx='#'+[d[0],d[1],d[2]].map(v=>v.toString(16).padStart(2,'0')).join('');row+=hexes[hx]!==undefined?String(hexes[hx]):'.';}rows.push(row);}
      return {rows,w:[rbPageLinePx(l1,layout),layout===1?rbPageLinePx(l2,layout):0]};}"""
    pcases=[
     ('seite plain normal',3,1,7,7,'Hi Raver',''),('seite plain links',3,0,5,7,'Party',''),('seite plain rechts',4,2,3,7,'Ok',''),
     ('seite plain eng',0,1,6,6,'RAVESTICK',''),('seite plain mini',2,1,4,4,'mini text',''),('seite plain zweizeilig',1,0,4,2,'RAVE','STICK'),
     ('seite farben',3,1,7,7,'Hi '+M(3)+'Rav'+R0+' '+M(6)+'Go',''),('seite farben rechts',3,2,7,7,M(2)+'A'+M(5)+'B'+M(4)+'C',''),
     ('seite schriftmix',3,1,7,7,F(1)+'LIVE '+F(2)+'DJ',''),('seite eng+farbe',0,0,6,6,'AB'+M(3)+'cd'+F(3)+'ef',''),
     ('seite mini+farbe',2,1,5,5,'abc'+M(3)+'Dd'+F(1)+'ef',''),('seite bold',4,1,3,3,'Gr'+M(5)+'öße',''),
     ('seite zweizeilig farben',1,1,4,2,'RAVE'+M(5)+'STICK','BAR '+M(3)+'OPEN'),('seite zweizeilig rechts',1,2,6,7,M(2)+'ab',M(4)+'cd'),
     ('seite symbole',3,1,4,4,'Du '+HEARTC+' '+M(5)+'x',''),
    ]
    # zu breite Seiten lehnt die Seite ab, wie die Firmware (renderBlockPage liefert 0)
    for l1 in ['Hi '+M(3)+'Raver'+R0+' '+M(6)+'Go','Größe '+M(5)+'äöü']:
        assert page.evaluate("([l])=>validatePage({layout:l.layout,align:1,color1:7,color2:7,line1:l.t,line2:''})",[{'layout':3 if 'Raver' in l1 else 4,'t':l1}]).startswith('Inhalt zu breit'),l1
    for name,layout,align,c1,c2,l1,l2 in pcases:
        web=page.evaluate(PJS,[layout,align,c1,c2,l1,l2])
        res=subprocess.run([H,'page',str(layout),str(align),str(c1),str(c2),l1.encode('utf8').hex(),l2.encode('utf8').hex()],capture_output=True,text=True)
        assert res.returncode==0,(name,res.stdout[:200])
        lines=res.stdout.strip().split('\n'); fw=lines[1:]
        assert any(c!='.' for row in fw for c in row),('leeres Bild',name)
        fwW=[int(x) for x in lines[0].split('=')[1].split(',')]
        assert fwW[0]==web['w'][0] and (layout!=1 or fwW[1]==web['w'][1]),('Breite',name,fwW,web['w'])
        if fw!=web['rows']:
            bad+=1; print('MISMATCH',name,'fwW',fwW,'webW',web['w'])
            for y in range(16):
                if fw[y]!=web['rows'][y]: print('FW ',fw[y]); print('WEB',web['rows'][y])
    assert not bad,f'{bad} Fälle weichen ab'
    assert not errs,errs
    b.close(); shutil.rmtree(tmp,ignore_errors=True)
print(f'PASS Parität: {len(cases)} Lauftext- und {len(pcases)} Seitenfälle Firmware (Sketch-Code) = Browser-Vorschau, Texte ohne Stilmarken unverändert zu v0.4.3.4, Streaming = Vollpuffer.')
