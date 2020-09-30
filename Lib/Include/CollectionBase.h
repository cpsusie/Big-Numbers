#pragma once

#include "Iterator.h"

template<typename T> class CollectionBase {
public:
  CollectionBase<T> &operator=(const CollectionBase<T> &src) {
    if(this == &src) {
      return *this;
    }
    clear();
    addAll(src);
    return *this;
  }

  virtual ~CollectionBase() {
  }
  virtual void        clear()                    = 0;
  virtual size_t      size()               const = 0;
  virtual bool        add(     const T &e)       = 0;
  virtual Iterator<T> getIterator()        const = 0;
  virtual bool        hasOrder()           const = 0;
  inline  bool        isEmpty()            const {
    return (size() == 0);
  }
  bool addAll(const Iterator<T> &it) {
    const size_t oldSize = size();
    for(Iterator<T> it1 = it; it1.hasNext();) {
      add(it1.next());
    }
    return size() != oldSize;
  }
  // Add every element in c to this. Return true if any elements were added.
  virtual bool addAll(const CollectionBase<T> &c) {
    return addAll(c.getIterator());
  }
};
