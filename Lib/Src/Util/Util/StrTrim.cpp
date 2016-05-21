#include "pch.h"

TCHAR *strTrim(TCHAR *s) {
  return strTrimLeft(strTrimRight(s));
}
