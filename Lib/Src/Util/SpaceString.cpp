#include "pch.h"

String spaceString(std::streamsize length, TCHAR ch) {
  if (length <= 0) {
    return EMPTYSTRING;
  }
  String result;
  delete[] result.m_buf;
  result.m_buf = new TCHAR[result.m_capacity = (result.m_len = (size_t)length) + 1];
  TMEMSET(result.m_buf, ch, (size_t)length);
  result.m_buf[length] = 0;
  return result;
}
