#include "pch.h"

#include <Math/Int128.h>

#include <comdef.h>
#include <atlconv.h>

#define BASICSTRING basic_string<Ctype, char_traits<Ctype>, allocator<Ctype> >

template<class Ctype> class StringTemplate : public BASICSTRING {
public:
  StringTemplate() {
  }
  StringTemplate(const char    *str);
  StringTemplate(const wchar_t *str);

  StringTemplate &operator+=(const StringTemplate &str);
  StringTemplate &operator+=(char    ch);
  StringTemplate &operator+=(wchar_t ch);
};

template<class Ctype> StringTemplate<Ctype>::StringTemplate(const char *str) {
  if (sizeof(Ctype) == sizeof(char)) {
    BASICSTRING::operator=((const Ctype*)str);
  } else {
    USES_CONVERSION;
    BASICSTRING::operator=((const Ctype*)A2W(str));
  }
}

template<class Ctype> StringTemplate<Ctype>::StringTemplate(const wchar_t *str) {
  if (sizeof(Ctype) == sizeof(wchar_t)) {
    BASICSTRING::operator=((const Ctype*)str);
  } else {
    USES_CONVERSION;
    BASICSTRING::operator=((const Ctype*)W2A(str));
  }
}

template<class Ctype> StringTemplate<Ctype> operator+(const StringTemplate<Ctype> &s1, const StringTemplate<Ctype> &s2) {
  return StringTemplate<Ctype>((BASICSTRING)s1 + (BASICSTRING)s2)
}

template<class Ctype> StringTemplate<Ctype> &StringTemplate<Ctype>::operator+=(const StringTemplate &str) {
  BASICSTRING::operator+=(str.c_str());
  return *this;
}

template<class Ctype> StringTemplate<Ctype> &StringTemplate<Ctype>::operator+=(char    ch) {
  BASICSTRING::operator+=((Ctype)ch);
  return *this;
}

template<class Ctype> StringTemplate<Ctype> &StringTemplate<Ctype>::operator+=(wchar_t ch) {
  BASICSTRING::operator+=((Ctype)ch);
  return *this;
}

#define MAXTMPSIZE 1000
template<class Ctype> StringTemplate<Ctype> getFillerString(streamsize l, Ctype ch = ' ') {
  StringTemplate<Ctype> result;
  if (l <= MAXTMPSIZE) {
    Ctype tmp[MAXTMPSIZE+1], *cp = tmp + l;
    for(*cp = 0; cp-- > tmp;) *cp = ch;
    result = tmp;
  }
  else {
    Ctype *tmp = new Ctype[(int)l + 1], *cp = tmp + l;
    for(*cp = 0; cp-- > tmp;) *cp = ch;
    result = tmp;
    delete[] tmp;
  }
  return result;
}

template<class Ctype> class Int128Stream : public StringTemplate<Ctype>, public StreamParameters {
private:
  Int128Stream &addResult(const StringTemplate<Ctype> &prefix, const char *buf);
public:
  Int128Stream(ostream &out) : StreamParameters(out) {
  }
  Int128Stream(wostream &out) : StreamParameters(out) {
  }

  Int128Stream &operator<<(const _int128 &n);
  Int128Stream &operator<<(const _uint128 &n);
};


template<class Ctype> Int128Stream<Ctype> &Int128Stream<Ctype>::addResult(const StringTemplate<Ctype> &prefix, const char *buf) {
  const streamsize wantedWidth = getWidth();
  const streamsize resultWidth = strlen(buf) + prefix.length();
  StringTemplate<Ctype> result;
  if (wantedWidth > resultWidth) {
    const streamsize fillerLength = wantedWidth - resultWidth;
    switch (getFlags() & ios::adjustfield) {
    case ios::left:
      result =  prefix;
      result += buf;
      result += getFillerString<Ctype>(fillerLength);
      break;
    case ios::right:
      result =  getFillerString<Ctype>(fillerLength);
      result += prefix;
      result += buf;
      break;
    case ios::internal:
      result = prefix;
      result += getFillerString<Ctype>(fillerLength, prefix.length()?'0':(Ctype)getFiller());
      result += buf;
      break;
    default: // do as ios::left
      result =  prefix;
      result += buf;
      result += getFillerString<Ctype>(fillerLength);
      break;
    }
  } else {
    result =  prefix;
    result += buf;
  }
  *this += result;
  return *this;
}

