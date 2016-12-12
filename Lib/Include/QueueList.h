#pragma once

#include "LinkedList.h"

template <class T> class QueueList : protected LinkedList<T> {
public:
  void put(const T &e) {
    LinkedList<T>::add(e);
  }

  T get() {
    T e = first();
    removeFirst();
    return e;
  }

  void clear() {
    LinkedList<T>::clear();
  }
  
  bool isEmpty() const {
    return size() == 0;
  }
  
  size_t size() const {
    return LinkedList<T>::size();
  }
  
  T &operator[](int index) {
    return LinkedList<T>::operator[](index);
  }
  
  const T &operator[](size_t index) const {
    return LinkedList<T>::operator[](index);
  }

  Iterator<T> getIterator() {
    return LinkedList<T>::getIterator();
  }
};
