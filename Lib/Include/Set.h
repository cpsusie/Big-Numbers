#pragma once

#include "ObjectManager.h"
#include "Comparator.h"
#include "Collection.h"

class AbstractKey {
public:
  virtual const void *key() const = 0;
  virtual ~AbstractKey() {
  }
};

class AbstractSet : public AbstractCollection {
public:
  virtual AbstractComparator *getComparator() const = 0;
  virtual void               *getMin()        const = 0;
  virtual void               *getMax()        const = 0;
};

template <typename T> class Set : public Collection<T> {
public:
  Set(AbstractSet *set) : Collection<T>(set) {
  }

  Set &operator=(const CollectionBase<T> &src) {
    __super::operator=(src);
    return *this;
  }

  Comparator<T> &getComparator() {
    return *(Comparator<T>*)(((AbstractSet*)m_collection)->getComparator());
  }

  // Set intersection = set of elements that are in both sets.
  Set operator*(const Set &set) const {
    Set result((AbstractSet*)m_collection->clone(false));
    if(size() < set.size()) {
      for(auto it = getIterator(); it.hasNext();) {
        const T &e = it.next();
        if(set.contains(e)) {
          result.add(e);
        }
      }
    } else {
      for(auto it = set.getIterator(); it.hasNext();) {
        const T &e = it.next();
        if(contains(e)) {
          result.add(e);
        }
      }
    }
    return result;
  }

  // Set union = set of elements, that are in at least 1 of the sets.
  Set operator+(const Set &set) const {
    Set result(*this);
    result.addAll(set);
    return result;
  }

  // Set difference = set of elements in *this and not in set.
  Set operator-(const Set &set) const {
    Set result(*this);
    result.removeAll(set);
    return result;
  }

  // s1^s2 = (s1-s2) + (s2-s1) (symmetric difference) = set of elements that are in only one of the sets
  Set operator^(const Set &set) const {
    Set result((AbstractSet*)m_collection->clone(false));
    for(auto it = getIterator(); it.hasNext();) {
      const T &e = it.next();
      if(!set.contains(e)) {
        result.add(e);
      }
    }
    for(auto it = set.getIterator(); it.hasNext();) {
      const T &e = it.next();
      if(!contains(e)) {
        result.add(e);
      }
    }
    return result;
  }

  // Subset. Return true if all elements in *this are in set
  bool operator<=(const Set &set) const {
    for(auto it = getIterator(); it.hasNext();) {
      if(!set.contains(it.next())) {
        return false;
      }
    }
    return true;
  }

  bool operator<(const Set &set) const { // pure subset
    return (size() < set.size()) && (*this <= set);
  }

  bool operator>=(const Set &set) const {
    return set <= *this;
  }

  bool operator>(const Set &set) const {
    return set < *this;
  }

  const T &getMin() const {
    return *(T*)(((AbstractSet*)m_collection)->getMin());
  }

  const T &getMax() const {
    return *(T*)(((AbstractSet*)m_collection)->getMax());
  }
};
