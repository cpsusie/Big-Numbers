#pragma once

#include "Iterator.h"
#include "Packer.h"

template<typename T> class CollectionBase {
public:
  CollectionBase &operator=(const CollectionBase &src) {
    if(this == &src) {
      return *this;
    }
    clear();
    addAll(src);
    return *this;
  }

  virtual ~CollectionBase() {
  }
  virtual void             clear()               = 0;
  virtual size_t           size()          const = 0;
  virtual bool             add(const T &e)       = 0;
  virtual ConstIterator<T> getIterator()   const = 0;
  virtual Iterator<T>      getIterator()         = 0;
  virtual bool             hasOrder()      const = 0;
  inline  bool             isEmpty()       const {
    return (size() == 0);
  }
  bool addAll(const ConstIterator<T> &it) {
    const size_t oldSize = size();
    for(auto it1 = it; it1.hasNext();) {
      add(it1.next());
    }
    return size() != oldSize;
  }
  // Add every element in c to this. Return true if any elements were added.
  virtual bool addAll(const CollectionBase &c) {
    return addAll(c.getIterator());
  }

  void save(ByteOutputStream &s) const {
    const int    esize = sizeof(T);
    const UINT64 n     = size();
    Packer header;
    header << esize << n;
    header.write(s);
    Packer dp;
    for(auto it = getIterator(); it.hasNext();) {
      dp << it.next();
    }
    dp.write(s);
  }

  void load(ByteInputStream &s) {
    DEFINEMETHODNAME;
    Packer header;
    int    esize;
    UINT64 n;
    if(!header.read(s)) {
      throwException(_T("%s:Couldn't read header"), method);
    }
    header >> esize >> n;
    if(esize != sizeof(T)) {
      throwException(_T("%s:Invalid element size:%d bytes. Expected %zu bytes")
                    ,method, esize, sizeof(T));
    }
    CHECKUINT64ISVALIDSIZET(n);
    clear();
    Packer dp;
    if(!dp.read(s)) {
      throwException(_T("%s:Couldn't load data. Expected %s elements"),method, format1000(n).cstr());
    }
    for(size_t i = 0; i < n; i++) {
      T e;
      dp >> e;
      add(e);
    }
  }

  inline String toString(AbstractStringifier<T> &sf, const TCHAR *delimiter = _T(","), BracketType bracketType = BT_ROUNDBRACKETS) const {
    return getIterator().toString(sf, delimiter, bracketType);
  }

  inline String toString(const TCHAR *delimiter = _T(","), BracketType bracketType = BT_ROUNDBRACKETS) const {
    return getIterator().toString(delimiter, bracketType);
  }
};

template<typename OSTREAMTYPE, typename T, class D=StreamDelimiter> OSTREAMTYPE &operator<<(OSTREAMTYPE &out, const CollectionBase<T> &c) {
  const D      delimiter;
  const UINT64 size = c.size();
  out << size << delimiter;
  for(auto it = c.getIterator(); it.hasNext();) {
    out << it.next() << delimiter;
  }
  return out;
}

template<typename ISTREAMTYPE, typename T> ISTREAMTYPE &operator>>(ISTREAMTYPE &in, CollectionBase<T> &c) {
  UINT64 size;
  in >> size;
  CHECKUINT64ISVALIDSIZET(size);
  c.clear();
  for(size_t i = 0; i < size; i++) {
    T e;
    in >> e;
    c.add(e);
  }
  return in;
}
