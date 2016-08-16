#include "pch.h"
#include <Random.h>
#include <HashSet.h>

#ifdef __NEVER__
static FILE *traceFile = NULL;
#define TRACE_NEW(p)    fprintf(traceFile, "new %p %s(%d)\n", p, __FILE__, __LINE__)
#define TRACE_DELETE(p) fprintf(traceFile, "delete %p %s(%d)\n", p, __FILE__, __LINE__)

class FileInitializer {
public:
  FileInitializer();
 ~FileInitializer();
};

FileInitializer::FileInitializer() {
  traceFile = FOPEN("c:\\temp\\pointers.txt", "w");
}

FileInitializer::~FileInitializer() {
  if(traceFile != NULL) {
    fclose(traceFile);
    traceFile = NULL;
  }
}

static FileInitializer initFile;
#else

#define TRACE_NEW(p)
#define TRACE_DELETE(p)

#endif

HashSetTable::HashSetTable(const HashSetImpl &owner, size_t capacity) : m_owner(owner), m_capacity(max(5, capacity)) {
  m_size        = 0;
  m_updateCount = 0;
  m_table       = new HashSetNode*[m_capacity];
  TRACE_NEW(m_table);
  memset(m_table, 0, sizeof(m_table[0])*m_capacity);
  m_firstLink   = m_lastLink = NULL;
}

HashSetTable::~HashSetTable() {
  clear();
  TRACE_DELETE(m_table);
  delete[] m_table;
}

void HashSetTable::insert(size_t index, HashSetNode *n) {
  HashSetNode *q = m_table[index];
  m_table[index] = n;
  n->m_next      = q;
  if(q != NULL) {
    q->m_prev = &n->m_next;
  }
  n->m_prev = &m_table[index];

  if(m_firstLink == NULL) {
    m_firstLink            = n;
    n->m_prevLink          = NULL;
  } else {
    m_lastLink->m_nextLink = n;
    n->m_prevLink          = m_lastLink;
  }
  m_lastLink    = n;
  n->m_nextLink = NULL;
  m_size++;
  m_updateCount++;
}

void HashSetTable::remove(HashSetNode *n) {
  if(n->m_next) {
    n->m_next->m_prev = n->m_prev;
  }
  *n->m_prev = n->m_next; // p->prev always != NULL !

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

const AbstractKey *HashSetTable::select() const {
  if(m_size == 0) {
    throwException(_T("%s:Cannot select from empty set"), __TFUNCTION__);
  }

  if(randInt() % 2 == 0) {
    for(const HashSetNode *p = m_firstLink;; p = p->m_nextLink) {
      if(p->m_nextLink == NULL || randInt() % 3 == 0) {
        return p;
      }
    }
  } else {
    for(const HashSetNode *p = m_lastLink;; p = p->m_prevLink) {
      if(p->m_prevLink == NULL || randInt() % 3 == 0) {
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
  m_firstLink = m_lastLink = NULL;
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

HashSetImpl::HashSetImpl(const AbstractObjectManager &objectManager, HashFunction hash, const AbstractComparator &comparator, size_t capacity) {
  m_objectManager = objectManager.clone();
  m_comparator    = comparator.clone();
  m_hash          = hash;
  m_table         = new HashSetTable(*this, capacity);
  TRACE_NEW(m_table);
}

AbstractCollection *HashSetImpl::clone(bool cloneData) const {
  HashSetImpl *clone = new HashSetImpl(*m_objectManager, m_hash, *m_comparator, getCapacity());
  TRACE_NEW(clone);
  if(cloneData) {
    AbstractIterator *it = ((HashSetImpl*)this)->getIterator();
    while(it->hasNext()) {
      clone->add(it->next());
    }
    TRACE_DELETE(it);
    delete it;
  }
  return clone;
}

HashSetImpl::~HashSetImpl() {
  clear();
  TRACE_DELETE(m_table);
  delete m_table;

  delete m_comparator;
  delete m_objectManager;
}

HashSetNode::HashSetNode() {
  m_key      = NULL;
  m_next     = NULL;
  m_prev     = NULL;
  m_nextLink = NULL;
  m_prevLink = NULL;
}

HashSetNode *HashSetImpl::allocateNode() const {
  HashSetNode *result = new HashSetNode();
  TRACE_NEW(result);
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
  TRACE_DELETE(n);
  delete n;
}

void HashSetImpl::resize(size_t newCapacity) {
  HashSetTable *newTable = new HashSetTable(*this, newCapacity);
  TRACE_NEW(newTable);
  try {
    size_t capacity = newTable->getCapacity();
    for(HashSetNode *p = m_table->m_firstLink; p; p = p->m_nextLink) {
      unsigned long hashIndex = m_hash(p->m_key) % capacity;
      newTable->insert(hashIndex, cloneNode(p));
    }
    TRACE_DELETE(m_table);
    delete m_table;
    m_table = newTable;
  } catch(...) { // clean up the mess
    TRACE_DELETE(newTable);
    delete newTable;
    throw;
  }
}

bool HashSetImpl::insertNode(HashSetNode *n) {
  unsigned long hashIndex = m_hash(n->m_key) % getCapacity();
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
  const unsigned long hashIndex = m_hash(key) % getCapacity();
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
  return findNode(key) != NULL;
}

const void *HashSetImpl::select() const {
  return m_table->select()->key();
}

void *HashSetImpl::select() {
  const void *key = m_table->select()->key();
  return (void*)key;
}

const void *HashSetImpl::getMin() const {
  DEFINEMETHODNAME;
  throwUnsupportedOperationException(method);
  return NULL;
}

const void *HashSetImpl::getMax() const {
  DEFINEMETHODNAME;
  throwUnsupportedOperationException(method);
  return NULL;
}

AbstractIterator *HashSetImpl::getIterator() {
  AbstractIterator *result = new HashSetIterator(*this);
  TRACE_NEW(result);
  return result;
}

const HashSetNode *HashSetImpl::findNode(const void *key) const {
  const unsigned long hashIndex = m_hash(key) % getCapacity();
  for(const HashSetNode *p = m_table->m_table[hashIndex]; p; p = p->m_next) {
    if(m_comparator->cmp(key, p->m_key) == 0) {
      return p;
    }
  }
  return NULL;
}

HashSetNode *HashSetImpl::findNode(const void *key) {
  const unsigned long hashIndex = m_hash(key) % getCapacity();
  for(HashSetNode *p = m_table->m_table[hashIndex]; p; p = p->m_next) {
    if(m_comparator->cmp(key, p->m_key) == 0) {
      return p;
    }
  }
  return NULL;
}

#ifdef __CHECK_INTEGRITY

static int countcall = 0;
void HashSetImpl::checktable(const char *label) const {
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
    printf("%s:count simple:%d size:%d\n", label, count, size());
    pause();
  }
  count = 0;
  const HashSetNode *last = NULL;
  for(const HashSetNode *p  = m_table->m_firstLink; p; p = p->m_nextLink) {
    if(p->m_prevLink != last) {
      pause();
    }
    count++;
    last = p;
  }
  if(last != m_table->m_lastLink) {
    printf("%s %d:lastnode != lastlink\n", label, countcall);
    pause();
  }
  if(count != size()) {
    printf("%s %d:count link:%d size:%d\n", label, countcall, count, size());
    pause();
  }
}

#endif
