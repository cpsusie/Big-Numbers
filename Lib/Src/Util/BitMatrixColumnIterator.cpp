#include "pch.h"
#include "BitMatrixIterator.h"

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
  if(!hasNext()) noNextElementError(__TFUNCTION__);
  for(m_p.r = m_next.r++; m_next.r < m_matrix.getRowCount(); m_next.r++) {
    if(m_matrix.get(m_next)) return &m_p;
  }
  m_hasNext = false;
  return &m_p;
}

void BitMatrixColumnIterator::remove() {
  if(!hasCurrent()) noCurrentElementError(__TFUNCTION__);
  m_matrix.set(m_p, false);
  setCurrentUndefined();
}

ConstIterator<MatrixIndex> BitMatrix::getColumnIterator(size_t c) const {
  checkValidColumn(__TFUNCTION__, c);
  return ConstIterator<MatrixIndex>(new BitMatrixColumnIterator(this, c));
}

Iterator<MatrixIndex> BitMatrix::getColumnIterator(size_t c) {
  checkValidColumn(__TFUNCTION__, c);
  return Iterator<MatrixIndex>(new BitMatrixColumnIterator(this, c));
}
