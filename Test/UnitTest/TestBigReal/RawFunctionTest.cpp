#include "stdafx.h"
#include "FunctionTest.h"

static int myGetDecimalDigitCount(UINT64 x) {
  int count;
  for(count = 0; x > 0; count++) {
    x /= 10;
  }
  return count;
}

void testGetDecimalDigitCount(TestStatistic &stat) {

#if defined(TESTTIME)
  double startTime = getProcessTime();
  for(i = 1; i < BIGREALBASE; i++) {
    int ddc1 = myGetDecimalDigitCount(i);
  }
  double oldTime = getProcessTime() - startTime;

  startTime = getProcessTime();
  for(i = 0; i < BIGREALBASE; i++) {
    int ddc2 = BigReal::getDecimalDigitCount(i);
  }
  double newTime = getProcessTime() - startTime;

  _tprintf(_T("oldTime:%lf. newTime:%lf\n"),oldTime,newTime);
#endif

#define ENDRANGEx86 100000000            // 1e8
#define ENDRANGEx64 10000000000000000000 // 1e19

  for(BRDigitTypex86 i = 1;; i *= 10) {
    int ddc1, ddc2;
    if(i>1) {
      const BRDigitTypex86 im1 = i - 1;
      ddc1 = myGetDecimalDigitCount(       im1);
      ddc2 = BigReal::getDecimalDigitCount(im1);
      verify(ddc1 == ddc2);
    }

    if(i == ENDRANGEx86) break;

    ddc1 = myGetDecimalDigitCount(       i);
    ddc2 = BigReal::getDecimalDigitCount(i);
    verify(ddc1 == ddc2);
  }

  for(BRDigitTypex64 i = 1;;i *= 10) {
    int ddc1, ddc2;
    if(i>1) {
      const BRDigitTypex64 im1 = i - 1;
      ddc1 = myGetDecimalDigitCount(       im1);
      ddc2 = BigReal::getDecimalDigitCount(im1);
      verify(ddc1 == ddc2);
    }

    if(i == ENDRANGEx64) break;

    ddc1 = myGetDecimalDigitCount(       i);
    ddc2 = BigReal::getDecimalDigitCount(i);
    verify(ddc1 == ddc2);
  }

  stat.setEndMessageToOk();
}

void testPow10(TestStatistic &stat) {
  for(UINT i = 0, p=1; i <= 9; i++, p*=10) {
    verify(BigReal::pow10x86(i) == p);
  }
  UINT64 p64 = 1;
  for(UINT i = 0; i <= 19; i++, p64 *= 10) {
    verify(BigReal::pow10x64(i) == p64);
  }
  stat.setEndMessageToOk();
}

void testIsPow10(TestStatistic &stat) {
  for(BRDigitTypex86 i = 0, p = 1; i <= 9; i++, p *= 10) {
    verify(BigReal::isPow10(p) == i   );
    verify(BigReal::isPow10(p + 1) < 0);
    verify(BigReal::isPow10(p - 1) < 0);
  }
  for(BRDigitTypex64 i = 0, p = 1; i <= 19; i++, p *= 10) {
    verify(BigReal::isPow10(p) == i   );
    verify(BigReal::isPow10(p + 1) < 0);
    verify(BigReal::isPow10(p - 1) < 0);
  }
  stat.setEndMessageToOk();
}

void testGetDecimalDigits(TestStatistic &stat) {
  DigitPool *pool = stat.getDigitPool();
  for(FullFormatBigReal x(1,pool); x < 200; x *= 1.1) {
    stat.out() << _T("getDecimalDigits(") << x << _T(") = ") << x.getDecimalDigits() << endl;
  }
}

static int myGetExpo10(BRDigitType x) {
  return (x == 0) ? 0 : (int)floor(log10(x));
}

void testGetExpo10(TestStatistic &stat) {
  DigitPool *pool = stat.getDigitPool();
  for(BRDigitType i = 0; i < BIGREALBASE; i = ((i+1) * 5) / 4) {
    const BigReal ni(i, pool);
    const BRExpoType e10 = BigReal::getExpo10(ni);
    const int        me10 = myGetExpo10(i);
    verify(e10 == me10);
  }
  stat.setEndMessageToOk();
}

void testMultPow10(TestStatistic &stat) {
  DigitPool *pool = stat.getDigitPool();
  const BigReal _10(10, pool);

  for(FullFormatBigReal x = e(BigReal(123456789,pool),-20); x < e(pool->_1(),20);  x *= _10) {
    for(int i = -20; i < 20; i++) {
      FullFormatBigReal y = x;
      y.multPow10(i);
//      x.print(); _tprintf(_T(" %3d "),i); y.print(); _tprintf(_T("\n"));
      FullFormatBigReal z = x*e(pool->_1(),i);
      if(y != z) {
        ERRLOG << _T("Error in multPow10")    << endl
               << _T("x:") << x               << endl
               << _T("i:") << iparam(12) << i << endl
               << _T("y:") << y               << endl
               << _T("z:") << z               << endl;
        THROWTESTERROR();
      }
    }
  }
  stat.setEndMessageToOk();
}

