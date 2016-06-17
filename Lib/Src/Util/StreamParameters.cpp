#include "pch.h"
#include <StreamParameters.h>

StreamParameters::StreamParameters(streamsize precision, streamsize width, int flags) {
  m_precision = precision;
  m_width     = width;
  m_flags     = flags;
}

StreamParameters::StreamParameters(const tostream &stream) {
  m_precision = stream.precision();
  m_width     = stream.width();
  m_flags     = stream.flags();
}

tostream &operator<<(tostream &out, const StreamParameters &p) {
  out.precision(p.m_precision);
  out.width(p.m_width);
  out.flags(p.m_flags);
  return out;
}

TCHAR *StreamParameters::addModifier(TCHAR *dst) const {
  if(m_flags & ios::left) {
    *(dst++) = _T('-');
  }
  if(m_flags & ios::showpos) {
    *(dst++) = _T('+');
  }
  if(m_flags & ios::showbase) {
    *(dst++) = _T('#');
  }
  return dst;
}

TCHAR *StreamParameters::addWidth(TCHAR *dst) const {
  if(m_width != 0) {
    _i64tot(m_width,dst,10);
    dst += _tcsclen(dst);
  }
  return dst;
}

TCHAR *StreamParameters::addPrecision(TCHAR *dst) const {
  if(m_precision != 0) {
    *(dst++) = _T('.');
    _i64tot(m_precision,dst,10);
    dst += _tcsclen(dst);
  }
  return dst;
}

TCHAR *StreamParameters::addPrefix(TCHAR *dst, bool withPrecision) const {
  *(dst++) = _T('%');
  dst = addModifier(dst);
  dst = addWidth(dst);
  if(withPrecision) {
    dst = addPrecision(dst);
  }
  return dst;
}

TCHAR *StreamParameters::addIntSpecifier(TCHAR *dst, bool isSigned) const {
  if(m_flags & ios::dec) {
    *(dst++) = isSigned ? _T('d') : _T('u');
  } else if(m_flags & ios::oct) {
    *(dst++) = _T('o');
  } else if(m_flags & ios::hex) {
    *(dst++) = (m_flags & ios::uppercase) ? _T('X') : _T('x');
  } else {
    *(dst++) = isSigned ? _T('d') : _T('u');
  }
  return dst;
}

TCHAR *StreamParameters::addFloatSpecifier(TCHAR *dst) const {
  if(m_flags & ios::scientific) {
    *(dst++) = (m_flags & ios::uppercase) ? _T('E') : _T('e');
  } else if(m_flags & ios::fixed) {
    *(dst++) = _T('f');
  } else {
    *(dst++) = (m_flags & ios::uppercase) ? _T('G') : _T('g');
  }
  return dst;
}

String StreamParameters::getStringFormat() const {
  TCHAR tmp[30],*cp;
  cp = addPrefix(tmp,true);
  *(cp++) = _T('s');
  *cp = 0;
  return tmp;
}

String StreamParameters::getCharFormat() const {
  TCHAR tmp[30],*cp;
  cp = addPrefix(tmp,true);
  *(cp++) = _T('c');
  *cp = 0;
  return tmp;
}

String StreamParameters::getUCharFormat() const {
  TCHAR tmp[30],*cp;
  cp = addPrefix(tmp,true);
  *(cp++) = _T('c');
  *cp = 0;
  return tmp;
}

String StreamParameters::getShortFormat() const {
  TCHAR tmp[30],*cp;
  cp = addPrefix(tmp,false);
  *(cp++) = _T('h');
  cp = addIntSpecifier(cp,true);
  *cp = 0;
  return tmp;
}

String StreamParameters::getUShortFormat() const {
  TCHAR tmp[30],*cp;
  cp = addPrefix(tmp,false);
  *(cp++) = _T('h');
  cp = addIntSpecifier(cp,false);
  *cp = 0;
  return tmp;
}

String StreamParameters::getIntFormat() const {
  TCHAR tmp[30],*cp;
  cp = addPrefix(tmp,false);
  cp = addIntSpecifier(cp,true);
  *cp = 0;
  return tmp;
}

String StreamParameters::getUIntFormat() const {
  TCHAR tmp[30],*cp;
  cp = addPrefix(tmp,false);
  cp = addIntSpecifier(cp,false);
  *cp = 0;
  return tmp;
}

String StreamParameters::getLongFormat() const {
  TCHAR tmp[30],*cp;
  cp = addPrefix(tmp,false);
  *(cp++) = _T('l');
  cp = addIntSpecifier(cp,true);
  *cp = 0;
  return tmp;
}

String StreamParameters::getULongFormat() const {
  TCHAR tmp[30],*cp;
  cp = addPrefix(tmp,false);
  *(cp++) = _T('l');
  cp = addIntSpecifier(cp,false);
  *cp = 0;
  return tmp;
}

String StreamParameters::getInt64Format() const {
  TCHAR tmp[30],*cp;
  cp = addPrefix(tmp,false);
  _tcscpy(cp,_T("I64"));
  cp += 3;
  cp = addIntSpecifier(cp,true);
  *cp = 0;
  return tmp;
}

String StreamParameters::getUInt64Format() const {
  TCHAR tmp[30],*cp;
  cp = addPrefix(tmp,false);
  _tcscpy(cp,_T("I64"));
  cp += 3;
  cp = addIntSpecifier(cp,false);
  *cp = 0;
  return tmp;
}

String StreamParameters::getFloatFormat() const {
  TCHAR tmp[30],*cp;
  cp = addPrefix(tmp,true);
  cp = addFloatSpecifier(cp);
  *cp = 0;
  return tmp;
}

String StreamParameters::getDoubleFormat() const {
  TCHAR tmp[30],*cp;
  cp = addPrefix(tmp,true);
  *(cp++) = _T('l');
  cp = addFloatSpecifier(cp);
  *cp = 0;
  return tmp;
}
