"""Mobile UI regressions for the v0.4.2.1 hardware-test fixes."""
from playwright.sync_api import sync_playwright
from pathlib import Path
from ast import literal_eval, parse
source=Path(__file__).with_name('browser_v042_smoke.py').read_text()
tree=parse(source)
mock=next(literal_eval(n.value) for n in tree.body if getattr(n,'targets',None) and getattr(n.targets[0],'id','')=='mock')
html=Path(__file__).parents[1].joinpath('web/index.html').read_text().replace('<script>','<script>'+mock,1)
with sync_playwright() as p:
 browser=p.chromium.launch(executable_path='/usr/bin/chromium',args=['--no-sandbox'],headless=True)
 page=browser.new_page(viewport={'width':390,'height':780})
 errors=[];page.on('pageerror',lambda e:errors.append(str(e)))
 page.set_content(html,wait_until='domcontentloaded');page.wait_for_timeout(1100)
 assert not errors,errors
 page.evaluate("navigate('editor')")
 page.locator('#tabPages').click()
 page.locator('#line1').fill('RAVE')
 page.locator('#pageAdd').click();page.wait_for_timeout(300)
 assert page.locator('#pageRail button').count()==2
 page.locator('#line1').fill('BANANE')
 page.locator('#tabScroll').click();page.locator('#tabPages').click();page.wait_for_timeout(100)
 assert page.locator('#pageRail button').count()==2,'Tab must not silently reset draft'
 assert page.locator('#line1').input_value()=='BANANE'
 assert not page.locator('#previewTools').is_visible()
 assert page.locator('#pageSpeed').is_visible()
 assert page.locator('#pageDuration').count()==0
 assert page.locator('#programStart').count()==0
 # User declines clear: preserve entire draft.
 page.once('dialog',lambda d:d.dismiss())
 page.locator('#v42NewBlock').click();page.wait_for_timeout(170)
 assert page.locator('#pageRail button').count()==2
 page.once('dialog',lambda d:d.accept())
 page.locator('#v42NewBlock').click();page.wait_for_timeout(300)
 assert page.locator('#pageRail button').count()==1
 assert page.locator('#line1').input_value()==''
 page.locator('#tabScroll').click()
 assert page.locator('#previewTools').is_visible()
 assert page.locator('#previewCaption').inner_text()=='Lauftextvorschau'
 assert page.locator('#v42ClearText').is_visible()
 assert page.locator('[data-undo],[data-redo]').count()==0
 assert 'Experimentelles Datenbudget' not in page.content()
 # Two saved texts to build a program with mobile scrolling.
 page.locator('#scrollText').fill('TEST')
 page.locator('#libSaveAs').click();page.locator('.v42-dialog:not(.hidden) .primary').click();page.wait_for_timeout(300)
 page.evaluate("navigate('programs')")
 assert page.locator('#v42SaveProgram').is_visible()
 scroll=page.evaluate('''() => ({scroll:document.scrollingElement.scrollHeight,view:innerHeight,overflow:getComputedStyle(document.querySelector('#view-programs .inner-panel')).overflow})''')
 assert scroll['overflow']!='hidden',scroll
 assert scroll['scroll']>=scroll['view'],scroll
 page.evaluate("navigate('settings')")
 assert page.locator('#v42BackupFooter').is_visible()
 assert page.locator('#v42BackupFooter').evaluate('(el)=>el.closest("#v42SettingsBackup")!==null')
 assert not page.locator('button:has-text("Tafeln suchen")').count()
 page.evaluate("navigate('home')");page.locator('#editShortcuts').click();page.locator('[data-shortcut="2"]').click();page.wait_for_timeout(180)
 assert page.locator('#v42ShortcutLabel').is_disabled()
 assert page.locator('#v42ShortcutSave').is_disabled()
 assert not page.locator('#v42ShortcutDelete').is_visible()
 page.locator('#v42ShortcutType').select_option('1');page.wait_for_timeout(100)
 assert page.locator('#v42ShortcutLabel').is_disabled()
 page.locator('#v42ShortcutTarget').select_option('1')
 assert page.locator('#v42ShortcutLabel').is_enabled()
 page.locator('#v42ShortcutLabel').fill('RAVE')
 page.locator('#v42ShortcutSave').click();page.wait_for_timeout(300)
 assert page.evaluate('window.__store.shortcuts[1].label')=='RAVE'
 page.locator('[data-shortcut="2"]').click();page.wait_for_timeout(140)
 assert page.locator('#v42ShortcutDelete').is_visible()
 page.once('dialog',lambda d:d.accept())
 page.locator('#v42ShortcutDelete').click();page.wait_for_timeout(180)
 assert page.evaluate('window.__store.shortcuts[1]')=={'type':0,'id':0,'label':''}
 assert not errors,errors
 print('PASS v0.4.2.1 mobile UI: draft retained, reset confirmation, preview, program scroll, settings, typed shortcut and full deletion')
 browser.close()
