#include "pch.h"
#include "HashSetTable.h"
#include "HashSetNode.h"

AbstractKey *HashSetTable::select(RandomGenerator &rnd) const {
  if(m_size == 0) {
    throwSelectFromEmptyCollectionException(__TFUNCTION__);
  }

  if(rnd.nextBool()) {
    for(HashSetNode *p = m_firstLink;; p = p->m_nextLink) {
      if((p->m_nextLink == nullptr) || (rnd.nextInt(3) == 0)) {
        return p;
      }
    }
  } else {
    for(HashSetNode *p = m_lastLink;; p = p->m_prevLink) {
      if((p->m_prevLink == nullptr) || (rnd.nextInt(3) == 0)) {
        return p;
      }
    }
  }
  return m_firstLink;
}

