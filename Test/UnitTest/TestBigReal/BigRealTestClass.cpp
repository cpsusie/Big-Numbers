#include "stdafx.h"
#include <Date.h>
#include <Console.h>
#include <CPUInfo.h>
#include "BigRealTestClass.h"

BigRealTestClass::~BigRealTestClass() {
  if(m_digitPool) {
    BigRealThreadPool::releaseDigitPool(m_digitPool);
    m_digitPool = NULL;
  }
}

DigitPool *BigRealTestClass::getDigitPool() {
  if(m_digitPool == NULL) {
    m_digitPool = BigRealThreadPool::fetchDigitPool();
  }
  return m_digitPool;
}

BigReal BigRealTestClass::getRandomDividend(int length) {
  for(;;) {
    BigReal result = getRandom(length,length, getRandomGenerator(), getDigitPool(), false);
    if(BigReal::getExpo10(result) == length-1) {
      return (rand()%2) ? result : -result;
    }
  }
}

void BigRealTestClass::measureProduct(bool measureSplitProd, bool measureSplitProdRealTime, bool measureShortProd, int splitLength) {
  if(splitLength > 0) {
    BigReal::s_splitLength = splitLength;
  }

  _tprintf(_T("measureProduct:splitLength:%d\n"), (int)BigReal::s_splitLength);

  const String dir                   = format(_T("prod%s"),Date().toString("yyyyMMdd").cstr());
  const String splitProductFileName  = format(_T("%s\\splitProduct%d.dat")      , dir.cstr(), BigReal::s_splitLength);
  const String splitProductRFileName = format(_T("%s\\splitProductR%d.dat")     , dir.cstr(), BigReal::s_splitLength);
  const String shortProductFileName  = format(_T("%s\\shortProduct.dat"), dir.cstr());

  const int minProductLength      = 30;
  const int maxProductLength      = 620000;
  const int stepCount             = 500;
  double factor = root((double)maxProductLength/minProductLength, stepCount);

  if(measureSplitProd) {
    FILE *f = MKFOPEN(splitProductFileName ,"w"); fclose(f);
  }
  if(measureSplitProdRealTime) {
    FILE *f = MKFOPEN(splitProductRFileName,"w"); fclose(f);
  }
  if(measureShortProd) {
    FILE *f = MKFOPEN(shortProductFileName ,"w"); fclose(f);
  }

  for(double d = minProductLength; d <= maxProductLength; d *= factor) {
    const int p = (int)d;
    Array<BigReal> x, y;
    for(int i = 0; i < 11; i++) {
      x.add(BigReal::random(maxProductLength));
    }
    for(int i = 0; i < 13; i++) {
      y.add(BigReal::random(maxProductLength));
    }
    const BigReal f = e(1, -p);
    MeasureBinaryOperator mSplitProd(PROD,      x, y, f);
    MeasureBinaryOperator mShortProd(SHORTPROD, x, y, f);

    _tprintf(_T("p:%6d "), p); fflush(stdout);
    if(measureSplitProd) {
      const double t = measureTime(mSplitProd, MEASURE_PROCESSTIME);
      _tprintf(_T("time(splitProd):%12.3le "), t); fflush(stdout);
      FILE *f = FOPEN(splitProductFileName, "a");
      _ftprintf(f,_T("%d %.3le\n"), p, t);
      fclose(f);
    }
    if(measureSplitProdRealTime) {
      const double t = measureTime(mSplitProd, MEASURE_REALTIME);
      _tprintf(_T("time(splitProd):%12.3le "), t); fflush(stdout);
      FILE *f = FOPEN(splitProductRFileName, "a");
      _ftprintf(f,_T("%d %.3le\n"), p, t);
      fclose(f);
    }
    if(measureShortProd) {
      const double t = measureTime(mShortProd, MEASURE_PROCESSTIME);
      _tprintf(_T("time(shortProd):%12.3le"), t);
      FILE *f = FOPEN(shortProductFileName, "a");
      _ftprintf(f,_T("%d %.3le\n"), p, t);
      fclose(f);
    }
/*
    if(fabs(mProd.getResult() - mShortProd.getResult()) > f) {
      _tprintf(_T("FEJL"));
      pause();
    }
*/
    _tprintf(_T("\n"));
    fflush(stdout);
  }
  clearLine();
}

