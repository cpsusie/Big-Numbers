#include "stdafx.h"
#include <stdlib.h>
#include <memory.h>

static void sort(void *base, size_t nelem, size_t width, AbstractComparator &comparator, char *pivot) {
  DECLARE_STACK(stack, 80);
  PUSH(stack, base, nelem);

  while(!ISEMPTY(stack)) {
    POP(stack, base, nelem, void);
tailrecurse:
    switch(nelem) {
    case 0:
    case 1:
      continue;

    case 2:
      SORT2(0, 1);
      continue;

    case 3:
      SORT3OPT(0,1,2);
      continue;

    default:
      SORT3OPT(0, nelem/2, nelem-1);
      PASSIGN(pivot, EPTR(nelem/2));
      break;
    }

    char *pi = EPTR(1), *pj = EPTR(nelem-2);
    do {
      while(pi <= pj && PNEEDSWAP(pivot, pi)) pi += width; 
      while(pi <= pj && PNEEDSWAP(pj, pivot)) pj -= width;
      if(pi < pj) {
        PSWAP(pi, pj);
      }
      if(pi <= pj) {
        pi += width;
        pj -= width;
      }
    } while(pi <= pj);
    if(pj > base) {
      PUSH(stack, base,(pj - (char*)base) / width+1);        // actually sort(base,j+1,width, compare,pivot);
    }
    const size_t i = (pi - (char*)base) / width;
    if(i < nelem-1) {   // actually sort(EPTR(i),nelem-i, width, compare,pivot);
      base  = pi;
      nelem -= i;
      goto tailrecurse;
    }
  }
}

void quickSort3NoRecursion(void *base, size_t nelem, size_t width, AbstractComparator &comparator) {
  char buffer[100], *pivot = width <= sizeof(buffer) ? buffer : new char[width];
  try {
    sort(base,nelem,width,comparator,pivot);
    if(pivot!=buffer) delete[] pivot;
  } catch(...) {
    if(pivot!=buffer) delete[] pivot;
    throw;
  }
}
