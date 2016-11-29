#include "stdafx.h"
#include "Compressor.h"

const int Compressor::endOfStream = 256;

void Compressor::printRatios(FILE *f) const {
  _ftprintf(f,_T("%s"), getRatios().cstr());
}

String Compressor::getRatios() const {
  const double ratio = 100.0 - PERCENT(m_compressedSize, m_rawSize);

  return format(_T("Compressed size  :%s\n"
                   "Raw        size  :%s\n"
                   "Compression ratio:%.1lf%%\n"
                  )
                ,format1000(m_compressedSize).cstr()
                ,format1000(m_rawSize).cstr()
                ,ratio
               );
}