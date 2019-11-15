#include "pch.h"

class TanConstants {
public:
  const BigReal      &c1 ;
  const ConstBigReal  c2 ;
  const ConstBigReal &c3 ;
  const ConstBigReal &c4 ;
  const ConstBigReal &c5 ;
  const ConstBigReal  c6 ;
  const ConstBigReal  c7 ;
  const ConstBigReal  c8 ;
  const ConstBigReal  c9 ;
  const ConstBigReal  c10;
  const ConstBigReal  c11;
  const ConstBigReal  c12;
  const ConstBigReal &c13;
  const BigReal      &c14;
  const ConstBigReal  c15;
  const BigReal      &c16;

  TanConstants()
    :c1 ( BigReal::_1)
    ,c2 ( 0.1  )
    ,c3 ( c2   )
    ,c4 ( c2   )
    ,c5 ( c2   )
    ,c6 ( 0.04 )
    ,c7 ( 0.005)
    ,c8 ( 0.15 )
    ,c9 ( 0.37 )
    ,c10( 10   )
    ,c11( 0.11 )
    ,c12( 0.21 )
    ,c13( c11  )
    ,c14( BigReal::_2)
    ,c15( 0.13 )
    ,c16( BigReal::_05)
  {
  }
};

static const TanConstants TANC;

BigReal tan(const BigReal &x, const BigReal &f, DigitPool *digitPool) {
  VALIDATETOLERANCE(f)
  _SELECTDIGITPOOL(x);
  if(!x._isfinite()) {
    return pool->nan();
  }

  BigReal z = fabs(x, pool);

  if(z > TANC.c1) {
    BigReal m = floor(quot(x,pi(APCquot(<,TANC.c2,z,pool),pool),TANC.c3,pool),pool)+TANC.c16;
    BigReal r = APCquot(<,TANC.c4,fabs(m,pool),pool);
    BigReal q(TANC.c5,pool);
    z = fabs(dif(x, prod(pi(APCprod(<,r,q,pool),pool),m, APCprod(<,TANC.c6,q,pool),pool),APCprod(<,TANC.c7,q,pool),pool),pool);
    while(z < q) {
      q = APCprod(<,q,q,pool);
      z = fabs(dif(x, prod(pi(APCprod(<,q,r,pool),pool),m, APCprod(<,TANC.c6,q,pool),pool),APCprod(<,TANC.c7,q,pool),pool),pool);
    }
    BigReal d = APCprod(<,z,APCprod(<,TANC.c8,f,pool),pool);
    BigReal a = sin(x, d, pool);
    return quot(a
               ,cos(x, APCquot(<,APCprod(<,APCprod(<,d,TANC.c9,pool),z,pool),fabs(a)+APCprod(<,d,TANC.c10,pool),pool),pool)
               ,APCprod(<,TANC.c11,f,pool)
               ,pool
               );
  } else {
    BigReal a = sin(x, APCprod(<,TANC.c12,f,pool),pool);
    return quot(a
               ,cos(x, APCquot(<,APCprod(<,TANC.c13,f,pool),fabs(a,pool)+APCprod(<,TANC.c14,f,pool),pool),pool)
               ,APCprod(<,TANC.c15,f,pool)
               ,pool
               );
  }
}
