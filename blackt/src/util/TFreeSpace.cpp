#include "util/TFreeSpace.h"

namespace BlackT {


TFreeSpace::TFreeSpace() { }

int TFreeSpace::claim(int size) {
  for (FreeSpaceMap::iterator it = freeSpace_.begin();
       it != freeSpace_.end();
       ++it) {
    if (it->second >= size) {
      // save old info (erasing will invalidate the iterator)
      int oldpos = it->first;
      int oldsize = it->second;
      int newpos = oldpos + size;
      int newsize = oldsize - size;
      
      freeSpace_.erase(it);
      
      // remap remaining free space in block (if nonzero)
      if (newsize > 0) {
        freeSpace_[newpos] = newsize;
      }
      
      return oldpos;
    }
  }
  
  // not enough space available
  return -1;
}

void TFreeSpace::free(int offset, int size) {
/*  // Check if new block overlaps or is adjacent to an existing one
  
  // Does starting address overlap?
  FreeSpaceMap::iterator findIt = findBlockStart(offset);
  if (findIt != freeSpace.end()) {
    
    
    return;
  }
  
  // Does ending address overlap?
  FreeSpaceMap::iterator findIt = findBlockStart(offset + size);
  if (findIt != freeSpace.end()) {
    
    
    return;
  } */
  
  if (size == 0) return;
  
  freeSpace_[offset] = size;
}

void TFreeSpace::defragment() {
  
}

TFreeSpace::FreeSpaceMap::iterator TFreeSpace::findBlockStart(int offset) {
  for (FreeSpaceMap::iterator it = freeSpace_.begin();
       it != freeSpace_.end();
       ++it) {
    if (offset < it->first) continue;
    if (offset < (it->first + it->second)) return it;
  }
  
  // No block contains specified address
  return freeSpace_.end();
}


}
