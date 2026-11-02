// --- driver ---------------------------------------------------------------------------
#include <map>
static uint32_t topFor(uint8_t l){ return l==0?1:l==2?4:0; }
typedef std::vector<std::vector<uint8_t>> Grid;
static Grid unpack(const uint8_t*planes,uint16_t W){
  Grid g(16,std::vector<uint8_t>(W,0)); const uint16_t plane=W*2;
  for(uint16_t x=0;x<W;++x)for(uint8_t y=0;y<16;++y){
    const uint16_t off=x*2+y/8; const uint8_t mask=0x80>>(y&7); uint8_t c=0;
    if(planes[off]&mask)c|=4; if(planes[plane+off]&mask)c|=2; if(planes[plane*2+off]&mask)c|=1; g[y][x]=c; }
  return g; }
// full buffer path (drawInlineUtf8) – as queueText/renderText
static Grid renderFull(const String&a,const String*b,uint8_t layout,uint8_t c1,uint8_t c2,uint16_t&W){
  pendingLayout=layout;
  TextMeasure m1=measureText(a,layout), m2=b?measureText(*b,1):TextMeasure{0,0};
  uint32_t cols=max(m1.pixels,m2.pixels); W=cols<SIGN_WIDTH?SIGN_WIDTH:(uint16_t)cols;
  std::vector<uint8_t> buf(W*6,0); pixelData=buf.data(); streamDrawingTile=false;
  if(layout==1){ drawInlineUtf8(a,1,0,0,c1,W); if(b)drawInlineUtf8(*b,1,0,8,c2,W);} 
  else drawInlineUtf8(a,layout,1,topFor(layout),c1,W);
  Grid g=unpack(buf.data(),W); pixelData=nullptr; return g; }
// streaming path: prepareStreamText + copyPayloadSlice over the whole payload, in odd-sized chunks
static Grid renderStream(const String&a,const String*b,uint8_t layout,uint8_t c1,uint8_t c2,uint16_t&W,size_t chunk=20){
  pendingLayout=layout;
  TextMeasure m1=measureText(a,layout), m2=b?measureText(*b,1):TextMeasure{0,0};
  uint32_t cols=max(m1.pixels,m2.pixels); W=cols<SIGN_WIDTH?SIGN_WIDTH:(uint16_t)cols;
  size_t total=prepareStreamText(a,b,layout,c1,c2,W,m1.chars,m2.chars,m1.pixels,m2.pixels);
  std::vector<uint8_t> payload(total); for(size_t o=0;o<total;o+=chunk){ size_t n=min(chunk,total-o); copyPayloadSlice(o,n,payload.data()+o);} 
  streamPayload=false; return unpack(payload.data()+IMAGE_HEADER_BYTES,W); }
static void dump(const char*name,const Grid&g,uint16_t W){
  printf("## %s W=%u\n",name,W);
  for(int y=0;y<16;++y){ for(uint16_t x=0;x<W;++x){ uint8_t c=g[y][x]; putchar(c?'0'+c:'.'); } putchar('\n'); } }
