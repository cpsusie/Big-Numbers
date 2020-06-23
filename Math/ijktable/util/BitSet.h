#pragma once

#include <intrin.h>
#include <vector>
#include "util.h"
#include "iterator.h"

#ifdef IS32BIT
#define BITSET_ASM_OPTIMIZED
#endif

class BitSet {
public:
  typedef ulong Atom;

#define _BS_BITSINATOM      (sizeof(BitSet::Atom)*8)
#define _BS_ATOMCOUNT(size) (((size)-1) / _BS_BITSINATOM + 1)
#define _BS_ATOMINDEX(i)    ((i)/_BS_BITSINATOM)
#define _BS_ATOM(p,i)       p[_BS_ATOMINDEX(i)]
#define _BS_ATOMBIT(i)      ((BitSet::Atom)1 << ((i)%_BS_BITSINATOM))
#define _BS_MASKATOM(i)     BitSet::s_mask[i]

#ifdef _DEBUG
#define BS_CHECKINDEX(i) \
  if(i >= m_capacity) throwIndexOutOfRange(__FUNCTION__, i, EMPTYSTRING)
#else
#define BS_CHECKINDEX(i)
#endif

  static inline uint getPopCount(Atom a) {
    return __popcnt(a);
  }
  friend class BitSetIndex;
private:
  static Atom s_mask[_BS_BITSINATOM+1];
  friend class InitBitSetMask;
protected:
  Atom   *m_p;
  size_t  m_capacity;
  static inline size_t getAtomCount(size_t capacity) {
    return _BS_ATOMCOUNT(capacity);
  }
  string indexOutOfRangeString(size_t index) const;
  void throwIndexOutOfRange(const char *method, size_t index, _In_z_ _Printf_format_string_ char const * const format,...) const;

public:
  // can store integers from [0..capacity-1]
  explicit BitSet(size_t capacity);
  BitSet(const BitSet &set);
  ~BitSet() {
    SAFEDELETEARRAY(m_p);
  }
  BitSet &operator=(const BitSet &rhs);
  // Add i to the set, if it's not already there
  inline BitSet &operator+=(size_t i) {
    return add(i);
  }
  // Remove i from the set if it's there
  inline BitSet &operator-=(size_t i) {
    return remove(i);
  }
  // this = this + rhs
  BitSet &operator+=(const BitSet &rhs);
  // this = this - rhs
  BitSet &operator-=(const BitSet &rhs);
  // this = this * rhs
  BitSet &operator*=(const BitSet &rhs);
  // this = this ^ rhs
  BitSet &operator^=(const BitSet &rhs);
  // Same as +=
  inline BitSet &operator|=(const BitSet &rhs) {
    return *this += rhs;
  }
  // Same as *=
  inline BitSet &operator&=(const BitSet &rhs) {
    return *this *= rhs;
  }
  // Subset. return true if all elements from lts are in rhs too
  friend bool    operator<=(const BitSet &lts, const BitSet &rhs);
  // Pure subset. Same as (lts < rhs) && (lts != rhs)
  friend inline bool operator<(const BitSet &lts, const BitSet &rhs) {
    return (lts <= rhs) && (lts != rhs);
  }
  // Same as rhs <= lts
  friend inline bool operator>=(const BitSet &lts, const BitSet &rhs) {
    return rhs <= lts;
  }
  // Same as rhs <  lts
  friend inline bool operator>(const BitSet &lts, const BitSet &rhs) {
    return rhs < lts;
  }
  // Equal. return true if the two sets contain the same elements
  friend inline bool operator==(const BitSet &lts, const BitSet &rhs) {
    return bitSetCmp(lts,rhs) == 0;
  }
  // Same as !(lts == rhs)
  friend inline bool operator!=(const BitSet &lts, const BitSet &rhs) {
    return bitSetCmp(lts,rhs) != 0;
  }
  // NB! NOT the same as relational operators subset,pure subset.
  friend int     bitSetCmp( const BitSet &i1, const BitSet &i2);
  // Number of elements. fast version
  size_t         size() const;
  // Return true if set is empty
  bool           isEmpty() const;
  // Remove i from set
  inline BitSet &remove(size_t i) {
    BS_CHECKINDEX(i);
    _BS_ATOM(m_p,i) &= ~_BS_ATOMBIT(i);
    return *this;
  }
  // Insert i into set
  inline BitSet &add(     size_t i) {
    BS_CHECKINDEX(i);
    _BS_ATOM(m_p,i) |= _BS_ATOMBIT(i);
    return *this;
  }
  // Remove interval [a;b] from set
  BitSet  &remove(  size_t a, size_t b);
  // Insert interval [a;b] into set
  BitSet  &add(     size_t a, size_t b);
  // Return if set contains i
  inline bool contains(size_t i) const {
    if(i >= m_capacity) return false;
    return (_BS_ATOM(m_p,i) & _BS_ATOMBIT(i)) ? true : false;
  }
  // Return number of elements < i. if i not in set -1 is returned
  intptr_t getIndex(size_t i) const;
  // Return number of elements between from and to. both included.
  size_t   getCount(size_t from, size_t to) const;
  // Remove all elements from set
  inline BitSet &clear() {
    memset(m_p, 0, getAtomCount() * sizeof(Atom));
    return *this;
  }
  // Complementset
  BitSet  &invert();
  // Return number of bits in set = maximum number of elements in set
  inline size_t getCapacity() const {
    return m_capacity;
  }
  // Return *this
  BitSet &setCapacity(size_t newCapacity);
  inline size_t getAtomCount() const {
     return _BS_ATOMCOUNT(m_capacity);
  }
  inline const Atom *getFirstAtom() const {
    return m_p;
  }
  inline const Atom *getLastAtom() const {
    return getFirstAtom() + getAtomCount() - 1;
  }
  inline size_t getAtomIndex(size_t i) const {
    return _BS_ATOMINDEX(i);
  }

