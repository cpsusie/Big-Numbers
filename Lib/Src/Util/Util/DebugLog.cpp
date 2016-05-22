#include "pch.h"
#include <TinyBitSet.h>
#include <Date.h>

typedef enum {
  FLAG_REDIDRECT
 ,FLAG_APPEND
 ,FLAG_STDOUTATTY
 ,FLAG_STDOUTISCHECKED
} DebugLogFlags;

#pragma warning(disable : 4073)
#pragma init_seg(lib)

static FILE    *traceFile        = stdout;
static BitSet8  traceFlags;
static TCHAR   *redirectFileName = NULL; // has to be a pointer, so it will not be deallocated before any
                                         // destructors of static variables do som logging
static BYTE     timeFormatCode   = 0;

static const TCHAR *timeFormats[] = {
  NULL
 ,_T("hh:mm:ss")
 ,_T("ddMMYY")
 ,_T("ddMMYY hh:mm:ss")
};

void unredirectDebugLog() {
  if(traceFile != stdout) {
    fclose(traceFile);
    traceFile = stdout;
  }
  traceFlags.remove(FLAG_REDIDRECT);
}

static void setRedirectFileName(const String &fileName) {
  if(redirectFileName != NULL) {
    FREE(redirectFileName);
    redirectFileName = NULL;
  }
  redirectFileName = _tcsdup(fileName.cstr());
}

void redirectDebugLog(bool append, const TCHAR *fileName) {
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
}

void debugLogSetTimePrefix(bool prefixWithDate, bool prefixWithTime) {
  timeFormatCode = (prefixWithDate ? 2 : 0) | (prefixWithTime ? 1 : 0);
}

static bool stdoutAtty() {
  if(!traceFlags.contains(FLAG_STDOUTISCHECKED)) {
    if(isatty(stdout)) {
      traceFlags.add(FLAG_STDOUTATTY);
    }
    traceFlags.add(FLAG_STDOUTISCHECKED);
  }
  return traceFlags.contains(FLAG_STDOUTATTY);
}

void debugLog(const TCHAR *format,...) {
  if (traceFile == NULL) traceFile = stdout;
  if(!traceFlags.contains(FLAG_REDIDRECT) && (traceFile == stdout) && !stdoutAtty()) {
    redirectDebugLog();
  }
  if(traceFlags.contains(FLAG_REDIDRECT) && (traceFile == stdout)) {
    traceFile = MKFOPEN(redirectFileName, traceFlags.contains(FLAG_APPEND) ? _T("a") : _T("w"));
    traceFlags.remove(FLAG_REDIDRECT);
  }

  if(timeFormatCode) {
    _ftprintf(traceFile, _T("%s:"), Timestamp().toString(timeFormats[timeFormatCode]).cstr());
  }

  va_list argptr;
  va_start(argptr, format);
  _vftprintf(traceFile, format, argptr);
  va_end(argptr);
  fflush(traceFile);
}

void debugLogLine(const TCHAR *fileName, int line) {
  debugLog(_T("%-30s:line %4d\n"), fileName, line);
}
