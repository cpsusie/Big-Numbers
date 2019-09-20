#include "pch.h"
#include <StrStream.h>

using namespace std;

#define TENE0  1
#define TENE1  10
#define TENE2  100
#define TENE3  1000
#define TENE4  10000
#define TENE5  100000
#define TENE6  1000000
#define TENE7  10000000
#define TENE8  100000000
#define TENE9  1000000000
#define TENE10 10000000000
#define TENE11 100000000000
#define TENE12 1000000000000
#define TENE13 10000000000000
#define TENE14 100000000000000
#define TENE15 1000000000000000
#define TENE16 10000000000000000
#define TENE17 100000000000000000
#define TENE18 1000000000000000000
#define TENE19 10000000000000000000

// Assume n = [0..1e19]
static int getDecimalDigitCount(UINT64 n) {
  // Binary search
  if(n < TENE10) {                          // n < 1e10
    if(n < TENE5) {                         // n < 1e5
      if(n < TENE2) {                       // n < 1e2
        if(n < TENE1) {                     //
          return n ? 1 : 0;                 // n < 1e1
        } else {                            // 1e1 <= n < 1e2
          return 2;                         //
        }                                   //
      } else {                              // 1e2 <= n < 1e5
        if(n < TENE4) {                     // 1e2 <= n < 1e4
          return (n < TENE3) ? 3 : 4;       // 1e2 <= n < 1e3
        } else {                            // 1e3 <= n < 1e5
          return 5;                         //
        }                                   //
      }                                     //
    } else {                                // 1e5 <= n < 1e10
      if(n < TENE7) {                       // 1e5 <= n < 1e7
        return (n < TENE6) ? 6 : 7;         //
      } else {                              // 1e7 <= n < 1e10
        if(n < TENE9) {                     // 1e7 <= n < 1e9
          return (n < TENE8) ? 8 : 9;       //
        } else {                            // 1e9 <= n < 1e10
          return 10;                        //
        }                                   //
      }                                     //
    }                                       //
  } else {                                  // 1e10 <= n <= 1e19
    if(n < TENE15) {                        // 1e10 <= n < 1e15
      if(n < TENE12) {                      // 1e10 <= n < 1e12
        return (n < TENE11) ? 11 : 12;      // 1e10 <= n < 1e11
      } else {                              // 1e12 <= n < 1e15
        if(n < TENE14) {                    // 1e12 <= n < 1e13
          return (n < TENE13) ? 13 : 14;    //
        } else {                            // 1e13 <= n < 1e15
          return 15;                        //
        }                                   //
      }                                     //
    } else {                                // 1e15 <= n <= 1e19
      if(n < TENE17) {                      // 1e15 <= n < 1e17
        return (n < TENE16) ? 16 : 17;      //
      } else {                              // 1e17 <= n <= 1e19
        if(n < TENE19) {                    // 1e17 <= n < 1e19
          return (n < TENE18) ? 18 : 19;    //
        } else {                            // 1e18 <= n <= 1e19
          return 20;                        //
        }                                   //
      }                                     //
    }                                       //
  }                                         //
}                                           //

static const UINT64 power10Table[] = {
  TENE0    ,TENE1    ,TENE2    ,TENE3    ,TENE4
 ,TENE5    ,TENE6    ,TENE7    ,TENE8    ,TENE9
 ,TENE10   ,TENE11   ,TENE12   ,TENE13   ,TENE14
 ,TENE15   ,TENE16   ,TENE17   ,TENE18   ,TENE19
};

// Return 10^n. Assume n <= 19
static inline UINT64 pow10(UINT n) {
  return power10Table[n];
}

#define LOG10BASEx86  8
#define ESCEXPOx86   -900000000

#define LOG10BASEx64  18
#define ESCEXPOx64   -900000000000000000

