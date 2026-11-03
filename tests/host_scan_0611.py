"""v0.4.5.1: Entscheidungslogik des Vor-Scans (bleScanForSign) mit dem echten Sketch-Code, BLE-Klassen als Attrappen.
Prueft: Treffer bricht sofort ab, fremde Geraete = "nicht gesehen" (volle Zeit), nichts gehoert = "ohne Aussage".
Die echte Bibliothek (Arduino-Core 3.3.x) ist hier nicht verfuegbar: Signaturen der BLE-Klassen sind eine Annahme."""
from pathlib import Path
import subprocess, tempfile
ROOT=Path(__file__).resolve().parents[1]
ino=next((ROOT/'firmware').glob('RavestickBoard_v*/*.ino')).read_text(encoding='utf8')
a=ino.index('#if RB_PRESCAN\n// ---- v0.4.5.1: Vor-Scan'); b=ino.index('#endif',a)+6
src=(ROOT/'tests/host/scan_stub_0611.h').read_text()+ino[a:b]+(ROOT/'tests/host/scan_main_0611.cpp').read_text()
with tempfile.TemporaryDirectory() as d:
    (Path(d)/'t.cpp').write_text(src)
    subprocess.run(['g++','-std=c++17','-Wall','-Wno-unused','-o',str(Path(d)/'t'),str(Path(d)/'t.cpp')],check=True)
    r=subprocess.run([str(Path(d)/'t')],capture_output=True,text=True,check=True)
print(r.stdout.strip().splitlines()[-1].replace('PASS scan host','PASS Vor-Scan-Logik (echter Sketch-Code, BLE-Attrappen): gesehen/nicht gesehen/ohne Aussage'))
