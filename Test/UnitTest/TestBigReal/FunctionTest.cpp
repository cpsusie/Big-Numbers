#include "stdafx.h"
#include "resource.h"
#include <Console.h>
#include <ByteMemoryStream.h>
#include <CompressFilter.h>
#include <MathUtil.h>
#include <Math/MRisprime.h>
#include "FunctionTest.h"

void FunctionTest1ArgND64D80::runTest(int threadId, DigitPool *pool) {
  FullFormatBigReal maxTolerance = e(pool->_1(),-MAXDIGITS-6);
  Array<BigReal>    testData     = generateTestData(m_xInterval, pool);
  TestStatistic     stat(threadId, m_functionName, pool, XF, testData.size() * MAXDIGITS / DIGITSTEP, maxTolerance);

  for(size_t i = 0; i < testData.size(); i++) {
    const BigReal &x           = testData[i];
    const BigReal  exactResult = m_f1 ? m_f1(x, maxTolerance) : m_f1pool(x, maxTolerance, pool);

    if(m_f1_64 != NULL) {
      checkBigRealDouble64(m_functionName, m_f1_64, x, exactResult);
    }

    if(m_f1_80 != NULL) {
      checkBigRealDouble80(m_functionName, m_f1_80, x, exactResult);
    }

    for(int p = 0; p <= MAXDIGITS; p += DIGITSTEP) {
      if(stat.isTimeToPrint()) {
        stat.printLoopMessage(_T("i:%-4d[0..%d], p:%-4d[0..%d]")
                             ,i, testData.size()-1
                             ,p, MAXDIGITS);
      }
      if(m_f1) {
        stat.checkError(m_f1    , x, stat.getRandomTolerance(-p), exactResult);
      } else {
        stat.checkError(m_f1pool, x, stat.getRandomTolerance(-p), exactResult);
      }
    }
  }
}

void FunctionTest2ArgND64D80::runTest(int threadId, DigitPool *pool) {
  const FullFormatBigReal maxTolerance   = e(pool->_1(),-MAXDIGITS-6);
  const Array<BigReal>    xTestData      = generateTestData(m_xInterval, pool, 10);
  const Array<BigReal>    yTestData      = generateTestData(m_yInterval, pool, 10);
  TestStatistic          stat(threadId, m_functionName, pool, XYF, xTestData.size() * yTestData.size() * MAXDIGITS / DIGITSTEP, maxTolerance);

  for(size_t i = 0; i < xTestData.size(); i++) {
    const BigReal &x = xTestData[i];
    for(size_t j = 0; j < yTestData.size(); j++) {
      const BigReal &y           = yTestData[j];
      const BigReal  exactResult = m_f2 ? m_f2( x, y, maxTolerance) : m_f2pool(x, y, maxTolerance, pool);

      if(m_f2_64 != NULL) {
        checkBigRealDouble64(m_functionName, m_f2_64, x, y, exactResult);
      }

      if(m_f2_80 != NULL) {
        checkBigRealDouble80(m_functionName, m_f2_80, x, y, exactResult);
      }

      for(int p = 0; p <= MAXDIGITS; p += DIGITSTEP) {
        if(stat.isTimeToPrint()) {
          stat.printLoopMessage(_T("i:%-4d[0..%d], j:%-4d[0..%d], p:%-4d[0..%d]")
                               ,i, xTestData.size()-1
                               ,j, yTestData.size()-1
                               ,p, MAXDIGITS);
        }
        if(m_f2) {
          stat.checkError(m_f2, x, y, stat.getRandomTolerance(-p), exactResult);
        } else {
          stat.checkError(m_f2pool, x, y, stat.getRandomTolerance(-p), exactResult);
        }
      }
    }
  }
}

void BigRealExactBinaryOperatorTest::runTest(int threadId, DigitPool *pool) {
  const int              length         = 30;
  TestStatistic          stat(threadId, m_functionName, pool, XY, 4 * sqr((MAXSCALE - MINSCALE + 1)) / (sqr(SCALESTEP)*DIGITSTEP));

  for(int xsign = 1; xsign >= -1; xsign -= 2) {
    for(int ysign = 1; ysign >= -1; ysign -= 2) {
      for(int xScale = MINSCALE; xScale <= MAXSCALE; xScale += SCALESTEP) {
        for(int yScale = MINSCALE; yScale <= MAXSCALE; yScale += SCALESTEP) {
          const BigReal x      = (xsign == 1) ? stat.getRandom(length, xScale) : -stat.getRandom(length, xScale);;
          const BigReal y      = (ysign == 1) ? stat.getRandom(length, yScale) : -stat.getRandom(length, yScale);
          const BigReal result = m_op2(x, y);

          if(stat.isTimeToPrint()) {
            stat.printLoopMessage(_T("sign(x):%c, xScale:%-4d[%d..%d], sign(y):%c, yScale:%-4d[%d..%d]")
                                 ,(xsign>0)?'+':'-', xScale, MINSCALE, MAXSCALE
                                 ,(ysign>0)?'+':'-', yScale, MINSCALE, MAXSCALE
                                 );
          }
          if(!m_check(x, y, result)) {
            ERRLOG << _T("Error in ") << m_functionName            << endl
                   << _T("x:"       ) << FullFormatBigReal(x)      << endl
                   << _T("y:"       ) << FullFormatBigReal(y)      << endl
                   << _T("result:"  ) << FullFormatBigReal(result) << endl;
            throwException(_T("Error in %s"), m_functionName.cstr());
          }
        }
      }
    }
  }
  stat.setEndMessageToOk();
}

