#include "stdafx.h"

void shellSortSlow(void *base, size_t nelem, size_t width, AbstractComparator &comparator) {
  bool stable;

  size_t gap = nelem / 2;
  if((gap & 1) == 0) {
    gap--;
  }
  do {
    do {
      stable = true;
      int i;
      char *p1, *p2;
      for(i=0, p1 = EPTR(i), p2 = EPTR(gap); i < (int)(nelem - gap); i++, p1 += width, p2 += width)
        if(PNEEDSWAP(p1, p2)) {
          PSWAP(p1, p2);
          stable = false;
        }
    } while(!stable);
    gap /= 2;
    if((gap & 1) == 0) {
      gap--;
    }
  } while(gap >= 1);
}
