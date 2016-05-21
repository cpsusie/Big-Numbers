#pragma once

#include "CompactArray.h"
#include "Iterator.h"
#include "Packer.h"

#define _BITSET_ATOMSIZE 32

class BitSet {
private:
  DECLARECLASSNAME;
  unsigned int selectRandomNonEmptyAtom() const;
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
  void getRangeTable(CompactArray<unsigned int> &rangeTable, unsigned char shift) const;
private:
  static Atom mask[_BITSET_ATOMSIZE+1];
  friend class InitBitSetMask;
protected:
  Atom         *m_p;
  unsigned long m_capacity;
  static int getAtomCount(unsigned long capacity);
public:
  explicit BitSet(unsigned long capacity); // can store integers from [0..capacity-1]
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

  BitSet &operator+=(unsigned long i) {                                 // Add i to the set, if it's not already there
    return add(i);
  }

  BitSet &operator-=(unsigned long i) {                                 // Remove i from the set if it's there
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
  unsigned long  oldsize() const;                                       // Number of elements, slow version
  unsigned long  size() const;                                          // Number of elements. fast version
  bool           isEmpty() const;                                       // Return true if set is empty
  BitSet &remove(unsigned long i);                                      // Remove i from set
  BitSet &add(   unsigned long i);                                      // Insert i into set
  BitSet &remove(unsigned long a, unsigned long b);                     // Remove interval [a;b] from set
  BitSet &add(   unsigned long a, unsigned long b);                     // Insert interval [a;b] into set
  bool    contains(unsigned long i) const;                              // Return if set contains i
  int     getIndex(unsigned long i) const;                              // Return number of elements < i. if i not in set -1 is returned
  unsigned int getCount(unsigned int from, unsigned int to) const;      // Return number of elements between from and to. both included.
  unsigned int select() const;                                          // Returns a random element from non empty set. throws Exception if set is empty
  BitSet &clear();                                                      // Remove all elements from set
  BitSet &invert();                                                     // Complementset
  unsigned long getCapacity() const {                                   // Return number of bits in set = maximum number of elements in set
    return m_capacity;
  }
  void setCapacity(unsigned long newCapacity);
  int getAtomCount() const;
  const Atom *getFirstAtom() const {
    return m_p;
  }
  unsigned long getAtomIndex(unsigned long i) const;
  String toString() const;
  String toStringIntervals() const;

  unsigned long hashCode() const;
  tostream &dump(tostream &s = tcout);
  friend tostream &operator<<(tostream &s, const BitSet &rhs);
  void dump(FILE *f = stdout);
  Iterator<unsigned int> getIterator();        // iterator that iterates elements of bitset in ascending  order
  Iterator<unsigned int> getReverseIterator(); // iterator that iterates elements of bitset in descending order
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
  BitSet      &m_s;
  unsigned int m_next;
  unsigned int m_current;
  bool         m_hasNext;
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
  BitSet      &m_s;
  unsigned int m_next;
  unsigned int m_current;
  bool         m_hasNext;
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
  const BitSet              &m_bitSet;
  unsigned char              m_shift;
  CompactArray<unsigned int> m_rangeTable;
public:
  BitSetIndex(const BitSet &src);
  void rebuildRangeTable();            // should be called whenever bitset is changed
  int getIndex(unsigned long i) const; // faster than Bitset.getIndex(i)
  String getInfoString() const;

  void save(ByteOutputStream &s) const; // also save bitset. to be used in BitSetFileIndex
  const CompactArray<unsigned int> &getRangeTable() const {
    return m_rangeTable;
  }
};

class BitSetFileIndex {                 // use BitSetIndex saved with BitSetIndex.save 
private:
  mutable ByteInputFile      m_f;
  const unsigned long long   m_startOffset;
  unsigned char              m_shift;
  CompactArray<unsigned int> m_rangeTable;
  unsigned __int64           m_bitsStartOffset; // offset to the first byte in bit-array. not bitset.capacity
  BitSet                    *m_loadedIntervals; // contains intervals already loaded. start as empty, and works as a cache
  BitSet                    *m_bitSet;
public:
  BitSetFileIndex(const String &fileName, unsigned __int64 startOffset); // load rangetable
  ~BitSetFileIndex();
  int getIndex(unsigned long i) const;          // lazy-read bitset, saved with BitSetIndex.save
  const CompactArray<unsigned int> &getRangeTable() const {
    return m_rangeTable;
  }
};

class BitMatrix : private BitSet {
private:
  unsigned int m_rowCount, m_columnCount;
  inline unsigned int getIndex(unsigned int r, unsigned int c) const {
    assert((r < m_rowCount) && (c < m_columnCount));
    return r*m_columnCount + c;
  }
  inline CPoint indexToPoint(unsigned int index) const {
    return CPoint(index % m_columnCount, index / m_columnCount);
  }
  void checkSameDimension(const BitMatrix &m) const;
  friend class BitMatrixIterator;
  
public:
  BitMatrix(unsigned int rowCount, int columnCount) 
    : m_rowCount(rowCount), m_columnCount(columnCount)
    , BitSet(rowCount*columnCount)
  {
  }
  BitMatrix(const CSize &size) 
    : m_rowCount(size.cy), m_columnCount(size.cx)
    , BitSet(size.cx*size.cy)
  {
  }
  void set(unsigned int r, unsigned int c, bool v);
  inline void set(const CPoint &p, bool v) {
    set(p.y,p.x,v);
  }
  inline bool get(unsigned int r, unsigned int c) const {
    return contains(getIndex(r,c));
  }
  inline bool get(const CPoint &p) const {
    return contains(getIndex(p.y,p.x));
  }
  inline void clear() {
    BitSet::clear();
  }
  inline void invert() {
    BitSet::invert();
  }
  inline unsigned int getRowCount() const {
    return m_rowCount;
  }
  inline unsigned int getColumnCount() const {
    return m_columnCount;
  }
  inline CSize getDimension() const {
    return CSize(m_columnCount, m_rowCount);
  }

  BitSet getRow(   unsigned int r) const;
  BitSet getColumn(unsigned int c) const;
  bool operator==(const BitMatrix &m) const;
  inline bool operator!=(const BitMatrix &m) const {
    return !(*this == m);
  }
  BitMatrix &operator&=(const BitMatrix &m);
  BitMatrix &operator|=(const BitMatrix &m);
  BitMatrix &operator^=(const BitMatrix &m);
  BitMatrix &operator-=(const BitMatrix &m);

  Iterator<CPoint> getIterator();

  String toString() const;
};

