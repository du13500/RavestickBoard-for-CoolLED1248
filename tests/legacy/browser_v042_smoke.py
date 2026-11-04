from playwright.sync_api import sync_playwright
from pathlib import Path
web=Path(__file__).parents[1]/'web/index.html'
mock=r'''
window.__store={items:[],shortcuts:Array.from({length:6},()=>({type:0,id:0,label:''})),progs:[],pages:[{line1:'HELLO',line2:'',layout:0,align:1,color1:7,color2:7,duration:7}],next:1};
window.fetch=async function(url,opt={}){
 let u=new URL(url,'https://rb.local');let q=opt.body?new URLSearchParams(opt.body):u.searchParams;
 let get=(k,def='')=>q.get(k)??def;let s=window.__store;
 let ret={ok:true,message:'OK'},status=200;
 switch(u.pathname){
 case '/api/font':ret={};break;
 case '/api/status':ret={ok:true,state:'idle',message:'',playing:false,current:0,powerKnown:false,powerOn:true,brightness:75,speed:80};break;
 case '/api/program':ret={ok:true,pages:s.pages,playing:false,current:0};break;
 case '/api/program/set':s.pages[+get('index')]={line1:get('line1'),line2:get('line2'),layout:+get('layout'),align:+get('align'),color1:+get('color1'),color2:+get('color2'),duration:+get('duration')};break;
 case '/api/program/add':s.pages.push(get('copy')!==''?{...s.pages[+get('copy')]}:{line1:'',line2:'',layout:0,align:1,color1:7,color2:7,duration:7});break;
 case '/api/program/delete':s.pages.splice(+get('index'),1);break;
 case '/api/block/reset':s.pages=[{line1:'',line2:'',layout:0,align:1,color1:6,color2:7,duration:7}];break;
 case '/api/block/save':{
 const id=+get('id')||s.next++;let old=s.items.find(x=>x.id===id);
 const item={id,name:get('name'),kind:'block',mode:1,layout:5,color1:7,color2:7,pages:s.pages.map(x=>({...x})),favorite:old?.favorite||false};
 if(old)Object.assign(old,item);else s.items.push(item);ret={ok:true,id};break;}
 case '/api/block/get':ret={ok:true,id:+get('id'),pages:s.items.find(x=>x.id===+get('id'))?.pages||[]};break;
 case '/api/block/load':s.pages=s.items.find(x=>x.id===+get('id'))?.pages.map(x=>({...x}))||[];break;
 case '/api/block/meta':Object.assign(s.items.find(x=>x.id===+get('id')),get('name')?{name:get('name')}:{favorite:get('favorite')==='1'});break;
 case '/api/block/clone':{let src=s.items.find(x=>x.id===+get('id'));let id=s.next++;s.items.push({...src,name:get('name'),id});ret={ok:true,id};break;}
 case '/api/library':ret={ok:true,ready:true,items:s.items.map(({text,line2,pages,...x})=>({...x,pages:pages?.length||0})),shortcuts:s.shortcuts.map(x=>x.type===1?x.id:0),total:1990000,used:12000};break;
 case '/api/library/get':ret={ok:true,...s.items.find(x=>x.id===+get('id'))};break;
 case '/api/library/save':{
 const id=+get('id')||s.next++,item={id,name:get('name'),text:get('text'),line2:get('line2'),layout:+get('layout'),mode:+get('mode'),color1:+get('color1'),color2:+get('color2'),favorite:get('favorite')==='1',kind:'text'};
 let old=s.items.find(x=>x.id===id);if(old)Object.assign(old,item);else s.items.push(item);ret={ok:true,id};break;}
 case '/api/library/delete':s.items=s.items.filter(x=>x.id!==+get('id'));break;
 case '/api/collections':ret={ok:true,items:s.progs.map(p=>({id:p.id,name:p.name,count:p.ids.length,seconds:p.seconds}))};break;
 case '/api/collections/get':ret={ok:true,...s.progs.find(x=>x.id===+get('id'))};break;
 case '/api/collections/save':{let id=+get('id')||s.progs.length+1,p={id,name:get('name'),ids:get('ids').split(',').map(Number),seconds:+get('seconds')};let old=s.progs.find(x=>x.id===id);if(old)Object.assign(old,p);else s.progs.push(p);ret={ok:true,id};break;}
 case '/api/shortcuts':ret={ok:true,items:s.shortcuts};break;
 case '/api/shortcuts/set':s.shortcuts[+get('slot')-1]={type:+get('type'),id:+get('id'),label:get('label')};break;
 case '/api/shortcuts/run':break;
 default:break;
 }
 return {ok:status<400,status,json:async()=>ret,text:async()=>JSON.stringify(ret)};
};
'''
with sync_playwright() as p:
 browser=p.chromium.launch(executable_path='/usr/bin/chromium',headless=True,args=['--no-sandbox'])
 page=browser.new_page(viewport={'width':390,'height':780},accept_downloads=True)
 errs=[];page.on('pageerror',lambda e: errs.append(str(e)))
 html=web.read_text().replace('<script>','<script>'+mock,1)
 page.set_content(html,wait_until='domcontentloaded')
 page.wait_for_timeout(1300)
 print('errors after init', errs[:8]); print('library status', page.locator('#libStatus').inner_text())
 assert not errs,errs
 assert page.locator('#v42SearchToggle').count()==1
 page.evaluate("navigate('editor')")
 page.locator('#scrollText').fill('HALLO RAVE')
 print('before save',flush=True);page.locator('#libSaveAs').click(timeout=2500);page.wait_for_timeout(200)
 assert page.locator('.v42-dialog').count()>=2
 page.locator('.v42-dialog:not(.hidden) .v42-sheet input').last.fill('Test-Text')
 page.locator('.v42-dialog:not(.hidden) .primary').click();page.wait_for_timeout(400)
 assert len(page.evaluate('window.__store.items'))==1
 page.evaluate("navigate('editor')")
 print('before block',flush=True);page.locator('#tabPages').click(timeout=2500);page.wait_for_timeout(400)
 page.locator('#line1').fill('BANA');page.locator('#pageSave').click();page.wait_for_timeout(200)
 print('before save block',flush=True);page.locator('#v42SaveBlock').click(timeout=2500);page.wait_for_timeout(150)
 print('dialog status',page.locator('.v42-dialog:not(.hidden)').count(), page.locator('#status').inner_text(), flush=True)
 page.locator('.v42-dialog:not(.hidden) .primary').click(timeout=3000);page.wait_for_timeout(350);print('after block dialog click',page.evaluate('window.__store.items.length'),flush=True)
 assert len(page.evaluate('window.__store.items'))==2,page.evaluate('window.__store.items')
 print("before program nav",flush=True);page.evaluate("navigate('programs')");print("after program nav",flush=True)
 print('before program form',page.locator('#v42ProgramName').count(),page.locator('#v42ProgramName').is_visible(),page.evaluate('document.getElementById("view-programs").className'),flush=True);page.locator('#v42ProgramName').fill('Test-Programm',timeout=3000)
 page.evaluate('v42RefreshPrograms()');print('program choices',page.locator('#v42StepChoose option').all_text_contents(),'items',page.evaluate('libraryItems'),'errors',errs,flush=True);page.locator('#v42StepChoose').select_option('2',timeout=3000);page.locator('#v42StepAdd').click()
 print('before program save',flush=True);page.locator('#v42SaveProgram').click(timeout=3000);page.wait_for_timeout(500)
 assert len(page.evaluate('window.__store.progs'))==1
 print('before home nav',flush=True);page.evaluate("navigate('home')")
 print('before shortcut',flush=True);page.locator('#editShortcuts').click(timeout=3000)
 page.locator('[data-shortcut="1"]').click();page.wait_for_timeout(400)
 page.locator('#v42ShortcutType').select_option('3');page.locator('#v42ShortcutTarget').select_option('4');page.locator('#v42ShortcutLabel').fill('ROT 🔴')
 page.locator('#v42ShortcutSave').click();page.wait_for_timeout(400)
 assert page.evaluate('window.__store.shortcuts[0].type')==3
 assert not errs,errs
 print('PASS v0.4.2 mock: startup, text save, block save, collection, typed shortcut',len(page.evaluate('window.__store.items')))
 browser.close()
