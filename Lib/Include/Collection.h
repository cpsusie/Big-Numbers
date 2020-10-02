#pragma once

#include "CollectionBase.h"
#include "Packer.h"
#include "CompactArray.h"

class AbstractCollection {
private:
  AbstractCollection(           const AbstractCollection &); // not implemented
  AbstractCollection &operator=(const AbstractCollection &); // not implemented
public:
  AbstractCollection() {
  }
  virtual AbstractCollection *clone(bool cloneData)        const = 0;
  virtual ~AbstractCollection() {
  }
  virtual void                clear()                            = 0;
  virtual size_t              size()                       const = 0;
  virtual bool                add(     const void *e)            = 0;
  virtual bool                remove(  const void *e)            = 0;
  virtual bool                contains(const void *e)      const = 0;
  virtual void               *select(RandomGenerator &rnd) const = 0;
  virtual AbstractIterator   *getIterator()                const = 0;
  virtual bool                hasOrder()                   const = 0;
};

template<typename T> class Collection : public CollectionBase<T> {
protected:
  AbstractCollection *m_collection;
public:
  Collection(AbstractCollection *collection) {
    m_collection = collection; TRACE_NEW(collection);
  }

  Collection(const Collection<T> &src) {
    m_collection = src.m_collection->clone(true); TRACE_NEW(m_collection);
  }

  Collection<T> &operator=(const Collection<T> &src) {
    __super::operator=(src);
    return *this;
  }

  Collection<T> &operator=(const CollectionBase<T> &src) {
    __super::operator=(src);
    return *this;
  }

  ~Collection() override {
    SAFEDELETE(m_collection);
  }

  void clear() override {
    m_collection->clear();
  }
  size_t size() const override {
    return m_collection->size();
  }

  bool add(const T &e) override {
    return m_collection->add(&e);
  }

  bool remove(const T &e) {
    return m_collection->remove(&e);
  }

  bool contains(const T &e) const {
    return m_collection->contains(&e);
  }

  const T &select(RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) const {
    return *(const T*)m_collection->select(rnd);
  }

  T &select(RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) {
    return *(T*)m_collection->select(rnd);
  }

  Collection<T> getRandomSample(size_t k, RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) const {
    if(k > size()) {
      throwInvalidArgumentException(__TFUNCTION__, _T("k(=%s) > size(=%s)")
                                   ,format1000(k).cstr(), format1000(size()).cstr());
    }
    CompactArray<const T*> tmp;
    tmp.setCapacity(k);
    Iterator<T> it = getIterator();
    while(tmp.size() < (int)k) {
      tmp.add(&it.next());
    }
    if(k > 0) {
      for(size_t i = k+1; it.hasNext(); i++) {
        const size_t j = randSizet(i, rnd);
        const T     &e = it.next();
        if(j < k) {
          tmp[j] = &e;
        }
      }
    }
    Collection<T> result(m_collection->clone(false));
    for(size_t i = 0; i < k; i++) {
      result.add(*tmp[i]);
    }
    return result;
  }

  Iterator<T> getIterator() const override {
    return Iterator<T>(m_collection->getIterator());
  }
  bool hasOrder() const override {
    return m_collection->hasOrder();
  }

  bool removeAll(const Iterator<T> &it) {
    const size_t oldSize = size();
    for(Iterator<T> it1 = it; it1.hasNext();) {
      remove(it1.next());
    }
    return size() != oldSize;
  }

  // Remove every element in c from this. Return true if any elements were removed.
  bool removeAll(const CollectionBase<T> &c) {
    if(this == &c) {
      if(isEmpty()) return false;
      clear();
      return true;
    }
    return removeAll(c.getIterator());
  }

  // Remove every element from this that is not contained in c. Return true if any elements were removed.
  bool retainAll(const Collection<T> &c) {
    if(this == &c) return false; // Don't change anything. every element in this is in c too => nothing needs to be removed
    bool changed = false;
    for(Iterator<T> it = getIterator(); it.hasNext();) {
      if(!c.contains(it.next())) {
        it.remove();
        changed = true;
      }
    }
    return changed;
  }

  bool containsAll(const Iterator<T> &it) const {
    for(Iterator<T> it1 = it; it1.hasNext();) {
      if(!contains(it1.next())) {
        return false;
      }
    }
    return true;
  }

  // Returns true if every element in c is contained in this
  bool containsAll(const CollectionBase<T> &c) const {
    if(this == &c) return true;
    return containsAll(c.getIterator());
  }

  bool operator==(const Collection<T> &c) const {
    DEFINEMETHODNAME;
    if(this == &c) return true;
    const size_t n = size();
    if(n != c.size()) {
      return false;
    }
    Iterator<T> it1 = getIterator();
    Iterator<T> it2 = c.getIterator();
    size_t count = 0;
    while(it1.hasNext() && it2.hasNext()) {
      const T &e1 = it1.next();
      const T &e2 = it2.next();
      count++;
      if(!(e1 == e2)) {
        return false;
      }
    }
    if(count != n) {
      throwException(_T("%s:Iterators returned %zd elements, size=%zd")
                    ,method, count, n);
    }
    if(it1.hasNext() || it2.hasNext()) {
      throwException(_T("%s:iterators didn't return same number of elements")
                    ,method);
    }
    return true;
  }

  bool operator!=(const Collection<T> &c) const {
    return !(*this == c);
  }

  void save(ByteOutputStream &s) const {
    const int    esize = sizeof(T);
    const UINT64 n     = size();
    Packer header;
    header << esize << n;
    header.write(s);
    for(Iterator<T> it = getIterator(); it.hasNext();) {
      Packer p;
      p << it.next();
      p.write(s);
    }
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
    for(size_t i = 0; i < n; i++) {
      Packer p;
      if(!p.read(s)) {
        throwException(_T("%s:Unexpected eos. Expected %s elements. got %s")
                      ,method, format1000(n).cstr(), format1000(i).cstr());
      }
      T e;
      p >> e;
      add(e);
    }
  }

  String toString(const TCHAR *delimiter = _T(",")) const {
    String result = _T("(");
    Iterator<T> it = getIterator();
    if(it.hasNext()) {
      result += it.next().toString();
      while(it.hasNext()) {
        result += delimiter;
        result += it.next().toString();
      }
    }
    result += _T(")");
    return result;
  }

  String toStringBasicType(const TCHAR *delimiter = _T(",")) const {
    String result = _T("(");
    Iterator<T> it = getIterator();
    if(it.hasNext()) {
      result += ::toString(it.next());
      while(it.hasNext()) {
        result += delimiter;
        result += ::toString(it.next());
      }
    }
    result += _T(")");
    return result;
  }

  String toString(AbstractStringifier<T> &sf, TCHAR *delimiter = _T(",")) const {
    String result = _T("(");
    result += getIterator().toString(sf, delimiter);
    result += _T(")");
    return result;
  }
};

template<typename S, typename T, class D=StreamDelimiter> S &operator<<(S &out, const Collection<T> &c) {
  const D      delimiter;
  const UINT64 size = c.size();
  out << size << delimiter;
  for(Iterator<T> it = c.getIterator(); it.hasNext();) {
    out << it.next() << delimiter;
  }
  return out;
}

template<typename S, typename T> S &operator>>(S &in, Collection<T> &c) {
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
