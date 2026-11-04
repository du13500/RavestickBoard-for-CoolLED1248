
#include <algorithm>
static std::string name24(const char*n){char b[24];signNameClean(n,b,sizeof(b));return b;}
int main(){
 // Adressen pruefen und normalisieren
 char a[18]; assert(signAddrNormalize("C0:FF:EE:00:00:01",a)); assert(!strcmp(a,"c0:ff:ee:00:00:01"));
 assert(!signAddrNormalize("c0:ff:ee:00:00",a)); assert(!signAddrNormalize("c0-ff-ee-00-00-01",a)); assert(!signAddrNormalize("gg:00:00:03:00:01",a)); assert(!signAddrNormalize("",a)); assert(!signAddrNormalize(nullptr,a));
 assert(!signAddrNormalize("c0:ff:ee:00:00:01x",a));
 // Namen saeubern: ASCII, kein " und \, Laengenbegrenzung 23
 assert(name24("CoolLED 1248")=="CoolLED 1248"); assert(name24("a\"b\\c")=="a?b?c"); assert(name24("\xC3\xA4")=="??"); assert(name24(std::string(40,'x').c_str()).size()==23);
 // Treffer sammeln: Tafel am Namen, per Dienst, eigene Tafel ohne Namen; fremde Geraete nur mit all
 BLEScan*sc=BLEDevice::getScan();
 std::snprintf(signMine.addr,18,"%s","c0:ff:ee:00:00:01"); std::snprintf(signMine.name,24,"%s","CoolLED1248");
 sc->script={
  {"11:22:33:44:55:66",-60,""},            // fremd, kein Name
  {"AA:BB:CC:DD:EE:01",-80,"CoolLEDX-AB"}, // Tafel am Namen
  {"aa:bb:cc:dd:ee:02",-50,"",true},       // Tafel per Dienst FFF0
  {"C0:FF:EE:00:00:01",-90,""},            // eigene Tafel, keine Namensangabe
  {"AA:BB:CC:DD:EE:01",-70,"CoolLEDX-AB"}, // doppelt, staerker
  {"kaputt",-10,"x"},                      // ungueltige Adresse: wird nicht aufgenommen
  {"77:77:77:77:77:77",-40,"Kopfhoerer"}
 };
 signScanRun();
 assert(signHeardAll==7); assert(signHitCount==5);
 assert(sc->active==false);                // aktiver Scan wird nach der Suche wieder abgeschaltet
 String j=signHitsJson(false);
 // Reihenfolge: Tafeln zuerst, nach Signal: ee:02 (-50), ee:01 (-70, staerkster Wert), eigene (-90)
 size_t p2=j.s.find("aa:bb:cc:dd:ee:02"),p1=j.s.find("aa:bb:cc:dd:ee:01"),pm=j.s.find("c0:ff:ee:00:00:01");
 assert(p2!=std::string::npos&&p1!=std::string::npos&&pm!=std::string::npos&&p2<p1&&p1<pm);
 assert(j.s.find("11:22:33:44:55:66")==std::string::npos); assert(j.s.find("77:77:77:77:77:77")==std::string::npos);
 assert(j.s.find("\"rssi\":-70")!=std::string::npos); assert(j.s.find("\"rssi\":-80")==std::string::npos);
 assert(j.s.find("\"mine\":true")!=std::string::npos);
 String ja=signHitsJson(true);
 assert(ja.s.find("11:22:33:44:55:66")!=std::string::npos&&ja.s.find("77:77:77:77:77:77")!=std::string::npos);
 // alle: Nicht-Tafeln hinter den Tafeln
 assert(ja.s.find("11:22:33:44:55:66")>ja.s.find("c0:ff:ee:00:00:01"));
 // aktive Tafel wird markiert
 std::snprintf(signActive.addr,18,"%s","aa:bb:cc:dd:ee:01");
 String jb=signHitsJson(false); size_t k=jb.s.find("aa:bb:cc:dd:ee:01"); assert(jb.s.find("\"active\":true",k)!=std::string::npos);
 // Ueberlauf: 12 Plaetze, 14 fremde Geraete verdraengen nichts Wichtiges; eine Tafel verdraengt das schwaechste Fremdgeraet
 sc->script.clear();
 for(int i=0;i<14;++i){char b[18];std::snprintf(b,18,"10:00:00:00:00:%02x",i);sc->script.push_back({b,-50-i,""});}
 sc->script.push_back({"AA:BB:CC:DD:EE:09",-95,"CoolLED"});
 signScanRun(); assert(signHitCount==12);
 String jc=signHitsJson(false); assert(jc.s.find("aa:bb:cc:dd:ee:09")!=std::string::npos);
 String jd=signHitsJson(true); assert(jd.s.find("10:00:00:00:00:0b")==std::string::npos);   // schwächstes Fremdgerät wurde verdrängt
 assert(jd.s.find("10:00:00:00:00:00")!=std::string::npos);        // das staerkste Fremdgeraet bleibt
 puts("PASS sign host"); return 0; }
