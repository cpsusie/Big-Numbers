#include "stdafx.h"

static void insertionSortAnyWidth(void *base, size_t nelem, size_t width, AbstractComparator &comparator) {
  char *tmp = new char[width];
  char *pi = EPTR(1);
  try { 
    for(size_t i = 1; i < nelem; i++, pi += width) {
      size_t l,r;
      for(l = 0, r = i; l < r;) {
        size_t m = ( l + r ) / 2;
        if(comparator.cmp(EPTR(m), pi) <= 0) {
          l = m+1;
        } else {
          r = m;
        }
      }
      memcpy(tmp        , pi, width);
      char *pr = EPTR(r);
      memmove(pr + width, pr, (i - r) * width);
      memcpy( pr        , tmp, width);
    }
    delete[] tmp;
  } catch(...) {
    delete[] tmp;
    throw;
  }
}

template <class T> class InsertionSortClass {
public:
  void sort(T *base, size_t nelem, AbstractComparator &comparator);
};

template <class T> void InsertionSortClass<T>::sort(T *base, size_t nelem, AbstractComparator &comparator) {
  T *pi = base + 1;
  for(size_t i = 1; i < nelem; i++, pi++) {
    size_t l,r;
    for(l = 0, r = i; l < r;) {
      size_t m = (l + r) / 2;
      if(comparator.cmp(base+m, pi) <= 0) {
        l = m+1;
      } else {
        r = m;
      }
    }
    T tmp = *pi;
    T *pr = base + r;
    memmove(pr + 1, pr, (i - r) * sizeof(T));
    *pr = tmp;
  }
}

void insertionSort(void *base, size_t nelem, size_t width, AbstractComparator &comparator) {
  switch(width) {
  case sizeof(char)  :
    { InsertionSortClass<char>().sort((char*)base,nelem,comparator);
      break;
    }
  case sizeof(short) :
    { InsertionSortClass<short>().sort((short*)base,nelem,comparator);
      break;
    }
  case sizeof(long)  : // include pointertypes
    { InsertionSortClass<long>().sort((long*)base,nelem,comparator);
      break;
    }
  case sizeof(__int64):
    { InsertionSortClass<__int64>().sort((__int64*)base,nelem,comparator);
      break;
    }
  default            : // for all other values of width, we must use the hard way to copy and swap elements
    insertionSortAnyWidth(base,nelem,width,comparator);
    break;
  }
}
