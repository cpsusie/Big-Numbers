#include "stdafx.h"
#include <MyUtil.h>
#include <Math/BigReal.h>
#include "Calculator.h"

// returns x so that integrale(t*exp(-t))dt from x to #inf < k
int findLimit(const BigReal &k) {
  BigReal x = -rLn(k,8);
  x += rLn(x,8);
  BigReal f;
  do { // newtonraphson iteration
    BigReal e = rExp(-x,8);
    f = (x+BigReal::_1)*e-k;
    BigReal df(-x*e);
    x -= rQuot(f,df,8);
//    cout << "x:" << x << " f:" << f << "\n";
  } while(compareAbs(rQuot(f,k,8),e(BigReal::_1,-3)) > 0);
  return (int)getInt64(x) + 1;
}

// assumes 1 < x < 2
static BigReal gamma1_2(const BigReal &x, int ndigits) {
  BigReal factor = BigReal::_1;
  BigReal c1     = e(BigReal::_1,-ndigits-8);
  BigReal TR     = findLimit(e(BigReal::_1,-ndigits-8));
  BigReal p      = BigReal::_1;
//  cout << "TR:" << TR << "\n"; cout.flush();
  BigReal currentSum, lastSum;
  for(int i = 0;;i++) {
    lastSum = currentSum;
    const BigReal term = quot(quot(p,factor,c1),x+i,c1);
//    cout << "term("<<i<<"):" << FullFormatBigReal(term) << "\n"; cout.flush();
    currentSum = sum(currentSum,term,c1);
    if(currentSum == lastSum) {
      break;
    }
    p *= TR;
    factor = -factor * (i+1);
  }
  return currentSum * rPow(TR,x,ndigits);
}

BigReal gamma(const BigReal &x, int ndigits) {
  BigReal result, p;
  if(isInteger(x)) {
    if(x < BigReal::_1) return BigReal::_BR_PINF;
    for(result = p = BigReal::_1; p < x; ++p) {
      result = rProd(p, result,ndigits);
    }
    return result;
  } else if(x > BigReal::_2) {
    for(result = BigReal::_1, p = x - BigReal::_1; p > BigReal::_2; --p) {
      result = rProd(p, result, ndigits);
    }
    return rProd(gamma1_2(p,ndigits), rProd(p, result, ndigits), ndigits);
  } else if(x < BigReal::_1) {
    for(result = BigReal::_1, p = x; p < x; ++p) {
      result = rProd(p,result,ndigits);
    }
    return rQuot(gamma1_2(p,ndigits), result, ndigits);
  } else {
    return gamma1_2(x,ndigits);
  }
}

BigReal factorial(const BigReal &x, int ndigits) {
  return gamma(x+BigReal::_1,ndigits);
}
