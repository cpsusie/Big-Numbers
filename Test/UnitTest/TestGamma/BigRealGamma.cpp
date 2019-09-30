#include "stdafx.h"
#include "BigRealGamma.h"

#define _1 pool->get1()
#define _2 pool->get2()

// returns x so that integrale(t*exp(-t))dt from x to #inf < k
static int findLimit(const BigReal &k) {
  DigitPool *pool = k.getDigitPool();
  BigReal x = -rLn(k,8);
  x += rLn(x,8);
  BigReal f(pool);
  do { // newtonraphson iteration
    BigReal e = rExp(-x,8);
    f = (x+_1)*e-k;
    BigReal df(-x*e);
    x -= rQuot(f,df,8);
//    cout << "x:" << x << " f:" << f << "\n";
  } while(compareAbs(rQuot(f,k,8),e(_1,-3)) > 0);
  return (int)getInt64(x) + 1;
}

// assumes 1 < x < 2
static BigReal gamma1_2(const BigReal &x, int ndigits) {
  DigitPool *pool   = x.getDigitPool();
  BigReal    factor = _1;
  BigReal    c1     = e(_1,-ndigits-8);
  BigReal    TR     = findLimit(e(_1,-ndigits-8));
  BigReal    p      = _1;
//  cout << "TR:" << TR << "\n"; cout.flush();
  BigReal    currentSum(pool), lastSum(pool);
  for(BigReal i(pool->get0());;) {
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
  return currentSum * rPow(TR,x,ndigits);
}

BigReal gamma(const BigReal &x, int ndigits) {
  DigitPool *pool = x.getDigitPool();
  BigReal result(_1), p(pool);
  if(isInteger(x)) {
    if(x < _1) throwBigRealException(_T("Gamma not defined for integers < 1"));
    for(p = _2; p < x; ++p) {
      result = rProd(result,p,ndigits);
    }
    return result;
  }
  if(x > _2) {
    for(p = x; --p > _2;) {
      result = rProd(result,p,ndigits);
    }
    return rProd(rProd(result,p, ndigits),gamma1_2(p,ndigits), ndigits);
  } else if(x > _1) {
    return gamma1_2(x, ndigits);
  } else { // x < 1
    for(p = x; p < x; ++p) {
      result = rProd(result,p,ndigits);
    }
    return rQuot(gamma1_2(p,ndigits), result, ndigits);
  }
}

BigReal fac(const BigReal &x, int ndigits) {
  DigitPool *pool = x.getDigitPool();
  return gamma(x+_1,ndigits);
}
