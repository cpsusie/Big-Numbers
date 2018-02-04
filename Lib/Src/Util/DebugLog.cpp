#include "pch.h"
#include <TinyBitSet.h>
#include <Date.h>
#include <Semaphore.h>

typedef enum {
  FLAG_REDIDRECT
 ,FLAG_APPEND
 ,FLAG_STDOUTATTY
 ,FLAG_STDOUTISCHECKED
 ,FLAG_ENVIRONCHECKED
 ,FLAG_ENVIRONSET
} DebugLogFlags;

#pragma warning(disable : 4073)
#pragma init_seg(lib)

static FILE     *traceFile        = stdout;
static BitSet8   traceFlags;
static TCHAR    *redirectFileName = NULL; // has to be a pointer, so it will not be deallocated before any
                                         // destructors of static variables do som logging
static Semaphore gate;
static BYTE      timeFormatCode   = 0;

static const String timeFormats[] = {
  EMPTYSTRING
 ,_T("hh:mm:ss")
 ,_T("ddMMyy")
 ,_T("ddMMyy hh:mm:ss")
};

bool isDebugLogRedirected() {
  gate.wait();
  const bool result = traceFlags.contains(FLAG_REDIDRECT) || (traceFile != stdout);
  gate.signal();
  return result;
}

void unredirectDebugLog() {
  gate.wait();
  if(traceFile != stdout) {
    fclose(traceFile);
    traceFile = stdout;
  }
  traceFlags.remove(FLAG_REDIDRECT);
  gate.signal();
}

static void setRedirectFileName(const String &fileName) {
  if(redirectFileName != NULL) {
    FREE(redirectFileName);
    redirectFileName = NULL;
  }
  redirectFileName = _tcsdup(fileName.cstr());
}

void redirectDebugLog(bool append, const TCHAR *fileName) {
  gate.wait();
  if(traceFile != stdout) {
    fclose(traceFile);
    traceFile = stdout;
  }

  if(fileName) {
    setRedirectFileName(fileName);
  } else {
    FileNameSplitter fileInfo(getModuleFileName());
#ifdef UNICODE
#ifdef _DEBUG
    const String fname = _T("DebugUTrace_"  ) + fileInfo.getFileName();
#else
    const String fname = _T("ReleaseUTrace_") + fileInfo.getFileName();
#endif
#else

#ifdef _DEBUG
    const String fname = _T("DebugTrace_"  ) + fileInfo.getFileName();
#else
    const String fname = _T("ReleaseTrace_") + fileInfo.getFileName();
#endif
#endif

    setRedirectFileName(fileInfo.setDrive(_T("C")).setDir(_T("\\temp")).setFileName(fname).setExtension(_T("txt")).getFullPath());
  }

  traceFlags.add(FLAG_REDIDRECT);
  if(append) {
    traceFlags.add(FLAG_APPEND);
  } else {
    traceFlags.remove(FLAG_APPEND);
  }
  gate.signal();
}

void debugLogSetTimePrefix(bool prefixWithDate, bool prefixWithTime) {
  timeFormatCode = (prefixWithDate ? 2 : 0) | (prefixWithTime ? 1 : 0);
}

static bool stdoutAtty() {
  if(!traceFlags.contains(FLAG_STDOUTISCHECKED)) {
    gate.wait();
    if(isatty(stdout)) {
      traceFlags.add(FLAG_STDOUTATTY);
    }
    traceFlags.add(FLAG_STDOUTISCHECKED);
    gate.signal();
  }
  return traceFlags.contains(FLAG_STDOUTATTY);
}

static inline bool isEnvironRedirection() {
  if(!traceFlags.contains(FLAG_ENVIRONCHECKED)) {
    gate.wait();
    TCHAR *v = _tgetenv(_T("DEBUGLOG")); // could parse v, if it contains info about append, timeformat,etc.
    if(v != NULL) traceFlags.add(FLAG_ENVIRONSET);
    traceFlags.add(FLAG_ENVIRONCHECKED);
    gate.signal();
  }
  return traceFlags.contains(FLAG_ENVIRONSET);
}

static inline bool mustRedirect() {
  return !stdoutAtty() || isEnvironRedirection();
}

void debugLog(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  vdebugLog(format, argptr);
  va_end(argptr);
}

void vdebugLog(_In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {
  if(traceFile == NULL) {
    traceFile = stdout;
  }
  if(!traceFlags.contains(FLAG_REDIDRECT) && (traceFile == stdout) && mustRedirect()) {
    redirectDebugLog();
  }
  if(traceFlags.contains(FLAG_REDIDRECT) && (traceFile == stdout)) {
    gate.wait();
    traceFile = MKFOPEN(redirectFileName, traceFlags.contains(FLAG_APPEND) ? _T("a") : _T("w"));
//    setvbuf(traceFile, NULL, _IONBF, 100);
    traceFlags.remove(FLAG_REDIDRECT);
    gate.signal();
  }

  if(timeFormatCode) {
    _ftprintf(traceFile, _T("%s:"), Timestamp().toString(timeFormats[timeFormatCode]).cstr());
  }

  _vftprintf(traceFile, format, argptr);
  fflush(traceFile);
}

void debugLogLine(const TCHAR *fileName, int line) {
  debugLog(_T("%-30s:line %4d\n"), fileName, line);
}
