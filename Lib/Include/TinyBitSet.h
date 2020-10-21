#pragma once

#include "CompactArray.h"
#include <intrin.h>

// Add no virtual functions
template<typename T> class TinyBitSet {
private:
  static inline UINT getSize(BYTE b) {
    return __popcnt(b);
  }
  static inline UINT getSize(USHORT s) {
    return (UINT)__popcnt16(s);
  }
  static inline UINT getSize(ULONG i) {
    return (UINT)__popcnt(i);
  }
  static inline UINT getSize(UINT64 i) {
#if defined(IS64BIT)
    return (UINT)__popcnt64(i);
#else
    return getSize((ULONG)i) + getSize((ULONG)(i>>32));
#endif
  }
  void indexError(const TCHAR *method, UINT i) const {
    throwInvalidArgumentException(method, _T("Value i=%u out of range, capacity=%u"), i, getCapacity());
  }
  void throwInvalidAB(const TCHAR *method, UINT a, UINT b) const {
    throwInvalidArgumentException(method, _T("(a=%u,b=%u), capacity=%u"), a,b, getCapacity());
  }

  T m_bits; // T must be unsigned char,short,long or __int64 for this to compile
protected:
  inline explicit TinyBitSet(T bits) : m_bits(bits) {
  };
public:
  inline TinyBitSet() : m_bits(0) {
  }
  inline T getBits() const {
    return m_bits;
  }
  inline UINT getCapacity() const {
    return sizeof(T) * 8;
  }
  inline T mask(UINT i) const {
    return (i>=getCapacity()) ? -1 : (((T)1<<i) - 1);
  }
  inline bool contains(UINT i) const {
    return (i>=getCapacity()) ? false : ((m_bits & ((T)1<<i)) != 0);
  }
  inline void add(UINT i) {
    if(i >= getCapacity()) indexError(__TFUNCTION__, i);
    m_bits |= ((T)1<<i);
  }
  inline TinyBitSet &operator+=(UINT i) {
    add(i);
    return *this;
  }
  inline void remove(UINT i) {
    if(i >= getCapacity()) indexError(__TFUNCTION__, i);
    m_bits &= ~((T)1<<i);
  }
  inline TinyBitSet &operator-=(UINT i) {
    remove(i);
    return *this;
  }
  inline void add(UINT a, UINT b) {
    if(b >= getCapacity()) throwInvalidAB(__TFUNCTION__, a,b);
    if(a > b) return;
    m_bits |= ~mask(a) & mask(b + 1);
  }
  inline void remove(UINT a, UINT b) {
    if(b >= getCapacity()) throwInvalidAB(__TFUNCTION__, a,b);
    if(a > b) return;
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
  inline UINT size() const {
    return getSize(m_bits);
  }
  UINT select(RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) const {                     // Returns a random element from non empty set. throws Exception if set is empty
    if(isEmpty()) throwSelectFromEmptyCollectionException(__TFUNCTION__);
    CompactArray<BYTE> members(64);
    T bits = m_bits;
    for(BYTE i = 0; bits; i++, bits >>= 1) {
      if(bits & 1) {
        members.add(i);
      }
    }
    return members.select(rnd);
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

  inline operator T() const {
    return m_bits;
  }
  class TinyBitSetIterator : public AbstractIterator {
  private:
    TinyBitSet &m_set;
    UINT        m_next, m_current, m_end;
    bool        m_hasNext;

    inline void setCurrentUndefined() {
      m_current = -1;
    }
    void first(UINT start, UINT end) {
      m_end     = min(m_set.getCapacity()-1, end);
      m_hasNext = false;
      setCurrentUndefined();
      if(start > m_end) return;
      if((m_set.getBits() & ~m_set.mask(start))) {
        for(;;start++) {
          if(m_set.contains(start)) {
            m_hasNext = (m_next = start) <= m_end;
            return;
          }
        }
      }
    }
  public:
    inline TinyBitSetIterator(const TinyBitSet *set, UINT start, UINT end) : m_set(*(TinyBitSet*)set) {
      first(start, end);
    }
    AbstractIterator *clone()         override {
      return new TinyBitSetIterator(*this);
    }
    inline bool       hasNext() const override {
      return m_hasNext;
    }
    void             *next()          override {
      if(!m_hasNext) {
        noNextElementError(_T("TinyBitSetIterator"));
      }
      if((m_current = m_next++) >= m_end) {
        m_hasNext = false;
        return &m_current;
      }
      if(!(m_set.getBits() & ~m_set.mask(m_next))) {
        m_hasNext = false;
      } else {
        for(;;m_next++) {
          if(m_set.contains(m_next)) {
            m_hasNext = (m_next <= m_end);
            break;
          }
        }
      }
      return &m_current;
    }

    void              remove()        override {
      m_set.remove(m_current);
    }
  };

  class TinyBitSetReverseIterator : public AbstractIterator {
  private:
    TinyBitSet &m_set;
    UINT        m_next, m_current, m_end;
    bool        m_hasNext;

    inline void setCurrentUndefined() {
      m_current = -1;
    }
    void first(UINT start, UINT end) {
      start     = min(start, m_set.getCapacity()-1);
      m_hasNext = false;
      m_end     = end;
      setCurrentUndefined();
      if(start < m_end) {
        return;
      }
      if(m_set.getBits() & m_set.mask(start+1)) {
        for(;;start--) {
          if(m_set.contains(start)) {
            m_hasNext = (m_next = start) >= m_end;
            return;
          }
        }
      }
    }
  public:
    inline TinyBitSetReverseIterator(const TinyBitSet *set, UINT start, UINT end) : m_set(*(TinyBitSet*)set) {
      first(start, end);
    }
    AbstractIterator *clone()         override {
      return new TinyBitSetReverseIterator(*this);
    }
    bool              hasNext() const override {
      return m_hasNext;
    }
    void             *next()          override {
      if(!m_hasNext) {
        noNextElementError(_T("TinyBitSetReverseIterator"));
      }
      if((m_current = m_next) <= m_end) {
        m_hasNext = false;
        return &m_current;
      }
      if((m_set.getBits() & m_set.mask(m_next--)) == 0) {
        m_hasNext = false;
      } else {
        for(;;m_next--) {
          if(m_set.contains(m_next)) {
            m_hasNext = (m_next >= m_end);
            break;
          }
        }
      }
      return &m_current;
    }

    void remove()                   override {
      m_set.remove(m_current);
    }
  };

  // Iterates elements of bitset in ascending order,
  // beginning from smallest element >= start
  ConstIterator<UINT> getIterator(UINT start = 0, UINT end = -1) const {
    return ConstIterator<UINT>(new TinyBitSetIterator(this, start, end));
  }
  Iterator<UINT> getIterator(UINT start = 0, UINT end = -1) {
    return Iterator<UINT>(new TinyBitSetIterator(this, start, end));
  }

  // Iterates elements of bitset in descending order,
  // beginning from biggest element <= start
  ConstIterator<UINT> getReverseIterator(UINT start = -1, UINT end = 0) const {
    return ConstIterator<UINT>(new TinyBitSetReverseIterator(this, start, end));
  }
  Iterator<UINT> getReverseIterator(UINT start = -1, UINT end = 0) {
    return Iterator<UINT>(new TinyBitSetReverseIterator(this, start, end));
  }

  String toString(AbstractStringifier<UINT> *sf = nullptr, const TCHAR *delim = _T(",")) const {
    return sf ? getIterator().toString(*sf, delim) : getIterator().toString(UIntStringifier(),delim);
  }

  static TinyBitSet all() {
    return TinyBitSet(-1);
  }
};

class BitSet8 : public TinyBitSet<BYTE> {
public:
  inline BitSet8() {}
  inline BitSet8(const TinyBitSet<BYTE> &src) : TinyBitSet<BYTE>(src) {
  }
};

class BitSet16 : public TinyBitSet<USHORT> {
public:
  inline BitSet16() {}
  inline BitSet16(const TinyBitSet<USHORT> &src) : TinyBitSet<USHORT>(src) {
  }
  inline BitSet16(const TinyBitSet<BYTE>   &src) : TinyBitSet<USHORT>(src.getBits()) {
  }
};

class BitSet32 : public TinyBitSet<ULONG> {
public:
  inline BitSet32() {}
  inline BitSet32(const TinyBitSet<ULONG>  &src) : TinyBitSet<ULONG>(src) {
  }
  inline BitSet32(const TinyBitSet<USHORT> &src) : TinyBitSet<ULONG>(src.getBits()) {
  }
  inline BitSet32(const TinyBitSet<BYTE>   &src) : TinyBitSet<ULONG>(src.getBits()) {
  }
};

class BitSet64 : public TinyBitSet<UINT64> {
public:
  inline BitSet64() {}
  inline BitSet64(const TinyBitSet<UINT64> &src) : TinyBitSet<UINT64>(src) {
  }
  inline BitSet64(const TinyBitSet<ULONG>  &src) : TinyBitSet<UINT64>(src.getBits()) {
  }
  inline BitSet64(const TinyBitSet<USHORT> &src) : TinyBitSet<UINT64>(src.getBits()) {
  }
  inline BitSet64(const TinyBitSet<BYTE>   &src) : TinyBitSet<UINT64>(src.getBits()) {
  }
};
