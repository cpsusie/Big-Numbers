#include "pch.h"

ULONG stringHash(const String &str) {
  return strHash(str.cstr());
}

int stringHashCmp(String const &key, String const &elem) {
  return strHashCmp(key.cstr(), elem.cstr());
}

