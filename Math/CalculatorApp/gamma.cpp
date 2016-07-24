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
    f = (x+BIGREAL_1)*e-k;
    BigReal df(-x*e);
    x -= rQuot(f,df,8);
//    cout << "x:" << x << " f:" << f << "\n";
  } while(compareAbs(rQuot(f,k,8),e(BIGREAL_1,-3)) > 0);
  return (int)getInt64(x) + 1;
}

// assumes 1 < x < 2
static BigReal gamma1_2(const BigReal &x, int ndigits) {
  BigReal factor = BIGREAL_1;
  BigReal c1     = e(BIGREAL_1,-ndigits-8);
  BigReal TR     = findLimit(e(BIGREAL_1,-ndigits-8));
  BigReal p      = BIGREAL_1;
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
    if(x < BIGREAL_1) throwBigRealException(_T("Gamma not defined for integers < 1"));
    for(result = p = BIGREAL_1; p < x; ++p) {
      result = rProd(result,p,ndigits);
    }
    return result;
  } else if(x > BIGREAL_2) {
    for(result = BIGREAL_1, p = x - BIGREAL_1; p > BIGREAL_2; --p) {
      result = rProd(result,p,ndigits);
    }
    return rProd(rProd(result,p, ndigits),gamma1_2(p,ndigits), ndigits);
  } else if(x < BIGREAL_1) {
    for(result = BIGREAL_1, p = x; p < x; ++p) {
      result = rQuot(result,p,ndigits);
    }
    return rProd(result,gamma1_2(p,ndigits), ndigits);
  } else {
    return gamma1_2(x,ndigits);
  }
}

BigReal fac(const BigReal &x, int ndigits) {
  return gamma(x+BIGREAL_1,ndigits);
}
