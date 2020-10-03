#pragma once

#include "LinkedList.h"

template <typename T> class QueueList : protected LinkedList<T> {
public:
  void clear() override {
    __super::clear();
  }
  size_t size() const override {
    return __super::size();
  }
  inline bool isEmpty() const {
    return (size() == 0);
  }

  void put(const T &e) {
    __super::add(e);
  }

  T get() {
    T e = first();
    removeFirst();
    return e;
  }

  T &operator[](size_t index) {
    return __super::operator[](index);
  }

  const T &operator[](size_t index) const {
    return __super::operator[](index);
  }

  Iterator<T> getIterator() const override {
    return __super::getIterator();
  }
};
