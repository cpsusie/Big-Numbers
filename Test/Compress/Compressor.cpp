#include "stdafx.h"
#include "Compressor.h"

const int Compressor::endOfStream = 256;

void Compressor::printRatios(FILE *f) {
  const double ratio = 100.0 - PERCENT(m_compressedSize, m_rawSize);

  _ftprintf(f,_T("Compressed size  :%s\n") ,format1000(m_compressedSize).cstr());
  _ftprintf(f,_T("Expanded size    :%s\n") ,format1000(m_rawSize).cstr());
  _ftprintf(f,_T("Compression ratio:%.1lf%%\n"), ratio);
}
