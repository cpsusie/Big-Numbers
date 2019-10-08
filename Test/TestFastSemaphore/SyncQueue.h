#pragma once

#include "FastSemaphore.h"
#include <QueueList.h>

template <class T> class SyncQueue : private QueueList<T> {
private:
  mutable FSemaphore m_gate;
  FSemaphore         m_emptySem;
public:
  SyncQueue() : m_emptySem(0) {
  }

  void put(const T &v) {
    m_gate.wait();
    __super::put(v);
    m_emptySem.signal(); // tell any thread, waiting in get that we are not empty anymore
    m_gate.signal(); // open gate
  }

  T get() {
    for(;;) {
      m_gate.wait();
      if(!isEmpty()) {
        break;
      }
      m_gate.signal(); // open gate
      m_emptySem.wait();
    }
    T result = __super::get();
    m_gate.signal(); // open gate
    return result;
  }

  void clear() {
    m_gate.wait();
    __super::clear();
    m_gate.signal();
  }

  bool addAll(const Collection<T> &c) {
    m_gate.wait();
    const bool result = __super::addAll(c);
    m_gate.signal();
    return result;
  }

  bool addAll(const CompactArray<T> &a) {
    m_gate.wait();
    const bool result = __super::addAll(a);
    m_gate.signal();
    return result;
  }

  bool isEmpty() const {
    return size() == 0;
  }

  size_t size() const {
    return __super::size();
  }

  T operator[](size_t index) const {
    m_gate.wait();
    T result = QueueList<T>::operator[](index);
    m_gate.signal();
    return result;
  }
};
