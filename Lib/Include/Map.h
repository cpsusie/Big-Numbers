#pragma once

#include "ObjectManager.h"
#include "Iterator.h"
#include "Comparator.h"
#include "Set.h"

class AbstractEntry {
public:
  virtual const void *key() const   = 0;
  virtual       void *value()       = 0;
  virtual const void *value() const = 0;
};

template <class K, class V> class Entry : public AbstractEntry {
public:
  const K &getKey() const {
    return *(K*)key();
  }

  V &getValue() {
    return *(V*)value();
  }

  const V &getValue() const {
    return *(V*)value();
  }
};

class AbstractMap {
public:
  virtual bool put(const void *key, void *value) = 0;
  virtual bool put(const void *key, const void *value) = 0;
  virtual bool remove(const void *key) = 0;
  virtual const void *get(const void *key) const = 0;
  virtual       void *get(const void *key) = 0;
  virtual int size() const = 0;
  virtual AbstractEntry *selectEntry() const = 0;
  virtual const AbstractEntry *getMinEntry() const = 0;
  virtual const AbstractEntry *getMaxEntry() const = 0;
  virtual void clear() = 0;
  virtual bool hasOrder() const = 0;
  virtual AbstractComparator *getComparator() = 0;
  virtual AbstractMap *cloneMap(bool cloneData) const = 0;
  virtual AbstractSet *getEntrySet();
  virtual AbstractIterator *getIterator() = 0;
  virtual AbstractSet *getKeySet();
  virtual AbstractIterator *getKeyIterator() = 0;
  virtual AbstractCollection *getValues(int (*cmp)(const void *e1, const void *e2));
  virtual AbstractIterator *getValueIterator();
  virtual ~AbstractMap() {}
};

