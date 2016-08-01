#pragma once

class AddrRange {
private:
  __int64 m_first;
  __int64 m_last;

public:
  AddrRange() {
    clear();
  }

  AddrRange(__int64 a1, __int64 a2);

  void clear() {
    m_first = m_last = -1;
  }

  bool isEmpty() const {
    return m_first < 0;
  }

  bool contains(__int64 addr) const;

  bool overlap(const AddrRange &range) const;

  __int64 getFirst() const {
    return isEmpty() ? 0 : m_first;
  }

  __int64 getLast() const {
    return isEmpty() ? 0 : m_last;
  }

  __int64 getLength() const {
    return isEmpty() ? 0 : m_last - m_first;
  }

  bool operator==(const AddrRange &r) const;
  bool operator!=(const AddrRange &r) const;

  String toString() const;
};
