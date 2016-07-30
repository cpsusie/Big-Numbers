#include "stdafx.h"

static void bubbleSortAnyWidth(void *base, size_t nelem, size_t width, AbstractComparator &comparator) {
  size_t i;
  char *p1, *p2;

  while(nelem--)  {
    for(i = nelem, p1 = EPTR(0), p2 = EPTR(1); i--; p1 += width, p2 += width) {
      PSORT2(p1, p2);
    }
  }
}

template <class T> class BubbleSortClass {
public:
  void sort(T *base, size_t nelem, AbstractComparator &comparator);
};

template <class T> void BubbleSortClass<T>::sort(T *base, size_t nelem, AbstractComparator &comparator) {
  size_t i;
  T *p1,*p2;

  while(nelem--)  {
    for(i = nelem, p1 = base, p2 = base+1; i--; p1++, p2++) {
      TPSORT2(p1, p2);
    }
  }
}

void bubbleSort(void *base, size_t nelem, size_t width, AbstractComparator &comparator) {
  switch(width) {
  case sizeof(char)  :
    { BubbleSortClass<char>().sort((char*)base,nelem,comparator);
      break;
    }
  case sizeof(short) :
    { BubbleSortClass<short>().sort((short*)base,nelem,comparator);
      break;
    }
  case sizeof(long)  : // include pointertypes
    { BubbleSortClass<long>().sort((long*)base,nelem,comparator);
      break;
    }
  case sizeof(__int64):
    { BubbleSortClass<__int64>().sort((__int64*)base,nelem,comparator);
      break;
    }
  default            : // for all other values of width, we must use the hard way to copy and swap elements
    bubbleSortAnyWidth(base,nelem,width,comparator);
    break;
  }
}
