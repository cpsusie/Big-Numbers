#include "stdafx.h"
#include <HashSet.h>

#define RADIX 10

unsigned long cubicsum(unsigned long x) {
  unsigned long sum = 0;
  while(x > 0) {
    int digit = x % RADIX;
    sum += digit * digit * digit;
    x /= RADIX;
  }
  return sum;
}

#define MAXCOUNT 40

int main(int argc, char **argv) {
  for(unsigned int x = 3; x < 5000; x+=3) {
    printf("%d:",x);
    unsigned int count = 0;
    IntHashSet set;
    for(unsigned int v = x;count < MAXCOUNT;count++) {
      set.add(v);
      v = cubicsum(v);
      printf(" %2d",v);
      if(set.contains(v))
        break;
    }
    if(count == MAXCOUNT) printf("...");
    printf("\n");
  }
  return 0;
}
