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
    ,c4( BigReal::_05)
  {
  }
};

static const SinConstants SINC;

BigReal sin(const BigReal &x, const BigReal &f) {
  VALIDATETOLERANCE(f)

  DigitPool *pool = x.getDigitPool();
  if(x.isZero()) {
    return pool->_0();
  }

  return cos(dif(pi(APCprod(<,SINC.c1,f,pool),pool)*SINC.c4,x,APCprod(<,SINC.c2,f,pool),pool),APCprod(<,SINC.c3,f,pool));
}
