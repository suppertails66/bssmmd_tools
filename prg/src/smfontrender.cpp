#include "sm/SmFont.h"
#include "sm/SmFontCalc.h"
#include "util/TIfstream.h"
#include "util/TPngConversion.h"
#include "util/TThingyTable.h"
#include "util/TCharFmt.h"
#include "util/TFileManip.h"
#include <iostream>

using namespace std;
using namespace BlackT;
using namespace Md;

int main(int argc, char* argv[]) {
  if (argc < 5) {
    cout << "Bishoujo Senshi Sailor Moon (MD) font renderer" << endl;
    cout << "Usage: " << argv[0] << " <infont> <thingy> <intext>"
      << " <outfile>" << endl;
    
    return 0;
  }
  
  char* infileName = argv[1];
  char* thingyName = argv[2];
  char* intextName = argv[3];
  char* outfileName = argv[4];
  
  SmFont font;
  font.load(string(infileName));
  
  TThingyTable thingy;
  thingy.readUtf8(string(thingyName));
  
  TUtf32Chars input;
  {
    string strInput;
    {
      TArray<TByte> rawInput;
      TFileManip::readEntireFile(rawInput, string(intextName));
      for (int i = 0; i < rawInput.size(); i++) {
        strInput += (char)rawInput[i];
      }
    }
    
    TCharFmt::utf8To32(strInput, input);
  }
  
  TGraphic dst;
  dst.resize(SmFontCalc::getWordLength(input, font, thingy),
             16);
  dst.clearTransparent();
  
  int x = 0;
  for (int i = 0; i < input.size(); i++) {
    const SmFontEntry& entry = font.entries[thingy.getRevEntry(input[i])];
    dst.copy(entry.graphic,
             TRect(x, 0, 0, 0),
             TRect(0, 0, 0, 0));
    x += entry.advanceWidth;
  }
  
  TPngConversion::graphicToRGBAPng(string(outfileName), dst);
  
  return 0;
}
