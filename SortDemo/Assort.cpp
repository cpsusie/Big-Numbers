#include "stdafx.h"
#include <stdlib.h>
#include "assort.h"

static void swap( char *p1, char *p2, size_t w) {
  size_t i;
  char tmp;
  for( i = w; i--; ) {
    tmp = *p1; *p1++ = *p2; *p2++ = tmp;
  }
}

void assort( void *base, size_t nel, size_t width, int (*cmp)(const void *, const void *) ) {
  size_t  i, gap;
  char    *p1, *p2;

  for( gap = 1; gap <= nel; gap = 3*gap + 1 );

  for( gap /= 3;  gap > 0; gap /= 3 )
    for( i = gap; i < nel; i++ )
      for( int j = i-gap; j >= 0 ; j -= gap ) {
        p1 = (char*)base + (width * j);
        p2 = (char*)base + (width * (j+gap));
        if(cmp( p1, p2 ) <= 0 )
          break;
        swap(p1,p2,width);
      }
}

