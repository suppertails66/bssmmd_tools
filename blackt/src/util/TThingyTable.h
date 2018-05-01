#ifndef TTHINGYTABLE_H
#define TTHINGYTABLE_H


#include <string>
#include <map>

namespace BlackT {


struct TThingyTable {
  typedef std::string TableEntry;
  typedef std::map<int, TableEntry> RawTable;
  typedef std::map<int, int> ReverseMap;

  TThingyTable();
  TThingyTable(std::string filename);
  
  void readUtf8(std::string filename);
  
  std::string getEntry(int charID) const;
  
  int getRevEntry(int charID) const;
  
  RawTable entries;
  ReverseMap revEntries;
};


}


#endif 
