#include "pch.h"

static FastSemaphore lnLock;

class Ln10Constants {
public:
  DigitPool *m_digitPool;

  const ConstBigReal  c4;
  const ConstBigReal  c6;
  const ConstBigReal  c7;
  const int           c8;
  const ConstBigReal  c9;
  const ConstBigReal c10;
  const ConstBigReal c11;
  const ConstBigReal c12;
  const ConstBigReal c14;
  const ConstBigReal c15;

  BigReal *m_ln10Value; // cache
  BigReal *m_ln10Error; // cache

  Ln10Constants()
    :c4  ( 0.52)
    ,c6  ( e(ConstBigReal(6),-12))
    ,c7  ( 2.30258509299404)
    ,c8  ( -23  )
    ,c9  ( 0.7  )
    ,c10 ( 0.027)
    ,c11 ( 0.36 )
    ,c12 ( 0.014)
    ,c14 ( 0.01 )
    ,c15 ( 10   )
  {
    m_digitPool = BigRealResourcePool::fetchDigitPool();
    m_digitPool->setName(_T("LN"));
    m_ln10Value = new BigReal(m_digitPool); TRACE_NEW(m_ln10Value);
    m_ln10Error = new BigReal(m_digitPool); TRACE_NEW(m_ln10Error);
  }
  ~Ln10Constants() {
    SAFEDELETE(m_ln10Error);
    SAFEDELETE(m_ln10Value);
    BigRealResourcePool::releaseDigitPool(m_digitPool);
    m_digitPool = NULL;
  }
};

static const Ln10Constants LN10C;

BigReal BigReal::ln10(const BigReal &f, DigitPool *digitPool) { // static
  VALIDATETOLERANCE(f)
  _SELECTDIGITPOOL(f);

  BigReal result(pool);

  lnLock.wait();
  try {

#define _0 pool->_0()
#define _1 pool->_1()
#define ln10Value (*LN10C.m_ln10Value)
#define ln10Error (*LN10C.m_ln10Error)

    DigitPool *LNPOOL = LN10C.m_digitPool;

    if(ln10Error.isZero() || (f < ln10Error)) {
      if(f >= LN10C.c6) {
        ln10Value = LN10C.c7;
      } else {
        BigReal z(LN10C.c7, LNPOOL);
        BRExpoType k = BigReal::getExpo10(APCprod(<,LN10C.c4,f,LNPOOL));
        BRExpoType a[100];
        int n = 0;
        while(k < LN10C.c8) {
          k /= 2;
          a[n++] = k;
        }
        BigReal r(LNPOOL);
        for(int i = n-1; i >= 0; i--) {
          r = exp(z,e(BigReal(LN10C.c9,LNPOOL),a[i]),LNPOOL);
          z += quot(dif(LN10C.c15,r,_0,LNPOOL),r,e(LN10C.c10,a[i],LNPOOL), LNPOOL);
        }
        r = exp(z,APCprod(<,LN10C.c11,f,LNPOOL),LNPOOL);
        ln10Value = z + quot(dif(LN10C.c15,r,_0,LNPOOL),r,prod(f,LN10C.c12,_0,LNPOOL),LNPOOL);
      }
      ln10Error = f;
      result = ln10Value;
      goto End;
    }
    copy(result,ln10Value,prod(f,LN10C.c14,_0,LNPOOL));
  } catch(...) {
    lnLock.notify();
    throw;
  }
End:
  lnLock.notify();
  return result;
}

/*
static const ConstBigReal polyCoef[] = {
   5.9034869834262016e-001
 , 4.4838356452778050e-001
 ,-3.1027650082856274e-002
};
// Assume 1 <= x <= 10
BigReal BigReal::lnEstimate(const BigReal &x) { // static
  DigitPool *pool = x.getDigitPool();
  BigReal t = x-BigReal::_2;
  BigReal sum(polyCoef[2],pool);
  for(int i = 1; i >= 0; i--) {
    sum = APCsum(#,APCprod(#,sum,t,pool),polyCoef[i],pool);
  }
  return sum;
}

/*
static const double polyCoefD[] = {
   5.9034869834262016e-001
 , 4.4838356452778050e-001
 ,-3.1027650082856274e-002
};
// Assume 1 <= x <= 10
BigReal BigReal::lnEstimateD(const BigReal &x) { // static
  DigitPool *pool = x.getDigitPool();
  double t = getDouble(x) - 2;
  double sum = polyCoefD[2];
  for(int i = 1; i >= 0; i--) {
    sum = sum * t + polyCoefD[i];
  }
  return sum;
}
*/

