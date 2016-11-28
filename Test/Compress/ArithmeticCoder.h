#pragma once

#include "Compressor.h"

#define USE_FAST_SYMBOL_CONVERSION

class Symbol {
public:
  USHORT m_lowCount;
  USHORT m_highCount;
  USHORT m_scale;
};

class ArithmeticCoder : public Compressor {
private:
  Symbol  m_symbolTable[257];
  USHORT  m_totals[258];
  USHORT *m_totalsMap;
  USHORT  m_code;
  USHORT  m_low;
  USHORT  m_high;
  long    m_underflow;

  void        initEncoder();
  void        initDecoder( BitInputStream &in);
  void        buildModel(  ResetableByteInputStream &input, BitOutputStream &output);
  ULONG      *countBytes(  ResetableByteInputStream &input, ULONG *counts);
  BYTE       *scaleCounts(ULONG *counts, BYTE *scaledCounts);
  const BYTE *outputCounts(BitOutputStream &output, const BYTE *scaledCounts);
  void        buildTotals(const BYTE *scaledCounts);
  void        inputCounts(BitInputStream &input);
  void        encodeSymbol(BitOutputStream &out, const Symbol &s);
  void        removeSymbolFromStream(BitInputStream &in, const Symbol &s);
  inline void getSymbolScale(Symbol &s) const {
    s.m_scale = m_totals[endOfStream+1];
  }

  inline int  getCurrentCount(const Symbol &s) const {
    const long range = (long)(m_high - m_low) + 1;
    short int count = (short)((((long)(m_code - m_low)+1) * s.m_scale - 1) / range);
    return count;
  }
  inline const Symbol &convertIntToSymbol(int ch) const {
    return m_symbolTable[ch];
  }

#ifdef USE_FAST_SYMBOL_CONVERSION
  void buildTotalsMap();

  inline int convertSymbolToInt(UINT count, Symbol &s) const {
    const int c   = m_totalsMap[count];
    s.m_lowCount  = m_totals[c];
    s.m_highCount = m_totals[c+1];
    return c;
  }
#else
  inline int convertSymbolToInt(int count, Symbol &s) const {
    int c;
    for(c = endOfStream; count < m_totals[c]; c--);

    s.m_lowCount  = m_totals[c];
    s.m_highCount = m_totals[c+1];
    return c;
  }
#endif

  void    flushEncoder(BitOutputStream &out);
  void    outputBits(  BitOutputStream &out, ULONG code, int count);

public:
  ArithmeticCoder() {
    m_totalsMap = NULL;
  }
  ~ArithmeticCoder();
  void compress(ResetableByteInputStream &input, ByteOutputStream &output, int quality);
  void expand(  ByteInputStream          &input, ByteOutputStream &output);
};
