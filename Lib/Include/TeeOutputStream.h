#pragma once

#include "ByteStream.h"

class TeeOutputStream : public ByteOutputStream {
private:
  ByteOutputStream &m_s1, &m_s2;

  TeeOutputStream(const TeeOutputStream &src);            // Not defined. Class not cloneable
  TeeOutputStream &operator=(const TeeOutputStream &src); // Not defined. Class not cloneable
public:
  TeeOutputStream(ByteOutputStream &s1, ByteOutputStream &s2) : m_s1(s1), m_s2(s2) {
  }

  void putBytes(const BYTE *src, UINT n);
  void putByte(BYTE b);
};