template <class K, class V> class Map {
protected:
  AbstractMap *m_map;
  Map(AbstractMap *map) { 
    m_map = map;
  }
  static int compareValues(const void *v1, const void *v2) {
    return *(V*)v1 == *(V*)v2 ? 0 : 1;
  }
public:
  Map(const Map<K, V> &src) {
    m_map = src.m_map->cloneMap(true);
  }
  virtual ~Map() {
    delete m_map;
  }

  Map &operator=(const Map<K, V> &src) {
    if(this == &src) {
      return *this;
    }
    clear();
    addAll(src);
    return *this;
  }

  bool put(const K &key, V &value) {
    return m_map->put(&key, &value);
  }

  bool put(const K &key, const V &value) {
    return m_map->put(&key, &value);
  }

  const V *get(const K &key) const {
    return (V*)m_map->get(&key);
  }

  V *get(const K &key) {
    return (V*)m_map->get(&key);
  }

  bool remove(const K &key) {
    return m_map->remove(&key);
  }

  int size() const {
    return m_map->size();
  }

  void clear() {
    m_map->clear();
  }

  bool hasOrder() const {
    return m_map->hasOrder();
  }

  Comparator<K> &getComparator() {
    return *(Comparator<K>*)(m_map->getComparator());
  }

  bool isEmpty() const {
    return m_map->size() == 0;
  }

  Set<Entry<K, V> > entrySet() {
    return Set<Entry<K, V> >(m_map->getEntrySet());
  }

  Set<K> keySet() {
    return Set<K>(m_map->getKeySet());
  }

  Collection<V> values() {
    return Collection<V>(m_map->getValues(compareValues));
  }

  bool addAll(const Map<K, V> &src) {
    bool changed = false;
    for(Iterator<Entry<K, V> > it = ((Map<K, V>&)src).entrySet().getIterator(); it.hasNext();) {
      Entry<K, V> &entry = it.next();
      if(put(entry.getKey(), entry.getValue())) {
        changed = true;
      }
    }
    return changed;
  }

  bool removeAll(const Collection<K> &src) {
    bool changed = false;
    for(Iterator<K> it = ((Collection<K>&)src).getIterator(); it.hasNext();) {
      if(remove(it.next())) {
        changed = true;
      }
    }
    return changed;
  }

  friend Map<K, V> operator*(const Map<K, V> &m1, const Map<K, V> &m2) { // intersection
    Map<K, V> result(m1.m_map->cloneMap(false));
    for(Iterator<K> it = ((Map<K, V>&)m1).keySet().getIterator(); it.hasNext();) {
      const K &key   = it.next();
      const V *value = m2.get(key);
      if(value != NULL) {
        result.put(key, *value);
      }
    }
    return result;
  }

  friend Map<K, V> operator+(const Map<K, V> &m1, const Map<K, V> &m2) { // union
    Map<K, V> result(m1);
    result.addAll(m2);
    return result;
  }

  friend Map<K, V> operator-(const Map<K, V> &m1, const Map<K, V> &m2) { // difference
    Map<K, V> result(m1);
    result.removeAll(((Map<K, V>&)m2).keySet());
    return result;
  }

  bool operator==(const Map<K, V> &m) const {
    const int n = size();
    if(n != m.size()) {
      return false;
    }
    Iterator<Entry<K, V> > it1 = ((Map<K, V>*)this)->entrySet().getIterator();
    Iterator<Entry<K, V> > it2 = ((Map<K, V>&)m).entrySet().getIterator();
    int count = 0;
    while(it1.hasNext() && it2.hasNext()) {
      const Entry<K, V> &e1 = it1.next();
      const Entry<K, V> &e2 = it2.next();
      count++;
      if(!(e1.getKey() == e2.getKey()) || !(e1.getValue() == e2.getValue())) {
        return false;
      }
    }
    if(count != n) {
      throwException(_T("Map.operator==:iterators returned %d elements, size=%d"), count, n);
    }
    if(it1.hasNext() || it2.hasNext()) {
      throwException("Map.operator==:iterators didn't return same number of elements");
    }
    return true;
  }

  bool operator!=(const Map<K, V> &m) const {
    return !(*this == m);
  }


  void save(ByteOutputStream &s) const {
    const int keySize   = sizeof(K);
    const int valueSize = sizeof(V);
    const int n         = size();
    Packer header;
    header << keySize << valueSize << n;
    header.write(s);
    for(Iterator<Entry<K, V> > it = ((Map<K, V>*)this)->entrySet().getIterator(); it.hasNext();) {
      Packer p;
      const Entry<K, V> &e = it.next();
      p << e.getKey() << e.getValue();
      p.write(s);
    }
  }

  void load(ByteInputStream &s) {
    Packer header;
    int keySize, valueSize, size;
    if(!header.read(s)) {
      throwException("Map.load:Couldn't read header");
    }
    header >> keySize >> valueSize >> size;
    if(keySize != sizeof(K)) {
      throwException(_T("Map.load:Invalid keysize:%d bytes. Expected keysize = %d bytes"), keySize, sizeof(K));
    }
    if(valueSize != sizeof(V)) {
      throwException(_T("Map.load:Invalid valuesize:%d bytes. Expected valuesize = %d bytes"), valueSize, sizeof(V));
    }
    clear();
    for(int i = 0; i < size; i++) {
      Packer p;
      if(!p.read(s)) {
        throwException(_T("Map.load:Unexpected eos. Expected %d entries. got %d"), size, i);
      }
      K key;
      V value;
      p >> key >> value;
      put(key, value);
    }
  }

  friend Packer &operator<<(Packer &p, const Map<K, V> &m) {
    const int size = m.size();
    p << size;
    for(Iterator<Entry<K, V> > it = ((Map<K, V>&)m).getIterator(); it.hasNext();) {
      const Entry<K, V> &entry = it.next();
      p << entry.getKey() << p << entry.getElement();
    }
    return p;
  }

  friend Packer &operator>>(Packer &p, Map<K, V> &m) {
    int size;
    p >> size;
    for(int i = 0; i < size; i++) {
      K key;
      V value;
      p >> key >> value;
      m.put(key, value);
    }
    return p;
  }

  String toString() const {
    String result = _T("(");
    int count = 0;
    for(Iterator<Entry<K, V> > it = ((Map<K, V>*)this)->getIterator(); it.hasNext();) {
      const Entry<K, V> &entry = it.next();
      if(count++) {
        result += _T(",");
      }
      result += _T("(") + entry.getKey().toString() + _T(",") + entry.getValue().toString() + _T(")");
    }
    result += _T(")");
    return result;
  }
};
