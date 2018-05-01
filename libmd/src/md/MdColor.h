#ifndef MDCOLOR_H
#define MDCOLOR_H


#include "util/TColor.h"
#include "util/TByte.h"
#include "util/TTwoDArray.h"
#include "util/TTwoDByteArray.h"
#include "util/TGraphic.h"

namespace Md {


struct MdColor {

  MdColor();
  
  MdColor(int rawColor);
  
  BlackT::TColor toColor() const;
  
  int r;
  int g;
  int b;
  
};


}


#endif 
