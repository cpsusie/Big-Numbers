#pragma once

#include <atomic>

class Singleton {
private:
  static std::atomic<UINT> s_count;
  const String m_className;
  Singleton(           const Singleton &src); // not implemented. Singleton not cloneable
  Singleton &operator=(const Singleton &src); // not implemented. Singleton not cloneable
  static String getClassNameFromConstructorName(const TCHAR *constructorName);
protected:
  Singleton(const TCHAR *constructorName);
  virtual ~Singleton();
public:
  inline const String &getClassName() const {
    return m_className;
  }
};

// At most 1 instance will be created. when deleted (at program exit), a new instance can NOT be created
#define DEFINESINGLETON(SingletonClass)                               \
private:                                                              \
  /* Copy constructor declared private, but not implemented */        \
  SingletonClass(const SingletonClass &src);                          \
  /* Default assign operator declared private, but not implemented */ \
  SingletonClass &operator=(const SingletonClass &src);               \
public:                                                               \
  static SingletonClass &getInstance() {                              \
    static SingletonClass instance;                                   \
    return instance;                                                  \
  }

