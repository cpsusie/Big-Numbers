#pragma once

#include "TestStatistic.h"
#include "TestInterval.h"

class AbstractFunctionTest {
protected:
  const String m_functionName;
  AbstractFunctionTest(const String &functionName) : m_functionName(functionName) {
  }
public:
  virtual void runTest(int threadId, DigitPool *pool) = 0;
  virtual ~AbstractFunctionTest() {
  }
  const String &getFunctionName() const {
    return m_functionName;
  }
};

class FunctionTest1ArgND64D80 : public AbstractFunctionTest {
private:
  const TestInterval    m_xInterval;
  BigRealFunction1      m_f1;
  DoubleFunction1       m_f1_64;
  Double80Function1     m_f1_80;
public:
  FunctionTest1ArgND64D80(const String        &functionName
                         ,const TestInterval  &xInterval
                         ,BigRealFunction1     f
                         ,DoubleFunction1      f64
                         ,Double80Function1    f80) : AbstractFunctionTest(functionName)
                                                    , m_xInterval(xInterval)
                                                    , m_f1(f), m_f1_64(f64), m_f1_80(f80)
  {
  }
  void runTest(int threadId, DigitPool *pool);
};

class FunctionTest2ArgND64D80 : public AbstractFunctionTest {
private:
  const TestInterval    m_xInterval;
  const TestInterval    m_yInterval;
  BigRealFunction2      m_f2;
  DoubleFunction2       m_f2_64;
  Double80Function2     m_f2_80;
public:
  FunctionTest2ArgND64D80(const String        &functionName
                         ,const TestInterval  &xInterval
                         ,const TestInterval  &yInterval
                         ,BigRealFunction2     f
                         ,DoubleFunction2      f64
                         ,Double80Function2    f80) : AbstractFunctionTest(functionName)
                                                    , m_xInterval(xInterval)
                                                    , m_yInterval(yInterval)
                                                    , m_f2(f), m_f2_64(f64), m_f2_80(f80)
  {
  }
  void runTest(int threadId, DigitPool *pool);
};


class FunctionTest1ArgRelative : public AbstractFunctionTest {
private:
  const TestInterval    m_xInterval;
  rBigRealFunction1     m_rf1;
public:
  FunctionTest1ArgRelative(const String        &functionName
                          ,const TestInterval  &xInterval
                          ,rBigRealFunction1    f) : AbstractFunctionTest(functionName)
                                                   , m_xInterval(xInterval)
                                                   , m_rf1(f)
  {
  }
  void runTest(int threadId, DigitPool *pool);
};

class FunctionTest2ArgRelative : public AbstractFunctionTest {
private:
  const TestInterval    m_xInterval;
  const TestInterval    m_yInterval;
  rBigRealFunction2     m_rf2;
public:
  FunctionTest2ArgRelative(const String        &functionName
                          ,const TestInterval  &xInterval
                          ,const TestInterval  &yInterval
                          ,rBigRealFunction2    f) : AbstractFunctionTest(functionName)
                                                   , m_xInterval(xInterval)
                                                   , m_yInterval(yInterval)
                                                   , m_rf2(f)
  {
  }
  void runTest(int threadId, DigitPool *pool);
};

typedef bool (*BigRealBinaryOperatorCheckFunction)(const BigReal &x, const BigReal &y, const BigReal &result);

class BigRealExactBinaryOperatorTest : public AbstractFunctionTest {
private:
  BigRealBinaryOperator              m_op2;
  BigRealBinaryOperatorCheckFunction m_check;
public:
  BigRealExactBinaryOperatorTest(const String                      &functionName
                                ,BigRealBinaryOperator              op
                                ,BigRealBinaryOperatorCheckFunction check) : AbstractFunctionTest(functionName)
                                                                           , m_op2(op)
                                                                           , m_check(check)
  {
  }
  void runTest(int threadId, DigitPool *pool);
};


class OperatorTest2ArgND64D80Pool : public AbstractFunctionTest {
private:
  BigRealFunction2Pool m_f2Pool;
  DoubleFunction2      m_f2_64;
  Double80Function2    m_f2_80;
  void specialTest(int threadId, DigitPool *pool);
public:
  OperatorTest2ArgND64D80Pool(const String         &functionName
                             ,BigRealFunction2Pool  f
                             ,DoubleFunction2       f64
                             ,Double80Function2     f80) : AbstractFunctionTest(functionName)
                                                         , m_f2Pool(f), m_f2_64(f64), m_f2_80(f80)
  {
  }
  void runTest(int threadId, DigitPool *pool);
};

class OperatorTest2ArgRelative : public AbstractFunctionTest {
private:
  rBigRealFunction2Pool m_rf2Pool;
public:
  OperatorTest2ArgRelative(const String &functionName,rBigRealFunction2Pool f) : AbstractFunctionTest(functionName)
                                                                               , m_rf2Pool(f)
  {
  }
  void runTest(int threadId, DigitPool *pool);
};

typedef void (*RawTestFunction)(TestStatistic &stat);

class RawFunctionTest : public AbstractFunctionTest {
private:
  RawTestFunction m_testFunction;
public:
  RawFunctionTest(const String &functionName, RawTestFunction f) : AbstractFunctionTest(functionName), m_testFunction(f)
  {
  }
  void runTest(int threadId, DigitPool *pool) {
    TestStatistic stat(threadId, getFunctionName(), pool, EMPTY, 0);
    m_testFunction(stat);
  };
};

