#pragma once

#include <BitStream.h>
#include <ByteStream.h>

class Compressor {
protected:
  static const int endOfStream;

  UINT64 m_compressedSize, m_rawSize;
public:
  virtual void compress(ResetableByteInputStream &input, ByteOutputStream &output, int quality = 3) = 0;
  virtual void expand(  ByteInputStream          &input, ByteOutputStream &output) = 0;
  virtual void dump()  const {}
  virtual void check() const {}
  virtual ~Compressor() {
  }
  void printRatios(FILE *f = stdout) const;
  String getRatios() const;
};