template<class Ctype> Int128Stream<Ctype> &Int128Stream<Ctype>::operator<<(const _int128 &n) {
  StringTemplate<Ctype> prefix;
  char                  buf[200];
  const int             flags = getFlags();
  switch (flags & ios::basefield) {
  case ios::dec:
    { const bool negative = (n < 0);
      const _uint128 v = negative ? -n : n;
      _ui128toa(v, buf, 10);
      if (negative) {
        prefix = "-";
      } else if (flags & ios::showpos) {
        prefix = "+";
      }
      break;
    }
  case ios::hex:
    { const _uint128 v = n;
      if (flags & ios::showbase) prefix = "0x";
      _ui128toa(v, buf, 16);
      if (flags & ios::uppercase) {
        for (char *cp = buf; *cp; cp++) {
          if (iswlower(*cp)) *cp = _toupper(*cp);
        }
      }
    }
    break;
  case ios::oct:
    { const _uint128 v = n;
      if (flags & ios::showbase) {
        prefix = "0";
      }
      _ui128toa(v, buf, 8);
      break;
    }
  }
  return addResult(prefix, buf);
}

template<class Ctype> Int128Stream<Ctype> &Int128Stream<Ctype>::operator<<(const _uint128 &n) {
  StringTemplate<Ctype> prefix;
  char                  buf[200];
  const int             flags = getFlags();
  switch (flags & ios::basefield) {
  case ios::dec:
    _ui128toa(n, buf, 10);
    if (flags & ios::showpos) {
      prefix = "+";
    }
    break;
  case ios::hex:
    { if (flags & ios::showbase) prefix = "0x";
      _ui128toa(n, buf, 16);
      if (flags & ios::uppercase) {
        for (char *cp = buf; *cp; cp++) {
          if (iswlower(*cp)) *cp = _toupper(*cp);
        }
      }
    }
    break;
  case ios::oct:
    if (flags & ios::showbase) {
      prefix = "0";
    }
    _ui128toa(n, buf, 8);
    break;
  }
  return addResult(prefix, buf);
}

#define peekChar(in,ch)           { ch = in.peek(); if(ch == EOF) in >> ch; }
#define appendCharGetNext(in, ch) { in >> ch; buf += ch; peekChar(in,ch);   }
#define skipChar(in, ch)          { in >> ch; peekChar(in, ch);             }

template <class IStreamType, class Ctype> void eatWhite(IStreamType &in) {
  Ctype ch;
  for(;;in >> ch) {
	  peekChar(in, ch);
	  if(!iswspace(ch)) {
	    return;
	  }
  }
}

template <class IStreamType, class Ctype> IStreamType &operator>> (IStreamType &in, _int128 &n) {
  if(in.ipfx(0)) {
    StringTemplate<Ctype> buf;
    Ctype                 ch;
    bool                  gotDigits = false;
    const int             flags = in.flags();
    if (flags & ios::skipws) {
      eatWhite<IStreamType, Ctype>(in);
    }
    peekChar(in, ch);

    switch (flags & ios::basefield) {
    case ios::dec:
      if (ch == '+' || ch =='-') appendCharGetNext(in, ch);
      while(iswdigit(ch)) {
        appendCharGetNext(in, ch);
        gotDigits = true;
      }
      if (gotDigits) {
        n.parseDec(buf.c_str());
      }
      break;
    case ios::hex:
      if (ch == '0') {
        skipChar(in, ch);
        if ((ch == 'x') || (ch == 'X')) {
          skipChar(in, ch);
        }
        else {
          buf += '0';
          gotDigits = true;
        }
      }
      while (iswxdigit(ch)) {
        appendCharGetNext(in, ch);
        gotDigits = true;
      }
      if (gotDigits) {
        n.parseHex(buf.c_str());
      }
      break;
    case ios::oct:
      if (ch != '0') {
        buf += '0';
      }
      while(iswodigit(ch)) {
        appendCharGetNext(in, ch);
        gotDigits = true;
      }
      if (gotDigits) {
        n.parseOct(buf.c_str());
      }
      break;
    }
    if(!gotDigits) {
      in.putback(ch);
      in.setf(ios::failbit);
    }
    in.isfx();
  }
  return in;
}

