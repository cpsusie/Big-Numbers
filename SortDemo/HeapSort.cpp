#include "stdafx.h"

static void heapSortAnyWidth(void *base, size_t nelem, size_t width, AbstractComparator &comparator) {
  size_t f,s;
  char *fp, *cp;

  for(size_t i = 1; i < nelem; i++) {        // Insert into ptqueue. base[0] is biggest
    for(f = i/2, cp = EPTR(i), fp = EPTR(f); cp != fp; cp = fp, f/=2, fp = EPTR(f)) {
      if(comparator.cmp(fp,cp) >= 0) {
        break;
      }
      PSWAP(fp, cp);
    }
  }
  for(size_t i = nelem-1; i > 0;) {                 // delete from ptqueue
    cp = EPTR(i);
    PSWAP((char*)base, cp);
    i--;                                     // ptqueues size is decremented by 1
    for(f = 0, s = 1; s <= i; f = s, s*=2) { // bubble base[0] down to its place
      cp = EPTR(s);
      if(s < i) {                            // f got two children. Use the biggest to bubble
        char *cp2 = cp + width;
        if(comparator.cmp(cp, cp2) < 0) {    // cp2 is biggest
          s++;
          cp = cp2;
        }
      }
      fp = EPTR(f);
      if(comparator.cmp(fp, cp) >= 0) {
        break;                               // f is on its place
      }
      PSWAP(fp, cp);
    }
  }
}

template <class T> class HeapSortClass {
public:
  void sort(T *base, size_t nelem, AbstractComparator &comparator);
};

template <class T> void HeapSortClass<T>::sort(T *base, size_t nelem, AbstractComparator &comparator) {
  size_t f,s;
  T *fp, *cp;

  for(size_t i = 1; i < nelem; i++) {        // Insert into ptqueue. base[0] is biggest
    for(f = i/2, cp = base+i, fp = base+f; cp != fp; cp = fp, f/=2, fp = base+f) {
      if(comparator.cmp(fp,cp) >= 0) {
        break;
      }
      TPSWAP(fp, cp);
    }
  }
  for(size_t i = nelem-1; i > 0;) {                 // delete from ptqueue
    cp = base + i--;                         // remove to root of ptqueue (biggest) and
    TPSWAP(base, cp);

    for(f = 0, s = 1; s <= i; f = s, s*=2) { // bubble base[0] down to its place
      cp = base+s;
      if(s < i) {                            // f got two children. Use the biggest to bubble
        T *cp2 = cp + 1;
        if(comparator.cmp(cp, cp2) < 0) {    // cp2 is biggest
          s++;
          cp = cp2;
        }
      }
      fp = base+f;
      if(comparator.cmp(fp, cp) >= 0) {
        break;                               // f is on its place
      }
      TPSWAP(fp, cp);
    }
  }
}

void heapSort(void *base, size_t nelem, size_t width, AbstractComparator &comparator) {
  switch(width) {
  case sizeof(char)  :
    { HeapSortClass<char>().sort((char*)base,nelem,comparator);
      break;
    }
  case sizeof(short) :
    { HeapSortClass<short>().sort((short*)base,nelem,comparator);
      break;
    }
  case sizeof(long)  : // include pointertypes
    { HeapSortClass<long>().sort((long*)base,nelem,comparator);
      break;
    }
  case sizeof(__int64):
    { HeapSortClass<__int64>().sort((__int64*)base,nelem,comparator);
      break;
    }
  default            : // for all other values of width, we must use the hard way to copy and swap elements
    heapSortAnyWidth(base,nelem,width,comparator);
    break;
  }
}