// Raw functions. all taking TestStatistic & as parameter
void testPositiveFloat32Conversion( TestStatistic &stat);
void testNegativeFloat32Conversion( TestStatistic &stat);
void testPositiveDouble64Conversion(TestStatistic &stat);
void testNegativeDouble64Conversion(TestStatistic &stat);
void testPositiveDouble80Conversion(TestStatistic &stat);
void testNegativeDouble80Conversion(TestStatistic &stat);
void testConstructors(              TestStatistic &stat);

void testGetFirst32(                TestStatistic &stat);
void testGetFirst64(                TestStatistic &stat);
#ifdef IS64BIT
void testGetFirst128(               TestStatistic &stat);
#endif
void testGetDecimalDigitCount(      TestStatistic &stat);
void testPow10(                     TestStatistic &stat);
void testIsPow10(                   TestStatistic &stat);
void testGetExpo10(                 TestStatistic &stat);
void testMultPow10(                 TestStatistic &stat);
void testGetExpo2(                  TestStatistic &stat);
void testGetDecimalDigits(          TestStatistic &stat);

void testAPCSum(                    TestStatistic &stat);
void testAPCProd(                   TestStatistic &stat);
void testAPCQuot(                   TestStatistic &stat);
void testAPCPow(                    TestStatistic &stat);

void testRandBigReal(               TestStatistic &stat);
void testRandBigInt(                TestStatistic &stat);
void testRandBigRational(           TestStatistic &stat);
void testQuot3(                     TestStatistic &stat);
void testAssignOperators(           TestStatistic &stat);
void testIntSum(                    TestStatistic &stat);
void testIntDif(                    TestStatistic &stat);
void testIntProd(                   TestStatistic &stat);
void testIntQuot(                   TestStatistic &stat);
void testIntRem(                    TestStatistic &stat);
void testRatSum(                    TestStatistic &stat);
void testRatDif(                    TestStatistic &stat);
void testRatProd(                   TestStatistic &stat);
void testRatQuot(                   TestStatistic &stat);
void testRatMod(                    TestStatistic &stat);
void testPi(                        TestStatistic &stat);
void testReadWriteBigReal(          TestStatistic &stat);
void testReadWriteBigInt(           TestStatistic &stat);
void testReadWriteBigRational(      TestStatistic &stat);

#define MAXDIGITS       400
#define DIGITSTEP         3
#define MINSCALE        -20
#define MAXSCALE         20
#define SCALESTEP         3
#define MAXLENGTH        20

typedef SynchronizedQueue<AbstractFunctionTest*> TestQueue;

class TesterJob : public Runnable {
private:
  static Semaphore s_gate, s_allDone;
  static int       s_runningCount;
  static bool      s_allOk;
  static bool      s_stopOnError;
  static double    s_totalThreadTime;
  const int        m_id;
  static void incrRunning();
  static void decrRunning();

  static void addTimeUsage(double threadTime);
  static TestQueue s_testQueue, s_doneQueue;

  static BigRealJobArray s_testerJobs;
  static void startAll(UINT threadCount);
  static void releaseAll();
  static void waitUntilAllDone() {
    s_allDone.wait();
  }

public:
  TesterJob(int id) : m_id(id) {
  }
  UINT run();
  static int getRunningCount() {
    return s_runningCount;
  }
  static inline void addFunctionTest(AbstractFunctionTest *test) {
    TRACE_NEW(test);
    s_testQueue.put(test);
  }
  static bool allOk() {
    return s_allOk;
  }
  static void runAll(UINT threadCount, bool stopOnError);
  static void shuffleTestOrder();
  static double getTotalThreadTime() {
    return s_totalThreadTime;
  }
};

inline void testFunction(const String       &functionName
                        ,const TestInterval &xInterval
                        ,BigRealFunction1    f
                        ,DoubleFunction1     f64 = NULL
                        ,Double80Function1   f80 = NULL) {
  TesterJob::addFunctionTest(new FunctionTest1ArgND64D80(functionName, xInterval, f, f64, f80));
}

inline void testFunction(const String       &functionName
                        ,const TestInterval &interval
                        ,rBigRealFunction1   f) {
  TesterJob::addFunctionTest(new FunctionTest1ArgRelative(functionName, interval, f));
}

inline void testFunction(const String       &functionName
                        ,const TestInterval &xInterval
                        ,const TestInterval &yInterval
                        ,BigRealFunction2    f
                        ,DoubleFunction2     f64 = NULL
                        ,Double80Function2   f80 = NULL) {
  TesterJob::addFunctionTest(new FunctionTest2ArgND64D80(functionName, xInterval, yInterval, f, f64, f80));
}

inline void testExactBinaryOperator(const String                      &functionName
                                   ,BigRealBinaryOperator              op
                                   ,BigRealBinaryOperatorCheckFunction check) {
  TesterJob::addFunctionTest(new BigRealExactBinaryOperatorTest(functionName, op, check));
}

inline void testOperator(const String        &functionName
                        ,BigRealFunction2Pool op
                        ,DoubleFunction2      op64 = NULL
                        ,Double80Function2    op80 = NULL) {
  TesterJob::addFunctionTest(new OperatorTest2ArgND64D80Pool(functionName, op, op64, op80));
}

static void testOperator(const String &functionName, rBigRealFunction2Pool op) {
  TesterJob::addFunctionTest(new OperatorTest2ArgRelative(functionName, op));
}

inline void testFunction(const String &functionName
                        ,const TestInterval &xInterval
                        ,const TestInterval &yInterval
                        ,rBigRealFunction2 f) {
  TesterJob::addFunctionTest(new FunctionTest2ArgRelative(functionName, xInterval, yInterval, f ));
}

class DigitMonitorThread : public Thread {
public:
  DigitMonitorThread() {
    setDeamon(true);
    start();
  }
  UINT run();
};