// Values for BigReal::m_low, if m_expo == BIGREAL_ESCEXPO
#define BIGREAL_ZEROLOW    FP_ZERO
#define BIGREAL_NANLOW     FP_NAN
#define BIGREAL_INFLOW     FP_INFINITE // +/- infinite depending on m_negative

template<class VTYPE> class Digit {
public:
  VTYPE n;
  VTYPE next;
  VTYPE prev;
  inline bool hasNext() const {
    return next != 0;
  }
};

template<class ETYPE, class VTYPE> class BigRealType {
public:
  VTYPE         m_vfptr;               // pointer to vtable
  VTYPE         m_first;               // Most significand  digit
  VTYPE         m_last;                // Least significand digit
  ETYPE         m_expo, m_low;         // if isZero() then (m_expo,m_low)=(BIGREAL_ZEROEXPO,0) else m_expo = m_low+getLength()-1
  bool          m_negative;            // True for x < 0. else false
  VTYPE         m_digitPool;
};

typedef BigRealType<int  , DWORD> BigRealx86;
typedef BigRealType<INT64, QWORD> BigRealx64;

// -------------------------------------------------------------------

template<class INTTYPE> TCHAR *digitToStr(TCHAR *dst, INTTYPE n, UINT width) {
  TCHAR tmp[50], *d = width ? tmp : dst;
  if(sizeof(n) == sizeof(QWORD)) {
    _i64tot(n, d, 10);
  } else {
    _itot((DWORD)n, d, 10);
  }
  if(width) {
    const int zeroCount = (int)width - (int)_tcslen(tmp);
    if(zeroCount <= 0) {
      _tcscpy(dst, d);
    } else { // zeroCount > 0
      TMEMSET(dst, _T('0'), zeroCount);
      _tcscpy(dst + zeroCount, d);
    }
  }
  return dst;
}

