#include "md/MdColor.h"
#include "util/ByteConversion.h"

using namespace std;
using namespace BlackT;

namespace Md {


  MdColor::MdColor()
    : r(0), g(0), b(0) { }
  
  MdColor::MdColor(int rawColor)
    : r(0), g(0), b(0) {
    r = (rawColor & 0x000E) >> 1;
    g = (rawColor & 0x00E0) >> 5;
    b = (rawColor & 0x0E00) >> 9;
  }
  
  TColor MdColor::toColor() const {
//    return TColor(r << 5, g << 5, b << 5,
//                  !(r && g && b)
//                    ? TColor::fullAlphaTransparency
//                    : TColor::fullAlphaOpacity);
    return TColor(r << 5, g << 5, b << 5,
                  TColor::fullAlphaOpacity);
  }



}
