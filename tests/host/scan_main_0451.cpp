
int main(){
 BLEScan*sc=BLEDevice::getScan();
 // 1) Tafel gesehen (nach zwei fremden Geraeten) -> sofort Abbruch
 sc->script={{"11:22:33:44:55:66",-80},{"aa:bb:cc:dd:ee:ff",-70},{"FF:00:00:03:7E:C3",-55},{"99:99:99:99:99:99",-40}};
 int8_t v=bleScanForSign(); assert(v==1); assert(scanLastMs==900); assert(scanLastRssi==-55); assert(scanAnyAdv==3); // 4. Geraet nach stop() nicht mehr gezaehlt
 // 2) nur fremde Geraete -> nicht gesehen, volle Zeit
 sc->script={{"11:22:33:44:55:66",-80},{"aa:bb:cc:dd:ee:ff",-70}};
 v=bleScanForSign(); assert(v==0); assert(scanLastMs==3000); assert(scanLastRssi==0);
 // 3) gar nichts gehoert -> ohne Aussage
 sc->script={}; v=bleScanForSign(); assert(v==-1);
 assert(diagnostics.scanRuns==3&&diagnostics.scanSeen==1&&diagnostics.scanMissed==1&&diagnostics.scanInconclusive==1);
 for(auto&e:events)puts(e.c_str());
 puts("PASS scan host"); return 0; }
