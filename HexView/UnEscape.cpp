#include "stdafx.h"

String Settings::unEscapeByte(BYTE byte) const {
  if(getShowAscii()) {
    if(isprint(byte)) {
      return format(_T("%c"), byte);
    } else {
      switch(byte) {
      case '\\'  : return _T("\\\\");
      case '\b'  : return _T("\\b");
      case '\f'  : return _T("\\f");
      case '\n'  : return _T("\\n");
      case '\r'  : return _T("\\r");
      case '\t'  : return _T("\\t");
      case '\033': return _T("\\e");
      }
    }
  }
  switch(getDataRadix()) {
  case 8 : return format(_T("\\0%03o"), byte);
  case 10: return format(_T("\\%d"), byte);
  case 16: return format(getDataHexUppercase()?_T("\\x%02X"):_T("\\x%02x"), byte);
  }
  return _T("?");
}

String Settings::unEscape(const ByteArray &a) const {
  String result;

  size_t size = a.size();
  for(const BYTE *bp = a.getData(); size--; bp++) {
    result += unEscapeByte(*bp);
  }
  return result;
}
