#pragma once

template <class T> class CompactKeyType {
private:
  T m_value;
public:
  CompactKeyType() {
  }
  inline CompactKeyType(T value) : m_value(value) {
  }
#pragma warning( push )
#pragma warning(disable:4311 4302)

  inline unsigned long hashCode() const {
    if (sizeof(m_value) > sizeof(unsigned long)) {
      unsigned long result = 0;
      const unsigned long *start = (unsigned long*)&m_value;
      const unsigned long *end   = start + sizeof(m_value) / sizeof(unsigned long);
      for (const unsigned long *p = start; p < end; p++) {
        result ^= *p;
      }
      return result;
    }
    else {
      return (unsigned long)m_value;
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

typedef CompactKeyType<int> CompactIntKeyType;
