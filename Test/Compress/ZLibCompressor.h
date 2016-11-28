#pragma once

#include "Compressor.h"

class ZLibCompressor : public Compressor {
public:
  void compress(ResetableByteInputStream &input, ByteOutputStream &output, int quality);
  void expand(  ByteInputStream          &input, ByteOutputStream &output);
};