static std::string toUtf8(uint32_t cp){ std::string s; if(cp<0x80)s+=char(cp); else if(cp<0x800){s+=char(0xC0|cp>>6);s+=char(0x80|(cp&63));} else if(cp<0x10000){s+=char(0xE0|cp>>12);s+=char(0x80|((cp>>6)&63));s+=char(0x80|(cp&63));} else {s+=char(0xF0|cp>>18);s+=char(0x80|((cp>>12)&63));s+=char(0x80|((cp>>6)&63));s+=char(0x80|(cp&63));} return s; }
static std::string M(uint32_t cp){ return toUtf8(cp); }
int main(int argc,char**argv){
  const char*mode=argc>1?argv[1]:"plain";
  if(!strcmp(mode,"plain")){
    // regression battery: text WITHOUT marks must be identical in old and new code
    const char*texts[]={"Hi Raver, Party!","RAVESTICK BAR","Ärger ÖL Üß äöü straße","Hallo \xE2\x9D\xA4\xEF\xB8\x8F Du \xF0\x9F\x92\x9B","1234567890 !?.,:-+/'()%&#=;_",
      "LIVE DJ 22 \xF0\x9F\x8D\x8C\xEF\xB8\x8E","Ein sehr langer Lauftext mit vielen Zeichen damit das Streaming greift und mehrere Tiles entstehen, Hallo Welt! 0123456789"};
    for(const char*t:texts)for(uint8_t layout=0;layout<=4;++layout){
      uint16_t W; String s(t); String s2("Zeile zwei");
      Grid g=renderFull(s,layout==1?&s2:nullptr,layout,4,6,W); char nm[64]; snprintf(nm,64,"full L%u %.12s",layout,t); dump(nm,g,W);
      Grid h=renderStream(s,layout==1?&s2:nullptr,layout,4,6,W); if(h!=g){printf("!! STREAM MISMATCH L%u %s\n",layout,t);} }
    return 0; }


  if(!strcmp(mode,"render")){   // render <layout> <c1> <c2> <hex text1> <hex text2>  -> grid rows (digits), first 64 columns plus width
    auto unhex=[&](const char*h){ std::string o; for(size_t i=0;h[i]&&h[i+1];i+=2){ unsigned v; sscanf(h+i,"%2x",&v); o+=char(v);} return String(o); };
    uint8_t layout=atoi(argv[2]),c1=atoi(argv[3]),c2=atoi(argv[4]); String a=unhex(argv[5]),b=unhex(argv[6]); uint16_t W;
    Grid g=renderFull(a,layout==1?&b:nullptr,layout,c1,c2,W); Grid h=renderStream(a,layout==1?&b:nullptr,layout,c1,c2,W);
    if(g!=h){ printf("STREAM-MISMATCH\n"); return 2; }
    printf("W=%u\n",W); for(int y=0;y<16;++y){ for(uint16_t x=0;x<min<uint16_t>(W,64);++x){ uint8_t c=g[y][x]; putchar(c?'0'+c:'.'); } putchar('\n'); }
    return 0; }
  if(!strcmp(mode,"styled")){
    int fails=0;
#define CHECK(c,msg) do{ if(!(c)){ printf("FAIL: %s (line %d)\n",msg,__LINE__); ++fails; } }while(0)
    auto colorsInCols=[&](const Grid&g,int x0,int x1){ std::map<int,int> m; for(int y=0;y<16;++y)for(int x=x0;x<x1&&x<(int)g[0].size();++x) if(g[y][x]) m[g[y][x]]++; return m; };
    auto rowsUsed=[&](const Grid&g,int x0,int x1,int&top,int&bot){ top=99;bot=-1; for(int y=0;y<16;++y)for(int x=x0;x<x1&&x<(int)g[0].size();++x) if(g[y][x]){ if(y<top)top=y; if(y>bot)bot=y; } };
    // 1) colour marks: layout 3, base 4 (red). "A" red, "B" magenta(5), "C" red again via reset, "D" cyan(3)
    {
      String t = String("A")+String(M(0x95))+"B"+String(M(0x90))+"C"+String(M(0x93))+"D";
      uint16_t W; Grid g=renderFull(t,nullptr,3,4,4,W); dump("colors L3",g,W);
      // each Original glyph has its own width; measure widths via tokenWidth
      int wA=tokenWidth('A',3),wB=tokenWidth('B',3),wC=tokenWidth('C',3),wD=tokenWidth('D',3);
      int total=wA+1+wB+1+wC+1+wD; int x0=(W-total)/2; // centered, W=64 when short
      auto cA=colorsInCols(g,x0,x0+wA), cB=colorsInCols(g,x0+wA+1,x0+wA+1+wB), cC=colorsInCols(g,x0+wA+wB+2,x0+wA+wB+2+wC), cD=colorsInCols(g,x0+wA+wB+wC+3,x0+total);
      CHECK(cA.size()==1&&cA.count(4),"A is red"); CHECK(cB.size()==1&&cB.count(5),"B is magenta"); CHECK(cC.size()==1&&cC.count(4),"C back to base red"); CHECK(cD.size()==1&&cD.count(3),"D is cyan");
      TextMeasure m=measureText(t,3), mp=measureText(String("ABCD"),3);
      CHECK(m.chars==4&&m.pixels==mp.pixels,"marks have no width / no char count");
      Grid h=renderStream(t,nullptr,3,4,6,W); CHECK(h==g,"stream==full colors");
    }
    // 2) font marks: base Normal(3): "LI" Normal, "VE" Mini(2) (code 0x9B), then back with 0x9C, "X" Fett(0x99)
    {
      String t = String("LI")+String(M(0x9B))+"ve"+String(M(0x9C))+"A"+String(M(0x99))+"B";
      uint16_t W; Grid g=renderFull(t,nullptr,3,7,7,W); dump("fonts L3",g,W);
      int wL=tokenWidth('L',3),wI=tokenWidth('I',3),wV=tokenWidth('V',2),wE=tokenWidth('E',2),wA=tokenWidth('A',3),wB=tokenWidth('B',4);
      int total=wL+1+wI+1+wV+1+wE+1+wA+1+wB; int x0=(W-total)/2;
      int top,bot;
      rowsUsed(g,x0,x0+wL+wI+2,top,bot); CHECK(top==2&&bot==13,"Normal glyphs rows 2..13");
      int mx0=x0+wL+wI+2; rowsUsed(g,mx0,mx0+wV+wE+1,top,bot); CHECK(top==4&&bot==10,"Mini glyphs centered rows 4..10");
      CHECK(total==measureText(t,3).pixels,"measureText equals per-char sum");
      Grid h=renderStream(t,nullptr,3,7,6,W); CHECK(h==g,"stream==full fonts");
    }
    // 3) Engschrift char inside Mini-single base: rows 1..14
    {
      String t = String("AB")+String(M(0x9A))+"CD";
      uint16_t W; Grid g=renderFull(t,nullptr,2,6,6,W); dump("eng in mini",g,W);
      int total=measureText(t,2).pixels, x0=(W-total)/2, top,bot;
      int wA=tokenWidth('A',2),wB=tokenWidth('B',2),wC=tokenWidth('C',0),wD=tokenWidth('D',0);
      rowsUsed(g,x0,x0+wA+wB+1,top,bot); CHECK(top==4&&bot==10,"base Mini rows 4..10");
      int ex=x0+wA+wB+2; rowsUsed(g,ex,ex+wC+wD+1,top,bot); CHECK(top==1&&bot==14,"Engschrift rows 1..14");
    }
    // 4) Zweizeilig Mini: font marks ignored, colour marks apply
    {
      String a=String("AB")+String(M(0x98))+"CD", ap("ABCD");
      String b=String(M(0x93))+"XY"; String bp("XY");
      uint16_t W,W2; Grid g=renderFull(a,&b,1,4,2,W); Grid p=renderFull(ap,&bp,1,4,2,W2);
      CHECK(W==W2,"same width");
      // top line identical to plain; bottom line colour changed 2 -> 3 only
      bool topSame=true; for(int y=0;y<8;++y) if(g[y]!=p[y]) topSame=false; CHECK(topSame,"line 1 unchanged (font mark ignored)");
      auto cb=colorsInCols(g,0,W); CHECK(cb.count(3)&&!cb.count(2),"line 2 recoloured to cyan");
      CHECK(measureText(a,1).pixels==measureText(ap,1).pixels,"font mark no width in two-line");
      Grid h=renderStream(a,&b,1,4,2,W); CHECK(h==g,"stream==full two-line");
    }
    // 5) symbols keep own colours (heart is recoloured by its code, not by marks); marks around it do not matter
    {
      String h1=String(M(0x2764)); String t=String(M(0x92))+h1+String(M(0x95))+"A";
      uint16_t W,W2; Grid g=renderFull(t,nullptr,3,4,4,W); Grid p=renderFull(h1+"A",nullptr,3,4,4,W2);
      auto c=colorsInCols(g,0,W); CHECK(c.count(4)&&c.count(5)&&c.size()==2,"heart red, A magenta, nothing else");
      bool heartSame=true; for(int y=0;y<16;++y)for(int x=0;x<20;++x) if(g[y][x]!=p[y][x]&&g[y][x]!=5&&p[y][x]!=4) heartSame=false; CHECK(heartSame,"heart pixels unchanged");
    }
    // 6) streaming across several tiles with many switches
    {
      std::string long_; for(int i=0;i<40;++i){ long_+=M(0x91+(i%7)); if(i%5==0) long_+=M(0x98+(i%4)); long_+="Ab "+std::to_string(i%10); }
      String t(long_); uint16_t W,W2; Grid g=renderFull(t,nullptr,3,4,4,W); Grid h=renderStream(t,nullptr,3,4,6,W2,37);
      CHECK(W==W2&&h==g,"stream==full long mixed"); CHECK(W>64*3,"long text spans many tiles");
      Grid h2=renderStream(t,nullptr,3,4,6,W2,1); CHECK(h2==g,"stream==full (1-byte chunks)");
    }
    // 7) decodeText ignores marks (block pages)
    {
      uint32_t out[48]; int n=decodeText(String("A")+String(M(0x93))+"B"+String(M(0x9C)),out); CHECK(n==2&&out[0]=='A'&&out[1]=='B',"decodeText skips marks");
    }
    // 8) a mark-only/space joiner used by programs keeps item 2 clean: base reset restores colour AND font
    {
      String a=String(M(0x95))+String(M(0x9B))+"AB"; String joined=a+String("\xC2\x90\xC2\x9C     ")+String("CD");
      uint16_t W; Grid g=renderFull(joined,nullptr,3,4,4,W);
      int total=measureText(joined,3).pixels; int x0=(total<64?(64-total)/2:0);
      int wA=tokenWidth('A',2),wB=tokenWidth('B',2),wC=tokenWidth('C',3);
      int cx=x0+wA+wB+2+5*5; // five normal spaces (5px + gap 1 each)
      cx = x0+wA+1+wB+1 + 5*(5+1);
      auto cC=colorsInCols(g,cx,cx+wC); CHECK(cC.size()==1&&cC.count(4),"item 2 starts with base colour");
      int top,bot; rowsUsed(g,cx,cx+wC,top,bot); CHECK(top==2&&bot==13,"item 2 starts with base font (not Mini)");
    }
    printf(fails?"STYLED: %d FAIL\n":"STYLED: ALL PASS\n",fails);
    return fails?1:0; }
  return 0; }
