#include "pch.h"

#include <Math/Int128.h>

#pragma warning(disable : 4073)
#pragma init_seg(lib)

const _uint128 _UI128_MAX(0xffffffffffffffff, 0xffffffffffffffff);

static const _uint128 _10(10);
static const _uint128 _1E9(1000000000);

char*_ui128toa(_uint128 value, char *str, int radix) {
  assert(radix >= 2 && radix <= 36);
  if (value.isZero()) {
    return strcpy(str, "0");
  }

  char *s = str;
  switch (radix) {
  case 2:
  case 16:
    { const int partLen = (radix == 2) ? 32 : 8;
      _ultoa(value.s4.i[3], str, radix);
      for (int i = 2; i >= 0; i--) {
        char tmpStr[40];
        _ultoa(value.s4.i[i], tmpStr, radix);
        int l = strlen(tmpStr);
        for (int i = partLen - l; i--;) *(s++) = '0';
        strcpy(s, tmpStr);
        s += l;
      }
      *s = 0;
    }
    return str;
  case 10:
    { for(;;) {
        const unsigned int c = value % _1E9;
        char tmpStr[20];
        _ultoa(c, tmpStr, 10);
        strcpy(s, _strrev(tmpStr));
        int l = strlen(tmpStr);
        s += l;
        value /= _1E9;
        if(value) {
          while(l++ < 9) *(s++) = '0'; // append zeroes
        } else {
          break;
        }
      }
      *s = 0;
      return _strrev(str);
    }
  }

  _uint128 v(value);
  const _uint128 r(radix);
  while (!v.isZero()) {
    const unsigned int c = v % r;
    *(s++) = radixLetter(c);
    v /= r;
  }
  *s = 0;
  return _strrev(str);
}

wchar_t *_ui128tow(_uint128 value, wchar_t *str, int radix) {
  assert(radix >= 2 && radix <= 36);
  if (value.isZero()) {
    return wcscpy(str, L"0");
  }

  wchar_t *s = str;
  switch (radix) {
  case 2:
  case 16:
    { const int partLen = (radix == 2) ? 32 : 8;
      _ultow(value.s4.i[3], str, radix);
      for (int i = 2; i >= 0; i--) {
        wchar_t tmpStr[40];
        _ultow(value.s4.i[i], tmpStr, radix);
        int l = wcslen(tmpStr);
        for (int i = partLen - l; i--;) *(s++) = '0';
        wcscpy(s, tmpStr);
        s += l;
      }
      *s = 0;
    }
    return str;
  case 10:
    { for(;;) {
        const unsigned int c = value % _1E9;
        wchar_t tmpStr[20];
        _ultow(c, tmpStr, 10);
        wcscpy(s, _wcsrev(tmpStr));
        int l = wcslen(tmpStr);
        s += l;
        value /= _1E9;
        if (value) {
          while(l++ < 9) *(s++) = '0'; // append zeroes
        } else {
          break;
        }
      }
      *s = 0;
      return _wcsrev(str);
    }
  }

  _uint128 v(value);
  const _uint128 r(radix);
  do {
    const unsigned int c = v % r;
    *(s++) = radixLetter(c);
    v /= r;
  } while (!v.isZero());
  *s = 0;
  return _wcsrev(str);
}


template<class CharType> const CharType *parseDec(const CharType *str, _uint128 &n) {
  bool gotDigit = false;
  if (*str == '+') str++;
  for(; iswdigit(*str); str++) {
    if(!gotDigit) {
      n = *str - '0';
      gotDigit = true;
    }
    else {
      const unsigned int d = *str - '0';
      n *= _10;
      n += d;
    }
  }
  return gotDigit ? str : NULL;
}

template<class CharType> const CharType *parseHex(const CharType *str, _uint128 &n) {
  bool gotDigit = false;
  for (; iswxdigit(*str); str++) {
    if(!gotDigit) {
      n = convertNumberChar(*str);
      gotDigit = true;
    }
    else {
      const unsigned int d = convertNumberChar(*str);
      n <<= 4;
      n += d;
    }
  }
  return gotDigit ? str : NULL;
}

template<class CharType> const CharType *parseOct(const CharType *str, _uint128 &n) {
  bool gotDigit = false;
  for (; iswodigit(*str); str++) {
    if(!gotDigit) {
      n = convertNumberChar(*str);
      gotDigit = true;
    }
    else {
      const unsigned int d = convertNumberChar(*str);
      n <<= 3;
      n += d;
    }
  }
  return gotDigit ? str : NULL;
}

const char *_uint128::parseDec(const char *str) {
  return ::parseDec<char>(str, *this);
}

const char *_uint128::parseHex(const char *str) {
  return ::parseHex<char>(str, *this);
}

const char *_uint128::parseOct(const char *str) {
  return ::parseOct<char>(str, *this);
}

_uint128::_uint128(const char *str) {
  bool ok = false;
  if (*str == '0') {
    switch (str[1]) {
    case 'x':
    case 'X':
      ok = parseHex(str + 2) != NULL;
      break;
    case 0:
      *this = 0;
      ok = true;
      break;
    default:
      ok = parseOct(str + 1) != NULL;
      break;
    }
  }
  else {
    ok = parseDec(str) != NULL;
  }
  if (!ok) {
    throwException("_uint128:string is not an integer");
  }
}



const wchar_t *_uint128::parseDec(const wchar_t *str) {
  return ::parseDec<wchar_t>(str, *this);
}

const wchar_t *_uint128::parseHex(const wchar_t *str) {
  return ::parseHex<wchar_t>(str, *this);
}

const wchar_t *_uint128::parseOct(const wchar_t *str) {
  return ::parseOct<wchar_t>(str, *this);
}

_uint128::_uint128(const wchar_t *str) {
  bool ok = false;
  if (*str == '0') {
    switch (str[1]) {
    case 'x':
      ok = parseHex(str + 2) != NULL;
      break;
    case 0:
      *this = 0;
      ok = true;
      break;
    default:
      ok = parseOct(str + 1) != NULL;
      break;
    }
  }
  else {
    ok = parseDec(str) != NULL;
  }
  if (!ok) {
    throw exception("_uint128:string is not an integer");
  }
}

