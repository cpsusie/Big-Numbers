#include "pch.h"
#include <Math/MRIsPrime.h>

#define _1 BigReal::_1
#define _2 BigReal::_2

bool MRisprime(int threadId, const BigInt &n, MillerRabinHandler *handler) { // Miller-Rabin probabilistic primality test
  static const ConstBigReal _3(3);

  DigitPool *pool = n.getDigitPool();
  if(n == _2) {
    return true;
  }
  if(n == _3) {
    return true;
  }
  if(n <  _2) {
    return false;
  }
  if(even(n)) {
    return false;
  }
  if((n % _3).isZero()) {
    return false;
  }
  if((n.getLastDigit() % 5) == 0) {
    return false;
  }
  BigInt nm1(n);
  --nm1; // nm1 is even
  BigInt r(nm1);
  int s = 0;
  while(even(r)) {
    r /= _2;
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
      for(int j = 1; j < s && y != nm1; j++) {
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
