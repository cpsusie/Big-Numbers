#include "pch.h"

class CotConstants {
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
  const BigReal      &c12;

  CotConstants()
    :c1 ( 2.2  )
    ,c2 ( 0.26 )
    ,c3 ( 0.1  )
    ,c4 ( 0.05 )
    ,c5 ( 0.1  )
    ,c6 ( 0.04 )
    ,c7 ( 0.003)
    ,c8 ( 0.15 )
    ,c9 ( 0.36 )
    ,c10( 11   )
    ,c11( 0.1  )
    ,c12( BigReal::_05)
  {
  }
};

static const CotConstants COTC;

BigReal cot(const BigReal &x, const BigReal &f, DigitPool *digitPool) {
  VALIDATETOLERANCE(f)
  _SELECTDIGITPOOL(x);
  if(!x._isfinite()) {
    return pool->nan();
  }

  BigReal z = fabs(x,pool);
  if(z > COTC.c1) {
    BigReal m = floor(quot(x,pi(APCquot(<,COTC.c2,z,pool),pool),COTC.c3,pool)+COTC.c12,pool);
    BigReal r = APCquot(<,COTC.c4,fabs(m,pool),pool);
    BigReal q(COTC.c5,pool);
    z = fabs(dif(x,prod(m, pi(APCprod(<,q,r,pool),pool),APCprod(<,COTC.c6,q,pool),pool),APCprod(<,COTC.c7,q,pool),pool),pool);
    while(z < q) {
      q = APCprod(<,q,q,pool);
      z = fabs(dif(x, prod(m, pi(APCprod(<,q,r,pool),pool),APCprod(<,COTC.c6,q,pool),pool),APCprod(<,COTC.c7,q,pool),pool),pool);
    }
  }
  BigReal d = APCprod(<,APCprod(<,COTC.c8,f,pool),z,pool);
  BigReal a = cos(x, d, pool);
  return quot(a
             ,sin(x,APCquot(<,APCprod(<,APCprod(<,COTC.c9,d,pool),z,pool),fabs(a,pool)+APCprod(<,COTC.c10,d,pool),pool),pool)
             ,APCprod(<,COTC.c11,f,pool)
             ,pool
             );
}