  // Iterates elements of bitset in ascending  order,
  // beginning from smallest element >= start
  Iterator<size_t> getIterator(       size_t start =  0, size_t end=-1);

  // Iterates elements of bitset in descending order,
  // beginning from biggest element <= start
  Iterator<size_t> getReverseIterator(size_t start = -1, size_t end= 0);

  friend class BitSetIterator;
  friend class BitSetReverseIterator;

  static const char setBitsCount[256]; // Number of set bits for each bytevalue
  string toString(AbstractStringifier<size_t> *sf, const char *delim = ",") const;

};

// Union        = set containing all elements from lts and rhs
inline BitSet operator+(const BitSet &lts, const BitSet &rhs) { // union
  return (lts.getCapacity() >= rhs.getCapacity()) ? (BitSet(lts) += rhs) : (BitSet(rhs) += lts);
}

// Intersection = set containing elements contained in both lts and rhs
inline BitSet operator*(const BitSet &lts, const BitSet &rhs) { // intersection
  return (lts.getCapacity() <= rhs.getCapacity()) ? (BitSet(lts) *= rhs) : (BitSet(rhs) *= lts);
}

// a^b = (a-b) | (b-a) (symmetric difference) = set containg elements that are in only lts or rhs, but not both
inline BitSet operator^(const BitSet &lts, const BitSet &rhs) { // xor, ie symmetric difference
  return (lts.getCapacity() >= rhs.getCapacity()) ? (BitSet(lts) ^= rhs) : (BitSet(rhs) ^= lts);
}

// Difference   = set containing elements from lts without the elements from rhs
inline BitSet operator-(const BitSet &lts, const BitSet &rhs) {
  return BitSet(lts) -= rhs;
}
// Same as +
inline BitSet operator|(const BitSet &lts, const BitSet &rhs) {
  return lts + rhs;
}
// Same as *
inline BitSet operator&(const BitSet &lts, const BitSet &rhs) {
  return lts * rhs;
}

class AbstractBitSetIterator : public AbstractIterator {
protected:
  BitSet &m_s;
  size_t  m_next, m_current, m_end;
  bool    m_hasNext;
  inline AbstractBitSetIterator(BitSet &set) : m_s(set) {
  }
  inline bool hasCurrent() const {
    return m_current < m_s.getCapacity();
  }
  inline void setCurrentUndefined() {
    m_current = -1;
  }
  inline void remove(const char *className) {
    if(!hasCurrent()) noCurrentElementError(className);
    m_s.remove(m_current);
    setCurrentUndefined();
  }

public:
  bool hasNext() const {
    return m_hasNext;
  }
};

class BitSetIterator : public AbstractBitSetIterator {
private:
  DECLARECLASSNAME;
  void first(size_t start, size_t end);
public:
  BitSetIterator(BitSet &set, size_t start=0, size_t end=-1)
    : AbstractBitSetIterator(set)
  {
    first(start, end);
  }
  AbstractIterator *clone() override;
  void *next()              override;
  void  remove()            override {
    AbstractBitSetIterator::remove(s_className);
  }
};

class BitSetReverseIterator : public AbstractBitSetIterator {
private:
  DECLARECLASSNAME;
  void first(size_t start, size_t end);
public:
  AbstractIterator *clone();
  BitSetReverseIterator(BitSet &set, size_t start=-1, size_t end=0)
    : AbstractBitSetIterator(set)
  {
    first(start, end);
  }
  void *next();
  void remove() {
    AbstractBitSetIterator::remove(s_className);
  }
};

inline string charBitSetToString(const BitSet &set, AbstractStringifier<size_t> *sf = CharacterFormater::stdAsciiFormater) {
  return ((BitSet&)set).getIterator().rangesToString(*sf);
}

