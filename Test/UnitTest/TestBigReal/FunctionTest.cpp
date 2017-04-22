#include "stdafx.h"
#include <Console.h>
#include "FunctionTest.h"

void FunctionTest1ArgND64D80::runTest(int threadId, DigitPool *pool) {
  const BigReal     from(m_xfrom, pool);
  const BigReal     to(  m_xto  , pool);

  FullFormatBigReal maxTolerance   = e(pool->get1(),-MAXDIGITS-6);
  Array<BigReal>    testData       = generateTestData(from,to, m_xexponentialStep);
  TestStatistic    stat(threadId, m_functionName, pool, XF, testData.size() * MAXDIGITS / DIGITSTEP, maxTolerance);

  for(size_t i = 0; i < testData.size(); i++) {
    const BigReal &x           = testData[i];
    const BigReal  exactResult = m_f1(x, maxTolerance);
  
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
      stat.checkError(m_f1, x, stat.getRandomTolerance(-p), exactResult);
    }
  }
}

void FunctionTest2ArgND64D80::runTest(int threadId, DigitPool *pool) {
  const BigReal           xfrom(m_xfrom, pool);
  const BigReal           xto(  m_xto  , pool);
  const BigReal           yfrom(m_yfrom, pool);
  const BigReal           yto(  m_yto  , pool);

  const FullFormatBigReal maxTolerance   = e(pool->get1(),-MAXDIGITS-6);
  const Array<BigReal>    xTestData      = generateTestData(xfrom,xto, m_xexponentialStep,10);
  const Array<BigReal>    yTestData      = generateTestData(yfrom,yto, m_yexponentialStep,10);
  TestStatistic          stat(threadId, m_functionName, pool, XYF, xTestData.size() * yTestData.size() * MAXDIGITS / DIGITSTEP, maxTolerance);

  for(size_t i = 0; i < xTestData.size(); i++) {
    const BigReal &x = xTestData[i];
    for(size_t j = 0; j < yTestData.size(); j++) {
      const BigReal &y           = yTestData[j];
      const BigReal  exactResult = m_f2( x, y, maxTolerance);

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
        stat.checkError(m_f2, x, y, stat.getRandomTolerance(-p), exactResult);
      }
    }
  }
}

void ExactBinaryOperatorTest::runTest(int threadId, DigitPool *pool) {
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
            ERRLOG << "Error in " << m_functionName         << NEWLINE
                   <<"x:"      << FullFormatBigReal(x)      << NEWLINE
                   <<"y:"      << FullFormatBigReal(y)      << NEWLINE
                   <<"result:" << FullFormatBigReal(result) << NEWLINE;
            throwException(_T("Error in %s"), m_functionName.cstr());
          }
        }
      }
    }
  }
  stat.setEndMessageToOk();
}

void OperatorTest2ArgND64D80Pool::specialTest(int threadId, DigitPool *pool) {
  BigReal                 x("8.356696602362382269528160105890000e+009", pool);
  BigReal                 y("8.356810893959301561557443735160000e+009", pool);
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

  const FullFormatBigReal maxTolerance   = e(pool->get1(),-MAXDIGITS-6);
  TestStatistic          stat(threadId, m_functionName, pool, XYF, MAXDIGITS, maxTolerance);

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

  const int              length         = 30;
  const FullFormatBigReal maxTolerance   = e(pool->get1(),-MAXDIGITS-6);
  TestStatistic          stat(threadId, m_functionName, pool, XYF, (sqr((MAXSCALE - MINSCALE + 1)) * MAXDIGITS) / (sqr(SCALESTEP)*DIGITSTEP), maxTolerance);

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
  const BigReal        from(m_xfrom, pool);
  const BigReal        to(  m_xto  , pool);

  const Array<BigReal> testData = generateTestData(from,to, m_xexponentialStep);
  TestStatistic       stat(threadId, m_functionName, pool, XD, testData.size() * MAXDIGITS / DIGITSTEP, MAXDIGITS);

  for(size_t i = 0; i < testData.size(); i++) {
    const BigReal &x           = testData[i];
    const BigReal  exactResult = m_rf1(x, MAXDIGITS);
  
    for(int digits = 1; digits <= MAXDIGITS; digits += DIGITSTEP) {
      if(stat.isTimeToPrint()) {
        stat.printLoopMessage(_T("i:%-4d[0..%d], digits:%-4d[0..%d]")
                             ,i, testData.size()-1
                             ,digits, MAXDIGITS);
      }
      stat.checkError(m_rf1, x, digits, exactResult);
    }
  }
}

