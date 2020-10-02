#include "pch.h"
#include "HashSetIterator.h"

DEFINECLASSNAME(HashSetIterator);

HashSetIterator::HashSetIterator(HashSetImpl &set) : m_set(&set) {
  m_updateCount = m_set->m_table->m_updateCount;
  m_next        = m_set->m_table->m_firstLink;
  m_current     = nullptr;
}

HashSetNode *HashSetIterator::nextNode() {
  if(m_next == nullptr) {
    noNextElementError(s_className);
  }
  __assume(m_next);
  checkUpdateCount();

  m_current = m_next;
  m_next    = m_next->m_nextLink;
  return m_current;
}

void HashSetIterator::remove() {
  if(m_current == nullptr) {
    noCurrentElementError(s_className);
  }
  __assume(m_current);
  checkUpdateCount();
  m_set->remove(m_current->key());
  m_current = nullptr;
  m_updateCount = m_set->m_table->m_updateCount;
}

void HashSetIterator::checkUpdateCount() const {
  if(m_updateCount != m_set->m_table->m_updateCount) {
    concurrentModificationError(s_className);
  }
}
