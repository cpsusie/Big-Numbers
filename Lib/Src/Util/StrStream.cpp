#include "pch.h"
#include <StrStream.h>

namespace OStreamHelper {

  void FloatStringFields::init(TCHAR *str) {
    TCHAR *firstDigit = findFirstDigit(str);
    TCHAR *estr       = _tcschr(firstDigit, _T('e'));
    TCHAR *comma      = _tcschr(firstDigit, _T('.'));

    if(estr == NULL) { // chop tmp into it's components <m_ciphers>.<m_decimals>e<m_expo10>
      m_expo10 = 0;
    } else {
      *(estr++) = 0;
      m_expo10 = _ttoi(estr);
    }

    if(comma == NULL) {
      m_decimals = EMPTYSTRING;
    } else {
      *(comma++) = 0;
      m_decimals = comma;
    }
    m_ciphers = firstDigit;
  }

  bool round5DigitString(String &str, UINT wantedLength) {
    bool carry = false;
    if(wantedLength < str.length()) {
      if(str[wantedLength] >= '5') {
        if(wantedLength == 0) {
          carry = true;
        } else {
          TCHAR *first = str.cstr(), *dp = first + wantedLength - 1;
          for(; dp >= first; dp--) {
            if(*dp < _T('9')) {
              (*dp)++;
              break;
            } else {
              *dp = '0';
            }
          }
          carry = (dp < first);
        }
      }
      str.remove(wantedLength, str.length() - wantedLength);
    }
    return carry;
  }

}; // namespace OStreamHelper