void BigRealTestClass::measureSplitLength() {
  const String dir = format(_T("prod-REALTIME%s"),Date().toString("yyyyMMdd").cstr());
  for(int xLength = 4000; xLength <= 40000; xLength = (int)(1.3*xLength)) {
    for(int yLength = xLength; yLength <= 40000; yLength = (int)(1.3*yLength)) {

      const String fileName = format(_T("%s\\splitfac%05d_%05d.dat"), dir.cstr(), xLength, yLength);
      FILE *dataFile = MKFOPEN(fileName,"w");

      for(BigReal::s_splitLength = 120; BigReal::s_splitLength <= 600; BigReal::s_splitLength += 5) {
        Array<BigReal> x, y;
        for(int i = 0; i < 11; i++) {
          x.add(BigReal::random(xLength));
        }
        for(int i = 0; i < 13; i++) {
          y.add(BigReal::random(yLength));
        }

        MeasureBinaryOperator mProd(PROD,x,y,0);
        double timeUsage = measureTime(mProd, MEASURE_REALTIME);

        _tprintf(_T("BigRealLength:(%4d,%4d) splitLength:%3d time:%20.3le sec.\n")
                ,xLength, yLength, (int)BigReal::s_splitLength, timeUsage);
        fflush(stdout);

        _ftprintf(dataFile,_T("%d %.3le\n"), (int)BigReal::s_splitLength, timeUsage);
      }
      fclose(dataFile);
    }
  }
}

class TimeUsageMethod {
public:
  char *m_method;
  double m_timeUsage;
  TimeUsageMethod(char *method, double timeUsge);
};

TimeUsageMethod::TimeUsageMethod(char *method, double timeUsage) {
  m_method    = method;
  m_timeUsage = timeUsage;
}

static int timeUsageMethodCmp(const TimeUsageMethod &t1, const TimeUsageMethod &t2) {
  return getInt(dsign(t1.m_timeUsage - t2.m_timeUsage));
}

void BigRealTestClass::measureQuot() {
  tofstream dataFile("quotMeasure.dat");

  const String header = "ResultDigits y                              MTime32(L)  ETime32(L)  q32(L)=MT/ET MTime64(L) M32/M64     MTime(N)    ETime(N)    q(N)=MT/ET";

  tcout     << header << NEWLINE;
  dataFile  << header << NEWLINE;

  const BigReal yStep1 = e(BigReal(165),-2);
  const BigReal yStep2 = BigReal(3);
  const BigReal yEnd   = e(BIGREAL_1,30);

  for(BigInt y = 2; y < yEnd; y = (y.getLength() == 1) ? (y * yStep1) : (y * yStep2)) {
    for(unsigned int p = 24; p < 1000000; p = (p < 60) ? (p + 3) : (p *= 3)) {
      Array<BigReal> xArray,yArray;
      int xLength = rand() % 1000 + 20;
      for(int i = 0; i < 11; i++) {
        xArray.add(getRandomDividend(xLength));
      }

      yArray.add(y);

      const BRExpoType expo10x0 = BigReal::getExpo10(xArray[0]);
      const BRExpoType expo10y0 = BigReal::getExpo10(yArray[0]);

      for(size_t i = 1; i < xArray.size(); i++) {
        const BRExpoType expo10xi = BigReal::getExpo10(xArray[i]);
        if(expo10xi != expo10x0) {
          throwException(_T("expo10(x[%d]=%s)=%s != expo10(x[0]=%s)=%s\n")
                        ,(int)i
                        ,xArray[i].toString().cstr(),format1000(expo10xi).cstr()
                        ,xArray[0].toString().cstr(),format1000(expo10x0).cstr());
        }
      }

      BigReal f = e(1,expo10x0 - expo10y0 - p);

      TimeUsageMethod l32("L32",measureTime(MeasureBinaryOperator(QUOTLINEAR32, xArray, yArray, f)));
      TimeUsageMethod l64("L64",measureTime(MeasureBinaryOperator(QUOTLINEAR64, xArray, yArray, f)));
      TimeUsageMethod lnt("N"  ,measureTime(MeasureBinaryOperator(QUOTNEWTON  , xArray, yArray, f)));

      Array<TimeUsageMethod> ta;
      ta.add(l32);
      ta.add(l64);
      ta.add(lnt);
      ta.sort(timeUsageMethodCmp);

      const BRExpoType expectedResultExpo = expo10x0 - expo10y0; // Expected exponent of result
      const BRExpoType errorExpo          = BigReal::getExpo10(f);
      const BRExpoType resultDigits       = expectedResultExpo - errorExpo;

      double linear32TimeExpected = resultDigits * y.getLength() / 1.6e7;
      double newtonTimeExpected   = ::pow((double)resultDigits,1.07)/5e6;

      dataFile << iparam(12)  << resultDigits                           << _T(" ")
               << iparam(30)  << y                                      << _T(" ")
               << udparam(4)  << l32.m_timeUsage                        << _T(" ")
               << udparam(4)  << linear32TimeExpected                   << _T(" ")
               << udparam(4)  << l32.m_timeUsage/linear32TimeExpected   << _T(" ")
               << udparam(4)  << l64.m_timeUsage                        << _T(" ")
               << udparam(4)  << l32.m_timeUsage/l64.m_timeUsage        << _T(" ")
               << udparam(4)  << lnt.m_timeUsage                        << _T(" ")
               << udparam(4)  << newtonTimeExpected                     << _T(" ")
               << udparam(4)  << lnt.m_timeUsage/newtonTimeExpected     << _T(" ")
               << ta[0].m_method << _T("/") << ta[1].m_method << _T("/") << ta[2].m_method
               << NEWLINE;
      dataFile.flush();

      tcout    << iparam(12)  << resultDigits                           << _T(" ")
               << iparam(30)  << y                                      << _T(" ")
               << udparam(4)  << l32.m_timeUsage                        << _T(" ")
               << udparam(4)  << linear32TimeExpected                   << _T(" ")
               << udparam(4)  << l32.m_timeUsage/linear32TimeExpected   << _T(" ")
               << udparam(4)  << l64.m_timeUsage                        << _T(" ")
               << udparam(4)  << l32.m_timeUsage/l64.m_timeUsage        << _T(" ")
               << udparam(4)  << lnt.m_timeUsage                        << _T(" ")
               << udparam(4)  << newtonTimeExpected                     << _T(" ")
               << udparam(4)  << lnt.m_timeUsage/newtonTimeExpected     << _T(" ")
               << ta[0].m_method << _T("/") << ta[1].m_method << _T("/") << ta[2].m_method
               << NEWLINE;
      tcout.flush();

    }
  }
  dataFile.close();
}

