#include "pch.h"

#ifdef IS64BIT

#include <Math/Int128.h>

#pragma warning(disable : 4073)
#pragma init_seg(lib)

const _uint128 _UI128_MAX(0xffffffffffffffff, 0xffffffffffffffff);

static const _uint128 _0(0);
static const _uint128 _10(10);
static const _uint128 _16(16);
static const _uint128 _8(16);

char*_ui128toa(_uint128 value, char *str, int radix) {
  assert(radix >= 2 && radix <= 36);
  char *s = str;
  const _uint128 r(radix);
  while (value != _0) {
    const unsigned int c = value % r;
    *(s++) = radixLetter(c);
    value /= r;
  }
  if (s == str) {
    return strcpy(str, "0");
  }
  else {
    *s = 0;
    return _strrev(str);
  }
}

wchar_t *_ui128tow(_uint128 value, wchar_t *str, int radix) {
  assert(radix >= 2 && radix <= 36);
  wchar_t *s = str;
  const _uint128 r(radix);
  while (value != _0) {
    const unsigned int c = value % r;
    *(s++) = wradixLetter(c);
    value /= r;
  }
  if (s == str) {
    return wcscpy(str, L"0");
  }
  else {
    *s = 0;
    return _wcsrev(str);
  }
}


template<class CharType> const CharType *parseDec(const CharType *str, _uint128 &n) {
  bool gotDigit = false;
  for(; isdigit(*str); str++) {
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
  for (; isxdigit(*str); str++) {
    if(!gotDigit) {
      n = convertNumberChar(*str);
      gotDigit = true;
    }
    else {
      const unsigned int d = convertNumberChar(*str);
      n *= _16;
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
      n *= _8;
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

#endif
