#include "pch.h"

TCHAR *String::newCharBuffer(const TCHAR *s, size_t &length, size_t &capacity) { // static
  length = _tcsclen(s);
  TCHAR *result = new TCHAR[capacity = length + 1];
  if(result == NULL) {
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
  MEMCPY(m_buf, s.m_buf, m_capacity); // handles 0-characters in s
}

String::String(const TCHAR *s) {
  m_buf = newCharBuffer(s?s:_T("null"), m_len, m_capacity);
}

String::String(char ch) {
  TCHAR tmp[] = { (TCHAR)ch, 0 };
  m_buf = newCharBuffer(tmp, m_len, m_capacity);
}

String::String(SHORT n) {
  TCHAR tmp[20];
  _stprintf(tmp, _T("%hd"), n);
  m_buf = newCharBuffer(tmp, m_len, m_capacity);
}

String::String(USHORT n) {
  TCHAR tmp[20];
  _stprintf(tmp, _T("%hu"), n);
  m_buf = newCharBuffer(tmp, m_len, m_capacity);
}

String::String(INT n) {
  TCHAR tmp[20];
  _stprintf(tmp, _T("%d"), n);
  m_buf = newCharBuffer(tmp, m_len, m_capacity);
}

String::String(UINT n) {
  TCHAR tmp[20];
  _stprintf(tmp, _T("%u"), n);
  m_buf = newCharBuffer(tmp, m_len, m_capacity);
}

String::String(LONG n) {
  TCHAR tmp[20];
  _stprintf(tmp, _T("%ld"), n);
  m_buf = newCharBuffer(tmp, m_len, m_capacity);
}

String::String(ULONG n) {
  TCHAR tmp[20];
  _stprintf(tmp, _T("%lu"), n);
  m_buf = newCharBuffer(tmp, m_len, m_capacity);
}

String::String(INT64 n) {
  TCHAR tmp[100];
  _stprintf(tmp, _T("%I64d"), n);
  m_buf = newCharBuffer(tmp, m_len, m_capacity);
}

String::String(UINT64 n) {
  TCHAR tmp[100];
  _stprintf(tmp, _T("%I64u"), n);
  m_buf = newCharBuffer(tmp, m_len, m_capacity);
}

String::String(float flt) {
  TCHAR tmp[100];
  _stprintf(tmp, _T("%g"), flt);
  m_buf = newCharBuffer(tmp, m_len, m_capacity);
}

String::String(double dbl) {
  TCHAR tmp[100];
  _stprintf(tmp, _T("%lg"), dbl);
  m_buf = newCharBuffer(tmp, m_len, m_capacity);
}

String::~String() {
  delete[] m_buf;
}

String &String::operator=(const String &rhs) {
  if(this != &rhs) {
    if(rhs.m_len < m_capacity && rhs.m_len + 100 < m_capacity) {
      MEMCPY(m_buf, rhs.m_buf, (m_len=rhs.m_len)+1);
    } else {
      delete[] m_buf;
      m_buf = new TCHAR[m_capacity = (m_len = rhs.m_len)+1];
      MEMCPY(m_buf, rhs.m_buf, m_capacity); // handles 0-characters in rhs
    }
  }
  return *this;
}

String &String::operator=(const TCHAR *s) {
  if(s == NULL) {
    s = _T("null");
  }
  size_t length = _tcsclen(s);
  if(length < m_capacity && length + 100 > m_capacity) {
    _tcscpy(m_buf, s);
    m_len = length;
  } else {
    TCHAR *old = m_buf;
    m_buf     = newCharBuffer(s, m_len, m_capacity);
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
      MEMCPY(m_buf, old, m_len);
      delete[] old;
    }
    MEMCPY(m_buf + m_len, rhs.m_buf, rhs.m_len+1);  // handles 0-characters in rhs
    m_len = newLength;
  }
  return *this;
}

String &String::operator+=(const TCHAR *rhs) {
  if(rhs == NULL) {
    rhs = _T("null");
  }
  const size_t length = _tcsclen(rhs);
  if(length == 0) {
    return *this;
  }
  const size_t newLength = m_len + length;
  if(m_capacity < newLength + 1) {
    TCHAR *old = m_buf;
    m_buf = new TCHAR[m_capacity = (newLength + 1) * 3];
    MEMCPY(m_buf, old, m_len);
    _tcsncpy(m_buf + m_len, rhs, length);
    m_buf[m_len = newLength] = 0;
    delete[] old; // Dont delete old before now, rhs and old might overlap
  } else {
    _tcsncpy(m_buf + m_len, rhs, length); // Dont use strcpy. rhs and m_buf might overlap, which would give an infinite loop
    m_buf[m_len = newLength] = 0;
  }
  return *this;
}

