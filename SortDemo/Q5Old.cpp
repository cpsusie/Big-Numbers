#include "stdafx.h"
#include <MyAssert.h>

static void sort5AnyWidth(void *base, size_t nelem, size_t width, AbstractComparator &comparator, char *pivot) {
  DECLARE_STACK(stack, 80);
  PUSH(stack, base, nelem);

  while(!ISEMPTY(stack)) {
    POP(stack, base, nelem, void);
    char *pi, *pj;
tailrecurse:
    switch(nelem) {
    case 0:
    case 1:
      continue;
    case 2:
      SORT2(0, 1);
      continue;
    case 3:
      SORT3OPT(0, 1, 2)
      continue;
    case 4:
      SORT4OPT(0, 1, 2, 3,;);
      continue;
    default:
#define SORT(i,j)      if(NEEDSWAP(i,j)) SWAP(i,j)
      { const size_t k = nelem/5;
        char *p0 = EPTR(0);
        char *p1 = EPTR(k);
        char *p2 = EPTR(nelem/2);
        char *p3 = EPTR(nelem-1-k);
        char *p4 = EPTR(nelem-1);

        SORT(0,1);  
        SORT(3,4);
        if(NEEDSWAP(2,4)) {
          SWAP(0,1);
          if(NEEDSWAP(0,4)) {
            SWAP(0,3);
            SORT(1,4);
            SORT(2,4);
          } else {                    
            if(NEEDSWAP(0,3)) {
              SWAP(0,2);
              SORT(0,3);
            } else {
              SWAP(0,1);
              SORT(2,3);
            }
          }
        } else {                      
          SWAP(1,3);
          if(NEEDSWAP(0,4)) {
            SWAP(0,2);
            SORT(2,4);
          } else {                    
            if(NEEDSWAP(2,3)) {
              SORT(1,3);
              SORT(2,3);
            } else {
              SWAP(3,4);
              if(NEEDSWAP(0,1)) {
                SORT(0,2);
              } else {                
                if(NEEDSWAP(1,4)) {
                  SWAP(1,2);
                  SWAP(2,4);
                } else {
                  SORT(1,2);
                }
              }
            }
          }
        }

        pi = EPTR(1);
        if(pi != p1) {
          PSWAP(p1, pi);
        }

        pj = EPTR(nelem-2);
        if(pj != p3) {
          PSWAP(pj, p3);
        }
        pi = EPTR(2);
        pj = EPTR(nelem-3);
        memcpy(pivot, p2, width);
      }
      break;
    }

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
    if(j > 0) {
      PUSH(stack, base,j+1);       // actually sort(base,j+1,width, compare,pivot);
    }
    if(i < nelem-1) {              // actually sort(EPTR(i),nelem-i, width, compare,pivot);
      base  = pi;
      nelem -= i;
      goto tailrecurse;
    }
  }
}

static void quickSort5AnyWidth(void *base, size_t nelem, size_t width, AbstractComparator &comparator) {
  char *pivot = new char[width];
  try {
    sort5AnyWidth(base, nelem, width, comparator, pivot);
    delete[] pivot;
  } catch(...) {
    delete[] pivot;
    throw;
  }
}

#pragma check_stack(off)

template <class T> class QuickSort5Class {
  public:
    void sort(T *base, size_t nelem, AbstractComparator &comparator);
};

template <class T> void QuickSort5Class<T>::sort(T *base, size_t nelem, AbstractComparator &comparator) {
  DECLARE_STACK(stack, 80);
  PUSH(stack, base, nelem);
  T pivot;

  while(!ISEMPTY(stack)) {
    POP(stack, base, nelem, T);
tailrecurse:
    T *pi, *pj;

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
      TSORT4OPT(0, 1, 2, 3,;);
      continue;
    default:
#undef SWAP
#undef SORT

//#define NEEDSWAP(i,j)  PNEEDSWAP(p##i,p##j)
#define SWAP(i,j)      TPSWAP( p##i, p##j)
#define SORT(i,j)      TPSORT2(p##i, p##j)

      { const size_t k = nelem/5;
        T *p0 = TEPTR(0);
        T *p1 = TEPTR(k);
        T *p2 = TEPTR(nelem/2);
        T *p3 = TEPTR(nelem-1-k);
        T *p4 = TEPTR(nelem-1);

        SORT(0,1);  
        SORT(3,4);
        if(NEEDSWAP(2,4)) {
          SWAP(0,1);
          if(NEEDSWAP(0,4)) {
            SWAP(0,3);
            SORT(1,4);
            SORT(2,4);
          } else {                    
            if(NEEDSWAP(0,3)) {
              SWAP(0,2);
              SORT(0,3);
            } else {
              SWAP(0,1);
              SORT(2,3);
            }
          }
        } else {                      
          SWAP(1,3);
          if(NEEDSWAP(0,4)) {
            SWAP(0,2);
            SORT(2,4);
          } else {                    
            if(NEEDSWAP(2,3)) {
              SORT(1,3);
              SORT(2,3);
            } else {
              SWAP(3,4);
              if(NEEDSWAP(0,1)) {
                SORT(0,2);
              } else {                
                if(NEEDSWAP(1,4)) {
                  SWAP(1,2);
                  SWAP(2,4);
                } else {
                  SORT(1,2);
                }
              }
            }
          }
        }

        pi = TEPTR(1);
        if(pi != p1) {
          TPSWAP(p1, pi);
        }

        pj = TEPTR(nelem-2);
        if(pj != p3) {
          TPSWAP(pj, p3);
        }
        pi = TEPTR(2);
        pj = TEPTR(nelem-3);
        pivot = *p2;
      }
      break;
    }

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

void quickSort5(void *base, size_t nelem, size_t width, AbstractComparator &comparator) {
  switch(width) {
  case sizeof(char)  :
    { QuickSort5Class<char>().sort((char*)base, nelem, comparator);
      break;
    }
  case sizeof(short) :
    { QuickSort5Class<short>().sort((short*)base, nelem, comparator);
      break;
    }
  case sizeof(long)  : // include pointertypes
    { QuickSort5Class<long>().sort((long*)base, nelem, comparator);
      break;
    }
  case sizeof(__int64):
    { QuickSort5Class<__int64>().sort((__int64*)base, nelem, comparator);
      break;
    }
  default            : // for all other values of width, we must use the hard way to copy and swap elements
    quickSort5AnyWidth(base, nelem, width, comparator);
    break;
  }
}
