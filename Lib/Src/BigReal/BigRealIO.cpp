#include "pch.h"
#include <String.h>
#include <ctype.h>

using namespace std;

#include "../Math/IStreamUtil.h"

template <class IStreamType, class CharType> IStreamType &operator>>(IStreamType &in, BigReal &x) {
  SETUPISTREAM(in)

  if(FloatingValueStreamScanner::getInstance().match(in, &buf) < 0) {
    ungetbuf(in);
    RESTOREISTREAM(in);
    in.setstate(ios::failbit);
    return in;
  }
  try {
    x = BigReal(buf.cstr(), x.getDigitPool());
  } catch (...) {
    RESTOREISTREAM(in);
    in.setstate(ios::failbit);
    throw;
  }
  RESTOREISTREAM(in);
  return in;
}

template <class IStreamType, class CharType> IStreamType &operator>>(IStreamType &in, BigInt &n) {
  SETUPISTREAM(in)

  if(IntegerValueStreamScanner::getInstance().match(in, &buf) < 0) {
    ungetbuf(in);
    RESTOREISTREAM(in);
    in.setstate(ios::failbit);
    return in;
  }
  try {
    n = BigInt(buf.cstr(), n.getDigitPool());
  } catch (...) {
    RESTOREISTREAM(in);
    in.setstate(ios::failbit);
    throw;
  }
  RESTOREISTREAM(in);
  return in;
}

template <class OStreamType> OStreamType &operator<<(OStreamType &out, const BigReal &x) {
  BigRealStream buf(out);
  buf << x;
  out << buf.cstr();
  return out;
}

template <class OStreamType> OStreamType &operator<<(OStreamType &out, const BigInt &n) {
  BigRealStream buf(out);
  buf << n;
  out << buf.cstr();
  return out;
}

template <class OStreamType> OStreamType &operator<<(OStreamType &out, const FullFormatBigReal &x) {
  BigRealStream buf(out);
  buf << x;
  out << buf.cstr();
  return out;
}

tistream &operator>>(tistream &in, BigReal &x) {
  return ::operator>><tistream,TCHAR>(in, x);
}

tistream &operator>>(tistream &in, BigInt &n) {
  return ::operator>><tistream, TCHAR>(in, n);
}

tostream &operator<<(tostream &out, const BigReal &x) {
  return ::operator<< <tostream>(out, x);
}

tostream &operator<<(tostream &out, const BigInt &n) {
  return ::operator<< <tostream>(out, n);
}

tostream &operator<<(tostream &out, const FullFormatBigReal &x) {
  return ::operator<< <tostream>(out, x);
}

void BigReal::print(FILE *f, bool spacing) const {
  BigRealStream buf;
  if(spacing) {
    buf.setSpaceChar(_T(':'));
  }
  buf << *this;
  _ftprintf(f,_T("%s"), buf.cstr());
}

void FullFormatBigReal::print(FILE *f, bool spacing) const {
  BigRealStream buf;
  if(spacing) {
    buf.setSpaceChar(_T(':'));
  }
  buf << *this;
  _ftprintf(f,_T("%s"), buf.cstr());
}

void BigInt::print(FILE *f, bool spacing) const {
  BigRealStream buf;
  if(spacing) {
    buf.setSpaceChar(_T(':'));
  }
  buf << *this;
  _ftprintf(f,_T("%s"), buf.cstr());
}

void BigReal::dump(FILE *f) const {
  _ftprintf(f, _T("["));
  if(isNegative()) {
    _ftprintf(f, _T("-"));
  }
  BRExpoType d = m_expo;
  for(const Digit *digit = m_first; digit; digit = digit->next) {
#ifdef IS32BIT
    _ftprintf(f,_T("%0*.*lu "), LOG10_BIGREALBASE,LOG10_BIGREALBASE,digit->n);
#else
    _ftprintf(f,_T("%0*.*llu "), LOG10_BIGREALBASE,LOG10_BIGREALBASE,digit->n);
#endif
  }
#ifdef IS32BIT
  _ftprintf(f,_T(" [expo,low,length]:[%4d,%4d,%4d]]"), m_expo,m_low,getLength());
#else
  _ftprintf(f,_T(" [expo,low,length]:[%4lld,%4lld,%4lld]]"), m_expo,m_low,getLength());
#endif
}
