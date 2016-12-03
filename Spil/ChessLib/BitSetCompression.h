#pragma once

#include <ByteStream.h>
#include <BitSet.h>
#include <Compressor.h>

class BitSetInputStream : public ResetableByteInputStream {
private:
  const BitSet    &m_src;
  Iterator<size_t> m_it;
  size_t           m_current, m_itValue;
public:
  BitSetInputStream(const BitSet &src) : m_src(src), m_it(NULL) {
    reset();
  }
  void reset() {
    m_it = ((BitSet&)m_src).getIterator();
    if(m_it.hasNext()) {
      m_current = 0;
      m_itValue = m_it.next();
    } else {
      m_current = -1;
      m_itValue = 0;
    }
  }
  intptr_t getBytes(BYTE *dst, size_t n) { // Must return the actual number of bytes read.
    BYTE *p;
    for (p = dst; n--;) {
      const int b = getByte();
      if(b == EOF) break;
      *(p++) = (BYTE)b;
    }
    return p - dst;
  }
  int getByte() {                    // Must return EOF when end-of-stream
    if(m_current < m_itValue) {
      m_current++;
      return 0;
    } else if(m_current == m_itValue) {
      m_current++;
      m_itValue = m_it.hasNext() ? m_it.next() : 0;
      return 1;
    }
    return EOF;
  }
};

class BitSetOutputStream : public ByteOutputStream {
private:
  BitSet &m_dst;
  size_t  m_counter;
public:
  BitSetOutputStream(BitSet &dst) : m_dst(dst) {
    m_counter = 0;
  }
  void putBytes(const BYTE *src, size_t n) {
    while (n--) putByte(*(src++));
  }
  void putByte(BYTE b) {
    switch (b) {
    case 0 :
      m_counter++;
      break;
    case 1 :
      m_dst.add(m_counter++);
      break;
    default:
      throwInvalidArgumentException(__TFUNCTION__, _T("b=%d"), b);
    }
  }
};

class BitSetCompressor {
private:
  Compressor &m_compressor;
public:
  BitSetCompressor(Compressor &compressor) : m_compressor(compressor) {
  }
  String compress(const BitSet &src, ByteOutputStream &out) {
    const INT64 capacity = src.getCapacity();
    out.putBytes((BYTE*)&capacity, sizeof(capacity));
    m_compressor.compress(BitSetInputStream(src), out);
    return m_compressor.getRatios();
  }

  String expand(ByteInputStream &in, BitSet &dst) {
    INT64 capacity;
    in.getBytesForced((BYTE*)&capacity, sizeof(capacity));
    dst.setCapacity(capacity);
    m_compressor.expand(in, BitSetOutputStream(dst));
    return m_compressor.getRatios();
  }
};