template<class BigReal, class ETYPE, class VTYPE, int log10Base, ETYPE escExpo> class BigRealAddIn {
private:
  DEBUGHELPER *m_helper;
  bool         m_hasDecimalPoint;
  String      &m_result;

  void getDigit(Digit<VTYPE> &d, VTYPE addr) const {
    if(sizeof(VTYPE) == sizeof(DWORD)) {
      m_helper->getObjectx86(&d, (DWORD)addr, sizeof(Digit<VTYPE>));
    } else {
      m_helper->getObjectx64(&d, addr, sizeof(Digit<VTYPE>));
    }
  }
  inline bool nextDigit(Digit<VTYPE> &d) const {
    if(!d.hasNext()) return false;
    getDigit(d, d.next);
    return true;
  }
  inline void addstr(const TCHAR *s) {
    m_result += s;
  }
  inline void addstr(const String &s) {
    m_result += s;
  }
  inline void addDigitStr(VTYPE n, UINT width = 0) {
    TCHAR tmp[100];
    addstr(digitToStr(tmp, n, width));
  }
  inline void addDecimalPoint() {
    if(!m_hasDecimalPoint) {
      addstr(_T("."));
      m_hasDecimalPoint = true;
    }
  }
  inline void addZeroes(int count) {
    m_result.insert(m_result.length(), count, '0');
  }
  inline void removeTrailingZeroes() {
    if(m_hasDecimalPoint) {
      StrStream::removeTralingZeroDigits(m_result);
    }
  }
  inline void removeLast(intptr_t n) {
    if(n > 0) {
      m_result.remove(m_result.length() - n, n);
    }
  }

  void setEllipsisAtEnd() {
    if(m_result.length() >= 3) {
      _tcscpy(&m_result[m_result.length() - 3], _T("..."));
    }
  }
  void formatNonNormal(const BigReal &n) {
    TCHAR tmp[100];
    switch(n.m_low) {
    case BIGREAL_ZEROLOW:
      StrStream::formatZero(m_result, 19, ios::fixed | ios::left, 21);
      break;
    case BIGREAL_NANLOW:
      addstr(StrStream::formatqnan(tmp));
      break;
    case BIGREAL_INFLOW:
      addstr(n.m_negative ? StrStream::formatninf(tmp) : StrStream::formatpinf(tmp));
      break;
    default:
      addstr(format(_T("Invalid state:expo:%I64d, low:%I64d"), (INT64)n.m_expo, (INT64)n.m_low));
      break;
    }
  }

public:
  BigRealAddIn(DEBUGHELPER *pHelper, String &dst) : m_helper(pHelper), m_result(dst), m_hasDecimalPoint(false) {
  }
  void toString(BigReal &n, size_t maxResult) {
    const ETYPE expo = n.m_expo;
    if(expo == escExpo) {
      formatNonNormal(n);
      return;
    }

    Digit<VTYPE> digit;
    getDigit(digit, n.m_first);

    const int    firstDigitCount = getDecimalDigitCount(digit.n);
    const int    firstExpo10     = firstDigitCount - 1;

    int totalDecimalDigitCount;
    if(expo == n.m_low) { // calculate total number of decimal digits in BigReal
      totalDecimalDigitCount = firstDigitCount;
    } else {
      Digit<VTYPE> lastDigit;
      getDigit(lastDigit, n.m_last);
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

    if(n.m_negative) {
      addstr(_T("-"));
    }
    const ETYPE expo10 = expo * log10Base + firstExpo10;

    if((expo10 < -4) || (expo10 >= 18)) { // use scientific format
      const String expoStr = format(_T("e%+03I64d"), (INT64)expo10);

      const int    maxSignificandDigits = maxResult - m_result.length() - expoStr.length() - 1;   // sign, exponent, decimalpoint
      const int    precision            = min(maxSignificandDigits, totalDecimalDigitCount) - 1;  // Number of decimal digits after decimalpoint
      const VTYPE  firstScaleE10        = (VTYPE)pow10(firstExpo10);

      addDigitStr(digit.n / firstScaleE10);
      if(precision > 0) {
        addDecimalPoint();

        digit.n %= firstScaleE10;
        int decimalsDone;

        if(precision < firstExpo10) {
          digit.n /= (VTYPE)pow10(firstExpo10 - precision);
          decimalsDone = precision;   // 0 < precision < firstExpo10 < log10Base
        } else {
          decimalsDone = firstExpo10; // firstExpo10 < log10Base
        }
        if(decimalsDone > 0) {
          addDigitStr(digit.n, decimalsDone);
        }
        for(int rest = precision - decimalsDone; (rest > 0) && nextDigit(digit); rest -= log10Base) {
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
        addstr(_T("0"));
        addDecimalPoint();
        precision = (int)maxResult - (int)m_result.length();
        if(expo10 < -1) {
          const ETYPE zeroCount = -expo10 - 1;
          addZeroes((int)zeroCount);
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
          addZeroes((int)d);
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
};

ADDIN_API HRESULT WINAPI AddIn_BigReal(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  try {
    String tmpStr;
    switch(pHelper->getProcessorType()) {
    case PRTYPE_X86:
      { BigRealx86 n;
        pHelper->getRealObject(&n, sizeof(n));
        BigRealAddIn<BigRealx86, int, DWORD, LOG10BASEx86, ESCEXPOx86>(pHelper, tmpStr).toString(n, maxResult - 1);
      }
      break;
    case PRTYPE_X64:
      { BigRealx64 n;
        pHelper->getRealObject(&n, sizeof(n));
        BigRealAddIn<BigRealx64, INT64, QWORD, LOG10BASEx64, ESCEXPOx64>(pHelper, tmpStr).toString(n, maxResult - 1);
      }
      break;
    }
    USES_CONVERSION;
    const char *cp = T2A(tmpStr.cstr());
    strncpy(pResult, cp, maxResult);
  } catch (...) {
    strncpy(pResult, "", maxResult);
  }
  return S_OK;
}
