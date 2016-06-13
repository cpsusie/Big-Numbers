#pragma once

#include "Semaphore.h"
#include "QueueList.h"

template <class T> class SynchronizedQueue : private QueueList<T> {
private:
  mutable Semaphore m_gate;
  Semaphore m_emptySem;
public:
  SynchronizedQueue() : m_emptySem(0) {
  }
  
  void put(const T &v) {
    m_gate.wait();
    QueueList<T>::put(v);
    m_emptySem.signal(); // tell any thread, waiting in get that we are not empty anymore
    m_gate.signal(); // open gate
  }
  
  T get(int milliseconds=INFINITE) {
    for(;;) {
      m_gate.wait();
      if(!isEmpty()) {
        break;
      }
      m_gate.signal(); // open gate
      if(!m_emptySem.wait(milliseconds)) { //wait=false => timeout
        throwTimeoutException(_T("timeout"));
      }
    }
    T result = QueueList<T>::get();
    m_gate.signal(); // open gate
    return result;
  }
  
  void clear() {
    m_gate.wait();
    QueueList<T>::clear();
    m_gate.signal();
  }

  bool isEmpty() const {
    return size() == 0;
  }
  
  size_t size() const { 
    return QueueList<T>::size();
  }

  T operator[](size_t index) const {
    m_gate.wait();
    T result = QueueList<T>::operator[](index);
    m_gate.signal();
    return result;
  }
};
