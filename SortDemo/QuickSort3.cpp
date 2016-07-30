#include "stdafx.h"
#include <stdlib.h>
#include <memory.h>

static void sort(void *base, size_t nelem, size_t width, AbstractComparator &comparator, char *pivot) {
  switch(nelem) {
  case 0:
  case 1:
    return; 
  case 2:
    SORT2(0, 1);
    return;
  case 3 : // sort the first, middle and last element. select pivot element as the middle
    SORT3OPT(0, 1, 2);
    return;
  default:
    SORT3OPT(0, nelem/2, nelem-1);
    break;
  }

  memcpy(pivot, EPTR(nelem/2), width);

// dont need to check first and last element against pivot element again
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
  size_t i = (pi - (char*)base) / width;

  if(pj > base       ) sort(base,(pj-(char*)base)/width+1, width, comparator, pivot);
  if(i < nelem-1) sort(pi  ,nelem-i                 , width, comparator, pivot);
}

static void quickSort3AnyWidth(void *base, size_t nelem, size_t width, AbstractComparator &comparator) {
  char buffer[100], *pivot = width <= sizeof(buffer) ? buffer : new char[width];
  try {
    sort(base,nelem,width,comparator,pivot);
    if(pivot!=buffer) delete[] pivot;
  } catch(...) {
    if(pivot!=buffer) delete[] pivot;
    throw;
  }
}

template <class T> class QuickSort3Class {
public:
  void sort(T *base, size_t nelem, AbstractComparator &comparator);
};

template <class T> void QuickSort3Class<T>::sort(T *base, size_t nelem, AbstractComparator &comparator) {
  switch(nelem) {
  case 0:
  case 1:
    return; 
  case 2:
    TSORT2(0, 1);
    return;
  case 3:  // sort the first, middle and last element. select pivot element as the middle
    TSORT3OPT(0, 1, 2);
    return;
  default:
    TSORT3OPT(0,nelem/2, nelem-1);
    break;
  }

  const T pivot = base[nelem/2];

// dont need to check first and last element against pivot element again
  T *pi = TEPTR(1), *pj = TEPTR(nelem-2);
  do {
    while(pi <= pj && PNEEDSWAP(&pivot, pi)) pi++;
    while(pi <= pj && PNEEDSWAP(pj, &pivot)) pj--;
    if(pi < pj) {
      TPSWAP(pi, pj);
    }
    if(pi <= pj) {
      pi++;
      pj--;
    }
  } while(pi <= pj);

  if(pj > base       ) sort(base,pj-base+1, comparator);
  const size_t i = pi - base;
  if(i < nelem-1) sort(pi  ,nelem-i  , comparator);
}

void quickSort3(void *base, size_t nelem, size_t width, AbstractComparator &comparator) {
  switch(width) {
  case sizeof(char)  :
    { QuickSort3Class<char>().sort((char*)base,nelem,comparator);
      break;
    }
  case sizeof(short) :
    { QuickSort3Class<short>().sort((short*)base,nelem,comparator);
      break;
    }
  case sizeof(long)  : // include pointertypes
    { QuickSort3Class<long>().sort((long*)base,nelem,comparator);
      break;
    }
  case sizeof(__int64):
    { QuickSort3Class<__int64>().sort((__int64*)base,nelem,comparator);
      break;
    }
  default            : // for all other values of width, we must use the hard way to copy and swap elements
    quickSort3AnyWidth(base,nelem,width,comparator);
    break;
  }
}
