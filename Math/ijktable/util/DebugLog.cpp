#include "stdafx.h"
#include "FileNameSplitter.h"
#include "FlagTraits.h"
#include "DebugLog.h"

#define FLAG_PREFIXTIME       0x01 // must be 01
#define FLAG_PREFIXDATE       0x02 // must be 02... see array timeFormats and macro GETDATETIMEPREFIX
#define FLAG_REDIDRECT        0x04
#define FLAG_APPEND           0x08
#define FLAG_STDOUTATTY       0x10
#define FLAG_STDOUTISCHECKED  0x20
#define FLAG_ENVIRONCHECKED   0x40
#define FLAG_ENVIRONSET       0x80

#define GETDATETIMEPREFIX(flags) (flags & (FLAG_PREFIXDATE | FLAG_PREFIXTIME))

class DebugLogger  {
private:
  FLAGTRAITS(DebugLogger, BYTE, m_flags);
  FILE                 *m_traceFile;
  TCHAR                *m_fileName; // has to be a pointer, so it will not be deallocated before any
                                    // destructors of static variables do som logging

  DebugLogger()
    : m_traceFile(stdout)
    , m_fileName(NULL)
    , m_flags(0) {
  }
  ~DebugLogger() {
    unredirect().releaseFileName();
  }
  DebugLogger(           const DebugLogger &src); // not implemented
  DebugLogger &operator=(const DebugLogger &src); // not implemented

  bool stdoutAtty();
  bool isEnvironRedirection();
  inline bool mustRedirect() {
    return !stdoutAtty() || isEnvironRedirection();
  }
  DebugLogger &setFileName(const string &fileName);
  DebugLogger &releaseFileName();
  static string generateFileName();
public:
  inline bool isRedirected() const {
    return isSet(FLAG_REDIDRECT) || (m_traceFile != stdout);
  }
  DebugLogger &setTimePrefix(bool prefixWithDate, bool prefixWithTime) {
    return setFlag(FLAG_PREFIXDATE, prefixWithDate).setFlag(FLAG_PREFIXTIME, prefixWithTime);
  }
  DebugLogger &redirect(bool append = false, const TCHAR *fileName = NULL);
  DebugLogger &unredirect();
  DebugLogger &vlog(_In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr);

  static DebugLogger &getInstance();
  inline void notify() {
  }
};

DebugLogger &DebugLogger::getInstance() { // static
  static DebugLogger instance;
  return instance;
}

DebugLogger &DebugLogger::setFileName(const string &fileName) {
  releaseFileName();
  m_fileName = STRDUP(fileName.c_str());
  return *this;
}

DebugLogger &DebugLogger::releaseFileName() {
  if(m_fileName != NULL) {
    FREE(m_fileName);
    m_fileName = NULL;
  }
  return *this;
}

bool DebugLogger::stdoutAtty() {
  if(!isSet(FLAG_STDOUTISCHECKED)) {
    if(true) { // if(isatty(stdout)) {
      setFlag(FLAG_STDOUTATTY);
    }
    setFlag(FLAG_STDOUTISCHECKED);
  }
  return isSet(FLAG_STDOUTATTY);
}

bool DebugLogger::isEnvironRedirection() {
  if(!isSet(FLAG_ENVIRONCHECKED)) {
    TCHAR *v = getenv("DEBUGLOG"); // could parse v, if it contains info about append, timeformat,etc.
    if(v != NULL) setFlag(FLAG_ENVIRONSET);
    setFlag(FLAG_ENVIRONCHECKED);
  }
  return isSet(FLAG_ENVIRONSET);
}

DebugLogger &DebugLogger::unredirect() {
  if(m_traceFile != stdout) {
    fclose(m_traceFile);
    m_traceFile = stdout;
  }
  return clrFlag(FLAG_REDIDRECT);
}

DebugLogger &DebugLogger::redirect(bool append, const TCHAR *fileName) {
  return unredirect()
        .setFileName(fileName?fileName:generateFileName().c_str())
        .setFlag(FLAG_REDIDRECT)
        .setFlag(FLAG_APPEND,append);
}

string DebugLogger::generateFileName() { // static
  FileNameSplitter fileInfo("");
  const string fname = "TraceConvexhull";
  return fileInfo.setDrive("C").setDir("\\temp").setFileName(fname).setExtension("txt").getFullPath();
}

DebugLogger &DebugLogger::vlog(_In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {
  if(m_traceFile == NULL) {
    m_traceFile = stdout;
  }
  if(!isSet(FLAG_REDIDRECT) && (m_traceFile == stdout) && mustRedirect()) {
    redirect();
  }
  if(isSet(FLAG_REDIDRECT) && (m_traceFile == stdout)) {
    m_traceFile = MKFOPEN(m_fileName, isSet(FLAG_APPEND) ? "a" : "w");
//    setvbuf(traceFile, NULL, _IONBF, 100);
    clrFlag(FLAG_REDIDRECT);
  }
  vfprintf(m_traceFile, format, argptr);
  fflush(m_traceFile);
  return *this;
}

void redirectDebugLog(bool append, const TCHAR *fileName) {
  DebugLogger::getInstance().redirect(append, fileName).notify();
}

void unredirectDebugLog() {
  DebugLogger::getInstance().unredirect().notify();
}

bool isDebugLogRedirected() {
  DebugLogger &logger = DebugLogger::getInstance();
  const bool result = logger.isRedirected();
  logger.notify();
  return result;
}

void vdebugLog(_In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {
  DebugLogger::getInstance().vlog(format, argptr).notify();
}

void debugLog(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  vdebugLog(format, argptr);
  va_end(argptr);
}

void debugLogLine(const TCHAR *fileName, int line) {
  debugLog("%-30s:line %4d\n", fileName, line);
}

void debugLogSetTimePrefix(bool prefixWithDate, bool prefixWithTime) {
  DebugLogger &logger = DebugLogger::getInstance();
  logger.setTimePrefix(prefixWithDate, prefixWithTime);
  logger.notify();
}
