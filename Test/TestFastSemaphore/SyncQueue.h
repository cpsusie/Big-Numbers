#pragma once

#include <FastSemaphore.h>
#include <QueueList.h>

template <class T> class SyncQueue : private QueueList<T> {
private:
  mutable FastSemaphore m_gate;
  TimedSemaphore        m_emptySem;
public:
  SyncQueue() : m_emptySem(0) {
  }

  void put(const T &v) {
    m_gate.wait();
    __super::put(v);
    m_emptySem.notify(); // tell any thread, waiting in get that we are not empty anymore
    m_gate.notify(); // open gate
  }

  T get(int milliseconds = -1) {
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
    T result = QueueList<T>::operator[](index);
    m_gate.notify();
    return result;
  }
};
