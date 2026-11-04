#!/usr/bin/env python3
"""v0.4.2.4 browser-only UI regression, backed by the v0.4.2.1 mock API."""
from pathlib import Path
from playwright.sync_api import sync_playwright
root=Path(__file__).resolve().parents[1]
html=(root/'web/index.html').read_text()
mock=(root/'tests/browser_v0421_smoke.py').read_text().split("mock=r'''",1)[1].split("'''",1)[0]
with sync_playwright() as p:
  browser=p.chromium.launch(headless=True,executable_path='/usr/bin/chromium',args=['--no-sandbox'])
  page=browser.new_page(viewport={'width':390,'height':780})
  page.set_default_timeout(6000)
  errors=[];page.on('pageerror',lambda e:errors.append(str(e)))
  page.on('dialog',lambda d:d.accept())
  page.set_content(html.replace('<script>','<script>'+mock,1),wait_until='domcontentloaded');page.wait_for_timeout(1000)
  assert not errors,errors
  assert page.locator('#rbDock').count()==1
  assert page.locator('#rbDock .v43-force-grid button').count()==8
  assert page.locator('.menu-title').count()==0
  assert 'Ersatzlaufzeit für Gruppenwechsel' not in page.content()
  assert page.locator('#view-home .shortcut-grid .shortcut').count()==6
  # home: one brightness + power actions + editor shortcut button; no modes or speed
  assert page.locator('#rbDock').is_visible()
  assert page.locator('#rbDock #homePowerOff').is_visible()
  assert not page.locator('#rbDock #dockSpeed').is_visible()
  assert page.locator('#rbDock #editShortcuts').is_visible()
  # mock screenshots for visual inspection
  (root/'tests/_screenshots').mkdir(exist_ok=True)
  page.screenshot(path=str(root/'tests/_screenshots/home.png'),full_page=False)
  page.evaluate("navigate('library')")
  assert page.locator('#rbDock #dockSpeed').is_visible()
  assert page.locator('#rbDock .v43-force-grid button').first.is_visible()
  assert not page.locator('#rbDock #editShortcuts').is_visible()
  page.screenshot(path=str(root/'tests/_screenshots/library.png'),full_page=False)
  # repeated mode presses must be independent of selected mode
  page.evaluate('''() => {window.__modes=[];const prev=window.api;window.api=async (path,data)=>{if(path==='/api/mode'){window.__modes.push(data.mode);return {ok:true}}return prev(path,data)}}''')
  page.locator('#rbDock .v43-force-btn[title="Laser"]').click()
  page.locator('#rbDock .v43-force-btn[title="Laser"]').click()
  assert page.evaluate('window.__modes')==['laser','laser']
  page.evaluate("navigate('editor')")
  assert page.locator('#rbDock #dockBrightness').is_visible()
  assert page.locator('#rbDock #dockSpeed').is_visible()
  assert page.locator('#previewRestart').locator('svg.rb-lucide').count()==1
  assert page.locator('#previewToggle').locator('svg.rb-lucide').count()==1
  assert page.locator('#previewSpeed').locator('svg.rb-lucide').count()==1
  assert page.locator('#previewSpeed').get_attribute('data-rb-icon')=='snail'
  page.locator('#previewSpeed').click()
  assert page.locator('#previewSpeed').get_attribute('data-rb-icon')=='bird'
  page.locator('#previewSpeed').click()
  assert page.locator('#previewSpeed').get_attribute('data-rb-icon')=='rabbit'
  page.locator('#previewSpeed').click()
  assert page.locator('#previewSpeed').get_attribute('data-rb-icon')=='snail'
  page.locator('#previewToggle').click()
  assert page.locator('#previewToggle').get_attribute('data-rb-icon')=='play'
  page.locator('#previewToggle').click()
  assert page.locator('#previewToggle').get_attribute('data-rb-icon')=='pause'
  assert not page.locator('#previewCaption').is_visible()
  assert page.locator('#previewRestart').inner_text()==''
  assert page.locator('#previewSpeed').inner_text()==''
  page.screenshot(path=str(root/'tests/_screenshots/editor.png'),full_page=False)
  # ensure bottom safe scroll and no dock occlusion of save controls, which are in normal document
  page.evaluate("navigate('programs')")
  page.locator('#v42ProgramName').fill('Mix')
  page.evaluate('window.scrollTo(0,document.documentElement.scrollHeight)');page.wait_for_timeout(120)
  rect=page.locator('#v42SaveProgram').bounding_box();dock=page.locator('#rbDock').bounding_box()
  assert rect and dock and rect['y']+rect['height']<=dock['y']+2, (rect,dock)
  page.screenshot(path=str(root/'tests/_screenshots/programs.png'),full_page=False)
  page.evaluate("navigate('ambience')")
  assert not page.locator('#rbDock #dockSpeed').is_visible()
  assert not page.locator('#rbDock .v43-force-grid').is_visible()
  page.screenshot(path=str(root/'tests/_screenshots/ambience.png'),full_page=False)
  # diagnostics' lucide bug icon and no dock
  page.evaluate("navigate('diagnostics')")
  assert not page.locator('#rbDock').is_visible()
  page.locator('#bananaMenu').click()
  assert page.locator('#mainMenu [data-nav="diagnostics"]').get_attribute('data-rb-icon')=='bug'
  assert page.locator('#mainMenu [data-nav="diagnostics"] svg.rb-lucide').count()==1
  assert not errors,errors
  print('PASS: v0.4.2.4 compact global dock, 8 direct modes (repeat), preview-only icons, mobile, no-occlusion, diagnostics')
  browser.close()
