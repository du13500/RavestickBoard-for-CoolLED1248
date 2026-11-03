"""v0.4.5.0: Programm-Reihenfolge (rb44ProgramOrder, v044MoveCollection) mit dem echten Code aus RavestickBoard_v042.h
am Rechner (g++), mit kleinen Stubs für String, LittleFS und Webserver. Aufruf: python tests/host_program_order_0602.py"""
import re, subprocess, sys, tempfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
src=(ROOT/'firmware/RavestickBoard_v0_6_0_2/RavestickBoard_v042.h').read_text(encoding='utf8')
a=src.index('static constexpr const char *RB44_PROGRAM_ORDER_FILE'); b=src.index('void v042GetCollection()')
body=src[a:b]
# v042ListCollections gehört dazu, ist aber nur Ausgabe: für den Test genügt die Reihenfolge-Funktion und die Verschiebung
body=body[:body.index('void v042ListCollections()')]+body[body.index('// POST /api/collections/move'):]
cpp=r'''
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <map>
#include <vector>
struct String{std::string s;String(){}String(const char*c):s(c){}String(const std::string&x):s(x){}
 String operator+(const String&o)const{return String(s+o.s);}String operator+(const char*c)const{return String(s+c);}
 String operator+(uint32_t n)const{return String(s+std::to_string(n));}String operator+(int n)const{return String(s+std::to_string(n));}
 int toInt()const{return atoi(s.c_str());}};
static std::map<std::string,std::vector<uint8_t>> disk; static bool failWrite=false;
struct File{std::string path;std::vector<uint8_t> buf;bool wr=false,ok=false;size_t pos=0;
 explicit operator bool()const{return ok;}
 size_t read(uint8_t*d,size_t n){size_t k=std::min(n,buf.size()-pos);memcpy(d,buf.data()+pos,k);pos+=k;return k;}
 size_t write(const uint8_t*d,size_t n){if(failWrite)return 0;buf.assign(d,d+n);return n;}
 void close(){if(wr&&!failWrite)disk[path]=buf;}};
struct FS_{bool exists(const char*p){return disk.count(p);}bool exists(const String&p){return disk.count(p.s);}
 File open(const char*p,const char*m){File f;f.path=p;if(m[0]=='w'){f.wr=true;f.ok=true;}else if(disk.count(p)){f.buf=disk[p];f.ok=true;}return f;}
 bool remove(const String&p){return disk.erase(p.s)>0;}} LittleFS;
static constexpr uint8_t RB42_PROGRAM_LIMIT=24; static bool libraryReady=true;
String rb42CollectionPath(uint32_t id){return String("/rb-lib/p")+id+".bin";}
struct Web{std::map<std::string,std::string> a;String arg(const char*k){return String(a.count(k)?a[k]:"");}} web;
static bool lastOk=false;static int lastCode=0;static std::string lastMsg;
void respond(bool ok,const String&m,int code=200){lastOk=ok;lastCode=code;lastMsg=m.s;}
bool libraryAvailable(){return true;}
uint32_t libraryRequestId(){return web.arg("id").toInt();}
'''+body+r'''
static void prog(int id,bool bak=false){disk[rb42CollectionPath(id).s+(bak?".bak":"")]={1};}
static std::vector<int> order(){uint8_t o[RB42_PROGRAM_LIMIT];uint8_t n=rb44ProgramOrder(o);return std::vector<int>(o,o+n);}
static bool mv(int id,int to){web.a["id"]=std::to_string(id);web.a["to"]=std::to_string(to);v044MoveCollection();return lastOk;}
#define CHECK(c) do{if(!(c)){printf("FAIL line %d: %s\n",__LINE__,#c);return 1;}}while(0)
int main(){
 // ohne Datei: aufsteigende ID
 prog(3);prog(5);prog(7);
 CHECK((order()==std::vector<int>{3,5,7}));
 // verschieben: 5 nach hinten, 7 nach vorn
 CHECK(mv(5,2));CHECK((order()==std::vector<int>{3,7,5}));
 CHECK(mv(7,0));CHECK((order()==std::vector<int>{7,3,5}));
 CHECK(mv(7,0));CHECK(lastOk&&lastMsg.find("unver")!=std::string::npos);        // gleiche Position
 // ungültige Anfragen
 CHECK(!mv(4,0)&&lastCode==400);CHECK(!mv(3,3)&&lastCode==400);CHECK(!mv(3,-1)&&lastCode==400);
 // neues Programm erscheint am Ende, gelöschtes verschwindet von selbst
 prog(1);CHECK((order()==std::vector<int>{7,3,5,1}));
 disk.erase(rb42CollectionPath(3).s);CHECK((order()==std::vector<int>{7,5,1}));
 // Sicherungskopie zählt als vorhanden (wie rb42Read)
 disk.erase(rb42CollectionPath(5).s);disk[rb42CollectionPath(5).s+".bak"]={1};CHECK((order()==std::vector<int>{7,5,1}));
 // beschädigte Ordnungsdatei: doppelte, unbekannte, zu große IDs werden ignoriert, Rest aufsteigend
 disk["/rb-lib/porder.bin"]={7,7,99,0,1};CHECK((order()==std::vector<int>{7,1,5}));
 // leere Ordnungsdatei
 disk["/rb-lib/porder.bin"]={};CHECK((order()==std::vector<int>{1,5,7}));
 // Schreibfehler wird gemeldet, Reihenfolge bleibt
 failWrite=true;CHECK(!mv(7,0)&&lastCode==500);failWrite=false;CHECK((order()==std::vector<int>{1,5,7}));
 // alle 24 Programme
 disk.clear();for(int i=1;i<=24;++i)prog(i);CHECK(order().size()==24);CHECK(mv(24,0));CHECK(order()[0]==24&&order()[1]==1&&order()[23]==23);
 CHECK(mv(1,23));CHECK(order()[23]==1&&order()[0]==24);
 CHECK(disk["/rb-lib/porder.bin"].size()==24);
 printf("ORDER: ALL PASS\n");return 0;}
'''
tmp=Path(tempfile.mkdtemp()); (tmp/'t.cpp').write_text(cpp)
r=subprocess.run(['g++','-std=c++17','-O1','-Wall','-Wno-unused','-o',str(tmp/'t'),str(tmp/'t.cpp')],capture_output=True,text=True)
if r.returncode: print(r.stderr[:2500]); sys.exit(1)
o=subprocess.run([str(tmp/'t')],capture_output=True,text=True).stdout
assert 'ORDER: ALL PASS' in o,o
print('PASS Programm-Reihenfolge (echter Code): Standard aufsteigend, Verschieben, ungültige Anfragen, neu/gelöscht/.bak, kaputte Datei, Schreibfehler, 24 Programme.')
