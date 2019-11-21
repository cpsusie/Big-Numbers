#include "stdafx.h"
#include <CPUInfo.h>
#include <Console.h>
#include "FunctionTest.h"
#include "TestBigReal.h"

static void testConstructors() {
  TesterJob::addFunctionTest(new RawFunctionTest(_T("Constructors"  ), testConstructors              ));
}

#define CONV_FLT32P 0x01
#define CONV_FLT32N 0x02
#define CONV_DBL64P 0x04
#define CONV_DBL64N 0x08
#define CONV_DBL80P 0x10
#define CONV_DBL80N 0x20

#define CONV_FLT32 (CONV_FLT32P|CONV_FLT32N)
#define CONV_DBL64 (CONV_DBL64P|CONV_DBL64N)
#define CONV_DBL80 (CONV_DBL80P|CONV_DBL80N)

#define CONV_ALL (CONV_FLT32 | CONV_DBL64 | CONV_DBL80)

static void testConversions(BYTE flags = CONV_ALL) {
  if(flags&CONV_FLT32P) TesterJob::addFunctionTest(new RawFunctionTest(_T("conv.Flt32+"), testPositiveFloat32Conversion ));
  if(flags&CONV_FLT32N) TesterJob::addFunctionTest(new RawFunctionTest(_T("conv.Flt32-"), testNegativeFloat32Conversion ));
  if(flags&CONV_DBL64P) TesterJob::addFunctionTest(new RawFunctionTest(_T("conv.Dbl64+"), testPositiveDouble64Conversion));
  if(flags&CONV_DBL64N) TesterJob::addFunctionTest(new RawFunctionTest(_T("conv.Dbl64-"), testNegativeDouble64Conversion));
  if(flags&CONV_DBL80P) TesterJob::addFunctionTest(new RawFunctionTest(_T("conv.Dbl80+"), testPositiveDouble80Conversion));
  if(flags&CONV_DBL80N) TesterJob::addFunctionTest(new RawFunctionTest(_T("conv.Dbl80-"), testNegativeDouble80Conversion));
}

static void testGetFirst32()           { TesterJob::addFunctionTest(new RawFunctionTest(_T("getFirst32"          ), testGetFirst32));           }
static void testGetFirst64()           { TesterJob::addFunctionTest(new RawFunctionTest(_T("getFirst64"          ), testGetFirst64));           }
#ifdef IS64BIT
static void testGetFirst128()          { TesterJob::addFunctionTest(new RawFunctionTest(_T("getFirst128"         ), testGetFirst128));          }
#endif
static void testGetDecimalDigitCount() { TesterJob::addFunctionTest(new RawFunctionTest(_T("getDecimalDigitCount"), testGetDecimalDigitCount)); }
static void testPow10()                { TesterJob::addFunctionTest(new RawFunctionTest(_T("pow10"               ), testPow10));                }
static void testIsPow10()              { TesterJob::addFunctionTest(new RawFunctionTest(_T("isPow10"             ), testIsPow10));              }
static void testGetExpo10()            { TesterJob::addFunctionTest(new RawFunctionTest(_T("getExpo10"           ), testGetExpo10));            }
static void testGetExpo2()             { TesterJob::addFunctionTest(new RawFunctionTest(_T("getexpo2"            ), testGetExpo2));             }
static void testMultPow10()            { TesterJob::addFunctionTest(new RawFunctionTest(_T("multPow10"           ), testMultPow10));            }
static void testAPCsum()               { TesterJob::addFunctionTest(new RawFunctionTest(_T("APCSum"              ), testAPCSum));               }
static void testAPCprod()              { TesterJob::addFunctionTest(new RawFunctionTest(_T("APCProd"             ), testAPCProd));              }
static void testAPCquot()              { TesterJob::addFunctionTest(new RawFunctionTest(_T("APCQuot"             ), testAPCQuot));              }
static void testAPCpow()               { TesterJob::addFunctionTest(new RawFunctionTest(_T("APCPow"              ), testAPCPow));               }

#pragma check_stack(off)

