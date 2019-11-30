#include "stdafx.h"
#include <Date.h>
#include <CPUInfo.h>
#include <Console.h>
#include "TestStatistic.h"

class SetTimeToPrint : public TimeoutHandler {
public:
  void handleTimeout(Timer &timer) {
    TestStatistic::setAllPrinting();
  }
};

void TestStatistic::setAllPrinting() {
  s_gate.wait();
  s_timeToPrint.addAll();
  s_gate.notify();
}

void TestStatistic::flushOutput() {
  s_gate.wait();
  s_timeToPrint.remove(m_threadId);
  s_gate.notify();
}

FastSemaphore TestStatistic::s_gate;
bool          TestStatistic::s_stopNow        = false;
static const int logyStartPos = getProcessorCount() + 1;
int           TestStatistic::s_logypos = logyStartPos;
tostream     *TestStatistic::s_errorLogStream = NULL;
bool          TestStatistic::s_timerIsStarted = false;
Timer         TestStatistic::s_timer(1);
BitSet32      TestStatistic::s_timeToPrint;
static        SetTimeToPrint timerEventHandler;

const String TestStatistic::s_signaturString[] = {
  EMPTYSTRING
 ,_T("(const BigReal &f)")
 ,_T("(const BigReal &x, unsigned int digits)")
 ,_T("(const BigReal &x, const BigReal &f)")
 ,_T("(const BigReal &x, const BigReal &y)")
 ,_T("(const BigReal &x, const BigReal &y, unsigned int digits)")
 ,_T("(const BigReal &x, const BigReal &y, const BigReal &f)")
};

TestStatistic::TestStatistic(int threadId, const String &name, DigitPool *pool, FunctionSignatur signatur, size_t maxTestCount, const BigReal &maxTolerance)
: m_threadId(threadId)
, m_name(name)
, m_digitPool(pool)
, m_signatur(signatur)
, m_maxTestCount((int)maxTestCount)
, m_maxTolerance(maxTolerance,pool)
, m_error(pool)
, m_minQ(pool)
, m_maxQ(pool)
, m_ypos(THREADYPOS(threadId))
{
  init();
}

TestStatistic::TestStatistic(int threadId, const String &name, DigitPool *pool, FunctionSignatur signatur, size_t maxTestCount, size_t maxDigits)
: m_threadId(threadId)
, m_name(name)
, m_digitPool(pool)
, m_signatur(signatur)
, m_maxTestCount((int)maxTestCount)
, m_maxTolerance(pool)
, m_error(pool)
, m_minQ(pool)
, m_maxQ(pool)
, m_ypos(THREADYPOS(threadId))
{
  m_maxDigits    = (int)maxDigits;
  init();
}

void TestStatistic::init() {
  m_totalUpdateCount  = 0;
  m_testCounter       = 0;
#ifndef _DEBUG
  m_rnd.randomize();
#endif

  s_gate.wait();
  if(!s_timerIsStarted) {
    s_timer.startTimer(2000, timerEventHandler, true);
    s_timerIsStarted = true;
  }
  s_gate.notify();

  printNameAndSignatur();
  m_startTime = AllTime();
}

#define NAMELENGTH 20

TestStatistic::~TestStatistic() {
  const AllTime timeUsage = AllTime() - m_startTime;

  endTest(timeUsage);

  if(m_endMessage.length() == 0) {
    m_endMessage = format(_T("UpdateCount:%u"), m_totalUpdateCount);
  }
  log(_T("Testing %-*s:Timeusage:%s %s"), NAMELENGTH, m_name.cstr(), timeUsage.toString(SEC).cstr(), m_endMessage.cstr());
/*
  FILE *dump = MKFOPEN(("c:\\temp\\BigRealErrors\\" + m_name + "Errors.txt").cstr(),"w");
  Set<Entry<int,int> > s = m_count.entrySet();
  for(Iterator<Entry<int,int> > it = s.getIterator(); it.hasNext();) {
    Entry<int,int> &e = it.next();
    fprintf(dump,"%d %.8le\n", e.getKey(), (double)e.getValue()/m_totalUpdateCount);
  }
  fclose(dump);
*/
}

tostream &TestStatistic::getErrorLog(bool logTime) { // static
  s_gate.wait();
  if(s_errorLogStream == NULL) {
    FileNameSplitter finfo(getModuleFileName());
    finfo.setDir(getSourceDir()).setFileName(finfo.getFileName() + _T("Errors")).setExtension(_T("log"));
    const String fileName = finfo.getFullPath();
    FILE *f = MKFOPEN(fileName, _T("a")); fclose(f);
    s_errorLogStream = new tofstream(fileName.cstr(), ios::out | ios::app | ios::unitbuf); TRACE_NEW(s_errorLogStream);
  }
  if(logTime) {
    *s_errorLogStream << Timestamp().toString() << endl;
  }
  s_gate.notify();
  return *s_errorLogStream;
}

