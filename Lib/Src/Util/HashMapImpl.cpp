#include "pch.h"
#include <HashMap.h>

DEFINECLASSNAME(HashMapImpl);

HashMapImpl::HashMapImpl(const AbstractObjectManager &keyManager, const AbstractObjectManager &dataManager, HashFunction hash, const AbstractComparator &comparator, unsigned long capacity)
: HashSetImpl(keyManager, hash, comparator, capacity)
{
  m_dataManager = dataManager.clone();
}

HashMapImpl::~HashMapImpl() {
  clear();
  delete m_dataManager;
}

class HashMapIterator : public HashSetIterator {
public:
  HashMapIterator(HashMapImpl &map) : HashSetIterator(map) {
  }

  AbstractIterator *clone() {
    return new HashMapIterator(*this);
  }
  void *next();
};

void *HashMapIterator::next() {
  return (AbstractEntry*)((HashMapNode*)nextNode());
}

AbstractIterator *HashMapImpl::getIterator() {
  return new HashMapIterator(*this);
}

AbstractMap *HashMapImpl::cloneMap(bool cloneData) const {
  HashMapImpl *clone = new HashMapImpl(*getKeyManager(), *getDataManager(), getHashFunction(),*getComparator(),getCapacity());
  if(cloneData) {
    AbstractIterator *it = ((HashMapImpl*)this)->getIterator();
    while(it->hasNext()) {
      const AbstractEntry *n = (AbstractEntry*)it->next();
      clone->put(n->key(), n->value());
    }
    delete it;
  }
  return clone;
}

HashSetNode *HashMapImpl::allocateNode() const {
  return new HashMapNode();
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

bool HashMapImpl::put(const void *key, void *value) {
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

void *HashMapImpl::get(const void *key) {
  HashSetNode *n = HashSetImpl::findNode(key);
  return (n != NULL) ? ((HashMapNode*)n)->value() : NULL;
}

const void *HashMapImpl::get(const void *key) const {
  const HashSetNode *n = HashSetImpl::findNode(key);
  return (n != NULL) ? ((HashMapNode*)n)->value() : NULL;
}

AbstractEntry *HashMapImpl::selectEntry() const {
  if(size() == 0) {
    throwMethodException(s_className, _T("select"), _T("Map is empty"));
  }
  return (HashMapNode*)findNode(select());
}

const AbstractEntry *HashMapImpl::getMinEntry() const {
  throwMethodUnsupportedOperationException(s_className, _T("getMinEntry"));
  return NULL;
}

const AbstractEntry *HashMapImpl::getMaxEntry() const {
  throwMethodUnsupportedOperationException(s_className, _T("getMaxEntry"));
  return NULL;
}

int HashMapImpl::size() const {
  return HashSetImpl::size();
}

void HashMapImpl::clear() {
  HashSetImpl::clear();
}
