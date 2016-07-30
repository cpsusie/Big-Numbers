#include "stdafx.h"
#include <alloc.h>
#include <mem.h>
#include "inssort.h"

void insertionSort(void *base, size_t nelem, size_t width,
                   int (*fcmp)(const void *, const void *)) {

  for(int i = nelem; i-- > 0;) {
    char *p1 = EPTR(i);
    char *p2 = EPTR(0);
    for(int j = 0; j < i; j++, p2 += width) {
      if(fcmp((void*)p1, (void*)p2) < 0) {
        PSWAP(p1, p2);
      }
    }
  }
}
