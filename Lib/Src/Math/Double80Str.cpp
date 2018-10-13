#include "pch.h"
#include <Math/Double80.h>
#include <Math/FPU.h>

template<class CharType> Double80 _strtod80(const CharType *s, CharType **end) {
  errno=0;
  bool     isNegative = false;
  bool     gotDigit   = false;
  Double80 result     = 0;
  while(iswspace(*s)) s++;
  if(*s == '-') {
    isNegative = true;
    s++;
  } else if(*s == '+') {
    s++;
  }

  _se_translator_function prevTranslator = FPU::setExceptionTranslator(FPUexceptionTranslator);
  FPU::clearExceptions();
  const FPUControlWord oldcw = FPU::adjustExceptionMask(FPU_OVERFLOW_EXCEPTION | FPU_UNDERFLOW_EXCEPTION,0);
  FPU::setRoundMode(FPU_ROUNDCONTROL_ROUND);
  try {
    if(iswdigit(*s)) {
      gotDigit = true;
      result = (int)(*(s++) - '0');
      while(iswdigit(*s)) {
        result *= 10;
        result += (int)(*(s++) - '0');
      }
    }
    if(*s == '.' && iswdigit(s[1])) {
      s++;
      Double80 decimals = 0, p = 10;
      for(;iswdigit(*s); p *= 10, decimals *= 10) {
        decimals += (int)(*(s++) - '0');
      }
      result += decimals/p;
    }
    if(*s == 'e' || *s == 'E') {
      const CharType *epos = s++;
      int expoSign = 1;
      if(*s == '-') {
        expoSign = -1;
        s++;
      } else if(*s == '+') {
        s++;
      }
      int exponent = 0;
      bool gotExpoDigits = false;
      while(iswdigit(*s)) {
        exponent = exponent * 10 + (*(s++) - '0');
        gotExpoDigits = true;
      }
      if(!gotExpoDigits) {
        s = epos;
      } else {
        exponent *= expoSign;
        if(exponent < -4900) {
          result /= Double80::pow10(4900);
          exponent += 4900;
          result /= Double80::pow10(-exponent);
        } else {
          result *= Double80::pow10(exponent);
        }
      }
    }
    if(!gotDigit) {
      return 0;
    }
    if(end) {
      *end = (CharType*)s;
    }
  } catch(FPUException fpue) {
    FPU::clearExceptionsNoWait();
    if(fpue.m_code == EXCEPTION_FLT_OVERFLOW) {
      result  = DBL80_MAX;
      errno   = ERANGE;
    } else if(fpue.m_code == EXCEPTION_FLT_UNDERFLOW) {
      result  = DBL80_MIN;
      errno   = ERANGE;
    } else {
      result  = DBL80_NAN;
    }
  } catch(...) {
    FPU::clearExceptionsNoWait();
    FPU::restoreControlWord(oldcw);
    FPU::setExceptionTranslator(prevTranslator);
    throw;
  }
  FPU::clearExceptionsNoWait();
  FPU::restoreControlWord(oldcw);
  FPU::setExceptionTranslator(prevTranslator);
  return isNegative ? -result : result;
}

Double80 strtod80(const char *s, char **end) {
  return _strtod80<UCHAR>((UCHAR*)s, (UCHAR**)end);
}

Double80 wcstod80(const wchar_t *s, wchar_t **end) {
  return _strtod80<wchar_t>(s, end);
}

#define ASM_OPTIMIZED

#define declareBuffer(b,size)          CharType b[size];  int b##_length = 0
#define declareAssignedBuffer(b,dst)   CharType *b = dst; int b##_length = 0
#define addChar(b,ch)                  b[b##_length++] = ch
#define addDigit(b,d)                  addChar(b,(d)+'0')
#define getLength(b)                   (b##_length)
#define isEmpty(b)                     (getLength(b)==0)
#define removeLast(b)                  b[--b##_length]

