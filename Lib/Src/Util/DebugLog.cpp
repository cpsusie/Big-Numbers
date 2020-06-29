#include "pch.h"
#include <MyUtil.h>
#include <Singleton.h>
#include <FastSemaphore.h>
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
  mutable FastSemaphore m_lock;
  FLAGTRAITS(DebugLogger, BYTE, m_flags);
  FILE                 *m_traceFile;
  TCHAR                *m_fileName; // has to be a pointer, so it will not be deallocated before any
                                    // destructors of static variables do som logging

  // no lock
  bool StdoutAtty();
  // no lock
  bool IsEnvironRedirection();
  // no lock
  inline bool MustRedirect() {
    return !StdoutAtty() || IsEnvironRedirection();
  }
  static String GenerateFileName();
  // no lock
  void SetFileName(const String &fileName);
  // no lock
  void ReleaseFileName();
  // no lock
  void Redirect(bool append = false, const TCHAR *fileName = NULL);
  // no lock
  void Unredirect();
  // no lock
  void Vlog(_In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr);

  DebugLogger()
    : Singleton(  __TFUNCTION__)
    , m_traceFile(stdout       )
    , m_fileName( NULL         )
    , m_flags(    0            )
  {
  }
  ~DebugLogger() override;
public:
  void redirect(bool append = false, const TCHAR *fileName = NULL);
  void unredirect();
  bool isRedirected() const;
  void setTimePrefix(bool prefixWithDate, bool prefixWithTime);
  void vlog(_In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr);
  DEFINESINGLETON(DebugLogger)
};

DebugLogger::~DebugLogger() {
  m_lock.wait();
  Unredirect();
  ReleaseFileName();
  m_lock.notify();
}



// private...no lock

String DebugLogger::GenerateFileName() { // static
  FileNameSplitter fileInfo(getModuleFileName());
#if defined(UNICODE)
#define _CHARSET_ "U"
#else
#define _CHARSET_
#endif
  String fnamePrefix(_T("Trace" _PLATFORM_ _TMPREFIX_ _CONFIGURATION_ _CHARSET_));
  fnamePrefix.replace('/', EMPTYSTRING);
  const String fname = fnamePrefix + fileInfo.getFileName();
  return fileInfo.setDrive(_T("C")).setDir(_T("\\temp")).setFileName(fname).setExtension(_T("txt")).getFullPath();
}

void DebugLogger::SetFileName(const String &fileName) {
  ReleaseFileName();
  m_fileName = _tcsdup(fileName.cstr());
}

void DebugLogger::ReleaseFileName() {
  if(m_fileName != NULL) {
    FREE(m_fileName);
    m_fileName = NULL;
  }
}

bool DebugLogger::StdoutAtty() {
  if(!isSet(FLAG_STDOUTISCHECKED)) {
    if(isatty(stdout)) {
      setFlag(FLAG_STDOUTATTY);
    }
    setFlag(FLAG_STDOUTISCHECKED);
  }
  return isSet(FLAG_STDOUTATTY);
}

bool DebugLogger::IsEnvironRedirection() {
  if(!isSet(FLAG_ENVIRONCHECKED)) {
    TCHAR *v = _tgetenv(_T("DEBUGLOG")); // could parse v, if it contains info about append, timeformat,etc.
    if(v != NULL) setFlag(FLAG_ENVIRONSET);
    setFlag(FLAG_ENVIRONCHECKED);
  }
  return isSet(FLAG_ENVIRONSET);
}

void DebugLogger::Redirect(bool append, const TCHAR *fileName) {
  Unredirect();
  SetFileName(fileName ? fileName : GenerateFileName().cstr());
  setFlag(FLAG_REDIDRECT);
  setFlag(FLAG_APPEND,append);
}

void DebugLogger::Unredirect() {
  if(m_traceFile != stdout) {
    fclose(m_traceFile);
    m_traceFile = stdout;
  }
  clrFlag(FLAG_REDIDRECT);
}

void DebugLogger::Vlog(_In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {
  if(m_traceFile == NULL) {
    m_traceFile = stdout;
  }
  if(!isSet(FLAG_REDIDRECT) && (m_traceFile == stdout) && MustRedirect()) {
    Redirect();
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
}

// public..... with lock
void DebugLogger::unredirect() {
  m_lock.wait();
  Unredirect();
  m_lock.notify();
}

void DebugLogger::redirect(bool append, const TCHAR *fileName) {
  m_lock.wait();
  Redirect(append, fileName);
  m_lock.notify();
}

bool DebugLogger::isRedirected() const {
  m_lock.wait();
  const bool result = isSet(FLAG_REDIDRECT) || (m_traceFile != stdout);
  m_lock.notify();
  return result;
}

void DebugLogger::setTimePrefix(bool prefixWithDate, bool prefixWithTime) {
  m_lock.wait();
  setFlag(FLAG_PREFIXDATE, prefixWithDate).setFlag(FLAG_PREFIXTIME, prefixWithTime);
  m_lock.notify();
}

void DebugLogger::vlog(_In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {
  m_lock.wait();
  try {
    Vlog(format, argptr);
    m_lock.notify();
  } catch(...) {
    m_lock.notify();
    throw;
  }
}

void redirectDebugLog(bool append, const TCHAR *fileName) {
  DebugLogger::getInstance().redirect(append, fileName);
}

void unredirectDebugLog() {
  DebugLogger::getInstance().unredirect();
}

bool isDebugLogRedirected() {
  return DebugLogger::getInstance().isRedirected();
}

void vdebugLog(_In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {
  DebugLogger::getInstance().vlog(format, argptr);
}

void debugLog(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  DebugLogger::getInstance().vlog(format, argptr);
  va_end(argptr);
}

void debugLogLine(const TCHAR *fileName, int line) {
  debugLog(_T("%-30s:line %4d\n"), fileName, line);
}

void debugLogSetTimePrefix(bool prefixWithDate, bool prefixWithTime) {
  DebugLogger::getInstance().setTimePrefix(prefixWithDate, prefixWithTime);
}
