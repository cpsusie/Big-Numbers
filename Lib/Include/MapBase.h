#pragma once

#include "CollectionBase.h"

class AbstractEntry {
public:
  virtual const void *key()   const = 0;
  virtual       void *value() const = 0;
};

template <typename K, typename V> class Entry : public AbstractEntry {
public:
  const K &getKey() const {
    return *(K*)key();
  }
  const V &getValue() const {
    return *(const V*)value();
  }
  V &getValue() {
    return *(V*)value();
  }
  String toString() const {
    std::wostringstream result;
    result << "(" << getKey() << "," + getValue() + ")";
    return result.str().c_str();
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

  virtual size_t                      size()                               const = 0;
  virtual void                        clear()                                    = 0;
  virtual bool                        put(   const K &key, const V &value)       = 0;
  virtual const                    V *get(   const K &key                ) const = 0;
  virtual                          V *get(   const K &key                )       = 0;
  virtual bool                        remove(const K &key                )       = 0;
  virtual ConstIterator<Entry<K,V> >  getIterator()                        const = 0;
  virtual Iterator<Entry<K,V> >       getIterator()                              = 0;
  virtual ConstIterator<K>            getKeyIterator()                     const = 0;
  virtual Iterator<K>                 getKeyIterator()                           = 0;
  virtual bool                        hasOrder()                           const = 0;

  inline bool                         isEmpty()                            const {
    return size() == 0;
  }

  bool addAll(const ConstIterator<Entry<K,V> > &it) {
    const size_t oldSize = size();
    for(auto it1 = it; it1.hasNext();) {
      const Entry<K,V> &e = it1.next();
      put(e.getKey(), e.getValue());
    }
    return size() != oldSize;
  }
  bool removeAll(const ConstIterator<K> &it) {
    const size_t oldSize = size();
    for(auto it1 = it; it1.hasNext();) {
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

  void save(ByteOutputStream &s) const {
    const int    keySize   = sizeof(K);
    const int    valueSize = sizeof(V);
    const UINT64 n         = size();
    Packer       header;
    header << keySize << valueSize << n;
    header.write(s);
    Packer dp;
    for(auto it = getIterator(); it.hasNext();) {
      const Entry<K, V> &e = it.next();
      dp << e.getKey() << e.getValue();
    }
    dp.write(s);
  }

  void load(ByteInputStream &s) {
    DEFINEMETHODNAME;
    Packer header;
    int    keySize, valueSize;
    UINT64 n;
    if(!header.read(s)) {
      throwException(_T("%s:Couldn't read header"), method);
    }
    header >> keySize >> valueSize >> n;
    if(keySize != sizeof(K)) {
      throwException(_T("%s:Invalid keysize:%d bytes. Expected keysize = %zu bytes")
                    ,method, keySize, sizeof(K));
    }
    if(valueSize != sizeof(V)) {
      throwException(_T("%s:Invalid valuesize:%d bytes. Expected valuesize = %zu bytes")
                    ,method, valueSize, sizeof(V));
    }
    CHECKUINT64ISVALIDSIZET(n);
    clear();
    Packer dp;
    if(!dp.read(s)) {
      throwException(_T("%s:Couldn't load data. Expected %s entries"),method, format1000(n).cstr());
    }
    for(size_t i = 0; i < n; i++) {
      K key;
      V value;
      dp >> key >> value;
      put(key, value);
    }
  }

  inline String toString(AbstractStringifier<Entry<K,V> > &sf, const TCHAR *delimiter = _T(","), BracketType bracketType = BT_ROUNDBRACKETS) const {
    return getIterator().toString(sf, delimiter, bracketType);
  }

  inline String toString(const TCHAR *delimiter = _T(","), BracketType bracketType = BT_ROUNDBRACKETS) const {
    return getIterator().toString(delimiter, bracketType);
  }
};

template<typename OSTREAMTYPE, typename K, typename V, typename D=StreamDelimiter> OSTREAMTYPE &operator<<(OSTREAMTYPE &out, const MapBase<K,V> &m) {
  const D      delimiter;
  const UINT64 size = m.size();
  out << size << delimiter;
  for(auto it = m.getIterator(); it.hasNext();) {
    const Entry<K, V> &e = it.next();
    out << e.getKey() << delimiter << e.getValue() << delimiter;
  }
  return out;
}

template<typename ISTREAMTYPE, typename K, typename V> ISTREAMTYPE &operator>>(ISTREAMTYPE &in, MapBase<K,V> &m) {
  UINT64 size;
  in >> size;
  CHECKUINT64ISVALIDSIZET(size);
  m.clear();
  for(size_t i = 0; i < size; i++) {
    K key;
    V value;
    in >> key >> value;
    m.put(key, value);
  }
  return in;
}
