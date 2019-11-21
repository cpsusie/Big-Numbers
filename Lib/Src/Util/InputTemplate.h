#pragma once

#include <MyUtil.h>

template<class T> T vinputTemplate(_In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {
  const String prompt = vformat(format, argptr);
  for(;;) {
    tcout << prompt.cstr();
    String line;
    readLine(stdin, line);
    T value;
    tcin >> value;
    if(tcin) return value;
    tcin.clear();
  }
}

template<class T> T inputTemplat(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  const T result = vinputTemplate<T>(format, argptr);
  va_end(argptr);
  return result;
}
