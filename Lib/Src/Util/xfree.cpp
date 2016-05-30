#include "pch.h"
#include <malloc.h>

void xfree(void *p) {
  free(p);
}