void OperatorTest2ArgND64D80Pool::specialTest(int threadId, DigitPool *pool) {
  BigReal x(_T("8.356696602362382269528160105890000e+009"), pool);
  BigReal y(_T("8.356810893959301561557443735160000e+009"), pool);
// test dif on these numbers will give a relative error about 6.13e-12 wihch is > 1e-12. but because the 2 numbers are almost equal
// We will loose precsion when first converted to double and then subtracted. In fact the relative error can become very big
// when 2 almost equal floating-point numbers are subtracted!!. So we have to accept an error > 1e-12 now and then

  double x64 = getDouble(x);
  double y64 = getDouble(y);

  BigReal x1 = x64;
  BigReal y1 = y64;

  double x1_64 = getDouble(x1);
  double y1_64 = getDouble(y1);

  bool xeq = x1_64 == x64;
  bool yeq = y1_64 == y64;

  const FullFormatBigReal maxTolerance   = e(pool->_1(),-MAXDIGITS-6);
  TestStatistic           stat(threadId, m_functionName, pool, XYF, MAXDIGITS, maxTolerance);

  const BigReal exactResult = m_f2Pool(x, y, maxTolerance, pool);

  for(int p = 0; p <= MAXDIGITS; p += DIGITSTEP) {

    if(m_f2_64 != NULL) {
      checkBigRealDouble64(m_functionName, m_f2_64, x, y, exactResult);
    }

    if(m_f2_80 != NULL) {
      checkBigRealDouble80(m_functionName, m_f2_80, x, y, exactResult);
    }

    stat.checkError(m_f2Pool, x, y, stat.getRandomTolerance(-p), exactResult);
  }
}


void OperatorTest2ArgND64D80Pool::runTest(int threadId, DigitPool *pool) {
//  specialTest(threadId, pool); // se comment if you get an error
//  return;

  const int               length         = 30;
  const FullFormatBigReal maxTolerance   = e(pool->_1(),-MAXDIGITS-6);
  TestStatistic           stat(threadId, m_functionName, pool, XYF, (sqr((MAXSCALE - MINSCALE + 1)) * MAXDIGITS) / (sqr(SCALESTEP)*DIGITSTEP), maxTolerance);

  for(int xScale = MINSCALE; xScale <= MAXSCALE; xScale += SCALESTEP) {
    for(int yScale = MINSCALE; yScale <= MAXSCALE; yScale += SCALESTEP) {
      const BigReal x           = stat.getRandom(length, xScale);
      const BigReal y           = stat.getRandom(length, yScale);
      const BigReal exactResult = m_f2Pool(x, y, maxTolerance, pool);

      for(int p = 0; p <= MAXDIGITS; p += DIGITSTEP) {
        if(stat.isTimeToPrint()) {
          stat.printLoopMessage(_T("xScale:%-4d[%d..%d], yScale:%-4d[%d..%d], p:%-4d[0..%d]")
                               ,xScale, MINSCALE, MAXSCALE
                               ,yScale, MINSCALE, MAXSCALE
                               ,p, MAXDIGITS
                               );
        }

        if(m_f2_64 != NULL) {
          checkBigRealDouble64(m_functionName, m_f2_64, x, y, exactResult);
        }

        if(m_f2_80 != NULL) {
          checkBigRealDouble80(m_functionName, m_f2_80, x, y, exactResult);
        }

        stat.checkError(m_f2Pool, x, y, stat.getRandomTolerance(-p), exactResult);
      }
    }
  }
}

void FunctionTest1ArgRelative::runTest(int threadId, DigitPool *pool) {
  const Array<BigReal> testData = generateTestData(m_xInterval, pool);
  TestStatistic        stat(threadId, m_functionName, pool, XD, testData.size() * MAXDIGITS / DIGITSTEP, MAXDIGITS);

  for(size_t i = 0; i < testData.size(); i++) {
    const BigReal &x           = testData[i];
    const BigReal  exactResult = m_rf1 ? m_rf1(x, MAXDIGITS) : m_rf1pool(x, MAXDIGITS, pool);

    for(int digits = 1; digits <= MAXDIGITS; digits += DIGITSTEP) {
      if(stat.isTimeToPrint()) {
        stat.printLoopMessage(_T("i:%-4d[0..%d], digits:%-4d[0..%d]")
                             ,i, testData.size()-1
                             ,digits, MAXDIGITS);
      }
      if(m_rf1) {
        stat.checkError(m_rf1, x, digits, exactResult);
      } else {
        stat.checkError(m_rf1pool, x, digits, exactResult);
      }
    }
  }
}

