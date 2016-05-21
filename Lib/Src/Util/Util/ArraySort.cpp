#include "pch.h"
#include <Random.h>

#pragma check_stack(off)

class ArrayImplComparator1 : public AbstractComparator { // derefer 1
private:
  int (*m_userSuppliedCmp)(const void *e1, const void *e2);
public:
  ArrayImplComparator1(int (*cmp)(const void *e1, const void *e2)) {
    m_userSuppliedCmp = cmp;
  }
  int cmp(const void *e1, const void *e2) {
    return m_userSuppliedCmp(*(const void**)e1, *(const void**)e2);
  }
  AbstractComparator *clone() const {
	return new ArrayImplComparator1(m_userSuppliedCmp);
  }
};

class ArrayImplComparator0 : public AbstractComparator { // derefer 0, ie call directly
private:
  int (*m_userSuppliedCmp)(const void **e1, const void **e2);
public:
  ArrayImplComparator0(int (*cmp)(const void **e1, const void **e2)) {
    m_userSuppliedCmp = cmp;
  }
  int cmp(const void *e1, const void *e2) {
    return m_userSuppliedCmp((const void **)e1, (const void **)e2);
  }
  AbstractComparator *clone() const {
	return new ArrayImplComparator0(m_userSuppliedCmp);
  }
};

class ArrayImplComparator : public AbstractComparator { // derefer 1
private:
  AbstractComparator &m_userSuppliedComparator;
public:
  ArrayImplComparator(AbstractComparator &comparator) : m_userSuppliedComparator(comparator) {
  }
  int cmp(const void *e1, const void *e2) {
    return m_userSuppliedComparator.cmp(*((const void**)e1), *((const void**)e2));
  }
  AbstractComparator *clone() const {
	  return new ArrayImplComparator(m_userSuppliedComparator);
  }
};

int ArrayImpl::getSortCount(int from, int count) const {
  const int length = size() - from;
  return min(count, length);
}

void ArrayImpl::shuffle(int from, int count) {
  if(from >= size()) {
    return;
  } else if(from < 0) {
    from = 0;
  }
  count = getSortCount(from, count);
  for(int i = from; i < count; i++ ) {
    swap(i, from + randInt() % count);
  }
}

void ArrayImpl::arraySort(int from, int count, int (*compare)(const void *e1, const void *e2)) {
  sort(from, count, ArrayImplComparator1(compare));
}

void ArrayImpl::arraySort(int from, int count, int (*compare)(const void **e1, const void **e2)) {
  sort(from, count, ArrayImplComparator0(compare));
}

void ArrayImpl::arraySort(int from, int count, AbstractComparator &comparator) {
  sort(from, count, ArrayImplComparator(comparator));
}

void ArrayImpl::sort(int from, int count, AbstractComparator &comparator) {
  if(from >= size()) {
    return;
  } else if(from < 0) {
    from = 0;
  }
  count = getSortCount(from, count);
  ::quickSort(m_elem+from, count, sizeof(void**), comparator);
}

/* --------------------------------------------------------- */
int ArrayImpl::lSearch(const void *key, int from, int count, int (*compare)(const void *e1, const void *e2)) const {
  return linearSearch(key, from, count, ArrayImplComparator1(compare));
}

int ArrayImpl::lSearch(const void *key, int from, int count, int (*compare)(const void **e1, const void **e2)) const {
  return linearSearch(key, from, count, ArrayImplComparator0(compare));
}

int ArrayImpl::lSearch(const void *key, int from, int count, AbstractComparator &comparator) const {
  return linearSearch(key, from, count, ArrayImplComparator(comparator));
}

int ArrayImpl::linearSearch(const void *key, int from, int count, AbstractComparator &comparator) const {
  if(from >= size()) {
    return -1;
  } else if(from < 0) {
    from = 0;
  }
  const int to = from + getSortCount(from, count);
  for(int i = from; i < to; i++) {
    if(comparator.cmp(&key, m_elem+i) == 0) {
      return i;
    }
  }
  return -1;
}

/* --------------------------------------------------------- */

int ArrayImpl::bSearch(const void *key, int from, int count, int (*compare)(const void *e1, const void *e2)) const {
  return binarySearch(key, from, count, ArrayImplComparator1(compare));
}

int ArrayImpl::bSearch(const void *key, int from, int count, int (*compare)(const void **e1, const void **e2)) const {
  return binarySearch(key, from, count, ArrayImplComparator0(compare));
}

int ArrayImpl::bSearch(const void *key, int from, int count, AbstractComparator &comparator) const {
  return binarySearch(key, from, count, ArrayImplComparator(comparator));
}

int ArrayImpl::binarySearch(const void *key, int from, int count, AbstractComparator &comparator) const {
  if(from >= size()) {
    return -1;
  } else if(from < 0) {
    from = 0;
  }
  int to = from + getSortCount(from, count) - 1;
  while(from <= to) {
    const int m = (from+to) / 2;
    const int c = comparator.cmp(&key, m_elem+m);
    if(c > 0) {
      from = m + 1;
    } else if(c < 0) {
      to = m - 1;
    } else {
      return m;
    }
  }
  return -1;
}

/* --------------------------------------------------------- */

int ArrayImpl::bInsert(const void *e, int (*compare)(const void  *e1, const void  *e2)) {
  return binaryInsert(e, ArrayImplComparator1(compare));
}

int ArrayImpl::bInsert(const void *e, int (*compare)(const void **e1, const void **e2)) {
  return binaryInsert(e, ArrayImplComparator0(compare));
}

int ArrayImpl::bInsert(const void *e, AbstractComparator &comparator) {
  return binaryInsert(e, ArrayImplComparator(comparator));
}

int ArrayImpl::binaryInsert(const void *e, AbstractComparator &comparator) {
  int l = 0, r = size();
  while(l < r) {
    const int m = (l+r)/2;
    if(comparator.cmp(&e, m_elem+m) > 0) {
      l = m + 1;
    } else {
      r = m;
    }
  }
  add(r, e, 1);
  return r;
}
