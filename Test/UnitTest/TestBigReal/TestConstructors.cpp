#include "stdafx.h"
#include "FunctionTest.h"

static void checkZero(DigitPool *pool) {
  BigReal n(0, pool);
  n.assertIsValidBigReal();
  verify(n.isZero());
  verify(getInt(n)      == 0);
  verify(getUint(n)     == 0);
  verify(getInt64(n)    == 0);
  verify(getUint64(n)   == 0);
  verify(getInt128(n)   == 0);
  verify(getUint128(n)  == 0);
  verify(getFloat(n)    == 0);
  verify(getDouble(n)   == 0);
  verify(getDouble80(n) == 0);
}

static String getFileName() {
  static String fileName;
  if(fileName.length() == 0) {
    fileName = FileNameSplitter(__FILE__).getFileName();
  }
  return fileName;
}

static void checkExact(int line, int x, DigitPool *pool) {
  BigReal n(x, pool);
  n.assertIsValidBigReal();
  const int x1 = getInt(n);

  if(x1 != x) {
    ERRLOG << getFileName() << " line " << line                          << NEWLINE
           << "x  :" << iparam(12)   << x                                << NEWLINE
           << "x1 :" << iparam(12)   << x1  << " difference:" << x1  - x << NEWLINE;
    throwException(_T("%s line %d. x=%d"), getFileName().cstr(), line, x);
  }
}

static void checkExact(int line, unsigned int x, DigitPool *pool) {
  BigReal n(x, pool);
  n.assertIsValidBigReal();
  const unsigned int x1 = getUint(n);

  if(x1 != x) {
    ERRLOG << getFileName() << " line " << line                       << NEWLINE
           << "x :" << iparam(12)   << x                              << NEWLINE
           << "x1:" << iparam(12)   << x1 << " difference:" << x1 - x << NEWLINE;
    throwException(_T("%s line %d. x=%u"), getFileName().cstr(), line, x);
  }
}

static void checkExact(int line, __int64 x, DigitPool *pool) {
  BigReal n(x, pool);
  n.assertIsValidBigReal();
  const __int64 x1 = getInt64(n);

  if(x1 != x) {
    ERRLOG << getFileName() << " line " << line                           << NEWLINE
           << "x  :" << iparam(12)     << x                               << NEWLINE
           << "x1 :" << iparam(12)     << x1  << " difference:" << x1 - x << NEWLINE;
    throwException(_T("%s line %d. x=%I64d"), getFileName().cstr(), line, x);
  }
}

static void checkExact(int line, unsigned __int64 x, DigitPool *pool) {
  BigReal n(x, pool);
  n.assertIsValidBigReal();
  const unsigned __int64 x1 = getUint64(n);

  if(x1 != x) {
    ERRLOG << getFileName() << " line " << line                       << NEWLINE
           << "x :" << iparam(12)   << x                              << NEWLINE
           << "x1:" << iparam(12)   << x1 << " difference:" << x1 - x << NEWLINE;
    throwException(_T("%s line %d. x=%I64u"), getFileName().cstr(), line, x);
  }
}

String toString(const _int128 &n) {
  TCHAR buf[200];
  _i128tot(n, buf, 10);
  return buf;
}

String toString(const _uint128 &n) {
  TCHAR buf[200];
  _ui128tot(n, buf, 10);
  return buf;
}

static void checkExact(int line, _int128 x, DigitPool *pool) {
  BigReal n(x, pool);
  n.assertIsValidBigReal();
  const _int128 x1 = getInt128(n);

  if(x1 != x) {
    ERRLOG << getFileName() << " line " << line                          << NEWLINE
           << "x   :" << iparam(35)   << x                               << NEWLINE
           << "x1  :" << iparam(35)   << x1 << " difference:" << x1  - x << NEWLINE;
    throwException(_T("%s line %d. x=%s"), getFileName().cstr(), line, toString(x).cstr());
  }
}

