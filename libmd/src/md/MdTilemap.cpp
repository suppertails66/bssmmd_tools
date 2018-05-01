#include "md/MdTilemap.h"

using namespace BlackT;

namespace Md {


MdTilemap::MdTilemap() { }
  
void MdTilemap::resize(int w, int h) {
  tileIds.resize(w, h);
}

const MdTileId& MdTilemap::getTileId(int x, int y) const {
  return tileIds.data(x, y);
}

void MdTilemap::setTileId(int x, int y, const MdTileId& tileId) {
  tileIds.data(x, y) = tileId;
}

void MdTilemap::read(const char* src, int w, int h) {
  resize(w, h);
  
  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i++) {
      tileIds.data(i, j).read(src);
      src += MdTileId::size;
    }
  }
}
  
void MdTilemap::toColorGraphic(BlackT::TGraphic& dst,
                    const MdVram& vram,
                    const MdPalette& pal) {
  dst.resize(tileIds.w() * MdPattern::w,
             tileIds.h() * MdPattern::h);
  dst.clearTransparent();
  
  for (int j = 0; j < tileIds.h(); j++) {
    for (int i = 0; i < tileIds.w(); i++) {
      tileIds.data(i, j).toColorGraphic(
        dst, vram, pal, (i * MdPattern::w), (j * MdPattern::h));
    }
  }
}
  
void MdTilemap::toGrayscaleGraphic(BlackT::TGraphic& dst,
                    const MdVram& vram) {
  dst.resize(tileIds.w() * MdPattern::w,
             tileIds.h() * MdPattern::h);
  dst.clearTransparent();
  
  for (int j = 0; j < tileIds.h(); j++) {
    for (int i = 0; i < tileIds.w(); i++) {
      tileIds.data(i, j).toGrayscaleGraphic(
        dst, vram, (i * MdPattern::w), (j * MdPattern::h));
    }
  }
}


}
