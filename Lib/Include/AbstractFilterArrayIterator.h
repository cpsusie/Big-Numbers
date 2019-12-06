#pragma once

#include "BitSet.h"

template<typename ArrayType> class AbstractFilterArrayIterator : public AbstractIterator {
private:
  ArrayType       &m_a;
  BitSet           m_filterSet;
  Iterator<size_t> m_it;
public:
  AbstractFilterArrayIterator(ArrayType &a, const BitSet &filterSet)
    : m_a(a)
    , m_filterSet(filterSet)
  {
    m_it = m_filterSet.getIterator();
  }
  AbstractIterator *clone() {
    return new AbstractFilterArrayIterator(m_a, m_filterSet);
  }

  inline bool hasNext() const {
    return m_it.hasNext();
  }

  void *next() {
    if(!m_it.hasNext()) {
      noNextElementError(__TFUNCTION__);
    }
    return &m_a[m_it.next()];
  }

  void remove() {
    throwUnsupportedOperationException(__TFUNCTION__);
  }
};
