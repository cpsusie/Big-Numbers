#include "pch.h"
#include <Random.h>
#include <TreeSet.h>
#include "TreeSetNode.h"

void *TreeSetImpl::getRandom(RandomGenerator &rnd) const {
  if(size() == 0) {
    throwEmptySetException(__TFUNCTION__);
  }

  const TreeSetNode *p = m_root;

  for(;;) {
    bool cont = (p->m_left != nullptr || p->m_right != nullptr) && rnd.nextBool();
    if(!cont) {
      return p->m_key;
    } else if(p->m_left == nullptr) {
      p = p->m_right;
    } else if(p->m_right == nullptr) {
      p = p->m_left;
    } else {
      p = rnd.nextBool() ? p->m_left : p->m_right;
    }
  }
}

void *TreeSetImpl::select(RandomGenerator &rnd) const {
  return getRandom(rnd);
}
