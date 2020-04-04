#pragma once

#include "CompactArray.h"

template <typename T> class CompactStack : private CompactArray<T> {
public:
  CompactStack() {
  }

  explicit CompactStack(UINT capacity) : CompactArray<T>(capacity) {
  }

  inline void push(const T &v) {
    __super::add(v);
  }

  T pop() {
    if(isEmpty()) {
      throwException("stack underflow");
    }
    T e = __super::last();
    __super::removeLast();
    return e;
  }

  T &top(UINT index = 0) {
    const int i = getHeight() - 1 - index;
    if(i < 0) {
      throwException("Cannot get element %u of stack with height %u", index, getHeight());
    }
    return (*this)[i];
  }

  const T &top(UINT index = 0) const {
    const int i = getHeight() - 1 - index;
    if(i < 0) {
      throwException("Cannot get element %u of stack with height %u", index, getHeight());
    }
    return (*this)[i];
  }

  inline UINT getHeight() const {
    return (UINT)__super::size();
  }

  inline void clear() {
    __super::clear();
  }

  inline bool isEmpty() const {
    return getHeight() == 0;
  }

  string toString() const {
    string result;
    for(UINT i = 0; i < getHeight(); i++) {
      result += top(i).toString();
      result += "\n";
    }
    return result;
  }
};
