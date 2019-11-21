#pragma once

#include "Semaphore.h"
#include "QueueList.h"

template <class T> class SynchronizedQueue : private QueueList<T> {
private:
  mutable Semaphore m_gate;
  Semaphore         m_emptySem;
public:
  SynchronizedQueue() : m_emptySem(0) {
  }

  void put(const T &v) {
    m_gate.wait();
    __super::put(v);
    m_emptySem.notify(); // tell any thread, waiting in get that we are not empty anymore
    m_gate.notify(); // open gate
  }

  T get(int milliseconds=INFINITE) {
    for(;;) {
      m_gate.wait();
      if(!isEmpty()) {
        break;
      }
      m_gate.notify(); // open gate
      if(!m_emptySem.wait(milliseconds)) { //wait=false => timeout
        throwTimeoutException(_T("timeout"));
      }
    }
    T result = __super::get();
    m_gate.notify(); // open gate
    return result;
  }

  void clear() {
    m_gate.wait();
    __super::clear();
    m_gate.notify();
  }

  bool addAll(const Collection<T> &c) {
    m_gate.wait();
    const bool result = __super::addAll(c);
    m_gate.notify();
    return result;
  }

  bool addAll(const CompactArray<T> &a) {
    m_gate.wait();
    const bool result = __super::addAll(a);
    m_gate.notify();
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
    T result = __super::operator[](index);
    m_gate.notify();
    return result;
  }
};
