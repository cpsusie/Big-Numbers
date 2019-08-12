#include "pch.h"

String &String::vprintf(_In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {
  TCHAR buffer[16384];
  int bufSize = ARRAYSIZE(buffer);
  TCHAR *tmp = buffer;
  for (;;) {
    int written;
    if((written = _vsntprintf(tmp, bufSize, format, argptr)) >= 0) {
      if ((written >= (int)m_capacity) || (written + 100 < (int)m_capacity)) {
        delete[] m_buf;
        m_buf = new TCHAR[m_capacity = written + 1];
      }
      TMEMCPY(m_buf, tmp, (m_len = written) + 1);
      if (tmp != buffer) {
        delete[] tmp;
      }
      break;
    }
    if (tmp != buffer) {
      delete[] tmp;
    }
    bufSize *= 3;
    tmp = new TCHAR[bufSize];
  }
  return *this;
}

String &String::printf(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  vprintf(format, argptr);
  va_end(argptr);
  return *this;
}

String format(_In_z_ _Printf_format_string_ TCHAR const * const Format, ...) {
  va_list argptr;
  va_start(argptr, Format);
  String result;
  result.vprintf(Format, argptr);
  va_end(argptr);
  return result;
}
