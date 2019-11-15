#include "pch.h"

#define _0 pool->_0()
#define _1 pool->_1()
#define _2 pool->_2()

// returns x so that integrale(t*exp(-t))dt from x to +inf < k
static int findLimit(const BigReal &k) {
  DigitPool *pool = k.getDigitPool();
  BigReal    x    = -rLn(k,8);
  x += rLn(x,8);
  BigReal f(pool);
  do { // newtonraphson iteration
    const BigReal e = rExp(-x,8);
    f = (x+_1)*e-k;
    BigReal df(-x*e);
    x -= rQuot(f,df,8);
//    cout << "x:" << x << " f:" << f << "\n";
  } while(BigReal::compareAbs(rQuot(f,k,8),e(_1,-3)) > 0);
  return (int)getInt64(x) + 1;
}

// assumes 1 < x < 2
static BigReal gamma1_2(const BigReal &x, size_t digits, DigitPool *pool) {
  BigReal       factor = _1;
  const BigReal c1     = e(_1, -(BRExpoType)digits - 8,pool);
  const BigReal TR(findLimit(c1), pool);
  BigReal       p      = _1;
//  cout << "TR:" << TR << "\n"; cout.flush();
  BigReal currentSum(pool), lastSum(pool);
  for(BigReal i = _0;;) {
    lastSum = currentSum;
    const BigReal term = quot(quot(p,factor,c1),x+i,c1);
//    cout << "term("<<i<<"):" << FullFormatBigReal(term) << "\n"; cout.flush();
    currentSum = sum(currentSum,term,c1);
    if(currentSum == lastSum) {
      break;
    }
    p *= TR;
    ++i;
    factor = -factor * i;
  }
  return currentSum * rPow(TR,x,digits,pool);
}

BigReal rGamma(const BigReal &x, size_t digits, DigitPool *digitPool) {
  _SELECTDIGITPOOL(x);
  BigReal result(pool), p(pool);
  if(isInteger(x)) {
    if(x < _1) return pool->pinf();
    for(result = p = _1; p < x; ++p) {
      result = rProd(p, result, digits);
    }
    return result;
  } else if(x > _2) {
    for(result = _1, p = x - _1; p > _2; --p) {
      result = rProd(p, result, digits);
    }
    return rProd(gamma1_2(p,digits,pool), rProd(p, result, digits,pool), digits,pool);
  } else if(x < _1) {
    for(result = _1, p = x; p < x; ++p) {
      result = rProd(p,result,digits);
    }
    return rQuot(gamma1_2(p,digits,pool), result, digits,pool);
  } else {
    return gamma1_2(x,digits,pool);
  }
}

BigReal rFactorial(const BigReal &x, size_t digits, DigitPool *digitPool) {
  _SELECTDIGITPOOL(x);
  return rGamma(sum(x,_1,_0,pool),digits, pool);
}
