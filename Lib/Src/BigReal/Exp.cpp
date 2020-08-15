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
    :c1  ( _T("2.71828182845235"))
    ,c2  ( 0.03)
    ,c3  ( 0.04)
    ,c4  ( _T("0.3"))
//  ,c5  ( _T("0.36787944117144232159552377016146")) // e^-1
    ,c5  ( _T("0.3678794411714423215955237701614608674458111310317678345078368016974614957448998"))
    ,c6  ( _T("3.4")) // >= log2(10)
    ,c7  ( BigReal::_2)
    ,c8  ( 1.1 )
    ,c9  ( 0.2 )
    ,c10 ( 3   )
    ,c11 ( 0.2 )
    ,c12 ( 0.3 )
    ,c13 ( 2.72)
    ,c14 ( 0.2 )
    ,c15 ( 0.76)
    ,c16 ( 0.4 )
    ,c17 ( BigReal::_1)
    ,c18 ( BigReal::_05)
    ,c19 ( -1  )
    ,c20 ( 10  )
  {
  }
};

static const ExpConstants EXPC;

#define _0 pool->_0()
#define _1 pool->_1()

BigReal exp( const BigReal &x, const BigReal &f, DigitPool *digitPool) {
  VALIDATETOLERANCE(f)
  _SELECTDIGITPOOL(x);
  if(!x._isfinite()) {
    return pool->nan();
  }

  if(x.isNegative()) {
    BigReal r = APCpow(<,EXPC.c1,BigInt(EXPC.c19,pool) - floor(x,pool),pool);
    BigReal g = APCprod(<,f,r,pool);
    if(g < EXPC.c20) {
      return quot(_1,exp(-x,APCprod(<,r,APCprod(<,EXPC.c3,g,pool),pool),pool), prod(f,EXPC.c4,_0,pool),pool);
    } else {
      return APCquot(#,EXPC.c1,r,pool);
    }
  } else {
    BigReal g = APCprod(<,f,APCpow(<,EXPC.c5,floor(x,pool)+_1,pool),pool);
    if(g >= _1) {
      return g;
    } else {
      BigReal kn = APCprod(>,EXPC.c6,BigReal::getExpo10N(x,pool),pool) + EXPC.c7;
      BigReal jn = APCprod(>,EXPC.c8, sqrt(-BigReal::getExpo10N(g,pool),EXPC.c9,pool),pool) + kn + EXPC.c10;
      jn = (jn < _1) ? _0 : floor(jn,pool);
      kn = (kn < _1) ? _1  : (kn > jn) ? (jn + _1) : floor(kn,pool);
      int j = (int)jn, k = (int)kn;
      BigReal h(pool),y(pool),d(pool),q(pool),l(pool);
      h = BigReal::pow2(-j);
      g = APCprod(<,g,APCprod(<,EXPC.c11,h,pool),pool);
      y = prod(x,h,g*EXPC.c12);
      d = APCprod(>,EXPC.c13,y,pool);
      q = BigReal(EXPC.c18, pool);
      l = _1;
      while(g < d) {
        l = l + l; // i.e l *= 2
        q = APCprod(>,q,q,pool);
        d = APCprod(>,q,APCprod(>,d,d,pool),pool);
      }
      BigReal u = APCquot(<,APCprod(<,EXPC.c14,g,pool),l,pool);
      BigReal v = APCprod(<,EXPC.c15,g,pool);
      BigReal w = APCprod(<,EXPC.c16,u,pool);
      BigReal s = _1;
      BigReal t = _1;
      BigReal n(pool);
      while(t > v) {
        ++n;
        t = quot(prod(t,y,w,pool),n,u,pool);
        s += t;
      }
      g = APCprod(<,EXPC.c17,g,pool);
      for(int i = k; i <= j; i++) {
        s = prod(s,s,g);
      }
      for(int i = 2; i <= k; i++) {
        s = prod(s,s,APCprod(#,g,APCprod(#,s,s,pool),pool), pool);
      }
      return s;
    }
  }
}
