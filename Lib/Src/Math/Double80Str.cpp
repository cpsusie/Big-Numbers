#include "pch.h"
#include <StrStream.h>
#include <Math/Double80.h>
#include <Math/FPU.h>

using namespace OStreamHelper;

template<typename CharType> const CharType *parseD80Decimal(const CharType *s, Double80 &result, bool &gotDigit) {
  if(iswdigit(*s)) {
    gotDigit = true;
#define TCHARTODECIMAL(ch) ((int)((ch) - '0'))
    result   = TCHARTODECIMAL(*(s++));
    while(iswdigit(*s)) {
      result *= 10;
      result += TCHARTODECIMAL(*(s++));
    }
  }
  bool     hasFraction    = false;
  Double80 fraction       = 0;
  UINT     fractionLength = 0;
  if((*s == '.') && iswdigit(s[1])) {
    s++;
    gotDigit = true;
    fraction = TCHARTODECIMAL(*(s++));
    for(fractionLength++; iswdigit(*s); fractionLength++) {
      fraction *= 10;
      fraction += TCHARTODECIMAL(*(s++));
    }
    if(fraction != 0) {
      hasFraction = true;
    }
  }
  if(gotDigit) {
    if((*s != 'e') && (*s != 'E')) {
      if(hasFraction) fraction /= Double80::pow10(fractionLength);
    } else {
      const CharType *epos = s++;
      bool expPositive = true;
      int  exponent    = 0;
      if(*s == '-') {
        expPositive = false;
        s++;
      } else if(*s == '+') {
        s++;
      }
      bool gotExpoDigits = false;
      if(iswdigit(*s)) {
        exponent = TCHARTODECIMAL(*(s++));
        gotExpoDigits = true;
        while(iswdigit(*s)) {
          exponent *= 10;
          exponent += TCHARTODECIMAL(*(s++));
        }
      }
      if(!gotExpoDigits) {
        s = epos;
        if(hasFraction) fraction /= Double80::pow10(fractionLength);
      } else if(exponent == 0) {
        if(hasFraction) fraction /= Double80::pow10(fractionLength);
      } else if(expPositive) {
        result *= Double80::pow10(exponent);
        if(hasFraction) fraction *= Double80::pow10(exponent - fractionLength);
      } else { // exponent < 0
        if(exponent > 4900) {
          if(hasFraction) { // finalize fraction
            result += fraction / Double80::pow10(fractionLength);
            hasFraction = false;
          }
          result /= Double80::pow10(4900);
          exponent -= 4900;
        }
        result   /= Double80::pow10(exponent);
        if(hasFraction) fraction /= Double80::pow10(exponent + fractionLength);
      }
    }
    if(hasFraction) {
      result += fraction;
    }
  }
  return s;
}

static UINT charToInt(wchar_t ch) {
  switch(ch) {
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9': return ch - '0';
  case 'a':
  case 'b':
  case 'c':
  case 'd':
  case 'e':
  case 'f':  return ch - 'a' + 10;
  case 'A':
  case 'B':
  case 'C':
  case 'D':
  case 'E':
  case 'F':  return ch - 'A' + 10;
  }
  return 0;
}

template<typename CharType> const CharType *parseD80Hex(const CharType *s, Double80 &result, bool &gotDigit) {
  if(iswxdigit(*s)) {
    gotDigit = true;
    result   = charToInt(*(s++));
    while(iswxdigit(*s)) {
      result *= 16;
      result += charToInt(*(s++));
    }
  }
  if((*s == '.') && iswxdigit(s[1])) {
    Double80 fraction = 0;
    UINT     fractionLength = 0;
    s++;
    gotDigit = true;
    fraction = charToInt(*(s++));
    for(fractionLength++; iswxdigit(*s); fractionLength++) {
      fraction *= 16;
      fraction += charToInt(*(s++));
    }
    if(fraction != 0) {
      result += fraction / Double80::pow2(fractionLength * 4);
    }
  }
  if(gotDigit && ((*s == 'p') || (*s == 'P'))) {
    const CharType *epos = s++;
    bool expPositive = true;
    int  exponent    = 0;
    if(*s == '-') {
      expPositive = false;
      s++;
    } else if(*s == '+') {
      s++;
    }
    bool gotExpoDigits = false;
    if(iswdigit(*s)) {
      exponent = TCHARTODECIMAL(*(s++));
      gotExpoDigits = true;
      while(iswdigit(*s)) {
        exponent *= 10;
        exponent += TCHARTODECIMAL(*(s++));
      }
    }
    if(!gotExpoDigits) {
      s = epos;
    } else if(exponent != 0) {
      if(expPositive) {
        result *= Double80::pow2(exponent);
      } else { // exponent < 0
        result /= Double80::pow2(exponent);
      }
    }
  }
  return s;
}

