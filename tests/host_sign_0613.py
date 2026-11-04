"""v0.5.0.0: Logik der Tafelverwaltung (Adresse prüfen, Namen säubern, Suchtreffer sammeln/sortieren/als JSON ausgeben,
Scan-Lauf) mit dem echten Sketch-Code, BLE-Klassen als Attrappen. Signaturen der echten Bibliothek: Annahme."""
from pathlib import Path
import subprocess, tempfile
ROOT=Path(__file__).resolve().parents[1]
ino=next((ROOT/'firmware').glob('RavestickBoard_v*/*.ino')).read_text(encoding='utf8')
a=ino.index('// ---- v0.5.0.0: Tafelverwaltung (Logik)'); b=ino.index('// ---- v0.5.0.0: Tafelverwaltung (Ende Logik)')
src=(ROOT/'tests/host/scan_stub_0613.h').read_text()+ino[a:b]+(ROOT/'tests/host/sign_main_0613.cpp').read_text()
with tempfile.TemporaryDirectory() as d:
    (Path(d)/'t.cpp').write_text(src)
    subprocess.run(['g++','-std=c++17','-Wall','-Wno-unused','-Wno-volatile','-o',str(Path(d)/'t'),str(Path(d)/'t.cpp')],check=True)
    r=subprocess.run([str(Path(d)/'t')],capture_output=True,text=True,check=True)
print(r.stdout.strip().splitlines()[-1].replace('PASS sign host','PASS Tafelverwaltung-Logik (echter Sketch-Code, BLE-Attrappen): Adresse, Name, Treffer, Reihenfolge, Überlauf'))