void BigRealTestClass::measureQuotRemainder() {
  tofstream oldDataFile("oldQuotRem.dat");
  tofstream newDataFile("newQuotRem.dat");

  const String header = "new Mod  old Mod";

  tcout    << header << NEWLINE;

  const BigReal yStep1 = e(BigReal("165"),-2);
  const BigReal yStep2 = BigReal("2.1234567891");
  const BigReal yStart = e(BIGREAL_1,-50);
  const BigReal yEnd   = e(BIGREAL_1, 50);

  for(BigReal y = yStart; y < yEnd; y = (y.getLength() == 1) ? (y * yStep1) : (y * yStep2)) {
    Array<BigReal> xArray, yArray;
    const int xLength = randInt(20,10000);
    for(int i = 0; i < 11; i++) {
      xArray.add(getRandomDividend(xLength));
    }

    yArray.add(y);

    const BigReal f;

    TimeUsageMethod oldQR("oldQR", measureTime(MeasureBinaryOperator(OPERATOR_MOD     , xArray, yArray, f), MEASURE_REALTIME));
    TimeUsageMethod newQR("newQR", measureTime(MeasureBinaryOperator(NEW_OPERATOR_MOD , xArray, yArray, f), MEASURE_REALTIME));

    const double &oldQRTime = oldQR.m_timeUsage;
    const double &newQRTime = newQR.m_timeUsage;

    oldDataFile << iparam(10)  << y.getLength()        << _T(" ")
                << udparam(4)  << oldQRTime
                << NEWLINE;

    newDataFile << iparam(10)  << y.getLength()        << _T(" ")
                << udparam(4)  << newQRTime
                << NEWLINE;

    tcout       << iparam(10)  << y.getLength()        << _T(" ")
                << udparam(4)  << newQRTime            << _T(" ")
                << udparam(4)  << oldQRTime            << _T(" ")
                << ((newQRTime < oldQRTime) ? _T("<-") : _T("  "))
                << NEWLINE;
    tcout.flush();

  }
  newDataFile.close();
  oldDataFile.close();
}

