#include "stdafx.h"
#include <Date.h>
#include <Console.h>
#include "TestUtil.h"

BigReal getRandom(int length, int exponent, RandomGenerator &rnd, DigitPool *pool, bool allowZero) {
  BigReal result(pool);
  do {
    result = randBigReal(length, rnd, pool);
  } while(!allowZero && result.isZero());
  return (exponent == 0) ? result : e(result, exponent);
}

static Array<BigReal> &generateTestDataExponentialStep(Array<BigReal> &a, const BigRealInterval &interval, DigitPool *pool, UINT count) {
  const BigReal &_1         = pool->_1();
  BigReal        t          = _1;
  const BigReal  fromM1(interval.getFrom() - _1, pool);
  const BigReal  to(    interval.getTo()       , pool);

  const BigReal  stepFactor = rRoot(to-fromM1,dmax(_1,BigReal(count, pool)-_1),20);

  for(UINT i = 0; i < count; i++, t *= stepFactor) {
    a.add(fromM1 + t);
  }
  return a;
}

static Array<BigReal> &generateTestDataLinearStep(Array<BigReal> &a, const BigRealInterval &interval, DigitPool *pool, UINT count) {
  const BigReal from(interval.getFrom(), pool);
  const BigReal to(  interval.getTo()  , pool);
  const BigReal step = rQuot(to - from, BigReal(count, pool), 20, pool);
  UINT          i    = 1;

  for(BigReal x = from; i < count; x += step, i++) {
    a.add(x);
  }
  a.add(to);
  return a;
}

Array<BigReal> generateTestData(const TestInterval &interval, DigitPool *pool, int count) {
  Array<BigReal> result;
  return interval.isExponentialStep() ? generateTestDataExponentialStep(result, interval, pool,count) : generateTestDataLinearStep(result, interval, pool, count);
}

float getRelativeError32(float x, DigitPool *pool, size_t *length) {
  BigReal n(x, pool);
  VALIDATEBIG(n);
  if(length) *length = n.getLength();
  const float f32    = (float)n;
  return fabs((f32-x)/x);
}

double getRelativeError64(double x, DigitPool *pool, size_t *length) {
  BigReal n(x, pool);
  VALIDATEBIG(n);
  if(length)  *length = n.getLength();
  const double d64    = (double)n;
  return fabs((d64-x)/x);
}

double getRelativeError80(const Double80 &x, DigitPool *pool, size_t *length) {
  BigReal n(x, pool);
  VALIDATEBIG(n);
  if(length)    *length = n.getLength();
  const Double80 d80    = (Double80)n;
  const double   error  = (double)fabs((d80-x)/x);
  return error;
}

double getRelativeError(const double &x, const BigReal &x0) {
  if(x0.isZero()) {
    return fabs(x);
  } else {
    const double d0 = (double)x0;
    return fabs((x-d0)/d0);
  }
}

Double80 getRelativeError(const Double80 &x, const BigReal &x0) {
  if(x0.isZero()) {
    return fabs(x);
  } else {
    const Double80 d0 = (Double80)x0;
    return fabs((x-d0)/d0);
  }
}

BigReal getRelativeError(const BigReal &x, const BigReal &x0) {
  return x0.isZero() ? fabs(x) : fabs(rQuot(x-x0,x0,20));
}

static const TCHAR *thisFile = __TFILE__;

const String &getSourceDir() {
  static String sourceDir;
  if (sourceDir.length() == 0) {
    sourceDir = FileNameSplitter(thisFile).getDir();
  }
  return sourceDir;
}

const String &getArchitecture() {
  static String architecture;
  if (architecture.length() == 0) {
#if defined(IS32BIT)
    architecture = _T("x86");
#else
    architecture = _T("x64");
#endif
  }
  return architecture;
}

const String &getCompileMode() {
  static String compileMode;
#if defined(_DEBUG)
  compileMode = _T("Debug");
#else
  compileMode = _T("Release");
#endif
  return compileMode;
}

const String getCompileArchitectureSignatureString() {
  return format(_T("%s(%s)"), getCompileMode().cstr(), getArchitecture().cstr());
}

const String getSignatureSubDir() {
  return FileNameSplitter::getChildName(getSourceDir(), getCompileArchitectureSignatureString());
}

static FILE  *logFile = NULL;

void log(_In_z_ _Printf_format_string_ TCHAR const * const format,...) {
  if (logFile == NULL) {
    const String logFileName = FileNameSplitter(getModuleFileName()).setDir(getSourceDir()).setExtension(_T("log")).getFullPath();
    logFile = MKFOPEN(logFileName, _T("a"));
  }

  va_list argptr;
  va_start(argptr,format);
  String s = vformat(format, argptr);
  va_end(argptr);

  s.replace(_T('\n'),_T(' '));
  _ftprintf(logFile,_T("%s %s\n"),Timestamp().toString().cstr(),s.cstr());
  fflush(logFile);
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