void FunctionTest2ArgRelative::runTest(int threadId, DigitPool *pool) {
  const Array<BigReal> xTestData      = generateTestData(m_xInterval, pool, 10);
  const Array<BigReal> yTestData      = generateTestData(m_yInterval, pool, 10);
  TestStatistic        stat(threadId, m_functionName, pool, XYD, xTestData.size() * yTestData.size() * MAXDIGITS / DIGITSTEP, MAXDIGITS);

  for(size_t i = 0; i < xTestData.size(); i++) {
    const BigReal &x = xTestData[i];
    for(size_t j = 0; j < yTestData.size(); j++) {
      const BigReal &y           = yTestData[j];
      const BigReal  exactResult = m_rf2 ? m_rf2(x, y, MAXDIGITS) : m_rf2pool(x, y, MAXDIGITS, pool);

      for(int digits = 0; digits <= MAXDIGITS; digits += DIGITSTEP) {
        if(stat.isTimeToPrint()) {
          stat.printLoopMessage(_T("i:%-4d[0..%d], j:%-4d[0..%d], digits:%-4d[0..%d]")
                               ,i, xTestData.size()-1
                               ,j, yTestData.size()-1
                               ,digits, MAXDIGITS);
        }
        if(m_rf2) {
          stat.checkError(m_rf2, x, y, digits, exactResult);
        } else {
          stat.checkError(m_rf2pool, x, y, digits, exactResult);
        }
      }
    }
  }
}

void OperatorTest2ArgRelative::runTest(int threadId, DigitPool *pool) {
  const int     length = 30;
  TestStatistic stat(threadId, m_functionName, pool, XYD, (sqr((MAXSCALE - MINSCALE + 1)) * MAXDIGITS) / (sqr(SCALESTEP)*DIGITSTEP), MAXDIGITS);

  for(int xScale = MINSCALE; xScale <= MAXSCALE; xScale += SCALESTEP) {
    for(int yScale = MINSCALE; yScale <= MAXSCALE; yScale += SCALESTEP) {
      const BigReal x           = stat.getRandom(length, xScale);
      const BigReal y           = stat.getRandom(length, yScale);
      const BigReal exactResult = m_rf2Pool(x, y, MAXDIGITS, pool);

      for(int digits = 1; digits <= MAXDIGITS; digits += DIGITSTEP) {
        if(stat.isTimeToPrint()) {
          stat.printLoopMessage(_T("xScale:%-4d[%d..%d], yScale:%-4d[%d..%d], digits:%-4d[0..%d]")
                               ,xScale, MINSCALE, MAXSCALE
                               ,yScale, MINSCALE, MAXSCALE
                               ,digits, MAXDIGITS);
        }
        stat.checkError(m_rf2Pool, x, y, digits, exactResult);
      }
    }
  }
}

void testRandBigReal(TestStatistic &stat) {
  DigitPool         *pool = stat.getDigitPool();
  RandomGenerator   &rnd  = stat.getRandomGenerator();
  const size_t       n    = 1000;

  for(size_t digits = 1; digits < n; digits++) {
    if(stat.isTimeToPrint()) stat.printLoopMessage(_T("Progress:%.2lf%%"), PERCENT(digits, n));
    BigReal from = randBigReal(digits, rnd, pool);
    verify(BigReal::_0 <= from);
    verify(from < BigReal::_1);
    const size_t fdd = from.getDecimalDigits();
    verify(fdd <= digits);
    BigReal to(from); to.multPow10(2);
    if(rnd.nextBool()) from = -from;
    for(int i = 0; i < 50; i++) {
      const size_t  rdigits = digits + rnd.nextInt(20);
      const BigReal r       = randBigReal(from, to, rdigits, rnd, pool);
      verify(from <= r);
      verify(r <= to);
      const size_t rdd = r.getDecimalDigits();
      verify(rdd <= rdigits);
    }
  }
  stat.setEndMessageToOk();
}

void testRandBigInt(TestStatistic &stat) {
  DigitPool         *pool = stat.getDigitPool();
  RandomGenerator   &rnd = stat.getRandomGenerator();
  const size_t       n   = 1000;

  for(size_t digits = 1; digits < n; digits++) {
    if(stat.isTimeToPrint()) stat.printLoopMessage(_T("Progress:%.2lf%%"), PERCENT(digits, n));
    const BigInt r = randBigInt(digits, rnd, pool);
    verify(r.getDecimalDigits() <= digits);
    verify(r >= BigReal::_0);
    for(int i = 0; i < 10; i++) {
      const BigInt r1 = randBigInt(r, rnd, pool);
      if(!r._isnormal()) {
        verify(isnan(r1));
        break;
      } else {
        verify(r1 >= BigReal::_0);
        verify(r1 < r);
      }
    }
  }
  stat.setEndMessageToOk();
}

void testRandBigRational(TestStatistic &stat) {
  DigitPool         *pool = stat.getDigitPool();
  RandomGenerator   &rnd = stat.getRandomGenerator();
  UINT               loop = 0, maxLoopCount = 123;

  for(BigInt maxden(30,pool); maxden <= BigReal::_i128_max; maxden.multiply2()) {
    if(stat.isTimeToPrint()) stat.printLoopMessage(_T("Progress:%.2lf%%"), PERCENT(loop, maxLoopCount));
    BigRational low = randBigRational(maxden, rnd);
    verify(BigRational::_0       <= low           );
    verify(low                   < BigRational::_1);
    verify(low.getDenominator()  <= maxden        );

    BigRational high = randBigRational(maxden, rnd);
    verify(BigRational::_0       <= high          );
    verify( high                 < BigRational::_1);
    verify(high.getDenominator() <= maxden        );

    low  *= BigInt(randInt(-30, 30, rnd), pool);
    high *= BigInt(randInt(-30, 30, rnd), pool);
    if(high < low) {
      swap(low, high);
    }
    for(int i = 0; i < 100; i++) {
      const BigInt      maxScale = randBigInt(10, rnd, pool);
      const BigRational r        = randBigRational(low, high, maxScale, rnd);
      verify((low <= r) && (r <= high));
    }
  }
  stat.setEndMessageToOk();
}

