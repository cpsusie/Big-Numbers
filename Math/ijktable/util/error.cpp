#include "stdafx.h"
#include "error.h"

void throwMethodException(const char *className, const char *method, _In_z_ _Printf_format_string_ char const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  const string msg = vformat(format, argptr);
  va_end(argptr);
  throwException("%s::%s:%s", className, method, msg.c_str());
}

void throwUnsupportedOperationException(const char *method) {
  throwException("%s:Unsupported operation", method);
}

void throwInvalidArgumentException(const char *method, _In_z_ _Printf_format_string_ char const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  const string msg = vformat(format, argptr);
  va_end(argptr);
  throwException("%s:Invalid argument. %s", method, msg.c_str());
}

void throwException(const string &s) {
  throw Exception(s);
}

void throwException(_In_z_ _Printf_format_string_ char const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  const string tmp = vformat(format, argptr);
  va_end(argptr);
  throwException(tmp);
}

void throwPROCEDURE_ERROR(const char *method, const string msg) {
  throwException("%s:%s", method, msg.c_str());
}

void throwPROCEDURE_ERROR(const char *method, _In_z_ _Printf_format_string_ char const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  const string msg = vformat(format, argptr);
  va_end(argptr);
  throwException("Error in %s:%s", method, msg.c_str());
}

#ifdef _DEBUG
void xassert(const char *fileName, int line, const char *exp) {
  fprintf(stderr, "\n\nAssertion %s at %s line %d failed.\n", exp, fileName, line);
  abort();
}
#endif
