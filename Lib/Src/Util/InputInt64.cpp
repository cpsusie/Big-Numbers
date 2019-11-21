#include "pch.h"
#include "InputTemplate.h"

INT64 inputInt64(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  const INT64 v = vinputTemplate<INT64>(format, argptr);
  va_end(argptr);
  return v;
}
