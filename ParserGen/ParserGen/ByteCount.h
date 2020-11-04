#pragma once

#include <LRparser.h>

typedef enum {
  TYPE_CHAR
 ,TYPE_UCHAR
 ,TYPE_SHORT
 ,TYPE_USHORT
 ,TYPE_INT
 ,TYPE_UINT
} IntegerType;

typedef enum {
  CPP
 ,JAVA
} Language;

class ByteCount {
private:
  UINT m_countx86;
  UINT m_countx64;

  ByteCount getAlignedSize() const;
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
  // sizeof(void*) in x86 and x64
  static const ByteCount s_pointerSize;

  String toString() const;
};

// Used only for language CPP, as unsigned types does not exist int Java
IntegerType  findUintType(     UINT        maxValue);
// Used for language CPP/JAVA
IntegerType  findIntType(int minValue, int maxValue, Language = CPP);
const TCHAR *getTypeName(      IntegerType type    , Language = CPP);
// Return type of TntegerType, 1,2,4 for char, short, int (signed/unsigned)
// typesize is independent of language
UINT         getTypeSize(      IntegerType type    );