String &String::operator+=(TCHAR ch) {
  TCHAR tmp[] = { ch, 0 };
  return *this += tmp;
}

String operator+(const String &lhs, const String &rhs) {
  String result;
  delete[] result.m_buf;
  result.m_len = lhs.m_len + rhs.m_len;
  result.m_capacity = result.m_len + 1;
  result.m_buf = new TCHAR[result.m_capacity];
  MEMCPY(result.m_buf, lhs.m_buf, lhs.m_len);             // handles 0-characters in lhs
  MEMCPY(result.m_buf+lhs.m_len, rhs.m_buf, rhs.m_len);   // and rhs
  result.m_buf[result.m_len] = 0;
  return result;
}

intptr_t String::find(TCHAR ch, size_t from) const {
  if(from >= m_len) {
    return -1;
  }
  TCHAR *s = _tcschr(m_buf + from, ch);
  return s ? s - m_buf : -1;
}

intptr_t String::find(const String &str, size_t from) const {
  if(str.length() + from > m_len) {
    return -1;
  }
  TCHAR *s = _tcsstr(m_buf + from, str.m_buf);
  return s ? (s - m_buf) : -1;
}

intptr_t String::find(const TCHAR *str, size_t from) const {
  if(from > m_len) {
    return -1;
  }
  TCHAR *s = _tcsstr(m_buf + from, str);
  return s ? (s - m_buf) : -1;
}

String &String::insert(size_t pos, TCHAR ch) {
  if(pos <= m_len) {
    if(m_capacity < m_len + 2) {
      m_capacity = (m_len + 2) * 2;
      TCHAR *tmp = new TCHAR[m_capacity];
      MEMCPY(tmp, m_buf, pos);
      tmp[pos] = ch;
      MEMCPY(tmp + pos + 1, m_buf + pos, m_len++ - pos + 1);
      delete[] m_buf;
      m_buf = tmp;
    } else {
      MEMMOVE(m_buf + pos + 1, m_buf + pos, m_len++ - pos + 1); // remember '\0'
      m_buf[pos] = ch;
    }
  }
  return *this;
}

String &String::insert(size_t pos, const String &s) {
  if((pos <= m_len) && (s.m_len > 0)) {
    if(m_capacity < m_len + s.m_len + 1) {
      TCHAR *tmp = new TCHAR[m_capacity = (m_len + s.m_len + 1) * 2];
      if(pos) {
        MEMCPY(tmp, m_buf, pos);
      }
      MEMCPY(tmp + pos, s.m_buf, s.m_len);
      MEMCPY(tmp + pos + s.m_len, m_buf + pos, m_len - pos + 1);
      m_len += s.m_len;
      delete[] m_buf;
      m_buf = tmp;
    } else {
      MEMMOVE(m_buf + pos + s.m_len, m_buf + pos, m_len - pos + 1); // remember '\0'
      MEMCPY(m_buf + pos, s.m_buf, s.m_len);
      m_len += s.m_len;
    }
  }
  return *this;
}

String &String::remove(size_t pos, size_t count) {
  if((pos < m_len) && (count > 0)) {
    size_t j = pos + count;
    if(j > m_len) {
      j = m_len;
      count = j - pos;
    }
    if(j < m_len) {
      MEMMOVE(m_buf + pos, m_buf + j, m_len - j);
    }
    m_len -= count;
    m_buf[m_len] = 0;
  }
  return *this;
}

String &String::removeLast() {
  if(m_len > 0) {
    remove(m_len-1,1);
  }
  return *this;
}

String &String::replace(TCHAR from, TCHAR to) {
  const TCHAR *last = m_buf + m_len - 1;
  for(TCHAR *s = m_buf; s <= last; s++) {
    if(*s == from) {
      *s = to;
    }
  }
  return *this;
}

String &String::replace(TCHAR from, const TCHAR *to) {
  return replace(from, String(to));
}

String &String::replace(const TCHAR *from, TCHAR to) {
  return replace(String(from), to);
}

String &String::replace(const TCHAR *from, const TCHAR *to) {
  return replace(String(from), String(to));
}

