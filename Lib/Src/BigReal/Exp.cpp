#include "pch.h"

class ExpConstants {
public:
  const ConstBigReal   c1;
  const ConstBigReal   c2;
  const ConstBigReal   c3;
  const ConstBigReal   c4;
  const ConstBigReal   c5;
  const ConstBigReal   c6;
  const BigReal       &c7;
  const ConstBigReal   c8;
  const ConstBigReal   c9;
  const ConstBigReal  c10;
  const ConstBigReal  c11;
  const ConstBigReal  c12;
  const ConstBigReal  c13;
  const ConstBigReal  c14;
  const ConstBigReal  c15;
  const ConstBigReal  c16;
  const BigReal      &c17;
  const BigReal      &c18;
  const ConstBigInt   c19;
  const ConstBigReal  c20;

  ExpConstants()
    :c1  ( 2.71828182845235)
    ,c2  ( 0.03)
    ,c3  ( 0.04)
    ,c4  ( 0.3 )
    ,c5  ( 0.367879) // 44117144232159552377016146"))
    ,c6  ( 3.4 )
    ,c7  ( BIGREAL_2)
    ,c8  ( 1.1 )
    ,c9  ( 0.2 )
    ,c10 ( 3   )
    ,c11 ( 0.2 )
    ,c12 ( 0.3 )
    ,c13 ( 2.72)
    ,c14 ( 0.2 )
    ,c15 ( 0.76)
    ,c16 ( 0.4 )
    ,c17 ( BIGREAL_1)
    ,c18 ( BIGREAL_HALF)
    ,c19 ( -1  )
    ,c20 ( 10  )
  {
  }
};

static const ExpConstants EXPC;

BigReal exp( const BigReal &x, const BigReal &f) {
  DEFINEMETHODNAME;
  if(!f.isPositive()) {
    throwInvalidToleranceException(method);
  }

  DigitPool *pool = x.getDigitPool();
#define _1 pool->get1()

  if(x.isNegative()) {
    BigReal r = PAPCpow(<,EXPC.c1,BigInt(EXPC.c19,pool) - floor(x),pool);
    BigReal g = PAPCprod(<,f,r,pool);
    if(g < EXPC.c20) {
      return quot(_1,exp(-x,PAPCprod(<,r,PAPCprod(<,EXPC.c3,g,pool),pool)), f*EXPC.c4);
    } else {
      return PAPCquot(#,EXPC.c1,r,pool);
    }
  } else {
    BigReal g = PAPCprod(<,f,PAPCpow(<,EXPC.c5,floor(x)+_1,pool),pool);
    if(g >= _1) {
      return g;
    } else {
      BigReal kn = PAPCprod(>,EXPC.c6,BigReal::getExpo10N(x),pool) + EXPC.c7;
      BigReal jn = PAPCprod(>,EXPC.c8, sqrt(-BigReal::getExpo10N(g),EXPC.c9),pool) + kn + EXPC.c10;
      jn = (jn < _1) ? pool->get0() : floor(jn);
      kn = (kn < _1) ? _1  : (kn > jn) ? (jn + _1) : floor(kn);
      int j = getInt(jn),k = getInt(kn);
      BigReal h(pool);
      h = BigReal::pow2(-j);
      g = PAPCprod(<,g,PAPCprod(<,EXPC.c11,h,pool),pool);
      BigReal y = prod(x,h,g*EXPC.c12);
      BigReal d = PAPCprod(>,EXPC.c13,y,pool);
      BigReal q = BigReal(EXPC.c18, pool);
      BigReal l = _1;
      while(g < d) {
        l = l + l; // i.e l *= 2
        q = PAPCprod(>,q,q,pool);
        d = PAPCprod(>,q,PAPCprod(>,d,d,pool),pool);
      }
      BigReal u = PAPCquot(<,PAPCprod(<,EXPC.c14,g,pool),l,pool);
      BigReal v = PAPCprod(<,EXPC.c15,g,pool);
      BigReal w = PAPCprod(<,EXPC.c16,u,pool);
      BigReal s = _1;
      BigReal t = _1;
      BigReal n(pool);
      while(t > v) {
        ++n;
        t = quot(prod(t,y,w,pool),n,u,pool);
        s += t;
      }
      g = PAPCprod(<,EXPC.c17,g,pool);
      for(int i = k; i <= j; i++) {
        s = prod(s,s,g);
      }
      for(int i = 2; i <= k; i++) {
        s = prod(s,s,PAPCprod(#,g,PAPCprod(#,s,s,pool),pool), pool);
      }
      return s;
    }
  }
}
