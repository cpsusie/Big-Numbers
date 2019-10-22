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

BigReal acos(const BigReal &x, const BigReal &f) {
  VALIDATETOLERANCE(f)

  DigitPool *pool = x.getDigitPool();

  if(x == pool->_1()) {
    return pool->_0();
  }

  return pi(APCprod(<,f,ACOSC.c1,pool), pool)*ACOSC.c3 - asin(x,APCprod(<,f,ACOSC.c2,pool));
}
