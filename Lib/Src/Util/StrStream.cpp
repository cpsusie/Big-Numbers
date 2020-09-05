#include "pch.h"
#include <StrStream.h>

namespace OStreamHelper {

  void FloatStringFields::init(TCHAR *str) {
    TCHAR *firstDigit = findFirstDigit(str);
    assert(firstDigit != NULL);
    TCHAR *comma      = _tcschr(str, _T('.'));
    TCHAR *estr       = _tcschr(firstDigit, _T('e'));

    if(estr == NULL) { // chop tmp into it's components <m_ciphers>.<m_decimals>e<m_expo10>
      m_expo10 = 0;
    } else {
      *(estr++) = 0;
      m_expo10 = (intptr_t)std::stoll(estr);
    }

    if(comma == NULL) {
      m_ciphers  = firstDigit;
      m_decimals = EMPTYSTRING;
    } else {
      *(comma++) = 0;
      m_ciphers  = (comma == firstDigit) ? _T("0") : firstDigit;
      m_decimals =  comma;
    }
  }

  intptr_t FloatStringFields::findFirstNonZero(const String &s) { // static
    const TCHAR *cp;
    for(cp = s.cstr(); *cp == '0'; cp++);
    return *cp ? (cp - s.cstr()) : -1;
  }

  size_t FloatStringFields::countLeadingZeroes(const String &s) { // static
    const intptr_t nz = findFirstNonZero(s);
    return (nz < 0) ? s.length() : nz;
  }

  size_t FloatStringFields::countTrailingZeroes(const String &s) { // static
    if(s.length() > 0) {
      const TCHAR *first = s.cstr(), *last = first + s.length() - 1, *cp = last;
      for(;(cp >= first) && (*cp == '0'); cp--);
      return last - cp;
    }
    return 0;
  }

  // remove leading zeroes from s if any.
  // return the number of zeroes removed
  size_t FloatStringFields::trimLeadingZeroes(String &s) { // static
    const size_t l0 = s.length();
    if(l0 > 0) {
      const TCHAR *cp;
      for(cp = s.cstr(); *cp == '0'; cp++);
      if(cp > s.cstr()) {
        s = cp;
        return l0 - s.length();
      }
    }
    return 0;
  }

  String &FloatStringFields::trimTrailingZeroes(String &s) { // static
    if(s.length() > 0) {
      const TCHAR *first = s.cstr(), *last = first + s.length() - 1, *cp = last;
      for(; (cp >= first) && (*cp == '0'); cp--);
      if(cp < last) {
        s = (cp < first) ? EMPTYSTRING : substr(s, 0, cp - first + 1);
      }
    }
    return s;
  }

  void FloatStringFields::normalize() {
    trimLeadingZeroes(m_ciphers);
    if(m_ciphers.length() > 1) {
      const String toMove = substr(m_ciphers, 1, m_ciphers.length());
      m_decimals.insert(0, toMove);
      m_ciphers = substr(m_ciphers, 0, 1);
      m_expo10 += toMove.length();
    } else if(m_ciphers.length() < 1) {
      const intptr_t dp = findFirstNonZero(m_decimals);
      if(dp < 0) {
        m_ciphers = "0"; // actually an error
      } else {
        m_ciphers += m_decimals[dp];
        m_decimals.remove(0, dp + 1);
        m_expo10 -= dp + 1;
      }
    }
  }

  bool round5DigitString(String &str, intptr_t wantedLength) {
    bool         carry = false;
    const size_t wl    = (wantedLength < 0) ? 0 : wantedLength;
    if(wl < str.length()) {
      if(str[wl] >= '5') {
        if(wl == 0) {
          carry = true;
        } else {
          TCHAR *first = str.cstr(), *dp = first + wl - 1;
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
      str.remove(wl, str.length() - wl);
    }
    return carry;
  }

}; // namespace OStreamHelper
