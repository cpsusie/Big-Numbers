#pragma once

#include "Iterator.h"
#include "Packer.h"
#include "CompactArray.h"

class AbstractCollection {
public:
  virtual bool add(const void *e) = 0;
  virtual bool remove(const void *e) = 0;
  virtual bool contains(const void *e) const = 0;
  virtual const void *select() const = 0;
  virtual void *select() = 0;
  virtual size_t size() const = 0;
  virtual void clear() = 0;
  virtual AbstractCollection *clone(bool cloneData) const = 0;
  virtual AbstractIterator *getIterator() = 0;
  virtual ~AbstractCollection() {}
};

template <class T> class Collection {
protected:
  AbstractCollection *m_collection;
public:
  Collection(AbstractCollection *collection) {
    m_collection = collection;
  }

  Collection(const Collection<T> &src) {
    m_collection = src.m_collection->clone(true);
  }

  virtual ~Collection() {
    delete m_collection;
  }

  Collection<T> &operator=(const Collection<T> &src) {
    if(this == &src) {
      return *this;
    }
    clear();
    addAll(src);
    return *this;
  }

  bool add(const T &e) {
    return m_collection->add(&e);
  }

  bool remove(const T &e) {
    return m_collection->remove(&e);
  }

  bool contains(const T &e) const {
    return m_collection->contains(&e);
  }

  const T &select() const {
    return *(const T*)m_collection->select();
  }

  T &select() {
    return *(T*)m_collection->select();
  }

  Collection<T> getRandomSample(size_t k) const {
    if(k > size()) {
      throwInvalidArgumentException(__TFUNCTION__, _T("k(=%u) > size(=%d)"), k, size());
    }
    CompactArray<const T*> tmp;
    tmp.setCapacity(k);
    Iterator<T> it = ((Collection<T>&)*this).getIterator();
    while(tmp.size() < (int)k) {
      tmp.add(&it.next());
    }
    if(k > 0) {
      for(size_t i = k+1; it.hasNext(); i++) {
        const size_t j = randSizet(i);
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

  size_t size() const {
    return m_collection->size();
  }

  void clear() {
    m_collection->clear();
  }

  Iterator<T> getIterator() {
    return Iterator<T>(m_collection->getIterator());
  }

  bool isEmpty() const {
    return m_collection->size() == 0;
  };

  // Adds every element in c to this. Return true if any elements were added.
  bool addAll(const Collection<T> &c) {
    bool changed = false;
    for(Iterator<T> it = ((Collection<T>&)c).getIterator(); it.hasNext(); ) {
      if(add(it.next())) changed = true;
    }
    return changed;
  }

  bool addAll(const CompactArray<T> &a) {
    bool changed = false;
    for(size_t i = 0; i < a.size(); i++) {
      if(add(a[i])) changed = true;
    }
    return changed;
  }

  // Remove every element in c from this. Return true if any elements were removed.
  bool removeAll(const Collection<T> &c) {
    if(this == &c) {
      if(isEmpty()) return false;
      clear();
      return true;
    }
    bool changed = false;
    for(Iterator<T> it = ((Collection<T>&)c).getIterator(); it.hasNext();) {
      if(remove(it.next())) {
        changed = true;
      }
    }
    return changed;
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

  // Returns true if every element in c is contained in this
  bool containsAll(const Collection<T> &c) const {
    if(this == &c) return true;
    for(Iterator<T> it = ((Collection<T>&)c).getIterator(); it.hasNext();) {
      if(!contains(it.next())) {
        return false;
      }
    }
    return true;
  }

  bool operator==(const Collection<T> &c) const {
    if(this == &c) return true;
    const size_t n = size();
    if(n != c.size()) {
      return false;
    }
    Iterator<T> it1 = ((Collection<T>&)*this).getIterator();
    Iterator<T> it2 = ((Collection<T>&)c).getIterator();
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
      throwException(_T("Collection.operator==:iterators returned %d elements, size=%d"), count, n);
    }
    if(it1.hasNext() || it2.hasNext()) {
      throwException("Collection.operator==:iterators didn't return same number of elements");
    }
    return true;
  }

  bool operator!=(const Collection<T> &c) const {
    return !(*this == c);
  }

  void save(ByteOutputStream &s) const {
    const int    esize = sizeof(T);
    const size_t n     = size();
    Packer header;
    header << esize << n;
    header.write(s);
    for(Iterator<T> it = ((Collection<T>*)this)->getIterator(); it.hasNext();) {
      Packer p;
      p << it.next();
      p.write(s);
    }
  }

  void load(ByteInputStream &s) {
    Packer header;
    int esize;
    size_t size;
    if(!header.read(s)) {
      throwException("Collection.load:Couldn't read header");
    }
    header >> esize >> size;
    if(esize != sizeof(T)) {
      throwException(_T("Collection.load:Invalid element size:%d bytes. Expected %d bytes"), esize, sizeof(T));
    }
    clear();
    for(size_t i = 0; i < size; i++) {
      Packer p;
      if(!p.read(s)) {
        throwException(_T("Collection.load:Unexpected eos. Expected %d elements. got %d"), size, i);
      }
      T e;
      p >> e;
      add(e);
    }
  }

  String toString(TCHAR delimiter = _T(',')) const {
    String result = _T("(");
    Iterator<T> it = ((Collection<T>*)this)->getIterator();
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

  String toStringBasicType(TCHAR delimiter = _T(',')) const {
    String result = _T("(");
    Iterator<T> it = ((Collection<T>*)this)->getIterator();
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
};

template<class S, class T, class D=StreamDelimiter> S &operator<<(S &out, const Collection<T> &c) {
  const D      delimiter;
  const size_t size = c.size();
  out << size << delimiter;
  for(Iterator<T> it = ((Collection<T>&)c).getIterator(); it.hasNext();) {
    out << it.next() << delimiter;
  }
  return out;
}

template<class S, class T> S &operator>>(S &in, Collection<T> &c) {
  size_t size;
  in >> size;
  for(size_t i = 0; i < size; i++) {
    T e;
    in >> e;
    c.add(e);
  }
  return in;
}
