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

  String toString() const {
    return _T("(") + getKey().toString() + _T(",") + getValue().toString() + _T(")");
  }
};

class AbstractMap {
public:
  virtual bool put(const void *key, void *value) = 0;
  virtual bool put(const void *key, const void *value) = 0;
  virtual bool remove(const void *key) = 0;
  virtual const void *get(const void *key) const = 0;
  virtual       void *get(const void *key) = 0;
  virtual size_t size() const = 0;
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

  size_t size() const {
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
    if (this == &src) {
      return false;
    }
    const size_t n = size();
    for(Iterator<Entry<K, V> > it = ((Map<K, V>&)src).entrySet().getIterator(); it.hasNext();) {
      Entry<K, V> &entry = it.next();
      put(entry.getKey(), entry.getValue());
    }
    return size() != n;
  }

  bool removeAll(const Collection<K> &src) {
    const size_t n = size();
    for(Iterator<K> it = ((Collection<K>&)src).getIterator(); it.hasNext();) {
      remove(it.next());
    }
    return size() != n;
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
    DEFINEMETHODNAME;
    const size_t n = size();
    if(n != m.size()) {
      return false;
    }
    Iterator<Entry<K, V> > it1 = ((Map<K, V>*)this)->entrySet().getIterator();
    Iterator<Entry<K, V> > it2 = ((Map<K, V>&)m).entrySet().getIterator();
    size_t count = 0;
    while(it1.hasNext() && it2.hasNext()) {
      const Entry<K, V> &e1 = it1.next();
      const Entry<K, V> &e2 = it2.next();
      count++;
      if(!(e1.getKey() == e2.getKey()) || !(e1.getValue() == e2.getValue())) {
        return false;
      }
    }
    if(count != n) {
      throwException(_T("%s:Iterators returned %zd elements, size=%zd"), method, count, n);
    }
    if(it1.hasNext() || it2.hasNext()) {
      throwException(_T("%s:Iterators didn't return same number of elements"), method);
    }
    return true;
  }

  bool operator!=(const Map<K, V> &m) const {
    return !(*this == m);
  }

  void save(ByteOutputStream &s) const {
    const int    keySize   = sizeof(K);
    const int    valueSize = sizeof(V);
    const size_t n         = size();
    Packer       header;
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
    DEFINEMETHODNAME;
    Packer header;
    int keySize, valueSize;
    size_t size;
    if(!header.read(s)) {
      throwException(_T("%s:Couldn't read header"), method);
    }
    header >> keySize >> valueSize >> size;
    if(keySize != sizeof(K)) {
      throwException(_T("%s:Invalid keysize:%d bytes. Expected keysize = %d bytes")
                    ,method, keySize, sizeof(K));
    }
    if(valueSize != sizeof(V)) {
      throwException(_T("%s:Invalid valuesize:%d bytes. Expected valuesize = %d bytes")
                    ,method, valueSize, sizeof(V));
    }
    clear();
    for(size_t i = 0; i < size; i++) {
      Packer p;
      if(!p.read(s)) {
        throwException(_T("%s:Unexpected eos. Expected %zd entries. got %zd")
                      ,method, size, i);
      }
      K key;
      V value;
      p >> key >> value;
      put(key, value);
    }
  }

  String toString(const TCHAR *delimiter = _T(",")) const {
    String result = _T("(");
    const TCHAR *delim = NULL;
    for(Iterator<Entry<K, V> > it = ((Map<K, V>*)this)->getIterator(); it.hasNext();) {
      const Entry<K, V> &entry = it.next();
      if(delim) result += delim; else delim = delimiter;
      result += entry.toString();
    }
    result += _T(")");
    return result;
  }
};

template<class S, class K, class V, class D=StreamDelimiter> S &operator<<(S &out, const Map<K, V> &m) {
  const D      delimiter;
  const size_t size = m.size();
  out << size << delimiter;
  for(Iterator<Entry<K, V> > it = ((Map<K, V>&)m).entrySet().getIterator(); it.hasNext();) {
    Entry<K, V> &e = it.next();
    out << e.getKey() << delimiter << e.getValue() << delimiter;
  }
  return out;
}

template<class S, class K, class V> S &operator>>(S &in, Map<K, V> &m) {
  size_t size;
  in >> size;
  for(size_t i = 0; i < size; i++) {
    K key;
    V value;
    in >> key >> value;
    m.put(key, value);
  }
  return in;
}
