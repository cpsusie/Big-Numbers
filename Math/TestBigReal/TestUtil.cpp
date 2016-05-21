#include "stdafx.h"
#include <Date.h>
#include <Console.h>
#include "TestUtil.h"

BigReal getRandom(int length, int exponent, Random &rnd, DigitPool *pool, bool allowZero) {
  BigReal result(pool);
  do {
    result = BigReal::random(length, &rnd, pool);
  } while(!allowZero && result.isZero());
  return (exponent == 0) ? result : e(result, exponent);
}

static Array<BigReal> &generateTestDataExponentialStep(Array<BigReal> &a, const BigReal &from, const BigReal &to, int count) {
  DigitPool    *pool       = from.getDigitPool();
  const BigReal &_1         = pool->get1();
  BigReal        t          = _1;
  const BigReal  fromM1     = from - _1;
  const BigReal  stepFactor = rRoot(to-fromM1,Max(_1,BigReal(count, pool)-_1),20);

  for(int i = 0; i < count; i++, t *= stepFactor) {
    a.add(fromM1 + t);
  }
  return a;
}

static Array<BigReal> &generateTestDataLinearStep(Array<BigReal> &a, const BigReal &from, const BigReal &to, int count) {
  DigitPool    *pool = from.getDigitPool();
  const BigReal  step = rQuot(to - from, BigReal(count, pool), 20, pool);
  int           i    = 1;

  for(BigReal x = from; i < count; x += step, i++) {
    a.add(x);
  }
  a.add(to);
  return a;
}

Array<BigReal> generateTestData(const BigReal &from, const BigReal &to, bool exponentialStep, int count) {
  Array<BigReal> result;
  return exponentialStep ? generateTestDataExponentialStep(result, from,to,count) : generateTestDataLinearStep(result, from,to,count);
}

float getRelativeError32(float x, DigitPool *pool, int *length) {
  BigReal n(x, pool);
  n.assertIsValidBigReal();
  if(length) *length = n.getLength();
  const float f32 = getFloat(n);
  return fabs((f32-x)/x);
}

double getRelativeError64(double x, DigitPool *pool, int *length) {
  BigReal n(x, pool);
  n.assertIsValidBigReal();
  if(length)  *length = n.getLength();
  const double d64    = getDouble(n);
  return fabs((d64-x)/x);
}

double getRelativeError80(const Double80 &x, DigitPool *pool, int *length) {
  BigReal n(x, pool);
  n.assertIsValidBigReal();
  if(length)    *length = n.getLength();
  const Double80 d80    = getDouble80(n);
  const double   error  = getDouble(fabs((d80-x)/x));
  return error;
}

double getRelativeError(const double &x, const BigReal &x0) {
  if(x0.isZero()) {
    return fabs(x);
  } else {
    const double d0 = getDouble(x0);
    return fabs((x-d0)/d0);
  }
}

Double80 getRelativeError(const Double80 &x, const BigReal &x0) {
  if(x0.isZero()) {
    return fabs(x);
  } else {
    const Double80 d0 = getDouble80(x0);
    return fabs((x-d0)/d0);
  }
}

BigReal getRelativeError(const BigReal &x, const BigReal &x0) {
  return x0.isZero() ? fabs(x) : fabs(rQuot(x-x0,x0,20));
}

static const TCHAR *thisFile = _T(__FILE__);

void log(const TCHAR *form,...) {
  const String dir         = FileNameSplitter(thisFile).getDir();
  const String logFileName = FileNameSplitter(getModuleFileName()).setDir(dir).setExtension(_T("log")).getFullPath();
  FILE        *logFile     = MKFOPEN(logFileName,_T("a"));

  va_list argptr;
  va_start(argptr,form);
  String s = vformat(form, argptr);
  va_end(argptr);

  s.replace(_T('\n'),_T(' '));
  _ftprintf(logFile,_T("%s %s\n"),Timestamp().toString().cstr(),s.cstr());
  fclose(logFile);
}

void clearLine() {
  static int w = 0, h = 0;
  static String emptyLine;
  if(w == 0) {
    Console::getWindowSize(w,h);
    emptyLine = spaceString(w-1) + _T("\r");
  }
  tcout << emptyLine;
  tcout.flush();
}
