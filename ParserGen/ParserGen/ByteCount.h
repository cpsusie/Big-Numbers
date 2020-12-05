#pragma once

#include "AbstractParserTables.h"

typedef enum {
  TYPE_CHAR     // [CHAR_MIN..CHAR_MAX ]
 ,TYPE_UCHAR    // [       0..UCHAR_MAX]
 ,TYPE_SHORT    // [SHRT_MIN..SHRT_MAX ]
 ,TYPE_USHORT   // [       0..USHRT_MAX]
 ,TYPE_INT      // [INT_MIN ..INT_MAX  ]
 ,TYPE_UINT     // [       0..UINT_MAX ]
} IntegerType;

// Used for language CPP/JAVA
IntegerType  findIntType(int minValue, int maxValue);
const TCHAR *getTypeName(IntegerType type);
// Return type of TntegerType, 1,2,4 for char, short, int (signed/unsigned)
// typesize is independent of language
UINT         getTypeSize(IntegerType type);

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
  inline ByteCount operator-(const ByteCount &rhs) const {
    return ByteCount(m_countx86 - rhs.m_countx86, m_countx64 - rhs.m_countx64);
  }

  friend inline ByteCount operator*(UINT n, const ByteCount &c) {
    return ByteCount(n*c.m_countx86,n*c.m_countx64);
  }
  friend inline ByteCount operator*(const ByteCount &c, UINT n) {
    return ByteCount(n*c.m_countx86,n*c.m_countx64);
  }

  static inline ByteCount wordAlignedSize(const ByteCount &c, UINT n) { // static
    return (n * c).getAlignedSize();
  }

  static inline ByteCount wordAlignedSize(UINT size) {
    return wordAlignedSize(ByteCount(size,size),1);
  }
  // compare m_countx64 first, then compare m_contx86
  int compare(const ByteCount &rhs) const;

  // sizeof(void*) in x86 and x64
  static const ByteCount s_pointerSize;

  String toString() const;
  String toStringTableForm() const;
  static constexpr UINT tableformWidth = 19;
};

inline bool operator==(const ByteCount &bc1, const ByteCount &bc2) { return bc1.compare(bc2) == 0; }
inline bool operator!=(const ByteCount &bc1, const ByteCount &bc2) { return bc1.compare(bc2) != 0; }
inline bool operator<=(const ByteCount &bc1, const ByteCount &bc2) { return bc1.compare(bc2) <= 0; }
inline bool operator>=(const ByteCount &bc1, const ByteCount &bc2) { return bc1.compare(bc2) >= 0; }
inline bool operator< (const ByteCount &bc1, const ByteCount &bc2) { return bc1.compare(bc2) <  0; }
inline bool operator> (const ByteCount &bc1, const ByteCount &bc2) { return bc1.compare(bc2) >  0; }

