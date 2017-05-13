#include "pch.h"

ULONG stringHash(const String &s) {
  return strnHash(s.cstr(), s.length());
}

int stringHashCmp(String const &s1, String const &s2) {
  return strnHashCmp(s1.cstr(), s2.cstr(), s1.length(), s2.length());
}

