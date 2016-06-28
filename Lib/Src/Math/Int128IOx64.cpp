#include "pch.h"

#ifdef IS64BIT

#include <Math/Int128.h>

class Int128Stream : public StrStream {
private:
  void addResult(const TCHAR *prefix, const TCHAR *buf);
public:
  Int128Stream(ostream &out) : StrStream(out) {
  }
  Int128Stream(wostream &out) : StrStream(out) {
  }

  friend Int128Stream &operator<<(Int128Stream &s, const _int128 &n);
  friend Int128Stream &operator<<(Int128Stream &s, const _uint128 &n);
};

void Int128Stream::addResult(const TCHAR *prefix, const TCHAR *buf) {
  const streamsize wantedWidth = getWidth();
  const streamsize resultWidth = _tcsclen(buf) + _tcsclen(prefix);
  String result;
  if(wantedWidth > resultWidth) {
    const streamsize fillerLength = wantedWidth - resultWidth;
    switch (getFlags() & ios::adjustfield) {
    case ios::left:
      result = format(_T("%s%s%s"), prefix, buf, spaceString(fillerLength).cstr());
      break;
    case ios::right:
      result = format(_T("%s%s%s"), spaceString(fillerLength).cstr(), prefix, buf);
      break;
    case ios::internal:
      result = format(_T("%s%s%s"), prefix, spaceString(fillerLength, getFiller()).cstr(), buf);
      break;
    }
  }
  append(result);
}

Int128Stream &operator<<(Int128Stream &s, const _int128 &n) {
  const TCHAR *prefix = _T("");
  TCHAR        buf[200];
  const int flags = s.getFlags();
  switch (flags & ios::basefield) {
  case ios::dec:
    { const bool negative = (n < 0);
      const _uint128 v = negative ? -n : n;
      _ui128tot(v, buf, 10);
      if ((flags & ios::showpos) || negative) {
        prefix = negative ? _T("-") : _T("+");
      }
      break;
    }
  case ios::hex:
    { const _uint128 v = n;
      if (flags & ios::showbase) prefix = _T("0x");
      _ui128tot(v, buf, 16);
      if (flags & ios::uppercase) {
        for (TCHAR *cp = buf; *cp; cp++) {
          if (_istlower(*cp)) *cp = _totupper(*cp);
        }
      }
    }
    break;
  case ios::oct:
    { const _uint128 v = n;
      if (flags & ios::showbase) {
        prefix = _T("0");
      }
      _ui128tot(v, buf, 8);
      break;
    }
  }
  s.addResult(prefix, buf);
  return s;
}

Int128Stream &operator<<(Int128Stream &s, const _uint128 &n) {
  const TCHAR *prefix = _T("");
  TCHAR        buf[200];
  const int flags = s.getFlags();
  switch (flags & ios::basefield) {
  case ios::dec:
    _ui128tot(n, buf, 10);
    if (flags & ios::showpos) {
      prefix = _T("+");
    }
    break;
  case ios::hex:
    { if (flags & ios::showbase) prefix = _T("0x");
      _ui128tot(n, buf, 16);
      if (flags & ios::uppercase) {
        for (TCHAR *cp = buf; *cp; cp++) {
          if (_istlower(*cp)) *cp = _totupper(*cp);
        }
      }
    }
    break;
  case ios::oct:
    if (flags & ios::showbase) {
      prefix = _T("0");
    }
    _ui128tot(n, buf, 8);
    break;
  }

  s.addResult(prefix, buf);
  return s;
}

#define peekChar(in,ch)           { ch = in.peek(); if(ch == EOF) in >> ch; }
#define appendCharGetNext(in, ch) { in >> ch; buf += ch; peekChar(in,ch);   }

template <class IStreamType, class CharType> void eatWhite(IStreamType &in) {
  CharType ch;
  for(;;in >> ch) {
	  peekChar(in, ch);
	  if(!iswspace(ch)) {
	    return;
	  }
  }
}

