from pathlib import Path
from bs4 import BeautifulSoup
import re,subprocess,gzip,hashlib,sys
from playwright.sync_api import sync_playwright
root=Path(__file__).resolve().parents[1]
h=(root/'web/index.html').read_text()
ino=(root/'firmware/RavestickBoard_v0_4_3_1/RavestickBoard_v0_4_3_1.ino').read_text()
header=(root/'firmware/RavestickBoard_v0_4_3_1/RavestickBoard_WebUI.h').read_text()
assert 'FW_VERSION[] = "0.4.3.1-dev"' in ino
assert 'out.reserve(11000)' not in ino and 'json.reserve(5000)' not in ino
assert 'diagnosticReportStream();' in ino and 'RbDiagnosticStream json;' in ino
assert 'httpLargestMinDrop' in ino and 'httpLowestUnattributedSystemMin' in ino
assert 'Promise.all([fetch(\'/api/diagnostics' not in h
assert h.count('refreshDiagnostics();')==1, h.count('refreshDiagnostics();')
assert h.count('fetch(\'/api/diagnostics.txt\'')==1
assert 'v42NewBlock:\'flame-kindling\'' in h
assert 'let reportCache' in h
assert 'grid-template-columns:minmax(0,1fr);grid-template-rows:repeat(7,' in h
s=BeautifulSoup(h,'html.parser')
assert set(b.get_text(' ',strip=True) for b in s.select('#view-ambience .ambience-colors button'))==set()  # dynamic
assert [b.get('id') for b in s.select('#pageShow,#v42SendDraftBand,#v42SaveBlock')]==['pageShow','v42SendDraftBand','v42SaveBlock']
assert len(s.select_one('#pageShow').parent.find_all('button',recursive=False))==4
assert s.select_one('#libRename')['data-rb-icon']=='text-cursor-input'
assert s.select_one('#v42NewBlock')['data-rb-icon']=='flame-kindling'
for v in ['0.4.3-dev','0.4.2.5.1-dev']:
 assert v not in h
# gzip: reconstruct compressed payload and compare exact bytes with HTML input
array=header.split('PAGE_HTML_GZIP[] PROGMEM = {',1)[1].split('};',1)[0]
gz=bytes(int(c,16) for c in re.findall(r'0x[0-9a-f]{2}',array))
raw=gzip.decompress(gz);assert raw==('\n'+h.rstrip('\n')+'\n').encode('utf-8')
assert hashlib.sha256(raw).hexdigest()[:24] in header
js='\n'.join(tag.get_text() for tag in s.select('script:not([src])'))
js_tmp=Path('/mnt/data/rb0431_script.js'); js_tmp.write_text(js)
subprocess.run(['node','--check',str(js_tmp)],check=True)
mock='''<script>
window.__req=[];
window.fetch=async(p,opt={})=>{p=String(p).split('?')[0];__req.push(p);
let r={ok:true};
if(p==='/api/font')r={ok:true};
if(p==='/api/program')r={ok:true,pages:[{line1:'',line2:'',layout:3,align:1,color1:6,color2:7,duration:7}],current:0};
if(p==='/api/library')r={ready:true,items:[],shortcuts:[],used:0,total:100000};
if(p==='/api/collections')r={ok:true,items:[]};
if(p==='/api/shortcuts')r={ok:true,items:Array.from({length:8},()=>({type:0,id:0,label:''}))};
if(p==='/api/status')r={state:'idle',message:'Bereit',brightness:80,speed:80,powerKnown:true,powerOn:true};
if(p==='/api/diagnostics')r={version:'0.4.3.1-dev',uptime:'1 min',reset:'USB',freeHeap:42000,minHeap:30000,maxBlock:25000,appAvailable:500000,wifiClients:1,bleSuccess:0,errors:0,jobsOK:0,jobsFailed:0,pageShows:0,recent:'Ein Ereignis',lastError:'Keiner'};
return {ok:true,status:200,json:async()=>r,text:async()=>p===' /api/diagnostics.txt'?'Bericht':'Bericht'};
};</script>'''
with sync_playwright() as pw:
 browser=pw.chromium.launch(executable_path='/usr/bin/chromium',headless=True,args=['--no-sandbox','--disable-dev-shm-usage'])
 for size in [(390,844),(375,667),(430,932)]:
  context=browser.new_context(viewport={'width':size[0],'height':size[1]},is_mobile=True,has_touch=True)
  page=context.new_page();errors=[];page.on('pageerror',lambda err:errors.append(str(err)))
  page.set_content(h.replace('<script>',mock+'<script>',1),wait_until='domcontentloaded')
  page.wait_for_function('!document.documentElement.classList.contains("rb-booting")',timeout=15000)
  # Ambience view: 7 buttons in one column
  page.evaluate("navigate('ambience')")
  names=page.locator('#ambienceColors button').all_text_contents()
  assert names==['Rot','Gelb','Grün','Cyan','Blau','Magenta','Weiß'],names
  rects=[page.locator('#ambienceColors button').nth(i).bounding_box() for i in range(7)]
  assert all(abs(rects[i]['x']-rects[0]['x'])<2 for i in range(7)),rects
  assert all(rects[i+1]['y']>rects[i]['y']+rects[i]['height']-1 for i in range(6)),rects
  # test unique diagnostics JSON only once on navigation
  page.evaluate("navigate('diagnostics')")
  page.wait_for_timeout(200)
  paths=page.evaluate('__req');assert paths.count('/api/diagnostics')==1,paths
  assert '/api/diagnostics.txt' not in paths,paths
  page.locator('#diagReload').click();page.wait_for_timeout(100)
  assert page.evaluate("__req.filter(x=>x==='/api/diagnostics').length")==2
  # block controls disabled single page, after adding page changed
  page.evaluate("navigate('editor');setTab('pages')")
  assert page.locator('#pageDelete').is_disabled()
  assert page.locator('#v42NewBlock').is_disabled()
  assert page.locator('#pageDelete').get_attribute('data-rb-icon')=='trash-off'
  page.evaluate("pages.push(samplePage());renderSelected()")
  assert page.locator('#pageDelete').is_enabled()
  assert page.locator('#v42NewBlock').is_enabled()
  assert page.locator('#v42NewBlock').get_attribute('data-rb-icon')=='flame-kindling'
  assert len(page.locator('#pageShow').locator('xpath=..').locator('button').all())==4
  assert not errors,errors
  if size==(390,844):
   page.screenshot(path='/mnt/data/RavestickBoard_v0.4.3.1_Browser_Test.png',full_page=True)
  context.close()
 browser.close()
print('PASS: JS parse, gzip/ETag, firmware version, JSON diagnostics only once, 7 one-column colors, disabled/active block actions, 3 mobile viewports.')
