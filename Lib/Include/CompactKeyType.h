#pragma once

template <class T> class CompactKeyType {
private:
  T m_value;
public:
  CompactKeyType() {
  }
  inline CompactKeyType(T value) : m_value(value) {
  }
  inline unsigned long hashCode() const {
    return (unsigned long)m_value;
  }
  inline bool operator==(const CompactKeyType &k) const {
    return m_value == k.m_value;
  }
  operator T() const {
    return m_value;
  }
};

typedef CompactKeyType<int> CompactIntKeyType;
