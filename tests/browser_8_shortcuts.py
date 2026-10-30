"""UI regression for eight shortcuts, no favorites, adaptive home, speed, boot icon."""
from pathlib import Path
import json
from playwright.sync_api import sync_playwright
ROOT=Path(__file__).resolve().parents[1]
html=(ROOT/'web/index.html').read_text(encoding='utf8')
responses={
 '/api/font':{}, '/api/program':{'pages':[{'line1':'','line2':'','layout':0,'align':0,'color1':4,'color2':4,'duration':2000}],'current':0},
 '/api/library':{'ready':True,'items':[{'id':1,'name':'Testtext','kind':'text','mode':0,'favorite':0},{'id':2,'name':'Noch einer','kind':'text','mode':0,'favorite':1}], 'shortcuts':[0]*6,'used':200,'total':2097152},
 '/api/collections':{'items':[]},
 '/api/shortcuts':{'items':[{'type':0,'id':0,'label':''} for _ in range(8)]},
 '/api/status':{'ok':True},
}
with sync_playwright() as p:
    browser=p.chromium.launch(executable_path='/usr/bin/chromium',headless=True,args=['--no-sandbox','--disable-dev-shm-usage'])
    for w,h in [(390,844),(393,852),(430,932),(390,667),(375,580),(844,390)]:
        context=browser.new_context(viewport={'width':w,'height':h},device_scale_factor=1,is_mobile=True,has_touch=True)
        page=context.new_page()
        errors=[]
        page.on('pageerror',lambda ex:errors.append(str(ex)))
        shim='<script>window.fetch=async function(path){const data='+json.dumps(responses,ensure_ascii=False)+';const p=String(path).split("?")[0];if(p==="/api/font")await new Promise(r=>setTimeout(r,60));return {ok:true,json:async()=>data[p]||{},text:async()=>JSON.stringify(data[p]||{})};};</script>'
        page.set_content(html.replace('<script>',shim+'<script>',1),wait_until='domcontentloaded')
        page.wait_for_function('!document.documentElement.classList.contains("rb-booting")',timeout=10000)
        assert page.locator('#shortcutGrid button.shortcut').count()==8
        assert page.locator('#shortcutGrid svg.rb-lucide').count()==8
        assert page.locator('body[data-rb-view="home"] .rb-dock-speed').is_visible()
        assert page.locator('#rbBootScreen svg.rb-lucide').count()==1
        assert not page.locator('#rbBootScreen').is_visible()
        assert not errors,errors[:3]
        dim=page.evaluate('''()=>{let ids=['#appShell','.app-header','#view-home','#shortcutGrid','#rbDock'];return Object.fromEntries(ids.map(id=>{let b=document.querySelector(id).getBoundingClientRect();return[id,{top:Math.round(b.top),bottom:Math.round(b.bottom),height:Math.round(b.height)}]}))}''')
        assert dim['#shortcutGrid']['bottom']<=dim['#rbDock']['top']+1,(w,h,dim)
        assert dim['#rbDock']['bottom']<=max(h,dim['#appShell']['bottom'])+2,(w,h,dim)
        if h>=667 and w<500:
          # On tall mobile sizes, the bottom margin should be small compared to screen height
          bottom_gap = h - dim['#rbDock']['bottom']
          assert bottom_gap < 30,(w,h,bottom_gap,dim)
        if (w,h)==(390,844):
          page.screenshot(path='/mnt/data/rb4252_home.png',full_page=True)
          # 8th shortcut is clickable in editing flow
          page.locator('#editShortcuts').click()
          page.locator('.shortcut[data-shortcut="8"]').click()
          assert page.locator('#v42ShortcutDialog').is_visible()
          assert '8' in page.locator('#v42ShortcutTitle').inner_text()
          page.locator('#v42ShortcutDialog .secondary').last.click() if False else None
          # hide modal for navbar navigation
          page.evaluate("document.getElementById('v42ShortcutDialog').classList.add('hidden')")
          page.locator('#bananaMenu').click()
          texts=[t.inner_text() for t in page.locator('#mainMenu button[data-nav]').all()]
          assert texts==['Kurzwahl','Texteditor','Bibliothek','Programme','Ambiente','Diagnose','Einstellungen'],texts
          page.locator('#mainMenu button[data-nav="library"]').click()
          assert page.locator('#view-library').is_visible()
          assert page.locator('#libList [data-rb-icon="star-plus"]').count()==0
          assert page.locator('#libList [data-rb-icon="star-check"]').count()==0
          assert page.locator('#libList .library-row').count()==2
          page.screenshot(path='/mnt/data/rb4252_library.png',full_page=True)
        print('viewport',w,h,'layout',dim,'errors',len(errors))
        context.close()
    browser.close()
print('Browser smoke PASS')
