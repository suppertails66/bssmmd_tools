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
#include "util/TThingyTable.h"
#include "util/TCharFmt.h"
#include "util/TCsv.h"
#include "md/MdPattern.h"
#include "md/MdPalette.h"
#include "md/MdPaletteLine.h"
#include "sm/SmScript.h"
#include "sm/SmFont.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;
using namespace BlackT;
using namespace Md;
  
int intFromIss(std::istringstream& iss) {
  std::string str;
  iss >> str;
  return TStringConversion::stringToInt(str);
}

int main(int argc, char* argv[]) {
  if (argc < 5) {
    cout << "Bishoujo Senshi Sailor Moon (MD) text script generator" << endl;
    cout << "Usage: " << argv[0] << " <infile> <outprefix> <thingy> <font>"
//      << " [options]"
      << endl;
    
//    cout << "Options: " << endl;
//    cout << "  -o   " << "Set starting offset" << endl;
//    cout << "  -t   " << "Generate 'translator's copy'" << endl;
//    cout << "  -p   " << "Specify script pointer" << endl;
    
    return 0;
  }
  
  char* infile = argv[1];
  char* outprefix = argv[2];
  char* thingyname = argv[3];
  char* fontprefix = argv[4];
  
  TThingyTable thingy;
  thingy.readUtf8(string(thingyname));
  
  SmFont font;
  font.load(string(fontprefix));
  
  TIfstream ifs(infile, ios_base::binary);
  TCsv csv;
  csv.readUtf8(ifs);
  
  int result = 0;
  
  int pos = 0;
  while (pos < csv.numRows()) {
//    istringstream iss(csv.cell(1, pos));
//    int origOffset = intFromIss(iss);
//    int origSize = intFromIss(iss);
    
    istringstream iss(csv.cell(1, pos));
    intFromIss(iss);
    intFromIss(iss);
    intFromIss(iss);
    intFromIss(iss);
    intFromIss(iss);
    int numPointers = intFromIss(iss);
    std::string pointersString;
    for (int i = 0; i < numPointers; i++) {
      pointersString += TStringConversion::intToString(
        intFromIss(iss), TStringConversion::baseHex);
      if (i < numPointers - 1) pointersString += " ";
    }
  
    SmScript script;
    int subresult = script.readCsv(csv, &pos, font, thingy);
    
    if (subresult != 0) result = subresult;
    
    int origOffset = script.offset_;
    int origSize = script.origSize_;
    
    std::string offsetString
      = TStringConversion::intToString(origOffset,
            TStringConversion::baseHex);
    std::string sizeString
      = TStringConversion::intToString(origSize,
            TStringConversion::baseHex);
    
    std::string outfilename = std::string(outprefix)
      + offsetString
      + ".bin";
    TOfstream ofs(outfilename.c_str(), ios_base::binary);
    script.write(ofs);
    
    std::cout << "[Script_" << offsetString << "]" << std::endl;
    std::cout << "source=" << outfilename << std::endl;
    std::cout << "origPos=" << offsetString << std::endl;
    std::cout << "origSize=" << sizeString << std::endl;
    std::cout << "pointers=" << pointersString << std::endl;
    std::cout << std::endl;
  }
  
  return result;
}