void BigRealTestClass::testQuotRemainder() {
  DigitPool *pool = getDigitPool();

#define TRYLOOP

#ifdef TRYLOOP

  FILE *ErrorFile = FOPEN("c:\\temp\\QRErrors.txt", "w");
  FILE *OKFile      = FOPEN("c:\\temp\\QROk.txt", "w");

  const BigReal xStep1("1.235");
  const BigReal xStep2("2.12234191");
  const BigReal yStep1("1.621");
  const BigReal yStep2("2.567891");
  const BigReal xStart = e(BIGREAL_1,-50, pool);
  const BigReal xEnd   = e(BIGREAL_1, 50, pool);
  const BigReal yStart = e(BIGREAL_1,-50, pool);
  const BigReal yEnd   = e(BIGREAL_1, 50, pool);

  for(FullFormatBigReal x(xStart, pool); x < xEnd; x *= ((x.getLength() < 4) ? xStep1 : yStep2)) {
    for(FullFormatBigReal y(yStart, pool); y < yEnd; y *= ((y.getLength() < 4) ? yStep1 : yStep2)) {

      _tprintf(_T("x,y:(%40s,%40s)\r"), toString(x,33,40,ios::scientific).cstr(), toString(y,33,40,ios::scientific).cstr());

      BigInt            quotient(pool),  quotient1( pool);
      FullFormatBigReal  remainder(pool), remainder1(pool);

      quotRemainder( x, y, &quotient,  &remainder );
      quotRemainder1(x, y, &quotient1, &remainder1);

      const bool qerror = quotient1  != quotient;
      const bool rerror = remainder1 != remainder;

      FullFormatBigReal(pool), qdiff,rdiff(pool);
      FILE *f;
      if(qerror || rerror) {
        f = ErrorFile;
        qdiff = quotient1  - quotient;
        rdiff = remainder1 - remainder;
      } else {
        f = OKFile;
      }

      String header;
      if(qerror || rerror) {
        if(qerror) header += _T("Q-error ");
        if(rerror) header += _T("R-error");
      } else {
        header = _T("OK");
      }

      _ftprintf(f, _T("%s\nX:%s\nY:%s\nnew-Q:%s\nold-Q:%s\nnew-R:%s\nold-R:%s\n%s%s\n")
               ,header.cstr()
               ,x.toString().cstr(), y.toString().cstr()
               ,quotient1.toString().cstr()
               ,quotient.toString().cstr()
               ,remainder1.toString().cstr()
               ,remainder.toString().cstr()
               ,(qerror?format(_T("qdiff:%s\n"), qdiff.toString().cstr()).cstr():_T(""))
               ,(rerror?format(_T("rdiff:%s\n"), rdiff.toString().cstr()).cstr():_T(""))
               );
    }
  }
  fclose(OKFile    );
  fclose(ErrorFile);


#else

  for(;;) {
    const FullFormatBigReal x = inputBigReal(*pool, _T("Enter x:"));
    const FullFormatBigReal y = inputBigReal(*pool, _T("Enter y:"));
    BigInt            quotient1( pool), quotient( pool);
    FullFormatBigReal  remainder1(pool), remainder(pool);

    quotRemainder(x, y, &quotient, &remainder);

    _tprintf(_T("x:%50s y:%50s\n"), x.toString().cstr(), y.toString().cstr());

    quotRemainder1(x, y, &quotient1, &remainder1);
    if((quotient1 != quotient) || (remainder1 != remainder)) {
      _tprintf(_T("fejl:\nnew quotient1 :%-30s old quotient1 :%-30s\nnew remainder1:%-30s old remainder1:%-30s\n")
            ,quotient1.toString().cstr()  ,quotient.toString().cstr()
            ,remainder1.toString().cstr() ,remainder.toString().cstr()
            );
    } else {
      _tprintf(_T("Ok - q=%s, remainder1=%s\n"), quotient1.toString().cstr(), remainder1.toString().cstr());
    }
  }
#endif

}

void BigRealTestClass::testMultiThreadedProduct() {
  FullFormatBigReal x = BigReal::random(4000000);
  FullFormatBigReal y = BigReal::random(4000000);

  AllTime startTime;
  FullFormatBigReal z = prod(x, y, 0);

  const AllTime timeUsage = AllTime() - startTime;

  _tprintf(_T("TimeUsage:%s\n"), timeUsage.toString(SEC).cstr());

  FILE *f = FOPEN(_T("c:\\temp\\multiProd.txt"), _T("w"));
  _ftprintf(f, _T("x:")); x.print(f); _ftprintf(f, _T("\n"));
  _ftprintf(f, _T("y:")); y.print(f); _ftprintf(f, _T("\n"));
  _ftprintf(f, _T("z:")); z.print(f); _ftprintf(f, _T("\n"));
  fclose(f);
  _tprintf(_T("Max active Thread:%d\n"), BigRealThreadPool::getInstance().getMaxActiveThreads());
}


