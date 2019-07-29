#pragma once

#include "ByteStream.h"
#include "ByteArray.h"

class ByteMemoryInputStream : public ByteInputStream {
private:
  const BYTE *m_p;
  size_t      m_size;
  size_t      m_pos;

  ByteMemoryInputStream(const ByteMemoryInputStream &src);            // Not defined. Class not cloneable
  ByteMemoryInputStream &operator=(const ByteMemoryInputStream &src); // Not defined. Class not cloneable
public:
  ByteMemoryInputStream(const BYTE *src);
  ByteMemoryInputStream(const ByteArray &src);

  intptr_t getBytes(BYTE *dst, size_t n);
  int getByte();
};

class ByteMemoryOutputStream : public ByteOutputStream {
private:
  BYTE      *m_p;
  size_t     m_pos;
  ByteArray *m_dstArray;

  ByteMemoryOutputStream(const ByteMemoryOutputStream &src);            // Not defined. Class not cloneable
  ByteMemoryOutputStream &operator=(const ByteMemoryOutputStream &src); // Not defined. Class not cloneable
public:
  ByteMemoryOutputStream(BYTE *dst);
  ByteMemoryOutputStream(ByteArray &dst);

  void putBytes(const BYTE *src, size_t n);
  void putByte(BYTE b);
};
