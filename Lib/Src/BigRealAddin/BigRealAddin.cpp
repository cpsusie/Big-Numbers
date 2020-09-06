#include "pch.h"
#include <Math/BigReal/BigReal.h>

using namespace std;

template<typename VTYPE> class DigitType {
public:
  VTYPE n;
  VTYPE next;
  VTYPE prev;
  inline bool hasNext() const {
    return next != 0;
  }
  inline bool hasPrev() const {
    return prev != 0;
  }
  inline void clear() {
    memset(0, 0, sizeof(*this));
  }
};

static const UINT64 debugPattern = 0xccccccccccccccccui64;

template<typename ETYPE, typename VTYPE> class BigRealType {
public:
  VTYPE         m_vfptr;               // pointer to vtable
  VTYPE         m_first;               // Most significand  digit
  VTYPE         m_last;                // Least significand digit
  ETYPE         m_expo, m_low;         // if isZero() then (m_expo,m_low)=(BIGREAL_ZEROEXPO,0) else m_expo = m_low+getLength()-1
  VTYPE         m_digitPool;
  BYTE          m_flags;
  inline bool isNegative() const {
    return m_flags & BR_NEG;
  }
  inline void checkInitializing() const {
    if(memcmp(&m_first, &debugPattern, 6) == 0) {
      throw 1;
    }
  }
};

typedef BigRealType<BRExpoTypex86, BRDigitTypex86> BigRealx86;
typedef BigRealType<BRExpoTypex64, BRDigitTypex64> BigRealx64;

// -------------------------------------------------------------------

template<typename INTTYPE> char *digitToStr(char *dst, INTTYPE n, UINT width) {
  char tmp[50], *d = width ? tmp : dst;
  if(sizeof(n) == sizeof(QWORD)) {
    _i64toa(n, d, 10);
  } else {
    _itoa((DWORD)n, d, 10);
  }
  if(width) {
    const int zeroCount = (int)width - (int)strlen(tmp);
    if(zeroCount <= 0) {
      strcpy(dst, d);
    } else { // zeroCount > 0
      memset(dst, '0', zeroCount);
      strcpy(dst + zeroCount, d);
    }
  }
  return dst;
}

