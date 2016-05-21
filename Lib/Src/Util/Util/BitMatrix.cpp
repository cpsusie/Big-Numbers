#include "pch.h"
#include <BitSet.h>

void BitMatrix::set(unsigned int r, unsigned int c, bool v) {
  if(v) {
    add(getIndex(r,c));
  } else {
    remove(getIndex(r,c));
  }
}

BitSet BitMatrix::getRow(unsigned int r) const {
  BitSet result(m_columnCount);
  for(unsigned int c = 0, index = r*m_columnCount; c < m_columnCount; c++) {
    if(contains(index++)) result.add(c);
  }
  return result;
}

BitSet BitMatrix::getColumn(unsigned int c) const {
  BitSet result(m_rowCount);
  for(unsigned int r = 0, index = c; r < m_rowCount; r++, index += m_columnCount) {
    if(contains(index)) result.add(r);
  }
  return result;
}

bool BitMatrix::operator==(const BitMatrix &m) const {
  if((m_rowCount != m.m_rowCount) || (m_columnCount != m.m_columnCount)) {
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
  BitMatrix &m_matrix;
  CPoint     m_p;
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
  m_p = m_matrix.indexToPoint(*(unsigned int*)(BitSetIterator::next()));
  return &m_p;
}

Iterator<CPoint> BitMatrix::getIterator() {
  return Iterator<CPoint>(new BitMatrixIterator(*this));
}

void BitMatrix::checkSameDimension(const BitMatrix &m) const {
  if((m_rowCount != m.m_rowCount) || (m_columnCount != m.m_columnCount)) {
    throwException(_T("BitMatrices must have the same dimensions. dim(this)=(%d,%d), dim(m)=(%d,%d)")
                  ,m_rowCount, m_columnCount, m.m_rowCount, m.m_columnCount
                  );
  }
}

String BitMatrix::toString() const {
  String result;
  for(unsigned int r = 0; r < m_rowCount; r++) {
    for(unsigned int c = 0; c < m_columnCount; c++) {
      result += get(r,c)?_T("1"):_T("0");
    }
    result += _T("\n");
  }
  return result;
}
