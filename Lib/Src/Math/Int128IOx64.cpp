#include "pch.h"

#ifdef IS64BIT

#include <Math/Int128.h>

#pragma warning(disable : 4073)
#pragma init_seg(lib)

const _int128 _I128_MIN(0x8000000000000000, 0x0000000000000000);
const _int128 _I128_MAX(0x7fffffffffffffff, 0xffffffffffffffff);

static const _int128 _0(0);
static const _int128 _10(10);
static const _int128 _16(16);
static const _int128 _8(16);

char *_i128toa(_int128 value, char *str, int radix) {
  assert(radix >= 2 && radix <= 36);
  char *s = str;
  const bool negative = value < _0;
  if (negative && (radix == 10)) {
    _uint128 v = -value;
    while (v != _0) {
      const unsigned int c = v % _10;
      *(s++) = radixLetter(c);
      v /= _10;
    }
    *(s++) = '-';
    *s = 0;
    return _strrev(str);
  }

  _uint128 v(value);
  const _uint128 r(radix);
  while (v != _0) {
    const unsigned int c = v % r;
    *(s++) = radixLetter(c);
    v /= r;
  }
  if (s == str) {
    return strcpy(str, "0");
  }
  else {
    *s = 0;
    return _strrev(str);
  }
  return str;
}

wchar_t *_i128tow(_int128 value, wchar_t *str, int radix) {
  assert(radix >= 2 && radix <= 36);
  wchar_t *s = str;
  const bool negative = value < _0;
  if (negative && (radix == 10)) {
    _uint128 v = -value;
    while (v != _0) {
      const unsigned int c = v % _10;
      *(s++) = wradixLetter(c);
      v /= _10;
    }
    *(s++) = '-';
    *s = 0;
    return _wcsrev(str);
  }

  _uint128 v(value);
  const _uint128 r(radix);
  while (v != _0) {
    const unsigned int c = v % r;
    *(s++) = radixLetter(c);
    v /= r;
  }
  if (s == str) {
    return wcscpy(str, L"0");
  }
  else {
    *s = 0;
    return _wcsrev(str);
  }
  return str;
}

const char *_int128::parseDec(const char *str) { // return pointer to char following the number
  bool negative = false;
  switch (*str) {
  case '+':
    str++;
    break;
  case '-':
    str++;
    negative = true;
  }
  bool gotDigit = false;
  while (isdigit(*str)) {
    if (!gotDigit) {
      *this = _0;
      gotDigit = true;
    }
    const unsigned int d = *(str++) - '0';
    *this *= _10;
    *this += d;
  }
  if (!gotDigit) {
    return NULL;
  }
  if (negative) {
    *this = -*this;
  }
  return str;
}

const char *_int128::parseHex(const char *str) {
  bool gotDigit = false;
  while (isxdigit(*str)) {
    if (!gotDigit) {
      *this = 0;
      gotDigit = true;
    }
    const unsigned int d = convertNumberChar(*(str++));
    *this *= _16;
    *this += d;
  }
  return gotDigit ? str : NULL;
}

const char *_int128::parseOct(const char *str) {
  bool gotDigit = false;
  while (isodigit(*str)) {
    if (!gotDigit) {
      *this = 0;
      gotDigit = true;
    }
    const unsigned int d = convertNumberChar(*(str++));
    *this *= _8;
    *this += d;
  }
  return gotDigit ? str : NULL;
}

_int128::_int128(const char *str) {
  bool ok = false;
  if (*str == '-') {
    ok = parseDec(str) != NULL;
  } else {
    if (isdigit(*str)) {
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
    }
  }
  if (!ok) {
    throw exception("_int128:string is not an integer");
  }
}

#endif