template<typename CharType> Double80 _strtod80_locale(const CharType *s, CharType **end, _locale_t locale) {
  errno=0;
  bool     isNegative     = false;
  bool     gotDigit       = false;
  Double80 result         = 0;
  while(iswspace(*s)) s++;
  if(*s == '-') {
    isNegative = true;
    s++;
  } else if(*s == '+') {
    s++;
  }

  _se_translator_function prevTranslator = FPU::setExceptionTranslator(FPUexceptionTranslator);
  FPU::clearExceptions();
  FPUControlWord oldcw = FPU::getControlWord(), ctrlWord = oldcw;
  FPU::setControlWord(ctrlWord.adjustExceptionMask(FPU_OVERFLOW_EXCEPTION, FPU_UNDERFLOW_EXCEPTION | FPU_DENORMALIZED_EXCEPTION)
                              .setRoundMode(FPU_ROUNDCONTROL_ROUND));
  try {
    if((s[0] == '0') && ((s[1] == 'x') || (s[1] == 'X'))) {
      s = parseD80Hex(s+2, result, gotDigit);
    } else {
      s = parseD80Decimal(s, result, gotDigit);
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

Double80 _strtod80_l(const char *s, char **end, _locale_t locale) {
  return _strtod80_locale<UCHAR>((UCHAR*)s, (UCHAR**)end, locale);
}

Double80 _wcstod80_l(const wchar_t *s, wchar_t **end, _locale_t locale) {
  return _strtod80_locale<wchar_t>(s, end, locale);
}

#define declareBuffer(b,size)          CharType b[size];  int b##_length = 0
#define declareAssignedBuffer(b,dst)   CharType *b = dst; int b##_length = 0
#define addChar(b,ch)                  b[b##_length++] = ch
#define addDigit(b,d)                  addChar(b,(d)+'0')
#define getLength(b)                   (b##_length)
#define isEmpty(b)                     (getLength(b)==0)
#define removeLast(b)                  b[--b##_length]

// Assume x > 0
static char *getDigitsStr(char *str, Double80 &x, int &e10) {
  const UINT64 tmp  = (UINT64)x;
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
      diffInt = (int)diff;
    } while(diff != diffInt);
    char fraction[10];
    return strcat(str, _itoa(diffInt,fraction,10));
  }
}

// floor(MAX_UINT64 / 10)
static const Double80 d80Maxui64q10(0x1999999999999999ui64); //  1.844.674.407.370.955.161

#define MINEXPO10 -4910

// Assume m > 0, and expo10 approx = expo10(m)
// normalize m, so _UI64_MAX/10 < m <= _UI64_MAX and x = (negative?-1:1) * m * 10^(expo10-DBL80_DIG)
static void normalizeValue(Double80 &m, int &expo10) {
  if(expo10 > 0) {
    const Double80 tmp = m / Double80::pow10(expo10);
    if(tmp >= d80Maxui64q10) {
      m = tmp;
    } else {
      expo10--;
      m /= Double80::pow10(expo10);
    }
  } else if(expo10 < 0) {
    if(expo10 >= MINEXPO10) { // take care of denormalized numbers
      const Double80 tmp = m * Double80::pow10(-expo10);
      if(tmp >= d80Maxui64q10) {
        m = tmp;
      } else {
        expo10--;
        m *= Double80::pow10(-expo10);
      }
    } else {              // use 2 steps to prevent overflow
      m *= Double80::pow10(-MINEXPO10);
      const Double80 tmp = m * Double80::pow10(MINEXPO10 - expo10);
      if(tmp >= d80Maxui64q10) {
        m = tmp;
      } else {
        expo10--;
        m *= Double80::pow10(MINEXPO10 - expo10);
      }
    }
  }
  if(m < d80Maxui64q10) {
    m *= 10;
    expo10--;
  }
}


template<typename CharType> CharType *_d80tostr(CharType *dst, const Double80 &x) {
  if(!isfinite(x)) {
    return formatUndefined(dst,_fpclass(x));
  } else if(x.isZero()) {
    return strCpy(dst, "0.0000000000000000000e+000");
  }

  Double80 m      = x;
  bool     negative;
  if(negative = m.isNegative()) {
    m = -m;
  }

  // Assertion: (m > 0) && (x = (negative?-1:1) * m)
  char         digits[30];
  int          expo10;
  const UINT64 ui64    = (UINT64)m;
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
    normalizeValue(m, expo10);
    // Assertion: _UI64_MAX/10 < m <= _UI64_MAX and x = (negative?-1:1) * m * 10^(expo10-DBL80_DIG)
#if defined(_DEBUG)
    if((m <= d80Maxui64q10) || (m > _UI64_MAX)) {
      int fisk = 1;
    }
#endif
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
  return _d80tostr(dst, x);
}

wchar_t *d80tow(wchar_t *dst, const Double80 &x) {
  return _d80tostr(dst, x);
}