template <class IStreamType, class CharType> IStreamType &operator>> (IStreamType &in, _int128 &n) {
  if(in.ipfx(0)) {
    String   buf;
    CharType ch;
    bool     gotDigits = false;

    eatWhite<IStreamType, CharType>(in);
    peekChar(in, ch);

    const int flags = in.flags();
    switch (flags & ios::basefield) {
    case ios::dec:
      if (ch == '+' || ch =='-') appendCharGetNext(in, ch);
      while(iswdigit(ch)) {
        appendCharGetNext(in, ch);
        gotDigits = true;
      }
      break;
    case ios::hex:
      if (ch == '0') {
        appendCharGetNext(in, ch);
        if (ch == 'x' || ch == 'X') {
          appendCharGetNext(in, ch)
        }
        else {
          gotDigits = true;
        }
      }
      while (iswxdigit(ch)) {
        appendCharGetNext(in, ch);
        gotDigits = true;
      }
      break;
    case ios::oct:
      if (ch != '0') {
        buf += _T('0');
      }
      while(isodigit(ch)) {
        appendCharGetNext(in, ch);
        gotDigits = true;
      }
      break;
    }
    if(!gotDigits) {
      in.putback(ch);
      in.setf(ios::failbit);
      in.isfx();
      return in;
    }
    try {
      n = _int128(buf.cstr());
    } catch(...) {
      in.setf(ios::failbit);
      in.isfx();
      throw;
    }
    in.isfx();
  }
  return in;
}

template <class IStreamType, class CharType> IStreamType &operator>> (IStreamType &in, _uint128 &n) {
  if(in.ipfx(0)) {
    String   buf;
    CharType ch;
    bool     gotDigits = false;

    eatWhite<IStreamType, CharType>(in);
    peekChar(in, ch);

    const int flags = in.flags();
    switch (flags & ios::basefield) {
    case ios::dec:
      if (ch == '+') appendCharGetNext(in, ch);
      while(iswdigit(ch)) {
        appendCharGetNext(in, ch);
        gotDigits = true;
      }
      break;
    case ios::hex:
      if (ch == '0') {
        appendCharGetNext(in, ch);
        if (ch == 'x' || ch == 'X') {
          appendCharGetNext(in, ch)
        }
        else {
          gotDigits = true;
        }
      }
      while (iswxdigit(ch)) {
        appendCharGetNext(in, ch);
        gotDigits = true;
      }
      break;
    case ios::oct:
      if (ch != '0') {
        buf += _T('0');
      }
      while(isodigit(ch)) {
        appendCharGetNext(in, ch);
        gotDigits = true;
      }
      break;
    }
    if(!gotDigits) {
      in.putback(ch);
      in.setf(ios::failbit);
      in.isfx();
      return in;
    }
    try {
      n = _uint128(buf.cstr());
    } catch(...) {
      in.setf(ios::failbit);
      in.isfx();
      throw;
    }
    in.isfx();
  }
  return in;
}

template <class OStreamType> OStreamType &operator<<(OStreamType &out, const _int128 &n) {
  if(out.opfx()) {
    Int128Stream buf(out);
    buf << n;
    out << buf.cstr();
    out.osfx();
  }
  return out;
}

template <class OStreamType> OStreamType &operator<<(OStreamType &out, const _uint128 &n) {
  if(out.opfx()) {
    Int128Stream buf(out);
    buf << n;
    out << buf.cstr();
    out.osfx();
  }
  return out;
}

istream  &operator>>(istream &s, _int128 &n) {
  return ::operator>> <istream, char>(s, n);
}

ostream  &operator<<(ostream &s, const _int128 &n) {
  return ::operator<< <ostream>(s, n);
}

wistream &operator>>(wistream &s, _int128 &n) {
  return ::operator>> <wistream, wchar_t>(s, n);
}

wostream &operator<<(wostream &s, const _int128 &n) {
  return ::operator<< <wostream>(s, n);
}


istream  &operator>>(istream &s, _uint128 &n) {
  return ::operator>> <istream, char>(s, n);
}

ostream  &operator<<(ostream &s, const _uint128 &n) {
  return ::operator<< <ostream>(s, n);
}

wistream &operator>>(wistream &s, _uint128 &n) {
  return ::operator>> <wistream, wchar_t>(s, n);
}

wostream &operator<<(wostream &s, const _uint128 &n) {
  return ::operator<< <wostream>(s, n);
}

#endif
