#include "stdafx.h"

#define SHRINKFACTOR 1.3

static void combSort11AnyWidth(void *base, size_t nelem, size_t width, AbstractComparator &comparator) {
  bool        changed;
  size_t      gap  = nelem;
  const char *endp = EPTR(nelem);
  do {
    gap = (size_t)((double)gap/SHRINKFACTOR);
    switch(gap) {
    case 0 : gap =  1; break;
    case 9 :
    case 10: gap = 11; break;
    }
    changed = false;
    for(char *p1 = EPTR(0), *p2 = EPTR(gap); p2 < endp; p1 += width, p2 += width) {
      if(PNEEDSWAP(p1, p2)) {
        PSWAP(p1, p2);
        changed = true;
      }
    }
  } while(changed || (gap > 1));
}

template <class T> class CombSort11Class {
public:
  void sort(T *base, size_t nelem, AbstractComparator &comparator);
};

template <class T> void CombSort11Class<T>::sort(T *base, size_t nelem, AbstractComparator &comparator) {
  bool     changed;
  size_t   gap  = nelem;
  const T *endp = base + nelem;
  do {
    gap = (size_t)((double)gap/SHRINKFACTOR);
    switch(gap) {
    case 0 : gap =  1; break;
    case 9 :
    case 10: gap = 11; break;
    }
    changed = false;
    for(T *p1 = base, *p2 = base+gap; p2 < endp; p1++, p2++) {
      if(PNEEDSWAP(p1, p2)) {
        TPSWAP(p1, p2);
        changed = true;
      }
    }
  } while(changed || (gap > 1));
}

void combSort11(void *base, size_t nelem, size_t width, AbstractComparator &comparator) {
  switch(width) {
  case sizeof(char)  :
    { CombSort11Class<char>().sort((char*)base, nelem, comparator);
      break;
    }
  case sizeof(short) :
    { CombSort11Class<short>().sort((short*)base, nelem, comparator);
      break;
    }
  case sizeof(long)  : // include pointertypes
    { CombSort11Class<long>().sort((long*)base, nelem, comparator);
      break;
    }
  case sizeof(__int64):
    { CombSort11Class<__int64>().sort((__int64*)base, nelem, comparator);
      break;
    }
  default            : // for all other values of width, we must use the hard way to copy and swap elements
    combSort11AnyWidth(base, nelem, width, comparator);
    break;
  }
}
