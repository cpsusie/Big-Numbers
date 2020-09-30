#pragma once

#include "CollectionBase.h"

class AbstractEntry {
public:
  virtual const void *key() const   = 0;
  virtual       void *value()       = 0;
  virtual const void *value() const = 0;
};

template <typename K, typename V> class Entry : public AbstractEntry {
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

template<typename K, typename V> class MapBase {
public:
  MapBase<K,V> &operator=(const MapBase<K,V> &src) {
    if(this == &src) {
      return *this;
    }
    clear();
    addAll(src);
    return *this;
  }
  virtual ~MapBase() {
  }

  virtual size_t   size()                         const = 0;
  virtual void     clear()                              = 0;
  virtual bool     put(   const K &key, const V &value) = 0;
  virtual const V *get(   const K &key)           const = 0;
  virtual       V *get(   const K &key)                 = 0;
  virtual bool     remove(const K &key)                 = 0;
  virtual Iterator<Entry<K, V> > getIterator()    const = 0;
  virtual bool     hasOrder()                     const = 0;

  inline bool isEmpty() const {
    return size() == 0;
  }

  bool addAll(const Iterator<Entry<K,V> > &it) {
    const size_t oldSize = size();
    for(Iterator<Entry<K,V> > it1 = it; it1.hasNext();) {
      const Entry<K,V> &e = it1.next();
      put(e.getKey(), e.getValue());
    }
    return size() != oldSize;
  }
  bool removeAll(const Iterator<K> &it) {
    const size_t oldSize = size();
    for(Iterator<K> it1 = it; it1.hasNext();) {
      remove(it1.next());
    }
    return size() != oldSize;
  }
  bool addAll(const MapBase<K,V> &m) {
    if(this == &m) {
      return false;
    }
    return addAll(m.getIterator());
  }
  bool removeAll(const CollectionBase<K> &c) {
    return removeAll(c.getIterator());
  }
};
