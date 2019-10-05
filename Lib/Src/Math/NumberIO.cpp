#include "pch.h"
#include <StrStream.h>
#include <Math/Number.h>

using namespace std;

class NumberValueStreamScanner : public RegexIStream {
private:
  static StringArray getRegexLines();
  NumberValueStreamScanner() : RegexIStream(getRegexLines(), true) {
  }
public:
  static const RegexIStream &getInstance() {
    static NumberValueStreamScanner s_instance;
    return s_instance;
  }
};

static const char *numberPatterns[] = {
  "[+\\-]?[0-9]*\\(\\.[0-9]+\\)\\(e[+\\-]?[0-9]+\\)?"    // float
 ,"[+\\-]?[0-9]+\\(\\.[0-9]*\\)?\\(e[+\\-]?[0-9]+\\)?"   // float
 ,"[+\\-]?[0-9]+\\(/[0-9]+\\)?"                          // rational
 ,NULL
};

StringArray NumberValueStreamScanner::getRegexLines() { // static
  return StringArray(numberPatterns);
}

template <class IStreamType, class CharType> IStreamType &getNumber(IStreamType &in, Number &n) {
  IStreamScanner<IStreamType, CharType> scanner(in);

  const RegexIStream &regex = NumberValueStreamScanner::getInstance();
  String buf;
  if(regex.match(in, &buf) < 0) {
    scanner.endScan(false);
    return in;
  }
  try {
    n = _tcstonum(buf.cstr(), NULL);
  }
  catch (...) {
    scanner.endScan(false);
    throw;
  }
  scanner.endScan();
  return in;
}

// always use decimal, if hexfloat, then use scientific
template<class OStreamType> OStreamType &putNumber(OStreamType &out, const Number &n) {
  const FormatFlags flg = out.flags();
  if((flg & ios::floatfield) == ios::hexfloat) {
    out.setf(0, ios::fixed);
  }
  if((flg & ios::basefield) != ios::dec) {
    out.setf(0, ios::oct | ios::hex);
    out.setf(1, ios::dec);
  }
  switch(n.getType()) {
  case NUMBERTYPE_FLOAT    : out << getFloat(   n); break;
  case NUMBERTYPE_DOUBLE   : out << getDouble(  n); break;
  case NUMBERTYPE_DOUBLE80 : out << getDouble80(n); break;
  case NUMBERTYPE_RATIONAL : out << getRational(n); break;
  default                  :
    out.flags(flg);
    throwInvalidArgumentException(__TFUNCTION__, _T("Unknown type:%d"), n.getType());
  }
  out.flags(flg);
  return out;
}

ostream  &operator<<(ostream  &out, const Number &n) {
  return putNumber(out, n);
}

wostream &operator<<(wostream &out, const Number &n) {
  return putNumber(out, n);
}

istream  &operator>>(istream  &in, Number &n) {
  return getNumber<istream, char>(in, n);
}

wistream &operator>>(wistream &in, Number &n) {
  return getNumber<wistream, wchar_t>(in, n);
}

String toString(const Number &n, StreamSize precision, StreamSize width, FormatFlags flags) {
  return (TowstringStream(precision, width, flags) << n).str().c_str();
}

