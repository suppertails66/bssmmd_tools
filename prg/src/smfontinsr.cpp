#include "sm/SmFont.h"
#include "util/TStringConversion.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include "util/TFileManip.h"
#include "md/MdPattern.h"
#include "md/MdPaletteLine.h"
#include <iostream>

using namespace std;
using namespace BlackT;
using namespace Md;

int main(int argc, char* argv[]) {
  if (argc < 7) {
    cout << "Font inserter for Bishoujo Senshi Sailor Moon (MD)" << endl;
    cout << "Usage: " << argv[0] << " <inrom> <outrom> <inprefix>"
      " <fontoffset> <widthoffset> <palette>" << endl;
    
    return 0;
  }
  
  char* inrom = argv[1];
  char* outrom = argv[2];
  string inprefix(argv[3]);
  int fontoffset = TStringConversion::stringToInt(string(argv[4]));
  int widthoffset = TStringConversion::stringToInt(string(argv[5]));
  char* palette = argv[6];
  
  TIfstream romifs(inrom, ios_base::binary);
  TBufStream rom(0x800000);
  rom.writeFrom(romifs, romifs.size());
  romifs.close();
  
  // someday i'll fix this
  int romsize = rom.tell();
  
  SmFont font;
  font.load(inprefix);
  
  TArray<TByte> rawpal;
  TFileManip::readEntireFile(rawpal, string(palette));
  MdPaletteLine palLine(rawpal.data());
  
  // write new font
  rom.seek(fontoffset);
  for (int i = 0; i < font.entries.size(); i++) {
    TGraphic& grp = font.entries[i].graphic;
    
    MdPattern tl, bl, tr, br;
    
    tl.fromColorGraphic(grp, palLine, 0, 0);
    bl.fromColorGraphic(grp, palLine, 0, 8);
    tr.fromColorGraphic(grp, palLine, 8, 0);
    br.fromColorGraphic(grp, palLine, 8, 8);
    
    char buffer[MdPattern::uncompressedSize];
    
    tl.write(buffer);
    rom.write(buffer, MdPattern::uncompressedSize);
    bl.write(buffer);
    rom.write(buffer, MdPattern::uncompressedSize);
    tr.write(buffer);
    rom.write(buffer, MdPattern::uncompressedSize);
    br.write(buffer);
    rom.write(buffer, MdPattern::uncompressedSize);
  }
  
  // write width table
  rom.seek(widthoffset);
  for (int i = 0; i < font.entries.size(); i++) {
//    rom.writeu8be(font.entries[i].width / 2);
//    rom.writeu8be(font.entries[i].advanceWidth / 2);
//    rom.writeu8be(font.entries[i].advanceWidth / 2);
    rom.writeu8be(font.entries[i].width);
    rom.writeu8be(font.entries[i].width);
  }
//  rom.writeu32be(12345678);
  
  rom.seek(romsize);
  rom.save(outrom);
  
  return 0;
}
