#include "stdafx.h"
#include "FunctionTest.h"

static int myGetDecimalDigitCount(unsigned long x) {
  int count;
  for(count = 0; x > 0 && count <= 10; count++) {
    x /= 10;
  }
  return count;
}

void testGetDecimalDigitCount(TestStatistic &stat) {
  unsigned int i;

#ifdef TESTTIME
  double startTime = getProcessTime();
  for(i = 0; i < BIGREALBASE; i++) {
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

  for(i = 0; i < BIGREALBASE; i = (i+1) * 2) {
    int ddc1 = myGetDecimalDigitCount(i);
    int ddc2 = BigReal::getDecimalDigitCount(i);
    if(ddc1 != ddc2) {
      ERRLOG << _T("Error in getDecimalDigitCount")                                                            << NEWLINE
             << _T("Failed for i = ") << iparam(12) << i << _T(", ddc1 = ") << ddc1 << _T(", ddc2 = ") << ddc2 << NEWLINE;
      throwException(_T("Error in getDecimalDigitCount"));
    }
  }
  stat.setEndMessageToOk();
}

void testGetDecimalDigits(TestStatistic &stat) {
  DigitPool *pool = stat.getDigitPool();
  for(FullFormatBigReal x(1,pool); x < 200; x *= 1.1) {
    stat.out() << _T("getDecimalDigits(") << x << _T(") = ") << x.getDecimalDigits() << NEWLINE;
  }
}

static int myGetExpo10(unsigned long x) {
  return (x == 0) ? 0 : (int)floor(log10(x));
}

void testGetExpo10(TestStatistic &stat) {
  DigitPool *pool = stat.getDigitPool();
  for(unsigned long i = 0; i < BIGREALBASE; i+=7) {
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

  for(FullFormatBigReal x = e(BigReal(123456789,pool),-20); x < e(pool->get1(),20);  x *= _10) {
    for(int i = -20; i < 20; i++) {
      FullFormatBigReal y = x;
      y.multPow10(i);
//      x.print(); _tprintf(_T(" %3d "),i); y.print(); _tprintf(_T("\n"));
      FullFormatBigReal z = x*e(pool->get1(),i);
      if(y != z) {
        ERRLOG << _T("Error in multPow10")    << NEWLINE
               << _T("x:") << x               << NEWLINE
               << _T("i:") << iparam(12) << i << NEWLINE
               << _T("y:") << y               << NEWLINE
               << _T("z:") << z               << NEWLINE;
        throwException(_T("Error in testMultPow10"));
      }
    }
  }
  stat.setEndMessageToOk();
}

