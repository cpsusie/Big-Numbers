#pragma once

#include "MyString.h"
#include "Array.h"
#include "Exception.h"

template <class T> class Stack : private Array<T> {
public:
  void push(const T &v) {
    add(v);
  }

  T pop() {
    if(isEmpty()) {
      throwException(_T("Stack::pop:Stack is empty"));
    }
    T e = last();
    removeLast();
    return e;
  }

  T &top(unsigned int index = 0) {
    int i = size() - 1 - index;
    if(i < 0) {
      throwException(_T("Cannot get element %d of stack with height %d"), index, size());
    }
    return (*this)[i];
  }

  const T &top(unsigned int index = 0) const {
    int i = size() - 1 - index;
    if(i < 0) {
      throwException(_T("Cannot get element %d of stack with height %d"), index, size());
    }
    return (*this)[i];
  }

  inline unsigned int getHeight() const {
    return size();
  }

  inline void clear() {
    Array<T>::clear();
  }

  inline bool isEmpty() const {
    return size() == 0;
  }

  String toString() const {
    String result;
    for(unsigned int i = 0; i < getHeight(); i++) {
      result += top(i).toString() + _T("\n");
    }
    return result;
  }
};
