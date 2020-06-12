#pragma once

static void myVerify(bool b, TCHAR *str) {
  if(!b) {
    Assert::IsTrue(b, str);
  }
}

#if defined(verify)
#undef verify
#endif
#define verify(expr) myVerify(expr, _T(#expr))

void OUTPUT(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  const String msg = vformat(format, argptr);
  va_end(argptr);
  Logger::WriteMessage(msg.cstr());
}

#if defined(_DEBUG)
#define INFO(...) OUTPUT(__VA_ARGS__)
#else
#define INFO(...)
#endif

class DebugLogRedirector {
public:
  inline DebugLogRedirector() {
    if(!isDebugLogRedirected()) redirectDebugLog(true);
  }
};

static DebugLogRedirector _redirector;

#define verifyEqualsInt(   expected, value)            verify(value == expected)
#define verifyAlmostEquals(expected, value,tolerance)  verify(fabs((expected)-(value)) <= (tolerance))
