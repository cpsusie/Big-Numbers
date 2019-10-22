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

BigReal acot(const BigReal &x, const BigReal &f) {
  VALIDATETOLERANCE(f)

  DigitPool *pool = x.getDigitPool();

  return dif(pi(APCprod(<,ACOTC.c1,f,pool), pool)*ACOTC.c3, atan(x,APCprod(<,ACOTC.c2,f,pool)),BigReal::_0, pool);
}
