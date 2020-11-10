#include "pch.h"
#include <HashSet.h>
#include "HashSetNode.h"
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

void HashSetTable::clear() {
  if(m_size == 0) {
    return;
  }
  HashSetNode *next;
  for(auto p = m_firstLink; p; p = next) {
    next = p->m_nextLink;
    m_owner.deleteNode(p);
  }
  memset(m_table, 0, sizeof(m_table[0])*m_capacity);
  m_firstLink = m_lastLink = nullptr;
  m_size = 0;
  m_updateCount++;
}

UINT HashSetTable::chainLength(size_t index) const {
  UINT count = 0;
  for(auto p = m_table[index]; p; p = p->m_next) {
    count++;
  }
  return count;
}

CompactUIntArray HashSetTable::getLength() const {
  const size_t capacity = getCapacity();
  CompactUIntArray tmp(capacity);
  UINT m = 0;
  for(size_t index = 0; index < capacity; index++) {
    const UINT l = chainLength(index);
    tmp.add(l);
    if(l > m) {
      m = l;
    }
  }
  CompactUIntArray result(m+1);
  result.insert(0, (UINT)0, m + 1);
  for(size_t index = 0; index < capacity; index++) {
    result[tmp[index]]++;
  }
  return result;
}

UINT HashSetTable::getMaxChainLength() const {
  UINT m = 0;
  const size_t capacity = getCapacity();
  for(size_t i = 0; i < capacity; i++) {
    const UINT l = chainLength(i);
    if(l > m) {
      m = l;
    }
  }
  return m;
}
