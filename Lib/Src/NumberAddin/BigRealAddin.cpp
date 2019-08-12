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

class Digitx86 {
public:
  DWORD         n;
  DWORD         next;
  DWORD         prev;
};

class BigRealx86 {
public:
  DWORD         m_vfptr;               // pointer to vtable
  DWORD         m_first;               // Most significand  digit
  DWORD         m_last;                // Least significand digit
  int           m_expo, m_low;         // if isZero() then (m_expo,m_low)=(BIGREAL_ZEROEXPO,0) else m_expo = m_low+getLength()-1
  bool          m_negative;            // True for x < 0. else false
  DWORD         m_digitPool;
};

// -------------------------------------------------------------------

#define LOG10BASEx64  18
#define ESCEXPOx64   -900000000000000000

class Digitx64 {
public:
  QWORD          n;
  QWORD          next;
  QWORD          prev;
};

class BigRealx64 {
public:
  QWORD          m_vfptr;               // pointer to vtable
  QWORD          m_first;               // Most significand  digit
  QWORD          m_last;                // Least significand digit
  INT64          m_expo, m_low;         // if isZero() then (m_expo,m_low)=(BIGREAL_ZEROEXPO,0) else m_expo = m_low+getLength()-1
  bool           m_negative;            // True for x < 0. else false
  QWORD          m_digitPool;
};

// Values for BigReal::m_low, if m_expo == BIGREAL_ESCEXPO
#define BIGREAL_ZEROLOW    FP_ZERO
#define BIGREAL_NANLOW     FP_NAN
#define BIGREAL_INFLOW     FP_INFINITE // +/- infinite depending on m_negative

template<class BigReal, class Digit> class BigRealAddIn {
private:
  DEBUGHELPER *m_helper;
  Digit &getDigit(Digit &d, QWORD addr) const {
    if(sizeof(Digit) == sizeof(Digitx86)) {
      return *(Digit*)m_helper->getObjectx86(&d, (DWORD)addr, sizeof(Digit));
    } else {
      return *(Digit*)m_helper->getObjectx64(&d, addr, sizeof(Digit));
    }
  }

public:
  BigRealAddIn(DEBUGHELPER *pHelper) : m_helper(pHelper) {
  }
  String toString(BigReal &n, INT64 zeroExpo, int log10Base, size_t maxResult) const;
};

template<class BigReal, class Digit> String BigRealAddIn<BigReal, Digit>::toString(BigReal &n, INT64 escExpo, int log10Base, size_t maxResult) const {
  const INT64 expo = n.m_expo;
  String      result;

  if(expo == escExpo) {
    switch(n.m_low) {
    case BIGREAL_ZEROLOW:
      StrStream::formatZero(result, 19, ios::fixed|ios::left, 21);
      break;
    case BIGREAL_NANLOW : 
      StrStream::formatqnan(result);
      break;
    case BIGREAL_INFLOW :
      if(n.m_negative) {
        StrStream::formatninf(result);
      } else {
        StrStream::formatpinf(result);
      }
      break;
    default:
      result = format(_T("Invalid state:expo:%I64, low:%I64d"), expo, (INT64)n.m_low);
    }
    return result;
  }

  Digit digit;
  getDigit(digit, n.m_first);

  const int    firstDigitCount = getDecimalDigitCount(digit.n);
  const int    scale           = firstDigitCount - 1;
  const UINT64 scaleE10        = pow10(scale);
  int          decimalsDone    = 0;

  if(n.m_negative) {
    result = _T("-");
  }
  const INT64 e = expo * log10Base + scale;
  String expoStr;
  if(e != 0) {
    expoStr += format(_T("e%+03I64d"), e);
  }
  INT64 decimalDigits;
  if(n.m_expo == n.m_low) { // calculate total number of decimal digits in BigReal
    decimalDigits = firstDigitCount;
  } else {
    Digit lastDigit;
    getDigit(lastDigit, n.m_last);
    int lastDigitCount;
    if(lastDigit.n == 0) {
      lastDigitCount = 0;
    } else {
      lastDigitCount = log10Base;
      for(UINT64 last = lastDigit.n; last % 10 == 0; last /= 10) {
        lastDigitCount--;
      }
    }
    decimalDigits = (expo - n.m_low - 1) * log10Base + firstDigitCount + lastDigitCount;
  }
  const int    maxSignificandDigits = maxResult - result.length() - expoStr.length() - 1;   // sign, exponent, decimalpoint
  const int    precision            = (int)min(maxSignificandDigits-1, decimalDigits-1);    // number of digits after decimalpoint
  const bool   numberTooLong        = precision < decimalDigits - 1;
  const UINT64 firstDecimalDigit    = digit.n / scaleE10;

  result += format(_T("%llu"), firstDecimalDigit);
  bool hasDecimalPoint = false;

  if(precision > 0) {
    result += _T("."); hasDecimalPoint = true;

    UINT64 fraction = digit.n % scaleE10;
    if(precision < scale) {
      fraction /= pow10(scale - precision);
      decimalsDone = precision; // precision < scale < log10Base
    } else {
      decimalsDone = scale; // scale < log10Base
    }
    if(decimalsDone > 0) {
      result += format(_T("%0*.*I64u"), decimalsDone, decimalsDone, fraction);
    }
    while(decimalsDone < precision) {
      if(digit.next == 0) break;
      getDigit(digit, digit.next);
      UINT64 part       = digit.n;
      int    partLength = log10Base;
      int    rest       = precision - decimalsDone;
      if(rest < log10Base) {
        partLength = rest;
        part /= pow10(log10Base - rest);
      }
      result += format(_T("%0*.*I64u"), partLength, partLength, part);
      decimalsDone += partLength;
    }
  }
  if(numberTooLong && (result.length() + expoStr.length() == maxResult)) {
    for(int i = 0; (i < 3) && (result.length() > 0); i++) {
      result.removeLast();
    }
    result += _T("...");
  } else if(hasDecimalPoint) { // remove trailing zeros after decimalpoint
    while(result.last() == _T('0')) result.removeLast();
    if(result.last() == _T('.')) result.removeLast();
  }
  result += expoStr;
  return result;
}

ADDIN_API HRESULT WINAPI AddIn_BigReal(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  try {
    String tmpStr;
    switch(pHelper->getProcessorType()) {
    case PRTYPE_X86:
      { BigRealx86 n;
        pHelper->getRealObject(&n, sizeof(n));
        tmpStr = BigRealAddIn<BigRealx86, Digitx86>(pHelper).toString(n, ESCEXPOx86, LOG10BASEx86, maxResult - 1);
      }
      break;
    case PRTYPE_X64:
      { BigRealx64 n;
        pHelper->getRealObject(&n, sizeof(n));
        tmpStr = BigRealAddIn<BigRealx64, Digitx64>(pHelper).toString(n, ESCEXPOx64, LOG10BASEx64, maxResult - 1);
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
