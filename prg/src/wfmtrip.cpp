#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TStringConversion.h"
#include "util/TArray.h"
#include "util/TByte.h"
#include <iostream>

using namespace std;
using namespace BlackT;

int main(int argc, char* argv[]) {
  if (argc < 4) {
    cout << "Width-format tilemap extractor" << endl;
    cout << "Usage: " << argv[0] << " <infile> <outfile> <offset>" << endl;
    
    return 0;
  }
  
  int offset = TStringConversion::stringToInt(argv[3]);
  
  TIfstream ifs(argv[1], ios_base::binary);
  ifs.seek(offset);
  
  int width = ifs.readu16be();
  TArray<TByte> data;
  data.resize(width * 4);
  ifs.read((char*)data.data(), data.size());
  
  TOfstream ofs(argv[2], ios_base::binary);
  ofs.write((char*)data.data(), data.size());
  
/*  cout << "[Tilemap00]" << endl;
  cout << "source=" << endl;
  cout << "dest=" << endl;
  cout << "priority=0" << endl;
  cout << "useWidthFormat=1" << endl;
  cout << endl; */
  
  return 0;
}
