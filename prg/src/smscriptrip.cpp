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
#include "md/MdPattern.h"
#include "md/MdPalette.h"
#include "md/MdPaletteLine.h"
#include "sm/SmScript.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace BlackT;
using namespace Md;

int main(int argc, char* argv[]) {
  if (argc < 4) {
    cout << "Bishoujo Senshi Sailor Moon (MD) text script extractor" << endl;
    cout << "Usage: " << argv[0] << " <infile> <outfile> <thingy> [options]" << endl;
    
    cout << "Options: " << endl;
    cout << "  -o   " << "Set starting offset" << endl;
    cout << "  -t   " << "Generate 'translator's copy'" << endl;
    cout << "  -p   " << "Specify script pointer" << endl;
    
    return 0;
  }
  
  char* infile = argv[1];
  char* outfile = argv[2];
  char* thingyname = argv[3];
  
  int offset = 0;
  TOpt::readNumericOpt(argc, argv, "-o", &offset);
  
  std::vector<int> pointers;
  for (int i = 0; i < argc - 1; i++) {
    if (strcmp(argv[i], "-p") == 0) {
      pointers.push_back(
        TStringConversion::stringToInt(std::string(argv[i + 1])));
    }
  }
  
  bool showControlCodes = true;
  char* topt
    = TOpt::getOpt(argc, argv, "-t");
  if (topt != NULL) {
    showControlCodes = false;
  }
  
  TThingyTable thingy = TThingyTable(std::string(thingyname));
  
  TIfstream ifs(infile, ios_base::binary);
  ifs.seek(offset);
  
  SmScript script;
  script.read(ifs);
  
  TOfstream ofs(outfile, ios_base::binary);
  script.writeCsv(ofs, thingy, SmScript::standard, showControlCodes,
                  pointers);
  
  return 0;
}
