#pragma once

#include <Unicode.h>
#include <StrStream.h>

using namespace std;
using namespace OStreamHelper;
using namespace IStreamHelper;

#define _BR_DEFAULT_GROUPSIZE 8
class BigRealStreamParameters : public StreamParameters {
private:
  TCHAR m_separatorChar;
  UINT  m_groupSize;
  // Assume x._isnormal()
  size_t findMaxDecimalDigitCount(const BigReal &x) const;

public:
  BigRealStreamParameters(StreamSize prec = 6, StreamSize width = 0, FormatFlags flags = 0, TCHAR separatorChar = 0, UINT groupSize=_BR_DEFAULT_GROUPSIZE)
    : StreamParameters(prec, width, flags)
    , m_separatorChar(separatorChar      )
    , m_groupSize(groupSize              )
  {
  }
  template<typename P> BigRealStreamParameters(const P &p, TCHAR separatorChar=0, UINT groupSize=_BR_DEFAULT_GROUPSIZE)
    : StreamParameters(p            )
    , m_separatorChar( separatorChar)
    , m_groupSize(     groupSize    )
  {
  }

  inline TCHAR separator(TCHAR separatorChar) {
    const TCHAR old = m_separatorChar;  m_separatorChar = separatorChar; return old;
  }

  inline TCHAR separator() const {
    return m_separatorChar;
  }
  String &getFormattedString(String &dst, const BigReal &x);
};

template <typename IStreamType, typename CharType> IStreamType &getBigReal(IStreamType &in, BigReal &x) {
  IStreamScanner<IStreamType, CharType> scanner(in);
  String buf;
  if(DecFloatValueStreamScanner::getInstance().match(in, &buf) < 0) {
    scanner.endScan(false);
    return in;
  }
  try {
    x = BigReal(buf.cstr(), x.getDigitPool());
  } catch(...) {
    scanner.endScan(false);
    throw;
  }
  scanner.endScan();
  return in;
}

template <typename OStreamType> OStreamType &putBigReal(OStreamType &out, const BigReal &x, TCHAR separatorChar=0, UINT groupSize=_BR_DEFAULT_GROUPSIZE) {
  String tmp;
  out << BigRealStreamParameters(out, separatorChar,groupSize).getFormattedString(tmp, x);
  return out;
}

template <typename IStreamType, typename CharType> IStreamType &getBigInt(IStreamType &in, BigInt &n) {
  IStreamScanner<IStreamType, CharType> scanner(in);
  String buf;
  if(DecIntValueStreamScanner::getInstance().match(in, &buf) < 0) {
    scanner.endScan(false);
    return in;
  }
  try {
    n = BigInt(buf.cstr(), n.getDigitPool());
  } catch(...) {
    scanner.endScan(false);
    throw;
  }
  scanner.endScan();
  return in;
}

template <typename OStreamType> OStreamType &putBigInt(OStreamType &out, const BigInt &x, TCHAR separatorChar=0, UINT groupSize=_BR_DEFAULT_GROUPSIZE) {
  const StreamSize  oldprec = out.precision();
  const FormatFlags oldflags = out.flags();
  out.flags((oldflags | ios::fixed) & ~(ios::scientific|ios::showpoint));
  out.precision(0);
  putBigReal(out, x, separatorChar,groupSize);
  out.flags(oldflags);
  out.precision(oldprec);
  return out;
}

template <typename OStreamType> OStreamType &putFullFormatBigReal(OStreamType &out, const BigReal &x, TCHAR separatorChar=0, UINT groupSize=_BR_DEFAULT_GROUPSIZE) {
  const StreamSize  oldprec  = out.precision();
  const FormatFlags oldflags = out.flags();
  if(isfinite(x)) {
    BRExpoType e = BigReal::getExpo10(x);
    intptr_t   autoprec;
    if(e < 0) {
      autoprec = x.getLength() * BIGREAL_LOG10BASE;
      if(e % BIGREAL_LOG10BASE == 0) {
        autoprec -= BIGREAL_LOG10BASE;
      } else {
        autoprec -= -e % BIGREAL_LOG10BASE;
      }
    } else {
      autoprec = (x.getLength() - 1) * BIGREAL_LOG10BASE + (e % BIGREAL_LOG10BASE);
    }

    if(x.getLength() > 1) {
      for(BRDigitType last = x.getLastDigit(); last % 10 == 0; last /= 10) autoprec--;
    }
    out.flags((oldflags | ios::scientific) & ~ios::fixed);
    out.precision(autoprec);
  }
  putBigReal(out, x, separatorChar,groupSize);
  out.flags(    oldflags);
  out.precision(oldprec);
  return out;
}

template <typename OStreamType> OStreamType &putBigRational(OStreamType &out, const BigRational &x, TCHAR separatorChar=0, UINT groupSize=_BR_DEFAULT_GROUPSIZE) {
  DigitPool *pool = x.getDigitPool();
  if(!isfinite(x)) {
    char tmp[100];
    out << formatUndefined(tmp, _fpclass(x), (out.flags() & ios::uppercase) != 0);
  } else if(x._isinteger()) {
    putBigInt(out, x.getNumerator(), separatorChar,groupSize);
  } else {
    stringstream tmpstream;
    tmpstream.flags(out.flags());
    putBigInt(tmpstream, x.getNumerator(), separatorChar,groupSize);
    tmpstream.flags(0);
    tmpstream << "/";
    tmpstream.flags(out.flags() & ~(ios::showpos));
    putBigInt(tmpstream, x.getDenominator(), separatorChar,groupSize);
    out << tmpstream.str().c_str();
  }
  return out;
}

template <typename IStreamType, typename CharType> IStreamType &getBigRational(IStreamType &in, BigRational &x) {
  DigitPool        *pool = x.getDigitPool();
  const FormatFlags flgs = in.flags();
  BigInt            num(pool->_0()), den(pool->_1());
  in >> num;
  if(in) {
    in.flags(flgs & ~ios::skipws);
    if(in.peek() == '/') {
      in.get();
      in >> den;
    }
    in.flags(flgs);
  }
  if(in) {
    try {
      x = BigRational(num, den);
    } catch(...) {
      in.setstate(ios::failbit);
      in.flags(flgs);
      throw;
    }
  }
  in.flags(flgs);
  return in;
}
