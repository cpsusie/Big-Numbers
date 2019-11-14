#pragma once

#include <FastSemaphore.h>

template<class T> class FactoryTemplate {
private:
  T              *m_instance;
  FastSemaphore   m_gate;
public:
  FactoryTemplate() : m_instance(NULL) {
  }
  ~FactoryTemplate() {
    SAFEDELETE(m_instance);
  }
  T &getInstance() {
    m_gate.wait();
    if(m_instance == NULL) {
      m_instance = new T; TRACE_NEW(m_instance);
    }
    m_gate.notify();
    return *m_instance;
  }
};
