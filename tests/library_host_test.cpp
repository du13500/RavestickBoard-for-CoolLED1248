#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <algorithm>
using namespace std;
class String {
public:
 string value;
 String() {} String(const char *x):value(x?x:""){} String(const string &x):value(x){} String(char x):value(1,x){} String(int x):value(to_string(x)){} String(unsigned int x):value(to_string(x)){} String(long x):value(to_string(x)){} String(unsigned long x):value(to_string(x)){}
 size_t length() const{return value.length();} bool isEmpty()const{return value.empty();}const char *c_str()const{return value.c_str();} char operator[](size_t n)const{return value[n];}
 int toInt()const{try{return stoi(value);}catch(...){return 0;}};
 String&operator+=(const String &b){value+=b.value;return *this;} String&operator+=(const char*b){value+=b;return *this;}
 bool operator==(const char*b)const{return value==b;} bool operator!=(const char*b)const{return value!=b;}
 operator string()const{return value;}
};
inline String operator+(String a,const String &b){a.value+=b.value;return a;}
inline String operator+(String a,const char*b){a.value+=b;return a;}
inline ostream&operator<<(ostream&o,const String &s){return o<<s.value;}
class File {
 shared_ptr<fstream> fp;
 size_t fileSize=0;
public:
 File()=default;
 File(const filesystem::path &path,const string &mode){ios::openmode flags=ios::binary;if(mode=="r") flags|=ios::in;else flags|=ios::out|ios::trunc;fp=make_shared<fstream>(path,flags);if(*fp){fileSize=filesystem::file_size(path);}}
 explicit operator bool()const{return fp&&*fp;}
 size_t size()const{return fileSize;}
 size_t readBytes(char*buf,size_t n){fp->read(buf,n);return fp->gcount();}
 size_t read(uint8_t*buf,size_t n){return readBytes((char*)buf,n);}
 size_t write(const uint8_t*p,size_t n){fp->write((const char*)p,n);return *fp?n:0;}
 void flush(){if(fp)fp->flush();}
 void close(){if(fp)fp->close();}
};
struct FakeFS {
 bool valid=false; filesystem::path root="/tmp/rb041_library_host";
 filesystem::path real(const String &path){return root / filesystem::path(path.value).relative_path();}
 bool begin(bool){return valid;}
 bool format(){filesystem::remove_all(root);filesystem::create_directories(root);valid=true;return true;}
 bool exists(const String &path){return filesystem::exists(real(path));}
 bool exists(const char *path){return exists(String(path));}
 bool remove(const String &path){return filesystem::remove(real(path));}
 bool remove(const char *path){return remove(String(path));}
 bool rename(const String &a,const String &b){try{filesystem::rename(real(a),real(b));return true;}catch(...){return false;}}
 bool mkdir(const char *path){return filesystem::create_directories(real(path));}
 File open(const String &path,const char *mode){return File(real(path),mode);}
 File open(const char *path,const char *mode){return File(real(path),mode);}
 size_t totalBytes(){return 512*1024;}size_t usedBytes(){return 1000;}
} LittleFS;
struct FakeWeb {
 map<string,String> args;
 int status=0;string response;
 bool hasArg(const char*key){return args.count(key)>0;}
 String arg(const char*key){return args[key];}
 void sendHeader(const char*,const char*){}
 void setContentLength(int){}
 void send(int st,const char*,const String &payload){status=st;response=payload.value;}
 void sendContent(const String &part){response+=part.value;}
} web;
#define CONTENT_LENGTH_UNKNOWN -1
void diagnosticEvent(const char*,const String&,bool=false){}
String jsonQuote(const String&s){string t="\"";for(char c:s.value){if(c=='\\'||c=='\"')t+='\\';t+=c;}return String(t+'\"');}
void respond(bool ok,const String&s,int code=200){web.send(code,"json",String("{\"ok\":")+(ok?"true":"false")+",\"message\":"+jsonQuote(s)+"}");}
bool reserveJob(){return true;}
struct TextMeasure{uint32_t chars,pixels;};
TextMeasure measureText(const String&s,uint8_t){return{uint32_t(s.length()),uint32_t(s.length()*6)};}
constexpr uint16_t SIGN_WIDTH=64;constexpr uint32_t IMAGE_HEADER_BYTES=107,BYTES_PER_COLUMN=6,MAX_IMAGE_BYTES=30*1024;
bool autoProgram=false;String pendingText,pendingSecondLine,currentState,currentMessage;
uint8_t pendingLayout=0,pendingColor=7,pendingColor2=7,pendingMode=2;
enum Job:uint8_t{NONE,TEXT};Job nextJob=NONE;
#include "../firmware/RavestickBoard_v0_4_1/RavestickBoard_Library.h"
void clearArgs(){web.args.clear();web.response.clear();web.status=0;}
void put(const char*k,const string&v){web.args[k]=String(v);}
int main(){
 static_assert(offsetof(LibraryRecord,checksum)+sizeof(uint32_t)==sizeof(LibraryRecord),"record checksum last");
 static_assert(offsetof(LibraryMeta,checksum)+sizeof(uint32_t)==sizeof(LibraryMeta),"meta checksum last");
 filesystem::remove_all(LittleFS.root);
 libraryBegin();assert(!libraryReady);
 clearArgs();put("confirm","INIT_LIBRARY_FS");onLibraryInit();assert(libraryReady);assert(web.status==200);
 clearArgs();onLibraryList();assert(web.response.find("\"count\":0")!=string::npos);
 clearArgs();put("name","Hallo 🍌");put("text","RAVESTICK");put("line2","");put("layout","0");put("color1","7");put("color2","6");put("mode","2");put("favorite","1");onLibrarySave();assert(web.status==200);assert(libraryMeta.count==1);uint32_t id=libraryMeta.ids[0];
 clearArgs();put("slot","2");put("id",to_string(id));onLibraryShortcut();assert(web.status==200 && libraryMeta.shortcuts[1]==id);
 clearArgs();put("id",to_string(id));onLibraryGet();assert(web.status==200 && web.response.find("RAVESTICK")!=string::npos);
 clearArgs();put("id",to_string(id));onLibraryDelete();assert(web.status==409);
 clearArgs();put("id",to_string(id));onLibrarySend();assert(web.status==202 && pendingText.value=="RAVESTICK" && pendingColor==7);
 clearArgs();put("slot","2");put("id","0");onLibraryShortcut();assert(web.status==200);
 clearArgs();put("id",to_string(id));onLibraryDelete();assert(web.status==200 && libraryMeta.count==0);
 clearArgs();put("name","NEU");put("text","CODE");put("line2","");put("layout","1");put("color1","6");put("color2","2");put("mode","1");put("favorite","0");onLibrarySave();assert(web.status==200 && libraryMeta.count==1 && libraryMeta.ids[0]!=id);
 uint32_t newId=libraryMeta.ids[0];
 // Simulate power cycle (same filesystem, reload journal and validate stable IDs)
 libraryReady=false;memset(&libraryMeta,0,sizeof(libraryMeta)); libraryBegin();
 assert(libraryReady && libraryMeta.count==1 && libraryMeta.ids[0]==newId);
 assert(libraryLoad(newId));assert(strcmp(libraryItem.name,"NEU")==0);
 // Corrupt the *older* journal: the other slot must still be usable on reboot.
 const char *oldJournal=(libraryMeta.sequence & 1)?"/rb-lib/meta-b.bin":"/rb-lib/meta-a.bin";
 if (LittleFS.exists(oldJournal)) {File corrupt=LittleFS.open(oldJournal,"w");const uint8_t bad[]={0,1,2};corrupt.write(bad,3);corrupt.close();}
 libraryReady=false;libraryBegin();assert(libraryReady);
 assert(libraryMeta.count==1 && libraryMeta.ids[0]==newId);
 // Simulate crash after old version moved to backup but before temp committed
 const String path=libraryPath(newId),bak=libraryPath(newId,".bak");
 assert(LittleFS.rename(path,bak));assert(!LittleFS.exists(path));
 libraryReady=false;libraryBegin();assert(libraryReady&&libraryLoad(newId));
 // Create 3 records and test reorder, including reboot persistence
 for(int i=0;i<3;i++){clearArgs();put("name","Text "+to_string(i));put("text","ABC");put("line2","");put("layout","3");put("color1","7");put("color2","7");put("mode","2");put("favorite","0");onLibrarySave();assert(web.status==200);}
 assert(libraryMeta.count==4);auto last=libraryMeta.ids[3];
 clearArgs();put("id",to_string(last));put("to","0");onLibraryMove();assert(web.status==200 && libraryMeta.ids[0]==last);
 libraryReady=false;libraryBegin();assert(libraryMeta.ids[0]==last);
 clearArgs();onLibraryList();assert(web.response.find("Text 2")!=string::npos);
 cout<<"PASS: first-boot format confirmation, CRUD, flash persistence, stable ids, deletion protection, quick slots, send, reorder, restart recovery, JSON list and schema checksum\n";
}