template <class IStreamType, class Ctype> IStreamType &operator>> (IStreamType &in, _uint128 &n) {
  if(in.ipfx(0)) {
    StringTemplate<Ctype> buf;
    Ctype                 ch;
    bool                  gotDigits = false;
    const int             flags = in.flags();
    if (flags & ios::skipws) {
      eatWhite<IStreamType, Ctype>(in);
    }
    peekChar(in, ch);

    switch (flags & ios::basefield) {
    case ios::dec:
      if (ch == '+') appendCharGetNext(in, ch);
      while(iswdigit(ch)) {
        appendCharGetNext(in, ch);
        gotDigits = true;
      }
      if (gotDigits) {
        n.parseDec(buf.c_str());
      }
      break;
    case ios::hex:
      if (ch == '0') {
        skipChar(in, ch);
        if (ch == 'x' || ch == 'X') {
          skipChar(in, ch)
        } else {
          buf += '0';
          gotDigits = true;
        }
      }
      while (iswxdigit(ch)) {
        appendCharGetNext(in, ch);
        gotDigits = true;
      }
      if (gotDigits) {
        n.parseHex(buf.c_str());
      }
      break;
    case ios::oct:
      if (ch != '0') {
        buf += '0';
      }
      while(iswodigit(ch)) {
        appendCharGetNext(in, ch);
        gotDigits = true;
      }
      if (gotDigits) {
        n.parseOct(buf.c_str());
      }
      break;
    }
    if(!gotDigits) {
      in.putback(ch);
      in.setf(ios::failbit);
    }
    in.isfx();
  }
  return in;
}

template <class OStreamType, class Ctype> OStreamType &operator<<(OStreamType &out, const _int128 &n) {
  if(out.opfx()) {
    Int128Stream<Ctype> buf(out);
    buf << n;
    out << buf.c_str();
    if(out.flags() & ios::unitbuf) {
      out.flush();
    }
    out.osfx();
  }
  return out;
}

template <class OStreamType, class Ctype> OStreamType &operator<<(OStreamType &out, const _uint128 &n) {
  if(out.opfx()) {
    Int128Stream<Ctype> buf(out);
    buf << n;
    out << buf.c_str();
    if(out.flags() & ios::unitbuf) {
      out.flush();
    }
    out.osfx();
  }
  return out;
}

istream  &operator>>(istream &s, _int128 &n) {
  return ::operator>> <istream, char>(s, n);
}

ostream  &operator<<(ostream &s, const _int128 &n) {
  return ::operator<< <ostream, char>(s, n);
}

wistream &operator>>(wistream &s, _int128 &n) {
  return ::operator>> <wistream, wchar_t>(s, n);
}

wostream &operator<<(wostream &s, const _int128 &n) {
  return ::operator<< <wostream, wchar_t>(s, n);
}


istream  &operator>>(istream &s, _uint128 &n) {
  return ::operator>> <istream, char>(s, n);
}

ostream  &operator<<(ostream &s, const _uint128 &n) {
  return ::operator<< <ostream, char>(s, n);
}

wistream &operator>>(wistream &s, _uint128 &n) {
  return ::operator>> <wistream, wchar_t>(s, n);
}

wostream &operator<<(wostream &s, const _uint128 &n) {
  return ::operator<< <wostream, wchar_t>(s, n);
}
