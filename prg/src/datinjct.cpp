#include "util/TFreeSpace.h"
#include "util/TIniFile.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include <vector>
#include <sstream>
#include <iostream>

using namespace std;
using namespace BlackT;

int nextInt(istream& ifs) {
  string str;
  ifs >> str;
  return TStringConversion::stringToInt(str);
}



int main(int argc, char* argv[]) {
  if (argc < 5) {
    cout << "Data injection utility" << endl;
    cout << "Usage: " << argv[0] << " <spacefile> <scriptfile> <src> <dst>"
      << endl;
    
    return 0;
  }
  
  TFreeSpace freeSpace;
  
  TBufStream ofs(0x800000);
  {
    TIfstream ifs(argv[3], ios_base::binary);
    ofs.writeFrom(ifs, ifs.size());
  }
  int romSize = ofs.tell();
  
  TIniFile freeSpaceFile(argv[1]);
  
  for (SectionKeysMap::const_iterator it = freeSpaceFile.cbegin();
       it != freeSpaceFile.cend();
       ++it) {
    int pos = TStringConversion::stringToInt(it->first);
    int size = TStringConversion::stringToInt(
      freeSpaceFile.valueOfKey(it->first, "size"));
    freeSpace.free(pos, size);
  }
  
  TIniFile scriptFile(argv[2]);
  
  for (SectionKeysMap::const_iterator it = scriptFile.cbegin();
       it != scriptFile.cend();
       ++it) {
    string sourceFile = scriptFile.valueOfKey(it->first, "source");
    int origPos = TStringConversion::stringToInt(
      scriptFile.valueOfKey(it->first, "origPos"));
    int origSize = TStringConversion::stringToInt(
      scriptFile.valueOfKey(it->first, "origSize"));
      
    vector<int> pointers;
    if (scriptFile.hasKey(it->first, "pointers")) {
      string pointersString = scriptFile.valueOfKey(it->first, "pointers");
      istringstream iss(pointersString);
      while (iss.good()) {
        int ptr = nextInt(iss);
        pointers.push_back(ptr);
//        cout << ptr << endl;
      }
    }
    
    vector<int> sizes16;
    if (scriptFile.hasKey(it->first, "rawSize16")) {
      string sizesString = scriptFile.valueOfKey(it->first, "rawSize16");
      istringstream iss(sizesString);
      while (iss.good()) {
        int sz = nextInt(iss);
        sizes16.push_back(sz);
//        cout << ptr << endl;
      }
    }
    
    TIfstream ifs(sourceFile.c_str(), ios_base::binary);
    int newSize = ifs.size();
    // word-align inserted data
    int claimSize = newSize + (newSize % 2);
    
    // Put at original position if possible
    if (newSize <= origSize) {
      ofs.seek(origPos);
      ofs.writeFrom(ifs, newSize);
      
      cout << sourceFile << ": overwrote at 0x"
        << hex << origPos << dec << endl;
    }
    // Relocate if larger than original size
    else {
      // Find new position
      int newPos = freeSpace.claim(claimSize);
      
      // If we can't get that much space, error
      if (newPos == -1) {
        cerr << "Error injecting " << sourceFile << ": can't find "
          << claimSize << " bytes of free space" << endl;
        return 1;
      }
      
      // Insert at new position
      ofs.seek(newPos);
      ofs.writeFrom(ifs, newSize);
      
      if (ofs.tell() > romSize) romSize = ofs.tell();
    
      // Update pointers
      for (unsigned int i = 0; i < pointers.size(); i++) {
//        int origPtr = ofs.readu32be();
        ofs.seek(pointers[i]);
        ofs.writeu32be(newPos);
      }
      
      // Update size references
      if (sizes16.size() != 0) {
        int rawFileSize;
        {
          string rawSourceFile = scriptFile.valueOfKey(it->first, "rawSource");
          TIfstream rawifs(rawSourceFile.c_str(), ios_base::binary);
          rawFileSize = rawifs.size();
        }
        
        for (unsigned int i = 0; i < sizes16.size(); i++) {
          ofs.seek(sizes16[i]);
          ofs.writeu16be(rawFileSize);
        }
        
      }
      
      // Free up original space for other content
      freeSpace.free(origPos, origSize);
      
      cout << sourceFile << ": moved from 0x"
        << hex << origPos << " to 0x"
        << newPos << dec << endl;
    }
  }
  
  ofs.seek(romSize);
  ofs.save(argv[4]);
  
  return 0;
}

