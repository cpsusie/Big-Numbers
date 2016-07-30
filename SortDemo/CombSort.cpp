#include "stdafx.h"

#define SHRINKFACTOR 1.3

static void combSortAnyWidth(void *base, size_t nelem, size_t width, AbstractComparator &comparator) {
  bool changed;
  size_t gap = nelem;

  do {
    gap = (int)((float)gap/SHRINKFACTOR);
    switch(gap) {
    case 0 : gap =  1; break;
    case 9 :
    case 10: gap = 11; break;
    }
    changed = false;
    char *p1 = EPTR(0), *p2 = EPTR(gap);
    for(size_t i = gap; i < nelem; i++, p1 += width, p2 += width) {
      if(PNEEDSWAP(p1, p2)) {
        PSWAP(p1, p2);
        changed = true;
      }
    }
  } while(changed || (gap > 1));
}

template <class T> class CombSortClass {
public:
  void sort(T *base, size_t nelem, AbstractComparator &comparator);
};

template <class T> void CombSortClass<T>::sort(T *base, size_t nelem, AbstractComparator &comparator) {
  bool changed;
  size_t gap = nelem;

  do {
    gap = (int)((float)gap/SHRINKFACTOR);
    switch(gap) {
    case 0 : gap =  1; break;
    case 9 :
    case 10: gap = 11; break;
    }
    changed = false;
    T *p1 = base, *p2 = base+gap;;
    for(size_t i = nelem-gap; i--; p1++, p2++) {
      if(PNEEDSWAP(p1, p2)) {
        TPSWAP(p1, p2);
        changed = true;
      }
    }
  } while(changed || (gap > 1));
}

void combSort(void *base, size_t nelem, size_t width, AbstractComparator &comparator) {
  switch(width) {
  case sizeof(char)  :
    { CombSortClass<char>().sort((char*)base,nelem,comparator);
      break;
    }
  case sizeof(short) :
    { CombSortClass<short>().sort((short*)base,nelem,comparator);
      break;
    }
  case sizeof(long)  : // include pointertypes
    { CombSortClass<long>().sort((long*)base,nelem,comparator);
      break;
    }
  case sizeof(__int64):
    { CombSortClass<__int64>().sort((__int64*)base,nelem,comparator);
      break;
    }
  default            : // for all other values of width, we must use the hard way to copy and swap elements
    combSortAnyWidth(base,nelem,width,comparator);
    break;
  }
}