void testQuot3(TestStatistic &stat) {
  DigitPool         *pool           = stat.getDigitPool();
  const int          length         = 50;
  const unsigned int totalTestCount = (sqr((MAXSCALE - MINSCALE + 1)) * MAXDIGITS) / (sqr(SCALESTEP)*DIGITSTEP);

  stat.setTotalTestCount(totalTestCount);

  for(int xScale = MINSCALE; xScale <= MAXSCALE; xScale += SCALESTEP) {
    for(int yScale = MINSCALE; yScale <= MAXSCALE; yScale += SCALESTEP) {
      for(int digits = 1; digits <= MAXDIGITS; digits += DIGITSTEP) {

        if(stat.isTimeToPrint()) {
          stat.printLoopMessage(_T("xScale:%-4d[%d..%d], yScale:%-4d[%d..%d], digits:%-4d[0..%d]")
                               ,xScale, MINSCALE, MAXSCALE
                               ,yScale, MINSCALE, MAXSCALE
                               ,digits, MAXDIGITS);
        }

        const BigReal x         = stat.getRandom(length, xScale, false);
        const BigReal y         = stat.getRandom(length, yScale, false);
        const BigReal tolerance = e(pool->_1(), -digits, pool);
        const BigReal qn        = BigReal::quotNewton(  x, y, tolerance, pool);
        const BigReal ql32      = BigReal::quotLinear32(x, y, tolerance, pool);
        const BigReal ql64      = BigReal::quotLinear64(x, y, tolerance, pool);

        if(fabs(qn - ql32) > tolerance || fabs(qn - ql64) > tolerance || fabs(ql32 - ql64) > tolerance) {
          ERRLOG << _T("Error in testQuot3")                                                             << endl
                 << _T("x        :")                                   << FullFormatBigReal(x)           << endl
                 << _T("y        :")                                   << FullFormatBigReal(y)           << endl
                 << _T("Tolerance:")                                   << tolerance                      << endl
                 << _T("quotNewton(x,y,tolerance)  :")                 << FullFormatBigReal(qn)          << endl
                 << _T("quotLinear32(x,y,tolerance):")                 << FullFormatBigReal(ql32)        << endl
                 << _T("quotLinear64(x,y,tolerance):")                 << FullFormatBigReal(ql64)        << endl
                 << _T("quotNewton - quotLinear32  :")                 << FullFormatBigReal(qn - ql32)   << endl
                 << _T("quotNewton - quotLinear64  :")                 << FullFormatBigReal(qn - ql64)   << endl
                 << _T("quotLinear32 - quotLinear64:")                 << FullFormatBigReal(ql32 - ql64) << endl;
          THROWTESTERROR();
        }
      }
    }
  }
  stat.setEndMessageToOk();
}

static BigReal loadBigReal(int resId, DigitPool *pool) {
  Packer p;
  p.read(DecompressFilter(ByteMemoryInputStream(ByteArray().loadFromResource(resId, _T("BIGREAL")))));
  BigReal result(pool);
  p >> result;
  return result;
}

void testPi(TestStatistic &stat) {
  DigitPool    *pool        = stat.getDigitPool();

  static const char *pi2000Str =
    "3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679821480865132823066470938"
    "446095505822317253594081284811174502841027019385211055596446229489549303819644288109756659334461284756482337867831652712019091"
    "456485669234603486104543266482133936072602491412737245870066063155881748815209209628292540917153643678925903600113305305488204"
    "665213841469519415116094330572703657595919530921861173819326117931051185480744623799627495673518857527248912279381830119491298"
    "336733624406566430860213949463952247371907021798609437027705392171762931767523846748184676694051320005681271452635608277857713"
    "427577896091736371787214684409012249534301465495853710507922796892589235420199561121290219608640344181598136297747713099605187"
    "072113499999983729780499510597317328160963185950244594553469083026425223082533446850352619311881710100031378387528865875332083"
    "814206171776691473035982534904287554687311595628638823537875937519577818577805321712268066130019278766111959092164201989380952"
    "572010654858632788659361533818279682303019520353018529689957736225994138912497217752834791315155748572424541506959508295331168"
    "617278558890750983817546374649393192550604009277016711390098488240128583616035637076601047101819429555961989467678374494482553"
    "797747268471040475346462080466842590694912933136770289891521047521620569660240580381501935112533824300355876402474964732639141"
    "992726042699227967823547816360093417216412199245863150302861829745557067498385054945885869269956909272107975093029553211653449"
    "872027559602364806654991198818347977535663698074265425278625518184175746728909777727938000816470600161452491921732172147723501"
    "414419735685481613611573525521334757418494684385233239073941433345477624168625189835694855620992192221842725502542568876717904"
    "946016534668049886272327917860857843838279679766814541009538837863609506800642251252051173929848960841284886269456042419652850"
    "222106611863067442786220391949450471237137869609563643719172874677646575739624138908658326459958133904780275901";

  const int     step        = 5;
  const int     minDecimals = 1;
  const int     maxDecimals = 2000;
  const BigReal pi2000(pi2000Str, pool);

  stat.setTotalTestCount((maxDecimals - minDecimals) / step);

  for(int digits = minDecimals; digits < maxDecimals; digits += step) {
    const BigReal tolerance = e(pool->_1(), -digits, pool);
    const BigReal result    = pi(tolerance, pool);

    if(stat.isTimeToPrint()) {
      stat.printLoopMessage(_T("Tolerance:%s      "), stat.toString(tolerance).cstr());
    }

    const FullFormatBigReal error = fabs(result-pi2000);
    if(error > tolerance) {
      Console::setCursorPos(1,5);
      ERRLOG << _T("Error in pi"   )                                              << endl
             << _T("Tolerance    :") << tolerance                                 << endl
             << _T("pi(tolerance):") << FullFormatBigReal(result)                 << endl
             << _T("Difference = fabs(pi(tolerance)-pi2000):") << nparam << error << endl;
      THROWTESTERROR();
    } else {
      stat.update(error, tolerance);
    }
  }

  { const BigReal           pi100K    = loadBigReal(IDR_BIGREAL_PI100K, pool);
    const BigReal           tolerance = e(pool->_1(), -100000);
    const BigReal           piTest    = pi(tolerance);
    const FullFormatBigReal error     = fabs(piTest - pi100K);
    if(error > tolerance) {
      Console::setCursorPos(1, 5);
      ERRLOG << _T("Error in pi100K") << endl
             << _T("Tolerance    :") << tolerance << endl
             << _T("Difference = fabs(pi(tolerance)-pi100K):") << nparam << error << endl;
      THROWTESTERROR();
    }
  }
}