void FunctionTest2ArgRelative::runTest(int threadId, DigitPool *pool) {
  const BigReal        xfrom(m_xfrom, pool);
  const BigReal        xto(  m_xto  , pool);
  const BigReal        yfrom(m_yfrom, pool);
  const BigReal        yto(  m_yto  , pool);

  const Array<BigReal> xTestData      = generateTestData(xfrom,xto, m_xexponentialStep,10);
  const Array<BigReal> yTestData      = generateTestData(yfrom,yto, m_yexponentialStep,10);
  TestStatistic       stat(threadId, m_functionName, pool, XYD, xTestData.size() * yTestData.size() * MAXDIGITS / DIGITSTEP, MAXDIGITS);

  for(size_t i = 0; i < xTestData.size(); i++) {
    const BigReal &x = xTestData[i];
    for(size_t j = 0; j < yTestData.size(); j++) {
      const BigReal &y           = yTestData[j];
      const BigReal  exactResult = m_rf2(x, y, MAXDIGITS);

      for(int digits = 0; digits <= MAXDIGITS; digits += DIGITSTEP) {
        if(stat.isTimeToPrint()) {
          stat.printLoopMessage(_T("i:%-4d[0..%d], j:%-4d[0..%d], digits:%-4d[0..%d]")
                               ,i, xTestData.size()-1
                               ,j, yTestData.size()-1
                               ,digits, MAXDIGITS);
        }
        stat.checkError(m_rf2, x, y, digits, exactResult);
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
        const BigReal tolerance = e(BIGREAL_1, -digits, pool);
        const BigReal qn        = BigReal::quotNewton(  x, y, tolerance, pool);
        const BigReal ql32      = BigReal::quotLinear32(x, y, tolerance, pool);
        const BigReal ql64      = BigReal::quotLinear64(x, y, tolerance, pool);

        if(fabs(qn - ql32) > tolerance || fabs(qn - ql64) > tolerance || fabs(ql32 - ql64) > tolerance) {
          ERRLOG << _T("Error in ") << quot                                                              << NEWLINE
                 << _T("x        :")                                   << FullFormatBigReal(x)           << NEWLINE
                 << _T("y        :")                                   << FullFormatBigReal(y)           << NEWLINE
                 << _T("Tolerance:")                                   << tolerance                      << NEWLINE
                 << _T("quotNewton(x,y,tolerance)  :")                 << FullFormatBigReal(qn)          << NEWLINE
                 << _T("quotLinear32(x,y,tolerance):")                 << FullFormatBigReal(ql32)        << NEWLINE
                 << _T("quotLinear64(x,y,tolerance):")                 << FullFormatBigReal(ql64)        << NEWLINE
                 << _T("quotNewton - quotLinear32  :")                 << FullFormatBigReal(qn - ql32)   << NEWLINE
                 << _T("quotNewton - quotLinear64  :")                 << FullFormatBigReal(qn - ql64)   << NEWLINE
                 << _T("quotLinear32 - quotLinear64:")                 << FullFormatBigReal(ql32 - ql64) << NEWLINE;
          throwException(_T("Error in testQuot3"));
        } 
      }
    }
  }
  stat.setEndMessageToOk();
}

static const ConstBigReal pi2000(
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
  "222106611863067442786220391949450471237137869609563643719172874677646575739624138908658326459958133904780275901");


