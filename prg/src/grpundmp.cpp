#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include "util/TFolderManip.h"
#include "util/TStringConversion.h"
#include "util/TGraphic.h"
#include "util/TPngConversion.h"
#include "util/TOpt.h"
#include "util/TArray.h"
#include "util/TByte.h"
#include "util/TFileManip.h"
#include "md/MdPattern.h"
#include "md/MdPalette.h"
#include "md/MdPaletteLine.h"
#include <iostream>
#include <string>

using namespace std;
using namespace BlackT;
using namespace Md;

const static int patternsPerRow = 16;

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "Mega Drive-format raw graphics undumper" << endl;
    cout << "Usage: " << argv[0] << " <infile> <outfile> [options]" << endl;
    
    cout << "Options: " << endl;
    cout << "  -p   " << "Set palette line" << endl;
    cout << "  -n   " << "Set number of patterns" << endl;
    
    return 0;
  }
  
  char* infile = argv[1];
  char* outfile = argv[2];
  
  MdPaletteLine palLine;
  bool hasPalLine = false;
  char* palOpt = TOpt::getOpt(argc, argv, "-p");
  if (palOpt != NULL) {
    TArray<TByte> rawpal;
    TFileManip::readEntireFile(rawpal, palOpt);
    palLine = MdPaletteLine(rawpal.data());
    hasPalLine = true;
  }
  
  TGraphic src;
  TPngConversion::RGBAPngToGraphic(string(infile), src);
  
  int numPatterns = -1;
  TOpt::readNumericOpt(argc, argv, "-n", &numPatterns);
  if (numPatterns == -1) {
    numPatterns = (src.w() / MdPattern::w)
      * (src.h() / MdPattern::h);
  }
  
  TOfstream ofs(outfile, ios_base::binary);
  
  int x = 0;
  int y = 0;
  for (int i = 0; i < numPatterns; i++) {
//    int x = (i % patternsPerRow) * MdPattern::w;
//    int y = (i / patternsPerRow) * MdPattern::h;
    
    MdPattern pattern;
    
    int result;
    if (hasPalLine) {
      result = pattern.fromColorGraphic(src, palLine, x, y);
    }
    else {
//      pattern.fromGrayscaleGraphic(src, x, y);
    }
    
    if (result != 0) {
      cerr << "Error: could not convert pattern at ("
        << x << ", " << y << ")" << endl;
      return -1;
    }
    
    x += MdPattern::w;
    if (((x / MdPattern::w) % patternsPerRow) == 0) {
      x = 0;
      y += MdPattern::h;
    }
    
    char buffer[MdPattern::uncompressedSize];
    pattern.write(buffer);
    ofs.write(buffer, MdPattern::uncompressedSize);
  }
  
  return 0;
}