String &String::replace(const String &from, TCHAR to) {
  const size_t fromLength = from.length();
  if(fromLength == 0) {
    return *this;
  }
  if(fromLength == 1) {
    return replace(from[0], to);
  }
  // fromLength > 1 => string will bnot grow
  const TCHAR *last        = m_buf + m_len - fromLength;
  const size_t newCapacity = m_len + 1;
  TCHAR       *newBuf      = new TCHAR[newCapacity];
  TCHAR       *s, *d;
  for(s = m_buf, d = newBuf; s <= last;) {
    if(MEMCMP(s,from.m_buf, fromLength)) {
      *(d++) = *(s++);
    } else { // replace
      *(d++) = to;
      s += fromLength;
    }
  }
  const intptr_t rest = m_len - (s - m_buf);
  if(rest > 0) {
    MEMCPY(d, s, rest);
    d += rest;
  }

  *d = 0;
  delete[] m_buf;
  m_buf      = newBuf;
  m_len      = d - newBuf;
  m_capacity = newCapacity;
  return *this;
}

String &String::replace(TCHAR from, const String &to) {
  const size_t toLength = to.length();
  if(toLength == 1) {
    return replace(from, to[0]);
  }
  if(toLength == 0) { // String will be smaller
    const TCHAR *last = m_buf + m_len - 1;
    TCHAR *s,*d;
    for(d = s = m_buf; s <= last; s++) {
      if(*s != from) {
        *(d++) = *s;
      }
    }
    *d = 0;
    m_len = d - m_buf;
    return *this;
  }

  const TCHAR *last = m_buf + m_len - 1;
  int count = 0;
  for(const TCHAR *t = m_buf; t <= last; t++) { // count all occurrences of from in this. be aware of 0-characters
    if(*t == from) {
      count++;
    }
  }
  if(count == 0) {
    return *this; // nothing to do
  }
  const size_t newLength   = m_len + count * (toLength-1);
  const size_t newCapacity = (newLength + 1) * 2;
  TCHAR       *newBuf      = new TCHAR[newCapacity];
  TCHAR       *dst         = newBuf;

  // dont worry about overlap. copying to a new string
  for(const TCHAR *s = m_buf; s <= last;) {
    if(*s != from) {
      *(dst++) = *(s++);
    } else {
      MEMCPY(dst, to.m_buf, toLength);
      dst += toLength;
      s++;
    }
  }
  *dst = 0;
  delete[] m_buf;
  m_buf      = newBuf;
  m_len      = newLength;
  m_capacity = newCapacity;
  return *this;
}

String &String::replace(const String &from, const String &to) {
  const intptr_t fromLength = from.length();
  const intptr_t toLength   = to.length();
  if(fromLength == 0) {
    return *this;
  }
  if(fromLength == 1) {
    return replace(from[0], to);
  }
  if(toLength == 1) {
    return replace(from, to[0]);
  }

  int count = 0;
  const TCHAR *last = m_buf + m_len - fromLength;
  for(const TCHAR *t = m_buf; t <= last;) { // count all occurrences of from in this
    if(MEMCMP(t, from.m_buf, fromLength) == 0) {
      count++;
      t += fromLength;
    } else {
      t++;
    }
  }

  if(count == 0) {
    return *this;  // nothing to do
  }

  m_len += count * (toLength - fromLength);
  m_capacity = m_len + 1;
  TCHAR *newBuf = new TCHAR[m_capacity];
  TCHAR *s, *d;
  for(s = m_buf, d = newBuf; s <= last;) {
    if(MEMCMP(s, from.m_buf, fromLength)) {
      *(d++) = *(s++);
    } else { // replace
      if(toLength) {
        MEMCPY(d, to.m_buf, toLength);
        d += toLength;
      }
      s += fromLength;
    }
  }
  const intptr_t tail = m_len - (d - newBuf);
  if(tail > 0) {
    MEMCPY(d, s, tail);
    d += tail;
  }
  *d = 0;
  delete[] m_buf;
  m_buf = newBuf;
  return *this;
}

String rev(const String &str) {
  String result(str);
  for(TCHAR *left = result.m_buf, *right = left + result.m_len-1; left < right;) {
    TCHAR tmp  = *left;
    *(left++)  = *right;
    *(right--) = tmp;
  }
  return result;
}

intptr_t String::rfind(TCHAR ch) const {
  TCHAR *s = _tcsrchr(m_buf, ch);
  return s ? s - m_buf : -1;
}

String spaceString(std::streamsize length, TCHAR ch) {
  if(length <= 0) {
    return EMPTYSTRING;
  }
  String result;
  delete[] result.m_buf;
  result.m_buf = new TCHAR[result.m_capacity = (result.m_len = (size_t)length) + 1];
  MEMSET(result.m_buf, ch, (size_t)length);
  result.m_buf[length] = 0;
  return result;
}

