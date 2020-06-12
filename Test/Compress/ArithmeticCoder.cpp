#include "stdafx.h"
#include <CountedByteStream.h>
#include "ArithmeticCoder.h"

ArithmeticCoder::~ArithmeticCoder() {
  if (m_totalsMap) {
    delete m_totalsMap;
  }
}

void ArithmeticCoder::compress(ResetableByteInputStream &input, ByteOutputStream &output, int quality) {
  ByteCounter inputCounter, outputCounter;

  CountedByteOutputStream  outc(outputCounter, output);
  BitOutputStream          out(outc);

  buildModel(input, out);
  initEncoder();

  CountedByteInputStream   in(inputCounter   , input );
  int c;
  while((c = in.getByte()) != EOF) {
    encodeSymbol(out,convertIntToSymbol(c));
  }
  encodeSymbol(out, convertIntToSymbol(endOfStream));
  flushEncoder(out);
  out.putBits(0,16);

  m_rawSize        = inputCounter.getCount();
  m_compressedSize = outputCounter.getCount();
}

void ArithmeticCoder::expand(ByteInputStream &input, ByteOutputStream &output) {
  ByteCounter              inputCounter, outputCounter;
  CountedByteInputStream   inc(inputCounter , input );
  BitInputStream           in(inc);
  CountedByteOutputStream  out(outputCounter, output);

  inputCounts(in);
  initDecoder(in);

  for(;;) {
    Symbol s;
    getSymbolScale(s);
    int count = getCurrentCount(s);
    int c = convertSymbolToInt(count,s);
    if(c == endOfStream) {
      break;
    }
    char ch = c;
    removeSymbolFromStream(in,s);
    out.putByte(c);
  }

  m_compressedSize = inputCounter.getCount();
  m_rawSize        = outputCounter.getCount();
}

void ArithmeticCoder::buildModel(ResetableByteInputStream &input, BitOutputStream &output) {
  ULONG counts[256];
  BYTE  scaledCounts[256];
  buildTotals(outputCounts(output,scaleCounts(countBytes(input,counts), scaledCounts)));
}

ULONG *ArithmeticCoder::countBytes(ResetableByteInputStream &input, ULONG *counts) {
  for(int i = 0; i < 256;) {
    counts[i++] = 0;
  }
  int c;
  while((c = input.getByte()) != EOF) {
    counts[c]++;
  }
  input.reset();
  return counts;
}

BYTE *ArithmeticCoder::scaleCounts(ULONG *counts, BYTE *scaledCounts) {
  ULONG maxCount = 0;
  for(int i = 0; i < 256; i++) {
    if(counts[i] > maxCount) {
      maxCount = counts[i];
    }
  }

  ULONG scale = maxCount / 256 + 1;
  for(int i = 0; i < 256; i++) {
    scaledCounts[i] = (BYTE)(counts[i] / scale);
    if(scaledCounts[i] == 0 && counts[i] != 0) {
      scaledCounts[i] = 1;
    }
  }

  UINT total = 1;
  for(int i = 0; i < 256; i++) {
    total += scaledCounts[i];
  }
  if(total > 32767 - 256) {
    scale = 4;
  } else if(total > 16383) {
    scale = 2;
  } else {
    return scaledCounts;
  }

  for(int i = 0; i < 256; i++) {
    scaledCounts[i] /= (BYTE)scale;
  }
  return scaledCounts;
}

void ArithmeticCoder::buildTotals(const BYTE *scaledCounts) {
  m_totals[0] = 0;
  for(int i = 0; i < endOfStream; i++) {
    m_totals[i+1] = m_totals[i] + scaledCounts[i];
  }
  m_totals[endOfStream + 1] = m_totals[endOfStream] + 1;

  for(int i = 0; i < 257; i++) {
    Symbol &s     = m_symbolTable[i];
    s.m_scale     = m_totals[endOfStream + 1];
    s.m_lowCount  = m_totals[i];
    s.m_highCount = m_totals[i+1];
  }
}

