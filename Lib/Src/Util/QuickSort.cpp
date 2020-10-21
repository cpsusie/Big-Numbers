#include "pch.h"
#include <MemSwap.h>
#include <Comparator.h>

#define swap(p1, p2,w) memSwap(p1,p2,w)

#define PUSH(base,size) { baseStack[stackTop] = base; sizeStack[stackTop++] = size; }
#define POP(base,size)  { base = baseStack[--stackTop]; size = sizeStack[stackTop]; }

#define EPTR(n) ((char*)base+width*(n))

static void quickSortAnyWidth(void *base, size_t nelem, size_t width, AbstractComparator &comparator, char *pivot) {
  void  *baseStack[50];
  size_t sizeStack[50];
  int    stackTop = 0;
  char  *ip,*jp;

  PUSH(base, nelem);

  while(stackTop > 0) {
    POP(base, nelem);
tailrecurse:

    switch(nelem) {
    case 0:
    case 1:
      continue;

    case 2:
      ip = EPTR(0);
      jp = EPTR(1);
      if(comparator.cmp(ip, jp) > 0) {
        swap(ip, jp, width);
      }
      continue;

    case 3:
      ip = EPTR(0) ;
      jp = EPTR(1);
      if(comparator.cmp(ip, jp) > 0) {
        swap(ip, jp, width);
      }
      ip = EPTR(2);
      if(comparator.cmp(jp, ip) > 0) {
        swap(ip, jp, width);
        ip = EPTR(0);
        if(comparator.cmp(ip, jp) > 0) {
          swap(ip, jp, width);
        }
      }
      continue;

    default:
// Sort the first, middle and last element. select pivot element as the middle of the three
      ip = EPTR(0) ;
      jp = EPTR(nelem / 2);
      if(comparator.cmp( ip, jp) > 0) {
        swap(ip, jp, width);
      }
      ip = EPTR(nelem - 1);
      if(comparator.cmp(jp, ip) > 0) {
        swap(ip, jp, width);
        ip = EPTR(0);
        if(comparator.cmp(ip, jp) > 0) {
          swap(ip, jp, width);
        }
      }
      break;
    }

// Save the middle element in the pivot
    memcpy(pivot, jp, width);

// No need to check first and last element against pivot element again
    ip = EPTR(1); jp = EPTR(nelem-2);
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
    intptr_t i = (ip - (char*)base) / width;
    intptr_t j = (jp - (char*)base) / width;
    if(j > (intptr_t)nelem - i) {    // Sort the smallest partition first, to save stackspace
      if(j > 0) {
        PUSH(base, j+1);             // Sort(base,j+1,width, comparator,pivot);
      }
      if(i < (intptr_t)nelem-1) {         // Sort(ip,nelem-i, width, comparator,pivot);
        base = ip;
        nelem -= i;
        goto tailrecurse;
      }
    } else {
      if(i < (intptr_t)nelem - 1) {
        PUSH(ip, nelem-i);           // Sort(ip,nelem-i, width, comparator,pivot);
      }
      if(j > 0) {                    // Sort(base,j+1,width, comparator,pivot);
        nelem = j+1;
        goto tailrecurse;
      }
    }
  }
}

static void quicksortNoRecursionAnyWidth(void *base, size_t nelem, size_t width, AbstractComparator &comparator) {
  char tmp[1024], *pivot = (width > sizeof(tmp)) ? new char[width] : tmp;
  try {
    quickSortAnyWidth(base, nelem, width, comparator, pivot);
    if(pivot != tmp) delete[] pivot;
  } catch(...) {
    if(pivot != tmp) delete[] pivot;
    throw;
  }
}

// ------------------------------------- SingleThreaded QuicksortClass implementation --------------------------------------

#pragma check_stack(off)

#if defined(EPTR)
#undef EPTR
#endif
#define EPTR(n)    base+(n)
#if defined(swap)
#undef swap
#endif
#define swap(p1,p2) { const T tmp = *p1; *p1 = *p2; *p2 = tmp; }


