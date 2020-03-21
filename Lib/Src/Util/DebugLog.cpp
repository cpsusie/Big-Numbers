#include "pch.h"
#include <MyUtil.h>
#include <Singleton.h>
#include <FileNameSplitter.h>
#include <FlagTraits.h>
#include <Date.h>
#include <DebugLog.h>

#define FLAG_PREFIXTIME       0x01 // must be 01
#define FLAG_PREFIXDATE       0x02 // must be 02... see array timeFormats and macro GETDATETIMEPREFIX
#define FLAG_REDIDRECT        0x04
#define FLAG_APPEND           0x08
#define FLAG_STDOUTATTY       0x10
#define FLAG_STDOUTISCHECKED  0x20
#define FLAG_ENVIRONCHECKED   0x40
#define FLAG_ENVIRONSET       0x80

#define GETDATETIMEPREFIX(flags) (flags & (FLAG_PREFIXDATE | FLAG_PREFIXTIME))

static const TCHAR *timeFormats[] = {
  EMPTYSTRING
 ,hhmmss
 ,ddMMyy
 ,ddMMyyhhmmss
};

class DebugLogger : public Singleton {
private:
  friend class SingletonFactory;
  mutable FastSemaphore m_lock;
  FLAGTRAITS(BYTE, DebugLogger);
  FILE                 *m_traceFile;
  TCHAR                *m_fileName; // has to be a pointer, so it will not be deallocated before any
                                    // destructors of static variables do som logging

  DebugLogger(SingletonFactory *factory)
    : Singleton(factory)
    , m_traceFile(stdout)
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
  DebugLogger &setFileName(const String &fileName);
  DebugLogger &releaseFileName();
  static String generateFileName();
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
    m_lock.notify();
  }
};

DEFINESINGLETON(DebugLogger);

DebugLogger &DebugLogger::getInstance() { // static
  DebugLogger &logger = getDebugLogger();
  logger.m_lock.wait();
  return logger;
}

DebugLogger &DebugLogger::setFileName(const String &fileName) {
  releaseFileName();
  m_fileName = _tcsdup(fileName.cstr());
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
    if(isatty(stdout)) {
      setFlag(FLAG_STDOUTATTY);
    }
    setFlag(FLAG_STDOUTISCHECKED);
  }
  return isSet(FLAG_STDOUTATTY);
}

bool DebugLogger::isEnvironRedirection() {
  if(!isSet(FLAG_ENVIRONCHECKED)) {
    TCHAR *v = _tgetenv(_T("DEBUGLOG")); // could parse v, if it contains info about append, timeformat,etc.
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
        .setFileName(fileName?fileName:generateFileName().cstr())
        .setFlag(FLAG_REDIDRECT)
        .setFlag(FLAG_APPEND,append);
}

String DebugLogger::generateFileName() { // static
  FileNameSplitter fileInfo(getModuleFileName());
#ifdef UNICODE
#define _CHARSET_ "U"
#else
#define _CHARSET_
#endif
  String fnamePrefix(_T("Trace" _PLATFORM_ _TMPREFIX_ _CONFIGURATION_ _CHARSET_));
  fnamePrefix.replace('/', EMPTYSTRING);
  const String fname = fnamePrefix + fileInfo.getFileName();
  return fileInfo.setDrive(_T("C")).setDir(_T("\\temp")).setFileName(fname).setExtension(_T("txt")).getFullPath();
}

DebugLogger &DebugLogger::vlog(_In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {
  if(m_traceFile == NULL) {
    m_traceFile = stdout;
  }
  if(!isSet(FLAG_REDIDRECT) && (m_traceFile == stdout) && mustRedirect()) {
    redirect();
  }
  if(isSet(FLAG_REDIDRECT) && (m_traceFile == stdout)) {
    m_traceFile = MKFOPEN(m_fileName, isSet(FLAG_APPEND) ? _T("a") : _T("w"));
//    setvbuf(traceFile, NULL, _IONBF, 100);
    clrFlag(FLAG_REDIDRECT);
  }
  BYTE  prefixIndex;
  UINT  prefixLength = 0;
  TCHAR timestr[60];
  if(prefixIndex = GETDATETIMEPREFIX(m_flags)) {
    Timestamp().tostr(timestr, timeFormats[prefixIndex]);
    prefixLength = (UINT)_tcslen(timestr);
  }
  if(prefixLength == 0) {
    _vftprintf(m_traceFile, format, argptr);
  } else {
    String str = indentString(vformat(format, argptr), prefixLength);
    if(str.length() >= prefixLength) {
      _tcsncpy(str.cstr(), timestr, prefixLength);
    } else {
      str = timestr + str;
    }
    _fputts(str.cstr(), m_traceFile);
  }
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
  debugLog(_T("%-30s:line %4d\n"), fileName, line);
}

void debugLogSetTimePrefix(bool prefixWithDate, bool prefixWithTime) {
  DebugLogger &logger = DebugLogger::getInstance();
  logger.setTimePrefix(prefixWithDate, prefixWithTime);
  logger.notify();
}