const BYTE *ArithmeticCoder::outputCounts(BitOutputStream &output, const BYTE *scaledCounts) {
  int first;
  for(first = 0; first < 255 && scaledCounts[first] == 0;) {
    first++;
  }

  int next;
  for(; first < 256; first = next) {
    int last = first + 1;
    for(;;) {
      for(; last < 256; last++) {
        if(scaledCounts[last] == 0) {
          break;
        }
      }
      last--;
      for(next = last + 1; next < 256; next++) {
        if(scaledCounts[next] != 0) {
          break;
        }
      }

      if(next > 255) {
        break;
      }
      if((next - last) > 3) {
        break;
      }
      last = next;
    }
    output.putByte(first);
    output.putByte(last );
    for(int i = first; i <= last; i++) {
      output.putByte(scaledCounts[i]);
    }
  }
  output.putByte(0);

  return scaledCounts;
}

void ArithmeticCoder::inputCounts(BitInputStream &input) {
  BYTE scaledCounts[256];

  for(int i = 0; i  < 256; i++) {
    scaledCounts[i] = 0;
  }
  int first = input.getByte();
  int last  = input.getByte();
  for(;;) {
    for(int i = first; i <= last; i++) {
      scaledCounts[i] = input.getByte();
    }
    first = input.getByte();
    if(first == 0) {
      break;
    }
    last = input.getByte();
  }
  buildTotals(scaledCounts);
#if defined(USE_FAST_SYMBOL_CONVERSION)
  buildTotalsMap();
#endif
}

void ArithmeticCoder::initEncoder() {
  m_low       = 0;
  m_high      = 0xffff;
  m_underflow = 0;
}

void ArithmeticCoder::flushEncoder(BitOutputStream &out) {
  out.putBit(m_low & 0x4000);
  m_underflow++;
  while(m_underflow-- > 0) {
    out.putBit(~m_low & 0x4000);
  }
}


#if defined(USE_FAST_SYMBOL_CONVERSION)

void  ArithmeticCoder::buildTotalsMap() {
  m_totalsMap = new USHORT[0x10000];
  int i = 0xffff, index;
  for(index = ARRAYSIZE(m_totals)-1; index >= 0;) {
    const int t = m_totals[index];
    while(i >= t) m_totalsMap[i--] = index;
    for(index--; (index >= 0) && (m_totals[index] == t); index--);
  }
}

#endif

void ArithmeticCoder::encodeSymbol(BitOutputStream &out, const Symbol &s) {
  const long range = (long)(m_high - m_low) + 1;
  m_high = m_low + (USHORT)((range * s.m_highCount ) / s.m_scale - 1);
  m_low  = m_low + (USHORT)((range * s.m_lowCount  ) / s.m_scale);

  for(;;) {
    if((m_high & 0x8000) == (m_low & 0x8000)) {
      out.putBit(m_high & 0x8000);
      while(m_underflow > 0) {
        out.putBit(~m_high & 0x8000);
        m_underflow--;
      }
    } else if((m_low & 0x4000) && !(m_high & 0x4000)) {
      m_underflow++;
      m_low  &= 0x3fff;
      m_high |= 0x4000;
    } else {
      return;
    }
    m_low  <<= 1;
    m_high <<= 1;
    m_high |= 1;
  }
}

void ArithmeticCoder::initDecoder(BitInputStream &in) {
  m_code = 0;
  for(int i = 0; i < 16; i++) {
    m_code <<= 1;
    m_code += in.getBit();
  }
  m_low  = 0;
  m_high = 0xffff;
}

void ArithmeticCoder::removeSymbolFromStream(BitInputStream &in, const Symbol &s) {
  const long range = (long)(m_high - m_low) + 1;
  m_high = m_low + (USHORT)((range * s.m_highCount) / s.m_scale - 1);
  m_low  = m_low + (USHORT)((range * s.m_lowCount ) / s.m_scale);

  for(;;) {
    if((m_high & 0x8000) == (m_low & 0x8000)) {
      // empty
    } else if((m_low & 0x4000) == 0x4000 && (m_high & 0x4000) == 0) {
      m_code ^= 0x4000;
      m_low  &= 0x3fff;
      m_high |= 0x4000;
    } else {
      return;
    }
    m_low  <<= 1;
    m_high <<= 1;
    m_high |=  1;
    m_code <<= 1;
    m_code += in.getBit();
  }
}
