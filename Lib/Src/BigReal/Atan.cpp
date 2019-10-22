#include "pch.h"

class ATanConstants {
public:
  const ConstBigReal   c1;
  const ConstBigReal   c3;
  const BigReal       &c4;
  const ConstBigReal   c5;
  const ConstBigReal   c6;
  const ConstBigReal   c7;
  const ConstBigReal   c8;
  const ConstBigReal   c9;
  const ConstBigReal  c10;
  const ConstBigReal  c11;
  const ConstBigReal  c12;
  const ConstBigReal  c13;
  const BigReal      &c14;
  const ConstBigReal  c15;
  const ConstBigReal  c16;
  const ConstBigReal  c17;
  const ConstBigReal  c18;
  const ConstBigReal  c19;
  const ConstBigReal  c20;
  const BigReal      &c21;
  const BigReal      &c22;
  const BigReal      &c23;
  const ConstBigReal  c24;
  const ConstBigReal  c25;
  const BigReal      &c26;

  ATanConstants()
    :c1  ( 0.001)
    ,c3  ( 1.58 )
    ,c4  ( BigReal::_05)
    ,c5  ( 0.27 )
    ,c6  ( 0.07 )
    ,c7  ( 0.07 )
    ,c8  ( 0.2  )
    ,c9  ( 0.4  )
    ,c10 ( 0.35 )
    ,c11 ( 0.3  )
    ,c12 ( 3.3  )
    ,c13 ( -5   )
    ,c14 ( BigReal::_05)
    ,c15 ( 0.2  )
    ,c16 ( 0.22 )
    ,c17 ( 1.16 )
    ,c18 ( 2.5  )
    ,c19 ( -0.06)
    ,c20 ( -0.6 )
    ,c21 ( BigReal::_05)
    ,c22 ( BigReal::_1)
    ,c23 ( BigReal::_05)
    ,c24 ( 0.47 )
    ,c25 ( 0.08 )
    ,c26 ( BigReal::_2)
  {
  }
};

static const ATanConstants ATANC;

BigReal atan(const BigReal &x, const BigReal &f) {
  VALIDATETOLERANCE(f)

  DigitPool *pool = x.getDigitPool();
#define _1 pool->_1()

  BigReal y = x.isNegative() ? -x : x;

  if(f < y && f < ATANC.c3) {
    BigReal h = _1;
    BigReal g = f;
    while(y > ATANC.c4) {
      BigReal a = APCprod(<,g,y,pool);
      y = quot(sqrt(_1 + prod(y,y,APCprod(<,APCprod(<,ATANC.c5,y,pool),a,pool),pool),APCprod(<,ATANC.c6,a,pool)) - _1
              ,y
              ,APCprod(<,ATANC.c7,g,pool)
              );
      h = h + h;
      g = APCprod(<,ATANC.c8,g,pool);
    }
    g = APCprod(<,ATANC.c9,g,pool);
    BigReal jn = APCsum(<,APCsum(<,APCprod(<,sqrt(-BigReal::getExpo10N(g),ATANC.c11),ATANC.c10,pool)
                                  ,APCprod(<,ATANC.c12,BigReal::getExpo10N(y),pool)
                                  ,pool
                                 )
                         ,ATANC.c13
                         ,pool
                       );
    int j = (jn < _1) ? 0 : getInt(floor(jn));

    for(int i = 1; i <= j; i++) {
      BigReal a = APCquot(<,g,y,pool);
      y = quot(y,sqrt(_1+prod(y,y,APCprod(<,ATANC.c14,a,pool)),a)+_1,APCprod(<,g,ATANC.c15,pool));
      g = APCprod(<,ATANC.c16,g,pool);
    }
    const BigReal v = APCprod(<,ATANC.c17,BigReal::getExpo10N(APCprod(<,ATANC.c18,g,pool)),pool);
    const BigReal p = BigReal(BigReal::getExpo10(y) + 1,pool);
    const BigReal r = p.isZero() ? APCquot(<,ATANC.c19,v,pool) : APCprod(<,APCquot(<,ATANC.c20,v,pool),APCprod(<,p,p,pool),pool);
    const BigReal l = quot(v,APCsum(>,ln(y,r),r,pool),ATANC.c21,pool);
    if(l > _1) {
      const BigReal u = APCquot(<,APCprod(<,ATANC.c22,g,pool),l,pool);
      const BigReal w = APCprod(<,ATANC.c23,u,pool);
      BigReal n = _1;
      BigReal s = y;
      const BigReal t   = -prod(y,y,APCprod(<,ATANC.c24,u,pool));
      const UINT    l32 = getUint(l);
      for(UINT i = 1; i <= l32; i++) {
        n += ATANC.c26;
        s = prod(s,t,w,pool);
        y += quot(s,n,u,pool);
      }
    }
    if(x.isNegative()) {
      return -prod(y,h * BigReal::pow2(j),APCprod(<,ATANC.c25,f,pool),pool);
    } else {
      return  prod(y,h * BigReal::pow2(j),APCprod(<,ATANC.c25,f,pool),pool);
    }
  }
  return pool->_0();
}