void BigRealTestClass::testLnEstimate() {
  tofstream sStd("lnStd.dat"   );
  tofstream sNum("lnNum.dat"   );
//  tofstream sD64("lnD64.dat"   );
  tofstream sNumDiff("lnNumDiff.dat");
//  tofstream sD64Diff("lnD64Diff.dat");

  for(double i = 1; i <= 10+1e-6; i+=0.01) {
    const BigReal lnStd(log(i));
    const BigReal lnNum(BigReal::lnEstimate( i));
//    const BigReal lnD64(BigReal::lnEstimateD(i));
    sStd << i << _T(" ") << lnStd << NEWLINE;
    sNum << i << _T(" ") << lnNum << NEWLINE;
//    sD64 << i << _T(" ") << lnD64 << NEWLINE;
    const BigReal errNum(lnNum - lnStd);
//    const BigReal errD64(lnD64 - lnStd);
    sNumDiff << i << _T(" ") << errNum << NEWLINE;
//    sD64Diff << i << _T(" ") << errD64 << NEWLINE;
  }
  sStd.close();
  sNum.close();
//  sD64.close();
  sNumDiff.close();
//  sD64Diff.close();
}

void BigRealTestClass::testFullFormat() {
  for(int i = -20; i < 20; i++) {
    for(int j = 0; j < 35; j++) {
      BigReal x;
      do {
        x = e(BigReal::random(j),i);
        if(j == 0)
          break;
      } while(BigReal::getExpo10(x) != i-1);
      if(!x.isZero()) {
        BigInt lx(floor(log(BigReal(10),x,BigReal("1e-10"))));
        tcout << _T("lx:") << lx << _T(" i:") << i << _T("\n");
        continue;
      }
      tcout << _T("i,j:");
      tcout.width(3); tcout << i << _T(",");
      tcout.width(2); tcout << j << _T("):");
      tcout << FullFormatBigReal(x) << _T(" expo:") << BigReal::getExpo10(x) << _T(" length:") << x.getLength() << _T("\n");
      tcout.flush();
      BigRealStream s1;
      s1 << FullFormatBigReal(x);
      BigReal y(s1.cstr());
      if(x != y) {
        tcout << _T("fullformat:") << FullFormatBigReal(x) << _T("\n");
        tcout.flush();
        _tprintf(_T("Error:"));
        x.print(); _tprintf(_T("<>"));  y.print();
        _tprintf(_T("\n"));
        fflush(stdout);
        throwException(_T("Error in testFullFormat"));
      }
    }
  }
}

void BigRealTestClass::testGetDouble() {
  for(;;) {
    BigReal x;
    tcout << _T("enter BigReal:");
    tcin >> x;
    if(x.isZero()) break;
    double dx = getDouble(x);
    tcout.precision(17);
    tcout << dx << _T("\n");
    BigReal y(dx);
    if(x != y) {
      tcout << _T("y = ") << FullFormatBigReal(y) << _T("\n") << _T("x = ") << FullFormatBigReal(x) << _T("\n");
      tcout << _T("BigReal(x.getDouble()) != x\n");
    }
  }
}

void BigRealTestClass::testBigRealStream() {
  Array<BigReal> a;
  for(int i = 0; i < 10; i++) {
    a.add(BigReal::random(1000));
  }
  a.add(0);

  const String fileName = _T("c:\\temp\\testnumberStream.dat");
  a.save(ByteOutputFile(fileName));
  Array<BigReal> b;
  b.load(ByteInputFile(fileName));
  verify(b == a);
}

#ifdef __NEVER__
void BigRealTestClass::testCopy() {
  for(int length = 1; length < 10; length++) {
    FullFormatBigReal src = e(BigReal::random(10+4*length),length*2);
    FullFormatBigReal dst;
    copy(dst,src, length);
    dst.assertIsValidBigReal();
    _tprintf(_T("length:%d, src:%-50s, dst:%-50s\n"),length, src.toString(true).cstr(), dst.toString(true).cstr());
  }
}
#endif

