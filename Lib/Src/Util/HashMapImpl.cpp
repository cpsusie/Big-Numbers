#include "pch.h"
#include "HashMapIterator.h"

HashMapImpl::HashMapImpl(const AbstractObjectManager &keyManager, const AbstractObjectManager &dataManager, HashFunction hash, const AbstractComparator &comparator, size_t capacity)
: HashSetImpl(keyManager, hash, comparator, capacity)
{
  m_dataManager = dataManager.clone(); TRACE_NEW(m_dataManager);
}

AbstractCollection *HashMapImpl::clone(bool cloneData) const {
  throwUnsupportedOperationException(__TFUNCTION__);
  return NULL;
}

AbstractMap *HashMapImpl::cloneMap(bool cloneData) const {
  HashMapImpl      *clone = nullptr;
  AbstractIterator *it    = nullptr;
  clone = new HashMapImpl(*getKeyManager(), *getDataManager(), getHashFunction(), *getComparator(), getCapacity());
  try {
    if(cloneData) {
      it = getIterator(); TRACE_NEW(it);
      while(it->hasNext()) {
        const AbstractEntry *n = (AbstractEntry *)it->next();
        clone->put(n->key(), n->value());
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

HashMapImpl::~HashMapImpl() {
  clear();
  SAFEDELETE(m_dataManager);
}

void HashMapImpl::clear() {
  HashSetImpl::clear();
}

size_t HashMapImpl::size() const {
  return HashSetImpl::size();
}

HashSetNode *HashMapImpl::allocateNode() const {
  HashMapNode *n = new HashMapNode(); TRACE_NEW(n);
  return n;
}

HashMapNode *HashMapImpl::createNode(const void *key, const void *value) const {
  HashMapNode *n = (HashMapNode*)HashSetImpl::createNode(key);
  n->m_value = m_dataManager->cloneObject(value);
  return n;
}

HashSetNode *HashMapImpl::cloneNode(HashSetNode *n) const {
  const HashMapNode *mn = (HashMapNode*)n;
  return createNode(mn->key(), mn->m_value);
}

void HashMapImpl::deleteNode(HashSetNode *n) const {
  HashMapNode *mn = (HashMapNode*)n;
  m_dataManager->deleteObject(mn->m_value);
  HashSetImpl::deleteNode(n);
}

bool HashMapImpl::put(const void *key, const void *value) {
  HashMapNode *n = createNode(key, value);
  try {
    bool result = insertNode(n);
    if(!result) {
      deleteNode(n); // duplicate key
    }
    return result;
  } catch(...) {
    deleteNode(n);
    throw;
  }
}

bool HashMapImpl::remove(const void *key) {
  return HashSetImpl::remove(key);
}

void *HashMapImpl::get(const void *key) const {
  const HashSetNode *n = HashSetImpl::findNode(key);
  return (n != nullptr) ? ((HashMapNode*)n)->value() : nullptr;
}

AbstractEntry *HashMapImpl::getMinEntry() const {
  throwUnsupportedOperationException(__TFUNCTION__);
  return nullptr;
}

AbstractEntry *HashMapImpl::getMaxEntry() const {
  throwUnsupportedOperationException(__TFUNCTION__);
  return nullptr;
}
