#include "pch.h"

#include <Math/Int128.h>

#pragma warning(disable : 4073)
#pragma init_seg(lib)

const _int128  _I128_MIN(0x8000000000000000, 0x0000000000000000);
const _int128  _I128_MAX(0x7fffffffffffffff, 0xffffffffffffffff);
const _uint128 _UI128_MAX(0xffffffffffffffff, 0xffffffffffffffff);

static const _uint128 _10(10);

static UINT convertNumberChar(wchar_t digit) {
  switch(digit) {
  case '0': return 0;
  case '1': return 1;
  case '2': return 2;
  case '3': return 3;
  case '4': return 4;
  case '5': return 5;
  case '6': return 6;
  case '7': return 7;
  case '8': return 8;
  case '9': return 9;
  case 'a':
  case 'A': return 10;
  case 'b':
  case 'B': return 11;
  case 'c':
  case 'C': return 12;
  case 'd':
  case 'D': return 13;
  case 'e':
  case 'E': return 14;
  case 'f':
  case 'F': return 15;
  default :
    return 0;
  }
}

// Conversion from _int128/_uint128 to string

// Number of digits that should be appended to the string for each loop
// (index = radix = [2..36])
// For radix 8 and 32 the value is the bitshift for each digit
static const char digitCount[37] = {
   0, 0,32,20,16,13,12,11
 , 3,10, 9, 9, 8, 8, 8, 8
 , 8, 7, 7, 7, 7, 7, 7, 7
 , 6, 6, 6, 6, 6, 6, 6, 6
 , 5, 6, 6, 6, 6
};

// Highest power of radix that fits in 32 bit (index = radix)
// For radix 2,4,8,16,32 special code is used which doesn't use this table.
// For all other radices the element is used to get as many digits as possible
// by modulus and division
static const _uint128 powRadix[] = {
  0          //  not used
 ,0          //  not used
 ,0          //  not used
 ,0xcfd41b91 //  3486784401 =  3^20
 ,0          //  not used
 ,0x48c27395 //  1220703125 =  5^13
 ,0x81bf1000 //  2176782336 =  6^12
 ,0x75db9c97 //  1977326743 =  7^11
 ,0          //  not used
 ,0xcfd41b91 //  3486784401 =  9^10
 ,0x3b9aca00 //  1000000000 = 10^9
 ,0x8c8b6d2b //  2357947691 = 11^9
 ,0x19a10000 //   429981696 = 12^8
 ,0x309f1021 //   815730721 = 13^8
 ,0x57f6c100 //  1475789056 = 14^8
 ,0x98c29b81 //  2562890625 = 15^8
 ,0          //  not used
 ,0x18754571 //   410338673 = 17^7
 ,0x247dbc80 //   612220032 = 18^7
 ,0x3547667b //   893871739 = 19^7
 ,0x4c4b4000 //  1280000000 = 20^7
 ,0x6b5a6e1d //  1801088541 = 21^7
 ,0x94ace180 //  2494357888 = 22^7
 ,0xcaf18367 //  3404825447 = 23^7
 , 0xb640000 //   191102976 = 24^6
 , 0xe8d4a51 //   244140625 = 25^6
 ,0x1269ae40 //   308915776 = 26^6
 ,0x17179149 //   387420489 = 27^6
 ,0x1cb91000 //   481890304 = 28^6
 ,0x23744899 //   594823321 = 29^6
 ,0x2b73a840 //   729000000 = 30^6
 ,0x34e63b41 //   887503681 = 31^6
 ,0          //  not used
 ,0x4cfa3cc1 //  1291467969 = 33^6
 ,0x5c13d840 //  1544804416 = 34^6
 ,0x6d91b519 //  1838265625 = 35^6
 ,0x81bf1000 //  2176782336 = 36^6
};

#define ULTOSTR(v, str, radix)        \
{ if(sizeof(Ctype) == sizeof(char))   \
    _ultoa(v, (char*)str, radix);     \
  else                                \
    _ultow(v, (wchar_t*)str, radix);  \
}

#define STRLEN(str)      ((sizeof(Ctype)==sizeof(char))?strlen((char*)str):wcslen((wchar_t*)str))
#define STRCPY(dst, src) ((sizeof(Ctype)==sizeof(char))?(Ctype*)strcpy((char*)dst, (char*)src):(Ctype*)wcscpy((wchar_t*)dst, (wchar_t*)src))
#define STRREV(str)      ((sizeof(Ctype)==sizeof(char))?(Ctype*)_strrev((char*)str):(Ctype*)_wcsrev((wchar_t*)str))

