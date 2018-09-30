#include "pch.h"

class NewtonConstants {
public:
  const ConstBigReal c1;
  const int         c4;
  const ConstBigReal c5;
  const ConstBigReal c6;
  const ConstBigReal c7;

  NewtonConstants()
    : c1( e(ConstBigReal(7),-1) ) // changed! not 0.9
    , c4( -7                   ) // changed! not -9
    , c5( e(BIGREAL_1,-1)       )
    , c6( e(ConstBigReal(11),-2))
    , c7( e(ConstBigReal(37),-2))
  {
  }
};

static const NewtonConstants NC;

// Assume x != 0 and y != 0 and f != 0
BigReal BigReal::quotNewton(const BigReal &x, const BigReal &y, const BigReal &f, DigitPool *pool) { // static

  const BigReal g = APCprod(<,APCprod(<,NC.c1,f,pool), fabs(APCprod(<,y,reciprocal(x,pool),pool)),pool);

  BRExpoType k = getExpo10(g);
  BRExpoType a[100];
  int n = 0;

  while(k < NC.c4) {
    k /= 2;
    a[n++] = k;
  }

  BigReal z = reciprocal(y, pool);
  BigReal u = APCprod(<,NC.c5,fabs(z),pool);
  for(int i = n-1; i >= 0; i--) {
    z += prod(z,dif(pool->get1(),prod(y,z,e(NC.c6,a[i], pool),pool), BIGREAL_0, pool), e(u,a[i], pool), pool);
  }
  return prod(x,z+ prod(z,pool->get1() - prod(y, z, g*NC.c6, pool), u*g, pool), f*NC.c7, pool);
}

