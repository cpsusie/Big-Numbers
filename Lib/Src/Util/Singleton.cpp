#include "pch.h"
#include <Unicode.h>
#include <MyUtil.h>
#include <Singleton.h>

// #define TRACE_SINGLETON

#if defined(TRACE_SINGLETON)

// Don't use debugLog here....it uses a Singleton for logging
static void singletonTrace(UINT count, const TCHAR *function, const TCHAR *format, ...) {
  static FILE *logFile = MKFOPEN(_T("c:\\temp\\singletons.txt"), _T("w"));
  _ftprintf(logFile, _T("%*.*s%-*s(threadId:%5d):"), count,count, _T(""), max(0,30-(int)count),function, GetCurrentThreadId());
  va_list argptr;
  va_start(argptr, format);
  _vftprintf(logFile, format, argptr);
  va_end(argptr);
  fflush(logFile);
}

#define SINGTRACE(format,...) singletonTrace(s_count, __TFUNCTION__,_T(format), __VA_ARGS__)
#else
#define SINGTRACE(...)
#endif

std::atomic<UINT> Singleton::s_count = 0;

Singleton::Singleton(const TCHAR *constructorName) : m_className(getClassNameFromConstructorName(constructorName)) {
#if defined(TRACE_SINGLETON)
  s_count++;
  SINGTRACE("Singleton %s created\n", getClassName().cstr());
#endif
}

Singleton::~Singleton() {
#if defined(TRACE_SINGLETON)
  SINGTRACE("Singleton %s destroyed\n", getClassName().cstr());
  s_count--;
#endif
}

String Singleton::getClassNameFromConstructorName(const TCHAR *constructorName) { // static
  const String   result = constructorName;
  const intptr_t index  = result.rfind(_T(':'));
  return (index < 0) ? result : left(result, index - 1);
}
