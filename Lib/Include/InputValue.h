#pragma once

#include "MyUtil.h"

template<typename T> T vinputValue(_In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {
  const String prompt = vformat(format, argptr);
  for(;;) {
    tcout << prompt.cstr();
    T value;
    tcin >> value;
    if(tcin) return value;
    tcin.clear();
  }
}

template<typename T> T inputValue(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  const T result = vinputValue<T>(format, argptr);
  va_end(argptr);
  return result;
}