/* (2,2)-Minimax-approximation of log(x+2) in [-1,8] with max error = 0.000615707 */
static const double coef[] = {
   6.9367867671125893e-001
  ,7.9736567612245068e-001
  ,1.0405429082159669e-001
  ,4.2759734781983422e-001
  ,2.4134099152860442e-002
};

// Assume 1 <= x <= 10
BigReal BigReal::lnEstimate(const BigReal &x, DigitPool *digitPool) { // static
  _SELECTDIGITPOOL(x);
  assert((_1 <= x) && (x <= LN10C.c15));
  const double t = getDouble(x) - 2;
  double sum1 = coef[2];
  double sum2 = coef[4];
  int i;
  for( i = 1; i >= 0; i-- ) sum1 = sum1 * t + coef[i];
  for( i = 3; i > 2; i-- ) sum2 = sum2 * t + coef[i];
  return BigReal(sum1 / (sum2 * t + 1.0), pool);
}

class LnConstants {
public:
  const ConstBigReal c1;
  const ConstBigReal c16;
  const int          c18;
  const ConstBigReal c19;
  const ConstBigReal c20;
  const ConstBigReal c21;
  const ConstBigReal c22;
  const ConstBigReal c23;
  const ConstBigReal c24;
  const ConstBigReal c25;

  LnConstants()
   :c1  ( 0.9  )
   ,c16 ( 0.004)
   ,c18 ( -1   )
   ,c19 ( 0.2  )
   ,c20 ( 0.007)
   ,c21 ( 0.04 )
   ,c22 ( 0.1  )
   ,c23 ( 0.15 )
   ,c24 ( 0.1  )
   ,c25 ( 0.01 )
   {
   }
};

static LnConstants LNC;

BigReal ln(const BigReal &x, const BigReal &f, DigitPool *digitPool) {
  VALIDATETOLERANCE(f)
  if(!x.isPositive()) {
    throwBigRealInvalidArgumentException(method, _T("x<=0"));
  }

  _SELECTDIGITPOOL(x);
  if(x == _1) {
    return _0;
  }

  const BigReal g(APCprod(<,f, LNC.c1, pool));

  BRExpoType k = BigReal::getExpo10(g);

  BigReal y(pool);
  copy(y,x,APCprod(<,x, APCprod(<,f, LNC.c16,pool),pool));

  const BRExpoType m = BigReal::getExpo10(y);
  y = e(y,-m,pool);
  BigReal z = BigReal::lnEstimate(y,pool);

  BRExpoType a[100];
  int n = 0;
  while(k < LNC.c18) {
    k /= 2;
    a[n++] = k;
  }
  const BigReal u = APCprod(<, LNC.c19, y, pool);
  const BigReal v = APCprod(<, LNC.c20, y, pool);
  BigReal r(pool);
  for(int i = n-1; i >= 0; i--) {
    k = a[i];
    r = exp(z,e(u,k,pool));
    z += quot(dif(y,r,e(v,k,pool),pool),r,e(LNC.c21,k,pool),pool);
  }
//  cout << _T("ln loop done!\n"); cout.flush();
  if(g < LNC.c22) {
    r = exp(z,APCprod(<,u,g,pool),pool);
  } else {
    r = exp(z,APCprod(<,u,LNC.c22,pool),pool);
  }
  if(m == 0) {
    return z + quot(dif(y,r,v*g,pool),r,g*LNC.c23,pool);
  } else {
    return z
         + quot(dif(y,r,v*g,pool),r,g*LNC.c21,pool)
         + prod(BigReal(m,pool),BigReal::ln10(APCquot(<,APCprod(<,LNC.c24,f,pool),BigReal(abs(m),pool),pool),pool),prod(f,LNC.c25,_0,pool),pool);
  }
}

