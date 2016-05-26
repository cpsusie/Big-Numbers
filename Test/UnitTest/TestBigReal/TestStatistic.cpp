#include "stdafx.h"
#include <Date.h>
#include <CPUInfo.h>
#include <Console.h>
#include "TestStatistic.h"

using namespace std;

class SetTimeToPrint : public TimeoutHandler {
public:
  void handleTimeout(Timer &timer) {
    TestStatistic::setAllPrinting();
  }
};

void TestStatistic::setAllPrinting() {
  s_gate.wait();
  s_timeToPrint.addAll();
  s_gate.signal();
}

void TestStatistic::flushOutput() {
  s_gate.wait();
  s_timeToPrint.remove(m_threadId);
  s_gate.signal();
}

Semaphore TestStatistic::s_gate;
static const int logyStartPos = getProcessorCount() + 1;
int       TestStatistic::s_logypos = logyStartPos;
tostream *TestStatistic::s_errorLogStream = NULL;
bool      TestStatistic::s_timerIsStarted = false;
Timer     TestStatistic::s_timer(1);
BitSet32  TestStatistic::s_timeToPrint;
static    SetTimeToPrint timerEventHandler;

const String TestStatistic::s_signaturString[] = {
  _T("")
 ,_T("(const BigReal &f)")
 ,_T("(const BigReal &x, unsigned int digits)")
 ,_T("(const BigReal &x, const BigReal &f)")
 ,_T("(const BigReal &x, const BigReal &y)")
 ,_T("(const BigReal &x, const BigReal &y, unsigned int digits)")
 ,_T("(const BigReal &x, const BigReal &y, const BigReal &f)")
};

TestStatistic::TestStatistic(int threadId, const String &name, DigitPool *pool, FunctionSignatur signatur, unsigned int maxTestCount, const BigReal &maxTolerance) 
: m_threadId(threadId)
, m_name(name)
, m_digitPool(pool)
, m_signatur(signatur)
, m_maxTestCount(maxTestCount)
, m_maxTolerance(maxTolerance,pool)
, m_error(pool)
, m_minQ(pool)
, m_maxQ(pool)
, m_ypos(THREADYPOS(threadId))
{
  init();
}

