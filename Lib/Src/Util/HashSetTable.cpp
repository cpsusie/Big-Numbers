#include "pch.h"
#include "HashSetTable.h"

HashSetTable::HashSetTable(const HashSetImpl &owner, size_t capacity) : m_owner(owner), m_capacity(max(5, capacity)) {
  m_size        = 0;
  m_updateCount = 0;
  m_table       = new HashSetNode*[m_capacity]; TRACE_NEW(m_table);
  memset(m_table, 0, sizeof(m_table[0])*m_capacity);
  m_firstLink   = m_lastLink = nullptr;
}

HashSetTable::~HashSetTable() {
  clear();
  SAFEDELETEARRAY(m_table);
}

void HashSetTable::insert(size_t index, HashSetNode *n) {
  HashSetNode *q = m_table[index];
  m_table[index] = n;
  n->m_next      = q;
  if(q != nullptr) {
    q->m_prev = &n->m_next;
  }
  n->m_prev = &m_table[index];

  if(m_firstLink == nullptr) {
    m_firstLink            = n;
    n->m_prevLink          = nullptr;
  } else {
    m_lastLink->m_nextLink = n;
    n->m_prevLink          = m_lastLink;
  }
  m_lastLink    = n;
  n->m_nextLink = nullptr;
  m_size++;
  m_updateCount++;
}

void HashSetTable::remove(HashSetNode *n) {
  if(n->m_next) {
    n->m_next->m_prev = n->m_prev;
  }
  *n->m_prev = n->m_next; // p->prev always != nullptr !

  if(n->m_nextLink) {
    n->m_nextLink->m_prevLink = n->m_prevLink;
  } else {
    m_lastLink = n->m_prevLink;
  }
  if(n->m_prevLink) {
    n->m_prevLink->m_nextLink = n->m_nextLink;
  } else {
    m_firstLink = n->m_nextLink;
  }

  m_size--;
  m_updateCount++;
}

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

void HashSetTable::clear() {
  if(m_size == 0) {
    return;
  }
  HashSetNode *next;
  for(HashSetNode *p = m_firstLink; p; p = next) {
    next = p->m_nextLink;
    m_owner.deleteNode(p);
  }
  memset(m_table, 0, sizeof(m_table[0])*m_capacity);
  m_firstLink = m_lastLink = nullptr;
  m_size = 0;
  m_updateCount++;
}

int HashSetTable::chainLength(size_t index) const {
  int count = 0;
  for(HashSetNode *p = m_table[index]; p; p = p->m_next) {
    count++;
  }
  return count;
}

CompactIntArray HashSetTable::getLength() const {
  CompactIntArray result;
  CompactIntArray tmp;
  const size_t capacity = getCapacity();
  int m = 0;
  for(size_t index = 0; index < capacity; index++) {
    const int l = chainLength(index);
    tmp.add(l);
    if(l > m) {
      m = l;
    }
  }
  for(int i = 0; i <= m; i++) {
    result.add(0);
  }
  for(size_t index = 0; index < capacity; index++) {
    result[tmp[index]]++;
  }
  return result;
}

int HashSetTable::getMaxChainLength() const {
  int m = 0;
  const size_t capacity = getCapacity();
  for(size_t i = 0; i < capacity; i++) {
    const int l = chainLength(i);
    if(l > m) {
      m = l;
    }
  }
  return m;
}
