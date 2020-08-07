#include "pch.h"

#include <StrStream.h>
#include <Math/Int128.h>

using namespace std;
using namespace OStreamHelper;
using namespace IStreamHelper;

template<typename StringType> static StringType &format128(StringType &dst, const _uint128 &n, StreamParameters &param) {
  FormatFlags flags = param.flags() & ~ios::showpos; // never show +/- for unsigned
  const UINT  radix = param.radix();
  TCHAR       buf[200];
  switch(radix) {
  case 8 :
  case 16:
    if(n.isZero()) { // dont show base-prefix for 0
      flags &= ~ios::showbase;
    }
    // NB..continue case
  case 10:
    _ui128tot(n, buf, radix);
    if((radix == 16) && (flags & ios::uppercase)) {
      _tcsupr(buf);
    }
    break;
  }
  param.flags(flags);
  return formatFilledNumericField(dst, buf, false, param);
}

template<typename StringType> static StringType &format128(StringType &dst, const _int128 &n, StreamParameters &param) {
  FormatFlags flags    = param.flags();
  const UINT  radix    = param.radix();
  bool        negative;
  TCHAR       buf[200];
  switch(radix) {
  case 8 :
  case 16:
    negative = false;
    _ui128tot((_uint128&)n, buf, radix);
    if((radix == 16) && (flags & ios::uppercase)) {
      _tcsupr(buf);
    }
    if(n.isZero()) { // dont show base-prefix for 0
      flags &= ~ios::showbase;
      param.flags(flags);
    }
    break;
  case 10:
    { negative = (n < 0);
      const _uint128 v = negative ? -n : n;
      _ui128tot(v, buf, 10);
    }
    break;
  }
  return formatFilledNumericField(dst, buf, negative, param);
}

template<typename IStreamType, typename CharType> IStreamType &getInt128(IStreamType &in, _int128 &n) {
  IStreamScanner<IStreamType, CharType> scanner(in);
  CharType ch        = scanner.peek();
  bool     gotDigits = false;
  _int128  result;

  switch(ch) {
  case '-':
  case '+':
    ch = scanner.next();
  }

  switch(scanner.radix()) {
  case 10:
    while(iswdigit(ch)) {
      ch = scanner.next();
      gotDigits = true;
    }
    if(gotDigits) {
      errno  = 0;
      result = _tcstoi128(scanner.getBuffer().cstr(), NULL, 10);
    }
    break;
  case 16:
    { if(ch == '0') {
        ch = scanner.next();
        if((ch == 'x') || (ch == 'X')) {
          ch = scanner.next();
        } else {
          gotDigits = true;
        }
      }
      while(iswxdigit(ch)) {
        ch = scanner.next();
        gotDigits = true;
      }
      if(gotDigits) {
        errno = 0;
        String s = scanner.getBuffer();
        intptr_t index;
        if(((index = s.find(_T("0x"))) >= 0) || ((index = s.find(_T("0X"))) >= 0)) {
          s.remove(index, 2);
        }
        result = _tcstoi128(s.cstr(), NULL, 16);
      }
    }
    break;
  case 8:
    { if(ch == '0') {
        ch = scanner.next();
        if(!iswodigit(ch)) {
          gotDigits = true;
        }
      }
      while(iswodigit(ch)) {
        ch = scanner.next();
        gotDigits = true;
      }
      if(gotDigits) {
        errno  = 0;
        result = _tcstoi128(scanner.getBuffer().cstr(), NULL, 8);
      }
    }
    break;
  }
  scanner.endScan(gotDigits && (errno == 0));
  if(gotDigits) {
    n = result;
  }
  return in;
}

template <typename IStreamType, typename CharType> IStreamType &getUint128(IStreamType &in, _uint128 &n) {
  IStreamScanner<IStreamType, CharType> scanner(in);
  CharType ch        = scanner.peek();
  bool     gotDigits = false;
  _uint128 result;

  switch(ch) {
  case '-':
  case '+':
    ch = scanner.next();
  }

  switch(scanner.radix()) {
  case 10:
    while(iswdigit(ch)) {
      ch = scanner.next();
      gotDigits = true;
    }
    if(gotDigits) {
      errno  = 0;
      result = _tcstoui128(scanner.getBuffer().cstr(), NULL, 10);
    }
    break;
  case 16:
    { if(ch == '0') {
        ch = scanner.next();
        if((ch == 'x') || (ch == 'X')) {
          ch = scanner.next();
        } else {
          gotDigits = true;
        }
      }
      while(iswxdigit(ch)) {
        ch = scanner.next();
        gotDigits = true;
      }
      if(gotDigits) {
        errno  = 0;
        String s = scanner.getBuffer();
        intptr_t index;
        if(((index = s.find(_T("0x"))) >= 0) || ((index = s.find(_T("0X"))) >= 0)) {
          s.remove(index, 2);
        }
        result = _tcstoui128(s.cstr(), NULL, 16);
      }
    }
    break;
  case 8:
    { if(ch == '0') {
        ch = scanner.next();
        if(!iswodigit(ch)) {
          gotDigits = true;
        }
      }
      while(iswodigit(ch)) {
        ch = scanner.next();
        gotDigits = true;
      }
      if(gotDigits) {
        errno  = 0;
        result = _tcstoui128(scanner.getBuffer().cstr(), NULL, 8);
      }
    }
    break;
  }
  scanner.endScan(gotDigits && (errno == 0));
  if(gotDigits) {
    n = result;
  }
  return in;
}

template <typename OStreamType, typename I128Type> OStreamType &putI128(OStreamType &out, const I128Type &n) {
  String buf;
  out << format128(buf, n, StreamParameters(out));
  if(out.flags() & ios::unitbuf) {
    out.flush();
  }
  return out;
}

istream &operator>>(istream &in, _int128 &n) {
  return getInt128<istream, char>(in, n);
}

ostream  &operator<<(ostream &out, const _int128 &n) {
  return putI128(out, n);
}

wistream &operator>>(wistream &in, _int128 &n) {
  return getInt128<wistream, wchar_t>(in, n);
}

wostream &operator<<(wostream &out, const _int128 &n) {
  return putI128(out, n);
}


istream  &operator>>(istream &in, _uint128 &n) {
  return getUint128<istream, char>(in, n);
}

ostream  &operator<<(ostream &out, const _uint128 &n) {
  return putI128(out, n);
}

wistream &operator>>(wistream &in, _uint128 &n) {
  return getUint128<wistream, wchar_t>(in, n);
}

wostream &operator<<(wostream &out, const _uint128 &n) {
  return putI128(out, n);
}
