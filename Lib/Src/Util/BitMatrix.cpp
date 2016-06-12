#include "pch.h"
#include <BitSet.h>

void BitMatrix::set(size_t r, size_t c, bool v) {
  if(v) {
    add(getIndex(r,c));
  } else {
    remove(getIndex(r,c));
  }
}

BitSet BitMatrix::getRow(size_t r) const {
  const size_t count = getColumnCount();
  BitSet result(count);
  for(size_t c = 0, index = r*count; c < count; c++) {
    if(contains(index++)) result.add(c);
  }
  return result;
}

BitSet BitMatrix::getColumn(size_t c) const {
  const size_t count = getRowCount();
  BitSet result(count);
  for(size_t r = 0, index = c; r < count; r++, index += getColumnCount()) {
    if(contains(index)) result.add(r);
  }
  return result;
}

bool BitMatrix::operator==(const BitMatrix &m) const {
  if(getDimension() != m.getDimension()) {
    return false;
  }
  return ((BitSet&)*this) == m;
}

BitMatrix &BitMatrix::operator&=(const BitMatrix &m) {
  checkSameDimension(m);
  ((BitSet&)(*this)) &= m;
  return *this;
}

BitMatrix &BitMatrix::operator|=(const BitMatrix &m) {
  checkSameDimension(m);
  ((BitSet&)(*this)) |= m;
  return *this;
}

BitMatrix &BitMatrix::operator^=(const BitMatrix &m) {
  checkSameDimension(m);
  ((BitSet&)(*this)) ^= m;
  return *this;
}

BitMatrix &BitMatrix::operator-=(const BitMatrix &m) {
  checkSameDimension(m);
  ((BitSet&)(*this)) -= m;
  return *this;
}

class BitMatrixIterator : public BitSetIterator {
private:
  BitMatrix  &m_matrix;
  MatrixIndex m_p;
public:
  BitMatrixIterator(BitMatrix &m) : BitSetIterator(m), m_matrix(m) {
  }
  AbstractIterator *clone();
  void *next();
};

AbstractIterator *BitMatrixIterator::clone() {
  return new BitMatrixIterator(*this);
}

void *BitMatrixIterator::next() {
  m_p = m_matrix.indexToPoint(*(size_t*)(BitSetIterator::next()));
  return &m_p;
}

Iterator<MatrixIndex> BitMatrix::getIterator() {
  return Iterator<MatrixIndex>(new BitMatrixIterator(*this));
}

void BitMatrix::checkSameDimension(const BitMatrix &m) const {
  if(getDimension() != m.getDimension()) {
    throwException(_T("BitMatrices must have the same dimensions. dim(this)=%s, dim(m)=%s")
                  ,getDimension().toString().cstr(), m.getDimension().toString().cstr()
                  );
  }
}

String BitMatrix::toString() const {
  String result;
  for(size_t r = 0; r < getRowCount(); r++) {
    for(size_t c = 0; c < getColumnCount(); c++) {
      result += get(r,c)?_T("1"):_T("0");
    }
    result += _T("\n");
  }
  return result;
}
