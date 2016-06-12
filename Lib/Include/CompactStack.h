#pragma once

#include "MyString.h"
#include "CompactArray.h"
#include "Exception.h"

template <class T> class CompactStack : private CompactArray<T> {
public:
  CompactStack() {
  }

  explicit CompactStack(unsigned int capacity) : CompactArray<T>(capacity) {
  }

  inline void push(const T &v) {
    add(v);
  }

  T pop() {
    if(isEmpty()) {
      throwException(_T("stack underflow"));
    }
    T e = last();
    removeLast();
    return e;
  }

  T &top(unsigned int index = 0) {
    int i = (int)size() - 1 - index;
    if(i < 0) {
      throwException(_T("Cannot get element %d of stack with height %d."), index, size());
    }
    return (*this)[i];
  }

  const T &top(unsigned int index = 0) const {
    int i = (int)size() - 1 - index;
    if(i < 0) {
      throwException(_T("Cannot get element %d of stack with height %d."), index, size());
    }
    return (*this)[i];
  }

  inline UINT getHeight() const {
    return (UINT)size();
  }

  inline void clear() {
    CompactArray<T>::clear();
  }

  inline bool isEmpty() const {
    return size() == 0;
  }

  String toString() const {
    String result;
    for(int i = 0; i < getHeight(); i++) {
      result += top(i).toString() + _T("\n");
    }
    return result;
  }
};
