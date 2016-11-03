#pragma once

#include "MyString.h"
#include "Iterator.h"

template<class T> class TinyBitSet {
private:
  T m_bits; // T must be unsigned char,short,long or __int64 for this to compile
  inline explicit TinyBitSet(T bits) : m_bits(bits) {
  };
  inline T mask(UINT i) const {
    return (i==getCapacity()) ? -1 : (((T)1<<i) - 1);
  }
public:
  TinyBitSet() : m_bits(0) {
  }
/*
  TinyBitSet(int a,...) : m_bits(0) {
    va_list argptr;
    va_start(argptr,a);
    for(;a >= 0; a = va_arg(argptr, int)) {
      add(a);
    }
    va_end(argptr);
  }
*/
  inline bool contains(UINT i) const {
    return (m_bits & ((T)1<<i)) != 0;
  }
  inline void add(UINT i) {
    m_bits |= ((T)1<<i);
  }
  inline TinyBitSet &operator+=(UINT i) {
    add(i);
    return *this;
  }
  inline void remove(UINT i) {
    m_bits &= ~((T)1<<i);
  }
  inline TinyBitSet &operator-=(UINT pos) {
    remove(pos);
    return *this;
  }
  inline void add(UINT a, UINT b) {
    m_bits |= ~mask(a) & mask(b + 1);
  }
  inline void remove(UINT a, UINT b) {
    m_bits &= mask(a) | ~mask(b + 1);
  }
  inline void clear() {
    m_bits = 0;
  }
  inline void addAll() {
    m_bits = -1;
  }
  inline void invert() {
    m_bits = ~m_bits;
  }
  int size() const {
    int result = 0;
    for(T b = m_bits; b; b &= (b-1)) {
      result++;
    }
    return result;
  }
  UINT select() const {                     // Returns a random element from non empty set. throws Exception if set is empty
    if(isEmpty()) {
      throwException(_T("%s:Set is empty"), __TFUNCTION__);
    }
    CompactArray<unsigned char> members(64);
    T bits = m_bits;
    char *delim = NULL;
    for(unsigned char i = 0; bits; i++, bits >>= 1) {
      if(bits & 1) {
        members.add(i);
      }
    }
    return members.select();
  }

  inline UINT getCapacity() const {
    return sizeof(T) * 8;
  }
  inline bool isEmpty() const {
    return m_bits == 0;
  }
  inline TinyBitSet operator|(TinyBitSet s) const { // union
    return TinyBitSet(m_bits | s.m_bits);
  }
  inline TinyBitSet operator+(TinyBitSet s) const { // union
    return TinyBitSet(m_bits | s.m_bits);
  }
  inline TinyBitSet operator&(TinyBitSet s) const { // intersection
    return TinyBitSet(m_bits & s.m_bits);
  }
  inline TinyBitSet operator*(TinyBitSet s) const { // intersection
    return TinyBitSet(m_bits & s.m_bits);
  }
  inline TinyBitSet operator-(TinyBitSet s) const { // set difference
    return TinyBitSet(m_bits & ~s.m_bits);
  }
  inline TinyBitSet operator^(TinyBitSet s) const { // a^b = (a-b) | (b-a) (symmetric difference)
    return TinyBitSet(m_bits ^ s.m_bits);
  }
  inline TinyBitSet &operator|=(TinyBitSet s) {
    m_bits |= s.m_bits;
    return *this;
  }
  inline TinyBitSet &operator+=(TinyBitSet s) {
    m_bits |= s.m_bits;
    return *this;
  }
  inline TinyBitSet &operator&=(TinyBitSet s) {
    m_bits &= s.m_bits;
    return *this;
  }
  inline TinyBitSet &operator*=(TinyBitSet s) {
    m_bits &= s.m_bits;
    return *this;
  }
  inline TinyBitSet &operator-=(TinyBitSet s) {
    m_bits &= ~s.m_bits;
    return *this;
  }
  inline TinyBitSet &operator^=(TinyBitSet s) {
    m_bits ^= s.m_bits;
    return *this;
  }
  inline bool operator==(TinyBitSet s) const {      // equal
    return m_bits == s.m_bits;
  }
  inline bool operator!=(TinyBitSet s) const {      // not equal
    return m_bits != s.m_bits;
  }
  inline bool operator<=(TinyBitSet s) const {      // subset
    return (m_bits & s.m_bits) == m_bits;
  }
  inline bool operator<(TinyBitSet s) const {       // pure subset
    return (*this != s) && (*this <= s);
  }

  ULONG hashCode() const {
    if(sizeof(T) <= sizeof(ULONG)) {
      return m_bits;
    } else {
      return uint64Hash(m_bits);
    }
  }

  class TinyBitSetIterator : public AbstractIterator {
  private:
    TinyBitSet<T> &m_set;
    UINT           m_next;
    UINT           m_current;
    bool           m_hasNext;
    void first() {
      if(m_set.isEmpty()) {
        m_hasNext = false;
      } else {
        m_hasNext = true;
        for(m_next = 0;;m_next++) {
          if(m_set.contains(m_next)) {
            break;
          }
        }
      }
    }
  public:
    TinyBitSetIterator(TinyBitSet<T> &set) : m_set(set) { first(); }
    AbstractIterator *clone() {
      return new TinyBitSetIterator(*this);
    }
    inline bool hasNext() const {
      return m_hasNext;
    }
    void *next() {
      if(!m_hasNext) {
        noNextElementError(_T("TinyBitSetIterator"));
      }
      for(m_current = m_next; ++m_next < m_set.getCapacity();) {
        if(m_set.contains(m_next)) {
          break;
        }
      }
      m_hasNext = (m_next < m_set.getCapacity());
      return &m_current;
    }
    void remove() {;
      m_set.remove(m_current);
    }
  };

  class TinyBitSetReverseIterator : public AbstractIterator {
  private:
    TinyBitSet<T> &m_set;
    int            m_next;
    UINT           m_current;
    bool           m_hasNext;
    void first() {
      if(m_set.isEmpty()) {
        m_hasNext = false;
      } else {
        m_hasNext = true;
        for(m_next = m_set.getCapacity()-1;;m_next--) {
          if(m_set.contains(m_next)) {
            break;
          }
        }
      }
    }
  public:
    TinyBitSetReverseIterator(TinyBitSet<T> &set) : m_set(set) { first(); }
    AbstractIterator *clone() {
      return new TinyBitSetReverseIterator(*this);
    }
    inline bool hasNext() const {
      return m_hasNext;
    }
    void *next() {
      if(!m_hasNext) {
        noNextElementError(_T("TinyBitSetReverseIterator"));
      }
      for(m_current = m_next; --m_next >= 0;) {
        if(m_set.contains(m_next)) {
          break;
        }
      }
      m_hasNext = (m_next >= 0);
      return &m_current;
    }
    void remove() {
      m_set.remove(m_current);
    }
  };

  Iterator<UINT> getIterator() {        // iterator that iterates elements of bitset in ascending  order
    return Iterator<UINT>(new TinyBitSetIterator(*this));
  }
  Iterator<UINT> getReverseIterator() { // iterator that iterates elements of bitset in descending order
    return Iterator<UINT>(new TinyBitSetReverseIterator(*this));
  }

  String toString() const {
    String result = _T("(");
    T bits = m_bits;
    TCHAR *delim = NULL;
    for(int i = 0; bits; i++, bits >>= 1) {
      if(bits & 1) {
        if(delim) result += delim; else delim = _T(",");
        result += format(_T("%d"), i);
      }
    }
    return result + _T(")");
  }

  static TinyBitSet all() {
    return TinyBitSet(-1);
  }
};

class BitSet8 : public TinyBitSet<BYTE> {
};

class BitSet16 : public TinyBitSet<USHORT> {
};

class BitSet32 : public TinyBitSet<ULONG> {
};

class BitSet64 : public TinyBitSet<UINT64> {
};
