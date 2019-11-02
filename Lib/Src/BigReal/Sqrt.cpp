#include "pch.h"

class SqrtConstants {
public:
  const ConstBigReal  c1;
  const ConstBigReal  c2;
  const ConstBigReal  c6;
  const ConstBigReal  c7;
  const ConstBigReal  c8;
  const BigReal      &c9;

  SqrtConstants()
    :c1( 0.1245)
    ,c2( 1.17  )
    ,c6( 0.01  )
    ,c7( 0.28  )
    ,c8( 10    )
    ,c9( BigReal::_05)
  {
  }
};

static const SqrtConstants SQRTC;

BigReal sqrt(const BigReal &x, const BigReal &f) {
  VALIDATETOLERANCE(f)
  DigitPool *pool = x.getDigitPool();
  if(x.isNegative()) {
    pool->nan();
  }

  if(x.isZero()) {
    return pool->_0();
  }

  BRExpoType m = BigReal::getExpo10(x);
  if(m > 0) {
    m /= 2;
  } else {
    m = (m-1) / 2;
  }
  BigReal y = APCprod(<,e(SQRTC.c1,-m,pool),APCsum(<,x,e(SQRTC.c8,2*m,pool),pool),pool);

  y = APCprod(#,APCsum(#,APCquot(#,x,y,pool),y,pool),SQRTC.c9,pool);

  BigReal g = APCquot(<,APCprod(<,SQRTC.c2,f,pool),y,pool);

  BRExpoType k = BigReal::getExpo10(g);
  BRExpoType a[100];
  int n = 0;
  while(k < -1) {
    k /= 2;
    a[n++] = k;
  }
  BigReal u = APCprod(<,APCprod(<,y,y,pool),SQRTC.c6,pool);

  BigReal v = APCprod(<,SQRTC.c7,y,pool);

  for(int i = n-1; i >= 0; i--) {
    y += quot(dif(x,y*y,e(u,a[i],pool),pool),y+y,e(v,a[i],pool),pool);
  }
  return y + quot(dif(x,y*y,u*g,pool),y+y,v*g,pool);
}