// Assume x > 0
static char *getDigitsStr(char *str, Double80 &x, int &e10) {
  const UINT64 tmp  = getUint64(x);
  Double80     diff = x - tmp;          // diff >= 0
  if(diff.isZero()) {                   // diff == 0 => tmp == x => use ui64toa on tmp as result
    _ui64toa(tmp, str, 10);
    e10 += (int)strlen(str);
    return str;
  } else {                              // 0 < diff <= 1 => _MAXUI64 < x <= _MAXUI64+1
    _ui64toa(tmp, str, 10);
    e10 += (int)strlen(str);
    int diffInt;
    do {
      diff *= 10;
      diffInt = getInt(diff);
    } while(diff != diffInt);
    char fraction[10];
    return strcat(str, _itoa(diffInt,fraction,10));
  }
}

static const Double80 d80Maxui64q10(0x1999999999999999ui64); //  1.844.674.407.370.955.161

template<class CharType> CharType *_d80tostr(CharType *dst, const Double80 &x) {
  if(!isfinite(x)) {
    return strCpy(dst, StrStream::formatUndefined(x).cstr());
  } else if(x.isZero()) {
    return strCpy(dst, "0.0000000000000000000e+000");
  }

  Double80 m      = x;
  bool     negative;
  if(m.isNegative()) {
    m = -m;
    negative = true;
  } else {
    negative = false;
  }

  // Assertion: (m > 0) && (x = (negative?-1:1) * m)
  char         digits[30];
  int          expo10;
  const UINT64 ui64    = getUint64(m);
  if(m == ui64) {
    _ui64toa(ui64, digits, 10);
    if(DBL80_DIG > (expo10 = (int)strlen(digits) - 1)) {
      char *p = digits + DBL80_DIG + 1;
      *p = 0;
      for(const char *z = digits+expo10; --p > z;) {
        *p = '0';
      }
    }
  } else {
    const FPUControlWord cwSave = FPU::setRoundMode(FPU_ROUNDCONTROL_ROUND);
    expo10 = Double80::getExpo10(x) - DBL80_DIG + 1;
    if(expo10 < 0) {
      if(expo10 >= -4900) { // take care of extreme small numbers
        m *= Double80::pow10(-expo10);
      } else {              // use 2 steps to prevent overflow
        m *= Double80::pow10(4900);
        m *= Double80::pow10(-expo10 - 4900);
      }
    } else if(expo10 > 0) {
      m /= Double80::pow10(expo10);
    }
    if(m < d80Maxui64q10) {
      m *= 10;
      expo10--;
    }
    // Assertion: _UI64_MAX/10 < m <= _UI64_MAX and x = (negative?-1:1) * m * 10^(expo10-DBL80_DIG)
    getDigitsStr(digits, m, expo10);
    expo10--;
    FPU::restoreControlWord(cwSave);
  }

  declareAssignedBuffer(result, dst);
  if(negative) {
    addChar(result,'-');
  }
  const char *p = digits;
  addChar(result, *(p++));
  addChar(result, '.');
  while(*p) {
    addChar(result, *(p++));
  }
  addChar(result,'e');
  if(expo10 < 0) {
    addChar(result,'-');
    expo10 = -expo10;
  } else {
    addChar(result,'+');
  }
  declareBuffer(exponentBuffer,10);
  do {
    const int ed = expo10 % 10;
    expo10 /= 10;
    addDigit(exponentBuffer,ed);
  } while(expo10 != 0);
  for(int i = getLength(exponentBuffer); i <= 2; i++) {
    addChar(result,'0');
  }
  while(!isEmpty(exponentBuffer)) {
    addChar(result,removeLast(exponentBuffer));
  }
  addChar(result,0);
  return dst;
}

char *d80toa(char *dst, const Double80 &x) {
  return _d80tostr<char>(dst, x);
}

wchar_t *d80tow(wchar_t *dst, const Double80 &x) {
  return _d80tostr<wchar_t>(dst, x);
}