// For T = bacic types (char,short,long,double), we can make swap and save operations
// very fast, and these cases occur very often
// This is a private quicksort-template, where we use the fact, that the compiler
// generates much faster code to copy and swap elements, when we dont have
// to call a function to do it.
// Is only used for elements with size = sizeof(<primitive type>).
// where <primitive type> is on of <char>,<short>,<long> and <double>.
// For all other values of width, we call the general quicksortNoRecursionAnyWidth
template<typename T> void sort(T *base, size_t nelem, AbstractComparator &comparator) {
  T     *baseStack[50];
  size_t sizeStack[50];
  int    stackTop = 0;
  T     *ip, *jp;

  PUSH(base,nelem);

  while(stackTop > 0) {
    POP(base,nelem);
tailrecurse:
    switch(nelem) {
    case 0:
    case 1:
      continue;

    case 2:
      ip = EPTR(0);
      jp = EPTR(1);
      if(comparator.cmp(ip, jp) > 0) {
        swap(ip,jp);
      }
      continue;

    case 3:
      ip = EPTR(0);
      jp = EPTR(1);
      if(comparator.cmp(ip, jp) > 0) {
        swap(ip, jp);
      }
      ip = EPTR(2);
      if(comparator.cmp(jp, ip) > 0) {
        swap(ip, jp);
        ip = EPTR(0);
        if(comparator.cmp(ip, jp) > 0) {
          swap(ip, jp);
        }
      }
      continue;

    default:

// Sort the first, middle and last element. Select pivot element as the middle of the three
      ip = EPTR(0);
      jp = EPTR(nelem / 2);
      if(comparator.cmp(ip, jp) > 0) {
        swap(ip, jp);
      }
      ip = EPTR(nelem - 1);
      if(comparator.cmp(jp, ip) > 0) {
        swap(ip, jp);
        ip = EPTR(0);
        if(comparator.cmp(ip, jp) > 0) {
          swap(ip, jp);
        }
      }
      break;
    }

// Save the middle element in the pivot
    const T pivot = *jp;

// No need to check first and last element against pivot element again
    ip = EPTR(1); jp = EPTR(nelem-2);
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
    const intptr_t i = ip - base;
    const intptr_t j = jp - base;
    if(j > (intptr_t)nelem - i) {  // Sort the smallest partition first, to save stackspace
      if(j > 0) {
        PUSH(base, j+1);           // Save start, count of elements to be sorted later. ie. Sort(base,j+1,width, comparator);
      }
      if(i < (intptr_t)nelem-1) {  // Sort(ip, nelem-i, width, comparator);
        base  = ip;
        nelem -= i;
        goto tailrecurse;
      }
    } else {
      if(i < (intptr_t)nelem-1) {  // Save start,count of elements to be sorted later. ie Sort(ip,nelem-i, width, comparator);
        PUSH(ip, nelem-i);
      }
      if(j > 0) {                  // Sort(base,j+1,width, comparator);
        nelem = j+1;
        goto tailrecurse;
      }
    }
  }
}

void quickSort(void *base, size_t nelem, size_t width, AbstractComparator &comparator) {
  switch(width) {
  case sizeof(char)  :
    sort((char*)base,nelem,comparator);
    break;
  case sizeof(short) :
    sort((short*)base,nelem,comparator);
    break;
  case sizeof(long)  : // include pointertypes
    sort((long*)base,nelem,comparator);
    break;
  case sizeof(__int64):
    sort((__int64*)base,nelem,comparator);
    break;
  default            : // for all other values of width, we must use the hard way to copy and swap elements
    quicksortNoRecursionAnyWidth(base,nelem,width,comparator);
    break;
  }
}

class QuickSortComparator : public AbstractComparator {
private:
  int (__cdecl *m_compare)(const void*, const void*);
public:
  int cmp(const void *e1, const void *e2) override {
    return m_compare(e1,e2);
  }
  AbstractComparator *clone() const override {
	return new QuickSortComparator(m_compare);
  }
  QuickSortComparator(int (__cdecl *compare)(const void*, const void*)) {
    m_compare = compare;
  }
};

void quickSort(void *base, size_t nelem, size_t width, int (__cdecl *compare)(const void*, const void*)) {
  quickSort(base,nelem,width,QuickSortComparator(compare));
}
