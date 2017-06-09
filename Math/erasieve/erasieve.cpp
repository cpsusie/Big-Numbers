#include "stdafx.h"
#include <BitSet.h>

void findprimes(ULONG from, ULONG to, BitSet &primes, bool print) {
  primes.invert();
  primes.remove(0,1);
  int count = 0;
  for(ULONG i = 2; i <= to; i++) {
    if(primes.contains(i)) {
      if(print && i >= from) printf("%6d%c",i,(count++%10==9)?'\n':' ');
      for(ULONG j = i+i; j <= to; j+=i) {
        primes.remove(j);
      }
    }
  }
  if(print) {
    printf("\n");
  }
}

typedef CompactIntArray Primefactors;

int nextprime(BitSet &primes, int i) {
  for(i++;i < primes.size() ;i++) {
    if(primes.contains(i))  {
      return i;
    }
  }
  printf("nextprime:no more primes (i=%d)\n",i);
  exit(-1);
}

Primefactors findfactors(BitSet &primes, ULONG n) {
  Primefactors res;
  int i = 2;
  while(n > 1) {
    if(n % i == 0) {
      res.add(i);
      n /= i;
    } else {
      i = nextprime(primes,i);
    }
  }
  return res;
}

void dumpfactors(const Primefactors &f) {
  for(size_t i = 0; i < f.size(); i++) {
    printf("%d ", f[i]);
  }
}

static void usage() {
  fprintf(stderr,"Usage: erasieve [-nfrom-to] [-f]\n");
  exit(-1);
}

int main(int argc, char **argv) {
  ULONG from = 2;
  ULONG to   = 1000;
  bool printprimes  = true;
  bool printfactors = false;
  char *cp;
  for(argv++;*argv && *(cp = *argv) == '-'; argv++) {
    for(cp++;*cp;cp++) {
      switch(*cp) {
        case 'n':
          if(sscanf(cp+1,"%lu-%lu",&from,&to) != 2) usage();
          break;
        case 'f':
          printfactors = true; 
          printprimes  = false;
          continue;
      }
      break;
    }
  }

  BitSet primes(to+1);
  findprimes(from,to,primes,printprimes);
  if(printfactors) {
    for(ULONG i = from; i <= to; i++) {
      Primefactors fac = findfactors(primes,i);
      printf("%6lu :",i); dumpfactors(fac); printf("\n");
    }
  }
  return 0;
}
