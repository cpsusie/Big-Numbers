#pragma once

#include "FastSemaphore.h"
#include "CompactStack.h"

typedef enum {
  SINGLETON_NOTCREATED
 ,SINGLETON_CREATED
 ,SINGLETON_DESTROYED
} SingletonState;

class SingletonFactory;

class Singleton {
  friend void singletonsDestructor();
  friend class SingletonFactory;
private:
  SingletonFactory &m_factory;
  bool              m_registered;
  Singleton(           const Singleton &src); // not implemented. Singleton not cloneable
  Singleton &operator=(const Singleton &src); // not implemented. Singleton not cloneable
protected:
  Singleton(SingletonFactory *factory) : m_factory(*factory), m_registered(false) {
  }
  virtual ~Singleton() {
  }
  // should be called somewhere in constructor, to make automatic destruction atexit
  // If not called in destructor, it will be done in SingletonFactory, right after instance is created (by new)
  void registerThis();
};

class SingletonFactory {
  friend void singletonsDestructor();
private:
  SingletonFactory(           const SingletonFactory &src); // not implemented
  SingletonFactory &operator=(const SingletonFactory &src); // not implemented
protected:
  const TCHAR          *m_className;
  mutable FastSemaphore m_lock;
  SingletonState        m_state;
  template<typename T> T *newInstance() {
    T *s = new T(this);
    s->registerThis();
    return s;
  }
  void deleteInstance(Singleton *s) {
    delete s;
  }
  virtual void releaseInstance() {
  }
public:
  SingletonFactory(const TCHAR *className)
    : m_className(className)
    , m_state(SINGLETON_NOTCREATED)
  {
  }

  SingletonState getState() const {
    m_lock.wait();
    const SingletonState result = m_state;
    m_lock.notify();
    return result;
  }
  inline bool instanceExist() const {
    return (getState() == SINGLETON_CREATED);
  }
  inline const TCHAR *getClassName() const {
    return m_className;
  }
};

template<typename T> class SingletonFactoryTemplate : public SingletonFactory {
private:
  T *m_singleton;

  void releaseInstance() {
    m_lock.wait();
    if(m_singleton != NULL) {
      deleteInstance(m_singleton);
      m_singleton = NULL;
      m_state     = SINGLETON_DESTROYED;
    }
    m_lock.notify();
  }
public:
  SingletonFactoryTemplate(const TCHAR *className)
    : SingletonFactory(className)
    , m_singleton(NULL)
  {
  }
  T *getInstance() {
    m_lock.wait();
    if(m_singleton == NULL) {
      if(m_state != SINGLETON_NOTCREATED) {
        m_lock.notify();
        throwException(_T("%s:%s.state == %d. Must be 0"), __TFUNCTION__, m_className, m_state);
      }
      m_singleton = newInstance<T>();
      m_state     = SINGLETON_CREATED;
    }
    m_lock.notify();
    return m_singleton;
  }
};

// At most 1 instance will be created. when deleted (at program exit), a new instance canNOT be created
// Define a static function: static SingletonClass &get<SingletonClass>()
// containing a static SingletonFactoryTemplate<SingletonClass> factory, with a function:
// SingletonClass &getInstance(), which will return 1, and always the same instance, of the singleton.
// all Singletons are automatic deleted, in the opposite order than they were created, by use of atexit
// (See Singleton.cpp)
#define DEFINESINGLETON(SingletonClass)                                         \
static SingletonClass &get##SingletonClass() {                                  \
  static SingletonFactoryTemplate<SingletonClass> factory(_T(#SingletonClass)); \
  return *factory.getInstance();                                                \
}
