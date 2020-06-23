#include "pch.h"
#include <BitMatrix.h>

void BitMatrix::set(size_t r, size_t c, bool v) {
  if(!m_dim.isLegalIndex(r, c)) return;
  if(v) {
    add(getIndex(r, c));
  } else {
    remove(getIndex(r, c));
  }
}

BitMatrix &BitMatrix::setDimension(const MatrixDimension &dim) {
  if(dim == m_dim)  return *this;
  const size_t newBitCount = dim.getElementCount();
  if(newBitCount == 0) {
    throwInvalidArgumentException(__TFUNCTION__, _T("Dim=%s"), dim.toString().cstr());
  }
  if(isEmpty() || (dim.columnCount == m_dim.columnCount)) {
    setCapacity(newBitCount);
    m_dim = dim;
  } else {
    BitSet                old(*this);
    const MatrixDimension oldDim = m_dim;
    setCapacity(newBitCount);
    clear();
    m_dim = dim;
    const size_t copyR = min(oldDim.rowCount   , dim.rowCount   );
    const size_t copyC = min(oldDim.columnCount, dim.columnCount);
    for(size_t r = 0; r < copyR; r++) {
      const size_t firstIndexInRow = r*oldDim.columnCount;
      const size_t lastIndexInRow  = firstIndexInRow + copyC;
      for(Iterator<size_t> it = old.getIterator(firstIndexInRow); it.hasNext();) {
        const size_t index = it.next();
        if(index >= lastIndexInRow) break;
        set(r, index - firstIndexInRow, 1);
      }
    }
  }
  return *this;
}

void BitMatrix::checkValidRow(const TCHAR *method, size_t r) const {
  if(r >= getRowCount()) {
    throwInvalidArgumentException(method
                                 ,_T("r=%s. %s"), format1000(r).cstr()
                                 ,getDimensionString().cstr());
  }
}

void BitMatrix::checkValidColumn(const TCHAR *method, size_t c) const {
  if(c >= getColumnCount()) {
    throwInvalidArgumentException(method
                                 ,_T("c=%s. %s"), format1000(c).cstr()
                                 ,getDimensionString().cstr());
  }
}

BitSet &BitMatrix::getRow(size_t r, BitSet &s) const {
  checkValidRow(__TFUNCTION__, r);
  s.clear().setCapacity(getColumnCount());
  for(Iterator<MatrixIndex> it = ((BitMatrix*)this)->getRowIterator(r); it.hasNext();) {
    s.add(it.next().c);
  }
  return s;
}

BitSet &BitMatrix::getColumn(size_t c, BitSet &s) const {
  checkValidColumn(__TFUNCTION__, c);
  s.clear().setCapacity(getRowCount());
  for(Iterator<MatrixIndex> it = ((BitMatrix*)this)->getColumnIterator(c); it.hasNext();) {
    s.add(it.next().r);
  }
  return s;
}

void BitMatrix::setRow(size_t r, bool v) {
  checkValidRow(__TFUNCTION__, r);
  if(v) {
    add(getIndex(r,0), getIndex(r,getColumnCount()-1));
  } else {
    remove(getIndex(r,0), getIndex(r,getColumnCount()-1));
  }
}

void BitMatrix::setColumn(size_t c, bool v) {
  checkValidColumn(__TFUNCTION__, c);
  if(v) {
    for(size_t index = c; index < getCapacity(); index += getColumnCount()) {
      add(index);
    }
  } else {
    for(size_t index = c; index < getCapacity(); index += getColumnCount()) {
      remove(index);
    }
  }
}

bool BitMatrix::operator==(const BitMatrix &m) const {
  if(getDimension() != m.getDimension()) {
    return false;
  }
  return ((BitSet&)*this) == m;
}

BitMatrix &BitMatrix::operator&=(const BitMatrix &m) {
  checkSameDimension(__TFUNCTION__, m);
  ((BitSet&)(*this)) &= m;
  return *this;
}

BitMatrix &BitMatrix::operator|=(const BitMatrix &m) {
  checkSameDimension(__TFUNCTION__, m);
  ((BitSet&)(*this)) |= m;
  return *this;
}

BitMatrix &BitMatrix::operator^=(const BitMatrix &m) {
  checkSameDimension(__TFUNCTION__, m);
  ((BitSet&)(*this)) ^= m;
  return *this;
}

BitMatrix &BitMatrix::operator-=(const BitMatrix &m) {
  checkSameDimension(__TFUNCTION__, m);
  ((BitSet&)(*this)) -= m;
  return *this;
}

BitMatrix BitMatrix::operator*(const BitMatrix &rhs) const { // like normal matrix multiplication, using bool instead of floating points
  if(getColumnCount() != rhs.getRowCount()) {
    throwInvalidArgumentException(__TFUNCTION__
                                 ,_T("Invalid dimension. Left.%s. Right.%s")
                                 ,getDimensionString().cstr()
                                 ,rhs.getDimensionString().cstr());
  }

  const size_t rRows    = getRowCount();
  const size_t rColumns = rhs.getColumnCount();

  BitMatrix result(rRows, rColumns);

  CompactArray<BitSet*> columnArray(rColumns);
  for(size_t c = 0; c < rColumns; c++) {
    columnArray.add(new BitSet(rhs.getRowCount()));
    rhs.getColumn(c, *columnArray.last());
  }
  for(size_t r = 0; r < rRows; r++) {
    BitSet br(getColumnCount());
    getRow(r, br);
    for(size_t c = 0; c < rColumns; c++) {
      if(!(br & *columnArray[c]).isEmpty()) {
        result.set(r,c,true);
      }
    }
  }
  for(size_t c = 0; c < columnArray.size(); c++) {
    SAFEDELETE(columnArray[c]);
  }
  return result;
}

