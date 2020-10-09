#pragma once

#include <Singleton.h>
#include <StrStream.h>
#include <Math/Number.h>

namespace NumberIO {

using namespace std;
using namespace OStreamHelper;
using namespace IStreamHelper;

class NumberValueStreamScanner: public RegexIStream, public Singleton {
private:
  static StringArray getRegexLines();
  NumberValueStreamScanner() : RegexIStream(getRegexLines(), true), Singleton(__TFUNCTION__) {
  }
  DEFINESINGLETON(NumberValueStreamScanner)
};

template <typename IStreamType, typename CharType> IStreamType &getNumber(IStreamType &in, Number &n) {
  IStreamScanner<IStreamType, CharType> scanner(in);

  const RegexIStream &regex = NumberValueStreamScanner::getInstance();
  String buf;
  if(regex.match(in, &buf) < 0) {
    scanner.endScan(false);
    return in;
  }
  try {
    n = _tcstonum(buf.cstr(), NULL);
  } catch(...) {
    scanner.endScan(false);
    throw;
  }
  scanner.endScan();
  return in;
}

// always use decimal, if hexfloat, then use scientific
template<typename OStreamType> OStreamType &putNumber(OStreamType &out, const Number &n) {
  const FormatFlags flg = out.flags();
  if((flg & ios::floatfield) == ios::hexfloat) {
    out.setf(0, ios::fixed);
  }
  if((flg & ios::basefield) != ios::dec) {
    out.setf(0, ios::oct | ios::hex);
    out.setf(1, ios::dec);
  }
  switch(n.getType()) {
  case NUMBERTYPE_FLOAT   : out << (float   )n; break;
  case NUMBERTYPE_DOUBLE  : out << (double  )n; break;
  case NUMBERTYPE_DOUBLE80: out << (Double80)n; break;
  case NUMBERTYPE_RATIONAL: out << (Rational)n; break;
  default:
    out.flags(flg);
    throwInvalidArgumentException(__TFUNCTION__, _T("Unknown type:%d"), n.getType());
  }
  out.flags(flg);
  return out;
}

}; // namespace NumberIO
