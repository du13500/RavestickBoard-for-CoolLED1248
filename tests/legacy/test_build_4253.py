#!/usr/bin/env python3
"""Offline regression/source validation. Does NOT compile Arduino or test hardware."""
from pathlib import Path
import subprocess, hashlib, gzip, re, tempfile, json
from html.parser import HTMLParser
ROOT=Path(__file__).resolve().parents[1]
folder=ROOT/'firmware/RavestickBoard_v0_4_2_5_3'
ino=(folder/'RavestickBoard_v0_4_2_5_3.ino').read_text(encoding='utf8')
web=(ROOT/'web/index.html').read_text(encoding='utf8').rstrip('\n')
header=(folder/'RavestickBoard_WebUI.h').read_text(encoding='utf8')
lib=(folder/'RavestickBoard_Library.h').read_text(encoding='utf8')
coll=(folder/'RavestickBoard_v042.h').read_text(encoding='utf8')
payload=('\n'+web+'\n').encode('utf8')
compressed=bytes(int(x,16) for x in re.findall(r'0x([0-9a-f]{2})',header))
assert gzip.decompress(compressed)==payload
assert hashlib.sha256(payload).hexdigest()[:24] in header
assert '0.4.2.5.3-dev' in ino and '0.4.2.4.2-dev-B' not in ino
assert 'httpRoute("/api/http.txt", HTTP_GET, onHttpTraceDownload)' in ino
assert 'esp_timer_start_periodic(httpTimer, 20000)' in ino
assert 'web.send_P(200, "text/html; charset=utf-8", (PGM_P)PAGE_HTML_GZIP, PAGE_HTML_GZIP_LEN)' in ino
assert 'web.collectHeaders(requestHeaders, 1)' in ino
assert 'Cache-Control", "no-cache"' in ino
assert 'JsonChunkWriter json' in lib and 'JsonChunkWriter json' in coll
assert 'JsonChunkWriter' in lib and 'static constexpr size_t CAP = 1024' in lib
assert 'libraryLoad(id)' in lib and 'v042BlockPageCount(id)' in lib
assert 'rb42ReadCollection(id)' in coll and 'rb42MissingCount()' in coll
assert 'struct LibraryRecord {' in lib and 'LIB_SCHEMA = 1' in lib
assert 'struct RB42Collection {' in coll and 'RB42_PROGRAM_MAGIC = ' in coll
assert (ROOT/'LICENSE').read_text().count('MIT License')==1
assert 'Lucide' not in (ROOT/'LICENSE').read_text()
assert 'ISC License' in (ROOT/'THIRD_PARTY_NOTICES.md').read_text()
assert 'Adam Schulz' in (ROOT/'README.md').read_text()

class DOM(HTMLParser):
    def __init__(self):super().__init__();self.tags=[];self.errors=[]
    def handle_starttag(self,tag,attrs):
        d=dict(attrs)
        keys=[k for k,v in attrs]
        if len(keys)!=len(set(keys)):self.errors.append(('duplicate_attrs',tag,attrs))
        self.tags.append((tag,d))
parser=DOM();parser.feed(web.split('<script>',1)[0]);assert not parser.errors,parser.errors[:3]
static=[x for x in parser.tags if x[1].get('data-rb-painted')]
assert len(static)>55,len(static)
for tag,attrs in static:
    assert attrs['data-rb-icon']==attrs['data-rb-painted'],(tag,attrs)
assert 'type="button">🍌' not in web
assert '<canvas class="range-pict"' not in web
assert 'if(typeof canvas.getContext!==\'function\')return;' in web
assert len(re.findall(r'<script>',web))==1
# Regression: SVG pre-render must never strip the opening '<' of the label span.
assert '</svg>span>' not in web
assert '<html lang="de" class="rb-booting">' in web
assert '<body class="home-active" data-rb-view="home">' in web
assert "finally{document.documentElement.classList.remove('rb-booting');}" in web
from bs4 import BeautifulSoup
soup=BeautifulSoup(web,'html.parser')
navs=soup.select('#mainMenu button[data-nav]')
assert len(navs)==7
assert [b.select_one(':scope > span').get_text(strip=True) for b in navs]==[
  'Kurzwahl','Texteditor','Bibliothek','Programme','Ambiente','Diagnose','Einstellungen']