template<typename BRType, typename ETYPE, typename VTYPE, int log10Base, ETYPE nonNormalExpo> class BigRealAddIn {
private:
  DEBUGHELPER *m_helper;
  bool         m_hasDecimalPoint;
  bool         m_showFlags;
  string      &m_result;

  void getDigit(DigitType<VTYPE> &d, VTYPE addr) const {
    if(sizeof(VTYPE) == sizeof(DWORD)) {
      m_helper->getObjectx86(&d, (DWORD)addr, sizeof(DigitType<VTYPE>));
    } else {
      m_helper->getObjectx64(&d, addr, sizeof(DigitType<VTYPE>));
    }
  }
  inline bool nextDigit(DigitType<VTYPE> &d) const {
    if(!d.hasNext()) return false;
    getDigit(d, d.next);
    return true;
  }
  inline bool prevDigit(DigitType<VTYPE> &d) const {
    if(!d.hasPrev()) return false;
    getDigit(d, d.prev);
    return true;
  }
  inline void addstr(const char *s) {
    m_result += s;
  }
  inline void addstr(const string &s) {
    addstr(s.c_str());
  }
  // return digits added
  inline int addDigitStr(VTYPE n, UINT width = 0) {
    char tmp[100];
    addstr(digitToStr(tmp, n, width));
    return (int)strlen(tmp);
  }
  inline void addDecimalPoint() {
    if(!m_hasDecimalPoint) {
      addstr(".");
      m_hasDecimalPoint = true;
    }
  }

#define STACKEDDIGITS()  (digitStack.getHeight() * log10Base)
  // return string containing as much as possible up to maxlen digits from the integer. n.m_low >= 0
  string getTailString(const BRType &n, size_t maxlen) const {
    CompactStack<VTYPE> digitStack;
    for(int i = 0; (STACKEDDIGITS() < maxlen) && (i < n.m_low); i++) {
      digitStack.push(0);
    }
    if((STACKEDDIGITS() < maxlen)) {
      DigitType<VTYPE> digit;
      getDigit(digit, n.m_last);
      for(digitStack.push(digit.n); (STACKEDDIGITS() < maxlen) && prevDigit(digit);) {
        digitStack.push(digit.n);
      }
    }
    string result;
    while(!digitStack.isEmpty()) {
      char str[40];
      result += digitToStr(str, digitStack.pop(), log10Base);
    }
    if(result.length() > maxlen) {
      result.erase(0, result.length() - maxlen); // delete first characters...not last...we want the tail-digits
    }
    return result;
  }

  static string zeroString(UINT count) {
    string result;
    if(count > 0) result.insert(0, count, '0');
    return result;
  }

  static string infoString(ETYPE expo10) {
    char str[100];
    sprintf(str, "..Total:%I64d digits..", (INT64)expo10);
    return str;
  }

  inline void addZeroes(ETYPE count) {
    if(count > 0) {
      m_result.insert(m_result.length(), (size_t)count, '0');
    }
  }
  inline void removeTrailingZeroes() {
    if(m_hasDecimalPoint) {
      const char *first = m_result.c_str(), *last = first + m_result.length() - 1, *cp = last;
      while((cp > first) && (*cp == '0')) cp--;
      if(*cp == '.') cp--;
      if(cp < last) {
        removeLast(last - cp);
      }
    }
  }
  inline void removeLast(size_t n) {
    if((0 < n) && (n < m_result.length())) {
      m_result.erase(m_result.length() - n, n);
    }
  }

  void setEllipsisAtEnd() {
    if(m_result.length() >= 3) {
      strcpy(&m_result.at(m_result.length() - 3), "...");
    }
  }
  VTYPE pow10(UINT n) const {
    if(sizeof(VTYPE) == sizeof(BRDigitTypex86)) {
      return (VTYPE)BigReal::pow10x86(n);
    } else {
      return (VTYPE)BigReal::pow10x64(n);
    }
  }
  string makeExpoString(INT64 expo10) const {
    char tmp[100];
    sprintf(tmp, "e%+03I64d", expo10);
    return tmp;
  }
  void formatNonNormal(const BRType &n, bool showDecimals) {
    char tmp[100];
    switch(n.m_low) {
    case BIGREAL_ZEROLOW:
      addstr(showDecimals ? "0.0000000000000000000" : "0");
      break;
    case BIGREAL_INFLOW:
      addstr(n.isNegative() ? "-inf" : "inf");
      break;
    case BIGREAL_QNANLOW:
      addstr("nan(ind)");
      break;
    default:
      sprintf(tmp, "Invalid state:expo:%I64d, low:%I64d", (INT64)n.m_expo, (INT64)n.m_low);
      addstr(tmp);
      break;
    }
  }

  string makeFlagsString(BYTE flags) const {
    const String s = BigReal::flagsToString(flags);
    char tmp[200], result[200];
    strCpy(tmp, s.cstr());
    sprintf(result, "{%s}:", tmp);
    return result;
  }
public:
  BigRealAddIn(DEBUGHELPER *pHelper, string &dst, bool showFlags)
    : m_helper(pHelper)
    , m_result(dst)
    , m_showFlags(showFlags)
    , m_hasDecimalPoint(false) {
  }
  void toRealString(BRType &n, size_t maxResult) {
    const ETYPE expo = n.m_expo;
    if(m_showFlags) {
      addstr(makeFlagsString(n.m_flags));
    }
    if((expo == nonNormalExpo) || (n.m_first == 0)) {
      formatNonNormal(n, true);
      return;
    }

    DigitType<VTYPE> digit;
    getDigit(digit, n.m_first);

    const int    firstDigitCount = BigReal::getDecimalDigitCount(digit.n);
    const int    firstExpo10     = firstDigitCount - 1;

    int totalDecimalDigitCount;
    if(expo == n.m_low) { // calculate total number of decimal digits in BigReal
      totalDecimalDigitCount = firstDigitCount;
    } else {
      DigitType<VTYPE> lastDigit;
      if(n.m_last == 0) {
        lastDigit.clear();
      } else {
        getDigit(lastDigit, n.m_last);
      }
      int lastDigitCount;
      if(lastDigit.n == 0) {
        lastDigitCount = 0;
      } else {
        lastDigitCount = log10Base;
        for(VTYPE last = lastDigit.n; last % 10 == 0; last /= 10) {
          lastDigitCount--;
        }
      }
      totalDecimalDigitCount = (int)((expo - n.m_low - 1) * log10Base + firstDigitCount + lastDigitCount);
    }

    if(n.isNegative()) {
      addstr("-");
    }
    const ETYPE expo10 = expo * log10Base + firstExpo10;

    if((expo10 < -4) || (expo10 >= 18)) { // use scientific format
      const string expoStr              = makeExpoString(expo10);
      const int    maxSignificandDigits = maxResult - m_result.length() - expoStr.length() - 1;   // sign, exponent, decimalpoint
      const int    precision            = min(maxSignificandDigits, totalDecimalDigitCount) - 1;  // Number of decimal digits after decimalpoint
      const VTYPE  firstScaleE10        = pow10(firstExpo10);

      addDigitStr(digit.n / firstScaleE10);
      if(precision > 0) {
        addDecimalPoint();

        digit.n %= firstScaleE10;
        int decimalsDone;

        if(precision < firstExpo10) {
          digit.n /= pow10(firstExpo10 - precision);
          decimalsDone = precision;   // 0 < precision < firstExpo10 < log10Base
        } else {
          decimalsDone = firstExpo10; // firstExpo10 < log10Base
        }
        if(decimalsDone > 0) {
          addDigitStr(digit.n, decimalsDone);
        }
        while((m_result.length() < maxResult) && nextDigit(digit)) {
          addDigitStr(digit.n, log10Base);
        }
      }
      const intptr_t expectedSize = m_result.length() + expoStr.length();
      const intptr_t extraCount   = expectedSize - (intptr_t)maxResult;

      if(digit.hasNext() || (extraCount > 0)) {
        removeLast(extraCount);
        setEllipsisAtEnd();
      } else {
        removeTrailingZeroes();
      }
      addstr(expoStr);
    } else { // fixed format
      int precision, decimalsDone = 0;
      if(expo10 < 0) { // first handle integerpart
        addstr("0");
        addDecimalPoint();
        precision = (int)maxResult - (int)m_result.length();
        if(expo10 < -1) {
          const ETYPE zeroCount = -expo10 - 1;
          addZeroes(zeroCount);
          decimalsDone += (int)zeroCount;
        }
        addDigitStr(digit.n);
        decimalsDone += firstDigitCount;
      } else {
        addDigitStr(digit.n);
        ETYPE d = expo10 - firstExpo10;
        for(; (d > 0) && nextDigit(digit); d -= log10Base) {
          addDigitStr(digit.n, log10Base);
        }
        if(d > 0) {
          addZeroes(d);
        } else {
          addDecimalPoint();
        }
        precision = (int)maxResult - (int)m_result.length();
      }

      // now handle fraction if any
      for(int rest = precision - decimalsDone; (rest > 0) && nextDigit(digit); rest -= log10Base) {
        addDigitStr(digit.n, log10Base);
      }
      removeTrailingZeroes();

      const intptr_t extraCount = m_result.length() - (intptr_t)maxResult;
      if(digit.hasNext() || (extraCount > 0)) {
        removeLast(extraCount);
        setEllipsisAtEnd();
      }
    }
  }

  void toIntString(BRType &n, size_t maxResult) {
    const ETYPE expo = n.m_expo;
    if(m_showFlags) {
      addstr(makeFlagsString(n.m_flags));
    }
    if(expo == nonNormalExpo) {
      formatNonNormal(n, false);
      return;
    }

    DigitType<VTYPE> digit;
    getDigit(digit, n.m_first);

    const int    firstDigitCount = BigReal::getDecimalDigitCount(digit.n);
    const int    firstExpo10     = firstDigitCount - 1;

    int totalDecimalDigitCount = (int)((expo - n.m_low) * log10Base + firstDigitCount);

    if(n.isNegative()) {
      addstr("-");
    }
    const ETYPE expo10      = expo * log10Base + firstExpo10;
          ETYPE digitsToAdd = expo10;

    digitsToAdd -= addDigitStr(digit.n);
    for(;(m_result.length() < maxResult) && nextDigit(digit);) {
      digitsToAdd -= addDigitStr(digit.n, log10Base);
    }
    // digitsToAdd = digits yet to add to get complete string
    if(digitsToAdd > 0) { // still need part of the tail
      intptr_t spaceLeft = (intptr_t)maxResult - (intptr_t)m_result.length();
      if(digitsToAdd <= spaceLeft) { // no need for info-string
        // It must be zeroes, because above loop stopped while spaceLeft >= digitsToAdd > 0 => maxResult > |m_result| => nextDigit returned false
        addZeroes(digitsToAdd);
      } else {
        if(spaceLeft < 0) { // first (if needed) cut resultstring to have length maxResult
          removeLast(-spaceLeft); digitsToAdd -= spaceLeft; // m_result.length() == maxResult, digitsToAdd adjusted = #digits not printed
        }
        const string info = infoString(expo10);
        const string tail = getTailString(n, (int)min(digitsToAdd, 10));
        removeLast(info.length() + tail.length());
        addstr(info);
        addstr(tail);
      }
    }
  }
};

