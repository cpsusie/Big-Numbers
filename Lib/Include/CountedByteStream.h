#pragma once

#include "ByteStream.h"

class ByteCounter {
private:
  UINT64 m_counter;
public:
  ByteCounter() : m_counter(0) {
  }
  // will be called every time n bytes are read/written from/to CountedByteInput/OutputStream
  virtual void incrCount(UINT64 n) {
    m_counter += n;
  }
  virtual UINT getMaxChunkSize() const {
    return 500000;
  }
  inline UINT64 getCount() const {
    return m_counter;
  }
};

class CountedByteOutputStream : public ByteOutputStream {
private:
  ByteCounter      &m_counter;
  ByteOutputStream &m_dst;
public:
  CountedByteOutputStream(ByteCounter &counter, ByteOutputStream &dst);

  void putBytes(const BYTE *src, size_t n);
  void putByte(BYTE b);
  const ByteCounter &getCounter() const {
    return m_counter;
  }
};

class CountedByteInputStream : public ByteInputStream {
private:
  ByteCounter     &m_counter;
  ByteInputStream &m_src;
public:
  CountedByteInputStream(ByteCounter &counter, ByteInputStream &src);

  intptr_t getBytes(BYTE *dst, size_t n);
  int      getByte();
  const ByteCounter &getCounter() const {
    return m_counter;
  }
};
