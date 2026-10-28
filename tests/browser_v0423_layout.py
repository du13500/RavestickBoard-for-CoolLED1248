#!/usr/bin/env python3
"""Mobile editor and sticky-preview checks with simulated ESP32 responses."""
from pathlib import Path
from playwright.sync_api import sync_playwright
root=Path(__file__).resolve().parents[1]
html=(root/'web/index.html').read_text()
old=(root/'tests/browser_v0421_smoke.py').read_text()
mock=old.split("mock=r'''",1)[1].split("'''",1)[0]
with sync_playwright() as p:
 b=p.chromium.launch(headless=True,executable_path='/usr/bin/chromium',args=['--no-sandbox'])
 page=b.new_page(viewport={'width':390,'height':780})
 page.set_default_timeout(5000)
 errors=[];page.on('pageerror',lambda e:errors.append(str(e)))
 page.set_content(html.replace('<script>','<script>'+mock,1),wait_until='domcontentloaded');page.wait_for_timeout(1200)
 page.evaluate("navigate('editor')")
 assert page.locator('#v42ReturnLibrary').is_visible()
 assert page.locator('#tabScroll').is_visible()
 assert page.locator('#previewTools').is_visible()
 page.locator('#tabPages').click()
 assert page.locator('#previewTools').is_hidden()
 assert page.locator('#previewCaption').inner_text()==''
 page.locator('#line1').fill('RAVE')
 page.locator('#pageAdd').click();page.wait_for_timeout(170)
 assert page.locator('#pageRail button').count()==2
 page.locator('#line1').fill('BANANE')
 page.locator('#tabScroll').click();page.locator('#tabPages').click()
 assert page.locator('#line1').input_value()=='BANANE'
 # Compare sticky preview offset with true dynamic header height when scrolling.
 page.evaluate('window.scrollTo(0,250)');page.wait_for_timeout(260)
 geometry=page.evaluate('''() => {let h=document.querySelector('.app-header').getBoundingClientRect(),p=document.querySelector('.preview-sticky').getBoundingClientRect();return {headerBottom:h.bottom, previewTop:p.top, scrollY:scrollY, cssTop:getComputedStyle(document.querySelector('.preview-sticky')).top}}''')
 assert geometry['scrollY']>50,geometry
 assert geometry['previewTop']>=geometry['headerBottom']-2,geometry
 assert page.locator('#pageDuration').count()==0
 assert page.locator('#programStart').count()==0
 assert page.locator('[data-undo],[data-redo]').count()==0
 assert 'Experimentelles Datenbudget' not in page.content()
 page.evaluate("navigate('programs')");page.wait_for_timeout(160)
 assert page.locator('#v42SaveProgram').is_visible()
 overflow=page.evaluate('getComputedStyle(document.querySelector("#view-programs .inner-panel")).overflow')
 assert overflow!='hidden',overflow
 page.evaluate("navigate('settings')")
 assert page.locator('#v42BackupFooter').is_visible()
 assert not errors,errors
 print('PASS: mobile preview offset, tabs, block draft, program scrolling and settings',geometry)
 b.close()
