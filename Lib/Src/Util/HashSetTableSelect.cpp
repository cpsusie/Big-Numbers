#include "pch.h"
#include "HashSetTable.h"
#include "HashSetNode.h"

AbstractKey *HashSetTable::select(RandomGenerator &rnd) const {
  if(size() == 0) throwEmptySetException(__TFUNCTION__);
  if(rnd.nextBool()) {
    for(auto p = m_firstLink;; p = p->m_nextLink) {
      if((p->m_nextLink == nullptr) || (rnd.nextInt(3) == 0)) {
        return p;
      }
    }
  } else {
    for(auto p = m_lastLink;; p = p->m_prevLink) {
      if((p->m_prevLink == nullptr) || (rnd.nextInt(3) == 0)) {
        return p;
      }
    }
  }
  return m_firstLink;
}

