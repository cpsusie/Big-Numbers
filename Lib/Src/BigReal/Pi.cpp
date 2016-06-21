#include "pch.h"

static Semaphore pi_gate;
static DigitPool pi_pool(PI_DIGITPOOL_ID);

#define PIP (&pi_pool)

class PiConstants {
public:
  const ConstBigReal  c1  ;
  const ConstBigReal  c2  ;
  const ConstBigReal  c3  ;
  const ConstBigReal  c4  ;
  const ConstBigReal  c5  ;
  const ConstBigReal  c6  ;
  const ConstBigReal  c7  ;
  const ConstBigReal  c8  ;
  const ConstBigReal  c9  ;
  const ConstBigReal  c10 ;
  const ConstBigReal  c11 ;
  const ConstBigReal  c12 ;
  const ConstBigReal  c13 ;
  const ConstBigReal  c15 ;
  const BigReal      &c16 ;
  const ConstBigReal  c17 ;

  PiConstants() 
    :c1 ( e(ConstBigReal(11),-12))
    ,c2 ( 3.14159265358979)
    ,c3 ( 1.45)
    ,c4 ( 0.006)
    ,c5 ( 0.01)
    ,c6 ( 0.57)
    ,c7 ( 0.004)
    ,c8 ( 0.002)
    ,c9 ( 18)
    ,c10( 0.97)
    ,c11( 0.02)
    ,c12( 0.002)
    ,c13( 0.02)
    ,c15( 0.01)
    ,c16( BIGREAL_HALF)
    ,c17( 0.25)
  {
  }
};

static const PiConstants PIC;

BigReal pi(const BigReal &f, DigitPool *digitPool) {
  DEFINEMETHODNAME;
  DigitPool *pool = digitPool ? digitPool : f.getDigitPool();
  pi_gate.wait();

  try {
    if(!f.isPositive()) {
      throwInvalidToleranceException(method);
    }

    static BigReal piValue(PIP); // cache
    static BigReal piError(PIP); // cache

    if(piError.isZero() || f < piError) {
      if(f >= PIC.c1) {
        piValue = PIC.c2;
      } else {
        const BigInt l = floor(PAPCsum(>,PAPCprod(>,ln(-BigReal::getExpo10N(PAPCprod(<,f,PIC.c4,PIP)),PIC.c5),PIC.c3,PIP),PIC.c6,PIP));
        const int li = getInt(l);
        BigReal w = PAPCprod(<,PAPCprod(<,PIC.c7,f,PIP),cut(BigReal::pow2(-li),APC_DIGITS, PIP),PIP); // PAPCpow(<,c16,l));
        BigReal z = PAPCprod(<,PIC.c8,PAPCquot(<,f,l+PIP->get1(),PIP),PIP);
        BigReal u = PAPCprod(<,PIC.c9,PAPCprod(<,z,PAPCpow(<,PIC.c10,l,PIP),PIP),PIP);
        BigReal v = PAPCprod(<,PIC.c11,u,PIP);
        BigReal a = PIP->get1();
        BigReal b = sqrt(BigReal(PIC.c16,PIP),u);
        BigReal t = BigReal(PIC.c17,PIP);
        BigReal s(PIP), d(PIP);

        for(int i = 0; i <= li; i++) {
          s = a;
          a = prod(a+b,PIC.c16,u,PIP);
          b = sqrt(prod(b,s,v,PIP),u);
          d = s - a;
          t -= prod(BigReal::pow2(i),prod(d,d,w,PIP),z,PIP);
        }
        piValue = quot(prod(a,a,PAPCprod(<,PIC.c12,f,PIP)),t,PAPCprod(<,PIC.c13,f,PIP),PIP);
      }
      piError = f; // rettet fra PAPCprod(c14,f,1);

      pi_gate.signal();

      return BigReal(piValue, pool);
    }
    BigReal tmp(pool);
    copy(tmp, piValue,PAPCprod(<,PIC.c15, f,PIP));

    pi_gate.signal();
    return tmp;

  } catch(...) {
    pi_gate.signal();
    throw;
  }
}
