#include "md/MdPalette.h"
#include "util/ByteConversion.h"

using namespace std;
using namespace BlackT;

namespace Md {


  MdPalette::MdPalette() { }
  
  MdPalette::MdPalette(const TByte* src) {
    for (int i = 0; i < numLines; i++) {
      lines[i] = MdPaletteLine(src);
      src += MdPaletteLine::uncompressedSize;
    }
  }



} 
