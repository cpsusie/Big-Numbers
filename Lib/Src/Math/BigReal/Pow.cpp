#include "pch.h"

class PowConstants {
public:
  const ConstBigReal c1;
  const ConstBigReal c2;
  const ConstBigReal c3;

  PowConstants()
    :c1( 0.4)
    ,c2( 0.001)
    ,c3( 0.4)
  {
  }
};

static const PowConstants POWC;

BigReal pow(const BigReal &x, const BigReal &y, const BigReal &f) { // x^y
  DEFINEMETHODNAME(pow);
  if(!f.isPositive()) {
    throwInvalidToleranceException(method);
  }

  DigitPool *pool = x.getDigitPool();
#define _1 pool->get1()

  if(y.isZero()) {
    return _1;
  }
  if(y == _1) {
    return x;
  }
  if(x.isZero()) {
    if(y.isNegative()) {
      throwBigRealInvalidArgumentException(method, _T("x = 0 and y < 0"));
    }
    return pool->get0();
  }
  if(x.isNegative()) {
    if(even(y)) {
      return pow(-x,y,f);
    }
    if(odd(y)) {
      return -pow(-x,y,f);
    }
    throwBigRealInvalidArgumentException(method, _T("x < 0 and y not integer"));
  }

  BigReal a = (x > _1) ? PAPCpow(>,x, floor(y)+_1,pool) : PAPCpow(>,x,floor(y),pool);

  if(a > f) {
    const BigReal u = PAPCquot(<,f,a,pool);
    return exp(prod(y,ln(x, PAPCquot(<,PAPCprod(<,POWC.c1,u,pool),fabs(y),pool)),PAPCprod(<,POWC.c2,u,pool),pool),PAPCprod(<,POWC.c3,f,pool));
  } else {
    return pool->get0();
  }
}