void testIntSum(TestStatistic &stat) { // tester BigInt sum
  DigitPool       *pool = stat.getDigitPool();
  RandomGenerator &rnd  = stat.getRandomGenerator();
  const ULONG      n    = 1000;
  stat.setTotalTestCount(n);
  for(ULONG i = 0; i < n; i++) {
    const __int64  x64  = rnd.nextInt64();
    const __int64  y64  = rnd.nextInt64();
    const  _int128 z128 = (_int128)x64 + y64;
    if(stat.isTimeToPrint()) {
      stat.printLoopMessage(_T("i:%10u"), i);
    }
    const BigInt  X(x64, pool), Y(y64, pool), Za = X + Y, Zb = sum(X,Y);
    const _int128 Z128a = getInt128(Za);
    const _int128 Z128b = getInt128(Zb);
    if((Z128a != z128) || (Z128b != z128)) {
      ERRLOG << _T("Error in int sum")       << endl
             << _T("X            :") << X    << endl
             << _T("Y            :") << Y    << endl
             << _T("Za = X+Y     :") << Za   << endl
             << _T("Zb = sum(X,Y):") << Zb   << endl
             << _T("x64+y64      :") << z128 << endl;
      THROWTESTERROR();
    }
  }
  stat.setEndMessageToOk();
}

void testIntDif(TestStatistic &stat) { // tester BigInt dif
  DigitPool       *pool = stat.getDigitPool();
  RandomGenerator &rnd  = stat.getRandomGenerator();
  const ULONG      n    = 1000;
  stat.setTotalTestCount(n);
  for(ULONG i = 0; i < n; i++) {
    const __int64  x64  = rnd.nextInt64();
    const __int64  y64  = rnd.nextInt64();
    const  _int128 z128 = (_int128)x64 - y64;
    if(stat.isTimeToPrint()) {
      stat.printLoopMessage(_T("i:%10u"), i);
    }
    const BigInt  X(x64, pool), Y(y64, pool), Za = X - Y, Zb = dif(X,Y);
    const _int128 Z128a = getInt128(Za);
    const _int128 Z128b = getInt128(Zb);
    if((Z128a != z128) || (Z128b != z128)) {
      ERRLOG << _T("Error in int dif")       << endl
             << _T("X            :") << X    << endl
             << _T("Y            :") << Y    << endl
             << _T("Za = X-Y     :") << Za   << endl
             << _T("Zb = dif(X,Y):") << Zb   << endl
             << _T("x64-y64      :") << z128 << endl;
      THROWTESTERROR();
    }
  }
  stat.setEndMessageToOk();
}

void testIntProd(TestStatistic &stat) { // tester BigInt prod
  DigitPool       *pool = stat.getDigitPool();
  RandomGenerator &rnd  = stat.getRandomGenerator();
  const ULONG      n    = 1000;
  stat.setTotalTestCount(n);
  for(ULONG i = 0; i < n; i++) {
    __int64  x64  = rnd.nextInt64(_I64_MAX/4);
    __int64  y64  = rnd.nextInt64(_I64_MAX/4);
    if(rnd.nextBool()) x64 = -x64;
    if(rnd.nextBool()) y64 = -y64;
    const  _int128 z128 = (_int128)x64 * y64;
    if(stat.isTimeToPrint()) {
      stat.printLoopMessage(_T("i:%10u"), i);
    }

    const BigInt  X(x64, pool), Y(y64, pool), Za = X * Y, Zb = prod(X,Y);
    const _int128 Z128a = getInt128(Za);
    const _int128 Z128b = getInt128(Zb);
    if((Z128a != z128) || (Z128b != z128)) {
      ERRLOG << _T("Error in int prod")       << endl
             << _T("X             :") << X    << endl
             << _T("Y             :") << Y    << endl
             << _T("Za = X*Y      :") << Za   << endl
             << _T("Zb = prod(X,Y):") << Zb   << endl
             << _T("x64*y64       :") << z128 << endl;
      THROWTESTERROR();
    }
  }
  stat.setEndMessageToOk();
}

