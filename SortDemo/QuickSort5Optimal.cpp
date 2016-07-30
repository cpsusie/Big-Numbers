#include "stdafx.h"
/*
static void checkOrder(AbstractComparator &comparator, void *p0, void *p1,...) {
  int a[100];
  int n = 0;
  memset(a, 0, sizeof(a));

  a[n++] = *(int*)p0;
  a[n++] = *(int*)p1;
  va_list argptr;
  va_start(argptr, p1);
  for(void *p = va_arg(argptr, void*); p; p = va_arg(argptr,void*)) {
    a[n++] = *(int*)p;
  }
  va_end(argptr);

  for(int i = 1; i < n; i++) {
    if(comparator.cmp(&a[i-1], &a[i]) > 0) {
      String s;
      for(int k = 0; k < n; k++) {
        s += format(_T("%d "), a[k]);
      }
      AfxMessageBox(format(_T("unorder(%d):%s"), n, s.cstr()).cstr(), MB_ICONWARNING);
      return;
    }
  }
}
*/

static void sort5AnyWidth(void *base, size_t nelem, size_t width, AbstractComparator &comparator, char *pivot) {
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

    case 5:
      SORT5OPT(0, 1, 2, 3, 4, continue);
      continue;

    default:
      SORT3OPT(0, nelem/2, nelem-1);
      PASSIGN(pivot, EPTR(nelem/2));
      break;
    }

    char *pi = EPTR(1), *pj = EPTR(nelem-2);
    do {
      while(pi <= pj && comparator.cmp(pi,pivot) < 0) pi += width; 
      while(pi <= pj && comparator.cmp(pivot,pj) < 0) pj -= width;
      if(pi < pj) {
        PSWAP(pi, pj);
      }
      if(pi <= pj) {
        pi += width;
        pj -= width;
      }
    } while(pi <= pj);
    const size_t i = (pi - (char*)base) / width;
    const size_t j = (pj - (char*)base) / width;
    if(j > 0) {
      PUSH(stack, base, j+1);      // actually sort(base,j+1,width, compare,pivot);
    }
    if(i < nelem-1) {  // actually sort(EPTR(i),nelem-i, width, compare,pivot);
      base  = pi;
      nelem -= i;
      goto tailrecurse;
    }
  }
}

static void quickSort5AnyWidth(void *base, size_t nelem, size_t width, AbstractComparator &comparator) {
  char buffer[100], *pivot = width <= sizeof(buffer) ? buffer : new char[width];
  try {
    sort5AnyWidth(base, nelem, width, comparator, pivot);
    if(pivot!=buffer) delete[] pivot;
  } catch(...) {
    if(pivot!=buffer) delete[] pivot;
    throw;
  }
}

#pragma check_stack(off)

template <class T> class QuickSort5OptimalClass {
  public:
    void sort(T *base, size_t nelem, AbstractComparator &comparator);
};

template <class T> void QuickSort5OptimalClass<T>::sort(T *base, size_t nelem, AbstractComparator &comparator) {
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
      TSORT3OPT(0, 1, 2);
      continue;
    case 4:
      TSORT4OPT(0, 1, 2, 3, continue);
      continue;
    case 5:
      TSORT5OPT(0, 1, 2, 3, 4, continue);
      continue;
    default:
      TSORT3OPT(0, nelem/2, nelem-1);
      break;
    }

    const T pivot = base[nelem/2];
    T *pi = TEPTR(1), *pj = TEPTR(nelem-2);
    do {
      while(pi <= pj && comparator.cmp(pi,&pivot) < 0) pi++;
      while(pi <= pj && comparator.cmp(&pivot,pj) < 0) pj--;
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

void quickSort5Optimal(void *base, size_t nelem, size_t width, AbstractComparator &comparator) {
  switch(width) {
  case sizeof(char)  :
    { QuickSort5OptimalClass<char>().sort((char*)base, nelem, comparator);
      break;
    }
  case sizeof(short) :
    { QuickSort5OptimalClass<short>().sort((short*)base, nelem, comparator);
      break;
    }
  case sizeof(long)  : // include pointertypes
    { QuickSort5OptimalClass<long>().sort((long*)base, nelem, comparator);
      break;
    }
  case sizeof(__int64):
    { QuickSort5OptimalClass<__int64>().sort((__int64*)base, nelem, comparator);
      break;
    }
  default            : // for all other values of width, we must use the hard way to copy and swap elements
    quickSort5AnyWidth(base, nelem, width, comparator);
    break;
  }
}
