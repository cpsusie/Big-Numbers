#pragma once

#include <LRparser.h>

class ByteCount {
private:
  UINT m_countx86;
  UINT m_countx64;

  ByteCount getAlignedSize() const {
    const int restx86 = m_countx86%4, restx64 = m_countx64%8;
    return ByteCount(restx86 ? (m_countx86 + (4-restx86)) : m_countx86
                    ,restx64 ? (m_countx64 + (8-restx64)) : m_countx64);
  }
public:
  inline ByteCount() : m_countx86(0), m_countx64(0) {
  }
  inline ByteCount(UINT countx86, UINT countx64) : m_countx86(countx86), m_countx64(countx64) {
  }
  inline void clear() {
    m_countx86 = m_countx64 = 0;
  }
  inline UINT getByteCount(Platform platform) const {
    return (platform == PLATFORM_X86) ? m_countx86 : m_countx64;
  }
  inline bool isEmpty() const {
    return (m_countx86 == 0) && (m_countx64 == 0);
  }
  inline ByteCount operator+(const ByteCount &rhs) const {
    return ByteCount(m_countx86+rhs.m_countx86, m_countx64+rhs.m_countx64);
  }
  inline ByteCount &operator+=(const ByteCount &rhs) {
    m_countx86 += rhs.m_countx86;
    m_countx64 += rhs.m_countx64;
    return *this;
  }

  friend inline ByteCount operator*(UINT n, const ByteCount &c) {
    return ByteCount(n*c.m_countx86,n*c.m_countx64);
  }
  friend inline ByteCount operator*(const ByteCount &c, UINT n) {
    return ByteCount(n*c.m_countx86,n*c.m_countx64);
  }

  static inline ByteCount wordAlignedSize(const ByteCount &c, UINT n) { // static
    return (n *c).getAlignedSize();
  }

  static inline ByteCount wordAlignedSize(UINT size) {
    return wordAlignedSize(ByteCount(size,size),1);
  }
  static const ByteCount s_pointerSize;

  inline String toString() const {
    return format(_T("%s(x86)/%s(x64) bytes")
                 ,format1000(m_countx86).cstr()
                 ,format1000(m_countx64).cstr());
  }
};

typedef enum {
  TYPE_CHAR
 ,TYPE_UCHAR
 ,TYPE_SHORT
 ,TYPE_USHORT
 ,TYPE_INT
 ,TYPE_UINT
} IntegerType;

IntegerType  findUintType(     UINT        maxValue);
const TCHAR *getTypeName(      IntegerType type    );
UINT         getTypeSize(      IntegerType type    );
