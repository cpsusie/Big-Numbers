#pragma once

#include "FastSemaphore.h"
#include <DebugLog.h>

//#define TRACE_SINGLETONFACTORY
template<typename T> class SingletonFactoryTemplate {
private:
  T            *m_instance;
  FastSemaphore m_gate;
protected:
  virtual T    *newInstance()               = NULL;
  virtual void  deleteInstance(T *instance) = NULL;
  virtual ~SingletonFactoryTemplate() {
  }

public:
  SingletonFactoryTemplate() : m_instance(NULL) {
  }
  T &getInstance() {
    m_gate.wait();
    if(m_instance == NULL) {
      m_instance = newInstance();
    }
    m_gate.notify();
    return *m_instance;
  }
  void releaseInstance() {
    m_gate.wait();
    if(m_instance) {
      m_gate.notify();
      deleteInstance(m_instance);
      m_gate.wait();
      m_instance = NULL;
    }
    m_gate.notify();
  }
};

#define DEFINESINGLETONFACTORY(Singleton)                               \
class Singleton##Factory : public SingletonFactoryTemplate<Singleton> { \
protected:                                                              \
  Singleton *newInstance() {                                            \
    Singleton *p = new Singleton(); TRACE_NEW(p);                       \
    return p;                                                           \
  }                                                                     \
  void deleteInstance(Singleton *instance) {                            \
    SAFEDELETE(instance);                                               \
  }                                                                     \
public:                                                                 \
  ~Singleton##Factory() {                                               \
    releaseInstance();                                                  \
  }                                                                     \
}
