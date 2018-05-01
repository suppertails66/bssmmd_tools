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
#include <iomanip>
#include <string>
#include <map>
#include <vector>

using namespace std;
using namespace BlackT;
using namespace Md;

const int baseScriptPointerPos = 0x1af7e;

int main(int argc, char* argv[]) {
  if (argc < 2) return 0;
  
//  map<int, bool> unique;
  map<int, vector<int> > scriptOffsetToPointers;
  map<int, int > scriptIndexToOffset;

  TIfstream ifs(argv[1], ios_base::binary);
  
//  cout << "rm -f scripts/all.csv" << endl;
  
  int num = 0;
  while (!ifs.nextIsEof()) {
    unsigned int offset = ifs.readu32be();
    
    scriptOffsetToPointers[offset].push_back(
      baseScriptPointerPos + ifs.tell() - 4);
    
    // omit duplicate indices
    if (scriptOffsetToPointers[offset].size() < 2) {
      scriptIndexToOffset[num++] = offset;
    }
  }
    
  for (map<int, int >::const_iterator it
          = scriptIndexToOffset.cbegin();
       it != scriptIndexToOffset.cend();
       ++it) {
/*    if (unique.find(offset) != unique.end()) {
      cerr << "duplicate at " << hex << ifs.tell() - 4
        << ": " << hex << offset << endl;
      continue;
    }
    else {
      unique[offset] = true;
    } */
    
//    cout << "./smscriptrip bssm.md scripts/split/0x" << hex << offset
//      << ".csv bssm_thingy.txt -o 0x" << hex << offset << endl;
    int num = it->first;
    int offset = it->second;
    vector<int> pointers = scriptOffsetToPointers[offset];
    string ptrString;
    for (int i = 0; i < pointers.size(); i++) {
      ptrString += " -p ";
      ptrString += TStringConversion::intToString(pointers[i],
                      TStringConversion::baseHex);
    }

    cout << "./smscriptrip bssm.md scripts/split/" << dec << setw(2)
      << setfill('0') << num
      << ".csv bssm_thingy.txt -o 0x" << hex << offset << ptrString << endl;
    cout << "./smscriptrip bssm.md scripts/split_nocc/" << dec << setw(2)
      << setfill('0') << num
      << ".csv bssm_thingy.txt -t -o 0x" << hex << offset << ptrString << endl;
//    ++num;
//    cout << endl;
  }
  
  cout << "cat scripts/split/* > scripts/all.csv" << endl;
  cout << "cat scripts/split_nocc/* > scripts/all_nocc.csv" << endl;
  
//  cerr << unique.size() << endl;
  
  return 0;
}
