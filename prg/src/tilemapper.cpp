#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include "util/TIniFile.h"
#include "util/TStringConversion.h"
#include "util/TFreeSpace.h"
#include "util/TFileManip.h"
#include "util/TArray.h"
#include "util/TByte.h"
#include "util/TGraphic.h"
#include "util/TPngConversion.h"
#include "md/MdPattern.h"
#include "md/MdPalette.h"
#include "md/MdTilemap.h"
#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <cstring>

using namespace std;
using namespace BlackT;
using namespace Md;

// stupid and inefficient, but good enough for what we're doing
typedef vector<int> Blacklist;

int readIntString(const string& src, int* pos) {
  string numstr;
  while (*pos < src.size()) {
    // accept "x" for hex
    if (!isdigit(src[*pos]) && !(src[*pos] == 'x')) break;
    else {
      numstr += src[(*pos)++];
    }
  }
  
  if (*pos < src.size()) ++(*pos);
  
  return TStringConversion::stringToInt(numstr);
}

void readBlacklist(Blacklist& blacklist, const string& src) {
  int pos = 0;
  
  while ((pos < src.size())) {
    int next = readIntString(src, &pos);
    
    // check if this is a range
    if ((pos < src.size()) && (src[(pos - 1)] == '-')) {
      int next2 = readIntString(src, &pos);
      for (unsigned int i = next; i <= next2; i++) {
        blacklist.push_back(i);
      }
    }
    else {
      blacklist.push_back(next);
    }
  }
}

bool isBlacklisted(Blacklist blacklist, int value) {
  for (unsigned int i = 0; i < blacklist.size(); i++) {
    if (blacklist[i] == value) return true;
  }
  
  return false;
}

int processTile(const TGraphic& srcG, int x, int y,
                const MdPalette& palette,
                MdTileId* dstId,
                vector<MdPattern>& rawTiles,
                vector<MdPattern>& rawTilesFlipX,
                vector<MdPattern>& rawTilesFlipY,
                vector<MdPattern>& rawTilesFlipXY,
                int forcePaletteLine = -1,
                bool disableFlipping = false) {
  
  // Get target graphic
//  TGraphic tileG(MdPattern::w, MdPattern::h);
//  tileG.copy(srcG,
//             TRect(0, 0, 0, 0),
//             TRect(x, y, 0, 0));
  
  int paletteNum = 0;
  MdPattern pattern;
  // If palette forcing is on, use the specified palette
  if (forcePaletteLine != -1) {
    paletteNum = forcePaletteLine;
    int result = pattern.fromColorGraphic(
                             srcG, palette.lines[forcePaletteLine],
                             x, y);
    if (result != 0) paletteNum = MdPalette::numLines;
  }
  else {
    // Find a palette that matches this part of the image.
    // If none matches, we can't proceed.
    for ( ; paletteNum < MdPalette::numLines; paletteNum++) {
      int result = pattern.fromColorGraphic(
                               srcG, palette.lines[paletteNum],
                               x, y);
      if (result == 0) break;
    }
  }
  
  if (paletteNum >= MdPalette::numLines) return -1;
  
  dstId->palette = paletteNum;
  
  // Determine if target graphic matches any existing tile.
  // If so, we don't need to add a new tile.
  // We must also account for possible horizontal/vertical flipping.
  bool foundMatch = false;
  for (int i = 0; i < rawTiles.size(); i++) {
    if (pattern == rawTiles[i]) {
      dstId->pattern = i;
      
      foundMatch = true;
      break;
    }
    else if (!disableFlipping
              && (pattern == rawTilesFlipX[i])) {
      
      dstId->pattern = i;
      dstId->hflip = true;
      
      foundMatch = true;
      break;
    }
    else if (!disableFlipping
              && (pattern == rawTilesFlipY[i])) {
      
      dstId->pattern = i;
      dstId->vflip = true;
      
      foundMatch = true;
      break;
    }
    else if (!disableFlipping
              && (pattern == rawTilesFlipXY[i])) {
      
      dstId->pattern = i;
      dstId->hflip = true;
      dstId->vflip = true;
      
      foundMatch = true;
      break;
    }
  }
  
  // if we found a match, we're done
  if (foundMatch) {
//    cout << dstId->pattern << endl;
    return 0;
  }
  
  // otherwise, add a new tile
  
  rawTiles.push_back(pattern);
  
  MdPattern flipcopy = pattern;
  flipcopy.flipHorizontal();
  rawTilesFlipX.push_back(flipcopy);
  
  pattern.flipVertical();
  rawTilesFlipY.push_back(pattern);
  
  flipcopy.flipVertical();
  rawTilesFlipXY.push_back(flipcopy);
  
  dstId->pattern = rawTiles.size() - 1;
  
  return 0;
}