static inline double   sum(       double    x,       double    y) { return x + y; }
static inline double   diff(      double    x,       double    y) { return x - y; }
static inline double   prod(      double    x,       double    y) { return x * y; }
static inline double   quot(      double    x,       double    y) { return x / y; }
static inline Double80 sum( const Double80 &x, const Double80 &y) { return x + y; }
static inline Double80 diff(const Double80 &x, const Double80 &y) { return x - y; }
static inline Double80 prod(const Double80 &x, const Double80 &y) { return x * y; }
static inline Double80 quot(const Double80 &x, const Double80 &y) { return x / y; }

#pragma check_stack()

void testGetFirst() {
  testGetFirst32();
  testGetFirst64();
#ifdef IS64BIT
  testGetFirst128();
#endif
}

static void testSum() {
  testOperator(_T("sum" ), sum, sum, sum);
  testOperator(_T("rSum"), rSum);
}

static void testDif() {
  testOperator(_T("dif" ), dif, diff, diff);
  testOperator(_T("rDif"),rDif);
}

static void testProd() {
  testOperator(_T("prod" ), prod, prod, prod);
  testOperator(_T("rProd"),rProd);
}

static void testQuot() {
  testOperator(_T("quot" ), quot, quot, quot);
  testOperator(_T("rQuot"), rQuot);
}

static void testRandBigReal()     { TesterJob::addFunctionTest(new RawFunctionTest(_T("randBigReal"         ), testRandBigReal    )); }
static void testRandBigInt()      { TesterJob::addFunctionTest(new RawFunctionTest(_T("randBigInt"          ), testRandBigInt     )); }
static void testRandBigRational() { TesterJob::addFunctionTest(new RawFunctionTest(_T("randBigRational"     ), testRandBigRational)); }
static void testQuot3()           { TesterJob::addFunctionTest(new RawFunctionTest(_T("quot(Newton-L32-L64)"), testQuot3          )); }
static void testQuotLinear32()    { testOperator(_T("quotLinear32"), BigReal::quotLinear32); }
static void testQuotLinear64()    { testOperator(_T("quotLinear64"), BigReal::quotLinear64); }

static void testAssignOperators() { TesterJob::addFunctionTest(new RawFunctionTest(_T("assignop"            ), testAssignOperators)); }
static void testIntSum()          { TesterJob::addFunctionTest(new RawFunctionTest(_T("Int sum"             ), testIntSum         )); }
static void testIntDif()          { TesterJob::addFunctionTest(new RawFunctionTest(_T("Int dif"             ), testIntDif         )); }
static void testIntProd()         { TesterJob::addFunctionTest(new RawFunctionTest(_T("Int prod"            ), testIntProd        )); }
static void testIntQuot()         { TesterJob::addFunctionTest(new RawFunctionTest(_T("Int quot"            ), testIntQuot        )); }
static void testIntRem()          { TesterJob::addFunctionTest(new RawFunctionTest(_T("Int rem"             ), testIntRem         )); }
static void testMRIsPrime()       { TesterJob::addFunctionTest(new RawFunctionTest(_T("MRisprime"           ), testMRIsPrime      )); }

static bool checkBigRealPlus(const BigReal &x, const BigReal &y, const BigReal &result) {
  return dif(result,y,BigReal::_0) == x;
}

static bool checkBigRealMinus(const BigReal &x, const BigReal &y, const BigReal &result) {
  return sum(result,y,BigReal::_0) == x;
}

static void testOperatorPlus() {
  testExactBinaryOperator( _T("operatorPlus"),operator+,checkBigRealPlus);
}

static void testOperatorMinus() {
  testExactBinaryOperator( _T("operatorMinus"),operator-,checkBigRealMinus);
}

static bool checkBigRealMod(const BigReal &x, const BigReal &y, const BigReal &result) {
  DigitPool *pool = x.getDigitPool();
  BigInt n(pool);
  BigReal rem(pool);
  quotRemainder(x,y, &n, &rem);
  return (rem == result)
      && (sign(result) == sign(x))
      && (!n.isNegative())
      && (BigReal::compareAbs(result,y) < 0)
      && (x.isPositive() ? (fabs(y) * n + result == x) : (-fabs(y) * n + result == x));
}

