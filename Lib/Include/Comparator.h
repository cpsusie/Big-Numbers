#pragma once

class AbstractComparator {
public:
  virtual int cmp(const void *, const void *) = 0;
  virtual AbstractComparator *clone() const = 0;
  virtual ~AbstractComparator() {}
};

class AbstractSelector {
public:
  virtual bool select(const void *) = 0;
  virtual AbstractSelector *clone() const = 0;
  virtual ~AbstractSelector() {}
};

template <class T> class Comparator : public AbstractComparator {
private:
  inline int cmp(const void *e1, const void *e2) {
    return compare(*(const T*)e1, *(const T*)e2);
  }
public:
  virtual int compare(const T &e1, const T &e2) = 0;
};

template <class T> class Selector : public AbstractSelector {
private:
  inline bool select(const void *e) {
    return select(*(const T*)e);
  }
public:
  virtual bool select(const T &element) = 0;
};


template <class T> class FunctionComparator : public Comparator<T> {
private:
  typedef int (*CompareFunction)(const T*, const T*);
protected:
  CompareFunction m_usersuppliedcmp;
public:
  FunctionComparator() {
    m_usersuppliedcmp = NULL;
  }
  FunctionComparator(int (*cmp)(const T *e1, const T *e2)) {
    m_usersuppliedcmp = (CompareFunction)cmp;
  }
  FunctionComparator(int (*cmp)(const T &e1, const T &e2)) {
    m_usersuppliedcmp = (CompareFunction)cmp;
  }
  FunctionComparator(const FunctionComparator<T> &src) {
    m_usersuppliedcmp = src.m_usersuppliedcmp;
  }
  int compare(const T &e1, const T &e2) {
    return m_usersuppliedcmp(&e1, &e2);
  }
  inline bool isSet() const {
    return m_usersuppliedcmp != NULL;
  }
  AbstractComparator *clone() const {
    return new FunctionComparator<T>(*this);
  }
};

void quickSort(void *base, size_t nelem, size_t width, AbstractComparator &comparator);
void quickSort(void *base, size_t nelem, size_t width, int(__cdecl *compare)(const void*, const void*));
