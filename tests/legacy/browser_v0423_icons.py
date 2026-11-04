#!/usr/bin/env python3
"""Visual/action checks for direct BLE override icons and shortcut creation (mock)."""
from pathlib import Path
from playwright.sync_api import sync_playwright
root=Path(__file__).resolve().parents[1]
html=(root/'web/index.html').read_text()
old=(root/'tests/browser_v0421_smoke.py').read_text()
mock=old.split("mock=r'''",1)[1].split("'''",1)[0]
with sync_playwright() as p:
    browser=p.chromium.launch(executable_path='/usr/bin/chromium', headless=True, args=['--no-sandbox'])
    page=browser.new_page(viewport={'width':390,'height':780})
    page.set_default_timeout(5000)
    errors=[]
    page.on('pageerror',lambda e: errors.append(str(e)))
    page.on('dialog',lambda d:d.accept())
    page.set_content(html.replace('<script>','<script>'+mock,1),wait_until='domcontentloaded')
    page.wait_for_timeout(900)
    grids=page.locator('.v43-force-grid')
    assert grids.count()==3,grids.count()
    for grid in grids.all():
        buttons=grid.locator('button')
        assert buttons.count()==8,buttons.count()
        assert grid.locator('button svg.rb-lucide').count()==8,'Lucide paths missing'
        assert grid.locator('button[title="Standbild"] svg').count()==1
        assert grid.locator('button[title="Laser"] svg').count()==1
        assert not grid.locator('button.active,[aria-pressed="true"]').count()
    # Wrap network-level API, not view state; both identical presses must send.
    page.evaluate('''() => {
      window.__manualModes=[];
      const previous=window.api;
      window.api=async (path,data)=> {if(path==='/api/mode'){window.__manualModes.push(data.mode);return {ok:true}} return previous(path,data)};
    }''')
    page.evaluate("navigate('library')")
    page.locator('#view-library .v43-force-btn[title="Laser"]').click()
    page.locator('#view-library .v43-force-btn[title="Laser"]').click()
    page.locator('#view-library .v43-force-btn[title="Standbild"]').click()
    assert page.evaluate('window.__manualModes')==['laser','laser','static'],page.evaluate('window.__manualModes')
    # Idle shortcut directly opens editor instead of doing nothing.
    page.evaluate("navigate('home')")
    page.locator('.shortcut[data-shortcut="3"]').click()
    assert page.locator('#v42ShortcutDialog').is_visible()
    assert '3' in page.locator('#v42ShortcutTitle').inner_text()
    # Saved-entry editing may instantly create a fresh draft.
    page.locator('#v42ShortcutDialog .v42-dialog-close').click() if page.locator('#v42ShortcutDialog .v42-dialog-close').count() else page.evaluate("document.getElementById('v42ShortcutDialog').classList.add('hidden')")
    page.evaluate("navigate('editor')")
    assert page.locator('#v43NewContent').is_visible()
    page.locator('#scrollText').fill('SOMETHING')
    page.locator('#v43NewContent').click()
    assert page.locator('#scrollText').input_value()==''
    assert not errors,errors
    print('PASS: 3x 4-by-2 full Lucide icons; duplicate mode clicks; direct empty shortcut setup; editor New')
    browser.close()
