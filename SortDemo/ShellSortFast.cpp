#include "stdafx.h"
#include <stdlib.h>
#include <memory.h>

#if defined(IS32BIT)
#define ASM_OPTIMIZED
#endif

static void shellSortAnyWidth(void *base, size_t nelem, size_t width, AbstractComparator &comparator) {
  char *tmp = new char[width];
  size_t gap;
  for(gap = 1; gap <= nelem; gap = gap * 3 + 1);

#if defined(ASM_OPTIMIZED)
  __asm {
    push edi
    push esi
    push ecx
  }
#endif

  try {
    for(gap /= 3; gap > 0; gap /= 3) {
      const size_t hw = gap*width;
      for(size_t i = gap-1; i < nelem; i++) {
        char *pi = EPTR(i);
#if !defined(ASM_OPTIMIZED)
        memcpy(tmp,pi,width);
#else
        __asm {
          mov edi,tmp
          mov esi,pi
          mov ecx,width
          rep movsb
        }
#endif
        char *tp = pi - hw;
        for(;(tp >= base) && comparator.cmp(tp,tmp) > 0; pi = tp, tp -= hw) {
#if !defined(ASM_OPTIMIZED)
          memcpy(pi,tp,width);
#else
        __asm {
          mov edi,pi
          mov esi,tp
          mov ecx,width
          rep movsb
        }
#endif
        }
#if !defined(ASM_OPTIMIZED)
        memcpy(pi,tmp,width);
#else
        __asm {
          mov edi,pi
          mov esi,tmp
          mov ecx,width
          rep movsb
        }
#endif
      }
    }

#if defined(ASM_OPTIMIZED)
  __asm {
    pop ecx
    pop esi
    pop edi
  }
#endif

    delete[] tmp;
  } catch(...) {

#if defined(ASM_OPTIMIZED)
  __asm {
    pop ecx
    pop esi
    pop edi
  }
#endif

    delete[] tmp;
    throw;
  }
}

template <class T> class ShellSortClass {
public:
  void sort(T *base, size_t nelem, AbstractComparator &comparator);
};

template <class T> void ShellSortClass<T>::sort(T *base, size_t nelem, AbstractComparator &comparator) {
  size_t gap;
  for(gap = 1; gap <= nelem; gap = gap*3+1);
  for(gap /= 3; gap; gap /= 3) {
    for(size_t i = gap-1; i < nelem; i++) {
      T *pi = TEPTR(i);
      T tmp = *pi;
      for(T *tp = pi - gap; (tp >= base) && comparator.cmp(tp, &tmp) > 0; pi = tp, tp -= gap) {
        *(pi) = *(tp);
      }
      *pi = tmp;
    }
  }
}

void shellSortFast(void *base, size_t nelem, size_t width, AbstractComparator &comparator) {
  switch(width) {
  case sizeof(char)  :
    { ShellSortClass<char>().sort((char*)base, nelem, comparator);
      break;
    }
  case sizeof(short) :
    { ShellSortClass<short>().sort((short*)base, nelem, comparator);
      break;
    }
  case sizeof(long)  : // include pointertypes
    { ShellSortClass<long>().sort((long*)base, nelem, comparator);
      break;
    }
  case sizeof(__int64):
    { ShellSortClass<__int64>().sort((__int64*)base, nelem, comparator);
      break;
    }
  default            : // for all other values of width, we must use the hard way to copy and swap elements
    shellSortAnyWidth(base, nelem, width, comparator);
    break;
  }
}