void testPi(TestStatistic &stat) {
  DigitPool *pool        = stat.getDigitPool();
  const int  step        = 5;
  const int  minDecimals = 1;
  const int  maxDecimals = 2000;

  stat.setTotalTestCount((maxDecimals - minDecimals) / step);
  
  for(int digits = minDecimals; digits < maxDecimals; digits += step) {
    const BigReal tolerance = e(BIGREAL_1, -digits, pool);
    const BigReal result    = pi(tolerance, pool);

    if(stat.isTimeToPrint()) {
      stat.printLoopMessage(_T("Tolerance:%s      "), stat.toString(tolerance).cstr());
    }

    const FullFormatBigReal error = fabs(result-pi2000);
    if(error > tolerance) {
      Console::setCursorPos(1,5);
      ERRLOG << "Error in pi"                                                 << NEWLINE
             << "Tolerance    :" << tolerance                                 << NEWLINE
             << "pi(tolerance):" << FullFormatBigReal(result)                 << NEWLINE
             << "Difference = fabs(pi(tolerance)-pi2000):" << nparam << error << NEWLINE;
      throwException(_T("Error in pi"));
    } else {
      stat.update(error, tolerance);
    }
  }
}

void testModulus(TestStatistic &stat) {
  DigitPool *pool = stat.getDigitPool();
  stat.setTotalTestCount(820);

  for(double x = 1; x < _I64_MAX; x *= 1.1) {
    for(double y = 1; y <= x; y *= 1.1) {

      if(stat.isTimeToPrint()) {
        stat.printLoopMessage(_T("x:%10.2le y:%10.2le"), x, y);
      }

      __int64 x64 =  (__int64)x;
      __int64 y64 = -(__int64)y;
      __int64 z64 = x64 % y64;
      BigInt X(x64,pool),Y(y64,pool);
      BigInt Z = X % Y;
      __int64 Z64 = getInt64(Z);
      if(Z64 != z64) {
        ERRLOG << _T("Error in modulus") << NEWLINE
               << _T("X      :") << X    << NEWLINE
               << _T("Y      :") << Y    << NEWLINE
               << _T("Z = X%Y:") << Z    << NEWLINE
               << _T("x64%y64:") << z64  << NEWLINE;
        throwException(_T("Error in testModulus"));
      }
    }
  }
  stat.setEndMessageToOk();
}

void testIntegerDivision(TestStatistic &stat) { // tester BigInt-division
  DigitPool *pool = stat.getDigitPool();
  stat.setTotalTestCount(820);

  for(double x = 1; x < _I64_MAX; x *= 1.1) {
    for(double y = 1; y <= x; y *= 1.1) {

      if(stat.isTimeToPrint()) {
        stat.printLoopMessage(_T("x:%10.2le y:%10.2le"), x, y);
      }

      __int64 x64 =  (__int64)x;
      __int64 y64 = -(__int64)y;
      __int64 z64 = x64 / y64;
      BigInt X(x64,pool),Y(y64,pool);
      BigInt Z = X / Y;
      __int64 Z64 = getInt64(Z);
      if(Z64 != z64) {
        ERRLOG << _T("Error in integer division") << NEWLINE
               << _T("X      :") << X             << NEWLINE
               << _T("Y      :") << Y             << NEWLINE
               << _T("Z = X/Y:") << Z             << NEWLINE
               << _T("x64/y64:") << z64           << NEWLINE;
        throwException(_T("Error in APCpow"));
      }
    }
  }
  stat.setEndMessageToOk();
}

