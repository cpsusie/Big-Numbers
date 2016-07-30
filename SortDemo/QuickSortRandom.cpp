#include "stdafx.h"

static void sort(void *base, size_t nelem, size_t width, AbstractComparator &comparator, char *pivot) {
  memcpy(pivot,EPTR(rand() % nelem),width);

  char *pi = EPTR(0); 
  char *pj = EPTR(nelem-1);
  do {
    while(pi <= pj && comparator.cmp(pi,pivot) < 0) pi += width;
    while(pi <= pj && comparator.cmp(pivot,pj) < 0) pj -= width;
    if(pi < pj) {
      PSWAP(pi, pj);
    }
    if(pi<=pj) {
      pi += width;
      pj -= width;
    }
  } while(pi <= pj);

  size_t i = (pi - (char*)base) / width;
  size_t j = (pj - (char*)base) / width;

  if(j > 0      ) sort(base,j+1    , width, comparator, pivot);
  if(i < nelem-1) sort(pi  ,nelem-i, width, comparator, pivot);
}

void quickSortRandom(void *base, size_t nelem, size_t width, AbstractComparator &comparator) {
  char *pivot = new char[width];
  try {
    sort(base,nelem,width,comparator,pivot);
    delete[] pivot;
  } catch(...) {
    delete[] pivot;
    throw;
  }
}
