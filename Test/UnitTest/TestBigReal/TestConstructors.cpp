#include "stdafx.h"
#include "FunctionTest.h"

static void checkZero(DigitPool *pool) {
  BigReal n(0, pool);
  VALIDATEBIG(n);
  verify(n.isZero());
  verify((int     )n == 0);
  verify((UINT    )n == 0);
  verify((INT64   )n == 0);
  verify((UINT64  )n == 0);
  verify((_int128 )n == 0);
  verify((_uint128)n == 0);
  verify((float   )n == 0);
  verify((double  )n == 0);
  verify((Double80)n == 0);
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
  VALIDATEBIG(n);
  const int x1 = (int)n;

  if(x1 != x) {
    ERRLOG << getFileName() << _T(" line ") << line                              << endl
           << _T("x  :") << iparam(12)   << x                                    << endl
           << _T("x1 :") << iparam(12)   << x1  << _T(" difference:") << x1  - x << endl;
    throwException(_T("%s line %d. x=%d"), getFileName().cstr(), line, x);
  }
}

static void checkExact(int line, UINT x, DigitPool *pool) {
  BigReal n(x, pool);
  VALIDATEBIG(n);
  const UINT x1 = (UINT)n;

  if(x1 != x) {
    ERRLOG << getFileName() << _T(" line ") << line                           << endl
           << _T("x :") << iparam(12)   << x                                  << endl
           << _T("x1:") << iparam(12)   << x1 << _T(" difference:") << x1 - x << endl;
    throwException(_T("%s line %d. x=%u"), getFileName().cstr(), line, x);
  }
}

static void checkExact(int line, INT64 x, DigitPool *pool) {
  BigReal n(x, pool);
  VALIDATEBIG(n);
  const INT64 x1 = (INT64)n;

  if(x1 != x) {
    ERRLOG << getFileName() << _T(" line ") << line                               << endl
           << _T("x  :") << iparam(12)     << x                                   << endl
           << _T("x1 :") << iparam(12)     << x1  << _T(" difference:") << x1 - x << endl;
    throwException(_T("%s line %d. x=%I64d"), getFileName().cstr(), line, x);
  }
}

static void checkExact(int line, UINT64 x, DigitPool *pool) {
  BigReal n(x, pool);
  VALIDATEBIG(n);
  const UINT64 x1 = (UINT64)n;

  if(x1 != x) {
    ERRLOG << getFileName() << _T(" line ") << line                           << endl
           << _T("x :") << iparam(12)   << x                                  << endl
           << _T("x1:") << iparam(12)   << x1 << _T(" difference:") << x1 - x << endl;
    throwException(_T("%s line %d. x=%I64u"), getFileName().cstr(), line, x);
  }
}

static void checkExact(int line, _int128 x, DigitPool *pool) {
  BigReal n(x, pool);
  VALIDATEBIG(n);
  const _int128 x1 = (_int128)n;

  if(x1 != x) {
    ERRLOG << getFileName() << _T(" line ") << line                              << endl
           << _T("x   :") << iparam(35)   << x                                   << endl
           << _T("x1  :") << iparam(35)   << x1 << _T(" difference:") << x1  - x << endl;
    throwException(_T("%s line %d. x=%s"), getFileName().cstr(), line, toString(x).cstr());
  }
}

static void checkExact(int line, _uint128 x, DigitPool *pool) {
  BigReal n(x, pool);
  VALIDATEBIG(n);
  const _uint128 x1 = (_uint128)n;

  if(x1 != x) {
    ERRLOG << getFileName() << _T(" line ") << line                               << endl
           << _T("x :") << iparam(35)       << x                                  << endl
           << _T("x1:") << iparam(35)       << x1 << _T(" difference:") << x1 - x << endl;
    throwException(_T("%s line %d. x=%s"), getFileName().cstr(), line, toString(x).cstr());
  }
}

void testConstructors(TestStatistic &stat) {
  DigitPool *pool = stat.getDigitPool();

  checkZero(pool);

  checkExact(__LINE__, 10      ,pool);
  checkExact(__LINE__,-10      ,pool);
  checkExact(__LINE__,_I32_MIN ,pool);
  checkExact(__LINE__,_I32_MAX ,pool);
  checkExact(__LINE__,(UINT)_UI32_MAX,pool);
  checkExact(__LINE__,-123456780,pool);
  checkExact(__LINE__,-123456780,pool);
  checkExact(__LINE__,-1234567800000000i64 , pool);
  checkExact(__LINE__, 1234567800000000ui64, pool);


  checkExact(__LINE__, _I64_MIN  , pool);
  checkExact(__LINE__, _I64_MAX  , pool);
  checkExact(__LINE__, _UI64_MAX , pool);

  checkExact(__LINE__,_strtoi128( "-1234567800023487623423400000",nullptr,10) , pool);
  checkExact(__LINE__,_strtoi128( "1234567800023487623423400000" ,nullptr,10) , pool);
  checkExact(__LINE__,_strtoui128("1234567800023487623423400000" ,nullptr,10) , pool);
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

  double maxError80 = getRelativeError80(DBL80_MIN,pool);
  verify(maxError80 == 0 );
  maxError80 = getRelativeError80(-DBL80_MIN,pool);
  verify(maxError80 == 0 );
  maxError80 = getRelativeError80(DBL80_MAX,pool);
  verify(maxError80 == 0);
  maxError80 = getRelativeError80(-DBL80_MAX,pool);
  verify(maxError80 == 0);

  for(int i = 0; i < 100000; i++) {
    const int i32 = randInt();
    checkExact(__LINE__,i32, pool);
  }
  for(int i = 0; i < 100000; i++) {
    const UINT ui32 = randInt();
    checkExact(__LINE__,ui32, pool);
  }
  RandomGenerator &rnd = stat.getRandomGenerator();
  for(int i = 0; i < 100000; i++) {
    const INT64 i64 = rnd.nextInt64();
    checkExact(__LINE__,i64, pool);
  }
  for(int i = 0; i < 100000; i++) {
    const UINT64 ui64 = rnd.nextInt64();
    checkExact(__LINE__,ui64, pool);
  }

  stat.setEndMessageToOk();
}

