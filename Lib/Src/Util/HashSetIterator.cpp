#include "pch.h"
#include <HashSet.h>

DEFINECLASSNAME(HashSetIterator);

HashSetIterator::HashSetIterator(HashSetImpl &set) : m_set(&set) {
  m_updateCount = m_set->m_table->m_updateCount;
  m_next        = m_set->m_table->m_firstLink;
  m_current     = NULL;
}

AbstractIterator *HashSetIterator::clone() {
  return new HashSetIterator(*this);
}

bool HashSetIterator::hasNext() const {
  return m_next != NULL;
}

HashSetNode *HashSetIterator::nextNode() {
  if(m_next == NULL) {
    noNextElementError(s_className);
  }
  checkUpdateCount();

  m_current = m_next;
  m_next    = m_next->m_nextLink;
  return m_current; 
}

void HashSetIterator::remove() {
  if(m_current == NULL) {
    noCurrentElementError(s_className);
  }
  checkUpdateCount();
  m_set->remove(m_current->key());
  m_current = NULL;
  m_updateCount = m_set->m_table->m_updateCount;
}

void HashSetIterator::checkUpdateCount() const {
  if(m_updateCount != m_set->m_table->m_updateCount) {
    concurrentModificationError(s_className);
  }
}
