#pragma once

#include <intrin.h>
#include "CompactArray.h"

#ifdef IS32BIT
#define BITSET_ASM_OPTIMIZED
#endif

class Packer;

class BitSet {
public:
  typedef ULONG Atom;

#define _BS_BITSINATOM      (sizeof(BitSet::Atom)*8)
#define _BS_ATOMCOUNT(size) (((size)-1) / _BS_BITSINATOM + 1)
#define _BS_ATOMINDEX(i)    ((i)/_BS_BITSINATOM)
#define _BS_ATOM(p,i)       p[_BS_ATOMINDEX(i)]
#define _BS_ATOMBIT(i)      ((BitSet::Atom)1 << ((i)%_BS_BITSINATOM))
#define _BS_MASKATOM(i)     BitSet::s_mask[i]

#ifdef _DEBUG
#define BS_CHECKINDEX(i) \
  if(i >= m_capacity) throwIndexOutOfRange(__TFUNCTION__, i, EMPTYSTRING)
#else
#define BS_CHECKINDEX(i)
#endif

  static inline UINT getPopCount(Atom a) {
    return __popcnt(a);
  }
  friend class BitSetIndex;
  friend class FileBitSetIndex;
  void getRangeTable(CompactInt64Array &rangeTable, BYTE shift) const;
private:
  static Atom s_mask[_BS_BITSINATOM+1];
  friend class InitBitSetMask;
protected:
  Atom   *m_p;
  size_t  m_capacity;
  static inline size_t getAtomCount(size_t capacity) {
    return _BS_ATOMCOUNT(capacity);
  }
  String indexOutOfRangeString(size_t index) const;
  void throwIndexOutOfRange(const TCHAR *method, size_t index, _In_z_ _Printf_format_string_ TCHAR const * const format,...) const;

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
  // Returns a random element from non empty set. throws Exception if set is empty
  size_t   select(RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) const;
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
  void setCapacity(size_t newCapacity);
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

  void save(ByteOutputStream &s) const;
  void load(ByteInputStream  &s);

  friend Packer &operator<<(Packer &p, const BitSet &s);
  friend Packer &operator>>(Packer &p,       BitSet &s);

  static const char setBitsCount[256]; // Number of set bits for each bytevalue

  ULONG  hashCode()    const;
  String toBitString() const;
  String toString(AbstractStringifier<size_t> *sf = NULL, const TCHAR *delim = _T(",")) const;
  String toStringIntervals() const;
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

inline BitSet  compl(const BitSet &s) {
  return BitSet(s).invert();
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
  inline void remove(const TCHAR *className) {
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
  AbstractIterator *clone();
  BitSetIterator(BitSet &set, size_t start=0, size_t end=-1)
    : AbstractBitSetIterator(set)
  {
    first(start, end);
  }
  void *next();
  void remove() {
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

inline String charBitSetToString(const BitSet &set, AbstractStringifier<size_t> *sf = CharacterFormater::stdAsciiFormater) {
  return ((BitSet&)set).getIterator().rangesToString(*sf);
}

class FileBitSet {
private:
  mutable ByteInputFile  m_f;
  UINT64                 m_firstAtomOffset;       // offset to the first Atom in bit-array. not bitset.capacity
  size_t                 m_capacity;
  FileBitSet(const FileBitSet &src);              // not implemented. Not cloneable
  FileBitSet &operator=(const FileBitSet &src);   // not implemented. Not cloneable
public:
  FileBitSet(const String &fileName, UINT64 startOffset);
  bool contains(size_t i) const;
  inline size_t getCapacity() const {             // Return number of bits in set = maximum number of elements in set
    return m_capacity;
  }
};

class BitSetIndex {
private:
  const BitSet     &m_bitSet;
  BYTE              m_shift;
  CompactInt64Array m_rangeTable;
  BitSetIndex(const BitSetIndex &src);            // not implemented. Not cloneable
  BitSetIndex &operator=(const BitSetIndex &src); // not implemented. Not cloneable
public:
  BitSetIndex(const BitSet &src);
  void rebuildRangeTable();                       // should be called whenever bitset is changed
  intptr_t getIndex(size_t i) const;              // faster than Bitset.getIndex(i)
  String getInfoString() const;

  void save(ByteOutputStream &s) const;           // also save bitset. to be used in FileBitSetIndex
  inline const CompactInt64Array &getRangeTable() const {
    return m_rangeTable;
  }
  inline size_t size() const {
    return m_bitSet.size();
  }
  inline size_t getCapacity() const {
    return m_bitSet.getCapacity();
  }
};

class FileBitSetIndex {                                        // use BitSetIndex saved with BitSetIndex.save
private:
  mutable ByteInputFile  m_f;
  const UINT64           m_startOffset;
  BYTE                   m_shift;
  CompactInt64Array      m_rangeTable;
  UINT64                 m_bitsStartOffset;                    // offset to the first byte in bit-array. not bitset.capacity
  BitSet                *m_loadedIntervals;                    // contains intervals already loaded. start as empty, and works as a cache
  BitSet                *m_bitSet;
  mutable size_t         m_size;
  void                   loadBitRange(intptr_t rangeIndex, size_t &bitInterval, size_t &startBit) const;
  FileBitSetIndex(const FileBitSetIndex &src);                 // not implemented. Not cloneable
  FileBitSetIndex &operator=(const FileBitSetIndex &src);      // not implemented. Not cloneable
public:
  FileBitSetIndex(const String &fileName, UINT64 startOffset); // load rangetable
  ~FileBitSetIndex();
  intptr_t getIndex(size_t i) const;                           // lazy-read bitset, saved with BitSetIndex.save
  inline const CompactInt64Array &getRangeTable() const {
    return m_rangeTable;
  }
  size_t        size() const;
  inline size_t getCapacity() const {
    return m_bitSet->getCapacity();
  }
};
