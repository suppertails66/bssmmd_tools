#include "util/TThingyTable.h"
#include "util/TStringConversion.h"
#include "util/TCharFmt.h"
#include <fstream>

using namespace std;

namespace BlackT {


TThingyTable::TThingyTable() { }

TThingyTable::TThingyTable(string filename) {
  ifstream ifs(filename.c_str(), ios_base::binary);
  
  while (ifs.good()) {
    string numberString;
    getline(ifs, numberString, '=');
    if (!ifs.good()) {
      break;
    }
    
    // add hex ID for int conversion
//    numberString = string("0x") + numberString;
    int rawID = TStringConversion::stringToInt(numberString);
    
    string charString;
    getline(ifs, charString);
    entries[rawID] = TableEntry(charString);
  }
}
  
void TThingyTable::readUtf8(std::string filename) {
  ifstream ifs(filename.c_str(), ios_base::binary);
  
  while (ifs.good()) {
    string numberString;
    getline(ifs, numberString, '=');
    if (!ifs.good()) {
      break;
    }
    
    // add hex ID for int conversion
//    numberString = string("0x") + numberString;
    int rawID = TStringConversion::stringToInt(numberString);
    
    string charString;
    getline(ifs, charString);
    entries[rawID] = TableEntry(charString);
    
    TUtf16Chars chars;
    TCharFmt::utf8To16(charString, chars);
    if (chars.size() > 0) {
      revEntries[chars[0]] = rawID;
    }
  }
}

string TThingyTable::getEntry(int charID) const {
  RawTable::const_iterator it = entries.find(charID);
  
  if (it == entries.cend()) {
    return string("?");
  }
  
  return it->second;
}
  
int TThingyTable::getRevEntry(int charID) const {
  ReverseMap::const_iterator it = revEntries.find(charID);
  
  if (it == revEntries.cend()) {
    return -1;
  }
  
  return it->second;
}


} 
