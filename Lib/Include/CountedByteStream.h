#pragma once

#include "ByteStream.h"

class ByteCounter {
public:
  virtual void incrCount(unsigned int n) = 0; // will be called every time n bytes are read/written from/to CountedByteInput/OutputStream
  virtual unsigned int getMaxChunkSize() const {
    return 500000;
  }
};

class CountFileOffset : public ByteCounter {
private:
  int m_counter;
public:
  CountFileOffset() : m_counter(0) {
  }
  void incrCount(unsigned int n) {
    m_counter += n;
  }
  unsigned int getByteOffset() const {
    return m_counter;
  }
};

class CountedByteOutputStream : public ByteOutputStream {
private:
  ByteCounter      &m_counter;
  ByteOutputStream &m_dst;
public:
  CountedByteOutputStream(ByteCounter &counter, ByteOutputStream &dst);

  void putBytes(const BYTE *src, unsigned int n);
  void putByte(BYTE b);
};

class CountedByteInputStream : public ByteInputStream {
private:
  ByteCounter     &m_counter;
  ByteInputStream &m_src;
public:
  CountedByteInputStream(ByteCounter &counter, ByteInputStream &src);

  int getBytes(BYTE *dst, unsigned int n);
  int getByte();
};
