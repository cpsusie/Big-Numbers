#pragma once

#ifdef verify
#undef verify
#endif
#define verify(expr) Assert::IsTrue((expr), _T(#expr))

void OUTPUT(const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  const String msg = vformat(format, argptr);
  va_end(argptr);
  Logger::WriteMessage(msg.cstr());
}
