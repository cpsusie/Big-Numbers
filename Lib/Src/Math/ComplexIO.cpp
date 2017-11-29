#include "pch.h"
#include <Math/Complex.h>

StrStream &operator<<(StrStream &stream, const Complex &c) {
  if(c.im == 0) {
    stream << c.re;
  } else {
    stream.append(_T("("));
    stream << c.re;
    stream.append(_T(","));
    stream << c.im;
    stream.append(_T(")"));
  }
  return stream;
}

#define peekChar(in,ch) { ch = in.peek(); if(ch == EOF) in >> ch; }

template <class IStreamType, class CharType> void eatWhite(IStreamType &in) {
  CharType ch;
  for(;;in >> ch) {
    peekChar(in, ch);
    if(!_istspace(ch)) {
      return;
    }
  }
}

#define EATWHITE(stream) eatWhite<IStreamType, CharType>(stream);

template <class IStreamType, class CharType> IStreamType &operator>>(IStreamType &in, Complex &c) {
  /* inputformat for Complex;
      re
      (re)
      (re,im)
  */

  if(in.ipfx(0)) {
    Real re = 0, im = 0;
    CharType ch = 0;
    EATWHITE(in);
    peekChar(in, ch);
    if(ch == '(') {
      in >> ch;
      EATWHITE(in);
      in >> re;
      EATWHITE(in);
      peekChar(in, ch);
      if(ch == ',')  {
        in >> ch;
        EATWHITE(in);
        in >> im;
        eatWhite<IStreamType, CharType>(in);
      }
      peekChar(in, ch);
      if(ch == ')') {
        in >> ch;
      } else {
        in.clear(std::ios::badbit);
      }
    } else {
      in >> re;
    }
    if(in) {
      c = Complex(re,im);
    }
    in.isfx();
  }
  return in;
}


template <class OStreamType> OStreamType &operator<<(OStreamType &out, const Complex &c) {
  if(out.opfx()) {
    StrStream stream(out);
    stream << c;
    out << stream.cstr();
    out.osfx();
  }
  return out;
}

istream &operator>>(istream &in, Complex &c) {
  return ::operator>> <istream, char>(in, c);
}

ostream &operator<<(ostream &out, const Complex &c) {
  return ::operator<< <ostream>(out, c);
}

std::wistream &operator>>(std::wistream &in, Complex &c) {
  return ::operator>> <std::wistream, wchar_t>(in, c);
}

std::wostream &operator<<(std::wostream &out, const Complex &c) {
  return ::operator<< <std::wostream>(out, c);
}
