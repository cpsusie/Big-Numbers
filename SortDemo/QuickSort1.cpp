#include "stdafx.h"

static void sort(void *base, size_t nelem, size_t width, AbstractComparator &comparator, char *pivot) {
  memcpy(pivot, EPTR(nelem / 2), width);

  char *pi = EPTR(0); 
  char *pj = EPTR(nelem-1);
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

  if(pj>base         ) sort(base,(pj-(char*)base)/width+1, width, comparator, pivot);
  size_t i = (pi - (char*)base)/width;
  if(i < nelem-1) sort(pi  ,nelem-i, width, comparator, pivot);
}

static void quickSort1AnyWidth(void *base, size_t nelem, size_t width, AbstractComparator &comparator) {
  char buffer[100], *pivot = width <= sizeof(buffer) ? buffer : new char[width];
  try {
    sort(base,nelem,width,comparator,pivot);
    if(pivot!=buffer) delete[] pivot;
  } catch(...) {
    if(pivot!=buffer) delete[] pivot;
    throw;
  }
}

template <class T> class QuickSort1Class {
public:
  void sort(T *base, size_t nelem, AbstractComparator &comparator);
};

template <class T> void QuickSort1Class<T>::sort(T *base, size_t nelem, AbstractComparator &comparator) {
  const T pivot = base[nelem / 2];

  T *pi = base; 
  T *pj = base + nelem-1;
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

void quickSort1(void *base, size_t nelem, size_t width, AbstractComparator &comparator) {
  switch(width) {
  case sizeof(char)  :
    { QuickSort1Class<char>().sort((char*)base,nelem,comparator);
      break;
    }
  case sizeof(short) :
    { QuickSort1Class<short>().sort((short*)base,nelem,comparator);
      break;
    }
  case sizeof(long)  : // include pointertypes
    { QuickSort1Class<long>().sort((long*)base,nelem,comparator);
      break;
    }
  case sizeof(__int64):
    { QuickSort1Class<__int64>().sort((__int64*)base,nelem,comparator);
      break;
    }
  default            : // for all other values of width, we must use the hard way to copy and swap elements
    quickSort1AnyWidth(base,nelem,width,comparator);
    break;
  }
}
