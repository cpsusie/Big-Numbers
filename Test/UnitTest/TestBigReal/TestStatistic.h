#pragma once

#include <TreeMap.h>
#include <TinyBitSet.h>
#include <Timer.h>
#include "AllTime.h"

using namespace std;

#define LOOPXPOS        93
#define THREADYPOS(id) (id)

typedef enum {
  EMPTY
 ,__F
 ,XD
 ,XF
 ,XY
 ,XYD
 ,XYF
} FunctionSignatur;

#define nameparam StreamParameters(0,7,ios::left)
#define nparam    StreamParameters(20,28,ios::scientific | ios::left | ios::showpos | ios::unitbuf)

void checkBigRealDouble64(const String &functionName, DoubleFunction1   f64, const BigReal &x,                   const BigReal &exactResult);
void checkBigRealDouble64(const String &functionName, DoubleFunction2   f64, const BigReal &x, const BigReal &y, const BigReal &exactResult);
void checkBigRealDouble80(const String &functionName, Double80Function1 f80, const BigReal &x,                   const BigReal &exactResult);
void checkBigRealDouble80(const String &functionName, Double80Function2 f80, const BigReal &x, const BigReal &y, const BigReal &exactResult);

class StopException : public Exception {
public:
  StopException() : Exception("Stopped") {
  }
};
class TestStatistic {
private:
  const String           m_name;
  const FunctionSignatur m_signatur;
  const int              m_threadId;
  unsigned int           m_maxTestCount;
  const int              m_ypos;
  unsigned int           m_totalUpdateCount;
  unsigned int           m_testCounter;
  String                 m_endMessage;
  bool                   m_endMessageAdded;
  DigitPool             *m_digitPool;
  JavaRandom             m_rnd;
  BigReal                m_maxTolerance;
  BigReal                m_error;
  BigReal                m_minQ, m_maxQ;
  int                    m_maxDigits;
//  IntTreeMap<int>        m_count;
  AllTime                m_startTime;
  static bool            s_stopNow;
  static FastSemaphore   s_lock;
  static int             s_logypos;
  static tostream       *s_errorLogStream;
  static bool            s_timerIsStarted;
  static Timer           s_updateScreenTimer;
  static BitSet32        s_timeToPrint;
  static const String    s_signaturString[];

  void init();
  void endTest(const AllTime &timeUsage);
  const String &getSignaturString() const {
    return s_signaturString[m_signatur];
  }
  void printNameAndSignatur();
  void markError(const BigReal &x,                               const BigReal &tolerance, const BigReal &exactResult, const BigReal &result);
  void markError(const BigReal &x, const BigReal &y,             const BigReal &tolerance, const BigReal &exactResult, const BigReal &result);
  void markError(const BigReal &x,                   int digits, const BigReal &tolerance, const BigReal &exactResult, const BigReal &result);
  void markError(const BigReal &x, const BigReal &y, int digits, const BigReal &tolerance, const BigReal &exactResult, const BigReal &result);
  String getErrorHeader() const;
  String getParamStr(               const String &name, const BigReal &x) const;
  String getDigitsStr(              int   digits) const;
  String getToleranceStr(           const BigReal &tolerance) const;
  String getErrorStr(               const BigReal &exactResult, const BigReal &result, const BigReal &tolerance) const;
  String getRelativeErrorStr(       const BigReal &exactResult, const BigReal &result, const BigReal &tolerance) const;
  String getFunctionNameAndErrorStr(const BigReal &exactResult, const BigReal &result) const;
public:
  TestStatistic(int threadId, const String &name, DigitPool *pool, FunctionSignatur signatur, size_t maxTestCount, const BigReal &maxTolerance);
  TestStatistic(int threadId, const String &name, DigitPool *pool, FunctionSignatur signatur, size_t maxTestCount, size_t maxDigits=0);
  ~TestStatistic();
  void update( const BigReal &error, const BigReal &tolerance);
  void updateZ(const BigReal &tolerance);
  void update( const BigReal &Q);
  void checkError( BigRealFunction1     f, const BigReal &x,                   const BigReal &tolerance, const BigReal &exactResult);
  void checkError( BigRealFunction1Pool f, const BigReal &x,                   const BigReal &tolerance, const BigReal &exactResult);
  void checkError( BigRealFunction2     f, const BigReal &x, const BigReal &y, const BigReal &tolerance, const BigReal &exactResult);
  void checkError( BigRealFunction2Pool f, const BigReal &x, const BigReal &y, const BigReal &tolerance, const BigReal &exactResult);
  void checkError(rBigRealFunction1     f, const BigReal &x,                   int            digits   , const BigReal &exactResult);
  void checkError(rBigRealFunction1Pool f, const BigReal &x,                   int            digits   , const BigReal &exactResult);
  void checkError(rBigRealFunction2     f, const BigReal &x, const BigReal &y, int            digits   , const BigReal &exactResult);
  void checkError(rBigRealFunction2Pool f, const BigReal &x, const BigReal &y, int            digits   , const BigReal &exactResult);
  static String toString(const BigReal &n, int digits = 6);
  static void startUpdateScreenTimer();
  static void stopUpdateScreenTimer();

  inline bool isTimeToPrint() {
    m_testCounter++;
    return s_timeToPrint.contains(m_threadId);
  }
  inline unsigned int getTestCount() const {
    return m_testCounter;
  }
  const BigReal &getMinQ() const {
    return m_minQ;
  }
  const BigReal &getMaxQ() const {
    return m_maxQ;
  }
  void flushOutput();
  static void setAllPrinting();
  String getCountStr() const {
    return format(_T("Test(%5u/%-5u)"), m_testCounter, m_maxTestCount);
  }
  double getPercentDone() const {
    return PERCENT(m_testCounter, m_maxTestCount);
  }
  static void screenlog(       _In_z_ _Printf_format_string_ TCHAR const * const format,...);
  static void screenlogGotoTop() {
    s_logypos = 0;
  }
  void        vsetEndMessage(  _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr);
  void        setEndMessage(   _In_z_ _Printf_format_string_ TCHAR const * const format,...);
  void        addEndMessage(   _In_z_ _Printf_format_string_ TCHAR const * const format,...);
  void        printLoopMessage(_In_z_ _Printf_format_string_ TCHAR const * const format,...);
  void        setEndMessageToOk() {
    setEndMessage(_T("Ok"));
  }
  static void stopNow() {
    s_stopNow = true;
  }
  inline DigitPool *getDigitPool() {
    return m_digitPool;
  }
  inline RandomGenerator &getRandomGenerator() {
    return m_rnd;
  }
  inline BigReal getRandomTolerance(int exponent) {
    return getRandom(5, exponent, false);
  }

  inline BigReal getRandom(int length, int exponent, bool allowZero = true) {
    return ::getRandom(length, exponent, getRandomGenerator(), getDigitPool(), allowZero);
  }

  inline void setTotalTestCount(unsigned long n) {
    m_maxTestCount = n;
  }
  static tostream &getErrorLog(bool logTime = true);
  tostream &out() {
    return tcout;
  }
  static void flushAndCloseErorLog();
};

#define ERRLOG TestStatistic::getErrorLog()
#define TRACELOG TestStatistic::getErrorLog(false)
#define THROWTESTERROR() throwException(_T("Error in %s, line %d"), __TFUNCTION__,__LINE__)
