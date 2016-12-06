#include "stdafx.h"
#include <CountedByteStream.h>
#include <CompressFilter.h>
#include "ZLibCompressor.h"

void ZLibCompressor::compress(ResetableByteInputStream &input, ByteOutputStream &output, int quality) {
  ByteCounter              outputCounter;
  CountedByteOutputStream  out(outputCounter, output);
  CompressFilter           filter(out);
  BYTE                     buffer[4096];
  intptr_t                 n;
  size_t                   byteCounter = 0;

  while((n = input.getBytes(buffer, sizeof(buffer))) > 0) {
    filter.putBytes(buffer, n);
    byteCounter += n;
  }
  filter.flush();
  m_rawSize        = byteCounter;
  m_compressedSize = outputCounter.getCount();
}

void ZLibCompressor::expand(ByteInputStream &input, ByteOutputStream &output) {
  ByteCounter              inputCounter;
  CountedByteInputStream   in(inputCounter, input );
  DecompressFilter         filter(in);
  BYTE                     buffer[4096];
  intptr_t                 n;
  size_t                   byteCounter = 0;

  while((n = filter.getBytes(buffer, sizeof(buffer))) > 0) {
    output.putBytes(buffer, n);
    byteCounter += n;
  }
  m_compressedSize = inputCounter.getCount();
  m_rawSize        = byteCounter;
}
