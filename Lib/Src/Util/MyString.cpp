#include "pch.h"

TCHAR *String::newCharBuffer(const TCHAR *s, size_t &length, size_t &capacity) { // static
  length = _tcsclen(s);
  TCHAR *result = new TCHAR[capacity = length + 1];
  if(result == nullptr) {
    throwInvalidArgumentException(__TFUNCTION__, _T("Out of memory. Requested size=%s"), format1000(capacity).cstr());
  }
  return _tcscpy(result,s);
}

String::String() {
  m_buf = newCharBuffer(EMPTYSTRING, m_len, m_capacity);
}

String::String(const String &s) {
  m_len = s.m_len;
  m_buf = new TCHAR[m_capacity = m_len+1];
  TMEMCPY(m_buf, s.m_buf, m_capacity); // handles 0-characters in s
}

String::String(const wchar_t *s) {
  USES_WCONVERSION;
  m_buf = newCharBuffer(W2TNULL(s), m_len, m_capacity);
}

String::~String() {
  delete[] m_buf;
}

String &String::operator=(const String &rhs) {
  if(this != &rhs) {
    if(!needReallocate(rhs.m_len)) {
      TMEMCPY(m_buf, rhs.m_buf, (m_len=rhs.m_len)+1);
    } else {
      delete[] m_buf;
      m_buf = new TCHAR[m_capacity = (m_len = rhs.m_len)+1];
      TMEMCPY(m_buf, rhs.m_buf, m_capacity); // handles 0-characters in rhs
    }
  }
  return *this;
}

String &String::operator=(const wchar_t *s) {
  USES_WCONVERSION;
  const TCHAR *ms     = W2TNULL(s);
  const size_t length = _tcsclen(ms);

  if(!needReallocate(length)) {
    _tcscpy(m_buf, ms);
    m_len = length;
  } else {
    TCHAR *old = m_buf;
    m_buf     = newCharBuffer(ms, m_len, m_capacity);
    delete[] old;
  }
  return *this;
}

String &String::operator+=(const String &rhs) {
  if(this == &rhs) {
    *this = *this + rhs;
  } else {
    if(rhs.m_len == 0) {
      return *this;
    }
    const size_t newLength = m_len + rhs.m_len;
    if(m_capacity < newLength + 1) {
      TCHAR *old = m_buf;
      m_buf = new TCHAR[m_capacity = (newLength + 1) * 3];
      TMEMCPY(m_buf, old, m_len);
      delete[] old;
    }
    TMEMCPY(m_buf + m_len, rhs.m_buf, rhs.m_len+1);  // handles 0-characters in rhs
    m_len = newLength;
  }
  return *this;
}

String &String::operator+=(const wchar_t *rhs) {
  USES_WCONVERSION;
  const TCHAR *mrhs   = W2TNULL(rhs);
  const size_t length = _tcsclen(mrhs);
  if(length == 0) {
    return *this;
  }
  const size_t newLength = m_len + length;
  if(m_capacity < newLength + 1) {
    TCHAR *old = m_buf;
    m_buf = new TCHAR[m_capacity = (newLength + 1) * 3];
    TMEMCPY(m_buf, old, m_len);
    _tcsncpy(m_buf + m_len, mrhs, length);
    m_buf[m_len = newLength] = '\0';
    delete[] old; // Dont delete old before now, rhs and old might overlap
  } else {
    _tcsncpy(m_buf + m_len, mrhs, length); // Dont use strcpy. rhs and m_buf might overlap, which would give an infinite loop
    m_buf[m_len = newLength] = '\0';
  }
  return *this;
}

String operator+(const String &lhs, const String &rhs) {
  String result;
  delete[] result.m_buf;
  result.m_len = lhs.m_len + rhs.m_len;
  result.m_capacity = result.m_len + 1;
  result.m_buf = new TCHAR[result.m_capacity];
  TMEMCPY(result.m_buf, lhs.m_buf, lhs.m_len);             // handles 0-characters in lhs
  TMEMCPY(result.m_buf+lhs.m_len, rhs.m_buf, rhs.m_len);   // and rhs
  result.m_buf[result.m_len] = 0;
  return result;
}

void String::indexError(size_t index) const {
  throwInvalidArgumentException(__TFUNCTION__
                               , _T("Index %s out of range in string <%s>. length=%s")
                               , format1000(index).cstr()
                               , m_buf
                               , format1000(m_len).cstr());
}
