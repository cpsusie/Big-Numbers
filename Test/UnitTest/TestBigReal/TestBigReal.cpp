#include "stdafx.h"
#include <CPUInfo.h>
#include <Console.h>
#include "FunctionTest.h"
#include "TestBigReal.h"

static void testConstructors() {
  TesterJob::addFunctionTest(new RawFunctionTest(_T("Constructors"  ), testConstructors              ));
}

static void testConversionsFloat() {
  TesterJob::addFunctionTest(new RawFunctionTest(_T("conv.Flt-"     ), testNegativeFloatConversion   ));
  TesterJob::addFunctionTest(new RawFunctionTest(_T("conv.Flt+"     ), testPositiveFloatConversion   ));
}

static void testConversionsDouble() {
  TesterJob::addFunctionTest(new RawFunctionTest(_T("conv.Dbl-"     ), testNegativeDoubleConversion  ));
  TesterJob::addFunctionTest(new RawFunctionTest(_T("conv.Dbl+"     ), testPositiveDoubleConversion  ));
}

static void testConversionsDouble80() {
  TesterJob::addFunctionTest(new RawFunctionTest(_T("conv.Dbl80-"   ), testNegativeDouble80Conversion));
  TesterJob::addFunctionTest(new RawFunctionTest(_T("conv.Dbl80+"   ), testPositiveDouble80Conversion));
}

static void testConversions() {
  testConversionsFloat();
  testConversionsDouble();
  testConversionsDouble80();
}

static void testGetFirst() {
  TesterJob::addFunctionTest(new RawFunctionTest(_T("getFirst"), testGetFirst));
}

static void testGetDecimalDigitCount() {
  TesterJob::addFunctionTest(new RawFunctionTest(_T("getDecimalDigitCount"), testGetDecimalDigitCount));
}

static void testGetExpo10() {
  TesterJob::addFunctionTest(new RawFunctionTest(_T("getExpo10"), testGetExpo10));
}

static void testGetExpo2() {
  TesterJob::addFunctionTest(new RawFunctionTest(_T("getexpo2"), testGetExpo2));
}

static void testMultPow10() {
  TesterJob::addFunctionTest(new RawFunctionTest(_T("multPow10"), testMultPow10));
}

static void testAPCsum() {
  TesterJob::addFunctionTest(new RawFunctionTest(_T("APCSum"), testAPCSum));
}

static void testAPCprod() {
  TesterJob::addFunctionTest(new RawFunctionTest(_T("APCProd"), testAPCProd));
}

static void testAPCquot() {
  TesterJob::addFunctionTest(new RawFunctionTest(_T("APCQuot"), testAPCQuot));
}

static void testAPCpow() {
  TesterJob::addFunctionTest(new RawFunctionTest(_T("APCPow"), testAPCPow));
}

#pragma check_stack(off)

static inline double sum(double x, double y) {
  return x + y;
}

static inline double diff(double x, double y) {
  return x - y;
}

static inline double prod(double x, double y) {
  return x * y;
}

static inline double quot(double x, double y) {
  return x / y;
}

static inline Double80 sum(const Double80 &x, const Double80 &y) {
  return x+y;
}

static inline Double80 diff(const Double80 &x, const Double80 &y) {
  return x-y;
}

static inline Double80 prod(const Double80 &x, const Double80 &y) {
  return x*y;
}

static inline Double80 quot(const Double80 &x, const Double80 &y) {
  return x/y;
}

#pragma check_stack()

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

static void testQuot3() {
  TesterJob::addFunctionTest(new RawFunctionTest(_T("quot(Newton-L32-L64)"), testQuot3));
}

static void testAssignOperators() {
  TesterJob::addFunctionTest(new RawFunctionTest(_T("assignop"), testAssignOperators));
}

static void testQuotLinear32() {
  testOperator(_T("quotLinear32"), BigReal::quotLinear32);
}