void TestStatistic::flushAndCloseErorLog() { // static
  s_gate.wait();
  if(s_errorLogStream != NULL) {
    s_errorLogStream->flush();
    SAFEDELETE(s_errorLogStream);
  }
  s_gate.notify();
}

class ErrorLogCloser {
public:
  ~ErrorLogCloser() {
    TestStatistic::flushAndCloseErorLog();
  }
};

static ErrorLogCloser erorLogCloser;

void TestStatistic::printNameAndSignatur() {
  Console::printf(20,m_ypos, _T("Testing %-*s%-55s"), NAMELENGTH, m_name.cstr(), getSignaturString().cstr());
}

void TestStatistic::printLoopMessage(_In_z_ _Printf_format_string_ TCHAR const * const format,...) {
  if(s_stopNow) throw StopException();
  va_list argptr;
  va_start(argptr,format);
  const String str = vformat(format, argptr);
  va_end(argptr);
  Console::printf(102,m_ypos, _T("%-17s, %s"), getCountStr().cstr(), str.cstr());
  flushOutput();
}

String TestStatistic::toString(const BigReal &n, int digits) { // static
  return ::toString(n, digits,digits+9,ios::scientific | ios::showpos | ios::unitbuf);
}

void TestStatistic::vsetEndMessage(_In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {
  m_endMessage = vformat(format, argptr);
}

void TestStatistic::setEndMessage(_In_z_ _Printf_format_string_ TCHAR const * const format,...) {
  va_list argptr;
  va_start(argptr,format);
  vsetEndMessage(format, argptr);
  va_end(argptr);
  m_endMessageAdded = false;
}

void TestStatistic::addEndMessage(_In_z_ _Printf_format_string_ TCHAR const * const format,...) {
  va_list argptr;
  va_start(argptr,format);
  vsetEndMessage(format, argptr);
  va_end(argptr);
  m_endMessageAdded = true;
}

void TestStatistic::endTest(const AllTime &timeUsage) {
  String msgToShow;;
  if((m_endMessage.length() == 0) || m_endMessageAdded) {
    msgToShow = format(_T("%-*s: Time:%s minQ       :%s, maxQ:%s ")
                       ,NAMELENGTH, m_name.cstr()
                       ,timeUsage.toString(SEC).cstr()
                       ,toString(m_minQ,3).cstr()
                       ,toString(m_maxQ,3).cstr()
                       );
  }
  if(m_endMessage.length() > 0) {
    if(msgToShow.length() == 0) {
      msgToShow = format(_T("%-*s: Time:%s ")
                        ,NAMELENGTH, m_name.cstr()
                        ,timeUsage.toString(SEC).cstr()
                        );
    }
    msgToShow += m_endMessage;
  }
  screenlog(_T("%s"), msgToShow.cstr());
}


void TestStatistic::checkError(BigRealFunction1 f, const BigReal &x, const BigReal &tolerance, const BigReal &exactResult) {
  const BigReal result = f(x, tolerance);
  m_error = fabs(exactResult-result);
  if(m_error <= tolerance) {
    updateZ(tolerance);
  } else {
    markError(x, tolerance, exactResult, result);
  }
}

void TestStatistic::checkError(BigRealFunction1Pool f, const BigReal &x, const BigReal &tolerance, const BigReal &exactResult) {
  const BigReal result = f(x, tolerance,m_digitPool);
  m_error = fabs(exactResult-result);
  if(m_error <= tolerance) {
    updateZ(tolerance);
  } else {
    markError(x, tolerance, exactResult, result);
  }
}

void TestStatistic::checkError(BigRealFunction2 f, const BigReal &x, const BigReal &y, const BigReal &tolerance, const BigReal &exactResult) {
  const BigReal result = f(x, y, tolerance);
  m_error  = fabs(exactResult-result);
  if(m_error <= tolerance) {
    updateZ(tolerance);
  } else {
    markError(x,y,tolerance, exactResult, result);
  }
}

void TestStatistic::checkError( BigRealFunction2Pool f, const BigReal &x, const BigReal &y, const BigReal &tolerance, const BigReal &exactResult) {
  const BigReal result = f(x, y, tolerance, m_digitPool);
  m_error  = fabs(exactResult-result);
  if(m_error <= tolerance) {
    updateZ(tolerance);
  } else {
    markError(x,y,tolerance, exactResult, result);
  }
}

void TestStatistic::checkError(rBigRealFunction1 f, const BigReal &x, int digits, const BigReal &exactResult) {
  const BigReal result    = f(x, digits);
  const BigReal tolerance = e(m_digitPool->_1(), -digits);

  m_error = getRelativeError(result, exactResult);

  if(m_error <= tolerance) {
    updateZ(tolerance);
  } else {
    markError(x,digits,tolerance,exactResult, result);
  }
}

void TestStatistic::checkError(rBigRealFunction1Pool f, const BigReal &x, int digits, const BigReal &exactResult) {
  const BigReal result    = f(x, digits, m_digitPool);
  const BigReal tolerance = e(m_digitPool->_1(), -digits);

  m_error = getRelativeError(result, exactResult);

  if(m_error <= tolerance) {
    updateZ(tolerance);
  } else {
    markError(x,digits,tolerance,exactResult, result);
  }
}

void TestStatistic::checkError(rBigRealFunction2 f, const BigReal &x, const BigReal &y, int digits, const BigReal &exactResult) {
  const BigReal result    = f(x, y, digits);
  const BigReal tolerance = e(m_digitPool->_1(), -digits);

  m_error = getRelativeError(result, exactResult);

  if(m_error <= tolerance) {
    updateZ(tolerance);
  } else {
    markError(x,y,digits,tolerance, exactResult, result);
  }
}

void TestStatistic::checkError(rBigRealFunction2Pool f, const BigReal &x, const BigReal &y, int digits, const BigReal &exactResult) {
  const BigReal result    = f(x, y, digits, m_digitPool);
  const BigReal tolerance = e(m_digitPool->_1(), -digits);

  m_error = getRelativeError(result, exactResult);

  if(m_error <= tolerance) {
    updateZ(tolerance);
  } else {
    markError(x,y,digits,tolerance, exactResult, result);
  }
}

static const COORD &getConsoleSize() {
  static COORD sz = { 0, 0 };
  if(sz.X == 0) {
    sz = Console::getWindowSize();
  }
  return sz;
}

void TestStatistic::screenlog(_In_z_ _Printf_format_string_ TCHAR const * const format,...) {
  va_list argptr;
  va_start(argptr,format);
  const String str = vformat(format, argptr);
  va_end(argptr);
  StringArray lines(Tokenizer(str,_T("\n")));
  s_gate.wait();
  const COORD &wsz = getConsoleSize();
  for(size_t i = 0; i < lines.size(); i++) {
    const String &line = lines[i];
    const int filler = wsz.X - (int)line.length();
    if(filler > 0) {
      Console::printf(0, s_logypos++, _T("%s%*.*s"), lines[i].cstr(), filler,filler,_T(" "));
    } else {
      Console::printf(0,s_logypos++, _T("%s"), lines[i].cstr());
    }
  }
  if(s_logypos >= wsz.Y) {
    s_logypos = logyStartPos;
  }
  s_gate.notify();
}

void TestStatistic::markError(const BigReal &x, const BigReal &tolerance, const BigReal &exactResult, const BigReal &result) {
  screenlog(_T("%s(%s)\n%s")
           ,getErrorHeader().cstr()
           ,getParamStr(_T("x"), x).cstr()
           ,getErrorStr(exactResult, result, tolerance).cstr()
           );
  ERRLOG << getErrorHeader()                            << endl
         << getParamStr(_T("x"), x)                     << endl
         << getErrorStr(exactResult, result, tolerance) << endl;
}

void TestStatistic::markError(const BigReal &x, const BigReal &y, const BigReal &tolerance, const BigReal &exactResult, const BigReal &result) {
  screenlog(_T("%s(%s,%s)\n%s")
           ,getErrorHeader().cstr()
           ,getParamStr(_T("x"), x).cstr()
           ,getParamStr(_T("y"), y).cstr()
           ,getErrorStr(exactResult, result, tolerance).cstr()
           );
  ERRLOG << getErrorHeader()                            << endl
         << getParamStr(_T("x"), x)                     << endl
         << getParamStr(_T("y"), y)                     << endl
         << getErrorStr(exactResult, result, tolerance) << endl;
}

void TestStatistic::markError(const BigReal &x, int digits, const BigReal &tolerance, const BigReal &exactResult, const BigReal &result) {
  screenlog(_T("%s(%s,%s)\n%s")
           ,getErrorHeader().cstr()
           ,getParamStr(_T("x"), x).cstr()
           ,getDigitsStr(digits).cstr()
           ,getRelativeErrorStr(exactResult, result, tolerance).cstr()
           );
  ERRLOG << getErrorHeader()                            << endl
         << getParamStr(_T("x"), x)                     << endl
         << getDigitsStr(digits)                        << endl
         << getErrorStr(exactResult, result, tolerance) << endl;
}

void TestStatistic::markError(const BigReal &x, const BigReal &y, int digits, const BigReal &tolerance, const BigReal &exactResult, const BigReal &result) {
  screenlog(_T("%s(%s,%s,%s)\n%s")
           ,getErrorHeader().cstr()
           ,getParamStr(_T("x"), x).cstr()
           ,getParamStr(_T("y"), y).cstr()
           ,getDigitsStr(digits).cstr()
           ,getRelativeErrorStr(exactResult, result, tolerance).cstr()
           );
  ERRLOG << getErrorHeader()                            << endl
         << getParamStr(_T("x"), x)                     << endl
         << getParamStr(_T("y"), y)                     << endl
         << getDigitsStr(digits)                        << endl
         << getErrorStr(exactResult, result, tolerance) << endl;
}

String TestStatistic::getErrorHeader() const {
  return format(_T("Error in %s"), m_name.cstr());
}

String TestStatistic::getParamStr(const String &name, const BigReal &x) const {
  return format(_T("%s:%s"), name.cstr(),toString(x).cstr());
}

String TestStatistic::getDigitsStr(int digits) const {
  return format(_T("digits:%d"), digits);
}

String TestStatistic::getToleranceStr(const BigReal &tolerance) const {
  return format(_T("Tolerance:%s"), toString(tolerance).cstr());
}

String TestStatistic::getErrorStr(const BigReal &exactResult, const BigReal &result, const BigReal &tolerance) const {
  return format(_T("%s\n%s\n%s")
               ,getToleranceStr(tolerance).cstr()
               ,format(_T("Difference:%s"),toString(m_error).cstr()).cstr()
               ,getFunctionNameAndErrorStr(exactResult, result).cstr()
               );
}

String TestStatistic::getRelativeErrorStr(const BigReal &exactResult, const BigReal &result, const BigReal &tolerance) const {
  return format(_T("%s\n%s\n%s")
               ,getToleranceStr(tolerance).cstr()
               ,format(_T("Relative error:%s"), toString(m_error).cstr()).cstr()
               ,getFunctionNameAndErrorStr(exactResult, result).cstr()
               );
}

String TestStatistic::getFunctionNameAndErrorStr(const BigReal &exactResult, const BigReal &result) const {
  switch(m_signatur) {
  case XD:
    return format(_T("%s(x,%d)=%s\n%s(x,n)=%s")
                 ,m_name.cstr(), m_maxDigits, toString(exactResult).cstr()
                 ,m_name.cstr(), toString(result).cstr());
  case XF:
    return format(_T("%s(x,%s)=%s\n%s(x,f)=%s")
                 ,m_name.cstr(), toString(m_maxTolerance).cstr(), toString(exactResult).cstr()
                 ,m_name.cstr(), toString(result).cstr());
    break;
  case XYD:
    return format(_T("%s(x,y,%d)=%s\n%s(x,y,n)=%s")
                 ,m_name.cstr(), m_maxDigits, toString(exactResult).cstr()
                 ,m_name.cstr(), toString(result).cstr());
    break;
  case XYF:
    return format(_T("%s(x,y,%s)=%s\n%s(x,y,f)=%s")
                 ,m_name.cstr(), toString(m_maxTolerance).cstr(), toString(exactResult).cstr()
                 ,m_name.cstr(), toString(result).cstr());
    break;
  default:
    return format(_T("%s(unknown signatur). Exact:%s\n%s(unknown signatur). Result:%s")
                 ,m_name.cstr(), toString(exactResult).cstr()
                 ,m_name.cstr(), toString(result).cstr()
                 );
  }
}

void TestStatistic::update(const BigReal &error, const BigReal &tolerance) {
  if(error.isZero()) {
    update(error);
  } else {
    update(rQuot(error, tolerance, 8, m_digitPool));
  }
}

void TestStatistic::updateZ(const BigReal &tolerance) {
  if(m_error.isZero()) {
    update(m_error);
  } else {
    update(rQuot(m_error, tolerance, 8, m_digitPool));
  }
}

void TestStatistic::update(const BigReal &Q) {
  BRExpoType key;
  if(Q.isZero()) {
    key = 0;
  } else {
    if(m_minQ.isZero()) {
      m_minQ = m_maxQ = Q;
    } else if(Q < m_minQ) {
      m_minQ = Q;
    } else if(Q > m_maxQ) {
      m_maxQ = Q;
    }
    key = -BigReal::getExpo10(Q);
  }

/*
  int *count = m_count.get(key);
  if(count != NULL) {
    (*count)++;
  } else {
    m_count.put(key, 1);
  }
*/
  m_totalUpdateCount++;
}

void checkBigRealDouble64(const String &functionName, DoubleFunction1 f64, const BigReal &x, const BigReal &exactResult) {
  const double result               = f64(getDouble(x));
  const double error                = getRelativeError(result,exactResult);
  const double maxRelativetiveError = 1e-12;

  if(error > maxRelativetiveError) {
    ERRLOG << _T("Error in ") << functionName << _T(" (double)")                                             << endl
           << _T("x        :")                                       << FullFormatBigReal(x)                 << endl
           << functionName << _T("(x,almost exact)")                 << FullFormatBigReal(exactResult)       << endl
           << functionName << _T("(getDouble(x)):")                  << dparam( 15) << result                << endl
           << _T("Relative error:")                                  << udparam(15) << error                 << endl
           << _T("Max acceptable relative error:")                   << udparam(15) << maxRelativetiveError  << endl;
    throwException(_T("Error in %s"), functionName.cstr());
  }
}

void checkBigRealDouble64(const String &functionName, DoubleFunction2 f64, const BigReal &x, const BigReal &y, const BigReal &exactResult) {
  const double result               = f64(getDouble(x),getDouble(y));
  const double error                = getRelativeError(result,exactResult);
  const double maxRelativetiveError = 1e-12;

  if(error > maxRelativetiveError) {
    ERRLOG << _T("Error in ") << functionName << _T(" (double,double)")                                      << endl
           << _T("x        :")                                       << FullFormatBigReal(x)                 << endl
           << _T("y        :")                                       << FullFormatBigReal(y)                 << endl
           << functionName << _T("(x,almost exact)")                 << FullFormatBigReal(exactResult)       << endl
           << functionName << _T("(getDouble(x),getDouble(y)):")     << dparam( 15) << result                << endl
           << _T("Relative error:")                                  << udparam(15) << error                 << endl
           << _T("Max acceptable relative error:")                   << udparam(15) << maxRelativetiveError  << endl;
    throwException(_T("Error in %s"), functionName.cstr());
  }
}

void checkBigRealDouble80(const String &functionName, Double80Function1 f80, const BigReal &x, const BigReal &exactResult) {
  const Double80 result               = f80(getDouble80(x));
  const Double80 error                = getRelativeError(result,exactResult);
  const double   maxRelativetiveError = 1e-15;
  if(error > maxRelativetiveError) {
    ERRLOG << _T("Error in ") << functionName << _T(" (Double80)")                                           << endl
           << _T("x        :")                                       << FullFormatBigReal(x)                 << endl
           << functionName << _T("(x,almost exact)")                 << FullFormatBigReal(exactResult)       << endl
           << functionName << _T("(getDouble(x)):")                  << dparam( 15) << result                << endl
           << _T("Relative error:")                                  << udparam(15) << error                 << endl
           << _T("Max acceptable relative error:")                   << udparam(15) << maxRelativetiveError  << endl;
    throwException(_T("Error in %s"), functionName.cstr());
  }
}

void checkBigRealDouble80(const String &functionName, Double80Function2 f80, const BigReal &x, const BigReal &y, const BigReal &exactResult) {
  const Double80 result               = f80(getDouble80(x),getDouble80(y));
  const Double80 error                = getRelativeError(result,exactResult);
  const double   maxRelativetiveError = 1e-15;
  if(error > maxRelativetiveError) {
    ERRLOG << _T("Error in ") << functionName << _T(" (Double80,Double80)")                                  << endl
           << _T("x        :")                                       << FullFormatBigReal(x)                 << endl
           << _T("y        :")                                       << FullFormatBigReal(y)                 << endl
           << functionName << _T("(x,y,almost exact)")               << FullFormatBigReal(exactResult)       << endl
           << functionName << _T("(getDouble80(x),getDouble80(y)):") << dparam( 15) << result                << endl
           << _T("Relative error:")                                  << udparam(15) << error                 << endl
           << _T("Max acceptable relative error:")                   << udparam(15) << maxRelativetiveError  << endl;
    throwException(_T("Error in %s"), functionName.cstr());
  }
}