static void testOperatorModulus() {
  testExactBinaryOperator( _T("operatorMod"),operator%,checkBigRealMod);
}

static void testExactBinaryOperators() {
  testOperatorPlus();
  testOperatorMinus();
  testOperatorModulus();
  testIntSum();
  testIntDif();
  testIntProd();
  testIntQuot();
  testIntRem();
}

static void testSqrt() {
  testFunction(_T("sqrt" ), TestInterval(0,100,true), sqrt, sqrt, sqrt);
  testFunction(_T("rSqrt"), TestInterval(0,100,true), rSqrt);
}

static void testExp() {
  testFunction(_T("exp" ), TestInterval(-200, 100, false), exp, exp, exp);
  testFunction(_T("rExp"), TestInterval(-200, 100, false), rExp);
}

static void testLn() {
  const TestInterval interval(e(1, -100), 100, true);
  testFunction(_T("ln" ), interval, ln, log, log);
  testFunction(_T("rLn"), interval, rLn);
}

static void testLog10() {
  const TestInterval interval(e(1, -100), 100, true);
  testFunction(_T("log10" ), interval, log10, log10, log10);
  testFunction(_T("rLog10"), interval, rLog10);
}

static void testLn1() {
  const TestInterval interval(e(1, -100), 100, true);
  testFunction(_T("ln1"), interval, ln1);
}

static void testLog() {
  const TestInterval baseInterval(2, 10, false);
  const TestInterval yInterval(e(1, -100), 100, true);
  testFunction(_T("log" ), baseInterval, yInterval, log);
  testFunction(_T("rLog"), baseInterval, yInterval, rLog);
}

static void testPow() {
  const TestInterval baseInterval(2  ,  10, false);
  const TestInterval yInterval(  -100, 100, false);
  testFunction(_T("pow" ), baseInterval, yInterval, pow, pow, pow);
  testFunction(_T("rPow"), baseInterval, yInterval, rPow);
}

static void testPi() {
  TesterJob::addFunctionTest(new RawFunctionTest(_T("Pi"), testPi));
}

static void testSin() {
  const TestInterval xInterval(-100, 100, false);
  testFunction(_T("sin" ), xInterval, sin, sin, sin);
  testFunction(_T("rSin"), xInterval, rSin);
}

static void testCosPiThird(TestStatistic &stat) {
  DigitPool *pool = stat.getDigitPool();
  const BigReal tolerance = e(pool->_1(), -1000);
  const FullFormatBigReal cosPiThird = cos(quot(pi(tolerance), BigReal(3,pool), tolerance), tolerance) - pool->_05();
  verify(BigReal::compareAbs(cosPiThird,  tolerance) < 1);
  stat.setEndMessageToOk();
}

static void testCos() {
  const TestInterval xInterval(-100, 100, false);
  testFunction(_T("cos" ), xInterval, cos, cos, cos);
  testFunction(_T("rCos"), xInterval, rCos);
  TesterJob::addFunctionTest(new RawFunctionTest(_T("cos(pi/3)"), testCosPiThird));
}

static void testTan() {
  const TestInterval xInterval(-100, 100, false);
  testFunction(_T("tan" ), xInterval, tan, tan, tan);
  testFunction(_T("rTan"), xInterval, rTan);
}

static void testCot() {
  const TestInterval xInterval(-99, 100, false);
  testFunction(_T("cot" ), xInterval, cot, cot, cot);
  testFunction(_T("rCot"), xInterval, rCot);
}

static void testAsin() {
  const TestInterval xInterval(-1, 1, false);
  testFunction(_T("asin" ), xInterval, asin, asin, asin);
  testFunction(_T("rAsin"), xInterval, rAsin);
}

static void testAcos() {
  const TestInterval xInterval(-1, 1, false);
  testFunction(_T("acos" ), xInterval, acos, acos, acos);
  testFunction(_T("rAcos"), xInterval, rAcos);
}

static void testAtan() {
  const TestInterval xInterval(-99, 100, false);
  testFunction(_T("atan" ), xInterval, atan, atan, atan);
  testFunction(_T("rAtan"), xInterval, rAtan);
}

