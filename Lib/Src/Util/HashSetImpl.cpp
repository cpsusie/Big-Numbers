#include "pch.h"
#include "HashSetIterator.h"

HashSetImpl::HashSetImpl(const AbstractObjectManager &objectManager, HashFunction hash, const AbstractComparator &comparator, size_t capacity) {
  m_objectManager = objectManager.clone(); TRACE_NEW(m_objectManager);
  m_comparator    = comparator.clone();    TRACE_NEW(m_comparator   );
  m_hash          = hash;
  m_table         = new HashSetTable(*this, capacity); TRACE_NEW(m_table);
}

AbstractCollection *HashSetImpl::clone(bool cloneData) const {
  HashSetImpl      *clone = nullptr;
  AbstractIterator *it    = nullptr;
  clone = new HashSetImpl(*m_objectManager, m_hash, *m_comparator, getCapacity());
  try {
    if(cloneData) {
      it = getIterator(); TRACE_NEW(it);
      while(it->hasNext()) {
        clone->add(it->next());
      }
      SAFEDELETE(it);
    }
  } catch(...) {
    TRACE_NEW( clone);
    SAFEDELETE(it   );
    SAFEDELETE(clone);
    throw;
  }
  return clone;
}

HashSetImpl::~HashSetImpl() {
  clear();
  SAFEDELETE(m_table        );
  SAFEDELETE(m_comparator   );
  SAFEDELETE(m_objectManager);
}

void HashSetImpl::clear() {
  m_table->clear();
}

// return number of elements
size_t HashSetImpl::size() const {
  return m_table->size();
}

CompactIntArray HashSetImpl::getLength() const {
  return m_table->getLength();
}

size_t HashSetImpl::getCapacity() const {
  return m_table->getCapacity();
}

int HashSetImpl::getMaxChainLength() const {
  return m_table->getMaxChainLength();
}

HashSetNode *HashSetImpl::allocateNode() const {
  HashSetNode *result = new HashSetNode(); TRACE_NEW(result);
  return result;
}

HashSetNode *HashSetImpl::createNode(const void *key) const {
  HashSetNode *n = allocateNode();
  n->m_key = m_objectManager->cloneObject(key);
  return n;
}

HashSetNode *HashSetImpl::cloneNode(HashSetNode *n) const {
  return createNode(n->m_key);
}

void HashSetImpl::deleteNode(HashSetNode *n) const {
  m_objectManager->deleteObject(n->m_key);
  SAFEDELETE(n);
}

void HashSetImpl::resize(size_t newCapacity) {
  HashSetTable *newTable = new HashSetTable(*this, newCapacity); TRACE_NEW(newTable);
  try {
    size_t capacity = newTable->getCapacity();
    for(HashSetNode *p = m_table->m_firstLink; p; p = p->m_nextLink) {
      ULONG hashIndex = m_hash(p->m_key) % capacity;
      newTable->insert(hashIndex, cloneNode(p));
    }
    SAFEDELETE(m_table);
    m_table = newTable;
  } catch(...) { // clean up the mess
    SAFEDELETE(newTable);
    throw;
  }
}

bool HashSetImpl::insertNode(HashSetNode *n) {
  ULONG hashIndex = m_hash(n->m_key) % getCapacity();
  for(HashSetNode *q = m_table->m_table[hashIndex]; q; q = q->m_next) {
    if(m_comparator->cmp(n->m_key, q->m_key) == 0) {
      return false; // duplicate key
    }
  }

  if(size() > 5 * getCapacity()) {
    size_t newCapacity = 5 * size();
    if(newCapacity % 2 == 0) {
      newCapacity++;
    }
    resize(newCapacity);
    hashIndex = m_hash(n->m_key) % getCapacity();
  }
  m_table->insert(hashIndex, n);
  return true;
}

bool HashSetImpl::add(const void *key) {
  HashSetNode *n = createNode(key);
  try {
    const bool ret = insertNode(n);
    if(!ret) {
      deleteNode(n); // duplicate key
    }
    return ret;
  } catch(...) {
    deleteNode(n);
    throw;
  }
}

bool HashSetImpl::remove(const void *key) {
  const ULONG hashIndex = m_hash(key) % getCapacity();
  for(HashSetNode *p = m_table->m_table[hashIndex]; p; p = p->m_next) {
    if(m_comparator->cmp(key, p->m_key) == 0) {
      m_table->remove(p);
      deleteNode(p);
      return true;
    }
  }
  return false; // not found;
}

bool HashSetImpl::contains(const void *key) const {
  return findNode(key) != nullptr;
}

void *HashSetImpl::select(RandomGenerator &rnd) const {
  return (void*)m_table->select(rnd)->key();
}

void *HashSetImpl::getMin() const {
  throwUnsupportedOperationException(__TFUNCTION__);
  return nullptr;
}

void *HashSetImpl::getMax() const {
  throwUnsupportedOperationException(__TFUNCTION__);
  return nullptr;
}

AbstractIterator *HashSetImpl::getIterator() const {
  return new HashSetIterator((HashSetImpl&)*this);
}

HashSetNode *HashSetImpl::findNode(const void *key) const {
  const ULONG hashIndex = m_hash(key) % getCapacity();
  for(HashSetNode *p = m_table->m_table[hashIndex]; p; p = p->m_next) {
    if(m_comparator->cmp(key, p->m_key) == 0) {
      return p;
    }
  }
  return nullptr;
}

#if defined(__HASHSET_CHECK_INTEGRITY)

static int countcall = 0;
void HashSetImpl::checktable(const TCHAR *label) const {
  countcall++;
  size_t n = getCapacity();
  size_t count = 0;
  for(size_t i = 0; i < n; i++) {
    const HashSetNode * const *last = &m_table->m_table[i];
    for(const HashSetNode *p = m_table->m_table[i]; p; p = p->m_next) {
      if(p->m_prev != last) {
        pause();
      }
      count++;
      last = &p->m_next;
    }
  }
  if(count != size()) {
    _tprintf(_T("%s:count simple:%d size:%d\n"), label, count, size());
    pause();
  }
  count = 0;
  const HashSetNode *last = nullptr;
  for(const HashSetNode *p  = m_table->m_firstLink; p; p = p->m_nextLink) {
    if(p->m_prevLink != last) {
      pause();
    }
    count++;
    last = p;
  }
  if(last != m_table->m_lastLink) {
    _tprintf(_T("%s %d:lastnode != lastlink\n"), label, countcall);
    pause();
  }
  if(count != size()) {
    _tprintf(_T("%s %d:count link:%d size:%d\n"), label, countcall, count, size());
    pause();
  }
}

#endif __HASHSET_CHECK_INTEGRITY