void testAssignOperators(TestStatistic &stat) {
  DigitPool *pool = stat.getDigitPool();

  const BigReal &_1 = pool->get1();
  const BigReal &_2 = pool->get2();

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

void testGetFirst(TestStatistic &stat) {
  DigitPool *pool = stat.getDigitPool();

  static const char *list[] = {
    "1234567890123456"
   ,"98765432109876543210"
   ,"0"
   ,"12"
   ,"1"
  };

  const int minExponent = -10;
  const int maxExponent =  10;
  const int totalTestCount = ARRAYSIZE(list) * (maxExponent - minExponent) * 9;

  stat.setTotalTestCount(totalTestCount);

  for(int i = 0; i < ARRAYSIZE(list); i++) {
    const char *str = list[i];
    BigReal x(str, pool);
    for(int exponent = minExponent; exponent < maxExponent; exponent++) {
      FullFormatBigReal X(e(x,exponent));
      for(int k = 1; k <= 9; k++) {

        if(stat.isTimeToPrint()) {
          stat.printLoopMessage(_T("i:%d[0..%d], exponent:%3d[%d..%d], k:%d[0..9]")
                               ,i, ARRAYSIZE(list)-1
                               ,exponent, minExponent, maxExponent-1
                               ,k
                               );
        }

        String result = format(_T("%0*lu"),k,X.getFirst32(k));
        const int len     = (int)strlen(str);
        const int missing = k - len;
        String expected = substr(str + spaceString(missing,_T('0')),0,k);
        if(result != expected) {
          ERRLOG << "getFirst(" << X << "," << k << ")=" << result << NEWLINE
                 << "Expected:" << expected                        << NEWLINE;
          throwException(_T("Error in getFirst32"));
        }
      }
    }
  }
  stat.setEndMessageToOk();
}

void testReadWriteBigReal(TestStatistic &stat) {
  const char    *fileName = "numbers.dat";
  const int      count    = 500;
  tofstream      out(fileName);
  size_t         i;
  Array<BigReal> list;
  DigitPool     *pool = stat.getDigitPool();

  for(i = 0; i < count; i++) {
    int          xlen  = rand() % 6000 + 500;
    int          xexpo = rand() % 3000 - 1500;
    const BigReal x     = stat.getRandom(xlen, xexpo);

    out << FullFormatBigReal(x) << NEWLINE;
    list.add(x);
  }
  out.close();

  tifstream in(fileName);
  for(i = 0; i < list.size(); i++) {
    const BigReal &expected = list[i];
    BigReal x(pool);
    in >> x;
    if(in.bad()) {
      stat.out() << _T("Read number line ") << i << _T(" failed") << NEWLINE;
      throwException(_T("Error in testReadWriteBigReal"));
    }
    if(x != expected) {
      ERRLOG << _T("Read BigReal at line ") << i << _T("(=") << FullFormatBigReal(x) << _T(") != exptected (=") << FullFormatBigReal(expected) << _T(")") << NEWLINE;
      throwException(_T("Error in testReadWriteBigReal"));
    }
  }
  in.close();
  UNLINK(fileName);
  stat.setEndMessageToOk();
}

void testReadWriteInteger(TestStatistic &stat) {
  const char    *fileName = "integers.dat";
  const int      count    = 500;
  tofstream      out(fileName);
  size_t         i;
  Array<BigInt>  list;
  DigitPool     *pool = stat.getDigitPool();

  for(i = 0; i < count; i++) {
    int xlen = rand() % 6000 + 500;
    BigInt x(stat.getRandomGenerator().nextInteger(xlen, pool));
    out << x << NEWLINE;
    list.add(x);
  }
  out.close();

  tifstream in(fileName);
  for(i = 0; i < list.size(); i++) {
    const BigInt &expected = list[i];
    BigInt x(pool);
    in >> x;
    if(in.bad()) {
      stat.out() << _T("Read integer line ") << i << _T(" failed") << NEWLINE;
      throwException(_T("Error in testReadWriteInteger"));
    }
    if(x != expected) {
      ERRLOG << _T("Read BigReal at line ") << i << _T("(=") << FullFormatBigReal(x) << _T(") != exptected (=") << FullFormatBigReal(expected) << _T(")") << NEWLINE;
      throwException(_T("Error in testReadWriteInteger"));
    }
  }
  in.close();
  UNLINK(fileName);
  stat.setEndMessageToOk();
}

unsigned int DigitMonitorThread::run() {
  for(;;) {
    Sleep(3000);
    _tprintf(_T("Total digits allocated:%s\n")
            ,format1000(DigitPool::getTotalAllocatedDigitCount()).cstr()
            );
  }
}
