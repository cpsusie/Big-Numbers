#include "pch.h"

class CosConstants {
public:
  const ConstBigReal   c1;
  const ConstBigReal   c2;
  const ConstBigReal   c3;
  const ConstBigReal   c4;
  const ConstBigReal   c5;
  const ConstBigReal   c6;
  const ConstBigReal   c7;
  const ConstBigReal   c8;
  const ConstBigReal   c9;
  const ConstBigReal  c10;
  const ConstBigReal  c11;
  const ConstBigReal  c12;
  const ConstBigReal  c13;
  const ConstBigReal  c14;
  const ConstBigReal  c15;
  const ConstBigReal  c16;
  const ConstBigReal  c17;
  const ConstBigReal  c18;
  const ConstBigReal  c19;
  const ConstBigReal  c20;
  const BigReal      &c21;
  const BigReal      &c22;
  const ConstBigReal  c23;

  CosConstants()
    :c1  ( 4    )
    ,c2  ( 0.1  )
    ,c3  ( 0.1  )
    ,c4  ( 0.02 )
    ,c5  ( 0.007)
    ,c6  ( 0.08 )
    ,c7  ( 0.8  )
    ,c8  ( 0.3  )
    ,c9  ( 3.4  )
    ,c10 ( 5    )
    ,c11 ( 0.04 )
    ,c12 ( 5    )
    ,c13 ( 0.2  )
    ,c14 ( 2.72 )
    ,c15 ( 0.01 )
    ,c16 ( 0.05 )
    ,c17 ( 0.36 )
    ,c18 ( 0.02 )
    ,c19 ( 0.05 )
    ,c20 ( 5    )
    ,c21 ( BIGREAL_2)
    ,c22 ( BIGREAL_HALF)
    ,c23 ( 0.2)
  {
  }
};

static const CosConstants COSC;

BigReal cos(const BigReal &x, const BigReal &f) {
  DEFINEMETHODNAME;
  if(!f.isPositive()) {
    throwInvalidToleranceException(method);
  }

  DigitPool *pool = x.getDigitPool();
#define _1 pool->get1()

  if(f < _1) {
    const BigReal z = fabs(x);
    BigReal y(pool);
    if(z > COSC.c1) {
      const BigReal p = pi(PAPCquot(<,PAPCprod(<,COSC.c2,f,pool),z,pool),pool) * COSC.c21;
      y = dif(x,prod(p,floor(quot(x,p,COSC.c3,pool)+COSC.c22),f*COSC.c4,pool),f * COSC.c5,pool);
    } else {
      copy(y,x,f*COSC.c6);
    }

    BigReal jn = PAPCsum(>,COSC.c10,PAPCsum(>,PAPCprod(>,COSC.c7,sqrt(-BigReal::getExpo10N(f),COSC.c8),pool),PAPCprod(>,y,BigReal::getExpo10N(COSC.c9),pool),pool),pool);

    int j = (jn < _1) ? 0 : getInt(floor(jn));

    const BigReal h(BigReal::pow2(-j),pool);
    y = fabs(prod(y,h,h*COSC.c11*f,pool));
    BigReal g = PAPCprod(<,f, PAPCprod(<,PAPCpow(<,COSC.c13,BigInt(j,pool),pool),COSC.c12,pool),pool);
    const BigReal r = PAPCprod(>,COSC.c14,y,pool);
    BigReal d = PAPCprod(>,PAPCprod(>,COSC.c23,r,pool),r,pool);
    BigReal q(COSC.c23,pool);
    BigReal l = _1;
    while(g < d) {
      l = l + l;
      q = PAPCprod(>,q,q,pool);
      d = PAPCprod(>,PAPCprod(>,d,d,pool),q,pool);
    }
    BigReal n = _1;
    BigReal s = _1;
    BigReal t = _1;
    const BigReal a = -prod(y,y,PAPCprod(<,COSC.c15,g,pool),pool);
    const BigReal u = PAPCquot(<,PAPCprod(<,COSC.c16,g,pool),l,pool);
    const BigReal v = PAPCprod(<,COSC.c17,g,pool);
    const BigReal w = PAPCprod(<,COSC.c18,g,pool);

    while(fabs(t) > v) {
      t = quot(prod(t,a,w,pool),n*(n+_1),u,pool);
      s += t;
      n += COSC.c21;
    }

    g = PAPCprod(<,COSC.c19,g,pool);
    for(int i = 1; i <= j; i++) {
      s = prod(s,s,g,pool) * COSC.c21 - _1;
      g = PAPCprod(<,COSC.c20,g,pool);
    }
    return s;
  }
  return pool->get0();
}
