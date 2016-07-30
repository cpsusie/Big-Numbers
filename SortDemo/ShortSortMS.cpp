#include "stdafx.h"

void shortSortMS(void *base, size_t nelem, size_t width, AbstractComparator &comparator) {
  char *lo = EPTR(0);
  char *hi = EPTR(nelem-1);

  /* Note: in assertions below, i and j are always inside original bound of Array to sort. */

  while(hi > lo) {
    /* A[i] <= A[j] for i <= j, j > hi */
    char *max = lo;
    for(char *p = lo+width; p <= hi; p += width) {
        /* A[i] <= A[max] for lo <= i < p */
        if(comparator.cmp(p, max) > 0) {
          max = p;
        }
        /* A[i] <= A[max] for lo <= i <= p */
    }
    /* A[i] <= A[max] for lo <= i <= hi */
    swap(max, hi, width);
    /* A[i] <= A[hi] for i <= hi, so A[i] <= A[j] for i <= j, j >= hi */
    hi -= width;
    /* A[i] <= A[j] for i <= j, j > hi, loop top condition established */
  }
  /* A[i] <= A[j] for i <= j, j > lo, which implies A[i] <= A[j] for i < j,
     so Array is sorted */
}