void BigRealTestClass::testCopyAllDigits() {
  for(;;) {
    TCHAR line[200];
    _tprintf(_T("Enter number:")); GETS(line);
    BigReal x(line);
    if(x.isZero()) {
      continue;
    }
    BigReal y;
    y.copyAllDigits(x);
    y.m_expo     = x.m_expo;
    y.m_low      = x.m_low;
    y.m_negative = x.m_negative;
    try {
      y.assertIsValidBigReal();
    } catch(Exception e) {
      _tprintf(_T("%s\n"), e.what());
      continue;
    }

    SETBIGREALDEBUGSTRING(y);
    _tprintf(_T("x:%s\n"), x.toString().cstr());
    _tprintf(_T("y:%s\n"), y.toString().cstr());
    if(x != y) {
      _tprintf(_T("x != y\n"));
    }
  }
}

#define TEST_TRUNC 1
#define TEST_ROUND 2

#define CUT_TEST TEST_ROUND

#if(CUT_TEST == TEST_TRUNC)
#define RF(d)     rTrunc(d)
#define REFF(x,d) trunc(x,d)
#define RTXT      _T("rTrunc")
#define REFTXT    _T("trunc")

#else
#define RF(d)     rRound(d)
#define REFF(x,d) round(x,d)
#define RTXT      _T("rRound")
#define REFTXT    _T("round")
#endif

//#define INTERACTIVE

void BigRealTestClass::testTruncRound() {
  DigitPool *pool = getDigitPool();

#ifdef INTERACTIVE
  for(;;) {
    const FullFormatBigReal x         = inputBigReal(*pool, _T("Enter x:"));
    const int              digits    = inputInt(_T("Enter digits:"));
    const int              decDigits = BigReal::getExpo10(x) + 1;

    FullFormatBigReal x1(x, pool), xRef(pool);
    try {
      x1.RF(digits);
      xRef = REFF(x, digits - decDigits);
      x1.assertIsValidBigReal();
      if(x1 != xRef) {
        _tprintf(_T("x.%s(%d) gives another result than %s(x,%d)\nx.%s(%d) = %s\n%s(x,%d)=%s\n")
              ,RTXT,digits
              ,REFTXT,digits - decDigits
              ,RTXT,digits,x1.toString().cstr()
              ,REFTXT,digits - decDigits, xRef.toString().cstr()
              );
      }
    } catch(Exception e) {
      _tprintf(_T("%s\n"), e.what());
      continue;
    }

    _tprintf(_T("x.%s(%d) = %s\n"), RTXT, digits, x1.toString().cstr());
  }
#else
  _tprintf(_T("%8s %6s %6s %40s %30s %30s\n"), _T("cutDigit"), _T("expo10"), _T("length"), _T("x"), REFTXT, RTXT);

  Random rnd;
  rnd.randomize();
  int testCount = 0, exceptionCount = 0, mismatchCount = 0, okCount = 0;
  for(int cutDigits = 1; cutDigits < 35; cutDigits++) {
    for(int length = 1; length < 50; length++) {
      for(int expo10 = -10; expo10 < 20; expo10++) {
        BigReal x(pool);
        do {
          x = e(BigReal::random(length, &rnd, pool), expo10);
        } while(x.isZero());

        const BRExpoType decDigits = BigReal::getExpo10(x) + 1;

        BigReal x1(x);
        BigReal X1(pool);
        try {
          x1.RF(cutDigits);
          testCount++;
          x1.assertIsValidBigReal();
        } catch(Exception e) {
          exceptionCount++;
          _tprintf(_T("%8d %6d %6d %40s Expt in %s:%s\n")
                ,cutDigits, expo10, length
                ,FullFormatBigReal(x).toString().cstr()
                ,RTXT, e.what()
                );
          BigReal x3(x);
          x3.RF(cutDigits);

          continue;
        }
        X1 = REFF(x, cutDigits - decDigits);

        if(x1 != X1) {
          mismatchCount++;
          _tprintf(_T("%8d %6d %6d %40s %30s %30s\n")
                 ,cutDigits, expo10, length
                 ,FullFormatBigReal(x).toString().cstr()
                 ,FullFormatBigReal(X1).toString().cstr()
                 ,FullFormatBigReal(x1).toString().cstr()
                 );
          X1 = REFF(x, cutDigits - decDigits);
          BigReal x3(x);
          x3.RF(cutDigits);
          continue;
        }
        okCount++;
      }
    }
  }
  _tprintf(_T("BigReals tested:%s, Ok=%s"), format1000(testCount).cstr(), format1000(okCount).cstr());
  if(okCount < testCount) {
    _tprintf(_T(" Exceptions:%s ;Mismatches:%s\n"), format1000(exceptionCount).cstr(), format1000(mismatchCount).cstr());
  } else {
    _tprintf(_T("\n"));
  }
#endif
}

