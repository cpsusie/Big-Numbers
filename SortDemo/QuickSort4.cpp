#include "stdafx.h"

#pragma check_stack(off)

static void sort4AnyWidth(void *base, size_t nelem, size_t width, AbstractComparator &comparator, char *pivot) {
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
      SORT3OPT(0, 1, 2);
      continue;
    case 4:
      SORT4OPT(0, 1, 2, 3, continue);
      continue;
    default:
      SORT3OPT(0, nelem/2, nelem-1);
      memcpy(pivot, EPTR(nelem/2), width);
      break;
    }

    char *pi = EPTR(1), *pj = EPTR(nelem-2);
    do {
      while(pi <= pj && comparator.cmp(pi,pivot) < 0) pi += width; 
      while(pi <= pj && comparator.cmp(pivot,pj) < 0) pj -= width;
      if(pi < pj) {
        PSWAP(pi,pj);
      }
      if(pi <= pj) {
        pi += width;
        pj -= width;
      }
    } while(pi <= pj);
    const size_t i = (pi - (char*)base) / width;
    const size_t j = (pj - (char*)base) / width;
    if(j > nelem - i) {       // Sort the smallest partition first, to save stackspace
      if(j > 0) {
        PUSH(stack, base, j+1);      // Save start, count of elements to be sorted later. ie. Sort(base,j+1,width, comparator);
      }
      if(i < nelem-1) {       // Sort(pi, nelem-i, width, comparator);
        base  = pi;
        nelem -= i;
        goto tailrecurse;
      }
    } else {
      if(i < nelem-1) {       // Save start,count of elements to be sorted later. ie Sort(pi,nelem-i, width, comparator);
        PUSH(stack, pi, nelem-i); 
      }
      if(j > 0) {             // Sort(base,j+1,width, comparator);
        nelem = j+1;
        goto tailrecurse;
      }
    }


  }
}

void quickSort4AnyWidth(void *base, size_t nelem, size_t width, AbstractComparator &comparator) {
  char buffer[100], *pivot = width <= sizeof(buffer) ? buffer : new char[width];
  try {
    sort4AnyWidth(base,nelem,width,comparator,pivot);
    if(pivot!=buffer) delete[] pivot;
  } catch(...) {
    if(pivot!=buffer) delete[] pivot;
    throw;
  }
}

template <class T> class QuickSort4Class {
  public:
    void sort(T *base, size_t nelem, AbstractComparator &comparator);
};

template <class T> void QuickSort4Class<T>::sort(T *base, size_t nelem, AbstractComparator &comparator) {
  DECLARE_STACK(stack, 80);
  PUSH(stack, base, nelem);

  while(!ISEMPTY(stack)) {
    POP(stack, base, nelem, T);
tailrecurse:
    switch( nelem ) {
    case 0:
    case 1:
      continue;
    case 2:
      TSORT2(0, 1);
      continue;
    case 3:
      TSORT3OPT(0,1,2);
      continue;
    case 4:
      TSORT4OPT(0, 1, 2, 3, continue);
      continue;
    default:
      TSORT3OPT(0, nelem/2, nelem-1);
      break;
    }

    const T pivot = base[nelem/2];
    T *pi = TEPTR(1), *pj = TEPTR(nelem-2);
    do {
      while(pi <= pj && comparator.cmp(pi, &pivot) < 0) pi++;  // while e[i]  < pivot
      while(pi <= pj && comparator.cmp(&pivot, pj) < 0) pj--;  // while pivot < e[j]
      if(pi < pj) {
        TPSWAP(pi, pj);
      }
      if(pi <= pj) {
        pi++;
        pj--;
      }
    } while(pi <= pj);
    const size_t i = pi - base;
    const size_t j = pj - base;
    if(j > nelem - i) {       // Sort the smallest partition first, to save stackspace
      if(j > 0) {
        PUSH(stack, base, j+1);      // Save start, count of elements to be sorted later. ie. Sort(base,j+1,width, comparator);
      }
      if(i < nelem-1) {       // Sort(pi, nelem-i, width, comparator);
        base  = pi;
        nelem -= i;
        goto tailrecurse;
      }
    } else {
      if(i < nelem-1) {       // Save start,count of elements to be sorted later. ie Sort(pi,nelem-i, width, comparator);
        PUSH(stack, pi, nelem-i); 
      }
      if(j > 0) {             // Sort(base,j+1,width, comparator);
        nelem = j+1;
        goto tailrecurse;
      }
    }
  }
}

void quickSort4(void *base, size_t nelem, size_t width, AbstractComparator &comparator) {
  switch(width) {
  case sizeof(char)  :
    { QuickSort4Class<char>().sort((char*)base, nelem, comparator);
      break;
    }
  case sizeof(short) :
    { QuickSort4Class<short>().sort((short*)base, nelem, comparator);
      break;
    }
  case sizeof(long)  : // include pointertypes
    { QuickSort4Class<long>().sort((long*)base, nelem, comparator);
      break;
    }
  case sizeof(__int64):
    { QuickSort4Class<__int64>().sort((__int64*)base, nelem, comparator);
      break;
    }
  default            : // for all other values of width, we must use the hard way to copy and swap elements
    quickSort4AnyWidth(base, nelem, width, comparator);
    break;
  }
}
