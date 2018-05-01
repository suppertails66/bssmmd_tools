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
    cout << "Bishoujo Senshi Sailor Moon eyecatch logo renderer" << endl;
    cout << "Usage: " << argv[0] << " <infile> <paletteline> <outfile>"
      << endl;
    
    return 0;
  }
  
  TIfstream ifs(argv[1], ios_base::binary);
  
  TArray<TByte> data;
  data.resize(ifs.size());
  ifs.read((char*)data.data(), data.size());
  
  TIfstream palifs(argv[2], ios_base::binary);
  
  TArray<TByte> paldata;
  paldata.resize(palifs.size());
  palifs.read((char*)paldata.data(), paldata.size());
  
  MdPaletteLine palLine = MdPaletteLine(paldata.data());
  
  int numTiles = data.size() / MdPattern::uncompressedSize;
  int tilesPerSprite = 16;
  int numSprites = numTiles / 16;
  
  TGraphic output(numSprites * 32, 32);
  
  int pos = 0;
  for (int i = 0; i < numSprites; i++) {
    TGraphic sprite(32, 32);
    sprite.clearTransparent();
    
    for (int j = 0; j < 4; j++) {
      for (int k = 0; k < 4; k++) {
        MdPattern pattern;
        pattern.read((char*)(data.data() + pos));
        pos += MdPattern::uncompressedSize;
        
        pattern.toColorGraphic(sprite, palLine,
                               j * MdPattern::w, k * MdPattern::h);
      }
    }
    
    output.copy(sprite,
                TRect(i * 32, 0, 0, 0),
                TRect(0, 0, 0, 0));
  }
  
  TPngConversion::graphicToRGBAPng(string(argv[3]), output);
  
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
