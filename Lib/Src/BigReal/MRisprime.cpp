#include "pch.h"
#include <Math/MRIsPrime.h>

#define _1 pool->_1()
#define _2 pool->_2()

bool MRisprime(const BigInt &n, int threadId, MillerRabinHandler *handler) { // Miller-Rabin probabilistic primality test
  if(threadId < 0) {
    threadId = GetCurrentThreadId();
  }
  DigitPool *pool = n.getDigitPool();
  const BigInt _3(3,pool);

  if(n <= _1) {
    return false;
  } else if(n <= _3) {
    return true;
  } if(isEven(n) || ((n.getLastDigit() % 5) == 0) || (n % _3).isZero()) {
    return false;
  }

  BigInt nm1(n);
  --nm1; // nm1 is even
  BigInt r(nm1);
  int s = 0;
  while(isEven(r)) {
    r.divide2();
    s++;
  }

  MersenneTwister64 rnd;
  rnd.randomize();
  for(int i = 1; i < 30; i++) {
    if(handler != NULL) {
      handler->handleData(MillerRabinCheck(threadId, n, 0, format(_T("MRTest %-2d"), i)));
    }

    BigInt a(pool);
    do {
      a = randBigInt(nm1, rnd); // e(randBigReal(BigReal::getExpo10(nm1) + 1, &rnd, pool), BigReal::getExpo10(nm1) + 1, pool) % nm1;
    } while(a < _2);
    BigInt y = powmod(a,r,n);
    if((y != _1) && (y != nm1)) {
      for(int j = 1; (j < s) && (y != nm1); j++) {
        y = (y * y) % n;
        if(y == _1) {
          return false;
        }
      }
      if(y != nm1) {
        return false;
      }
    }
  }
  return true;
}
