#include "pch.h"
#include "BitMatrixIterator.h"

DEFINECLASSNAME(BitMatrixRowIterator);

AbstractIterator *BitMatrixRowIterator::clone() {
  return new BitMatrixRowIterator(*this);
}

void *BitMatrixRowIterator::next() {
  m_p = m_matrix.indexToPoint(*(size_t*)(__super::next()));
  return &m_p;
}

void BitMatrixRowIterator::remove() {
  if(!hasCurrent()) noCurrentElementError(s_className);
  m_matrix.set(m_p, false);
  setCurrentUndefined();
}

ConstIterator<MatrixIndex> BitMatrix::getRowIterator(size_t r) const {
  checkValidRow(__TFUNCTION__, r);
  return ConstIterator<MatrixIndex>(new BitMatrixRowIterator(this, r));
}

Iterator<MatrixIndex> BitMatrix::getRowIterator(size_t r) {
  checkValidRow(__TFUNCTION__, r);
  return Iterator<MatrixIndex>(new BitMatrixRowIterator(this, r));
}
