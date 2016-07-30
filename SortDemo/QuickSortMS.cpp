#include "stdafx.h"

static AbstractComparator *temporaryComparator;
static int compareFunction(const void *e1, const void *e2) {
  return temporaryComparator->cmp(e1,e2);
}

void quickSortMS(void *base, size_t nelem, size_t width, AbstractComparator &comparator) {
  temporaryComparator = &comparator;
  qsort(base,nelem,width,compareFunction);
}
