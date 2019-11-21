#include "pch.h"
#include "InputTemplate.h"

UINT inputUint(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  const UINT v = vinputTemplate<UINT>(format, argptr);
  va_end(argptr);
  return v;
}
