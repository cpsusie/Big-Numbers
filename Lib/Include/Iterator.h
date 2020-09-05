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
  virtual AbstractIterator *clone() = 0;
  virtual bool  hasNext() const     = 0;
  virtual void *next()              = 0;
  virtual void  remove()            = 0;
};

template <typename T> class Iterator {
protected:
  AbstractIterator *m_it;
public:
  Iterator() {
    m_it = NULL;
  }
  Iterator(AbstractIterator *it) {
    m_it = it; TRACE_NEW(it);
  }

  Iterator(const Iterator<T> &src) {
    m_it = src.m_it ? src.m_it->clone() : NULL; TRACE_NEW(m_it);
  }

  Iterator<T> &operator=(const Iterator<T> &src) {
    if(this == &src) {
      return *this;
    }
    SAFEDELETE(m_it);
    m_it = src.m_it ? src.m_it->clone() : NULL; TRACE_NEW(m_it);
    return *this;
  }

  ~Iterator() {
    SAFEDELETE(m_it);
  }

  inline bool hasNext() const {
    assert(m_it != NULL);
    return m_it->hasNext();
  }

  inline T &next() {
    assert(m_it != NULL);
    return *(T*)m_it->next();
  }

  inline void remove() {
    assert(m_it != NULL);
    m_it->remove();
  }

  String toString(AbstractStringifier<T> &sf, const TCHAR *delimiter = _T(",")) {
    String result;
    if(hasNext()) {
      result += sf.toString(next());
      while(hasNext()) {
        result += delimiter;
        result += sf.toString(next());
      }
    }
    return result;
  }

  String toString(const TCHAR *delimiter = _T(",")) {
    String result;
    if(hasNext()) {
      result += sf.toString(next());
      while(hasNext()) {
        result += delimiter;
        result += toString(next());
      }
    }
    return result;
  }

  // T must be enumerable (e1 + 1 == e2 must be defined for T e1,e2)
  String rangesToString(AbstractStringifier<T> &sf, const TCHAR *delimiter = _T(",")) {

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

    String       result    = _T("[");
    const TCHAR *delim     = NULL;
    bool         firstTime = true;
    T            first, last;
    while(hasNext()) {
      const T e = next();
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
