#include "pch.h"

String left(const String &str, intptr_t length) {
  if (length >= (intptr_t)str.m_len) {
    return str;
  }
  else if (length <= 0) {
    return EMPTYSTRING;
  }
  else {
    String result(str);
    result.m_buf[result.m_len = length] = 0;
    return result;
  }
}

String right(const String &str, intptr_t length) {
  if (length >= (intptr_t)str.m_len) {
    return str;
  }
  else if (length <= 0) {
    return EMPTYSTRING;
  }
  else {
    return String(str.m_buf + (intptr_t)str.m_len - length);
  }
}

String substr(const String &str, intptr_t from, intptr_t length) {
  if ((from < 0) || (from >= (intptr_t)str.m_len) || (length <= 0)) {
    return EMPTYSTRING;
  }
  else {
    if (length > (intptr_t)str.m_len - from) {
      length = str.m_len - from;
    }
    String result;
    delete[] result.m_buf;
    result.m_buf = String::newCharBuffer(str.m_buf + from, result.m_len, result.m_capacity);
    result.m_buf[result.m_len = length] = 0;
    return result;
  }
}
