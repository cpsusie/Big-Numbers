#include "pch.h"
#include "BitMatrixIterator.h"

DEFINECLASSNAME(BitMatrixIterator);

AbstractIterator *BitMatrixIterator::clone() {
  return new BitMatrixIterator(*this);
}

void *BitMatrixIterator::next() {
  m_p = m_matrix.indexToPoint(*(size_t*)(__super::next()));
  return &m_p;
}

void BitMatrixIterator::remove() {
  if(!hasCurrent()) noCurrentElementError(s_className);
  m_matrix.set(m_p, false);
  setCurrentUndefined();
}

Iterator<MatrixIndex> BitMatrix::getIterator() const {
  return Iterator<MatrixIndex>(new BitMatrixIterator((BitMatrix&)(*this)));
}
