#include "pch.h"

bool strToBool(const TCHAR *s) {
  String tmp = s;
  tmp.trim();
  if (tmp.equalsIgnoreCase(_T("false"))) {
    return false;
  } else if (tmp.equalsIgnoreCase(_T("true"))) {
    return true;
  }
  throwException(_T("%s:Invalid input:<%s>. Expected \"true\" or \"false\""), __TFUNCTION__, s);
  return false;
}