static void testAcot() {
  const TestInterval xInterval(-99, 100, false);
  testFunction(_T("acot" ), xInterval, acot, acot, acot);
  testFunction(_T("rAcot"), xInterval, rAcot);
}

static void testReadWriteBigReal()     { TesterJob::addFunctionTest(new RawFunctionTest(_T("read-write BigReal"    ), testReadWriteBigReal    )); }
static void testReadWriteBigInt()      { TesterJob::addFunctionTest(new RawFunctionTest(_T("read-write BigInt"     ), testReadWriteBigInt     )); }
static void testReadWriteBigRational() { TesterJob::addFunctionTest(new RawFunctionTest(_T("read-write BigRational"), testReadWriteBigRational)); }

static double getPiTimeEstimate(int decimals) {
  return ((7.98881e-7 * decimals - 0.00109247) * decimals + 64.2134) * decimals - 70058.7;
}

//static DigitMonitorThread digitMonitor;

static void initConsole() {
#ifdef _DEBUG
  tcout << _T("debuggerPresent:") << getDebuggerPresent() << endl;
#endif
  int winW, winH;
  CONSOLE_FONT_INFOEX info = Console::getFont();
  info.dwFontSize.X = 8;
  info.dwFontSize.Y = 12;
  Console::setFontSize(info.dwFontSize);
  Console::getLargestConsoleWindowSize(winW, winH);
  winW -= 10;
  winH--;
  Console::setWindowAndBufferSize(0, 0, winW, winH);
  Console::clear();
}

void testBigReal(int threadCount, bool stopOnError) {
  initConsole();
  tcout << _T("Testing BigReal.") << endl;
  log(_T("Begin test %s"), getCompileArchitectureSignatureString().cstr());

  AllTime startTime;

  if(!isDebugLogRedirected()) {
    redirectDebugLog();
  }

//StartAll:

  testGetDecimalDigitCount();
  testPow10();
  testIsPow10();
  testGetExpo10();
  testMultPow10();
  testGetFirst();

  testConstructors();
  testConversions();

//  testGetExpo2();

  testAPCsum();
  testAPCprod();
  testAPCquot();
  testAPCpow();

  testSum();
  testDif();
  testProd();
  testQuot();
  testQuot3();

  testAssignOperators();
  testQuotLinear32();
  testQuotLinear64();

  testExactBinaryOperators();
  testPi();
  testMRIsPrime();

  testSqrt();
  testExp();
  testLn();
  testLog10();
  testLog();
  testPow();

  testRandBigReal();
  testRandBigInt();
  testRandBigRational();

  testSin();
  testCos();
  testTan();
  testCot();
  testAsin();
  testAcos();
  testAtan();
  testAcot();

  testReadWriteBigReal();
  testReadWriteBigInt();
  testReadWriteBigRational();

//  TesterJob::shuffleTestOrder();

  if(threadCount <= 0) {
    threadCount = getProcessorCount();
  }

  TesterJob::runAll(threadCount, stopOnError);

//  if(TesterJob::allOk()) goto StartAll;

  const AllTime timeUsage1 = AllTime() - startTime;
  const AllTime timeUsage(timeUsage1.getProcessTime(), TesterJob::getTotalThreadTime(), timeUsage1.getRealTime());

//  testLn1();

  TestStatistic::screenlogGotoTop();
  TestStatistic::screenlog(_T("Total time usage:%s Total allocated Digits:%s")
                          ,timeUsage.toString(MMSS).cstr()
                          ,format1000(DigitPool::getTotalAllocatedDigitCount()).cstr()
                          );
  TestStatistic::screenlog(TesterJob::allOk() ? _T("BigReal ok.") : _T("BigReal failed."));

  log(_T("Total time usage:%s Total allocated Digits:%s")
     ,timeUsage.toString(MMSS).cstr()
     ,format1000(DigitPool::getTotalAllocatedDigitCount()).cstr()
     );
  log(_T("End test %s"), getCompileArchitectureSignatureString().cstr());
}

void testDouble80Conversions() {
  initConsole();
  tcout << _T("Testing BigReal.") << endl;

  testConversions(CONV_DBL80P);
  TesterJob::runAll(1, false);
}
