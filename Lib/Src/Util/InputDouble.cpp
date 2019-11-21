#include "pch.h"
#include "InputTemplate.h"

double inputDouble(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  const double v = vinputTemplate<double>(format, argptr);
  va_end(argptr);
  return v;
}
