#include "pch.h"
#include <Math/BigReal/BigRealResourcePool.h>

static Semaphore piLock;

class PiConstants {
public:
  DigitPool *m_digitPool;

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

  BigReal *m_piValue; // cache
  BigReal *m_piError; // cache

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
    ,c16( BigReal::_05)
    ,c17( 0.25)
  {
    m_digitPool = BigRealResourcePool::fetchDigitPool();
    m_digitPool->setName(_T("PI"));
    m_piValue = new BigReal(m_digitPool); TRACE_NEW(m_piValue);
    m_piError = new BigReal(m_digitPool); TRACE_NEW(m_piError);
  }
  ~PiConstants() {
    SAFEDELETE(m_piError);
    SAFEDELETE(m_piValue);
    BigRealResourcePool::releaseDigitPool(m_digitPool);
    m_digitPool = nullptr;
  }
};

static const PiConstants PIC;

BigReal pi(const BigReal &f, DigitPool *digitPool) {
  VALIDATETOLERANCE(f)
  _SELECTDIGITPOOL(f);

  BigReal result(pool);

  piLock.wait();
  try {
    DigitPool *PIP = PIC.m_digitPool;

#define _0 PIP->_0()
#define _1 PIP->_1()
#define PIVALUE (*PIC.m_piValue)
#define PIERROR (*PIC.m_piError)

    if(PIERROR.isZero() || (f < PIERROR)) {
      if(f >= PIC.c1) {
        PIVALUE = PIC.c2;
      } else {

        const BigInt l = floor(APCsum(>,APCprod(>,ln(-BigReal::getExpo10N(APCprod(<,f,PIC.c4,PIP),PIP),PIC.c5,PIP),PIC.c3,PIP),PIC.c6,PIP),PIP);
        const int li = (int)l;
        BigReal w = APCprod(<,APCprod(<,PIC.c7,f,PIP),cut(BigReal::pow2(-li),APC_DIGITS, PIP),PIP); // APCpow(<,c16,l));
        BigReal z = APCprod(<,PIC.c8,APCquot(<,f,l+_1,PIP),PIP);
        BigReal u = APCprod(<,PIC.c9,APCprod(<,z,APCpow(<,PIC.c10,l,PIP),PIP),PIP);
        BigReal v = APCprod(<,PIC.c11,u,PIP);
        BigReal a = _1;
        BigReal b = sqrt(BigReal(PIC.c16,PIP),u,PIP);
        BigReal t = BigReal(PIC.c17,PIP);
        BigReal s(PIP), d(PIP);

        for(int i = 0; i <= li; i++) {
          s = a;
          a = prod(a+b,PIC.c16,u,PIP);
          b = sqrt(prod(b,s,v,PIP),u,PIP);
          d = s - a;
          t -= prod(BigReal::pow2(i),prod(d,d,w,PIP),z,PIP);
        }
        PIVALUE = quot(prod(a,a,APCprod(<,PIC.c12,f,PIP),PIP),t,APCprod(<,PIC.c13,f,PIP),PIP);
      }
      PIERROR = f; // rettet fra APCprod(c14,f,1);
      result = PIVALUE;
      goto End;
    }
    copy(result, PIVALUE,APCprod(<,PIC.c15, f,PIP));
  } catch(...) {
    piLock.notify();
    throw;
  }
End:
  piLock.notify();
  return result;
}
