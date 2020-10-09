#pragma once

#include <Math/FPU.h>
#include <Math/Double80.h>
#include <StrStream.h>

namespace Double80Str {

using namespace OStreamHelper;

// assume isxdigit(ch)
static inline int charToInt(wchar_t ch) {
  extern unsigned char hexCharLookup[];
  return hexCharLookup[ch];
}

template<typename CharType> const CharType *parseD80Decimal(const CharType *s, Double80 &result, bool &gotDigit) {
  if(iswdigit(*s)) {
    gotDigit = true;
    result = charToInt(*(s++));
    while(iswdigit(*s)) {
      result *= 10;
      result += charToInt(*(s++));
    }
  }
  bool     hasFraction = false;
  Double80 fraction = 0;
  UINT     fractionLength = 0;
  if((*s == '.') && iswdigit(s[1])) {
    s++;
    gotDigit = true;
    fraction = charToInt(*(s++));
    for(fractionLength++; iswdigit(*s); fractionLength++) {
      fraction *= 10;
      fraction += charToInt(*(s++));
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
      int  exponent = 0;
      if(*s == '-') {
        expPositive = false;
        s++;
      } else if(*s == '+') {
        s++;
      }
      bool gotExpoDigits = false;
      if(iswdigit(*s)) {
        exponent = charToInt(*(s++));
        gotExpoDigits = true;
        while(iswdigit(*s)) {
          exponent *= 10;
          exponent += charToInt(*(s++));
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
        result /= Double80::pow10(exponent);
        if(hasFraction) fraction /= Double80::pow10(exponent + fractionLength);
      }
    }
    if(hasFraction) {
      result += fraction;
    }
  }
  return s;
}

template<typename CharType> const CharType *parseD80Hex(const CharType *s, Double80 &result, bool &gotDigit) {
  if(iswxdigit(*s)) {
    gotDigit = true;
    result = charToInt(*(s++));
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
    int  exponent = 0;
    if(*s == '-') {
      expPositive = false;
      s++;
    } else if(*s == '+') {
      s++;
    }
    bool gotExpoDigits = false;
    if(iswdigit(*s)) {
      exponent = charToInt(*(s++));
      gotExpoDigits = true;
      while(iswdigit(*s)) {
        exponent *= 10;
        exponent += charToInt(*(s++));
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

template<typename CharType> Double80 parseDouble80(const CharType *s, CharType **end, _locale_t locale) {
  errno = 0;
  bool     isNegative = false;
  bool     gotDigit = false;
  Double80 result = 0;
  while(iswspace(*s)) s++;
  if(*s == '-') {
    isNegative = true;
    s++;
  } else if(*s == '+') {
    s++;
  }

  _se_translator_function oldTranslator = FPU::setExceptionTranslator(FPUexceptionTranslator);
  FPU::clearExceptions();
  const FPUControlWord    oldcw = FPU::getControlWord();
  try {
    FPUControlWord ctrlWord = oldcw;
    FPU::setControlWord(ctrlWord.adjustExceptionMask(FPU_OVERFLOW_EXCEPTION, FPU_UNDERFLOW_EXCEPTION | FPU_DENORMALIZED_EXCEPTION)
                        .setRoundMode(FPU_ROUNDCONTROL_ROUND));

    if((s[0] == '0') && ((s[1] == 'x') || (s[1] == 'X'))) {
      s = parseD80Hex(s + 2, result, gotDigit);
    } else {
      s = parseD80Decimal(s, result, gotDigit);
    }
    if(end) {
      *end = (CharType *)s;
    }
  } catch(FPUException fpue) {
    FPU::clearExceptionsNoWait();
    if(fpue.m_code == EXCEPTION_FLT_OVERFLOW) {
      result = DBL80_MAX;
      errno = ERANGE;
    } else if(fpue.m_code == EXCEPTION_FLT_UNDERFLOW) {
      result = DBL80_MIN;
      errno = ERANGE;
    } else {
      result = DBL80_NAN;
    }
  } catch(...) {
    FPU::clearExceptionsNoWait();
    FPU::restoreControlWord(oldcw);
    FPU::setExceptionTranslator(oldTranslator);
    throw;
  }
  FPU::clearExceptionsNoWait();
  FPU::restoreControlWord(oldcw);
  FPU::setExceptionTranslator(oldTranslator);
  return isNegative ? -result : result;
}

// Assume x > 0
char   *getDigitsStr(char *str, Double80 &x, int &e10);
void    insertHexPrefixIfMissing(String &str);

// Assume x != 0 and finite and precision >= 0. ignore sign of x
// Return dst
String &formatHex(String &dst, const Double80 &x, StreamSize prec, FormatFlags flags);

// Assume m > 0, and expo10 approx = expo10(m)
// normalize m, so _UI64_MAX/10 < m <= _UI64_MAX and x = (negative?-1:1) * m * 10^(expo10-DBL80_DIG)
void    normalizeValue(Double80 &m, int &expo10);


#define declareBuffer(b,size)          CharType b[size];  int b##_length = 0
#define declareAssignedBuffer(b,dst)   CharType *b = dst; int b##_length = 0
#define addChar(b,ch)                  b[b##_length++] = ch
#define addDigit(b,d)                  addChar(b,(d)+'0')
#define getLength(b)                   (b##_length)
#define isEmpty(b)                     (getLength(b)==0)
#define removeLast(b)                  b[--b##_length]

extern const Double80 &d80Maxui64q10;

template<typename CharType> CharType *d80tostr(CharType *dst, const Double80 &x) {
  if(!isfinite(x)) {
    return formatUndefined(dst, _fpclass(x));
  } else if(x.isZero()) {
    return strCpy(dst, "0.0000000000000000000e+000");
  }

  Double80 m = x;
  bool     negative;
  if(negative = m.isNegative()) {
    m = -m;
  }

  // Assertion: (m > 0) && (x = (negative?-1:1) * m)
  char         digits[30];
  int          expo10;
  const UINT64 ui64 = (UINT64)m;
  if(m == ui64) {
    _ui64toa(ui64, digits, 10);
    if(DBL80_DIG > (expo10 = (int)strlen(digits) - 1)) {
      char *p = digits + DBL80_DIG + 1;
      *p = 0;
      for(const char *z = digits + expo10; --p > z;) {
        *p = '0';
      }
    }
  } else {
    const FPUControlWord cwSave = FPU::setRoundMode(FPU_ROUNDCONTROL_ROUND);
    expo10 = Double80::getExpo10(x) - DBL80_DIG + 1;
    normalizeValue(m, expo10);
    // Assertion: _UI64_MAX/10 < m <= _UI64_MAX and x = (negative?-1:1) * m * 10^(expo10-DBL80_DIG)
    assert((d80Maxui64q10 < m) && (m <= _UI64_MAX));
    getDigitsStr(digits, m, expo10);
    expo10--;
    FPU::restoreControlWord(cwSave);
  }

  declareAssignedBuffer(result, dst);
  if(negative) {
    addChar(result, '-');
  }
  const char *p = digits;
  addChar(result, *(p++));
  addChar(result, '.');
  while(*p) {
    addChar(result, *(p++));
  }
  addChar(result, 'e');
  if(expo10 < 0) {
    addChar(result, '-');
    expo10 = -expo10;
  } else {
    addChar(result, '+');
  }
  declareBuffer(exponentBuffer, 10);
  do {
    const int ed = expo10 % 10;
    expo10 /= 10;
    addDigit(exponentBuffer, ed);
  } while(expo10 != 0);
  for(int i = getLength(exponentBuffer); i <= 2; i++) {
    addChar(result, '0');
  }
  while(!isEmpty(exponentBuffer)) {
    addChar(result, removeLast(exponentBuffer));
  }
  addChar(result, 0);
  return dst;
}

}; // namespace Double80Str