String left(const String &str, intptr_t length) {
  if(length >= (intptr_t)str.m_len) {
    return str;
  } else if(length <= 0) {
    return EMPTYSTRING;
  } else {
    String result(str);
    result.m_buf[result.m_len = length] = 0;
    return result;
  }
}

String right(const String &str, intptr_t length) {
  if(length >= (intptr_t)str.m_len) {
    return str;
  } else if(length <= 0) {
    return EMPTYSTRING;
  } else {
    return String(str.m_buf + (intptr_t)str.m_len - length);
  }
}

String substr(const String &str, intptr_t from, intptr_t length) {
  if((from < 0) || (from >= (intptr_t)str.m_len) || (length <= 0)) {
    return EMPTYSTRING;
  } else {
    if(length > (intptr_t)str.m_len - from) {
      length = str.m_len - from;
    }
    String result;
    delete[] result.m_buf;
    result.m_buf = String::newCharBuffer(str.m_buf + from, result.m_len, result.m_capacity);
    result.m_buf[result.m_len=length] = 0;
    return result;
  }
}

String trim(const String &str) {
  String result(str);
  return result.trim();
}

String trimLeft(const String &str) {
  String result(str);
  return result.trimLeft();
}

String trimRight(const String &str) {
  String result(str);
  return result.trimRight();
}

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

void String::indexError(size_t index) const {
  throwInvalidArgumentException(__TFUNCTION__
                               , _T("Index %s out of range in string <%s>. length=%s")
                               , format1000(index).cstr()
                               , m_buf
                               , format1000(m_len).cstr());
}

TCHAR &String::operator[](size_t index) {
  if(index >= m_len) indexError(index);
  return m_buf[index];
}

const TCHAR &String::operator[](size_t index) const {
  if(index >= m_len) indexError(index);
  return m_buf[index];
}

bool operator==(const String &lhs, const String &rhs) {
  return _tcscmp(lhs.m_buf, rhs.m_buf) == 0;
}

bool String::equalsIgnoreCase(const String &s) const {
  return _tcsicmp(cstr(), s.cstr()) == 0;
}

bool operator==(const String &lhs, const TCHAR *rhs) {
  return _tcscmp(lhs.m_buf, rhs) == 0;
}

bool operator!=(const String &lhs, const String &rhs) {
  return _tcscmp(lhs.m_buf, rhs.m_buf) != 0;
}

bool operator!=(const String &lhs, const TCHAR *rhs) {
  return _tcscmp(lhs.m_buf, rhs) != 0;
}

bool operator>(const String &lhs, const String &rhs)  {
  return _tcscmp(lhs.m_buf, rhs.m_buf) > 0;
}

bool operator>=(const String &lhs, const String &rhs) {
  return _tcscmp(lhs.m_buf, rhs.m_buf) >= 0;
}

bool operator<(const String &lhs, const String &rhs)  {
  return _tcscmp(lhs.m_buf, rhs.m_buf) < 0;
}

bool operator<=(const String &lhs, const String &rhs) {
  return _tcscmp(lhs.m_buf, rhs.m_buf) <= 0;
}

String vformat(const TCHAR *format, va_list argptr) {
  String result;
  result.vprintf(format, argptr);
  return result;
}

String format(const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  String result;
  result.vprintf(format, argptr);
  va_end(argptr);
  return result;
}

String &String::printf(const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  vprintf(format, argptr);
  va_end(argptr);
  return *this;
}

String &String::vprintf(const TCHAR *format, va_list argptr) {
  TCHAR buffer[16384];
  int bufSize = ARRAYSIZE(buffer);
  TCHAR *tmp = buffer;
  for(;;) {
    int written;
    if((written = _vsntprintf(tmp, bufSize, format, argptr)) >= 0) {
      if((written >= (int)m_capacity) || (written + 100 < (int)m_capacity)) {
        delete[] m_buf;
        m_buf = new TCHAR[m_capacity = written + 1];
      }
      MEMCPY(m_buf, tmp, (m_len = written)+1);
      if(tmp != buffer) {
        delete[] tmp;
      }
      break;
    }
    if(tmp != buffer) {
      delete[] tmp;
    }
    bufSize *= 3;
    tmp = new TCHAR[bufSize];
  }
  return *this;
}

ULONG String::hashCode() const {
  return strHash(cstr());
}
