#include "stdafx.h"

static void shakerSortAnyWidth(void *base, size_t nelem, size_t width, AbstractComparator &comparator) {
  char *p1, *p2;
  for(intptr_t l = 0, r = nelem - 1; l < r; l++, r--) {
    intptr_t k;
    bool changed = false;
    for(k = l, p1 = EPTR(k), p2 = p1 + width; k++ < r; p1 += width, p2 += width) {
      if(PNEEDSWAP(p1, p2)) {
        PSWAP(p1, p2);
        changed = true;
      }
    }
    for(k = r, p1 = EPTR(k), p2 = p1 - width; k-- > l; p1 -= width, p2 -= width) {
      if(PNEEDSWAP(p2, p1)) {
        PSWAP(p1, p2);
        changed = true;
      }
    }
    if(!changed) {
      break;
    }
  }
}

template <class T> class ShakerSortClass {
public:
  void sort(T *base, size_t nelem, AbstractComparator &comparator);
};

template <class T> void ShakerSortClass<T>::sort(T *base, size_t nelem, AbstractComparator &comparator) {
  T *p1, *p2;
  for(intptr_t l = 0, r = nelem - 1; l < r; l++, r--) {
    intptr_t k;
    bool changed = false;
    for(k = l, p1 = base + k, p2 = p1 + 1; k++ < r; p1++, p2++) {
      if(PNEEDSWAP(p1, p2)) {
        TPSWAP(p1, p2);
        changed = true;
      }
    }
    for(k = r, p1 = base + k, p2 = p1 - 1; k-- > l; p1--, p2--) {
      if(PNEEDSWAP(p2, p1)) {
        TPSWAP(p1, p2);
        changed = true;
      }
    }
    if(!changed) {
      break;
    }
  }
}

void shakerSort(void *base, size_t nelem, size_t width, AbstractComparator &comparator) {
  switch(width) {
  case sizeof(char)  :
    { ShakerSortClass<char>().sort((char*)base,nelem,comparator);
      break;
    }
  case sizeof(short) :
    { ShakerSortClass<short>().sort((short*)base,nelem,comparator);
      break;
    }
  case sizeof(long)  : // include pointertypes
    { ShakerSortClass<long>().sort((long*)base,nelem,comparator);
      break;
    }
  case sizeof(__int64):
    { ShakerSortClass<__int64>().sort((__int64*)base,nelem,comparator);
      break;
    }
  default            : // for all other values of width, we must use the hard way to copy and swap elements
    shakerSortAnyWidth(base,nelem,width,comparator);
    break;
  }
}
