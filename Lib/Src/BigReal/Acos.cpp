#include "pch.h"

class ACosConstants {
public:
  const ConstBigReal c1;
  const ConstBigReal c2;
  const BigReal     &c3;

  ACosConstants()
    :c1( 0.2)
    ,c2( 0.9)
    ,c3(BigReal::_05)
  {
  }
};

static const ACosConstants ACOSC;

#define _0 pool->_0()
#define _1 pool->_1()

BigReal acos(const BigReal &x, const BigReal &f, DigitPool *digitPool) {
  VALIDATETOLERANCE(f)
  _SELECTDIGITPOOL(x);

  if(!x._isfinite() || (BigReal::compareAbs(x, _1) > 0)) {
    return pool->nan();
  }
  if(x == _1) {
    return _0;
  }

  return pi(APCprod(<,f,ACOSC.c1,pool), pool)*ACOSC.c3 - asin(x,APCprod(<,f,ACOSC.c2,pool),pool);
}
