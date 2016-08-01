#include "stdafx.h"

AddressToString::AddressToString() {
  init(0, 10, false, false);
}

AddressToString::AddressToString(__int64 maxValue, const Settings &settings) {
  init(maxValue, settings.getAddrRadix(), settings.getAddrHexUppercase(), settings.getAddrSeparators());
}

void AddressToString::update(__int64 maxValue, const Settings &settings) {
  if(m_maxValue   == maxValue 
  && m_radix      == settings.getAddrRadix() 
  && m_uppercase  == settings.getAddrHexUppercase()
  && m_separators == settings.getAddrSeparators()) {
    return;
  }
  init(maxValue, settings.getAddrRadix(), settings.getAddrHexUppercase(), settings.getAddrSeparators());
}

void AddressToString::init(__int64 maxValue, int radix, bool uppercase, bool separators) {
  m_maxValue   = maxValue;
  m_radix      = radix;
  m_uppercase  = uppercase;
  m_separators = separators;

  unsigned __int64 mv;
  for(m_addrLength = 1, mv = m_radix-1; mv < (unsigned __int64)m_maxValue; m_addrLength++) {
    mv = mv * m_radix + (m_radix-1);
  }
  TCHAR fieldType;
  switch(m_radix) {
  case  8: fieldType = _T('o'); break;
  case 10: fieldType = _T('u'); break;
  case 16: fieldType = m_uppercase ? _T('X') : _T('x'); break;
  default: throwException(_T("init:Invalid radix:%d"), m_radix);
  }

  m_offsetFormatString = format(_T("%%-%c"), fieldType);
  m_offsetFormat       = m_offsetFormatString.cstr();
  m_addrFormatString   = format(_T("%%0%dI64%c"), m_addrLength, fieldType);
  m_addrFormat         = m_addrFormatString.cstr();

  m_sampleTextString   = addrToString(maxValue) + _T(" ");
}

String AddressToString::offsetToString(int offset) const {
  return format(m_offsetFormat, offset);
}

#define INSERTSEPARATORS(result, temp, sep, grpsize)      \
{ if(m_addrLength <= grpsize) {                           \
    return temp;                                          \
  }                                                       \
                                                          \
  TCHAR *src = temp;                                      \
  TCHAR *dst = result;                                    \
  for(int d = m_addrLength; *(dst++) = *(src++);) {       \
    if((d-- % (grpsize) == 1) && d) {                     \
      *(dst++) = sep;                                     \
    }                                                     \
  }                                                       \
}

String AddressToString::addrToString(__int64 addr) const {
  TCHAR tmp[100];
  TCHAR result[100];
  _stprintf(tmp, m_addrFormat, addr);

  if(m_separators) {
    switch(m_radix) {
    case 8 :
      INSERTSEPARATORS(result, tmp, ' ', 3);
      return result;

    case 10:
      INSERTSEPARATORS(result, tmp, '.', 3);
      return result;

    case 16:
      INSERTSEPARATORS(result, tmp, ' ', 4);
      return result;
    default:
      throwException(_T("addrToString:Invalid radix:%d"), m_radix);
      return tmp;
    }
  } else {
    return tmp;
  }
}