int main(int argc, char* argv[]) {
  
  
  // Input:
  // * output filename for graphics
  //   (tilemaps assumed from input names)
  // * raw graphic(s)
  // * target offset in VRAM of tilemapped data
  // * optional output prefix
  // * palette
  //   (don't think we need this on a per-file basis?)
  
  if (argc < 2) {
    cout << "Megadrive tilemap generator" << endl;
    cout << "Usage: " << argv[0] << " <scriptfile>" << endl;
    
    return 0;
  }
  
  TIniFile script = TIniFile(string(argv[1]));
  
  if (!script.hasSection("Properties")) {
    cerr << "Error: Script has no 'Properties' section" << endl;
    return 1;
  }
  
  string paletteName, destName;
  int loadAddr = 0;
  int minTiles = 0;
  int maxTiles = -1;
  Blacklist blacklist;
  
  // Mandatory fields
  
  if (!script.hasKey("Properties", "palette")) {
    cerr << "Error: Properties.palette is undefined" << endl;
    return 1;
  }
  paletteName = script.valueOfKey("Properties", "palette");
  
  if (!script.hasKey("Properties", "dest")) {
    cerr << "Error: Properties.dest is undefined" << endl;
    return 1;
  }
  destName = script.valueOfKey("Properties", "dest");
  
  // Optional fields
  
  if (script.hasKey("Properties", "loadAddr")) {
    loadAddr = TStringConversion::stringToInt(script.valueOfKey(
      "Properties", "loadAddr"));
  }
  
  if (script.hasKey("Properties", "minTiles")) {
    minTiles = TStringConversion::stringToInt(script.valueOfKey(
      "Properties", "minTiles"));
  }
  
  if (script.hasKey("Properties", "maxTiles")) {
    maxTiles = TStringConversion::stringToInt(script.valueOfKey(
      "Properties", "maxTiles"));
  }
  
  if (script.hasKey("Properties", "blacklist")) {
    string blacklistStr = script.valueOfKey("Properties", "blacklist");
    readBlacklist(blacklist, blacklistStr);
  }
  
//  for (int i = 0; i < blacklist.size(); i++) {
//    cout << blacklist[i] << endl;
//  }
  
  // get palette
  MdPalette palette;
  {
    TArray<TByte> rawPalette;
    TFileManip::readEntireFile(rawPalette, paletteName);
    palette = MdPalette(rawPalette.data());
  }
  
  // 1. go through all source images and analyze for matching tiles
  // 2. create per-image tilemap corresponding to raw tile indices
  // 3. map raw tile indices to actual tile positions (accounting for
  //    blacklist, etc.)
  // 4. generate final tilemaps by mapping raw indices to final positions
  
//  vector<MdTilemap> rawTilemaps;
  map<string, MdTilemap> rawTilemaps;
  vector<MdPattern> rawTiles;
  vector<MdPattern> rawTilesFlipX;
  vector<MdPattern> rawTilesFlipY;
  vector<MdPattern> rawTilesFlipXY;
  
  for (SectionKeysMap::const_iterator it = script.cbegin();
       it != script.cend();
       ++it) {
    // iterate over all sections beginning with "Tilemap"
    string cmpstr = "Tilemap";
    if (it->first.substr(0, cmpstr.size()).compare(cmpstr) != 0) continue;
    string sectionName = it->first;
    
    string sourceStr, copyPriorityMapStr;
    int blanketPriority = 0;
    bool hasCopyPriorityMap = false;
    int forcePaletteLine = -1;
    bool noflip = false;
    
    // mandatory fields
    
    if (!script.hasKey(sectionName, "source")) {
      cerr << "Error: " << sectionName << ".source is undefined" << endl;
      return 1;
    }
    sourceStr = script.valueOfKey(sectionName, "source");
    
    // optional fields
    
    if (script.hasKey(sectionName, "copyPriorityMap")) {
      copyPriorityMapStr = script.valueOfKey(sectionName, "copyPriorityMap");
      hasCopyPriorityMap = true;
      
      // read copy map
      // ...
    }
    
    if (script.hasKey(sectionName, "priority")) {
      blanketPriority = TStringConversion::stringToInt(
        script.valueOfKey(sectionName, "priority"));
    }
    
    if (script.hasKey(sectionName, "palette")) {
      forcePaletteLine = TStringConversion::stringToInt(
        script.valueOfKey(sectionName, "palette"));
    }
    
    if (script.hasKey(sectionName, "noflip")) {
      noflip = (TStringConversion::stringToInt(
        script.valueOfKey(sectionName, "noflip")) != 0);
    }
    
    // get source graphic
    TGraphic srcG;
    TPngConversion::RGBAPngToGraphic(sourceStr, srcG);
    
    // infer tilemap dimensions from source size
    int tileW = srcG.w() / MdPattern::w;
    int tileH = srcG.h() / MdPattern::h;
    
    MdTilemap tilemap;
    tilemap.resize(tileW, tileH);
    
//    cout << tileW << " " << tileH << endl;

    for (int j = 0; j < tilemap.tileIds.h(); j++) {
      for (int i = 0; i < tilemap.tileIds.w(); i++) {
        MdTileId& tileId = tilemap.tileIds.data(i, j);
        tileId.priority = blanketPriority;
        tileId.hflip = false;
        tileId.vflip = false;
        // copy from priority map if needed
        // ...
      
        int result = processTile(srcG, i * MdPattern::w, j * MdPattern::h,
                                 palette,
                                 &tileId,
                                 rawTiles, rawTilesFlipX, rawTilesFlipY,
                                 rawTilesFlipXY,
                                 forcePaletteLine,
                                 noflip);
        
        if (result != 0) {
          cerr << "Error in " << sectionName
            << ": failed processing tile (" << i << ", " << j << ")" << endl;
          return 2;
        }
      }
    }
    
    rawTilemaps[sectionName] = tilemap;
  }
  
//  cout << rawTiles.size() << endl;

  // Produce the final arrangement of tiles
  
  map<int, MdPattern> outputTiles;
  map<int, int> rawToOutputMap;
  int outputTileNum = 0;
  for (int i = 0; i < rawTiles.size(); i++) {
    // Skip blacklisted content
    while (isBlacklisted(blacklist, outputTileNum)) {
      outputTiles[outputTileNum] = MdPattern();
      ++outputTileNum;
    }
    
    outputTiles[outputTileNum] = rawTiles[i];
    rawToOutputMap[i] = outputTileNum;
    ++outputTileNum;
  }
  
  // Give an error if tile limit exceeded
  if (outputTiles.size() > maxTiles) {
    cerr << "Error: Tile limit exceeded (limit is "
      << maxTiles << "; generated "
      << outputTiles.size() << ")" << endl;
    return -3;
  }
  
  // Write tile data
  {
    TOfstream ofs(destName.c_str(), ios_base::binary);
    char buffer[MdPattern::uncompressedSize];
    for (map<int, MdPattern>::const_iterator it = outputTiles.cbegin();
         it != outputTiles.cend();
         ++it) {
      it->second.write(buffer);
      ofs.write(buffer, MdPattern::uncompressedSize);
    }
    
    // pad with extra tiles to meet minimum length
    memset(buffer, 0, MdPattern::uncompressedSize);
    int padTiles = minTiles - outputTiles.size();
    for (int i = 0; i < padTiles; i++) {
      ofs.write(buffer, MdPattern::uncompressedSize);
    }
  }
  
  // Update tilemaps and write to destinations
  
  for (SectionKeysMap::const_iterator it = script.cbegin();
       it != script.cend();
       ++it) {
    // iterate over all sections beginning with "Tilemap"
    string cmpstr = "Tilemap";
    if (it->first.substr(0, cmpstr.size()).compare(cmpstr) != 0) continue;
    string sectionName = it->first;
    
    if (!script.hasKey(sectionName, "dest")) {
      cerr << "Error: " << sectionName << ".dest is undefined" << endl;
      return 1;
    }
    string destStr = script.valueOfKey(sectionName, "dest");
    
    MdTilemap& tilemap = rawTilemaps[sectionName];
    
    TOfstream ofs(destStr.c_str(), ios_base::binary);
    
    // Check if this tilemap needs to be output in DMA transfer format
    bool useDmaFormat = false;
    if (script.hasKey(sectionName, "useDmaFormat")) {
      useDmaFormat = TStringConversion::stringToInt(script.valueOfKey(
        sectionName, "useDmaFormat")) != 0;
    }
    
    // Check if this tilemap needs to be output in explicit-width format
    bool useWidthFormat = false;
    if (script.hasKey(sectionName, "useWidthFormat")) {
      useWidthFormat = TStringConversion::stringToInt(script.valueOfKey(
        sectionName, "useWidthFormat")) != 0;
    }
    
    // Output DMA header if using DMA format
    if (useDmaFormat) {
      // Read mandatory DMA arguments
      
      int dmaTargetFlag, dmaPlaneBaseAddr, dmaX, dmaY;
      
      if (!script.hasKey(sectionName, "dmaTargetFlag")) {
        cerr << "Error: " << sectionName << ".dmaTargetFlag is undefined"
          << endl;
        return 1;
      }
      dmaTargetFlag = TStringConversion::stringToInt(
        script.valueOfKey(sectionName, "dmaTargetFlag"));
      
      if (!script.hasKey(sectionName, "dmaPlaneBaseAddr")) {
        cerr << "Error: " << sectionName << ".dmaPlaneBaseAddr is undefined"
          << endl;
        return 1;
      }
      dmaPlaneBaseAddr = TStringConversion::stringToInt(
        script.valueOfKey(sectionName, "dmaPlaneBaseAddr"));
      
      if (!script.hasKey(sectionName, "dmaX")) {
        cerr << "Error: " << sectionName << ".dmaX is undefined"
          << endl;
        return 1;
      }
      dmaX = TStringConversion::stringToInt(
        script.valueOfKey(sectionName, "dmaX"));
      
      if (!script.hasKey(sectionName, "dmaY")) {
        cerr << "Error: " << sectionName << ".dmaY is undefined"
          << endl;
        return 1;
      }
      dmaY = TStringConversion::stringToInt(
        script.valueOfKey(sectionName, "dmaY"));
      
      // Output header
      
      // 2b tilemap height minus one
      ofs.writeu16be(tilemap.tileIds.h() - 1);
      
      // 2b target flag
      ofs.writeu16be(dmaTargetFlag);
      
      // 2b tilemap width (for DMA commands)
      ofs.writeu16be(tilemap.tileIds.w());
      
      // 2b tilemap width (again)
      ofs.writeu16be(tilemap.tileIds.w());
      
      // 4b DMA transfer command
      // base command: VRAM write, use DMA, memory to VRAM
      // CD = 100001 = 0x21
      // A = planeBase + (y * bytesPerPlaneRow) + (x * tileIdSize)
      unsigned int dmaCommand = 0x40000080;
      unsigned int targetAddr = dmaPlaneBaseAddr
        + (dmaY * 0x80)
        + (dmaX * MdTileId::size);
      dmaCommand |= ((targetAddr & 0x3FFF) << 16);
      dmaCommand |= ((targetAddr & 0xC000) >> 14);
      ofs.writeu32be(dmaCommand);
      
      // 2b header size (constant?)
      ofs.writeu16be(0x10);
      
      // 2b offset to non-DMA portion (size of tilemap data to be transferred
      // via DMA, plus size of this header
      ofs.writeu16be((tilemap.tileIds.w() * tilemap.tileIds.h()
                      * MdTileId::size)
                     + 0x10);
    }
    else if (useWidthFormat) {
      // write map width
      ofs.writeu16be(tilemap.tileIds.w());
    }
    
    for (int j = 0; j < tilemap.tileIds.h(); j++) {
      for (int i = 0; i < tilemap.tileIds.w(); i++) {
        MdTileId& id = tilemap.tileIds.data(i, j);
        id.pattern = rawToOutputMap[id.pattern] + loadAddr;
        char buffer[MdTileId::size];
        id.write(buffer);
        ofs.write(buffer, MdTileId::size);
      }
    }
    
    // write DMA terminator if using DMA mode
    if (useDmaFormat) {
      ofs.writeu16be(0xFFFF);
    }
    
  }
  
  return 0;
}
