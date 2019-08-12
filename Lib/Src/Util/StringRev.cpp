#include "pch.h"

String rev(const String &str) {
  String result(str);
  for (TCHAR *left = result.m_buf, *right = left + result.m_len - 1; left < right;) {
    TCHAR tmp = *left;
    *(left++) = *right;
    *(right--) = tmp;
  }
  return result;
}
