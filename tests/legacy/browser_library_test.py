from playwright.sync_api import sync_playwright
from pathlib import Path
import json
web=Path(__file__).parents[1]/'web/index.html'
mock=r'''
window.__store = {items: [], shortcuts: [0,0,0,0,0,0], nextId: 1};
window.fetch = async function(path,opt={}){
 let u=new URL(path,'https://rb.local/'),q=new URLSearchParams(u.search);
 if(opt.body)q=new URLSearchParams(opt.body);
 const get=(key,d='')=>q.get(key)||d;
 const store=window.__store,items=store.items,shortcuts=store.shortcuts;
 let data={ok:true},status=200;
 switch(u.pathname){
 case '/api/font':data={};break;
 case '/api/program':data={ok:true,playing:false,current:0,pages:[{line1:'HELLO',line2:'',layout:0,align:1,color1:7,color2:7,duration:7}]};break;
 case '/api/status':data={ok:true,state:'idle',message:'',playing:false,current:0,powerKnown:false,powerOn:true,brightness:75,speed:80};break;
 case '/api/library':data={ok:true,ready:true,count:items.length,items:items.map(({text,line2,color2,...rest})=>rest),shortcuts,total:1000000,used:5000+items.length*3500};break;
 case '/api/library/get':data={ok:true,...items.find(x=>x.id===+get('id'))};break;
 case '/api/library/save':{
 const id=+get('id',0)||store.nextId++,obj={id,name:get('name'),text:get('text'),line2:get('line2'),layout:+get('layout'),color1:+get('color1'),color2:+get('color2'),mode:+get('mode'),favorite:get('favorite')==='1'};
 const ix=items.findIndex(x=>x.id===id);if(ix>=0)items[ix]=obj;else items.push(obj);data={ok:true,id,message:'Gespeichert'};break;
 }
 case '/api/library/shortcut':shortcuts[+get('slot')-1]=+get('id');data={ok:true,message:'Belegt'};break;
 case '/api/library/send':data={ok:true,message:'Übertragung gestartet'};break;
 case '/api/library/move':{const ix=items.findIndex(x=>x.id===+get('id'));items.splice(+get('to'),0,items.splice(ix,1)[0]);break;}
 case '/api/library/delete':{const id=+get('id');if(shortcuts.includes(id)){data={ok:false,message:'Referenzschutz'};status=409;}else items.splice(items.findIndex(x=>x.id===id),1);break;}
 default:data={ok:true,message:'OK'};
 }
 return {ok:status<400,status,json:async()=>data,text:async()=>JSON.stringify(data)};
};
'''
with sync_playwright() as p:
 browser=p.chromium.launch(executable_path='/usr/bin/chromium',headless=True,args=['--no-sandbox'])
 page=browser.new_page(viewport={'width':390,'height':780},accept_downloads=True)
 errors=[];page.on('pageerror',lambda e:errors.append(str(e)))
 page.on('dialog',lambda d:d.accept('Festival-Banane'))
 page.set_content(web.read_text().replace('<script>','<script>'+mock,1),wait_until='domcontentloaded')
 page.wait_for_function('document.querySelector("#libStatus").textContent.includes("/ 64")',timeout=10000)
 assert '0 / 64' in page.locator('#libStatus').inner_text(),page.locator('#libStatus').inner_text()
 # Navigate using public app function; menu is hidden by design.
 page.evaluate("navigate('editor')")
 page.locator('#scrollText').fill('HELLO 🍌 RAVE')
 page.locator('#libSaveAs').click()
 page.wait_for_function('window.__store.items.length===1')
 assert page.locator('#libStatus').inner_text().startswith('1 / 64')
 page.evaluate("navigate('library')")
 page.locator('#libSlot').select_option('1')
 page.locator('#libAssign').click()
 page.wait_for_function('window.__store.shortcuts[0]===1')
 page.evaluate("navigate('home')")
 assert page.locator('.shortcut[data-shortcut="1"] span:nth-child(2)').inner_text()=='Festival-Banane'
 page.locator('.shortcut[data-shortcut="1"]').click()
 page.evaluate("navigate('library')")
 page.locator('#libDuplicate').click()
 page.wait_for_function('window.__store.items.length===2')
 page.locator('#libOpen').click()
 page.wait_for_function('document.getElementById("scrollText").value==="HELLO 🍌 RAVE"')
 page.locator('#scrollText').fill('HELLO 2')
 page.locator('#libSaveChanges').click()
 page.wait_for_function('window.__store.items.some(x=>x.text==="HELLO 2")')
 page.evaluate("navigate('library')")
 with page.expect_download() as download_info:
  page.locator('#libExport').click()
 download=download_info.value
 saved=json.loads(Path(download.path()).read_text())
 assert saved['schema']==1 and len(saved['items'])==2 and saved['shortcuts'][0]==1
 page.locator('#libImport').set_input_files({'name':'restore.json','mimeType':'application/json','buffer':json.dumps(saved,ensure_ascii=False).encode('utf-8')})
 page.wait_for_function('window.__store.items.length===4')
 assert page.locator('#libStatus').inner_text().startswith('4 / 64')
 assert not errors,errors
 print('PASS browser: save, reload library list, shortcut/send, duplicate, edit/update, JSON backup and additive import, no JS errors')
 browser.close()
