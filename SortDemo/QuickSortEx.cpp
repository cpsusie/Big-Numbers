#include "stdafx.h"

#pragma check_stack(off)

static void quickSortExAnyWidth(void *base, size_t nelem, size_t width, AbstractComparator &comparator, char *pivot) {
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

    default:
      SORT3OPT(0, nelem/2, nelem-1);
      memcpy(pivot, EPTR(nelem/2), width);
      break;
    }

// No need to check first and last element against pivot element again
    char *ip = EPTR(1), *jp = EPTR(nelem-2);
    do {
      while(ip <= jp && comparator.cmp(ip, pivot) < 0) ip += width; // while(e[i]  < pivot) i++;
      while(ip <= jp && comparator.cmp(pivot, jp) < 0) jp -= width; // while(pivot < e[j] ) j--;
      if(ip < jp) {
        swap(ip, jp, width);
      }
      if(ip <= jp) {
        ip += width;
        jp -= width;
      }
    } while(ip <= jp);
    const size_t i = (ip - (char*)base) / width;
    const size_t j = (jp - (char*)base) / width;
    if(j > (int)nelem - i) {         // Sort the smallest partition first, to save stackspace
      if(j > 0) {
        PUSH(stack, base, j+1);      // Sort(base,j+1,width, comparator,pivot);
      }
      if(i < nelem-1) {         // Sort(ip,nelem-i, width, comparator,pivot);
        base = ip;
        nelem -= i;
        goto tailrecurse;
      }
    } else {
      if(i < nelem - 1) {
        PUSH(stack, ip, nelem-i);    // Sort(ip,nelem-i, width, comparator,pivot);
      }
      if(j > 0) {                    // Sort(base,j+1,width, comparator,pivot);
        nelem = j+1;
        goto tailrecurse;
      }
    }
  }
}

static void quickSortExNoRecursionAnyWidth(void *base, size_t nelem, size_t width, AbstractComparator &comparator) {
  char buffer[100], *pivot = width <= sizeof(buffer) ? buffer : new char[width];
  try {
    quickSortExAnyWidth(base, nelem, width, comparator, pivot);
    if(pivot!=buffer) delete[] pivot;
  } catch(...) {
    if(pivot!=buffer) delete[] pivot;
    throw;
  }
}

#pragma check_stack(off)

#undef EPTR

#ifdef swap
#undef swap
#endif

#define swap(p1,p2) { const T tmp = *p1; *p1 = *p2; *p2 = tmp; }

template <class T> class QuicksortExClass {
  public:
    void sort(T *base, size_t nelem, AbstractComparator &comparator);
};

// This is a private quicksort-template, where we use the fact, that the compiler 
// generates much faster code to copy and swap elements, when we dont have 
// to call a function to do it.
// Is only used for elements with size = sizeof(<primitive type>).
// where <primitive type> is on of <char>,<short>,<long> and <__int64>.
// For all other values of width, we call the general quicksortNoRecursionAnyWidth
template <class T> void QuicksortExClass<T>::sort(T *base, size_t nelem, AbstractComparator &comparator) {
  DECLARE_STACK(stack, 80);
  PUSH(stack, base, nelem);

  while(!ISEMPTY(stack)) {
    POP(stack, base, nelem, T);
tailrecurse:
    switch(nelem) {
    case 0:
    case 1:
      continue; 

    case 2:
      TSORT2(0, 1);
      continue;

    case 3:
      TSORT3OPT(0, 1, 2);
      continue;

    default:
      TSORT3OPT(0, nelem/2, nelem-1);
      break;
    }

    const T pivot = base[nelem/2];

    T *ip = TEPTR(1), *jp = TEPTR(nelem-2);
    do {
      while(ip <= jp && comparator.cmp(ip, &pivot) < 0) ip++;  // while e[i]  < pivot
      while(ip <= jp && comparator.cmp(&pivot, jp) < 0) jp--;  // while pivot < e[j]
      if(ip < jp) {
        swap(ip,jp);
      }
      if(ip <= jp) {
        ip++;
        jp--;
      }
    } while(ip <= jp);
    const size_t i = ip - base;
    const size_t j = jp - base;
    if(j > nelem - i) {         // Sort the smallest partition first, to save stackspace
      if(j > 0) {
        PUSH(stack, base, j+1); // Save start, count of elements to be sorted later. ie. Sort(base,j+1,width, comparator);
      }
      if(i < nelem-1) {         // Sort(ip, nelem-i, width, comparator);
        base  = ip;
        nelem -= i;
        goto tailrecurse;
      }
    } else {
      if(i < nelem-1) {          // Save start,count of elements to be sorted later. ie Sort(ip,nelem-i, width, comparator);
        PUSH(stack, ip, nelem-i); 
      }
      if(j > 0) {                // Sort(base,j+1,width, comparator);
        nelem = j+1;
        goto tailrecurse;
      }
    }
  }
}

void quickSortEx(void *base, size_t nelem, size_t width, AbstractComparator &comparator) {

//  testSwap();

  switch(width) {
  case sizeof(char)  :
    { QuicksortExClass<char>().sort((char*)base,nelem,comparator);
      break;
    }
  case sizeof(short) :
    { QuicksortExClass<short>().sort((short*)base,nelem,comparator);
      break;
    }
  case sizeof(long)  : // include pointertypes
    { QuicksortExClass<long>().sort((long*)base,nelem,comparator);
      break;
    }
  case sizeof(__int64):
    { QuicksortExClass<__int64>().sort((__int64*)base,nelem,comparator);
      break;
    }
  default            : // for all other values of width, we must use the hard way to copy and swap elements
    quickSortExNoRecursionAnyWidth(base,nelem,width,comparator);
    break;
  }
}

