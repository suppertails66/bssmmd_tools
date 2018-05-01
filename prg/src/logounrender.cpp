#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TStringConversion.h"
#include "util/TArray.h"
#include "util/TByte.h"
#include "util/TGraphic.h"
#include "util/TPngConversion.h"
#include "md/MdPattern.h"
#include "md/MdPaletteLine.h"
#include <iostream>

using namespace std;
using namespace BlackT;
using namespace Md;

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "Bishoujo Senshi Sailor Moon eyecatch logo unrenderer" << endl;
    cout << "Usage: " << argv[0] << " <infile> <paletteline> <outfile>"
      << endl;
    
    return 0;
  }
  
  TGraphic grp;
  TPngConversion::RGBAPngToGraphic(string(argv[1]), grp);
  
  TIfstream palifs(argv[2], ios_base::binary);
  
  TArray<TByte> paldata;
  paldata.resize(palifs.size());
  palifs.read((char*)paldata.data(), paldata.size());
  
  MdPaletteLine palLine = MdPaletteLine(paldata.data());
  
  int numSprites = grp.w() / 32;
  
  TOfstream ofs(argv[3], ios_base::binary);
  
  for (int i = 0; i < numSprites; i++) {
    for (int j = 0; j < 4; j++) {
      for (int k = 0; k < 4; k++) {
        MdPattern pattern;
        int result = pattern.fromColorGraphic(grp, palLine,
                               (j * MdPattern::w) + (i * 32),
                               k * MdPattern::h);
        if (result != 0) {
          cerr << "Error: failed conversion at ("
            << j << ", " << k << ")" << endl;
          return -1;
        }
        
        char buffer[MdPattern::uncompressedSize];
        pattern.write(buffer);
        ofs.write(buffer, MdPattern::uncompressedSize);
      }
    }
  }
  
//  TOfstream ofs(argv[2], ios_base::binary);
//  ofs.write((char*)data.data(), data.size());
  
/*  cout << "[Tilemap00]" << endl;
  cout << "source=" << endl;
  cout << "dest=" << endl;
  cout << "priority=0" << endl;
  cout << "useWidthFormat=1" << endl;
  cout << endl; */
  
  return 0;
}
