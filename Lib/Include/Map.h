#pragma once

#include "MapBase.h"
#include "ObjectManager.h"
#include "Comparator.h"
#include "Set.h"

class AbstractMap {
public:
  virtual AbstractMap         *cloneMap(bool cloneData)                    const = 0;
  virtual ~AbstractMap() {
  }
  virtual void                 clear()                                           = 0;
  virtual size_t               size()                                      const = 0;
  virtual bool                 put(   const void *key, const void *value)        = 0;
  virtual void                *get(   const void *key)                     const = 0;
  virtual bool                 remove(const void *key)                           = 0;
  virtual AbstractEntry       *selectEntry(RandomGenerator &rnd)           const = 0;
  virtual AbstractEntry       *getMinEntry()                               const = 0;
  virtual AbstractEntry       *getMaxEntry()                               const = 0;
  virtual AbstractComparator  *getComparator()                             const = 0;
  virtual AbstractSet         *getEntrySet();
  virtual AbstractSet         *getKeySet();
  virtual AbstractCollection  *getValues(int (*cmp)(const void *e1, const void *e2));
  virtual AbstractIterator    *getIterator()                               const = 0;
  virtual bool                 hasOrder()                                  const = 0;
  virtual AbstractIterator    *getKeyIterator()                            const = 0;
  virtual AbstractIterator    *getValueIterator()                          const;
};

template <typename K, typename V> class Map : public MapBase<K,V> {
protected:
  AbstractMap *m_map;
  Map(AbstractMap *map) {
    m_map = map; TRACE_NEW(map);
  }
  static int compareValues(const void *v1, const void *v2) {
    return *(V*)v1 == *(V*)v2 ? 0 : 1;
  }
public:
  Map(const Map<K, V> &src) {
    m_map = src.m_map->cloneMap(true); TRACE_NEW(m_map);
  }
  ~Map() override {
    SAFEDELETE(m_map);
  }

  Map<K,V> &operator=(const Map<K,V> &src) {
    __super::operator=(src);
    return *this;
  }

  void clear() override {
    m_map->clear();
  }

  size_t size() const override {
    return m_map->size();
  }

  bool put(const K &key, const V &value) override {
    return m_map->put(&key, &value);
  }

  const V *get(const K &key) const override {
    return (V*)m_map->get(&key);
  }

  V *get(const K &key) override {
    return (V*)m_map->get(&key);
  }

  bool remove(const K &key) override {
    return m_map->remove(&key);
  }

  Comparator<K> &getComparator() const {
    return *(Comparator<K>*)(m_map->getComparator());
  }

  const Set<Entry<K, V> > entrySet() const {
    return Set<Entry<K,V> >(m_map->getEntrySet());
  }
  Set<Entry<K, V> >       entrySet() {
    return Set<Entry<K,V> >(m_map->getEntrySet());
  }
  const Set<K>            keySet() const {
    return Set<K>(m_map->getKeySet());
  }
  Set<K>                  keySet() {
    return Set<K>(m_map->getKeySet());
  }
  const Collection<V>     values() const {
    return Collection<V>(m_map->getValues(compareValues));
  }
  Collection<V>           values() {
    return Collection<V>(m_map->getValues(compareValues));
  }

  ConstIterator<Entry<K,V> > getIterator()    const override {
    return entrySet().getIterator();
  }
  Iterator<Entry<K,V> >      getIterator()          override {
    return entrySet().getIterator();
  }

  ConstIterator<K>           getKeyIterator() const override {
    return keySet().getIterator();
  }
  Iterator<K>                getKeyIterator()       override {
    return keySet().getIterator();
  }

  bool hasOrder()                             const override {
    return m_map->hasOrder();
  }

  friend Map<K, V> operator*(const Map<K, V> &m1, const Map<K, V> &m2) { // intersection
    Map<K, V> result(m1.m_map->cloneMap(false));
    for(ConstIterator<K> it = m1.keySet().getIterator(); it.hasNext();) {
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
    result.removeAll(m2.keySet());
    return result;
  }

  bool operator==(const Map<K, V> &m) const {
    DEFINEMETHODNAME;
    const size_t n = size();
    if(n != m.size()) {
      return false;
    }
    ConstIterator<Entry<K, V> > it1 = getIterator();
    ConstIterator<Entry<K, V> > it2 = m.getIterator();
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
};
