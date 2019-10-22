#include "pch.h"

class ASinConstants {
public:
  const ConstBigReal   c1;
  const ConstBigReal   c2;
  const ConstBigReal   c3;
  const ConstBigReal   c4;
  const ConstBigReal  &c5;
  const ConstBigReal   c6;
  const ConstBigReal   c7;
  const ConstBigReal   c8;
  const ConstBigReal  &c9;
  const BigReal      &c10;
  const ConstBigReal  c11;
  const ConstBigReal  c12;
  const BigReal      &c13;
  const ConstBigReal  c14;

  ASinConstants()
    :c1  ( e(BigReal::_1,-10))
    ,c2  ( 10  )
    ,c3  ( 0.14)
    ,c4  ( 0.1 )
    ,c5  ( c4  )
    ,c6  ( 0.05)
    ,c7  ( 0.7 )
    ,c8  ( 1.5 )
    ,c9  ( c1  )
    ,c10 ( BigReal::_2)
    ,c11 ( .01 )
    ,c12 ( .98 )
    ,c13 ( BigReal::_05)
    ,c14 ( -1  )
  {
  }
};

static const ASinConstants ASINC;

BigReal asin(const BigReal &x, const BigReal &f) {
  VALIDATETOLERANCE(f)

  DigitPool *pool = x.getDigitPool();
#define _1 pool->_1()

  if(x.isZero()) {
    return pool->_0();
  }

  if(x.isPositive()) {
    BigReal g(pool),q(pool),t(pool),y(pool),z(pool);
    if(x > _1) {
      throwBigRealInvalidArgumentException(method, _T("x>1"));
    }
    if(x == _1) {
      return pi(f,pool)*ASINC.c13;
    }
    q = ASINC.c1;
    z = dif(_1,x,q);
    while(z < APCprod(>,ASINC.c2,q,pool)) {
      q = APCprod(<,q,q,pool);
      z = dif(_1,x,q,pool);
    }
    g = APCquot(<,APCprod(<,ASINC.c3,f,pool),x,pool);
    t = sqrt(_1 - prod(x,x,APCprod(<,f,APCquot(<,e(ASINC.c5,BigReal::getExpo10(z)/2,pool),x,pool),pool),pool),dmin(g,BigReal(ASINC.c4,pool)));
    if(t.isPositive()) {
      return atan(quot(x,t,APCquot(<,APCprod(<,ASINC.c6,f,pool),APCprod(>,t,t,pool),pool),pool),APCprod(<,ASINC.c7,f,pool));
    } else {
      return pi(APCprod(<,f,ASINC.c8,pool), pool) * ASINC.c13;
    }
  } else { // x < 0
    if(x < ASINC.c14) {
      throwBigRealInvalidArgumentException(method, _T("x<-1"));
    }
    return -asin(-x,f);
  }
}
