#pragma once

static void myVerify(bool b, TCHAR *str) {
  if (!b) {
    Assert::IsTrue(b, str);
  }
}

#ifdef verify
#undef verify
#endif
#define verify(expr) myVerify(expr, _T(#expr))

void OUTPUT(const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  const String msg = vformat(format, argptr);
  va_end(argptr);
  Logger::WriteMessage(msg.cstr());
}

class DebugLogRedirector {
public:
  inline DebugLogRedirector() {
    if(!isDebugLogRedirected()) redirectDebugLog(true);
  }
};

static DebugLogRedirector _redirector;
