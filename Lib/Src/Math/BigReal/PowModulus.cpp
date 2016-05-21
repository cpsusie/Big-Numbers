#include "pch.h"

// Calculates a^r mod n
BigInt powmod(const BigInt &a, const BigInt &r, const BigInt &n) {
  DigitPool *pool = a.getDigitPool();

  BigInt p = pool->get1();
  BigInt tmpa(a);
  BigInt tmpr(BigReal(r, pool));


  int i = 1;
  while(!tmpr.isZero()) {
//    printf("p:"); p.print(); printf("\n");
//    setCursorPos(0,1);
//    printf("tmpr:"); tmpr.print(stdout,false); printf("              ");
//    printf("tmpa:"); tmpa.print(); printf("\n");

    if(odd(tmpr)) {
      p = (p * tmpa) % n;
      --tmpr;
    }
    else {
      tmpa = (tmpa * tmpa) % n;
      tmpr *= pool->getHalf();
    }
  }
  return p;
}

