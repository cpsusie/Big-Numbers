#include "pch.h"

unsigned long stringiHash(const String &str) {
  return striHash(str.cstr());
}

int stringiHashCmp(String const &key, String const &elem) {
  return striHashCmp(key.cstr(), elem.cstr());
}

