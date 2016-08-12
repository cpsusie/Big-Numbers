#pragma once

#include "CompactArray.h"
#include "Iterator.h"
#include "Packer.h"
#include "MatrixDimension.h"

#define _BITSET_ATOMSIZE 32

class BitSet {
private:
  size_t selectRandomNonEmptyAtom() const;
public:
#if _BITSET_ATOMSIZE == 32
  typedef unsigned long  Atom;
#elif _BITSET_ATOMSIZE == 16
  typedef unsigned short Atom;
#else
#error "set::_BITSET_ATOMSIZE must be 16 or 32"
#endif

  friend class BitSetIndex;
  friend class BitSetFileIndex;
  void getRangeTable(CompactArray<size_t> &rangeTable, unsigned char shift) const;
private:
  static Atom mask[_BITSET_ATOMSIZE+1];
  friend class InitBitSetMask;
protected:
  Atom   *m_p;
  size_t  m_capacity;
  static size_t getAtomCount(size_t capacity);
public:
  explicit BitSet(size_t capacity); // can store integers from [0..capacity-1]
  BitSet(const BitSet &set);
  ~BitSet();
  BitSet &operator=(const BitSet &rhs);
  friend BitSet  operator+ (const BitSet &lts, const BitSet &rhs);      // Union        = set containing all elements from lts and rhs
  friend BitSet  operator- (const BitSet &lts, const BitSet &rhs);      // Difference   = set containing elements from lts without the elements from rhs
  friend BitSet  operator* (const BitSet &lts, const BitSet &rhs);      // Intersection = set containing elements contained in both lts and rhs 
  friend BitSet  operator^ (const BitSet &lts, const BitSet &rhs);      // a^b = (a-b) | (b-a) (symmetric difference) = set containg elements that are in only lts or rhs, but not both

  friend BitSet  operator| (const BitSet &lts, const BitSet &rhs) {     // Same as +
    return lts + rhs;
  }

  friend BitSet  operator& (const BitSet &lts, const BitSet &rhs) {     // Same as *
    return lts * rhs;
  }

  BitSet &operator+=(size_t i) {                                 // Add i to the set, if it's not already there
    return add(i);
  }

  BitSet &operator-=(size_t i) {                                 // Remove i from the set if it's there
    return remove(i);
  }

  BitSet &operator+=(const BitSet &rhs);                                // this = this + rhs
  BitSet &operator-=(const BitSet &rhs);                                // this = this - rhs
  BitSet &operator*=(const BitSet &rhs);                                // this = this * rhs
  BitSet &operator^=(const BitSet &rhs);                                // this = this ^ rhs

  BitSet &operator|=(const BitSet &rhs) {                               // Same as +=
    return *this += rhs;
  } 

  BitSet &operator&=(const BitSet &rhs) {                               // Same as *=
    return *this *= rhs;
  }

  friend bool    operator<=(const BitSet &lts, const BitSet &rhs);      // Subset. return true if all elements from lts are in rhs too
  friend bool    operator< (const BitSet &lts, const BitSet &rhs);      // Pure subset. Same as (lts < rhs) && (lts != rhs)
  friend bool    operator>=(const BitSet &lts, const BitSet &rhs);      // Same as rhs <= lts
  friend bool    operator> (const BitSet &lts, const BitSet &rhs);      // Same as rhs <  lts
  friend bool    operator==(const BitSet &lts, const BitSet &rhs);      // Equal. return true if the two sets contain the same elements
  friend bool    operator!=(const BitSet &lts, const BitSet &rhs);      // Same as !(lts == rhs)
  friend int     bitSetCmp( const BitSet &i1, const BitSet &i2);            // NB! NOT the same as relational operators subset,pure subset.
  friend BitSet  compl(const BitSet &s);                                // Complementset
  size_t         oldsize() const;                                       // Number of elements, slow version
  size_t         size() const;                                          // Number of elements. fast version
  bool           isEmpty() const;                                       // Return true if set is empty
  BitSet &remove(size_t i);                                      // Remove i from set
  BitSet &add(   size_t i);                                      // Insert i into set
  BitSet &remove(size_t a, size_t b);                     // Remove interval [a;b] from set
  BitSet &add(   size_t a, size_t b);                     // Insert interval [a;b] into set
  bool    contains(size_t i) const;                              // Return if set contains i
  intptr_t     getIndex(size_t i) const;                              // Return number of elements < i. if i not in set -1 is returned
  size_t getCount(size_t from, size_t to) const;      // Return number of elements between from and to. both included.
  size_t select() const;                                          // Returns a random element from non empty set. throws Exception if set is empty
  BitSet &clear();                                                      // Remove all elements from set
  BitSet &invert();                                                     // Complementset
  inline size_t getCapacity() const {                                   // Return number of bits in set = maximum number of elements in set
    return m_capacity;
  }
  void setCapacity(size_t newCapacity);
  size_t getAtomCount() const;
  const Atom *getFirstAtom() const {
    return m_p;
  }
  size_t getAtomIndex(size_t i) const;
  String toString() const;
  String toStringIntervals() const;

  unsigned long hashCode() const;
  tostream &dump(tostream &s = tcout);
  friend tostream &operator<<(tostream &s, const BitSet &rhs);
  void dump(FILE *f = stdout);
  Iterator<size_t> getIterator();        // iterator that iterates elements of bitset in ascending  order
  Iterator<size_t> getReverseIterator(); // iterator that iterates elements of bitset in descending order
  friend class BitSetIterator;
  friend class BitSetReverseIterator;

