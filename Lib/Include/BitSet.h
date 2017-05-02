#pragma once

#include "CompactArray.h"
#include "Iterator.h"
#include "MatrixDimension.h"

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
  if(i >= m_capacity) throwIndexOutOfRange(__TFUNCTION__, i, _T(""))
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
  void throwIndexOutOfRange(const TCHAR *method, size_t index, const TCHAR *format,...) const;

public:
  // can store integers from [0..capacity-1]
  explicit BitSet(size_t capacity);
  BitSet(const BitSet &set);
  ~BitSet() {
    delete[] m_p;
  }
  BitSet &operator=(const BitSet &rhs);
  // Union        = set containing all elements from lts and rhs
  friend BitSet  operator+ (const BitSet &lts, const BitSet &rhs);
  // Difference   = set containing elements from lts without the elements from rhs
  friend inline BitSet operator- (const BitSet &lts, const BitSet &rhs) {
    BitSet result(lts);
    result -= rhs;
    return result;
  }
  // Intersection = set containing elements contained in both lts and rhs
  friend BitSet  operator* (const BitSet &lts, const BitSet &rhs);
  // a^b = (a-b) | (b-a) (symmetric difference) = set containg elements that are in only lts or rhs, but not both
  friend BitSet  operator^ (const BitSet &lts, const BitSet &rhs);
  // Same as +
  friend BitSet  operator| (const BitSet &lts, const BitSet &rhs) {
    return lts + rhs;
  }
  // Same as *
  friend BitSet  operator& (const BitSet &lts, const BitSet &rhs) {
    return lts * rhs;
  }
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
  friend inline bool operator< (const BitSet &lts, const BitSet &rhs) {
    return (lts <= rhs) && (lts != rhs);
  }
  // Same as rhs <= lts
  friend inline bool operator>=(const BitSet &lts, const BitSet &rhs) {
    return rhs <= lts;
  }
  // Same as rhs <  lts
  friend inline bool operator> (const BitSet &lts, const BitSet &rhs) {
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
  // Complementset
  friend BitSet  compl(const BitSet &s);
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
  size_t   select() const;
  // Remove all elements from set
  inline BitSet &clear() {
    memset(m_p, 0,getAtomCount() * sizeof(Atom));
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

  ULONG hashCode() const;
  friend tostream &operator<<(tostream &s, const BitSet &rhs);
  tostream &dump(tostream &s = tcout) const;
  void dump(FILE *f = stdout) const;
  String toString(AbstractStringifier<size_t> *sf = NULL, const TCHAR *delim = _T(",")) const;
  String toStringIntervals() const;

#ifdef __NEVER__
  size_t   oldSize() const;                            // Number of elements, slow version
  intptr_t oldGetIndex(size_t i) const;                // Return number of elements < i. if i not in set -1 is returned
  size_t   oldGetCount(size_t from, size_t to) const;  // Return number of elements between from and to. both included.
#endif
};

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

class FileBitSet {
private:
  mutable ByteInputFile  m_f;
  UINT64                 m_firstAtomOffset;     // offset to the first Atom in bit-array. not bitset.capacity
  size_t                 m_capacity;
  FileBitSet(const FileBitSet &src);            // not implemented. Not cloneable
  FileBitSet &operator=(const FileBitSet &src); // not implemented. Not cloneable
public:
  FileBitSet(const String &fileName, UINT64 startOffset);
  bool contains(size_t i) const;
  inline size_t getCapacity() const {                                   // Return number of bits in set = maximum number of elements in set
    return m_capacity;
  }
};

class BitSetIndex {
private:
  const BitSet     &m_bitSet;
  BYTE              m_shift;
  CompactInt64Array m_rangeTable;
public:
  BitSetIndex(const BitSet &src);
  void rebuildRangeTable();          // should be called whenever bitset is changed
  intptr_t getIndex(size_t i) const; // faster than Bitset.getIndex(i)
  String getInfoString() const;

  void save(ByteOutputStream &s) const; // also save bitset. to be used in FleBitSetIndex
  inline const CompactInt64Array &getRangeTable() const {
    return m_rangeTable;
  }
};

class FileBitSetIndex {                 // use BitSetIndex saved with BitSetIndex.save 
private:
  mutable ByteInputFile  m_f;
  const UINT64           m_startOffset;
  BYTE                   m_shift;
  CompactInt64Array      m_rangeTable;
  UINT64                 m_bitsStartOffset; // offset to the first byte in bit-array. not bitset.capacity
  BitSet                *m_loadedIntervals; // contains intervals already loaded. start as empty, and works as a cache
  BitSet                *m_bitSet;
public:
  FileBitSetIndex(const String &fileName, UINT64 startOffset); // load rangetable
  ~FileBitSetIndex();
  intptr_t getIndex(size_t i) const;          // lazy-read bitset, saved with BitSetIndex.save
  inline const CompactInt64Array &getRangeTable() const {
    return m_rangeTable;
  }
};

class BitMatrix : private BitSet {
private:
  MatrixDimension m_dim;
  inline size_t getIndex(size_t r, size_t c) const {
    return r*m_dim.columnCount + c;
  }
  inline MatrixIndex indexToPoint(size_t index) const {
    return MatrixIndex(index / m_dim.columnCount, index % m_dim.columnCount);
  }
  void checkSameDimension(const TCHAR *method, const BitMatrix &m) const;
  void checkValidRow(   const TCHAR *method, size_t r) const;
  void checkValidColumn(const TCHAR *method, size_t c) const;
  friend class BitMatrixIterator;
  friend class BitMatrixRowIterator;
  friend class BitMatrixColumnIterator;

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
  BitMatrix &operator=(const BitMatrix &m) {
    BitSet::operator=(m);
    m_dim = m.m_dim;
    return *this;
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
  BitMatrix &setDimension(const MatrixDimension &dim);
  inline size_t size() const {    // Number of true elements.
    return BitSet::size();
  }
  inline bool isEmpty() const {
    return BitSet::isEmpty();
  }
  BitSet &getRow(   size_t r, BitSet &s) const;
  BitSet &getColumn(size_t c, BitSet &s) const;
  inline BitSet getRow(size_t r) const {
    BitSet result(getColumnCount());
    return getRow(r, result);
  }
  inline BitSet getColumn(size_t c) const {
    BitSet result(getRowCount());
    return getColumn(c, result);
  }
  void    setRow(   size_t r, bool v);
  void    setColumn(size_t c, bool v);
  bool operator==(const BitMatrix &m) const;
  inline bool operator!=(const BitMatrix &m) const {
    return !(*this == m);
  }
  BitMatrix &operator&=(const BitMatrix &m);
  BitMatrix &operator|=(const BitMatrix &m);
  BitMatrix &operator^=(const BitMatrix &m);
  BitMatrix &operator-=(const BitMatrix &m);
  BitMatrix operator*  (const BitMatrix &rhs) const; // like normal matrix multiplication, using bool instead of floating points
  Iterator<MatrixIndex> getIterator();
  Iterator<MatrixIndex> getRowIterator(   size_t r);
  Iterator<MatrixIndex> getColumnIterator(size_t c);
  String toString() const;
  String getDimensionString() const {
    return format(_T("Dimension=%s"), m_dim.toString().cstr());
  }
};

