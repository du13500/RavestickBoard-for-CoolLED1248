from pathlib import Path
from playwright.sync_api import sync_playwright
import json, copy
ROOT=Path(__file__).resolve().parents[1]
html=(ROOT/'web/index.html').read_text(encoding='utf8')
# Browser-side fake API stores separate Flash-like library items and RAM editor pages.
mock_js=r'''
<script>
const __textPage={line1:'HELLO',line2:'',layout:0,align:1,color1:7,color2:7,duration:7};
window.__mock={
 entries:[
  {id:1,name:'Alter Lauftext',text:'ORIGINAL',line2:'',layout:3,color1:7,color2:7,mode:2,kind:'text',favorite:0},
  {id:2,name:'Alte Blockfolge',text:'[Blockfolge]',line2:'',layout:5,color1:7,color2:7,mode:1,kind:'block',favorite:0,pages:[{...__textPage}]}
 ],
 pages:[{...__textPage}], requests:[], nextId:3,
};
window.fetch=async function(path,opt={}) {
 const full=String(path),p=full.split('?')[0];const arg=new URLSearchParams(opt.body||'');
 window.__mock.requests.push({path:p,method:opt.method||'GET',data:Object.fromEntries(arg.entries())});
 const m=window.__mock;let r={ok:true};
 if(p==='/api/font')r={ok:true};
 else if(p==='/api/library')r={ready:true,items:m.entries.map(e=>({id:e.id,name:e.name,kind:e.kind,mode:e.mode,favorite:0,pages:e.pages?.length||0})),shortcuts:[0,0,0,0,0,0],used:200,total:100000};
 else if(p==='/api/library/get'){const id=Number(new URLSearchParams(full.split('?')[1]).get('id'));r=JSON.parse(JSON.stringify(m.entries.find(e=>e.id===id)));}
 else if(p==='/api/library/save'){
  const id=arg.has('id')?Number(arg.get('id')):m.nextId++;
  const prev=m.entries.find(e=>e.id===id);
  const row={id,name:arg.get('name'),text:arg.get('text'),line2:arg.get('line2'),layout:Number(arg.get('layout')),color1:Number(arg.get('color1')),color2:Number(arg.get('color2')),mode:Number(arg.get('mode')),kind:'text',favorite:0};
  if(prev)Object.assign(prev,row);else m.entries.push(row);r={ok:true,id};
 }
 else if(p==='/api/block/load'){const found=m.entries.find(e=>e.id===Number(arg.get('id')));m.pages=JSON.parse(JSON.stringify(found.pages));r={ok:true};}
 else if(p==='/api/block/save'){
  const id=arg.has('id')?Number(arg.get('id')):m.nextId++;const name=arg.get('name');
  const prev=m.entries.find(e=>e.id===id);
  const row={id,name,text:'[Blockfolge]',line2:'',layout:5,color1:7,color2:7,mode:1,kind:'block',favorite:0,pages:JSON.parse(JSON.stringify(m.pages))};
  if(prev)Object.assign(prev,row);else m.entries.push(row);r={ok:true,id};
 }
 else if(p==='/api/program')r={ok:true,pages:JSON.parse(JSON.stringify(m.pages)),current:0};
 else if(p==='/api/program/set'){const i=Number(arg.get('index'));m.pages[i]={line1:arg.get('line1'),line2:arg.get('line2'),layout:Number(arg.get('layout')),align:Number(arg.get('align')),color1:Number(arg.get('color1')),color2:Number(arg.get('color2')),duration:Number(arg.get('duration'))};r={ok:true};}
 else if(p==='/api/block/reset'){m.pages=[{...__textPage}];r={ok:true};}
 else if(p==='/api/collections')r={ok:true,items:[]};
 else if(p==='/api/shortcuts')r={ok:true,items:Array.from({length:8},()=>({type:0,id:0,label:''}))};
 else if(p==='/api/status')r={ok:true,state:'idle',message:'Bereit',brightness:80,speed:80};
 return {ok:true,json:async()=>r,text:async()=>JSON.stringify(r)};
};
</script>
'''
with sync_playwright() as p:
 browser=p.chromium.launch(executable_path='/usr/bin/chromium',headless=True,args=['--no-sandbox','--disable-dev-shm-usage'])
 context=browser.new_context(viewport={'width':390,'height':844},is_mobile=True,has_touch=True)
 page=context.new_page()
 errors=[]
 page.on('pageerror',lambda e:errors.append(str(e)))
 page.set_content(html.replace('<script>',mock_js+'<script>',1),wait_until='domcontentloaded')
 page.wait_for_function('!document.documentElement.classList.contains("rb-booting")',timeout=10000)
 # Library, edit preexisting text and choose Save As after changing the font!
 page.locator('#bananaMenu').click()
 page.locator('#mainMenu button[data-nav="library"]').click()
 page.locator('#libList .lib-pick').filter(has_text='Alter Lauftext').click()
 page.locator('#libOpen').click()
 page.locator('#scrollLayout').select_option('4')
 page.locator('#scrollText').fill('VARIANT TEXT')
 assert page.locator('#libSaveAs').is_visible(), 'Save As hidden after opening original text'
 assert page.locator('#libSaveChanges').is_visible(), 'Save original hidden after changing font'
 assert page.evaluate('v42EditorOrigin')=='library', 'Changing font detached source'
 assert page.evaluate('v42EditorDirty()') is True, 'Changing text/font should be unsaved'
 page.locator('#libSaveAs').click()
 assert page.locator('.v42-dialog:not(.hidden)').is_visible(), 'Name input missing'
 assert 'Variante' in page.locator('.v42-dialog:not(.hidden) input').input_value()
 page.locator('.v42-dialog:not(.hidden) input').fill('Neuer Lauftext')
 page.locator('.v42-dialog:not(.hidden) .primary').click()
 page.wait_for_function('window.__mock.entries.length===3')
 v=page.evaluate('window.__mock.entries')
 assert next(x for x in v if x['id']==1)['text']=='ORIGINAL',v
 assert next(x for x in v if x['id']==3)['text']=='VARIANT TEXT',v
 assert next(x for x in v if x['id']==3)['layout']==4,v
 assert page.evaluate('libraryEditorId')==3, 'Editor not rebound to new text'
 assert page.locator('#libSaveChanges').is_visible(), 'Cannot now update copy'
 # Now open existing block sequence, edit line and save as new record.
 page.locator('#bananaMenu').click()
 page.locator('#mainMenu button[data-nav="library"]').click()
 page.locator('#libList .lib-pick').filter(has_text='Alte Blockfolge').click()
 page.locator('#libOpen').click()
 assert page.locator('#v42SaveBlock').is_visible(), 'Block Save As hidden while editing'
 assert page.locator('#v42UpdateBlock').is_visible(), 'Block Update hidden while editing'
 page.locator('#line1').fill('NEUE SEITE')
 page.locator('#v42SaveBlock').click()
 assert page.locator('.v42-dialog:not(.hidden)').is_visible(), 'Block save as name missing'
 assert 'Variante' in page.locator('.v42-dialog:not(.hidden) input').input_value()
 page.locator('.v42-dialog:not(.hidden) input').fill('Neue Blockfolge')
 page.locator('.v42-dialog:not(.hidden) .primary').click()
 page.wait_for_function('window.__mock.entries.length===4')
 v=page.evaluate('window.__mock.entries')
 assert next(x for x in v if x['id']==2)['pages'][0]['line1']=='HELLO',v
 assert next(x for x in v if x['id']==4)['pages'][0]['line1']=='NEUE SEITE',v
 assert page.evaluate('v42BlockEditorId')==4, 'Editor not rebound to copied block'
 assert page.locator('#v42UpdateBlock').is_visible(), 'Cannot update saved copy'
 # After saving copy, updating should only overwrite copy, not original.
 page.locator('#line1').fill('NOCHMAL')
 page.locator('#v42UpdateBlock').click()
 page.wait_for_function("window.__mock.entries.find(e=>e.id===4).pages[0].line1==='NOCHMAL'")
 v=page.evaluate('window.__mock.entries')
 assert next(x for x in v if x['id']==2)['pages'][0]['line1']=='HELLO',v
 assert page.evaluate('window.__mock.requests.filter(r=>r.path==="/api/block/save").map(r=>r.data)').__class__==list
 assert not errors,errors
 print('PASS Browser: text existing variant, original preserved; changed font binding; block existing variant, original preserved; editor bound to new; update only copy; no JS exceptions.')
 context.close();browser.close()
