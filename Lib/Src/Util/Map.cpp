#include "pch.h"
#include <Map.h>

// ------------------------------------EntrySet--------------------------------------

class EntrySet : public AbstractSet {
private:
  AbstractMap *m_map;
  bool         m_deleteMap;
public:
  EntrySet(AbstractMap *map, bool deleteMap) : m_map(map) {
    m_deleteMap = deleteMap;
  }
  bool add(const void *e) {
    AbstractEntry* ae = (AbstractEntry*)e;
    return m_map->put(ae->key(),ae->value());
  }
  bool remove(const void *e) {
    return m_map->remove(((AbstractEntry*)e)->key());
  }
  bool contains(const void *e) const {
    return m_map->get(((AbstractEntry*)e)->key()) != NULL;
  }
  const void *select() const;
  void *select();
  size_t size() const {
    return m_map->size();
  }
  void clear() {
    m_map->clear();
  }
  bool hasOrder() const {
    return m_map->hasOrder();
  }
  AbstractComparator *getComparator() {
    return m_map->getComparator();
  }
  const void *getMin() const;
  const void *getMax() const;

  AbstractCollection *clone(bool cloneData) const {
    return new EntrySet(m_map->cloneMap(cloneData),true);
  }
  AbstractIterator *getIterator() {
    return m_map->getIterator();
  }
  ~EntrySet() {
    if(m_deleteMap) {
      SAFEDELETE(m_map);
    }
  }
};

const void *EntrySet::select() const {
  return m_map->selectEntry();
}

void *EntrySet::select() {
  return m_map->selectEntry();
}

const void *EntrySet::getMin() const {
  return m_map->getMinEntry();
}

const void *EntrySet::getMax() const {
  return m_map->getMaxEntry();
}

// ------------------------------------KeySet-------------------------------------

class KeySet : public AbstractSet {
private:
  AbstractMap *m_map;
  bool         m_deleteMap;
public:
  KeySet(AbstractMap *map, bool deleteMap) : m_map(map) {
    m_deleteMap = deleteMap;
  }
  bool add(const void *e) {
    throwUnsupportedOperationException(__TFUNCTION__);
    return false;
  }
  bool remove(const void *e) {
    return m_map->remove(e);
  }
  bool contains(const void *e) const {
    return m_map->get(e) != NULL;
  }

  const void *select() const;
  void *select();

  size_t size() const {
    return m_map->size();
  }
  void clear() {
    m_map->clear();
  }
  bool hasOrder() const {
    return m_map->hasOrder();
  }
  AbstractComparator *getComparator() {
    return m_map->getComparator();
  }
  const void *getMin() const;
  const void *getMax() const;
  AbstractCollection *clone(bool cloneData) const {
    return new KeySet(m_map->cloneMap(cloneData),true);
  }
  AbstractIterator *getIterator() {
    return m_map->getKeyIterator();
  }
  ~KeySet() {
    if(m_deleteMap) {
      SAFEDELETE(m_map);
    }
  }
};

const void *KeySet::select() const {
  return m_map->selectEntry()->key();
}

void *KeySet::select() {
  return (void*)m_map->selectEntry()->key();
}

const void *KeySet::getMin() const {
  return m_map->getMinEntry()->key();
}

const void *KeySet::getMax() const {
  return m_map->getMaxEntry()->key();
}
// ------------------------------------ValueCollection-------------------------------------

class ValueCollection : public AbstractCollection {
private:
  AbstractMap *m_map;
  bool         m_deleteMap;
  int        (*m_cmp)(const void *e1, const void *e2);
  void selectError(const TCHAR *method) const;
public:
  ValueCollection(AbstractMap *map, bool deleteMap,int (*cmp)(const void *e1, const void *e2)) : m_map(map) {
    m_deleteMap = deleteMap;
    m_cmp       = cmp;
  }
  bool add(const void *e) {
    throwUnsupportedOperationException(__TFUNCTION__);
    return false;
  }
  bool remove(const void *e) {
    throwUnsupportedOperationException(__TFUNCTION__);
    return false;
  }
  bool contains(const void *e) const;
  const void *select() const;
  void *select();

  size_t size() const {
    return m_map->size();
  }
  void clear() {
    m_map->clear();
  }
  AbstractCollection *clone(bool cloneData) const {
    return new ValueCollection(m_map->cloneMap(cloneData),true,m_cmp);
  }
  AbstractIterator *getIterator() {
    return m_map->getValueIterator();
  }

  ~ValueCollection() {
    if(m_deleteMap) {
      SAFEDELETE(m_map);
    }
  }
};

bool ValueCollection::contains(const void *e) const {
  bool result = false;
  AbstractIterator *it = ((ValueCollection*)this)->getIterator(); TRACE_NEW(it);
  while(it->hasNext()) {
    void *e1 = it->next();
    if(m_cmp(e,e1) == 0) {
      result = true;
      break;
    }
  }
  SAFEDELETE(it);
  return result;
}

const void *ValueCollection::select() const {
  if(size() == 0) {
    selectError(__TFUNCTION__);
  }
  return m_map->selectEntry()->value();
}

void *ValueCollection::select() {
  if(size() == 0) {
    selectError(__TFUNCTION__);
  }
  return m_map->selectEntry()->value();
}

void ValueCollection::selectError(const TCHAR *method) const {
  throwSelectFromEmptyCollectionException(method);
}

// ------------------------------------ValueIterator-----------------------------------------------------

class ValueIterator : public AbstractIterator {
private:
  AbstractIterator *m_it;
  ValueIterator(const ValueIterator *src) {
    m_it = src->m_it->clone(); TRACE_NEW(m_it);
  }
public:
  ValueIterator(AbstractMap *map) {
    m_it = map->getIterator(); TRACE_NEW(m_it);
  }
  AbstractIterator *clone() {
    return new ValueIterator(this);
  }
  ~ValueIterator() {
    SAFEDELETE(m_it);
  }
  bool hasNext() const {
    return m_it->hasNext();
  }
  void *next() {
    return ((AbstractEntry *)m_it->next())->value();
  }
  void remove() {
    m_it->remove();
  }
};

// -----------------------------------------------------------------------------------------

AbstractSet *AbstractMap::getEntrySet() {
  return new EntrySet(this, false);
}

AbstractSet *AbstractMap::getKeySet() {
  return new KeySet(this, false);
}

AbstractCollection *AbstractMap::getValues(int (*cmp)(const void *e1, const void *e2)) {
  return new ValueCollection(this, false, cmp);
}

AbstractIterator *AbstractMap::getValueIterator() {
  return new ValueIterator(this);
}
