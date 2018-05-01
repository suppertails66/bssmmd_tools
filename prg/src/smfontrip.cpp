#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TGraphic.h"
#include "util/TPngConversion.h"
#include "util/TArray.h"
#include "util/TByte.h"
#include "util/TStringConversion.h"
#include "md/Md.h"
#include "sm/SmFont.h"
#include <iostream>

using namespace std;
using namespace BlackT;
using namespace Md;

int main(int argc, char* argv[]) {
  if (argc < 4) {
    cout << "Font extractor for Bishoujo Senshi Sailor Moon (MD)" << endl;
    cout << "Usage: " << argv[0] << " <rom> <offset> <fontsize>"
      " <palette> [prefix]"
      << endl;
    
    return 0;
  }
  
  int offset = TStringConversion::stringToInt(argv[2]);
  int fontsize = TStringConversion::stringToInt(argv[3]);
  
  string prefix;
  if (argc >= 6) prefix = string(argv[5]);
  
  TIfstream palifs(argv[4], ios_base::binary);
  TArray<TByte> paldata;
  paldata.resize(palifs.size());
  palifs.read((char*)(paldata.data()), paldata.size());
  palifs.close();
  MdPaletteLine pal(paldata.data());
  
  TIfstream ifs(argv[1], ios_base::binary);
  ifs.seek(offset);
  TArray<TByte> data;
  data.resize(fontsize);
  ifs.read((char*)(data.data()), fontsize);
  ifs.close();
  
  SmFont font;
  
  // 4 tiles per character
  int numFontChars = fontsize / (MdPattern::uncompressedSize) / 4;
  int pos = 0;
  for (int i = 0; i < numFontChars; i++) {
    MdPattern tl, bl, tr, br;
    
    tl.read((char*)(data.data() + pos + (MdPattern::uncompressedSize * 0)));
    bl.read((char*)(data.data() + pos + (MdPattern::uncompressedSize * 1)));
    tr.read((char*)(data.data() + pos + (MdPattern::uncompressedSize * 2)));
    br.read((char*)(data.data() + pos + (MdPattern::uncompressedSize * 3)));
    
    TGraphic grp;
    grp.resize(MdPattern::w * 2, MdPattern::h * 2);
    grp.clearTransparent();
    
    tl.toColorGraphic(grp, pal, 0, 0);
    bl.toColorGraphic(grp, pal, 0, MdPattern::h);
    tr.toColorGraphic(grp, pal, MdPattern::w, 0);
    br.toColorGraphic(grp, pal, MdPattern::w, MdPattern::h);
    
    SmFontEntry entry { grp, 16, 16};
    font.addCharacter(entry);
    
//    string dst = prefix + TStringConversion::intToString(i) + ".png";
//    TPngConversion::graphicToRGBAPng(dst, grp);
    
    pos += MdPattern::uncompressedSize * 4;
  }
  
  font.save(prefix);
  
  return 0;
}

