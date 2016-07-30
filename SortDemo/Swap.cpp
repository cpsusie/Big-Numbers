#include "stdafx.h"

void swap(register char *p1, register char *p2, size_t w) {
#define swapBasicType(if_or_while,type,w)   \
  if_or_while(w >= sizeof(type)) {          \
    type tmp   = *(type*)p1;                \
    *(type*)p1 = *(type*)p2;                \
    *(type*)p2 = tmp;                       \
    w -= sizeof(type);                      \
    p1 += sizeof(type); p2 += sizeof(type); \
  }

  swapBasicType(while,long ,w)   /* take 4 bytes at a time */
  swapBasicType(if   ,short,w)   /* take 2 bytes at a time */
  swapBasicType(if   ,char ,w)   /* take the last (if any) */
}

/*
void swap( char *p1, char *p2, size_t width) {
  while(width--) {
    char tmp = *p1; *p1++ = *p2; *p2++ = tmp;
  }
}

*/