void testIntQuot(TestStatistic &stat) { // tester BigInt quot
  DigitPool       *pool = stat.getDigitPool();
  RandomGenerator &rnd  = stat.getRandomGenerator();
  const ULONG      n    = 1000;
  stat.setTotalTestCount(n);
  for(ULONG i = 0; i < n; i++) {
    const __int64  x64  = rnd.nextInt64();
    __int64  y64;
    do { y64 = rnd.nextInt64(); } while(y64 == 0);
    const __int64  z64  = x64 / y64;
    if(stat.isTimeToPrint()) {
      stat.printLoopMessage(_T("i:%10u"), i);
    }

    const BigInt  X(x64, pool), Y(y64, pool), Za = X / Y, Zb = quot(X,Y);
    const __int64 Z64a = getInt64(Za);
    const __int64 Z64b = getInt64(Zb);
    if((Z64a != z64) || (Z64b != z64)) {
      ERRLOG << _T("Error in int prod")       << endl
             << _T("X             :") << X    << endl
             << _T("Y             :") << Y    << endl
             << _T("Za = X/Y      :") << Za   << endl
             << _T("Zb = quot(X,Y):") << Zb   << endl
             << _T("x64/y64       :") << z64  << endl;
      THROWTESTERROR();
    }
  }
  stat.setEndMessageToOk();
}

void testIntRem(TestStatistic &stat) { // tester BigInt rem
  DigitPool       *pool = stat.getDigitPool();
  RandomGenerator &rnd  = stat.getRandomGenerator();
  const ULONG      n    = 1000;
  stat.setTotalTestCount(n);
  for(ULONG i = 0; i < n; i++) {
    const __int64  x64  = rnd.nextInt64();
    __int64  y64;
    do { y64 = rnd.nextInt64(); } while(y64 == 0);
    const __int64  z64  = x64 % y64;
    if(stat.isTimeToPrint()) {
      stat.printLoopMessage(_T("i:%10u"), i);
    }

    const BigInt  X(x64, pool), Y(y64, pool), Za = X % Y, Zb = rem(X,Y);
    const __int64 Z64a = getInt64(Za);
    const __int64 Z64b = getInt64(Zb);
    if((Z64a != z64) || (Z64b != z64)) {
      ERRLOG << _T("Error in int rem")       << endl
             << _T("X            :") << X    << endl
             << _T("Y            :") << Y    << endl
             << _T("Za = X%Y     :") << Za   << endl
             << _T("Zb = rem(X,Y):") << Zb   << endl
             << _T("x64%y64      :") << z64  << endl;
      THROWTESTERROR();
    }
  }
  stat.setEndMessageToOk();
}

void testMRIsPrime(TestStatistic &stat) {
  DigitPool *pool = stat.getDigitPool();

  static const char* prime200Str =
    "4104099511239423131236035542227957882673760571961180554513588534654406519875042180382666886167073837"
    "2825768788129141391284994375566860314332611644420154854421622536166703154271507101363910202679308323";

  static const char* prime500Str =
    "4374709331433953729233961995412554480983501654731356826759253785967425120827653060611283885837481757"
    "8554666168101918497598032204821863150402162873856795238062599114529795896259139388835236217689847856"
    "1690086706589184957333316073484416502305710002718746709379014065862697104973504193532839634923198684"
    "1902874049396171255013014814617922254553216076743992644745296993512256250437774942288257825896920719"
    "1442303122436571930652841006351288079547345744521666306242868107168223869152382980024675391467661317";

  const BigInt p200(prime200Str, pool);
  const BigInt p500(prime500Str, pool);

  verify(MRisprime(p200));
  verify(MRisprime(p500));
  const BigInt n1 = p200 + BigInt(10, pool);
  verify(!MRisprime(n1));
  stat.setEndMessageToOk();
}

void testAssignOperators(TestStatistic &stat) {
  DigitPool *pool = stat.getDigitPool();

  const BigReal &_1 = pool->_1();
  const BigReal &_2 = pool->_2();

  for(int i = 0; i < 100; i++) {
    if(stat.isTimeToPrint()) {
      stat.printLoopMessage(EMPTYSTRING);
    }

    BigReal x = stat.getRandom(20,5);
    BigReal tmp = x;
    tmp += tmp;
    verify(tmp == _2*x);
    tmp = x;
    tmp -= tmp;
    verify(tmp.isZero());
    tmp = x;
    tmp *= tmp;
    verify(tmp == x*x);
    if(!x.isZero()) {
      tmp = x;
      tmp %= tmp;
      verify(tmp == x % x);
    }
    BigReal y = x - stat.getRandom(20,5);
    tmp = x;
    tmp += y;
    verify(tmp == x+y);
    tmp = x;
    tmp -= y;
    verify(tmp == x-y);
    tmp = x;
    tmp *= y;
    verify(tmp == x * y);
    if(!y.isZero()) {
      tmp = x;
      tmp %= y;
      verify(tmp == x % y);
    }
    tmp = x;
    verify(tmp++ == x);
    verify(tmp   == x+_1);
    verify(++tmp == x+_2);
    verify(tmp   == x+_2);
    verify(--tmp == x+_1);
    verify(tmp   == x+_1);
    verify(tmp-- == x+_1);
    verify(tmp   == x);
  }
  stat.setEndMessageToOk();
}

