#include "pch.h"
#include "InputTemplate.h"

UINT64 inputUint64(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  const UINT64 v = vinputTemplate<UINT64>(format, argptr);
  va_end(argptr);
  return v;
}
