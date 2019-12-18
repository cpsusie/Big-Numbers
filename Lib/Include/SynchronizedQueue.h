#pragma once

#include "Semaphore.h"
#include "FastSemaphore.h"
#include "QueueList.h"

template <typename T> class SynchronizedQueue : private QueueList<T> {
private:
  mutable FastSemaphore m_lock;
  Semaphore             m_emptySem;
public:
  SynchronizedQueue() : m_emptySem(0) {
  }

  void put(const T &v) {
    m_lock.wait();
    __super::put(v);
    m_emptySem.notify(); // tell any thread, waiting in get that we are not empty anymore
    m_lock.notify(); // open gate
  }

  T get(int milliseconds=INFINITE) {
    for(;;) {
      m_lock.wait();
      if(!isEmpty()) {
        break;
      }
      m_lock.notify(); // open gate
      if(!m_emptySem.wait(milliseconds)) { //wait=false => timeout
        throwTimeoutException(_T("timeout"));
      }
    }
    T result = __super::get();
    m_lock.notify(); // open gate
    return result;
  }

  void clear() {
    m_lock.wait();
    __super::clear();
    m_lock.notify();
  }

  bool addAll(const Collection<T> &c) {
    m_lock.wait();
    const bool result = __super::addAll(c);
    m_lock.notify();
    return result;
  }

  bool addAll(const CompactArray<T> &a) {
    m_lock.wait();
    const bool result = __super::addAll(a);
    m_lock.notify();
    return result;
  }

  bool isEmpty() const {
    return size() == 0;
  }

  size_t size() const {
    return __super::size();
  }

  T operator[](size_t index) const {
    m_lock.wait();
    T result = __super::operator[](index);
    m_lock.notify();
    return result;
  }
};
