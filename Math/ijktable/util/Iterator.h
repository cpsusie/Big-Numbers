#pragma once

#include "util.h"

class AbstractIterator {
protected:
  void noNextElementError(         const char *className) const;
  void concurrentModificationError(const char *className) const;
  void noCurrentElementError(      const char *className) const;
  void unsupportedOperationError(  const char *method   ) const;

public:
  virtual AbstractIterator *clone() = 0;
  virtual ~AbstractIterator() {};
  virtual bool hasNext() const = 0;
  virtual void *next() = 0;
  virtual void remove() = 0;
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
    Assert(m_it != NULL);
    return m_it->hasNext();
  }

  inline T &next() {
    Assert(m_it != NULL);
    return *(T*)m_it->next();
  }

  inline void remove() {
    Assert(m_it != NULL);
    m_it->remove();
  }

  string toString(AbstractStringifier<T> &sf, const char *delimiter = ",") {
    const char *delim = NULL;
    string result;
    while(hasNext()) {
      if(delim) result += delim; else delim = delimiter;
      result += sf.toString(next());
    }
    return result;
  }

  // T must be enumerable (e1 + 1 == e2 must be defined for T e1,e2)
  string rangesToString(AbstractStringifier<T> &sf, const char *delimiter = ",") {

#define _FLUSHRANGE()                                                                       \
    { if(delim) result += delim; else delim = delimiter;                                    \
      if(first == last) {                                                                   \
        result += sf.toString(first);                                                       \
      } else {                                                                              \
        const char *formatStr = (first + 1 == last) ? "%s%s" : "%s-%s";                     \
        result += format(formatStr, sf.toString(first).c_str(), sf.toString(last).c_str()); \
      }                                                                                     \
    }

    string       result    = "[";
    const char  *delim     = NULL;
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
    result += "]";
    return result;
  }
};
