#include "pch.h"
#include <Math/Rational.h>

using namespace std;

StrStream &operator<<(StrStream &stream, const Rational &r) {
  long flags = stream.getFlags();

  String result = r.toString();
  if((flags & ios::showpos) && r.isPositive()) {
    result = _T("+") + result;
  }

  int fillerLength = stream.getWidth() - result.length();
  if(fillerLength <= 0) {
    stream.append(result);
  } else if(flags & ios::left) {
    stream.append(result).append(spaceString(fillerLength));
  } else {// right align
    stream.append(spaceString(fillerLength)).append(result);
  }
  return stream;
}

#define peekChar(in,ch)          { ch = in.peek(); if(ch == EOF) in >> ch; }
#define appendCharGetNext(in,ch) { in >> ch; buf += ch; peekChar(in,ch);   }

template <class IStreamType, class CharType> void eatWhite(IStreamType &in) {
  CharType ch;
  for(;;in >> ch) {
    peekChar(in, ch);
    if(!_istspace(ch)) {
      return;
    }
  }
}

template <class IStreamType, class CharType> IStreamType &operator>>(IStreamType &in, Rational &r) {
  if(in.ipfx(0)) {
    String   buf;
    CharType ch;
    bool     gotDigits = false;

    eatWhite<IStreamType, CharType>(in);
    peekChar(in, ch);
    if((ch == _T('-')) || (ch == _T('+'))) {
      appendCharGetNext(in, ch);
    }
    while(_istdigit(ch)) {
      appendCharGetNext(in, ch);
      gotDigits = true;
    }
    if(ch == _T('/')) {
      appendCharGetNext(in, ch);
      if((ch == _T('-')) || (ch == _T('+'))) {
        appendCharGetNext(in, ch);
      }
      while(_istdigit(ch)) {
        appendCharGetNext(in, ch);
        gotDigits = true;
      }
    }
    if(!gotDigits) {
      in.putback(ch);
      in.clear(ios::failbit);
      in.isfx();
      return in;
    }
    try {
      r = Rational(buf.cstr());
    } catch(...) {
      in.clear(ios::failbit);
      in.isfx();
      throw;
    }
    in.isfx();
  }
  return in;
}

template <class OStreamType> OStreamType &operator<<(OStreamType &out, const Rational &r) {
  if(out.opfx()) {
    StrStream stream(out);
    stream << r;
    out << stream.cstr();
    if(out.flags() & ios::unitbuf) {
      out.flush();
    }
    out.osfx();
  }
  return out;
}

tistream &operator>>(tistream &in, Rational &r) {
  return ::operator>> <tistream,TCHAR> (in, r);
}

tostream &operator<<(tostream &out, const Rational &r) {
  return operator<< <tostream>(out, r);
}