template<class Int128Type, class Ctype> Ctype *int128toStr(Int128Type value, Ctype *str, int radix) {
  assert(radix >= 2 && radix <= 36);

  bool setSign = false;
  if(value.isZero()) {
    str[0] = '0';
    str[1] = 0;
    return str;
  }

  const UINT dc = digitCount[radix];

  Ctype *s = str;
  switch(radix) {
  case 2 :
  case 4 :
  case 16:
    { for(int i = 3; i >= 0; i--) {
        if(value.s4.i[i]) {
          Ctype tmpStr[40];
          ULTOSTR(value.s4.i[i], tmpStr, radix);
          const size_t l = STRLEN(tmpStr);
          if(s != str) {
            for(size_t i = dc - l; i--;) *(s++) = '0'; // fill up with zeroes, if not leading digits
          }
          STRCPY(s, tmpStr);
          s += l;
        } else if(s != str) {
          for(size_t i = dc; i--;) *(s++) = '0'; // fill up with zeroes, if not leading digits
        }
      }
      *s = 0;
    }
    return str;
  case 8 : // Get 3 bits/digit giving 30 bits/loop, ie 10 digits/loop
  case 32: // Get 5 bits/digit giving 30 bits/loop too! which is 6 digits/loop
    { const UINT mask = (1 << dc) - 1;
      const UINT dpl  = 30 / dc;
      _uint128 v = value;
      for(;;) {
        UINT v30 = v.s4.i[0] & ((1<<30) - 1);
        v >>= 30;
        UINT count;
        for(count = 0; v30; count++, v30 >>= dc) {
          *(s++) = radixLetter(v30 & mask);
        }
        if(v.isZero()) break;
        while(count++ < dpl) *(s++) = '0';
      }
      *s = 0;
      return STRREV(str);
    }
  case 10:
    if(value.isNegative()) {
      value = -value;
      setSign = true;
    }
    // NB continue case
  default:
    _uint128 v = value;
    const _uint128 &divisor = powRadix[radix];
    for(;;) {
      const UINT c = v % divisor;
      Ctype tmpStr[40];
      ULTOSTR(c, tmpStr, radix);
      STRCPY(s, STRREV(tmpStr));
      size_t l = STRLEN(tmpStr);
      s += l;
      v /= divisor;
      if(v) {
        while(l++ < dc) *(s++) = '0'; // append zeroes
      } else {
        break;
      }
    }
    if(setSign) *(s++) = '-';
    *s = 0;
    return STRREV(str);
  }
}

char *_i128toa(_int128 value, char *str, int radix) {
  return int128toStr<_int128, char>(value, str, radix);
}

wchar_t *_i128tow(_int128 value, wchar_t *str, int radix) {
  return int128toStr<_int128, wchar_t>(value, str, radix);
}

char*_ui128toa(_uint128 value, char *str, int radix) {
  return int128toStr<_uint128, char>(value, str, radix);
}

wchar_t *_ui128tow(_uint128 value, wchar_t *str, int radix) {
  return int128toStr<_uint128, wchar_t>(value, str, radix);
}

static inline bool isRadixDigit(wchar_t ch, int radix, int &value) {
  int v;
  if(iswdigit(ch)) {
    v = ch - '0';
  } else if(iswalpha(ch)) {
    v = ch - (iswupper(ch)?'A':'a') + 10;
  } else {
    return false;
  }
  if(v >= radix) return false;
  value = v;
  return true;
}

template<class Int128Type, class Ctype> Int128Type strtoint128(const Ctype *s, Ctype **end, int radix) {
  assert((radix == 0) || (radix >= 2) && (radix <= 36));

  bool negative = false;
  bool gotDigit = false;
  while(iswspace(*s)) s++; // skip whitespace
  if(*s == '-') { // read optional sign
    s++;
    negative = true;
  } else if (*s == '+') {
    s++;
  }
  if(radix == 0) { // first determine if radix is 8,16 or 10
    if(*s == '0') {
      gotDigit = true;
      s++;
      if(end) *end = (Ctype*)s;
      if((*s == 'x') || (*s == 'X')) {
        radix = 16; s++;
      } else {
        radix = 8;
      }
    } else if(iswdigit(*s)) {
      radix = 10;
    } else {
      return 0; // nothing regognized
    }
  }
  Int128Type result = 0;
  int        digit;
  if(isRadixDigit(*(s++), radix, digit)) {
    gotDigit = true;
    result = digit;
    switch(radix) {
#define SHIFTLOOP(bitsPerDigit) while(isRadixDigit(*(s++), radix, digit)) { result <<= (bitsPerDigit); result |= digit; }
    case 2 : SHIFTLOOP(1) break;
    case 4 : SHIFTLOOP(2) break;
    case 8 : SHIFTLOOP(3) break;
    case 16: SHIFTLOOP(4) break;
    case 32: SHIFTLOOP(5) break;
    default:
      while(isRadixDigit(*(s++), radix, digit)) {
        result *= radix;
        result += digit;
      }
      break;
    }
  }
  if(!gotDigit) return 0;
  if(end) *end = (Ctype*)s;
  return negative ? -result : result;
}

_int128 _strtoi128(const char *str, char **end, int radix) {
  return strtoint128<_int128, char>(str,end,radix);
}

_uint128 _strtoui128(const char *str, char **end, int radix) {
  return strtoint128<_uint128, char>(str,end,radix);
}

_int128 _wcstoi128(const wchar_t *str, wchar_t **end, int radix) {
  return strtoint128<_int128, wchar_t>(str,end,radix);
}

_uint128 _wcstoui128(const wchar_t *str, wchar_t **end, int radix) {
  return strtoint128<_uint128, wchar_t>(str,end,radix);
}