class Ln1Constants {
public:
  const ConstBigReal  c1;
  const ConstBigReal  c2;
  const ConstBigReal  c3;
  const ConstBigReal  c4;
  const BigReal      &c5;
  const BigReal      &c6;
  const ConstBigReal  c7;
  const ConstBigReal  c8;
  const ConstBigReal  c9;
  const ConstBigReal c10;
  const ConstBigReal c11;
  const ConstBigReal c12;
  const ConstBigReal c13;
  const ConstBigReal c14;
  const ConstBigReal c15;
  const ConstBigReal c16;
  const ConstBigReal c17;
  const ConstBigReal c18;
  const ConstBigReal c19;
  const ConstBigReal c20;
  const ConstBigReal c21;
  const ConstBigReal c22;
  const ConstBigReal c23;
  const ConstBigReal c24;


  Ln1Constants()
   :c1(  0.06)
   ,c2(  0.01)
   ,c3(  0.09)
   ,c4(  1.5 )
   ,c5(  BigReal::_05)
   ,c6(  BigReal::_05)
   ,c7(  0.3 )
   ,c8(  0.3 )
   ,c9(  10.4)
   ,c10( 1.6 )
   ,c11( 0.86)
   ,c12( 0.2 )
   ,c13( 3.3 )
   ,c14( -5  )
   ,c15( 0.29)
   ,c16( 5.2 )
   ,c17( 0.2 )
   ,c18( -3.4)
   ,c19( 2.2 )
   ,c20( 0.4 )
   ,c21( 7   )
   ,c22( 0.03)
   ,c23( 0.08)
   ,c24( 0.1 )
   {
   }
};

static const Ln1Constants LN1C;

BigReal ln1(const BigReal &x, const BigReal &f, DigitPool *digitPool) {
  VALIDATETOLERANCE(f)
  _SELECTDIGITPOOL(x);
  if(!x._isfinite()) {
    return pool->nan();
  }
  if(x.isZero()) {
    return pool->ninf();
  }

  if(!x.isPositive()) {
    return pool->nan();
  }

  if(x == _1) {
    return _0;
  }

  BigReal y(pool);

  if(x < _1) {
    y = quot(_1,x,APCquot(>,APCprod(>,LN1C.c1,f,pool),x,pool),pool);
  } else {
    copy(y,x,APCprod(>,x,APCprod(>,LN1C.c2,f,pool),pool));
  }
  BigReal g = APCprod(<,LN1C.c3,f,pool);
  BigReal h = _1;
  while((y > LN1C.c4) && (g > LN1C.c5)) {
    y = sqrt(y,e(LN1C.c6,BigReal::getExpo10(y)/2,pool),pool);
    g = APCprod(<,LN1C.c7,g,pool);
    h = h + h;
  }
  while(y > LN1C.c4) {
    y = sqrt(y,e(g,BigReal::getExpo10(y)/2,pool));
    g = APCprod(<,LN1C.c8,g,pool);
    h = h + h;
  }
  BigReal s = y - _1;
  if(s > APCprod(>,LN1C.c9,g,pool)) {
    g = APCprod(<,LN1C.c10,g,pool);
    BigReal jn = APCsum(<,LN1C.c14,APCsum(<,APCprod(<,LN1C.c11,sqrt(-BigReal::getExpo10N(g,pool),LN1C.c12),pool),APCprod(<,LN1C.c13,BigReal::getExpo10N(s,pool),pool),pool),pool);
    int j = (jn < _1) ? 0 : getInt(floor(jn));
    for(int i = 1; i <= j; i++) {
      y = sqrt(y,g);
      g = APCprod(<,LN1C.c15,g,pool);
    }
    s = y - _1;
    BigReal l(pool);
    if(s < LN1C.c24) {
      l = quot(BigReal::getExpo10N(APCprod(<,LN1C.c16,g,pool)),BigReal(BigReal::getExpo10(s)+1,pool),LN1C.c17,pool);
    } else {
      l = APCprod(>,LN1C.c18,BigReal::getExpo10N(APCprod(<,LN1C.c16,g,pool)),pool);
    }
    if(l > _1) {
      const BigReal u = APCquot(<,APCprod(<,LN1C.c19,g,pool),l,pool);
      const BigReal w = APCprod(<,LN1C.c20,u,pool);
      const BigReal v = APCprod(<,LN1C.c21,g,pool);
      BigReal t = s;
      const BigReal z = -s;
      BigReal n = _1;
      while(BigReal::compareAbs(t,v) > 0) {
        ++n;
        t = prod(t,z,w,pool);
        s += quot(t,n,u,pool);
      }
    }
    if(x < _1) {
      return -prod(h*BigReal::pow2(j),s,prod(f,LN1C.c22,_0,pool),pool);
    } else {
      return  prod(h*BigReal::pow2(j),s,prod(f,LN1C.c23,_0,pool),pool);
    }
  } else {
    throwBigRealException(_T("cond. 1"));
    return _0;
  }
}

