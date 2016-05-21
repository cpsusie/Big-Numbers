#include "pch.h"

static Semaphore ln_gate;
static DigitPool ln_pool(LN_DIGITPOOL_ID);

#define LNPOOL (&ln_pool)

class Ln10Constants {
public:
  const ConstBigReal  c4;
  const ConstBigReal  c6;
  const ConstBigReal  c7;
  const int          c8;
  const ConstBigReal  c9;
  const ConstBigReal c10;
  const ConstBigReal c11;
  const ConstBigReal c12;
  const ConstBigReal c14;
  const ConstBigReal c15;
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
  }
};

static const Ln10Constants LN10C;

#define _1 pool->get1()

BigReal BigReal::ln10(const BigReal &f) { // static
  DEFINEMETHODNAME(ln10);
  ln_gate.wait();

  try {

    static BigReal ln10Error(LNPOOL); // cache
    static BigReal ln10Value(LNPOOL); // cache

    if(!f.isPositive()) {
      throwInvalidToleranceException(method);
    }

    if(ln10Error.isZero() || f < ln10Error) {
      if(f >= LN10C.c6) {
        ln10Value = LN10C.c7;
      } else {
        BigReal z(LN10C.c7, LNPOOL);
        int k = BigReal::getExpo10(PAPCprod(<,LN10C.c4,f,LNPOOL));
        int a[100];
        int n = 0;
        while(k < LN10C.c8) {
          k /= 2;
          a[n++] = k;
        }
        BigReal r(LNPOOL);
        for(int i = n-1; i >= 0; i--) {
          r = exp(z,e(BigReal(LN10C.c9,LNPOOL),a[i]));
          z += quot(dif(LN10C.c15,r,BIGREAL_0,LNPOOL),r,e(LN10C.c10,a[i],LNPOOL), LNPOOL);
        }
        r = exp(z,PAPCprod(<,LN10C.c11,f,LNPOOL));
        ln10Value = z + quot(dif(LN10C.c15,r,BIGREAL_0,LNPOOL),r,f*LN10C.c12);
      }
      ln10Error = f;
      ln_gate.signal();
      return ln10Value;
    }
    BigReal tmp(f.getDigitPool());
    copy(tmp,ln10Value,f*LN10C.c14);
    ln_gate.signal();

    return tmp;
  } catch(...) {
    ln_gate.signal();
    throw;
  }
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
  BigReal t = x-BIGREAL_2;
  BigReal sum(polyCoef[2],pool);
  for(int i = 1; i >= 0; i--) {
    sum = PAPCsum(#,PAPCprod(#,sum,t,pool),polyCoef[i],pool);
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
BigReal BigReal::lnEstimate(const BigReal &x) { // static
  const double t = getDouble(x) - 2;
  double sum1 = coef[2];
  double sum2 = coef[4];
  int i;
  for( i = 1; i >= 0; i-- ) sum1 = sum1 * t + coef[i];
  for( i = 3; i > 2; i-- ) sum2 = sum2 * t + coef[i];
  return BigReal(sum1 / (sum2 * t + 1.0), x.getDigitPool());
}

class LnConstants {
public:
  const ConstBigReal  c1;
  const ConstBigReal c16;
  const int         c18;
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

BigReal ln(const BigReal &x, const BigReal &f) {
  DEFINEMETHODNAME(ln);
  if(!x.isPositive()) {
    throwBigRealInvalidArgumentException(method, _T("x<=0"));
  }
  if(!f.isPositive()) {
    throwInvalidToleranceException(method);
  }

  DigitPool *pool = x.getDigitPool();
  if(x == _1) {
    return pool->get0();;
  }

  const BigReal g(PAPCprod(<,f, LNC.c1, pool));

  int k = BigReal::getExpo10(g);

  BigReal y(pool);
  copy(y,x,PAPCprod(<,x, PAPCprod(<,f, LNC.c16,pool),pool));
  
  const int m = BigReal::getExpo10(y);
  y = e(y,-m,pool);
  BigReal z = BigReal::lnEstimate(y);

  int a[100];
  int n = 0;
  while(k < LNC.c18) {
    k /= 2;
    a[n++] = k;
  }
  const BigReal u = PAPCprod(<, LNC.c19, y, pool);
  const BigReal v = PAPCprod(<, LNC.c20, y, pool);
  BigReal r(pool);
  for(int i = n-1; i >= 0; i--) {
    k = a[i];
    r = exp(z,e(u,k,pool));
    z += quot(dif(y,r,e(v,k,pool),pool),r,e(LNC.c21,k,pool),pool);
  }
//  cout << _T("ln loop done!\n"); cout.flush();
  if(g < LNC.c22) {
    r = exp(z,PAPCprod(<,u,g,pool));
  } else {
    r = exp(z,PAPCprod(<,u,LNC.c22,pool));
  }
  if(m == 0) {
    return z + quot(dif(y,r,v*g,pool),r,g*LNC.c23,pool);
  } else {
    return z 
         + quot(dif(y,r,v*g,pool),r,g*LNC.c21,pool) 
         + prod(BigReal(m,pool),BigReal::ln10(PAPCquot(<,PAPCprod(<,LNC.c24,f,pool),BigReal(abs(m),pool),pool)),prod(f,LNC.c25,BIGREAL_0,pool),pool);
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
   ,c4(  1.5)
   ,c5(  BIGREAL_HALF)
   ,c6(  BIGREAL_HALF)
   ,c7(  0.3)
   ,c8(  0.3)
   ,c9(  10.4)
   ,c10( 1.6)
   ,c11( 0.86)
   ,c12( 0.2)
   ,c13( 3.3)
   ,c14( -5)
   ,c15( 0.29)
   ,c16( 5.2)
   ,c17( 0.2)
   ,c18( -3.4)
   ,c19( 2.2)
   ,c20( 0.4)
   ,c21( 7)
   ,c22( 0.03)
   ,c23( 0.08)
   ,c24( 0.1)
   {
   }
};

static const Ln1Constants LN1C;

BigReal ln1(const BigReal &x, const BigReal &f) {
  DEFINEMETHODNAME(ln1);
  if(!x.isPositive()) {
    throwBigRealInvalidArgumentException(method, _T("x<=0"));
  }
  if(!f.isPositive()) {
    throwInvalidToleranceException(method);
  }

  DigitPool *pool = x.getDigitPool();

  if(x == _1) {
    return pool->get0();
  }

  BigReal y(pool);

  if(x < _1) {
    y = quot(_1,x,PAPCquot(>,PAPCprod(>,LN1C.c1,f,pool),x,pool),pool);
  } else {
    copy(y,x,PAPCprod(>,x,PAPCprod(>,LN1C.c2,f,pool),pool));
  }
  BigReal g = PAPCprod(<,LN1C.c3,f,pool);
  BigReal h = _1;
  while(y > LN1C.c4 && g > LN1C.c5) {
    y = sqrt(y,e(LN1C.c6,BigReal::getExpo10(y)/2,pool));
    g = PAPCprod(<,LN1C.c7,g,pool);
    h = h + h;
  }
  while(y > LN1C.c4) {
    y = sqrt(y,e(g,BigReal::getExpo10(y)/2,pool));
    g = PAPCprod(<,LN1C.c8,g,pool);
    h = h + h;
  }
  BigReal s = y - BIGREAL_1;
  if(s > PAPCprod(>,LN1C.c9,g,pool)) {
    g = PAPCprod(<,LN1C.c10,g,pool);
    BigReal jn = PAPCsum(<,LN1C.c14,PAPCsum(<,PAPCprod(<,LN1C.c11,sqrt(-BigReal::getExpo10N(g),LN1C.c12),pool),PAPCprod(<,LN1C.c13,BigReal::getExpo10N(s),pool),pool),pool);
    int j = (jn < _1) ? 0 : getInt(floor(jn));
    for(int i = 1; i <= j; i++) {
      y = sqrt(y,g);
      g = PAPCprod(<,LN1C.c15,g,pool);
    }
    s = y - BIGREAL_1;
    BigReal l(pool);
    if(s < LN1C.c24) {
      l = quot(BigReal::getExpo10N(PAPCprod(<,LN1C.c16,g,pool)),BigReal(BigReal::getExpo10(s)+1,pool),LN1C.c17,pool);
    } else {
      l = PAPCprod(>,LN1C.c18,BigReal::getExpo10N(PAPCprod(<,LN1C.c16,g,pool)),pool);
    }
    if(l > _1) {
      const BigReal u = PAPCquot(<,PAPCprod(<,LN1C.c19,g,pool),l,pool);
      const BigReal w = PAPCprod(<,LN1C.c20,u,pool);
      const BigReal v = PAPCprod(<,LN1C.c21,g,pool);
      BigReal t = s;
      const BigReal z = -s;
      BigReal n = _1;
      while(compareAbs(t,v) > 0) {
        ++n;
        t = prod(t,z,w,pool);
        s += quot(t,n,u,pool);
      }
    }
    if(x < _1) {
      return -prod(h*BigReal::pow2(j),s,f*LN1C.c22,pool);
    } else {
      return  prod(h*BigReal::pow2(j),s,f*LN1C.c23,pool);
    }
  } else {
    throwBigRealException(_T("cond. 1"));
    return pool->get0();
  }
}


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
    ,c2  ( e(BIGREAL_1,-10))
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

BigReal log(const BigReal &base, const BigReal &x, const BigReal &f) { // log(x) base base
  DEFINEMETHODNAME(log);

  DigitPool *pool = x.getDigitPool();

  if(base == _1) {
    throwBigRealInvalidArgumentException(method, _T("base=1"));
  }
  if(!base.isPositive()) {
    throwBigRealInvalidArgumentException(method, _T("base<=0"));
  }
  if(!x.isPositive()) {
    throwBigRealInvalidArgumentException(method, _T("x<=0"));
  }
  if(!f.isPositive()) {
    throwInvalidToleranceException(method);
  }

  BigReal r(pool);

  if(base < LOGC.c10) {
    if(base < LOGC.c9) {
      r = PAPCprod(<,PAPCsum(>,BigReal::getExpo10N(base),_1,pool),LOGC.c1,pool);
    } else {
      BigReal q = (LOGC.c2,pool);
      BigReal z = fabs(dif(base,_1,q,pool));
      while(z < PAPCprod(<,LOGC.c3,q,pool)) {
        q = PAPCprod(<,q,q,pool);
        z = fabs(dif(base,_1,q,pool));
      }
      r = PAPCprod(<,LOGC.c2,z,pool);
    }
  } else {
    r = PAPCprod(<,LOGC.c5, BigReal::getExpo10N(base), pool);
  }

  const BigReal d = PAPCprod(<,PAPCprod(<,LOGC.c6,r,pool),f,pool);
  const BigReal a = ln(x, d);
  return quot(a,ln(base,PAPCquot(<,PAPCprod(<,d,r,pool),PAPCsum(>,fabs(a),PAPCprod(<,LOGC.c7,d,pool),pool),pool)),PAPCprod(<,f,LOGC.c8,pool),pool);
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

BigReal log10(const BigReal &x, const BigReal &f) {
  DEFINEMETHODNAME(log10);
  if(!x.isPositive()) {
    throwBigRealInvalidArgumentException(method, _T("x<=0"));
  }
  if(!f.isPositive()) {
    throwInvalidToleranceException(method);
  }

  DigitPool *pool = x.getDigitPool();

  const BigReal &r = L10C.c5;
  const BigReal  d = PAPCprod(<,f, PAPCprod(<,L10C.c6,r,pool),pool);
  const BigReal  a = ln(x, d);
  return quot(a,BigReal::ln10(PAPCquot(<,PAPCprod(<,d,r,pool),PAPCsum(>,fabs(a),PAPCprod(<,L10C.c7,d,pool),pool),pool)),PAPCprod(<,L10C.c8,f,pool),pool);
}