assert all(b.select_one(':scope > svg.rb-lucide') for b in navs)
shortcuts=soup.select('#shortcutGrid .shortcut')
assert len(shortcuts)==8
assert 'RB42_SHORTCUT_COUNT = 8' in coll
assert 'RB42_SHORT_MAGIC = 0x52425333UL' in coll
assert 'RB42_SHORTCUT_FILE = "/rb-lib/shortcuts3.bin"' in coll
assert 'if(slot<=6)' in coll # prevent legacy metadata array overflow
assert 'for(uint8_t i=0;i<RB42_SHORTCUT_COUNT;++i)' in coll
assert 'row.append(pick,up,down)' in web
assert not any(x in web for x in ('v42SearchToggle','libSearchWrap','libSearch','libRefresh','library-filter','binoculars'))
assert 'const shown=libraryItems;' in web
assert 'await refreshLibrary()' in web
assert "if(view==='library')refreshLibrary();" in web
assert 'fav.onclick' not in web
assert "body[data-rb-view='home'] .rb-dock-speed" not in web
assert '{0x00DF, {14,17,18,20,18,17,22}}' in ino
assert '{0x1E9E, {14,17,18,20,18,17,22}}' in ino
assert all(c.select_one('.shortcut-icon[data-rb-icon="star-plus"][data-rb-painted="star-plus"] > svg.rb-lucide') for c in shortcuts)
assert soup.select_one('#v42ReloadMenu > svg.rb-lucide')


with tempfile.TemporaryDirectory() as tmp:
    js=Path(tmp)/'ui.js';js.write_text(web.split('<script>',1)[1].split('</script>',1)[0],encoding='utf8')
    subprocess.run(['node','--check',str(js)],check=True)
    old_header=header
    subprocess.run(['python',str(ROOT/'tools/sync_web.py')],check=True,capture_output=True)
    assert (folder/'RavestickBoard_WebUI.h').read_text(encoding='utf8')==old_header
    # C++ validate that generated header is syntactically valid and gzip bytes are binary-safe.
    stub=Path(tmp)/'webheader.cpp';stub.write_text('#include <stdint.h>\n#include <stddef.h>\n#define PROGMEM\n#include "'+str(folder/'RavestickBoard_WebUI.h')+'"\nstatic_assert(PAGE_HTML_GZIP_LEN > 0, "empty");\nint main(){return PAGE_HTML_GZIP_LEN<40000;}\n')
    subprocess.run(['g++','-std=c++17','-fsyntax-only',str(stub)],check=True)
    # compile the exact chunkwriter with narrow fake String/WebServer API
    body=lib[lib.index('struct JsonChunkWriter {'):lib.index('void onLibraryList() {')]
    stub2=Path(tmp)/'chunk.cpp';stub2.write_text('''#include <cstring>\n#include <cstdio>\n#include <cstdint>\n#include <string>\n#include <cassert>\nstruct String : std::string {using std::string::string;using std::string::operator=;using std::string::operator+=;bool reserve(size_t n){std::string::reserve(n);return true;} size_t length() const {return size();}};\nstruct FakeWeb{std::string result;int packets=0;void sendContent(const String &s){result+=s;packets++;}void sendContent(const char*s){result+=s;packets++;}}web;\n'''+body+'''int main(){JsonChunkWriter w;for(int i=0;i<30;i++){w.add("hello");w.addNumber(1234567890);}w.finish();assert(web.result.size()==(5+10)*30);assert(web.packets<5);return 0;}''')
    subprocess.run(['g++','-std=c++17',str(stub2),'-o',str(Path(tmp)/'chunk')],check=True)
    subprocess.run([str(Path(tmp)/'chunk')],check=True)

print(f'PASS 4253: version, MIT, API contracts, gzip {len(payload)} -> {len(compressed)} B, ETag, HTML {len(static)} prepainted elements, JS syntax, C++ headers, chunkwriter stub.')
print('NOT tested: full Arduino IDE compilation, hardware stability, live WebServer network behavior.')