void testGetFirst32(TestStatistic &stat) {
  DigitPool *pool = stat.getDigitPool();

  static const TCHAR *list[] = {
    _T("1234567890123456"    )
   ,_T("98765432109876543210")
   ,_T("0"                   )
   ,_T("12"                  )
   ,_T("1"                   )
  };

  const int minExponent    = -10;
  const int maxExponent    =  10;
  const int maxK           = 9;
  const int totalTestCount = ARRAYSIZE(list) * (maxExponent - minExponent) * maxK;

  stat.setTotalTestCount(totalTestCount);

  for(int i = 0; i < ARRAYSIZE(list); i++) {
    const TCHAR *str = list[i];
    BigReal x(str, pool);
    for(int exponent = minExponent; exponent < maxExponent; exponent++) {
      FullFormatBigReal X(e(x,exponent));
      for(int k = 1; k <= maxK; k++) {

        if(stat.isTimeToPrint()) {
          stat.printLoopMessage(_T("i:%d[0..%d], exponent:%3d[%d..%d], k:%d[0..%d]")
                               ,i, ARRAYSIZE(list)-1
                               ,exponent, minExponent, maxExponent-1
                               ,k
                               ,maxK
                               );
        }

        String result = format(_T("%0*lu"),k,X.getFirst32(k));
        const int len     = (int)_tcslen(str);
        const int missing = k - len;
        String expected = substr(str + spaceString(missing,_T('0')),0,k);
        if(result != expected) {
          ERRLOG << _T("getFirst32(") << X << _T(",") << k << _T(")=") << result << endl
                 << _T("Expected:")   << expected                                << endl;
          THROWTESTERROR();
        }
      }
    }
  }
  stat.setEndMessageToOk();
}

void testGetFirst64(TestStatistic &stat) {
  DigitPool *pool = stat.getDigitPool();

  static const TCHAR *list[] = {
    _T("1234567890123456")
   ,_T("9876543210987654321098765432109876543210")
   ,_T("0")
   ,_T("12")
   ,_T("1")
  };

  const int minExponent    = -10;
  const int maxExponent    = 10;
  const int maxK           = 19;
  const int totalTestCount = ARRAYSIZE(list) * (maxExponent - minExponent) * maxK;

  stat.setTotalTestCount(totalTestCount);

  for(int i = 0; i < ARRAYSIZE(list); i++) {
    const TCHAR *str = list[i];
    BigReal x(str, pool);
    for(int exponent = minExponent; exponent < maxExponent; exponent++) {
      FullFormatBigReal X(e(x, exponent));
      for(int k = 1; k <= maxK; k++) {

        if(stat.isTimeToPrint()) {
          stat.printLoopMessage(_T("i:%d[0..%d], exponent:%3d[%d..%d], k:%d[0..%d]")
            , i, ARRAYSIZE(list) - 1
            , exponent, minExponent, maxExponent - 1
            , k
            , maxK
          );
        }

        String result = format(_T("%0*I64u"), k, X.getFirst64(k));
        const int len = (int)_tcslen(str);
        const int missing = k - len;
        String expected = substr(str + spaceString(missing, _T('0')), 0, k);
        if (result != expected) {
          ERRLOG << _T("getFirst64(") << X << _T(",") << k << _T(")=") << result << endl
                 << _T("Expected:") << expected << endl;
          THROWTESTERROR();
        }
      }
    }
  }
  stat.setEndMessageToOk();
}

#ifdef IS64BIT
void testGetFirst128(TestStatistic &stat) {
  DigitPool *pool = stat.getDigitPool();

  static const TCHAR *list[] = {
    _T("1234567890123456")
   ,_T("987654321098765432109876543210987654321098765432109876543210")
   ,_T("0")
   ,_T("12")
   ,_T("1")
  };

  const int minExponent    = -10;
  const int maxExponent    = 10;
  const int maxK           = 38;
  const int totalTestCount = ARRAYSIZE(list) * (maxExponent - minExponent) * maxK;

  stat.setTotalTestCount(totalTestCount);

  for(int i = 0; i < ARRAYSIZE(list); i++) {
    const TCHAR *str = list[i];
    BigReal x(str, pool);
    for(int exponent = minExponent; exponent < maxExponent; exponent++) {
      FullFormatBigReal X(e(x, exponent));
      for(int k = 1; k <= maxK; k++) {

        if(stat.isTimeToPrint()) {
          stat.printLoopMessage(_T("i:%d[0..%d], exponent:%3d[%d..%d], k:%d[0..%d]")
            , i, ARRAYSIZE(list) - 1
            , exponent, minExponent, maxExponent - 1
            , k
            , maxK
          );
        }
        _uint128 ui128;
        String result = toString(X.getFirst128(ui128, k));
        int missing = k - (int)result.length();
        if(missing > 0) {
          result.insert(0, missing, '0');
        }
        const int len = (int)_tcslen(str);
        missing = k - len;
        String expected = substr(str + spaceString(missing, _T('0')), 0, k);
        if (result != expected) {
          ERRLOG << _T("getFirst128(") << X << _T(",") << k << _T(")=") << result << endl
                 << _T("Expected:") << expected << endl;
          THROWTESTERROR();
        }
      }
    }
  }
  stat.setEndMessageToOk();
}
#endif // IS64BIT

