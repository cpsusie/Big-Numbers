#include "pch.h"
#include "InputTemplate.h"

int inputInt(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  const int v = vinputTemplate<int>(format, argptr);
  va_end(argptr);
  return v;
}