void BigRealTestClass::testCopyrTrunc() {
  DigitPool *pool = getDigitPool();

#ifdef INTERACTIVE
  for(;;) {
    const FullFormatBigReal x         = inputBigReal(*pool, _T("Enter x:"));
    const int              digits    = inputInt(_T("Enter digits:"));

    FullFormatBigReal x1(1, pool), xRef(x, pool);
    try {
      xRef.rTrunc(digits);
      xRef.assertIsValidBigReal();
      x1.copyrTrunc(x, digits);
      x1.assertIsValidBigReal();
      if(x1 != xRef) {
        _tprintf(_T("x1.copyrTrunc(x,%d) gives another result than xRef.rTrunc(%d)\nx = %s\n, x1.copyrTunc(x,%d) = %s\nxRef.rTrunc(%d) = %s\n")
                ,digits
                ,digits
                ,x.toString().cstr()
                ,digits, x1.toString().cstr()
                ,digits, xRef.toString().cstr()
                );
      }
    } catch(Exception e) {
      _tprintf(_T("%s\n"), e.what());
      continue;
    }

    _tprintf(_T("x.%s(%d) = %s\n"), RTXT, digits, x1.toString().cstr());
  }
#else
  _tprintf(_T("%8s %6s %6s %40s %30s %30s\n"), _T("cutDigit"), _T("expo10"), _T("length"), _T("x"), REFTXT, RTXT);

  Random rnd;
  rnd.randomize();
  int testCount = 0, exceptionCount = 0, mismatchCount = 0, okCount = 0;
  for(int cutDigits = 1; cutDigits < 35; cutDigits++) {
    for(int length = 1; length < 50; length++) {
      for(int expo10 = -10; expo10 < 20; expo10++) {
        FullFormatBigReal x(pool);
        do {
          x = e(BigReal::random(length, &rnd, pool), expo10);
        } while(x.isZero());

        const BRExpoType decDigits = BigReal::getExpo10(x) + 1;

        FullFormatBigReal x1(x);
        FullFormatBigReal x2(BigReal("1230139182731039182371230912837", pool), pool);
        FullFormatBigReal xz(pool);
        FullFormatBigReal ref(x);
        ref.rTrunc(cutDigits);
        testCount++;
        try {
          x1.copyrTrunc(x, cutDigits);
          x1.assertIsValidBigReal();
        } catch(Exception e) {
          exceptionCount++;
          _tprintf(_T("%8d %6d %6d %40s Expt in x1:%s\n")
                ,cutDigits, expo10, length
                ,x.toString().cstr()
                ,e.what()
                );

          continue;
        }

        try {
          x2.copyrTrunc(x, cutDigits);
          x2.assertIsValidBigReal();
        } catch(Exception e) {
          exceptionCount++;
          _tprintf(_T("%8d %6d %6d %40s Expt in x2:%s\n")
                ,cutDigits, expo10, length
                ,x.toString().cstr()
                ,e.what()
                );

          continue;
        }

        try {
          xz.copyrTrunc(x, cutDigits);
          xz.assertIsValidBigReal();
        } catch(Exception e) {
          exceptionCount++;
          _tprintf(_T("%8d %6d %6d %40s Expt in xz:%s\n")
                ,cutDigits, expo10, length
                ,x.toString().cstr()
                ,e.what()
                );

          continue;
        }

        if(x1 != ref) {
          mismatchCount++;
          _tprintf(_T("%8d %6d %6d %40s %30s %30s\n")
                  ,cutDigits, expo10, length
                  ,x.toString().cstr()
                  ,x1.toString().cstr()
                  ,ref.toString().cstr()
                  );
          continue;
        }
        if(x2 != ref) {
          mismatchCount++;
          _tprintf(_T("%8d %6d %6d %40s %30s %30s\n")
                  ,cutDigits, expo10, length
                  ,x.toString().cstr()
                  ,x2.toString().cstr()
                  ,ref.toString().cstr()
                  );
          continue;
        }
        if(xz != ref) {
          FullFormatBigReal xz1(pool);
          xz1.copyrTrunc(x, cutDigits);
          mismatchCount++;
          _tprintf(_T("%8d %6d %6d %40s %30s %30s\n")
                  ,cutDigits, expo10, length
                  ,x.toString().cstr()
                  ,xz.toString().cstr()
                  ,ref.toString().cstr()
                  );
          continue;
        }
        okCount++;
      }
    }
  }
  _tprintf(_T("BigReals tested:%s, Ok=%s"), format1000(testCount).cstr(), format1000(okCount).cstr());
  if(okCount < testCount) {
    _tprintf(_T(" Exceptions:%s ;Mismatches:%s\n"), format1000(exceptionCount).cstr(), format1000(mismatchCount).cstr());
  } else {
    _tprintf(_T("\n"));
  }
#endif
}

