#pragma once

#include <MyUtil.h>

class FieldSet {
private:
  unsigned int m_bits[2];
  FieldSet(unsigned int i0, unsigned int i1) {
    m_bits[0] = i0; m_bits[1] = i1;
  }
  friend class FieldSetIterator;
public:
  FieldSet() {
    m_bits[0] = m_bits[1] = 0;
  }
  explicit FieldSet(int f,...); // terminate with -1
  inline void clear() {
    m_bits[0] = m_bits[1] = 0;
  }
  inline void add(unsigned int pos) {
    m_bits[pos/32] |= (1<<(pos%32));
  }
  inline void remove(unsigned int pos) {
    m_bits[pos/32] &= ~(1<<(pos%32));
  }
  inline bool contains(unsigned int pos) const {
    return (m_bits[pos/32] & (1<<(pos%32))) != 0;
  }
  inline FieldSet &operator+=(unsigned int pos) {
    add(pos);
    return *this;
  }
  inline FieldSet &operator-=(unsigned int pos) {
    remove(pos);
    return *this;
  }
  inline FieldSet &operator*=(const FieldSet &s) {
    m_bits[0] &= s.m_bits[0]; m_bits[1] &= s.m_bits[1];
    return *this;
  }
  inline FieldSet &operator+=(const FieldSet &s) {
    m_bits[0] |= s.m_bits[0]; m_bits[1] |= s.m_bits[1];
    return *this;
  }
  inline FieldSet &operator-=(const FieldSet &s) {
    m_bits[0] &= ~s.m_bits[0]; m_bits[1] &= ~s.m_bits[1];
    return *this;
  }
  inline FieldSet operator*(const FieldSet &s) const {
    return FieldSet(m_bits[0] & s.m_bits[0], m_bits[1] & s.m_bits[1]);
  }
  inline FieldSet operator+(const FieldSet &s) const {
    return FieldSet(m_bits[0] | s.m_bits[0], m_bits[1] | s.m_bits[1]);
  }
  inline FieldSet operator-(const FieldSet &s) const {
    return FieldSet(m_bits[0] & ~s.m_bits[0], m_bits[1] & ~s.m_bits[1]);
  }
  inline bool isEmpty() const {
    return (m_bits[0] || m_bits[1]) == 0;
  }

  inline bool operator==(const FieldSet &s) const {
    return (m_bits[0] == s.m_bits[0]) && (m_bits[1] == s.m_bits[1]);
  }
  inline bool operator!=(const FieldSet &s) const {
    return (m_bits[0] != s.m_bits[0]) || (m_bits[1] != s.m_bits[1]);
  }
  Iterator<int> getIterator() const;
  String toString() const;
};

