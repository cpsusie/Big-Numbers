#pragma once

#include <StrStream.h>
#include <Singleton.h>
#include <RegexIStream.h>
#include <Date.h>

namespace DateTimeIO {

using namespace IStreamHelper;
using namespace std;

class DateStreamScanner : public RegexIStream, public Singleton {
private:
  static StringArray getRegexLines();
  DateStreamScanner() : RegexIStream(getRegexLines()), Singleton(__TFUNCTION__) {
  }
  DEFINESINGLETON(DateStreamScanner)
};

template<typename IStreamType, typename CharType> IStreamType &getDate(IStreamType &in, Date &d) {
  IStreamScanner<IStreamType, CharType> scanner(in);

  const RegexIStream &regex = DateStreamScanner::getInstance();
  String buf;
  if(regex.match(in, &buf) < 0) {
    scanner.endScan(false);
    return in;
  }
  try {
    d = Date(buf);
  } catch(...) {
    scanner.endScan(false);
    throw;
  }
  scanner.endScan();
  return in;
};

template<typename OStreamType> OStreamType &putDate(OStreamType &out, const Date &d) {
  char tmp[100];
  out << d.tostr(tmp);
  return out;
}

// -------------------------------------------------------------------------------------------------------

class TimeStreamScanner: public RegexIStream, public Singleton {
private:
  static StringArray getRegexLines();
  TimeStreamScanner() : RegexIStream(getRegexLines()), Singleton(__TFUNCTION__) {
  }
  DEFINESINGLETON(TimeStreamScanner)
};

template<typename IStreamType, typename CharType> IStreamType &getTime(IStreamType &in, Time &t) {
  IStreamScanner<IStreamType, CharType> scanner(in);

  const RegexIStream &regex = TimeStreamScanner::getInstance();
  String buf;
  if(regex.match(in, &buf) < 0) {
    scanner.endScan(false);
    return in;
  }
  try {
    t = Time(buf);
  } catch(...) {
    scanner.endScan(false);
    throw;
  }
  scanner.endScan();
  return in;
};

template<typename OStreamType> OStreamType &putTime(OStreamType &out, const Time &t) {
  char tmp[100];
  out << t.tostr(tmp, hhmmssSSS);
  return out;
}

// -------------------------------------------------------------------------------------------------------

class TimestampStreamScanner: public RegexIStream, public Singleton {
private:
  static StringArray getRegexLines();
  TimestampStreamScanner() : RegexIStream(getRegexLines()), Singleton(__TFUNCTION__) {
  }
  DEFINESINGLETON(TimestampStreamScanner)
};

template<typename IStreamType, typename CharType> IStreamType &getTimestamp(IStreamType &in, Timestamp &t) {
  IStreamScanner<IStreamType, CharType> scanner(in);

  const RegexIStream &regex = TimestampStreamScanner::getInstance();
  String buf;
  if(regex.match(in, &buf) < 0) {
    scanner.endScan(false);
    return in;
  }
  try {
    t = Timestamp(buf);
  } catch(...) {
    scanner.endScan(false);
    throw;
  }
  scanner.endScan();
  return in;
};

template<typename OStreamType> OStreamType &putTimestamp(OStreamType &out, const Timestamp &t) {
  char tmp[100];
  out << t.tostr(tmp, ddMMyyyyhhmmssSSS);
  return out;
}

}; // namespace DateTimeIO
