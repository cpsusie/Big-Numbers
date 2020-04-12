#pragma once

template <typename T> class CompactKeyType {
private:
  T m_value;
public:
  CompactKeyType() {
  }
  inline CompactKeyType(T value) : m_value(value) {
  }
#pragma warning( push )
#pragma warning(disable:4311 4302)

  inline ULONG hashCode() const {
    if(sizeof(m_value) > sizeof(ULONG)) {
      ULONG result = 0;
      const ULONG *start = (ULONG*)&m_value;
      const ULONG *end   = start + sizeof(m_value) / sizeof(ULONG);
      for(const ULONG *p = start; p < end;) {
        result ^= *(p++);
      }
      return result;
    } else {
      return (ULONG)m_value;
    }
  }
#pragma warning( pop )

  inline bool operator==(const CompactKeyType &k) const {
    return m_value == k.m_value;
  }
  operator T() const {
    return m_value;
  }
};

class CompactStrKeyType {
private:
  const TCHAR *m_value;
public:
  CompactStrKeyType() : m_value(EMPTYSTRING) {
  }
  inline CompactStrKeyType(const TCHAR *value) : m_value(value) {
  }
#pragma warning( push )
#pragma warning(disable:4311 4302)

  inline ULONG hashCode() const {
    return strHash(m_value);
  }
#pragma warning( pop )

  inline bool operator==(const CompactStrKeyType &k) const {
    return _tcscmp(m_value,k.m_value) == 0;
  }
  operator const TCHAR*() const {
    return m_value;
  }
};

class CompactStrIKeyType {
private:
  const TCHAR *m_value;
public:
  CompactStrIKeyType() : m_value(EMPTYSTRING) {
  }
  inline CompactStrIKeyType(const TCHAR *value) : m_value(value) {
  }
#pragma warning( push )
#pragma warning(disable:4311 4302)

  inline ULONG hashCode() const {
    return striHash(m_value);
  }
#pragma warning( pop )

  inline bool operator==(const CompactStrIKeyType &k) const {
    return _tcsicmp(m_value,k.m_value) == 0;
  }
  operator const TCHAR*() const {
    return m_value;
  }
};

typedef CompactKeyType<short > CompactShortKeyType;
typedef CompactKeyType<USHORT> CompactUShortKeyType;
typedef CompactKeyType<int   > CompactIntKeyType;
typedef CompactKeyType<UINT  > CompactUIntKeyType;
typedef CompactKeyType<float > CompactFloatKeyType;
typedef CompactKeyType<double> CompactDoubleKeyType;
