#include "v042_program_core.h"
#include <cassert>
#include <iostream>
#include <unordered_map>
using namespace ravestick;
int main() {
 static_assert(kPageBytes==384);
 static_assert(kMaxBandPages>=25);
 std::unordered_map<unsigned,unsigned> size{{1,5},{2,3},{3,25},{4,50},{5,1}};
 auto lookup=[&](uint32_t id,Kind){return size.at(id);};
 auto pg=plan({{1,Kind::Block,1},{2,Kind::Block,1},{3,Kind::Block,1}},lookup);
 assert(pg.size()==1 && pg[0].pages==33);
 pg=plan({{1,Kind::Block,1},{2,Kind::Scroll,2},{3,Kind::Block,1}},lookup);
 assert(pg.size()==3);
 pg=plan({{4,Kind::Block,1},{4,Kind::Block,1}},lookup);
 assert(pg.size()==2 && pg[0].pages==50 && pg[1].pages==50);
 pg=plan({{1,Kind::Block,1},{2,Kind::Block,2}},lookup);
 assert(pg.size()==2);
 auto render=[](unsigned i,uint8_t* out){
  for(unsigned color=0;color<3;color++)
   for(unsigned j=0;j<128;j++)out[color*128+j]=static_cast<uint8_t>(i*10+color);
 };
 std::vector<uint8_t> rgb;
 packBlockBand(25,render,rgb);
 assert(rgb.size()==25*kPageBytes);
 for(unsigned off=0;off<rgb.size();off++) assert(rgb[off]==blockBandByte(25,off,render));
 assert(rgb[0]==0 && rgb[128]==10 && rgb[25*128]==1 && rgb[2*25*128]==2);
 try {packBlockBand(kMaxBandPages+1,render,rgb);assert(false);}catch(const std::length_error&){}
 assert(shortcutValid({Source::Ambience,7,"BLAU"}));
 assert(!shortcutValid({Source::Ambience,8,"Unbekannt"}));
 assert(!shortcutValid({Source::Library,0,"Fehlt"}));
 std::cout << "PASS: band planner, 25 pages, planes, streaming parity, boundary, labels\n";
}