void BigRealTestClass::testFractionate() {
  for(;;) {
    TCHAR line[200];
    _tprintf(_T("Enter number:")); GETS(line);
    BigReal  x(line);
    BigInt  intPart;
    BigReal  fraction;
    x.fractionate(&intPart, &fraction);

    _tprintf(_T("intPart :%s\n"), intPart.toString().cstr());
    _tprintf(_T("fracpart:%s\n"), fraction.toString().cstr());

    x.fractionate(&intPart, NULL);
    _tprintf(_T("intPart1:%s\n"), intPart.toString().cstr());

    x.fractionate(NULL, &fraction);
    _tprintf(_T("fracpart1:%s\n"), fraction.toString().cstr());

    intPart  = BigReal(_T("20202020202020202020202020202.20202020202"));
    fraction = BigReal(_T("10101010101010101010101010101.10101010101"));

    x.fractionate(&intPart, NULL);
    _tprintf(_T("intPart2:%s\n"), intPart.toString().cstr());

    x.fractionate(NULL, &fraction);
    _tprintf(_T("fracpart2:%s\n"), fraction.toString().cstr());

  }
}

void BigRealTestClass::testGetDecimalDigitCount64() {
  for(;;) {
    _tprintf(_T("indtast tal mellem 1 og %I64u:"), 9999999999999999999ui64);
    TCHAR line[1000];
    GETS(line);
    unsigned __int64 n;
    if(_stscanf(line,_T("%I64u"), &n) != 1) {
      _tprintf(_T("ikke et tal"));
    }
    const int dc = BigReal::getDecimalDigitCount64(n);
    _tprintf(_T("%I64u indeholder %d cifre\n"), n, dc);
  }
}


MeasureBinaryOperator::MeasureBinaryOperator(BinaryOperator op, const Array<BigReal> &x, const Array<BigReal> &y, const BigReal &f)
: m_op(op)
, m_x(x)
, m_y(y)
, m_f(f)
{
  BigRealThreadPool &poolCreator = BigRealThreadPool::getInstance();
  m_pool = poolCreator.fetchDigitPool();
  i = j = 0;
}

MeasureBinaryOperator::~MeasureBinaryOperator() {
  BigRealThreadPool &poolCreator = BigRealThreadPool::getInstance();
  poolCreator.releaseDigitPool(m_pool);
}

void MeasureBinaryOperator::f() {
  const BigReal &X = m_x[i];
  const BigReal &Y = m_y[j];

  switch(m_op) {
  case SUM               : m_result =         sum(         X,Y,m_f, m_pool); break;
  case DIF               : m_result =         dif(         X,Y,m_f, m_pool); break;
  case PROD              : m_result =         prod(        X,Y,m_f, m_pool); break;
  case SHORTPROD         : m_result = BigReal::shortProd(   X,Y,m_f, m_pool); break;
  case QUOTNEWTON        : m_result = BigReal::quotNewton(  X,Y,m_f, m_pool); break;
  case QUOTLINEAR32      : m_result = BigReal::quotLinear32(X,Y,m_f, m_pool); break;
  case QUOTLINEAR64      : m_result = BigReal::quotLinear64(X,Y,m_f, m_pool); break;
  case QUOTREMAINDER     :            quotRemainder(   X,Y, &m_intResult, &m_remainder); break;
  case QUOTREMAINDER1    :            quotRemainder1(  X,Y, &m_intResult, &m_remainder); break;
  case OPERATOR_MOD      : m_result = X % Y; break;
  case NEW_OPERATOR_MOD  : m_result = newModulusOperator(X,Y); break;

  default                : throwException(_T("Invalid operator. (=%d)"), m_op);
                           break;
  }
  i = (i+2) % m_x.size();
  j = (j+3) % m_y.size();
}

