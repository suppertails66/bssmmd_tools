#include "util/TStream.h"
#include "util/TSerialize.h"

namespace BlackT { 


int TStream::remaining() {
  return (size() - tell());
}

void TStream::seekoff(int offset) {
  seek(tell() + offset);
}

int TStream::readInt(
            int sz,
            EndiannessTypes::EndiannessType end,
            SignednessTypes::SignednessType sign) {
  return TSerialize::readInt(*this, sz, end, sign);
}

void TStream::writeInt(
            int value,
            int sz,
            EndiannessTypes::EndiannessType end,
            SignednessTypes::SignednessType sign) {
  TSerialize::writeInt(*this, value, sz, end, sign);
}

void TStream::readCstr(char* dst, int limit) {
  char c;
  int pos = 0;
  while ((c = get())) {
    if ((limit != -1) && (pos >= limit)) break;
    dst[pos++] = c;
  }
  
  // write terminator
  dst[pos++] = c;
}

void TStream::writeCstr(const char* src, int limit) {
  char c;
  int pos = 0;
  while ((c = src[pos++])) {
    if ((limit != -1) && (pos >= limit)) break;
    put(c);
  }
  
  // write terminator
  put(c);
}

void TStream::readCstrString(std::string& dst, int limit) {
  char c;
  int pos = 0;
  while ((c = get())) {
    if ((limit != -1) && (pos >= limit)) break;
    dst += c;
  }
}

void TStream::readRev(char* dst, int size) {
  read(dst, size);
  char* first = dst;
  char* last = dst + size - 1;
  while (first < last) {
    char temp = *last;
    *last = *first;
    *first = temp;
    ++first;
    --last;
  }
}

void TStream::writeRev(const char* src, int size) {
  // the lazy way
  const char* srcend = src + size - 1;
  while (srcend >= src) {
    write(srcend--, 1);
  }
}

void TStream::alignToBoundary(int byteBoundary) {
  int amt = byteBoundary - (size() % byteBoundary);
  if (amt != byteBoundary) {
    for (int i = 0; i < amt; i++) {
      get();
    }
  }
}

void TStream::alignToReadBoundary(int byteBoundary) {
  int amt = byteBoundary - (tell() % byteBoundary);
  if (amt != byteBoundary) {
    seekoff(amt);
  }
}

void TStream::alignToWriteBoundary(int byteBoundary) {
  alignToBoundary(byteBoundary);
}

bool TStream::nextIsEof() {
  get();
  bool result = eof();
  unget();
  return result;
}

void TStream::write(const std::string& str) {
  write(str.c_str(), str.size());
}

TStream::TStream() { }
TStream::~TStream() { }

int TStream::reads8le() {
  return readInt(1, EndiannessTypes::little, SignednessTypes::sign);
}

int TStream::reads16le() {
  return readInt(2, EndiannessTypes::little, SignednessTypes::sign);
}

int TStream::reads32le() {
  return readInt(4, EndiannessTypes::little, SignednessTypes::sign);
}

int TStream::reads64le() {
  return readInt(8, EndiannessTypes::little, SignednessTypes::sign);
}

int TStream::readu8le() {
  return readInt(1, EndiannessTypes::little, SignednessTypes::nosign);
}

int TStream::readu16le() {
  return readInt(2, EndiannessTypes::little, SignednessTypes::nosign);
}

int TStream::readu32le() {
  return readInt(4, EndiannessTypes::little, SignednessTypes::nosign);
}

int TStream::readu64le() {
  return readInt(8, EndiannessTypes::little, SignednessTypes::nosign);
}

void TStream::writes8le(int value) {
  writeInt(value, 1, EndiannessTypes::little, SignednessTypes::sign);
}

void TStream::writes16le(int value) {
  writeInt(value, 2, EndiannessTypes::little, SignednessTypes::sign);
}

void TStream::writes32le(int value) {
  writeInt(value, 4, EndiannessTypes::little, SignednessTypes::sign);
}

void TStream::writes64le(int value) {
  writeInt(value, 8, EndiannessTypes::little, SignednessTypes::sign);
}

void TStream::writeu8le(int value) {
  writeInt(value, 1, EndiannessTypes::little, SignednessTypes::nosign);
}

void TStream::writeu16le(int value) {
  writeInt(value, 2, EndiannessTypes::little, SignednessTypes::nosign);
}

void TStream::writeu32le(int value) {
  writeInt(value, 4, EndiannessTypes::little, SignednessTypes::nosign);
}

void TStream::writeu64le(int value) {
  writeInt(value, 8, EndiannessTypes::little, SignednessTypes::nosign);
}

void TStream::writes8be(int value) {
  writeInt(value, 1, EndiannessTypes::big, SignednessTypes::sign);
}

void TStream::writes16be(int value) {
  writeInt(value, 2, EndiannessTypes::big, SignednessTypes::sign);
}

void TStream::writes32be(int value) {
  writeInt(value, 4, EndiannessTypes::big, SignednessTypes::sign);
}

void TStream::writes64be(int value) {
  writeInt(value, 8, EndiannessTypes::big, SignednessTypes::sign);
}

void TStream::writeu8be(int value) {
  writeInt(value, 1, EndiannessTypes::big, SignednessTypes::nosign);
}

void TStream::writeu16be(int value) {
  writeInt(value, 2, EndiannessTypes::big, SignednessTypes::nosign);
}

void TStream::writeu32be(int value) {
  writeInt(value, 4, EndiannessTypes::big, SignednessTypes::nosign);
}

void TStream::writeu64be(int value) {
  writeInt(value, 8, EndiannessTypes::big, SignednessTypes::nosign);
}

int TStream::reads8be() {
  return readInt(1, EndiannessTypes::big, SignednessTypes::sign);
}

int TStream::reads16be() {
  return readInt(2, EndiannessTypes::big, SignednessTypes::sign);
}

int TStream::reads32be() {
  return readInt(4, EndiannessTypes::big, SignednessTypes::sign);
}

int TStream::reads64be() {
  return readInt(8, EndiannessTypes::big, SignednessTypes::sign);
}

int TStream::readu8be() {
  return readInt(1, EndiannessTypes::big, SignednessTypes::nosign);
}

int TStream::readu16be() {
  return readInt(2, EndiannessTypes::big, SignednessTypes::nosign);
}

int TStream::readu32be() {
  return readInt(4, EndiannessTypes::big, SignednessTypes::nosign);
}

int TStream::readu64be() {
  return readInt(8, EndiannessTypes::big, SignednessTypes::nosign);
}


}
