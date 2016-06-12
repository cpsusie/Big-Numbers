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

intptr_t ArrayImpl::getSortCount(intptr_t from, intptr_t count) const {
  const intptr_t length = (intptr_t)size() - from;
  return min(count, length);
}

void ArrayImpl::shuffle(size_t from, size_t count) {
  if(from >= size()) {
    return;
  }
  count = getSortCount(from, count);
  for(size_t i = from; i < count; i++ ) {
    swap(i, from + randInt() % count);
  }
}

void ArrayImpl::arraySort(size_t from, size_t count, int (*compare)(const void *e1, const void *e2)) {
  sort(from, count, ArrayImplComparator1(compare));
}

void ArrayImpl::arraySort(size_t from, size_t count, int (*compare)(const void **e1, const void **e2)) {
  sort(from, count, ArrayImplComparator0(compare));
}

void ArrayImpl::arraySort(size_t from, size_t count, AbstractComparator &comparator) {
  sort(from, count, ArrayImplComparator(comparator));
}

void ArrayImpl::sort(size_t from, size_t count, AbstractComparator &comparator) {
  if(from >= size()) {
    return;
  } else if(from < 0) {
    from = 0;
  }
  count = getSortCount(from, count);
  ::quickSort(m_elem+from, count, sizeof(void**), comparator);
}

/* --------------------------------------------------------- */
intptr_t ArrayImpl::lSearch(const void *key, size_t from, size_t count, int (*compare)(const void *e1, const void *e2)) const {
  return linearSearch(key, from, count, ArrayImplComparator1(compare));
}

intptr_t ArrayImpl::lSearch(const void *key, size_t from, size_t count, int (*compare)(const void **e1, const void **e2)) const {
  return linearSearch(key, from, count, ArrayImplComparator0(compare));
}

intptr_t ArrayImpl::lSearch(const void *key, size_t from, size_t count, AbstractComparator &comparator) const {
  return linearSearch(key, from, count, ArrayImplComparator(comparator));
}

intptr_t ArrayImpl::linearSearch(const void *key, size_t from, size_t count, AbstractComparator &comparator) const {
  if(from >= size()) {
    return -1;
  } else if(from < 0) {
    from = 0;
  }
  const size_t to = from + getSortCount(from, count);
  for(size_t i = from; i < to; i++) {
    if(comparator.cmp(&key, m_elem+i) == 0) {
      return i;
    }
  }
  return -1;
}

/* --------------------------------------------------------- */

intptr_t ArrayImpl::bSearch(const void *key, size_t from, size_t count, int (*compare)(const void *e1, const void *e2)) const {
  return binarySearch(key, from, count, ArrayImplComparator1(compare));
}

intptr_t ArrayImpl::bSearch(const void *key, size_t from, size_t count, int (*compare)(const void **e1, const void **e2)) const {
  return binarySearch(key, from, count, ArrayImplComparator0(compare));
}

intptr_t ArrayImpl::bSearch(const void *key, size_t from, size_t count, AbstractComparator &comparator) const {
  return binarySearch(key, from, count, ArrayImplComparator(comparator));
}

intptr_t ArrayImpl::binarySearch(const void *key, intptr_t from, size_t count, AbstractComparator &comparator) const {
  if(from >= (intptr_t)size()) {
    return -1;
  } else if(from < 0) {
    from = 0;
  }
  intptr_t to = from + getSortCount(from, count) - 1;
  while(from <= to) {
    const intptr_t  m = (from+to) / 2;
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

intptr_t ArrayImpl::bInsert(const void *e, int (*compare)(const void  *e1, const void  *e2)) {
  return binaryInsert(e, ArrayImplComparator1(compare));
}

intptr_t ArrayImpl::bInsert(const void *e, int (*compare)(const void **e1, const void **e2)) {
  return binaryInsert(e, ArrayImplComparator0(compare));
}

intptr_t ArrayImpl::bInsert(const void *e, AbstractComparator &comparator) {
  return binaryInsert(e, ArrayImplComparator(comparator));
}

intptr_t ArrayImpl::binaryInsert(const void *e, AbstractComparator &comparator) {
  intptr_t l = 0, r = size();
  while(l < r) {
    const intptr_t m = (l+r)/2;
    if(comparator.cmp(&e, m_elem+m) > 0) {
      l = m + 1;
    } else {
      r = m;
    }
  }
  add(r, e, 1);
  return r;
}
