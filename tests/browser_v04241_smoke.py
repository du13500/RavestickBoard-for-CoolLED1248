#!/usr/bin/env python3
"""Behavioral browser smoke: v0.4.2.4.1 UI without ESP32 (offline mocked API)."""
from pathlib import Path
from playwright.sync_api import sync_playwright
root=Path(__file__).resolve().parents[1]
html=(root/'web/index.html').read_text(encoding='utf8')
with sync_playwright() as p:
    browser=p.chromium.launch(headless=True,executable_path='/usr/bin/chromium',args=['--no-sandbox'])
    page=browser.new_page(viewport={'width':390,'height':780})
    errors=[]
    page.on('pageerror',lambda e: errors.append(str(e)))
    page.on('dialog',lambda d:d.accept())
    page.set_content(html,wait_until='domcontentloaded')
    page.wait_for_timeout(250)
    # Expected: initial firmware API not reachable in isolated offline HTML test.
    page.evaluate("""() => { window.__requests=[];api=async (path,body) => { if(path==='/api/mode')window.__requests.push(body.mode);
       if(path==='/api/shortcuts')return {items:[{type:0,id:0,label:''},{type:0,id:0,label:''},{type:0,id:0,label:''},{type:0,id:0,label:''},{type:0,id:0,label:''},{type:0,id:0,label:''}]};
       if(path==='/api/collections')return {items:[]};
       if(path==='/api/library')return {ready:true,items:[]};
       return {ok:true,message:'OK'};
    }; window.__events=[];window.notify=(t)=>window.__events.push(t); }""")
    dock=page.locator('#rbDock')
    assert dock.count()==1
    assert page.evaluate('getComputedStyle(document.getElementById("rbDock")).position')=='static'
    assert page.evaluate('getComputedStyle(document.getElementById("rbDock")).transform')=='none'
    assert page.locator('#view-home .shortcut').count()==6
    page.evaluate("navigate('home')")
    # Must occupy the normal flow after content, not occlude a dialog or buttons.
    assert page.locator('#view-home').bounding_box()['y']+page.locator('#view-home').bounding_box()['height'] <= dock.bounding_box()['y']+2
    edit=page.locator('#editShortcuts')
    assert edit.get_attribute('data-rb-icon')=='pencil'
    edit.click()
    assert edit.get_attribute('data-rb-icon')=='pencil-off'
    assert edit.get_attribute('aria-pressed')=='true'
    assert page.evaluate('window.__events')==[]
    edit.click()
    assert edit.get_attribute('data-rb-icon')=='pencil'
    assert edit.get_attribute('aria-pressed')=='false'
    assert page.evaluate('window.__events')==[]
    # Empty shortcut opens dialog without edit-mode prerequisite.
    page.locator('.shortcut[data-shortcut="2"]').click()
    page.wait_for_timeout(200)
    assert page.locator('#v42ShortcutDialog').is_visible()
    assert '2' in page.locator('#v42ShortcutTitle').inner_text()
    for id,name in [('v42ShortcutDelete','trash'),('v42ShortcutCancel','undo-dot'),('v42ShortcutSave','star-check')]:
        b=page.locator('#'+id)
        assert b.get_attribute('data-rb-icon')==name,(id,name)
        assert b.get_attribute('data-rb-only')=='1'
        assert b.locator('svg.rb-lucide').count()==1, (id,name)
        assert b.get_attribute('aria-label')
    page.locator('#v42ShortcutCancel').click()
    assert not page.locator('#v42ShortcutDialog').is_visible()
    for v in ['library','editor','programs','ambience']:
        page.evaluate('navigate('+repr(v)+')')
        assert dock.is_visible(),v
        assert page.evaluate('getComputedStyle(document.getElementById("rbDock")).position')=='static'
        # view ends before the dock; no viewport overlay
        rect=page.locator('#view-'+v).bounding_box()
        d=dock.bounding_box()
        assert rect['y']+rect['height'] <= d['y']+3,(v,rect,d)
    page.evaluate("navigate('library')")
    m=page.locator('#rbDock button.v43-force-btn[title="Laser"]')
    m.click(); m.click()
    assert page.evaluate('window.__requests')==['laser','laser']
    page.evaluate("navigate('editor')")
    for icon in ['snail','bird','rabbit','snail']:
        assert page.locator('#previewSpeed').get_attribute('data-rb-icon')==icon
        page.locator('#previewSpeed').click()
    page.evaluate("navigate('programs')")
    page.evaluate('window.scrollTo(0,document.documentElement.scrollHeight)')
    page.wait_for_timeout(200)
    save=page.locator('#v42SaveProgram')
    assert save.is_visible()
    # saving button still receives clicks (not covered by dock)
    assert page.evaluate('document.elementFromPoint(...Object.values((()=>{let r=document.getElementById("v42SaveProgram").getBoundingClientRect();return {x:r.left+r.width/2,y:r.top+r.height/2}})()))?.id') in ('v42SaveProgram', '')
    page.evaluate("navigate('diagnostics')")
    assert not dock.is_visible()
    assert page.locator('#mainMenu [data-nav="diagnostics"]').get_attribute('data-rb-icon')=='bug'
    assert not [e for e in errors if 'Script' in e], errors
    browser.close()
    print('PASS: v0.4.2.4.1 normal-flow dock, shortcut icon-only dialog, pencil/off, no mode toasts, scroll and navigation')
