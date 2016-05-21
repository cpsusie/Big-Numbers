#pragma once

class AbstractIterator {
protected:
  void noNextElementError(         const TCHAR *className) const;
  void concurrentModificationError(const TCHAR *className) const;
  void noCurrentElementError(      const TCHAR *className) const;
  void unsupportedOperationError(  const TCHAR *className) const;

public:
  virtual AbstractIterator *clone() = 0;
  virtual ~AbstractIterator() {};
  virtual bool hasNext() const = 0;
  virtual void *next() = 0;
  virtual void remove() = 0;
};

template <class T> class Iterator {
protected:
  AbstractIterator *m_it;
public:
  Iterator(AbstractIterator *it) {
    m_it = it;
  }

  Iterator(const Iterator<T> &src) {
    m_it = src.m_it->clone();
  }

  Iterator<T> &operator=(const Iterator<T> &src) {
    if(this == &src) {
      return *this;
    }
    delete m_it;
    m_it = src.m_it->clone();
    return *this;
  }

  ~Iterator() {
    delete m_it;
  }

  inline bool hasNext() const {
    return m_it->hasNext();
  }

  inline T &next() {
    return *(T*)m_it->next();
  }

  inline void remove() {
    m_it->remove();
  }
};