void testReadWriteBigReal(TestStatistic &stat) {
  const String   fileName = getTestFileName(__TFUNCTION__);
  const size_t   count    = 500;
  DigitPool     *pool     = stat.getDigitPool();
  Array<BigReal> list;

/* TODO
  list.add(pool->getnan());
  list.add(pool->getpinf());
  list.add(pool->getninf());
*/
  for(size_t i = 0; i < count; i++) {
    int          xlen = rand() % 6000 + 500;
    int          xexpo = rand() % 3000 - 1500;
    const BigReal x = stat.getRandom(xlen, xexpo);
    list.add(x);
  }

  tofstream out(fileName.cstr());
  for(size_t i = 0; i < list.size(); i++) {
    const BigReal &x = list[i];
    out << FullFormatBigReal(x) << endl;
  }
  out.close();

  tifstream in(fileName.cstr());
  for(size_t i = 0; i < list.size(); i++) {
    const BigReal &expected = list[i];
    BigReal data(pool);
    in >> data;
    if(in.bad()) {
      stat.out() << _T("Read number line ") << i << _T(" failed") << endl;
      THROWTESTERROR();
    }
    if(data._isfinite()) {
      if(data != expected) {
        ERRLOG << _T("Read BigReal at line ") << i << _T("(=") << FullFormatBigReal(data) << _T(") != exptected (=") << FullFormatBigReal(expected) << _T(")") << endl;
        THROWTESTERROR();
        verify(data == expected);
      }
    } else if(isPInfinity(data)) {
      verify(isPInfinity(expected));
    } else if (isNInfinity(data)) {
      verify(isNInfinity(expected));
    } else if(isnan(data)) {
      verify(isnan(expected));
    } else {
      throwException(_T("Unknown BigReal-classification for a[%zu]:%s"), i, toString(data).cstr());
    }
  }
  in.close();
  stat.setEndMessageToOk();
}

void testReadWriteBigInt(TestStatistic &stat) {
  const String   fileName = getTestFileName(__TFUNCTION__);
  const size_t   count    = 500;
  DigitPool     *pool     = stat.getDigitPool();
  Array<BigInt>  list;

  for(size_t i = 0; i < count; i++) {
    int xlen = rand() % 6000 + 500;
    BigInt x(randBigInt(xlen, stat.getRandomGenerator(), pool));
    list.add(x);
  }

  tofstream out(fileName.cstr());
  for (size_t i = 0; i < list.size(); i++) {
    const BigInt &x = list[i];
    out << x << endl;
  }
  out.close();

  tifstream in(fileName.cstr());
  for(size_t i = 0; i < list.size(); i++) {
    const BigInt &expected = list[i];
    BigInt x(pool);
    in >> x;
    if(in.bad()) {
      stat.out() << _T("Read integer line ") << i << _T(" failed") << endl;
      THROWTESTERROR();
    }
    if(x != expected) {
      ERRLOG << _T("Read BigReal at line ") << i << _T("(=") << FullFormatBigReal(x) << _T(") != exptected (=") << FullFormatBigReal(expected) << _T(")") << endl;
      THROWTESTERROR();
    }
  }
  in.close();
  stat.setEndMessageToOk();
}

void testReadWriteBigRational(TestStatistic &stat) {
  const String       fileName = getTestFileName(__TFUNCTION__);
  const size_t       count    = 500;
  RandomGenerator   &rnd      = stat.getRandomGenerator();
  DigitPool         *pool     = stat.getDigitPool();
  Array<BigRational> list;

  BigInt maxDen(e(pool->_1(), 30));
  BigRational from(randBigRational(maxDen, rnd)), to(randBigRational(maxDen, rnd));
  if(from > to) {
    BigRational tmp = from; from = to; to = tmp;
  }
  for(size_t i = 0; i < count; i++) {
    BigRational x(randBigRational(from, to, pool->_1(), rnd));
    list.add(x);
  }

  tofstream out(fileName.cstr());
  for(size_t i = 0; i < list.size(); i++) {
    const BigRational &x = list[i];
    out << x << endl;
  }
  out.close();

  tifstream in(fileName.cstr());
  for(size_t i = 0; i < list.size(); i++) {
    const BigRational &expected = list[i];
    BigRational x(pool);
    in >> x;
    if(in.bad()) {
      stat.out() << _T("Read Rational line ") << i << _T(" failed") << endl;
      THROWTESTERROR();
    }
    if(x != expected) {
      ERRLOG << _T("Read BigRational at line ") << i << _T("(=") << x << _T(") != exptected (=") << expected << _T(")") << endl;
      THROWTESTERROR();
    }
  }
  in.close();
  stat.setEndMessageToOk();
}

UINT DigitMonitorThread::run() {
  for(;;) {
    Sleep(3000);
    _tprintf(_T("Total digits allocated:%s\n")
            ,format1000(DigitPool::getTotalAllocatedDigitCount()).cstr()
            );
  }
}
