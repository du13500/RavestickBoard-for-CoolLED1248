
#include <string>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <strings.h>
#include <vector>
#include <cassert>
#define RB_PRESCAN 1
#define RB_PRESCAN_S 3
struct SignRef { char addr[18]; char name[24]; };
static SignRef signMine = {}; static SignRef signActive = {"c0:ff:ee:00:00:01",""};
#include <ctype.h>
struct String{std::string s;String(){}String(const char*c):s(c){}String(unsigned long v):s(std::to_string(v)){}String(unsigned v):s(std::to_string(v)){}String(int v):s(std::to_string(v)){}
 String&operator+=(const String&o){s+=o.s;return *this;}String&operator+=(const char*c){s+=c;return *this;}String&operator+=(char c){s+=c;return *this;}String(const std::string&x):s(x){}String operator+(const char*c)const{return String((s+c).c_str());}String operator+(const String&o)const{return String((s+o.s).c_str());}const char*c_str()const{return s.c_str();}};
static uint32_t fakeMs=1000; static uint32_t millis(){return fakeMs;}
static std::vector<std::string> events; static std::string phase;
struct { uint32_t scanRuns=0,scanSeen=0,scanMissed=0,scanInconclusive=0; } diagnostics;
static void diagnosticEvent(const char*c,const String&d,bool e=false){events.push_back(std::string(c)+": "+d.s);}
static void rbPhase(const char*p){phase=p;}
static int scanLastRssi=0; static uint32_t scanLastMs=0;
struct BLEAddress{std::string a;String toString()const{return String(a.c_str());}};
struct BLEUUID{uint16_t u;BLEUUID(uint16_t x):u(x){}};
struct BLEAdvertisedDevice{std::string addr;int rssi;std::string name="";bool ffo=false;BLEAddress getAddress()const{return BLEAddress{addr};}int getRSSI()const{return rssi;}bool haveName()const{return !name.empty();}String getName()const{return String(name.c_str());}bool haveServiceUUID()const{return ffo;}bool isAdvertisingService(BLEUUID u)const{return ffo&&u.u==0xFFF0;}};
struct BLEAdvertisedDeviceCallbacks{virtual ~BLEAdvertisedDeviceCallbacks(){}virtual void onResult(BLEAdvertisedDevice){}};
struct BLEScan;
struct BLEScan{BLEAdvertisedDeviceCallbacks*cb=nullptr;bool stopped=false;int activeCalls=0;std::vector<BLEAdvertisedDevice> script;uint32_t cost=3000;
 void setAdvertisedDeviceCallbacks(BLEAdvertisedDeviceCallbacks*c){cb=c;}bool active=false;void setActiveScan(bool b){active=b;}void setInterval(int){}void setWindow(int){}
 void start(uint32_t secs){stopped=false;fakeMs+=0;uint32_t spent=0;for(auto&d:script){if(stopped)break;spent+=300;cb->onResult(d);}if(!stopped)spent=secs*1000;fakeMs+=spent;}
 void stop(){stopped=true;}void clearResults(){}};
struct BLEDevice{static BLEScan*getScan(){static BLEScan s;return &s;}};