TestStatistic::TestStatistic(int threadId, const String &name, DigitPool *pool, FunctionSignatur signatur, unsigned int maxTestCount, int maxDigits)
: m_threadId(threadId)
, m_name(name)
, m_digitPool(pool)
, m_signatur(signatur)
, m_maxTestCount(maxTestCount)
, m_maxTolerance(pool)
, m_error(pool)
, m_minQ(pool)
, m_maxQ(pool)
, m_ypos(THREADYPOS(threadId))
{
  m_maxDigits    = maxDigits;
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
  s_gate.signal();

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

static const TCHAR *thisFile = _T(__FILE__);

tostream &TestStatistic::getErrorLog(bool logTime) { // static
  s_gate.wait();
  if(s_errorLogStream == NULL) {
    FileNameSplitter finfo(getModuleFileName());
    finfo.setDir(FileNameSplitter(thisFile).getDir()).setFileName(finfo.getFileName() + _T("Errors")).setExtension(_T("log"));
    const String fileName = finfo.getFullPath();
    FILE *f = MKFOPEN(fileName, "a"); fclose(f);
    s_errorLogStream = new tofstream(fileName.cstr(), ios::out | ios::app | ios::unitbuf);
  }
  if(logTime) {
    *s_errorLogStream << Timestamp().toString() << NEWLINE;
  }
  s_gate.signal();
  return *s_errorLogStream;
}

void TestStatistic::flushAndCloseErorLog() { // static
  s_gate.wait();
  if(s_errorLogStream != NULL) {
    s_errorLogStream->flush();
    delete s_errorLogStream;
    s_errorLogStream = NULL;
  }
  s_gate.signal();
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

void TestStatistic::printLoopMessage(const _TCHAR *format,...) {
  va_list argptr;
  va_start(argptr,format);
  const String str = vformat(format, argptr);
  va_end(argptr);
  Console::printf(102,m_ypos, _T("%-17s, %s"), getCountStr().cstr(), str.cstr());
  flushOutput();
}

String TestStatistic::toString(const BigReal &n, int digits) { // static 
  BigRealStream s(StreamParameters(digits,digits+9,ios::scientific | ios::showpos | ios::unitbuf));
  s << n;
  return s;
}

void TestStatistic::vsetEndMessage(const TCHAR *format, va_list argptr) {
  m_endMessage = vformat(format, argptr);
}

void TestStatistic::setEndMessage(const TCHAR *format,...) {
  va_list argptr;
  va_start(argptr,format);
  vsetEndMessage(format, argptr);
  va_end(argptr);
  m_endMessageAdded = false;
}

void TestStatistic::addEndMessage(const TCHAR *format,...) {
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
  const BigReal tolerance = e(m_digitPool->get1(), -digits);

  m_error = getRelativeError(result, exactResult);

  if(m_error <= tolerance) {
    updateZ(tolerance);
  } else {
    markError(x,digits,tolerance,exactResult, result);
  }
}

void TestStatistic::checkError(rBigRealFunction2 f, const BigReal &x, const BigReal &y, int digits, const BigReal &exactResult) {
  const BigReal result    = f(x, y, digits);
  const BigReal tolerance = e(m_digitPool->get1(), -digits);

  m_error = getRelativeError(result, exactResult);

  if(m_error <= tolerance) {
    updateZ(tolerance);
  } else {
    markError(x,y,digits,tolerance, exactResult, result);
  }
}

void TestStatistic::checkError(rBigRealFunction2Pool f, const BigReal &x, const BigReal &y, int digits, const BigReal &exactResult) {
  const BigReal result    = f(x, y, digits, m_digitPool);
  const BigReal tolerance = e(m_digitPool->get1(), -digits);

  m_error = getRelativeError(result, exactResult);

  if(m_error <= tolerance) {
    updateZ(tolerance);
  } else {
    markError(x,y,digits,tolerance, exactResult, result);
  }
}

static int getScreenHeight() {
  static int h = 0;
  if(h == 0) {
    h = Console::getWindowSize().Y;
  }
  return h;
}

void TestStatistic::screenlog(const TCHAR *format,...) {
  va_list argptr;
  va_start(argptr,format);
  const String str = vformat(format, argptr);
  va_end(argptr);
  StringArray lines(Tokenizer(str,_T("\n")));
  s_gate.wait();
  for(int i = 0; i < lines.size(); i++) {
    Console::printf(0,s_logypos++, _T("%s"), lines[i].cstr());
  }
  if(s_logypos >= getScreenHeight()) {
    s_logypos = logyStartPos;
  }
  s_gate.signal();
}

void TestStatistic::markError(const BigReal &x, const BigReal &tolerance, const BigReal &exactResult, const BigReal &result) {
  screenlog(_T("%s(%s)\n%s")
           ,getErrorHeader().cstr()
           ,getParamStr(_T("x"), x).cstr()
           ,getErrorStr(exactResult, result, tolerance).cstr()
           );
  ERRLOG << getErrorHeader()                            << NEWLINE
         << getParamStr(_T("x"), x)                     << NEWLINE
         << getErrorStr(exactResult, result, tolerance) << NEWLINE;
}

void TestStatistic::markError(const BigReal &x, const BigReal &y, const BigReal &tolerance, const BigReal &exactResult, const BigReal &result) {
  screenlog(_T("%s(%s,%s)\n%s")
           ,getErrorHeader().cstr()
           ,getParamStr(_T("x"), x).cstr()
           ,getParamStr(_T("y"), y).cstr()
           ,getErrorStr(exactResult, result, tolerance).cstr()
           );
  ERRLOG << getErrorHeader()                            << NEWLINE
         << getParamStr(_T("x"), x)                     << NEWLINE
         << getParamStr(_T("y"), y)                     << NEWLINE
         << getErrorStr(exactResult, result, tolerance) << NEWLINE;
}

void TestStatistic::markError(const BigReal &x, int digits, const BigReal &tolerance, const BigReal &exactResult, const BigReal &result) {
  screenlog(_T("%s(%s,%s)\n%s")
           ,getErrorHeader().cstr()
           ,getParamStr(_T("x"), x).cstr()
           ,getDigitsStr(digits).cstr()
           ,getRelativeErrorStr(exactResult, result, tolerance).cstr()
           );
  ERRLOG << getErrorHeader()                            << NEWLINE
         << getParamStr(_T("x"), x)                     << NEWLINE
         << getDigitsStr(digits)                        << NEWLINE
         << getErrorStr(exactResult, result, tolerance) << NEWLINE;
}

void TestStatistic::markError(const BigReal &x, const BigReal &y, int digits, const BigReal &tolerance, const BigReal &exactResult, const BigReal &result) {
  screenlog(_T("%s(%s,%s,%s)\n%s")
           ,getErrorHeader().cstr()
           ,getParamStr(_T("x"), x).cstr()
           ,getParamStr(_T("y"), y).cstr()
           ,getDigitsStr(digits).cstr()
           ,getRelativeErrorStr(exactResult, result, tolerance).cstr()
           );
  ERRLOG << getErrorHeader()                            << NEWLINE
         << getParamStr(_T("x"), x)                     << NEWLINE
         << getParamStr(_T("y"), y)                     << NEWLINE
         << getDigitsStr(digits)                        << NEWLINE
         << getErrorStr(exactResult, result, tolerance) << NEWLINE;
}

String TestStatistic::getErrorHeader() const {
  return format(_T("Error in %s"), m_name.cstr());
}

String TestStatistic::getParamStr(const String &name, const BigReal &x) const {
  return format(_T("%s:%s"), name.cstr(),x.toString().cstr());
}

String TestStatistic::getDigitsStr(int digits) const {
  return format(_T("digits:%d"), digits);
}

String TestStatistic::getToleranceStr(const BigReal &tolerance) const {
  return format(_T("Tolerance:%s"), tolerance.toString().cstr());
}

String TestStatistic::getErrorStr(const BigReal &exactResult, const BigReal &result, const BigReal &tolerance) const {
  return format(_T("%s\n%s\n%s")
               ,getToleranceStr(tolerance).cstr()
               ,format(_T("Difference:%s"),m_error.toString().cstr()).cstr()
               ,getFunctionNameAndErrorStr(exactResult, result).cstr()
               );
}

String TestStatistic::getRelativeErrorStr(const BigReal &exactResult, const BigReal &result, const BigReal &tolerance) const {
  return format(_T("%s\n%s\n%s")
               ,getToleranceStr(tolerance).cstr()
               ,format(_T("Relative error:%s"), m_error.toString().cstr()).cstr()
               ,getFunctionNameAndErrorStr(exactResult, result).cstr()
               );
}

String TestStatistic::getFunctionNameAndErrorStr(const BigReal &exactResult, const BigReal &result) const {
  switch(m_signatur) {
  case XD:
    return format(_T("%s(x,%d)=%s\n%s(x,n)=%s")
                 ,m_name.cstr(), m_maxDigits, exactResult.toString().cstr()
                 ,m_name.cstr(), result.toString().cstr());
  case XF:
    return format(_T("%s(x,%s)=%s\n%s(x,f)=%s")
                 ,m_name.cstr(), m_maxTolerance.toString().cstr(), exactResult.toString().cstr()
                 ,m_name.cstr(), result.toString().cstr());
    break;
  case XYD:
    return format(_T("%s(x,y,%d)=%s\n%s(x,y,n)=%s")
                 ,m_name.cstr(), m_maxDigits, exactResult.toString().cstr()
                 ,m_name.cstr(), result.toString().cstr());
    break;
  case XYF:
    return format(_T("%s(x,y,%s)=%s\n%s(x,y,f)=%s")
                 ,m_name.cstr(), m_maxTolerance.toString().cstr(), exactResult.toString().cstr()
                 ,m_name.cstr(), result.toString().cstr());
    break;
  default:
    return format(_T("%s(unknown signatur). Exact:%s\n%s(unknown signatur). Result:%s")
                 ,m_name.cstr(), exactResult.toString().cstr()
                 ,m_name.cstr(), result.toString().cstr()
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
  int key;
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
    ERRLOG << _T("Error in ") << functionName << _T(" (double)")                                             << NEWLINE
           << _T("x        :")                                       << FullFormatBigReal(x)                 << NEWLINE
           << functionName << _T("(x,almost exact)")                 << FullFormatBigReal(exactResult)       << NEWLINE
           << functionName << _T("(getDouble(x)):")                  << dparam( 15) << result                << NEWLINE
           << _T("Relative error:")                                  << udparam(15) << error                 << NEWLINE
           << _T("Max acceptable relative error:")                   << udparam(15) << maxRelativetiveError  << NEWLINE;
    throwException(_T("Error in %s"), functionName.cstr());
  }
}

void checkBigRealDouble64(const String &functionName, DoubleFunction2 f64, const BigReal &x, const BigReal &y, const BigReal &exactResult) {
  const double result               = f64(getDouble(x),getDouble(y));
  const double error                = getRelativeError(result,exactResult);
  const double maxRelativetiveError = 1e-12;

  if(error > maxRelativetiveError) {
    ERRLOG << _T("Error in ") << functionName << _T(" (double,double)")                                      << NEWLINE
           << _T("x        :")                                       << FullFormatBigReal(x)                 << NEWLINE
           << _T("y        :")                                       << FullFormatBigReal(y)                 << NEWLINE
           << functionName << _T("(x,almost exact)")                 << FullFormatBigReal(exactResult)       << NEWLINE
           << functionName << _T("(getDouble(x),getDouble(y)):")     << dparam( 15) << result                << NEWLINE
           << _T("Relative error:")                                  << udparam(15) << error                 << NEWLINE
           << _T("Max acceptable relative error:")                   << udparam(15) << maxRelativetiveError  << NEWLINE;
    throwException(_T("Error in %s"), functionName.cstr());
  }
}

void checkBigRealDouble80(const String &functionName, Double80Function1 f80, const BigReal &x, const BigReal &exactResult) {
  const Double80 result               = f80(getDouble80(x));
  const Double80 error                = getRelativeError(result,exactResult);
  const double   maxRelativetiveError = 1e-15;
  if(error > maxRelativetiveError) {
    ERRLOG << _T("Error in ") << functionName << _T(" (Double80)")                                           << NEWLINE
           << _T("x        :")                                       << FullFormatBigReal(x)                 << NEWLINE
           << functionName << _T("(x,almost exact)")                 << FullFormatBigReal(exactResult)       << NEWLINE
           << functionName << _T("(getDouble(x)):")                  << dparam( 15) << result                << NEWLINE
           << _T("Relative error:")                                  << udparam(15) << error                 << NEWLINE
           << _T("Max acceptable relative error:")                   << udparam(15) << maxRelativetiveError  << NEWLINE;
    throwException(_T("Error in %s"), functionName.cstr());
  }
}

void checkBigRealDouble80(const String &functionName, Double80Function2 f80, const BigReal &x, const BigReal &y, const BigReal &exactResult) {
  const Double80 result               = f80(getDouble80(x),getDouble80(y));
  const Double80 error                = getRelativeError(result,exactResult);
  const double   maxRelativetiveError = 1e-15;
  if(error > maxRelativetiveError) {
    ERRLOG << _T("Error in ") << functionName << _T(" (Double80,Double80)")                                  << NEWLINE
           << _T("x        :")                                       << FullFormatBigReal(x)                 << NEWLINE
           << _T("y        :")                                       << FullFormatBigReal(y)                 << NEWLINE
           << functionName << _T("(x,y,almost exact)")               << FullFormatBigReal(exactResult)       << NEWLINE
           << functionName << _T("(getDouble80(x),getDouble80(y)):") << dparam( 15) << result                << NEWLINE
           << _T("Relative error:")                                  << udparam(15) << error                 << NEWLINE
           << _T("Max acceptable relative error:")                   << udparam(15) << maxRelativetiveError  << NEWLINE;
    throwException(_T("Error in %s"), functionName.cstr());
  }
}
