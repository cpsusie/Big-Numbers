#include "pch.h"

class ACotConstants {
public:
  const ConstBigReal c1;
  const ConstBigReal c2;
  const BigReal     &c3;

  ACotConstants()
    :c1( 0.2)
    ,c2( 0.9)
    ,c3(BIGREAL_HALF)
  {
  }
};

static const ACotConstants ACOTC;

BigReal acot(const BigReal &x, const BigReal &f) {
  DEFINEMETHODNAME;
  if(!f.isPositive()) {
    throwInvalidToleranceException(method);
  }

  DigitPool *pool = x.getDigitPool();

  return dif(pi(PAPCprod(<,ACOTC.c1,f,pool), pool)*ACOTC.c3, atan(x,PAPCprod(<,ACOTC.c2,f,pool)),BIGREAL_0, pool);
}
