#include "pch.h"

String &String::replace(TCHAR from, TCHAR to) {
  const TCHAR *last = m_buf + m_len - 1;
  for (TCHAR *s = m_buf; s <= last; s++) {
    if (*s == from) {
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
  if (fromLength == 0) {
    return *this;
  }
  if (fromLength == 1) {
    return replace(from[0], to);
  }
  // fromLength > 1 => string will bnot grow
  const TCHAR *last = m_buf + m_len - fromLength;
  const size_t newCapacity = m_len + 1;
  TCHAR       *newBuf = new TCHAR[newCapacity];
  TCHAR       *s, *d;
  for (s = m_buf, d = newBuf; s <= last;) {
    if (TMEMCMP(s, from.m_buf, fromLength)) {
      *(d++) = *(s++);
    }
    else { // replace
      *(d++) = to;
      s += fromLength;
    }
  }
  const intptr_t rest = m_len - (s - m_buf);
  if (rest > 0) {
    TMEMCPY(d, s, rest);
    d += rest;
  }

  *d = 0;
  delete[] m_buf;
  m_buf = newBuf;
  m_len = d - newBuf;
  m_capacity = newCapacity;
  return *this;
}

String &String::replace(TCHAR from, const String &to) {
  const size_t toLength = to.length();
  if (toLength == 1) {
    return replace(from, to[0]);
  }
  if (toLength == 0) { // String will be smaller
    const TCHAR *last = m_buf + m_len - 1;
    TCHAR *s, *d;
    for (d = s = m_buf; s <= last; s++) {
      if (*s != from) {
        *(d++) = *s;
      }
    }
    *d = 0;
    m_len = d - m_buf;
    return *this;
  }

  const TCHAR *last = m_buf + m_len - 1;
  int count = 0;
  for (const TCHAR *t = m_buf; t <= last; t++) { // count all occurrences of from in this. be aware of 0-characters
    if (*t == from) {
      count++;
    }
  }
  if (count == 0) {
    return *this; // nothing to do
  }
  const size_t newLength = m_len + count * (toLength - 1);
  const size_t newCapacity = (newLength + 1) * 2;
  TCHAR       *newBuf = new TCHAR[newCapacity];
  TCHAR       *dst = newBuf;

  // dont worry about overlap. copying to a new string
  for (const TCHAR *s = m_buf; s <= last;) {
    if (*s != from) {
      *(dst++) = *(s++);
    }
    else {
      TMEMCPY(dst, to.m_buf, toLength);
      dst += toLength;
      s++;
    }
  }
  *dst = 0;
  delete[] m_buf;
  m_buf = newBuf;
  m_len = newLength;
  m_capacity = newCapacity;
  return *this;
}

String &String::replace(const String &from, const String &to) {
  const intptr_t fromLength = from.length();
  const intptr_t toLength = to.length();
  if (fromLength == 0) {
    return *this;
  }
  if (fromLength == 1) {
    return replace(from[0], to);
  }
  if (toLength == 1) {
    return replace(from, to[0]);
  }

  int count = 0;
  const TCHAR *last = m_buf + m_len - fromLength;
  for (const TCHAR *t = m_buf; t <= last;) { // count all occurrences of from in this
    if (TMEMCMP(t, from.m_buf, fromLength) == 0) {
      count++;
      t += fromLength;
    }
    else {
      t++;
    }
  }

  if (count == 0) {
    return *this;  // nothing to do
  }

  m_len += count * (toLength - fromLength);
  m_capacity = m_len + 1;
  TCHAR *newBuf = new TCHAR[m_capacity];
  TCHAR *s, *d;
  for (s = m_buf, d = newBuf; s <= last;) {
    if (TMEMCMP(s, from.m_buf, fromLength)) {
      *(d++) = *(s++);
    }
    else { // replace
      if (toLength) {
        TMEMCPY(d, to.m_buf, toLength);
        d += toLength;
      }
      s += fromLength;
    }
  }
  const intptr_t tail = m_len - (d - newBuf);
  if (tail > 0) {
    TMEMCPY(d, s, tail);
    d += tail;
  }
  *d = 0;
  delete[] m_buf;
  m_buf = newBuf;
  return *this;
}
