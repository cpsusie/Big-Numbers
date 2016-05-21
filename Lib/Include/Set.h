#pragma once

#include "ObjectManager.h"
#include "Comparator.h"
#include "Collection.h"
#include "Array.h"
#include "MyString.h"

class AbstractKey {
public:
  virtual const void *key() const = 0;
  virtual ~AbstractKey() {
  }
};

class AbstractSet : public AbstractCollection {
public:
  virtual bool                hasOrder()      const = 0;
  virtual AbstractComparator *getComparator()       = 0;
  virtual const void         *getMin()        const = 0;
  virtual const void         *getMax()        const = 0;
};

template <class T> class Set : public Collection<T> {
public:
  Set(AbstractSet *set) : Collection<T>(set) {
  }

  Set<T> &operator=(const Collection<T> &src) {
    if(this == &src) {
      return *this;
    }
    clear();
    addAll(src);
    return *this;
  }

  bool hasOrder() const {
    return ((AbstractSet*)m_collection)->hasOrder();
  }

  Comparator<T> &getComparator() {
    return *(Comparator<T>*)(((AbstractSet*)m_collection)->getComparator());
  }

  Set<T> operator*(const Set<T> &s) const { // intersection
    Set<T> result((AbstractSet*)m_collection->clone(false));
    if(size() < s.size()) {
      for(Iterator<T> it = ((Set<T>&)*this).getIterator(); it.hasNext();) {
        const T &e = it.next();
        if(s.contains(e)) {
          result.add(e);
        }
      }
    } else {
      for(Iterator<T> it = ((Set<T>&)s).getIterator(); it.hasNext();) {
        const T &e = it.next();
        if(contains(e)) {
          result.add(e);
        }
      }
    }
    return result;
  }

  Set<T> operator+(const Set<T> &s) const { // union
    Set<T> result(*this);
    result.addAll(s);
    return result;
  }

  Set<T> operator-(const Set<T> &s) const { // difference
    Set<T> result(*this);
    result.removeAll(s);
    return result;
  }

  Set <T> operator^(const Set<T> &rhs) const { // s1^s2 = (s1-s2) + (s2-s1) (symmetric difference) = set containg elements that are in only one of the sets
    Set<T> result((AbstractSet*)m_collection->clone(false));
    for(Iterator<T> it = ((Set<T>&)*this).getIterator(); it.hasNext();) {
      const T &e = it.next();
      if(!rhs.contains(e)) {
        result.add(e);
      }
    }
    for(Iterator<T> it = ((Set<T>&)rhs).getIterator(); it.hasNext();) {
      const T &e = it.next();
      if(!contains(e)) {
        result.add(e);
      }
    }
    return result;
  }

  bool operator<=(const Set<T> &rhs) const { // subset
    for(Iterator<T> it = ((Set<T>&)*this).getIterator(); it.hasNext();) {
      if(!rhs.contains(it.next())) {
        return false;
      }
    }
    return true;
  }

  bool operator<(const Set<T> &rhs) const { // pure subset
    return (size() < rhs.size()) && (*this <= rhs);
  }

  bool operator>=(const Set<T> &rhs) const {
    return rhs <= *this;
  }

  bool operator>(const Set<T> &rhs) const {
    return rhs < *this;
  }

  const T &getMin() const {
    return *(T*)(((AbstractSet*)m_collection)->getMin());
  }

  const T &getMax() const {
    return *(T*)(((AbstractSet*)m_collection)->getMax());
  }
};
