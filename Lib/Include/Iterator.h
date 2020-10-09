#pragma once

#include "BasicIncludes.h"

class AbstractIterator {
protected:
  void noNextElementError(         const TCHAR *className) const;
  void concurrentModificationError(const TCHAR *className) const;
  void noCurrentElementError(      const TCHAR *className) const;
  void unsupportedOperationError(  const TCHAR *method   ) const;

public:
  virtual ~AbstractIterator() {};
  virtual AbstractIterator *clone()         = 0;
  virtual bool              hasNext() const = 0;
  virtual void             *next()          = 0;
  virtual void              remove()        = 0;
};

template<typename T> class ConstIterator {
protected:
  AbstractIterator *m_it;
public:
  inline ConstIterator() {
    m_it = NULL;
  }
  inline ConstIterator(AbstractIterator *it) {
    m_it = it; TRACE_NEW(it);
  }

  inline ConstIterator(const ConstIterator<T> &src) {
    m_it = src.m_it ? src.m_it->clone() : NULL; TRACE_NEW(m_it);
  }

  ConstIterator<T> &operator=(const ConstIterator<T> &src) {
    if(this == &src) {
      return *this;
    }
    SAFEDELETE(m_it);
    m_it = src.m_it ? src.m_it->clone() : NULL; TRACE_NEW(m_it);
    return *this;
  }

  virtual ~ConstIterator() {
    SAFEDELETE(m_it);
  }

  inline bool hasNext() const {
    assert(m_it != NULL);
    return m_it->hasNext();
  }

  inline const T &next() {
    assert(m_it != NULL);
    return *(const T *)m_it->next();
  }

  String toString(AbstractStringifier<T> &sf, const TCHAR *delimiter = _T(",")) const {
    String result = "(";
    if(hasNext()) {
      ConstIterator<T> t = *this;
      result += sf.toString(t.next());
      while(t.hasNext()) {
        result += delimiter;
        result += sf.toString(t.next());
      }
    }
    result += ")";
    return result;
  }
  String toString(const TCHAR *delimiter = _T(",")) const {
    std::wostringstream result;
    result << "(";
    if(hasNext()) {
      ConstIterator<T> t = *this;
      result << t.next();
      while(t.hasNext()) {
        result << delimiter << t.next();
      }
    }
    result << ")";
    return result.str().c_str();
  }

  // T must be enumerable (e1 + 1 == e2 must be defined for T e1,e2)
  String rangesToString(AbstractStringifier<T> &sf, const TCHAR *delimiter = _T(",")) const {

#if defined(_FLUSHRANGE)
#undef _FLUSHRANGE
#endif

#define _FLUSHRANGE()                                                                     \
    { if(delim) result += delim; else delim = delimiter;                                  \
      if(first == last) {                                                                 \
        result += sf.toString(first);                                                     \
      } else {                                                                            \
        const TCHAR *formatStr = (first + 1 == last) ? _T("%s%s") : _T("%s-%s");          \
        result += format(formatStr, sf.toString(first).cstr(), sf.toString(last).cstr()); \
      }                                                                                   \
    }

    String           result    = _T("[");
    const TCHAR     *delim     = NULL;
    bool             firstTime = true;
    ConstIterator<T> t         = *this;
    T                first, last;

    while(t.hasNext()) {
      const T &e = t.next();
      if(firstTime) {
        first = last = e;
        firstTime = false;
      } else if(e == last+1) {
        last = e;
      } else {
        _FLUSHRANGE();
        first = last = e;
      }
    }
    if(!firstTime) {
      _FLUSHRANGE();
    }
    result += _T("]");
    return result;
  }
#undef _FLUSHRANGE
};

template<typename T> class Iterator: public ConstIterator<T> {
public:
  inline Iterator() {
    m_it = NULL;
  }
  inline Iterator(AbstractIterator *it) : ConstIterator<T>(it) {
  }
  inline Iterator(const Iterator<T> &src) : ConstIterator<T>(src) {
  }
  inline Iterator<T> &operator=(const Iterator<T> &src) {
    __super::operator=(src);
    return *this;
  }
  inline T &next() {
    return (T &)__super::next();
  }
  inline void remove() {
    assert(m_it != NULL);
    m_it->remove();
  }
};

template<template<typename...> class V,typename... Args> std::wostringstream &operator<<(std::wostringstream &out, const V<Args...> &v) {
  out << v.toString();
  return out;
}
