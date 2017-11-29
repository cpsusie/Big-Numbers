#include "pch.h"
#include <Math/Double80.h>

template<class CharType> Double80 _strtod80(const CharType *s, CharType **end) {
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
    const CharType *epos = s;
    s++;
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
        result *= exp10((Double80)-4900);
        exponent += 4900;
        result *= exp10((Double80)exponent);
      } else {
        result *= exp10((Double80)exponent);
      }
    }
  }
  if(!gotDigit) {
    return 0;
  }
  if(end) {
    *end = (CharType*)s;
  }
  return isNegative ? -result : result;
}

#define ASM_OPTIMIZED

#define declareBuffer(b,size)          CharType b[size];  int b##_length = 0
#define declareAssignedBuffer(b,dst)   CharType *b = dst; int b##_length = 0
#define addChar(b,ch)                  b[b##_length++] = ch
#define addDigit(b,d)                  addChar(b,(d)+'0')
#define getLength(b)                   (b##_length)
#define isEmpty(b)                     (getLength(b)==0)
#define removeLast(b)                  b[--b##_length]

template<class DstCharType> DstCharType *strCopy(DstCharType *dst, const char *src) {
  DstCharType *ret = dst;
  while(*(dst++) = *(src++));
  return ret;
}

#define copy(dst,src) strCopy<CharType>(dst,src)

template<class CharType> CharType *_d80tostr(CharType *dst, const Double80 &x) {
  if(isNan(x)) {
    if(!isInfinity(x)) {
      return copy(dst, "Nan");
    } else if(isPInfinity(x)) {
      return copy(dst, "+Infinity");
    } else if(isNInfinity(x)) {
      return copy(dst, "-Infinity");
    }
  } else if(x.isZero()) {
    return copy(dst, "0.00000000000000000e+000");
  }

  int expo10 = Double80::getExpo10(x);
  BYTE bcd[10];

#ifndef ASM_OPTIMIZED

  const USHORT cwSave = FPU::setRoundMode(FPU_ROUNDCONTROL_ROUND);
  Double80 m = (expo10 == 0) ? x : (x / exp10(expo10));
  m = m * tenE18;
  while(fabs(m) >= tenE18M1) {
    m = m / ten;
    expo10++;
  }

  // Assertion: 1 <= |m| < 1e18-1 and x = m * 10^(expo10-18)

  D80ToBCD(bcd, m);

  FPU::restoreControlWord(cwSave);

#else // ASM_OPTIMIZED

  D80ToBCDAutoScale(bcd, x, expo10);

#endif // ASM_OPTIMIZED

  declareAssignedBuffer(result, dst);

  if(bcd[9] & 0x80) {
    addChar(result,'-');
  }
  bool gotDigit = false;
  int decimals;
  for(int i = 8; i >= 0; i--) {
    if(gotDigit) {
      addDigit(result,bcd[i]>>4);
      addDigit(result,bcd[i]&0xf);
    } else if(bcd[i] == 0) {
      continue;
    } else {
      gotDigit = true;
      if(bcd[i] & 0xf0) {
        addDigit(result,bcd[i]>>4);
        addChar(result,'.');
        addDigit(result,bcd[i]&0x0f);
        decimals = i * 2 - 1;
      } else {
        addDigit(result,bcd[i]&0xf);
        addChar(result,'.');
        decimals = i * 2 - 2;
      }
    }
  }

  addChar(result,'e');
  expo10 += decimals - 16;
  if(expo10 < 0) {
    addChar(result,'-');
    expo10 = -expo10;
  } else {
    addChar(result,'+');
    if(expo10 == 0) {
      addChar(result,'0');
      addChar(result,'0');
      addChar(result,'0');
      addChar(result,0);
      return dst;
    }
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

Double80 strtod80(const char *s, char **end) {
  return _strtod80<UCHAR>((UCHAR*)s, (UCHAR**)end);
}

Double80 wcstod80(const wchar_t *s, wchar_t **end) {
  return _strtod80<wchar_t>(s, end);
}

char *d80toa(char *dst, const Double80 &x) {
  return _d80tostr<char>(dst, x);
}

wchar_t *d80tow(wchar_t *dst, const Double80 &x) {
  return _d80tostr<wchar_t>(dst, x);
}
