#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include "util/TArray.h"
#include "util/TByte.h"
#include "util/TOpt.h"
#include "util/TFileManip.h"
#include "util/TStringConversion.h"
#include "util/TPngConversion.h"
#include "md/MdVram.h"
#include "md/MdPalette.h"
#include "md/MdTilemap.h"
#include <iostream>

using namespace std;
using namespace BlackT;
using namespace Md;

int main(int argc, char* argv[]) {
  
  // Input:
  // * input tiles
  // * input tilemap
  // * output graphic filename
  //
  // options:
  // * palette (default: grayscale)
  // * VRAM load position (default: 0)
  // * output palette preview?
  
  int vramLoadPos = 0;
  bool hasPalette = false;
  MdPalette palette;
  
  if (argc < 6) {
    cout << "Utility to convert Megadrive tiles and maps to graphics" << endl;
    cout << "Usage: " << argv[0] << " <tiles> <tilemap> <tilew> <tileh>"
      " <outfile> [options]"
      << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << "  -p   Input palette file (default: grayscale)" << endl;
    cout << "  -v   Tile VRAM load position (default: "
      << vramLoadPos << ")" << endl;
    
    return 0;
  }
  
  char* tilesName = argv[1];
  char* tilemapName = argv[2];
  int tileW = TStringConversion::stringToInt(string(argv[3]));
  int tileH = TStringConversion::stringToInt(string(argv[4]));
  char* outfileName = argv[5];
  
  // Read arguments
  
  char* paletteFileName = TOpt::getOpt(argc, argv, "-p");
  if (paletteFileName != NULL) {
    hasPalette = true;
    TArray<TByte> rawpal;
    TFileManip::readEntireFile(rawpal, paletteFileName);
    palette = MdPalette(rawpal.data());
  }
  
  TOpt::readNumericOpt(argc, argv, "-v", &vramLoadPos);
  
  // Set up VRAM
  MdVram vram;
  {
    TArray<TByte> rawTiles;
    TFileManip::readEntireFile(rawTiles, tilesName);
    vram.read((char*)rawTiles.data(),
              rawTiles.size() / MdPattern::uncompressedSize,
              vramLoadPos);
  }
  
  // Read tilemap
  MdTilemap tilemap;
  {
    TArray<TByte> rawTilemap;
    TFileManip::readEntireFile(rawTilemap, tilemapName);
    tilemap.read((char*)rawTilemap.data(), tileW, tileH);
  }
  
//  cout << tilemap.tileIds.w() << " " << tilemap.tileIds.h() << endl;
  
  // Convert to graphic
  TGraphic g;
  if (hasPalette) {
    tilemap.toColorGraphic(g, vram, palette);
  }
  else {
    tilemap.toGrayscaleGraphic(g, vram);
  }
  
  // Save result
  TPngConversion::graphicToRGBAPng(string(outfileName), g);
  
  return 0;
}
