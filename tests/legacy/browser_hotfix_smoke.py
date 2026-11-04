"""Browser DOM regression for 4251, with delayed ESP32 API fixtures (no hardware)."""
from pathlib import Path
import json
from playwright.sync_api import sync_playwright
ROOT=Path(__file__).resolve().parents[1]
html=(ROOT/'web/index.html').read_text(encoding='utf8')
pages={'pages':[{'line1':'','line2':'','layout':0,'align':0,'color1':4,'color2':4,'duration':2000}],'current':0}
responses={
 '/api/font':{}, '/api/program':pages,
 '/api/library':{'ready':True,'items':[],'shortcuts':[0,0,0,0,0,0],'used':0,'total':2097152},
 '/api/collections':{'items':[]},
 '/api/shortcuts':{'items':[{'type':0} for _ in range(8)]},
 '/api/status':{'ok':True},
}
with sync_playwright() as p:
    browser=p.chromium.launch(executable_path='/usr/bin/chromium',headless=True,args=['--no-sandbox','--disable-dev-shm-usage'])
    context=browser.new_context(viewport={'width':390,'height':844},device_scale_factor=1,is_mobile=True,has_touch=True)
    page=context.new_page()
    errs=[]
    page.on('pageerror',lambda exc: errs.append(str(exc)))
    def handle(route):
        from urllib.parse import urlsplit
        path=urlsplit(route.request.url).path
        if path=='/':
            route.fulfill(status=200,body=html,headers={'content-type':'text/html; charset=utf-8'})
        elif path in responses:
            route.fulfill(status=200,body=json.dumps(responses[path]),headers={'content-type':'application/json'})
        elif path.startswith('/api/'):
            route.fulfill(status=200,body='{}',headers={'content-type':'application/json'})
        else:
            route.fulfill(status=404,body='')
    page.route('**/*',handle)
    # Intercept API calls inside an about:blank page; no real network or device needed.
    shim = '<script>window.fetch=async function(path){const data='+json.dumps(responses,ensure_ascii=False)+';const p=String(path).split("?")[0];if(p==="/api/font")await new Promise(r=>setTimeout(r,350));return {ok:true,json:async()=>data[p]||{},text:async()=>JSON.stringify(data[p]||{})};};</script>'
    page.set_content(html.replace('<script>',shim+'<script>',1), wait_until='domcontentloaded')
    assert page.locator('#rbBootScreen').is_visible(), 'Loader must be visible while async API is pending'
    assert page.locator('#appShell').is_hidden(), 'App must not flash partial state'
    page.wait_for_function("!document.documentElement.classList.contains('rb-booting')",timeout=10000)
    page.screenshot(path='/mnt/data/rb4251_browser_home.png',full_page=True)
    assert page.locator('#shortcutGrid .shortcut-icon svg.rb-lucide').count()==8
    assert page.locator('#shortcutGrid .shortcut').count()==8
    assert page.locator('.rb-dock-speed').is_visible(), 'Speed slider must be visible on home from first paint'
    assert page.locator('.rb-dock .pixel-range').count()==2
    assert page.locator('#rbBootScreen').is_hidden()
    page.locator('#bananaMenu').click()
    items=page.locator('#mainMenu button[data-nav]')
    expected=['Kurzwahl','Texteditor','Bibliothek','Programme','Ambiente','Diagnose','Einstellungen']
    actual=[x.inner_text() for x in items.all()]
    assert actual==expected,actual
    assert page.locator('#mainMenu button[data-nav] > span').count()==7
    page.locator('#mainMenu button[data-nav="editor"]').click()
    assert page.locator('#view-editor').is_visible()
    assert page.locator('.rb-dock-speed').is_visible()
    page.locator('#bananaMenu').click()
    page.locator('#mainMenu button[data-nav="home"]').click()
    assert page.locator('#view-home').is_visible()
    assert page.locator('.rb-dock-speed').is_visible()
    page.screenshot(path='/mnt/data/rb4251_browser_menu.png',full_page=True)
    print('HEADLESS BROWSER PASS: 7 correctly labeled navigation buttons; 8 first-paint shortcut icons; home speed row; splash removed.')
    print('Uncaught JS errors:',errs[:10])
    browser.close()
