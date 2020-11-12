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
private:
  template<typename S> static void flushRange(String &str, const S &first, const S &last, AbstractStringifier<S> &sf, const TCHAR * &delim, const TCHAR *delimiter) {
    if(delim) {
      str += delim;
    } else {
      delim = delimiter;
    }
    if(last == first) {
      str += sf.toString(first);
    } else  {
      str += format(_T("%s-%s"), sf.toString(first).cstr(), sf.toString(last).cstr());
    }
  }

protected:
  AbstractIterator *m_it;
public:
  inline ConstIterator() {
    m_it = nullptr;
  }
  inline ConstIterator(AbstractIterator *it) {
    m_it = it; TRACE_NEW(it);
  }

  inline ConstIterator(const ConstIterator<T> &src) {
    m_it = src.m_it ? src.m_it->clone() : nullptr; TRACE_NEW(m_it);
  }

  ConstIterator<T> &operator=(const ConstIterator<T> &src) {
    if(this == &src) {
      return *this;
    }
    SAFEDELETE(m_it);
    m_it = src.m_it ? src.m_it->clone() : nullptr; TRACE_NEW(m_it);
    return *this;
  }

  virtual ~ConstIterator() {
    SAFEDELETE(m_it);
  }

  inline bool hasNext() const {
    assert(m_it != nullptr);
    return m_it->hasNext();
  }

  inline const T &next() {
    assert(m_it != nullptr);
    return *(const T *)m_it->next();
  }

  String toString(AbstractStringifier<T> &sf, const TCHAR *delimiter = _T(",")) const {
    String result;
    result += '(';
    if(hasNext()) {
      ConstIterator<T> it = *this;
      result += sf.toString(it.next());
      while(it.hasNext()) {
        result += delimiter;
        result += sf.toString(it.next());
      }
    }
    result += ')';
    return result;
  }

  String toString(const TCHAR *delimiter = _T(",")) const {
    std::wostringstream result;
    result << '(';
    if(hasNext()) {
      ConstIterator<T> it = *this;
      result << it.next();
      while(it.hasNext()) {
        result << delimiter << it.next();
      }
    }
    result << ')';
    return result.str().c_str();
  }

  // T must be enumerable (e1 + 1 == e2 must be defined for T e1,e2)
  String rangesToString(AbstractStringifier<T> &sf, const TCHAR *delimiter = _T(",")) const {
    String result;
    result += '[';
    if(hasNext()) {
      ConstIterator<T> it    = *this;
      const TCHAR     *delim = nullptr;
      T                first = it.next(), last = first;
      while(it.hasNext()) {
        const T &e = it.next();
        if(e == last+1) {
          last = e;
        } else {
          flushRange(result,first,last,sf, delim, delimiter);
          first = last = e;
        }
      }
      flushRange(result,first,last,sf, delim, delimiter);
    }
    result += ']';
    return result;
  }
};

template<typename T> class Iterator: public ConstIterator<T> {
public:
  inline Iterator() {
    m_it = nullptr;
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
    assert(m_it != nullptr);
    m_it->remove();
  }
};

template<template<typename...> class V,typename... Args> std::wostringstream &operator<<(std::wostringstream &out, const V<Args...> &v) {
  out << v.toString();
  return out;
}
