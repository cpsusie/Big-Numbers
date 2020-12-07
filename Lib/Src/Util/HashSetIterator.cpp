#include "pch.h"
#include <HashSet.h>
#include "HashSetIterator.h"

HashSetIterator::HashSetIterator(const HashSetImpl *set) : m_set(*(HashSetImpl*)set) {
  m_updateCount = m_set.m_table->m_updateCount;
  m_next        = m_set.m_table->m_firstLink;
  m_current     = nullptr;
}

HashSetNode *HashSetIterator::nextNode() {
  if(m_next == nullptr) {
    noNextElementError(__TFUNCTION__);
  }
  __assume(m_next);
  checkUpdateCount(__TFUNCTION__);

  m_current = m_next;
  m_next    = m_next->m_nextLink;
  return m_current;
}

void HashSetIterator::remove() {
  if(m_current == nullptr) {
    noCurrentElementError(__TFUNCTION__);
  }
  __assume(m_current);
  checkUpdateCount(__TFUNCTION__);
  m_set.remove(m_current->key());
  m_current = nullptr;
  m_updateCount = m_set.m_table->m_updateCount;
}

void HashSetIterator::checkUpdateCount(const TCHAR *method) const {
  if(m_updateCount != m_set.m_table->m_updateCount) {
    concurrentModificationError(method);
  }
}

AbstractIterator *HashSetImpl::getIterator() const {
  return new HashSetIterator(this);
}