  void save(ByteOutputStream &s) const;
  void load(ByteInputStream  &s);

  friend Packer &operator<<(Packer &p, const BitSet &s);
  friend Packer &operator>>(Packer &p,       BitSet &s);

  static const char setBitsCount[256]; // Number of set bits for each bytevalue
};

class BitSetIterator : public AbstractIterator {
private:
  BitSet &m_s;
  size_t  m_next;
  size_t  m_current;
  bool    m_hasNext;
  void first();
public:
  AbstractIterator *clone();
  BitSetIterator(BitSet &s) : m_s(s) { first(); }
  bool hasNext() const { return m_hasNext; }
  void *next();
  void remove();
};

class BitSetReverseIterator : public AbstractIterator {
private:
  BitSet &m_s;
  size_t  m_next;
  size_t  m_current;
  bool    m_hasNext;
  void first();
public:
  AbstractIterator *clone();
  BitSetReverseIterator(BitSet &s) : m_s(s) { first(); }
  bool hasNext() const { return m_hasNext; }
  void *next();
  void remove();
};

class CharacterFormater {
public:
  virtual String formatChar(_TUCHAR ch) = NULL;
  static CharacterFormater *stdFormater;
  static CharacterFormater *extendedAsciiFormater;
  static CharacterFormater *octalEscapedAsciiFormater;
  static CharacterFormater *hexEscapedAsciiFormater;
  static CharacterFormater *hexEscapedExtendedAsciiFormater;
};


String charBitSetToString(const BitSet &set, CharacterFormater *charFormater = CharacterFormater::stdFormater);

class BitSetIndex {
private:
  const BitSet        &m_bitSet;
  unsigned char        m_shift;
  CompactArray<size_t> m_rangeTable;
public:
  BitSetIndex(const BitSet &src);
  void rebuildRangeTable();          // should be called whenever bitset is changed
  intptr_t getIndex(size_t i) const; // faster than Bitset.getIndex(i)
  String getInfoString() const;

  void save(ByteOutputStream &s) const; // also save bitset. to be used in BitSetFileIndex
  inline const CompactArray<size_t> &getRangeTable() const {
    return m_rangeTable;
  }
};

class BitSetFileIndex {                 // use BitSetIndex saved with BitSetIndex.save 
private:
  mutable ByteInputFile      m_f;
  const unsigned long long   m_startOffset;
  unsigned char              m_shift;
  CompactArray<size_t>       m_rangeTable;
  unsigned __int64           m_bitsStartOffset; // offset to the first byte in bit-array. not bitset.capacity
  BitSet                    *m_loadedIntervals; // contains intervals already loaded. start as empty, and works as a cache
  BitSet                    *m_bitSet;
public:
  BitSetFileIndex(const String &fileName, unsigned __int64 startOffset); // load rangetable
  ~BitSetFileIndex();
  intptr_t getIndex(size_t i) const;          // lazy-read bitset, saved with BitSetIndex.save
  inline const CompactArray<size_t> &getRangeTable() const {
    return m_rangeTable;
  }
};

class BitMatrix : private BitSet {
private:
  MatrixDimension m_dim;
  inline size_t getIndex(size_t r, size_t c) const {
    assert(m_dim.isLegalIndex(r, c));
    return r*m_dim.columnCount + c;
  }
  inline MatrixIndex indexToPoint(size_t index) const {
    return MatrixIndex(index / m_dim.columnCount, index % m_dim.columnCount);
  }
  void checkSameDimension(const BitMatrix &m) const;
  friend class BitMatrixIterator;
  
public:
  BitMatrix(size_t rowCount, size_t columnCount) 
    : m_dim(rowCount, columnCount)
    , BitSet(rowCount*columnCount)
  {
  }
  BitMatrix(const MatrixDimension &dim) 
    : m_dim(dim)
    , BitSet(dim.getElementCount())
  {
  }
  void set(size_t r, size_t c, bool v);
  inline void set(const MatrixIndex &i, bool v) {
    set(i.r,i.c,v);
  }
  inline bool get(size_t r, size_t c) const {
    return contains(getIndex(r,c));
  }
  inline bool get(const MatrixIndex &i) const {
    return contains(getIndex(i.r,i.c));
  }
  inline void clear() {
    BitSet::clear();
  }
  inline void invert() {
    BitSet::invert();
  }
  inline size_t getRowCount() const {
    return m_dim.rowCount;
  }
  inline size_t getColumnCount() const {
    return m_dim.columnCount;
  }
  inline const MatrixDimension &getDimension() const {
    return m_dim;
  }

  BitSet getRow(   size_t r) const;
  BitSet getColumn(size_t c) const;
  bool operator==(const BitMatrix &m) const;
  inline bool operator!=(const BitMatrix &m) const {
    return !(*this == m);
  }
  BitMatrix &operator&=(const BitMatrix &m);
  BitMatrix &operator|=(const BitMatrix &m);
  BitMatrix &operator^=(const BitMatrix &m);
  BitMatrix &operator-=(const BitMatrix &m);

  Iterator<MatrixIndex> getIterator();

  String toString() const;
};

