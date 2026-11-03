"""Hilfsprogramm fuer test_anim_core_0610.py: zerlegt GIF-Dateien mit dem ImageDecoder von Chromium (Playwright) in
zusammengesetzte RGBA-Bilder. Aufruf: gif_ref_chromium_0610.py out.json a.gif b.gif ...  (JSON: {pfad: [rgba-Hex je Bild]})"""
import sys, json, base64, threading, http.server
from playwright.sync_api import sync_playwright
class H(http.server.BaseHTTPRequestHandler):
    def do_GET(self):
        self.send_response(200); self.send_header('Content-Type', 'text/html'); self.end_headers(); self.wfile.write(b'<html></html>')
    def log_message(self, *a): pass
srv = http.server.HTTPServer(('127.0.0.1', 0), H); threading.Thread(target=srv.serve_forever, daemon=True).start()
res = {}
with sync_playwright() as p:
    br = p.chromium.launch(); pg = br.new_page(); pg.goto(f'http://localhost:{srv.server_port}/')
    for path in sys.argv[2:]:
        b = base64.b64encode(open(path, 'rb').read()).decode()
        res[path] = pg.evaluate("""async (b)=>{
          const u=Uint8Array.from(atob(b),c=>c.charCodeAt(0));
          const d=new ImageDecoder({data:u,type:'image/gif'});await d.tracks.ready;
          const n=d.tracks.selectedTrack.frameCount;const out=[];
          for(let i=0;i<n;i++){const {image}=await d.decode({frameIndex:i});
            const c=new OffscreenCanvas(image.displayWidth,image.displayHeight);const x=c.getContext('2d',{willReadFrequently:true});x.drawImage(image,0,0);
            const id=x.getImageData(0,0,c.width,c.height).data;let h='';for(let k=0;k<id.length;k++)h+=(id[k]+256).toString(16).slice(1);out.push(h);image.close();}
          return out;}""", b)
    br.close()
open(sys.argv[1], 'w').write(json.dumps(res))
