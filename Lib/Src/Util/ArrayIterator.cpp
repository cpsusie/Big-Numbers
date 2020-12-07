#include "pch.h"
#include <Array.h>

class ArrayIterator : public AbstractIterator {
private:
  ArrayImpl          &m_a;
  size_t              m_next;
  intptr_t            m_current;
  size_t              m_updateCount;
  inline void checkUpdateCount(const TCHAR *method) const {
    if(m_updateCount != m_a.m_updateCount) {
      concurrentModificationError(method);
    }
  }

public:
  ArrayIterator(const ArrayImpl *a);
  AbstractIterator *clone()         override;
  bool              hasNext() const override;
  void             *next()          override;
  void              remove()        override;
};

ArrayIterator::ArrayIterator(const ArrayImpl *a) : m_a(*(ArrayImpl*)a) {
  m_updateCount = m_a.m_updateCount;
  m_next        = 0;
  m_current     = -1;
}

AbstractIterator *ArrayIterator::clone() {
  return new ArrayIterator(*this);
}

bool ArrayIterator::hasNext() const {
  return m_next < m_a.size();
}

void *ArrayIterator::next() {
  if(m_next >= m_a.size()) {
    noNextElementError(__TFUNCTION__);
  }
  checkUpdateCount(__TFUNCTION__);
  m_current = m_next++;
  return m_a.getElement(m_current);
}

void ArrayIterator::remove() {
  if(m_current < 0) {
    noCurrentElementError(__TFUNCTION__);
  }
  checkUpdateCount(__TFUNCTION__);
  m_a.removeIndex(m_current,1);
  m_current     = -1;
  m_updateCount = m_a.m_updateCount;
}

AbstractIterator *ArrayImpl::getIterator() const {
  return new ArrayIterator(this);
}
