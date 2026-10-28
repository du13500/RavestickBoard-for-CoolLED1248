#!/usr/bin/env python3
"""Browser-only checks, using mocked ESP32 endpoints; not a firmware integration test."""
from pathlib import Path
from playwright.sync_api import sync_playwright
root=Path(__file__).resolve().parents[1]
html=(root/'web/index.html').read_text()
old=(root/'tests/browser_v0421_smoke.py').read_text()
mock=old.split("mock=r'''",1)[1].split("'''",1)[0]
# Current backend includes program missing markers and shortcut detaching on delete.
mock=mock.replace("count:p.ids.length,seconds:p.seconds", "count:p.ids.length,seconds:p.seconds,ids:p.ids,missing:p.ids.filter(id=>!s.items.find(x=>x.id===id)).length")
mock=mock.replace("case '/api/library/delete':s.items=s.items.filter(x=>x.id!==+get('id'));break;", "case '/api/library/delete':s.items=s.items.filter(x=>x.id!==+get('id'));s.shortcuts=s.shortcuts.map(x=>x.type===1&&x.id===+get('id')?{type:0,id:0,label:''}:x);break;")
mock=mock.replace("case '/api/collections/get':ret={ok:true,...s.progs.find(x=>x.id===+get('id'))};break;", "case '/api/collections/get':ret={ok:true,...s.progs.find(x=>x.id===+get('id'))};break;")
mock=mock.replace("case '/api/shortcuts/run':break;", "case '/api/shortcuts/run':break;")
with sync_playwright() as p:
 browser=p.chromium.launch(executable_path='/usr/bin/chromium',headless=True,args=['--no-sandbox'])
 page=browser.new_page(viewport={'width':390,'height':780},accept_downloads=True)
 page.set_default_timeout(5000)
 errs=[];dialogs=[]
 page.on('pageerror',lambda e:errs.append(str(e)))
 page.on('dialog',lambda d:(dialogs.append(d.message),d.accept()))
 page.set_content(html.replace('<script>','<script>'+mock,1),wait_until='domcontentloaded')
 page.wait_for_timeout(1100)
 assert not errs,errs
 assert page.locator('#v42EditorHeading').inner_text()=='Neuen Inhalt erstellen'
 # New draft text saved to library switches to visually distinct editing mode.
 page.evaluate("navigate('editor')")
 page.locator('#scrollText').fill('RAVE BANANE')
 page.locator('#libSaveAs').click()
 page.locator('.v42-dialog:not(.hidden) .v42-sheet input').last.fill('Testtext')
 page.locator('.v42-dialog:not(.hidden) .primary').click()
 page.wait_for_timeout(350)
 assert page.locator('#view-editor').evaluate("x=>x.classList.contains('editing-library')")
 assert not page.locator('#libSaveChanges').evaluate("x=>x.classList.contains('hidden')")
 assert page.locator('#libSaveAs').evaluate("x=>x.classList.contains('hidden')")
 assert not page.locator('#tabPages').is_visible()
 # Save changed text in place -> same ID, no new record.
 page.locator('#scrollText').fill('RAVE BANANE 2')
 page.locator('#libSaveChanges').click()
 page.wait_for_timeout(350)
 assert len(page.evaluate('window.__store.items'))==1
 assert page.evaluate('window.__store.items[0].text')=='RAVE BANANE 2'
 # Direct back link without modifications => no confirmation.
 n=len(dialogs)
 page.locator('#v42ReturnLibrary').click()
 page.wait_for_timeout(350)
 assert len(dialogs)==n,dialogs
 assert page.locator('#view-library').is_visible()
 # Reopen, modify and return => confirmation.
 page.locator('#libOpen').click()
 page.wait_for_timeout(350)
 page.locator('#scrollText').fill('UNSAVED')
 page.locator('#v42ReturnLibrary').click()
 page.wait_for_timeout(350)
 assert len(dialogs)==n+1,dialogs
 assert page.locator('#view-library').is_visible()
 # New editable block, save as library entry.
 page.evaluate("navigate('editor')")
 page.locator('#tabPages').click()
 page.locator('#line1').fill('BANA')
 page.locator('#v42SaveBlock').click()
 page.wait_for_timeout(150)
 page.locator('.v42-dialog:not(.hidden) .primary').click()
 page.wait_for_timeout(350)
 assert len(page.evaluate('window.__store.items'))==2,page.evaluate('window.__store.items')
 # Program with a text and a block; save it and test delete-dependency UI.
 page.evaluate("navigate('programs')")
 page.locator('#v42ProgramName').fill('Mein Mix')
 page.locator('#v42StepChoose').select_option('1')
 page.locator('#v42StepAdd').click()
 page.locator('#v42StepChoose').select_option('2')
 page.locator('#v42StepAdd').click()
 page.locator('#v42SaveProgram').click()
 page.wait_for_timeout(350)
 assert len(page.evaluate('window.__store.progs'))==1
 assert page.locator('#v42RunProgram').is_enabled()
 # Delete used text -> keep original program step, mark program as incomplete.
 page.evaluate("navigate('library')")
 page.evaluate('librarySelected=1;renderLibrary()')
 page.locator('#libDelete').click()
 page.wait_for_timeout(300)
 assert len(page.evaluate('window.__store.items'))==1
 assert page.locator('#view-library').is_visible()
 page.evaluate("navigate('programs')")
 assert 'fehlen' in page.locator('#v42Programs').inner_text()
 page.locator('#v42Programs button').first.click()
 page.wait_for_timeout(350)
 assert page.locator('#v42ProgramError').is_visible()
 assert page.locator('#v42RunProgram').is_disabled()
 # Reassign missing step via in-place select and persist repair.
 page.locator('#v42ProgramSteps .v42-program-invalid select').select_option('2')
 page.locator('#v42SaveProgram').click()
 page.wait_for_timeout(400)
 assert page.locator('#v42ProgramError').is_hidden()
 # Long BLE job polling should not create a toast for one failed status request.
 page.evaluate("document.querySelector('#status').hidden=true; rbPollFailures=0; rbPollFailureStarted=0")
 assert page.locator('#rbConnectionStatus').is_hidden()
 assert not errs,errs
 print('PASS: mock browser create/edit/save-in-place, back, confirm, block, mixed program, dangling refs, repair, no JS errors')
 browser.close()
