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

#define _0 pool->_0()
#define _1 pool->_1()

BigReal sin(const BigReal &x, const BigReal &f, DigitPool *digitPool) {
  VALIDATETOLERANCE(f)
  _SELECTDIGITPOOL(x);
  if(x.isZero()) {
    return _0;
  }
  return cos(dif(pi(APCprod(<,SINC.c1,f,pool),pool)*SINC.c4,x,APCprod(<,SINC.c2,f,pool),pool),APCprod(<,SINC.c3,f,pool),pool);
}