static void checkExact(int line, _uint128 x, DigitPool *pool) {
  BigReal n(x, pool);
  n.assertIsValidBigReal();
  const _uint128 x1 = getUint128(n);

  if(x1 != x) {
    ERRLOG << getFileName() << " line " << line                           << NEWLINE
           << "x :" << iparam(35)       << x                              << NEWLINE
           << "x1:" << iparam(35)       << x1 << " difference:" << x1 - x << NEWLINE;
    throwException(_T("%s line %d. x=%s"), getFileName().cstr(), line, toString(x).cstr());
  }
}

void testConstructors(TestStatistic &stat) {
  DigitPool *pool = stat.getDigitPool();
//  Double80::enableDebugString(true);
//  BigReal::enableDebugString(  true);

  checkZero(pool);

  checkExact(__LINE__, 10      ,pool);
  checkExact(__LINE__,-10      ,pool);
  checkExact(__LINE__,_I32_MIN ,pool);
  checkExact(__LINE__,_I32_MAX ,pool);
  checkExact(__LINE__,(unsigned int)_UI32_MAX,pool);
  checkExact(__LINE__,-123456780,pool);
  checkExact(__LINE__,-123456780,pool);
  checkExact(__LINE__,-1234567800000000i64 , pool);
  checkExact(__LINE__, 1234567800000000ui64, pool);


  checkExact(__LINE__, _I64_MIN  , pool);
  checkExact(__LINE__, _I64_MAX  , pool);
  checkExact(__LINE__, _UI64_MAX , pool);

  checkExact(__LINE__,_int128( _T("-1234567800023487623423400000")) , pool);
  checkExact(__LINE__,_int128( _T("1234567800023487623423400000" )) , pool);
  checkExact(__LINE__,_uint128(_T("1234567800023487623423400000" )) , pool);
  checkExact(__LINE__, _I128_MIN , pool);
  checkExact(__LINE__, _I128_MAX , pool);
  checkExact(__LINE__, _UI128_MAX, pool);

  float maxError32 = getRelativeError32(FLT_MIN,pool);
  verify(maxError32 == 0);
  maxError32 = getRelativeError32(-FLT_MIN,pool);
  verify(maxError32 == 0);
  maxError32 = getRelativeError32(FLT_MAX,pool);
  verify(maxError32 == 0);
  maxError32 = getRelativeError32(-FLT_MAX,pool);
  verify(maxError32 == 0);

  double maxError64 = getRelativeError64(DBL_MIN,pool);
  verify(maxError64 == 0);
  maxError64 = getRelativeError64(-DBL_MIN,pool);
  verify(maxError64 == 0);
  maxError64 = getRelativeError64(DBL_MAX,pool);
  verify(maxError64 == 0);
  maxError64 = getRelativeError64(-DBL_MAX,pool);
  verify(maxError64 == 0);

  double maxError80 = getRelativeError80(Double80::DBL80_MIN,pool);
  verify(maxError80 < 1.09e-19 );
  maxError80 = getRelativeError80(-Double80::DBL80_MIN,pool);
  verify(maxError80 < 1.09e-19 );
  maxError80 = getRelativeError80(Double80::DBL80_MAX,pool);
  verify(maxError80 == 0);
  maxError80 = getRelativeError80(-Double80::DBL80_MAX,pool);
  verify(maxError80 == 0);

  for(int i = 0; i < 100000; i++) {
    const int i32 = randInt();
    checkExact(__LINE__,i32, pool);
  }
  for(int i = 0; i < 100000; i++) {
    const unsigned int ui32 = randInt();
    checkExact(__LINE__,ui32, pool);
  }
  Random &rnd = stat.getRandomGenerator();
  for(int i = 0; i < 100000; i++) {
    const __int64 i64 = rnd.nextInt64();
    checkExact(__LINE__,i64, pool);
  }
  for(int i = 0; i < 100000; i++) {
    const unsigned __int64 ui64 = rnd.nextInt64();
    checkExact(__LINE__,ui64, pool);
  }

  stat.setEndMessageToOk();
}