#undef _1

class LogConstants {
public:
  const ConstBigReal c1 ;
  const ConstBigReal c2 ;
  const ConstBigReal c3 ;
  const ConstBigReal c4 ;
  const ConstBigReal c5 ;
  const ConstBigReal c6 ;
  const ConstBigReal c7 ;
  const ConstBigReal c8 ;
  const ConstBigReal c9 ;
  const ConstBigReal c10;

  LogConstants()
    :c1  ( -2.3            )
    ,c2  ( e(BigReal::_1,-10))
    ,c3  ( 10              )
    ,c4  ( 0.23            )
    ,c5  ( 2.3             )
    ,c6  ( 0.4             )
    ,c7  ( 11              )
    ,c8  ( 0.12            )
    ,c9  ( 0.1             )
    ,c10 ( 10              )
    {
    }
};

static const LogConstants LOGC;

#define _1 pool->_1()

BigReal log(const BigReal &base, const BigReal &x, const BigReal &f, DigitPool *digitPool) { // log(x) base base
  VALIDATETOLERANCE(f)
  _SELECTDIGITPOOL(x);
  if(!base._isfinite() || !x._isfinite()) {
    return pool->nan();
  }
  if(base == _1) {
    return pool->pinf();
  }
  if(!base.isPositive() || x.isNegative()) {
    return pool->nan();
  }
  if(x.isZero()) {
    return pool->ninf();
  }

  BigReal r(pool);

  if(base < LOGC.c10) {
    if(base < LOGC.c9) {
      r = APCprod(<,APCsum(>,BigReal::getExpo10N(base,pool),_1,pool),LOGC.c1,pool);
    } else {
      BigReal q = (LOGC.c2,pool);
      BigReal z = fabs(dif(base,_1,q,pool));
      while(z < APCprod(<,LOGC.c3,q,pool)) {
        q = APCprod(<,q,q,pool);
        z = fabs(dif(base,_1,q,pool));
      }
      r = APCprod(<,LOGC.c2,z,pool);
    }
  } else {
    r = APCprod(<,LOGC.c5, BigReal::getExpo10N(base,pool), pool);
  }

  const BigReal d = APCprod(<,APCprod(<,LOGC.c6,r,pool),f,pool);
  const BigReal a = ln(x, d, pool);
  return quot(a,ln(base,APCquot(<,APCprod(<,d,r,pool),APCsum(>,fabs(a,pool),APCprod(<,LOGC.c7,d,pool),pool),pool),pool),APCprod(<,f,LOGC.c8,pool),pool);
}

class Log10Constants {
public:
  const ConstBigReal c5;
  const ConstBigReal c6;
  const ConstBigReal c7;
  const ConstBigReal c8;

  Log10Constants()
   :c5 ( 2.3 )
   ,c6 ( 0.4 )
   ,c7 ( 11  )
   ,c8 ( 0.12)
  {
  }
};

static const Log10Constants L10C;

BigReal log10(const BigReal &x, const BigReal &f, DigitPool *digitPool) {
  VALIDATETOLERANCE(f)
  _SELECTDIGITPOOL(x);
  if(!x._isfinite()) {
    return pool->nan();
  }
  if(x.isZero()) {
    return pool->ninf();
  }
  if(!x.isPositive()) {
    return pool->nan();
  }

  const BigReal &r = L10C.c5;
  const BigReal  d = APCprod(<,f, APCprod(<,L10C.c6,r,pool),pool);
  const BigReal  a = ln(x, d, pool);
  return quot(a,BigReal::ln10(APCquot(<,APCprod(<,d,r,pool),APCsum(>,fabs(a),APCprod(<,L10C.c7,d,pool),pool),pool)),APCprod(<,L10C.c8,f,pool),pool);
}
