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
  const int      i32 = getInt(n);

  if(i32 != x) {
    ERRLOG << getFileName() << " line " << line                             << NEWLINE
           << "x   :" << iparam(12)   << x                                  << NEWLINE
           << "i32 :" << iparam(12)   << i32  << " difference:" << i32  - x << NEWLINE;
    throwException(_T("%s line %d. x=%d"), getFileName().cstr(), line, x);
  }
}

static void checkExact(int line, unsigned int x, DigitPool *pool) {
  BigReal n(x, pool);
  n.assertIsValidBigReal();
  const unsigned int     ui32 = getUint(n);

  if(ui32 != x) {
    ERRLOG << getFileName() << " line " << line                             << NEWLINE
           << "x   :" << iparam(12)   << x                                  << NEWLINE
           << "ui32:" << iparam(12)   << ui32 << " difference:" << ui32 - x << NEWLINE;
    throwException(_T("%s line %d. x=%u"), getFileName().cstr(), line, x);
  }
}

static void checkExact(int line, __int64 x, DigitPool *pool) {
  BigReal n(x, pool);
  n.assertIsValidBigReal();
  const __int64          i64  = getInt64(n);

  if(i64 != x) {
    ERRLOG << getFileName() << " line " << line                             << NEWLINE
           << "x   :" << iparam(12)   << x                                  << NEWLINE
           << "i64 :" << iparam(12)   << i64  << " difference:" << i64  - x << NEWLINE;
    throwException(_T("%s line %d. x=%I64d"), getFileName().cstr(), line, x);
  }
}

static void checkExact(int line, unsigned __int64 x, DigitPool *pool) {
  BigReal n(x, pool);
  n.assertIsValidBigReal();
  const unsigned __int64 ui64 = getUint64(n);

  if(ui64 != x) {
    ERRLOG << getFileName() << " line " << line                             << NEWLINE
           << "x   :" << iparam(12)   << x                                  << NEWLINE
           << "ui64:" << iparam(12)   << ui64 << " difference:" << ui64 - x << NEWLINE;
    throwException(_T("%s line %d. x=%I64u"), getFileName().cstr(), line, x);
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
  checkExact(__LINE__, 1234567800000000ui64,pool);


  checkExact(__LINE__,_I64_MIN ,pool);
  checkExact(__LINE__,_I64_MAX ,pool);
  checkExact(__LINE__,_UI64_MAX,pool);

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

