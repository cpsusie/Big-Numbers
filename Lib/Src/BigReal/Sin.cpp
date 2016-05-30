#include "pch.h"

class SinConstants {
public:
  const ConstBigReal  c1;
  const ConstBigReal  c2;
  const ConstBigReal  c3;
  const BigReal      &c4;

  SinConstants()
    :c1( 0.1 )
    ,c2( 0.05)
    ,c3( 0.9 )
    ,c4( BIGREAL_HALF)
  {
  }
};

static const SinConstants SINC;

BigReal sin(const BigReal &x, const BigReal &f) {
  DEFINEMETHODNAME(sin);
  if(!f.isPositive()) {
    throwInvalidToleranceException(method);
  }

  DigitPool *pool = x.getDigitPool();
  if(x.isZero()) {
    return pool->get0();
  }

  return cos(dif(pi(PAPCprod(<,SINC.c1,f,pool),pool)*SINC.c4,x,PAPCprod(<,SINC.c2,f,pool),pool),PAPCprod(<,SINC.c3,f,pool));
}