// ------------------------------------------------------------------

class BitMatrixIterator : public BitSetIterator {
private:
  DECLARECLASSNAME;
  BitMatrix  &m_matrix;
  MatrixIndex m_p;
  inline bool hasCurrent() const {
    return m_p.r < m_matrix.getRowCount();
  }
  inline void setCurrentUndefined() {
    m_p.r = -1;
  }

public:
  BitMatrixIterator(BitMatrix &m) : BitSetIterator(m), m_matrix(m) {
    setCurrentUndefined();
  }
  AbstractIterator *clone() override;
  void *next()              override;
  void remove()             override;
};

DEFINECLASSNAME(BitMatrixIterator);

AbstractIterator *BitMatrixIterator::clone() {
  return new BitMatrixIterator(*this);
}

void *BitMatrixIterator::next() {
  m_p = m_matrix.indexToPoint(*(size_t*)(BitSetIterator::next()));
  return &m_p;
}

void BitMatrixIterator::remove() {
  if(!hasCurrent()) noCurrentElementError(s_className);
  m_matrix.set(m_p, false);
  setCurrentUndefined();
}

Iterator<MatrixIndex> BitMatrix::getIterator() {
  return Iterator<MatrixIndex>(new BitMatrixIterator(*this));
}

// ------------------------------------------------------------------

class BitMatrixRowIterator : public BitSetIterator {
private:
  DECLARECLASSNAME;
  BitMatrix  &m_matrix;
  MatrixIndex m_p;
  static inline size_t firstIndex(const BitMatrix &m, size_t r) {
    return m.getIndex(r,0);
  }
  static inline size_t lastIndex(const BitMatrix &m, size_t r) {
    return m.getIndex(r,m.getColumnCount()-1);
  }
  inline bool hasCurrent() const {
    return m_p.c < m_matrix.getColumnCount();
  }
  inline void setCurrentUndefined() {
    m_p.c = -1;
  }
public:
  BitMatrixRowIterator(BitMatrix &m, size_t r)
    : BitSetIterator(m, firstIndex(m,r), lastIndex(m,r))
    , m_matrix(m) {
    setCurrentUndefined();
  }
  AbstractIterator *clone() override;
  void *next()              override;
  void remove()             override;
};

DEFINECLASSNAME(BitMatrixRowIterator);

AbstractIterator *BitMatrixRowIterator::clone() {
  return new BitMatrixRowIterator(*this);
}

void *BitMatrixRowIterator::next() {
  m_p = m_matrix.indexToPoint(*(size_t*)(BitSetIterator::next()));
  return &m_p;
}

void BitMatrixRowIterator::remove() {
  if(!hasCurrent()) noCurrentElementError(s_className);
  m_matrix.set(m_p, false);
  setCurrentUndefined();
}

Iterator<MatrixIndex> BitMatrix::getRowIterator(size_t r) {
  checkValidRow(__TFUNCTION__, r);
  return Iterator<MatrixIndex>(new BitMatrixRowIterator(*this, r));
}

// ------------------------------------------------------------------

class BitMatrixColumnIterator : public AbstractIterator {
private:
  DECLARECLASSNAME;
  BitMatrix  &m_matrix;
  MatrixIndex m_p, m_next;
  bool        m_hasNext;
  void first(size_t c);
  inline bool hasCurrent() const {
    return m_p.r < m_matrix.getRowCount();
  }
  inline void setCurrentUndefined() {
    m_p.r = -1;
  }
public:
  BitMatrixColumnIterator(BitMatrix &m, size_t c) : m_matrix(m) {
    first(c);
  }
  AbstractIterator *clone()       override;
  bool hasNext()            const override {
    return m_hasNext;
  }
  void *next()                    override;
  void  remove()                  override;
};

DEFINECLASSNAME(BitMatrixColumnIterator);

void BitMatrixColumnIterator::first(size_t c) {
  m_next.c = m_p.c = c;
  setCurrentUndefined();
  for(m_next.r = 0; m_next.r < m_matrix.getRowCount(); m_next.r++) {
    if(m_matrix.get(m_next)) {
      m_hasNext = true;
      return;
    }
  }
  m_hasNext = false;
}

AbstractIterator *BitMatrixColumnIterator::clone() {
  return new BitMatrixColumnIterator(*this);
}

void *BitMatrixColumnIterator::next() {
  if(!hasNext()) noNextElementError(s_className);
  for(m_p.r = m_next.r++; m_next.r < m_matrix.getRowCount(); m_next.r++) {
    if(m_matrix.get(m_next)) return &m_p;
  }
  m_hasNext = false;
  return &m_p;
}

void BitMatrixColumnIterator::remove() {
  if(!hasCurrent()) noCurrentElementError(s_className);
  m_matrix.set(m_p, false);
  setCurrentUndefined();
}

Iterator<MatrixIndex> BitMatrix::getColumnIterator(size_t c) {
  checkValidColumn(__TFUNCTION__, c);
  return Iterator<MatrixIndex>(new BitMatrixColumnIterator(*this, c));
}

// -------------------------------------------------------------------------

void BitMatrix::checkSameDimension(const TCHAR *method, const BitMatrix &m) const {
  if(getDimension() != m.getDimension()) {
    throwInvalidArgumentException(method
                                 ,_T("BitMatrices must have the same dimensions. this->%s, m.%s")
                                 ,getDimensionString().cstr(), m.getDimensionString().cstr()
                                 );
  }
}

String BitMatrix::toString() const {
  String result;
  for(size_t r = 0; r < getRowCount(); r++) {
    for(size_t c = 0; c < getColumnCount(); c++) {
      result += get(r, c) ? _T("1") : _T("0");
    }
    result += _T("\n");
  }
  return result;
}
