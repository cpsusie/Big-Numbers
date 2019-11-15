#include "pch.h"

class ACotConstants {
public:
  const ConstBigReal c1;
  const ConstBigReal c2;
  const BigReal     &c3;

  ACotConstants()
    :c1( 0.2)
    ,c2( 0.9)
    ,c3(BigReal::_05)
  {
  }
};

static const ACotConstants ACOTC;

#define _0 pool->_0()

BigReal acot(const BigReal &x, const BigReal &f, DigitPool *digitPool) {
  VALIDATETOLERANCE(f)
  _SELECTDIGITPOOL(x);
  if(!x._isfinite()) {
    return pool->nan();
  }

  return dif(pi(APCprod(<,ACOTC.c1,f,pool), pool)*ACOTC.c3, atan(x,APCprod(<,ACOTC.c2,f,pool),pool),_0, pool);
}