static void testQuotLinear64() {
  testOperator(_T("quotLinear64"), BigReal::quotLinear64);
}

static void testIntegerDivision() {
  TesterJob::addFunctionTest(new RawFunctionTest(_T("Integer division"), testIntegerDivision));
}

static bool checkPlus(const BigReal &x, const BigReal &y, const BigReal &result) {
  return dif(result,y,BIGREAL_0) == x;
}

static bool checkMinus(const BigReal &x, const BigReal &y, const BigReal &result) {
  return sum(result,y,BIGREAL_0) == x;
}

static void testOperatorPlus() {
  testExactBinaryOperator( _T("operatorPlus"),operator+,checkPlus);
}

static void testOperatorMinus() {
  testExactBinaryOperator( _T("operatorMinus"),operator-,checkMinus);
}

static void testModulus() {
  TesterJob::addFunctionTest(new RawFunctionTest(_T("modulus"), testModulus));
}

static bool checkModulus(const BigReal &x, const BigReal &y, const BigReal &result) {
  DigitPool *pool = x.getDigitPool();
  BigInt n(pool);
  BigReal rem(pool);
  quotRemainder(x,y, &n, &rem);
  return (rem == result)
      && (sign(result) == sign(x))
      && (!n.isNegative())
      && (compareAbs(result,y) < 0)
      && (x.isPositive() ? (fabs(y) * n + result == x) : (-fabs(y) * n + result == x));
}

static void testOperatorModulus() {
  testExactBinaryOperator( _T("operatorMod"),operator%,checkModulus);
}

static void testExactBinaryOperators() {
  testOperatorPlus();
  testOperatorMinus();
  testOperatorModulus();
  testModulus();
  testIntegerDivision();
}

static void testSqrt() {
  const BigReal from = 0;
  const BigReal to   = 100;
  testFunction(_T("sqrt" ), true, from, to, sqrt, sqrt, sqrt);
  testFunction(_T("rSqrt"), true, from, to, rSqrt);
}

static void testExp() {
  const BigReal from = -200;
  const BigReal to   = 100;
  testFunction(_T("exp" ), false, from, to, exp, exp, exp);
  testFunction(_T("rExp"), false, from, to, rExp);
}

static void testLn() {
  const BigReal from = e(1, -100);
  const BigReal to   = 100;
  testFunction(_T("ln" ), true, from, to, ln, log, log);
  testFunction(_T("rLn"), true, from, to, rLn);
}

static void testLog10() {
  const BigReal from = e(1,-100);
  const BigReal to   = 100;
  testFunction(_T("log10" ), true, from, to, log10, log10, log10);
  testFunction(_T("rLog10"), true, from, to, rLog10);
}

static void testLn1() {
  const BigReal from = e(1, -100);
  const BigReal to   = 100;
  testFunction(_T("ln1"), true, from, to, ln1);
}

static void testLog() {
  const BigReal baseFrom = 2;
  const BigReal baseTo   = 10;
  const BigReal yFrom    = e(1, -100);
  const BigReal yTo      = 100;
  testFunction(_T("log" ), false, baseFrom, baseTo, true, yFrom, yTo, log);
  testFunction(_T("rLog"), false, baseFrom, baseTo, true, yFrom, yTo, rLog);
}

static void testPow() {
  const BigReal baseFrom = 2;
  const BigReal baseTo   = 10;
  const BigReal yFrom    = -100;
  const BigReal yTo      = 100;
  testFunction(_T("pow" ), false, baseFrom, baseTo, false, yFrom, yTo, pow, pow, pow);
  testFunction(_T("rPow"), false, baseFrom, baseTo, false, yFrom, yTo, rPow);
}

static void testPi() {
  TesterJob::addFunctionTest(new RawFunctionTest(_T("Pi"), testPi));
}

static void testSin() {
  const BigReal from = -100;
  const BigReal to   = 100;
  testFunction(_T("sin" ), false, from, to, sin, sin, sin);
  testFunction(_T("rSin"), false, from, to, rSin);
}

