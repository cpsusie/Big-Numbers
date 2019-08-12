#include "pch.h"

String &String::trim() {
  strTrim(m_buf);
  m_len = _tcsclen(m_buf);
  return *this;
}

String &String::trimLeft() {
  strTrimLeft(m_buf);
  m_len = _tcsclen(m_buf);
  return *this;
}

String &String::trimRight() {
  strTrimRight(m_buf);
  m_len = _tcsclen(m_buf);
  return *this;
}