typedef BigRealAddIn<BigRealx86, BRExpoTypex86, BRDigitTypex86, BIGREAL_LOG10BASEx86, BIGREAL_NONNORMALx86> BigRealAddInx86;
typedef BigRealAddIn<BigRealx64, BRExpoTypex64, BRDigitTypex64, BIGREAL_LOG10BASEx64, BIGREAL_NONNORMALx64> BigRealAddInx64;

#define SHOWFLAGS (nBase != 10)
ADDIN_API HRESULT WINAPI AddIn_BigReal(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  try {
    string tmpStr;
    switch(pHelper->getProcessorType()) {
    case PRTYPE_X86:
      { BigRealx86 n;
        pHelper->getRealObject(&n, sizeof(n));
        n.checkInitializing();
        BigRealAddInx86(pHelper, tmpStr, SHOWFLAGS).toRealString(n, maxResult - 1);
      }
      break;
    case PRTYPE_X64:
      { BigRealx64 n;
        pHelper->getRealObject(&n, sizeof(n));
        n.checkInitializing();
        BigRealAddInx64(pHelper, tmpStr, SHOWFLAGS).toRealString(n, maxResult - 1);
      }
      break;
    }
    strncpy(pResult, tmpStr.c_str(), maxResult);
  } catch(...) {
    strncpy(pResult, "", maxResult);
  }
  return S_OK;
}

ADDIN_API HRESULT WINAPI AddIn_BigInt(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  try {
    string tmpStr;
    switch(pHelper->getProcessorType()) {
    case PRTYPE_X86:
      { BigRealx86 n;
        pHelper->getRealObject(&n, sizeof(n));
        n.checkInitializing();
        BigRealAddInx86(pHelper, tmpStr, SHOWFLAGS).toIntString(n, maxResult - 1);
       }
      break;
    case PRTYPE_X64:
      { BigRealx64 n;
        pHelper->getRealObject(&n, sizeof(n));
        n.checkInitializing();
        BigRealAddInx64(pHelper, tmpStr, SHOWFLAGS).toIntString(n, maxResult - 1);
      }
      break;
    }
    strncpy(pResult, tmpStr.c_str(), maxResult);
  } catch(...) {
    strncpy(pResult, "", maxResult);
  }
  return S_OK;
}