static void testCosPiThird(TestStatistic &stat) {
  DigitPool *pool = stat.getDigitPool();
  const BigReal tolerance = e(pool->get1(), -1000);
  const FullFormatBigReal cosPiThird = cos(quot(pi(tolerance), BigReal(3,pool), tolerance), tolerance) - pool->getHalf();
  verify(compareAbs(cosPiThird,  tolerance) < 1);
  stat.setEndMessageToOk();
}

static void testCos() {
  const BigReal from = -100;
  const BigReal to   = 100;
  testFunction(_T("cos" ), false, from, to, cos, cos, cos);
  testFunction(_T("rCos"), false, from, to, rCos);
  TesterJob::addFunctionTest(new RawFunctionTest(_T("cos(pi/3)"), testCosPiThird));
}

static void testTan() {
  const BigReal from = -100;
  const BigReal to   = 100;
  testFunction(_T("tan" ), false, from, to, tan, tan, tan);
  testFunction(_T("rTan"), false, from, to, rTan);
}

static void testCot() {
  const BigReal from = -99;
  const BigReal to   = 100;
  testFunction(_T("cot" ), false, from, to, cot, cot, cot);
  testFunction(_T("rCot"), false, from, to, rCot);
}

static void testAsin() {
  const BigReal from = -1;
  const BigReal to   = 1;
  testFunction(_T("asin" ), false, from, to, asin, asin, asin);
  testFunction(_T("rAsin"), false, from, to, rAsin);
}

static void testAcos() {
  const BigReal from = -1;
  const BigReal to   = 1;
  testFunction(_T("acos" ), false, from, to, acos, acos, acos);
  testFunction(_T("rAcos"), false, from, to, rAcos);
}

static void testAtan() {
  const BigReal from = -99;
  const BigReal to   = 100;
  testFunction(_T("atan" ), false, from, to, atan, atan, atan);
  testFunction(_T("rAtan"), false, from, to, rAtan);
}

static void testAcot() {
  const BigReal from = -99;
  const BigReal to   = 100;
  testFunction(_T("acot" ), false, from, to, acot, acot, acot);
  testFunction(_T("rAcot"), false, from, to, rAcot);
}

static void testReadWriteBigReal() {
  TesterJob::addFunctionTest(new RawFunctionTest(_T("read-write number"), testReadWriteBigReal));
}

static void testReadWriteInteger() {
  TesterJob::addFunctionTest(new RawFunctionTest(_T("read-write integer"), testReadWriteInteger));
}

static double getPiTimeEstimate(int decimals) {
  return ((7.98881e-7 * decimals - 0.00109247) * decimals + 64.2134) * decimals - 70058.7;
}

//static DigitMonitorThread digitMonitor;

void testBigReal(int threadCount) {
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
  winH --;
  Console::setWindowAndBufferSize(0,0,winW, winH);

  Console::clear();

  tcout << _T("Testing BigReal.") << endl;

  log(_T("Begin test %s"), getCompileArchitectureSignatureString().cstr());

#ifndef _DEBUG
  randomize();
#endif

  AllTime startTime;

  if(!isDebugLogRedirected()) {
    redirectDebugLog();
  }

//StartAll:

  testConstructors();

  testConversions();
  testGetFirst();

  testGetDecimalDigitCount();
  testGetExpo10();
  testMultPow10();

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

  testSqrt();
  testExp();
  testLn();
  testLog10();
  testLog();
  testPow();

  testPi();

  testSin();
  testCos();
  testTan();
  testCot();
  testAsin();
  testAcos();
  testAtan();
  testAcot();

  testReadWriteBigReal();
  testReadWriteInteger();

//  TesterJob::shuffleTestOrder();

  if(threadCount <= 0) {
    threadCount = getProcessorCount();
  }

  TesterJob::startAll(threadCount);
  TesterJob::waitUntilAllDone();
  TesterJob::releaseAll();

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
