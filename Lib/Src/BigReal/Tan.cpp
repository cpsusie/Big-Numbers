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
    :c1 ( BIGREAL_1)
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
    ,c14( BIGREAL_2)
    ,c15( 0.13 )
    ,c16( BIGREAL_HALF)
  {
  }
};

static const TanConstants TANC;

BigReal tan(const BigReal &x, const BigReal &f) {
  DEFINEMETHODNAME;
  if(!f.isPositive()) {
    throwInvalidToleranceException(method);
  }

  BigReal z = fabs(x);

  DigitPool *pool = x.getDigitPool();

  if(z > TANC.c1) {
    BigReal m = floor(quot(x,pi(PAPCquot(<,TANC.c2,z,pool),pool),TANC.c3,pool))+TANC.c16;
    BigReal r = PAPCquot(<,TANC.c4,fabs(m),pool);
    BigReal q(TANC.c5,pool);
    z = fabs(dif(x, prod(pi(PAPCprod(<,r,q,pool),pool),m, PAPCprod(<,TANC.c6,q,pool),pool),PAPCprod(<,TANC.c7,q,pool),pool));
    while(z < q) {
      q = PAPCprod(<,q,q,pool);
      z = fabs(dif(x, prod(pi(PAPCprod(<,q,r,pool),pool),m, PAPCprod(<,TANC.c6,q,pool),pool),PAPCprod(<,TANC.c7,q,pool),pool));
    }
    BigReal d = PAPCprod(<,z,PAPCprod(<,TANC.c8,f,pool),pool);
    BigReal a = sin(x, d);
    return quot(a,
                cos(x, PAPCquot(<,PAPCprod(<,PAPCprod(<,d,TANC.c9,pool),z,pool),fabs(a)+PAPCprod(<,d,TANC.c10,pool),pool)),
                PAPCprod(<,TANC.c11,f,pool)
               );
  } else {
    BigReal a = sin(x, PAPCprod(<,TANC.c12,f,pool));
    return quot(a,
                cos(x, PAPCquot(<,PAPCprod(<,TANC.c13,f,pool),fabs(a)+PAPCprod(<,TANC.c14,f,pool),pool)),
                PAPCprod(<,TANC.c15,f,pool)
               );
  }
}
