#include "pch.h"
#include <Math.h>
#include <Tokenizer.h>
#include <Date.h>

#pragma warning(disable : 4244)

Timestamp::Timestamp() {
  SYSTEMTIME st;
  GetLocalTime(&st);
  m_factor = getFactor(Date(st.wDay, st.wMonth, st.wYear), Time(st.wHour, st.wMinute, st.wSecond, st.wMilliseconds));
}

Timestamp::Timestamp(int day, int month, int year, int hour, int minute, int second, int millisecond) {
  m_factor = getFactor(Date(day, month, year), Time(hour, minute, second, millisecond));
}

Timestamp::Timestamp(const String &src) {
  init(src.cstr());
}

Timestamp::Timestamp(const TCHAR *src) {
  init(src);
}

void Timestamp::init(const TCHAR *src) {
  Tokenizer tok(src, _T(" "));
  String dateStr;
  if(tok.hasNext()) {
    dateStr = tok.next();
  }
  Date date(dateStr);
  Time time = tok.hasNext() ? Time(tok.next()) : Time(_T("00:00:00"));
  m_factor = getFactor(date, time);
  checkFactor(m_factor);
}

Timestamp::Timestamp(const Date &d, const Time &t) {
  m_factor = getFactor(d, t);
}

Timestamp::Timestamp(time_t tt) {
  m_factor = getFactor(Date(tt), Time(tt));
}

Timestamp::Timestamp(const Date &src) {
  m_factor = (__int64)src.m_factor * Time::getMaxFactor();
}

Timestamp::Timestamp(double d) { // ie type DATE
  m_factor = getFactor(Date(d), Time(fraction(d) * Time::getMaxFactor()));
}

__int64 Timestamp::getFactor(const Date &d, const Time &t) { // static
  return (__int64)d.m_factor * Time::getMaxFactor() + t.m_factor;
}

time_t Timestamp::gettime_t() const {
  int day, month, year;
  int hour, minute, second, millisecond;
  getDMY(day, month, year);
  getHMS(hour, minute, second, millisecond);

  struct tm tm;
  tm.tm_year  = year-1900;
  tm.tm_mon   = month-1;
  tm.tm_mday  = day;
  tm.tm_hour  = hour;
  tm.tm_min   = minute;
  tm.tm_sec   = second;
  tm.tm_isdst = -1; // then mktime will decide wether daylight saving is in effect
  return mktime(&tm); // yyyymmddhhmmss2time(year, month, day, hour, minute, second);
}

String Timestamp::cctime() { // static
  return cctime(getSystemTime());
}

String Timestamp::cctime(time_t tt) { // static
  char *s;
  s = ctime(&tt);
  s[24] = '\0';
  return s;
}

time_t Timestamp::getSystemTime() {  // static
  time_t tt;
  return(time(&tt));
}

Timestamp Timestamp::operator+(int count) const {
  Timestamp result(*this);
  result.m_factor += (__int64)count * Time::getMaxFactor();
  Timestamp::checkFactor(result.m_factor);
  return result;
}

const Timestamp &Timestamp::getMinTimestamp() {
  static const Timestamp minTimestamp(Date::getMinDate(), Time::getMinTime());
  return minTimestamp;
}

const Timestamp &Timestamp::getMaxTimestamp() {
  static const Timestamp maxTimestamp(Date::getMaxDate(), Time::getMaxTime());
  return maxTimestamp;
}

static void throwTimestampUnderflow() {
  throwException(_T("Timestamp becomes to small. First date in Gregorian calendar is %s"), Date::getMinDate().toString().cstr());
}

static void throwTimestampOverflow() {
  throwException(_T("Timestamp becomes to big. Last date is %s"), Date::getMaxDate().toString().cstr());
}

void Timestamp::checkFactor(__int64 factor) { // static 
  if(factor < getMinFactor()) {
    throwTimestampUnderflow();
  }
  if(factor > getMaxFactor()) {
    throwTimestampOverflow();
  }
}

Timestamp Timestamp::operator-(int count) const {
  Timestamp result(*this);
  result.m_factor -= (__int64)count * Time::getMaxFactor();
  Timestamp::checkFactor(result.m_factor);
  return result;
}

Timestamp &Timestamp::operator+=(int count) {
  return *this = *this + count;
}

Timestamp &Timestamp::operator-=(int count) {
  return *this = *this - count;
}

Timestamp Timestamp::operator++(int) {
  const Timestamp result = *this;
  *this += 1;
  return result;
}

Timestamp Timestamp::operator--(int) {
  const Timestamp result = *this;
  *this -= 1;
  return result;
}

static const double dayesPerYear = 146097.0 / 400; // 365.2425

double Timestamp::operator-(const Timestamp &r) const {
  return ((double)m_factor - r.m_factor) / Time::getMaxFactor();
}

static void throwInvalidTimeComponent(const TCHAR *function, TimeComponent c) {
  throwException(_T("Timestamp::%s:Invalid TimeComponent (=%d)."), function, c);
}

double diff(const Timestamp &from, const Timestamp &to, TimeComponent c) {
  switch(c) {
  case TMILLISECOND: return  to.m_factor - from.m_factor;
  case TSECOND     : return (to.m_factor - from.m_factor) / 1000;
  case TMINUTE     : return (to.m_factor - from.m_factor) / 60 / 1000;
  case THOUR       : return (to.m_factor - from.m_factor) / 3600 / 1000;
  case TDAYOFYEAR  : return (to.m_factor - from.m_factor) / 3600 / 1000 / 24;
  case TMONTH      : return (to.m_factor - from.m_factor) / 3600 / 1000 / 2  / dayesPerYear;
  case TYEAR       : return (to.m_factor - from.m_factor) / 3600 / 1000 / 24 / dayesPerYear;
  default          : throwInvalidTimeComponent(_T("diff"), c);
                     return to.m_factor - from.m_factor;
  }
}

Timestamp &Timestamp::add(TimeComponent c, int count) {
  switch(c) {
  case TMILLISECOND:
  case TSECOND     :
  case TMINUTE     :
  case THOUR       :
    { int s;
      switch(c) {
      case TMILLISECOND: s = count;               break;
      case TSECOND     : s = count * 1000;        break;
      case TMINUTE     : s = count * 1000 * 60;   break;
      case THOUR       : s = count * 1000 * 3600; break;
      }
      __int64 newFactor = m_factor + s;
      checkFactor(newFactor);
      m_factor = newFactor;
      break;
    }

  case TDAYOFMONTH :
  case TDAYOFYEAR  :
    { __int64 newFactor = m_factor + (__int64)count * Time::getMaxFactor();
      checkFactor(newFactor);
      m_factor = newFactor;
    }
    break;

  case TWEEK       :
    { __int64 newFactor = m_factor + 7 * (__int64)count * Time::getMaxFactor();
      checkFactor(newFactor);
      m_factor = newFactor;
    }
    break;

  case TMONTH      :
  case TYEAR       :
    { Date d = getDate();
      Time t = getTime();
      d.add(c, count);
      *this = Timestamp(d, t);
      break;
    }

  default          :
    throwInvalidTimeComponent(_T("add"), c);
  }
  return *this;
}

int Timestamp::get(TimeComponent c) {
  switch(c) {
  case TMILLISECOND: return getMilliSecond();
  case TSECOND     : return getSecond();
  case TMINUTE     : return getMinute();
  case THOUR       : return getHour();
  case TDAYOFMONTH : return getDayOfMonth();
  case TDAYOFYEAR  : return getDayOfYear();
  case TWEEK       : return getWeek();
  case TMONTH      : return getMonth();
  case TYEAR       : return getYear();
  default          : throwInvalidTimeComponent(_T("get"), c);
                     return 0;
  }
}

Timestamp &Timestamp::set(TimeComponent c, int value) {
  Time t = getTime();
  Date d = getDate();
  switch(c) {
  case TMILLISECOND: 
  case TSECOND     : 
  case TMINUTE     : 
  case THOUR       : 
    t.set(c, value);
    break;

  case TDAYOFMONTH : 
  case TDAYOFYEAR  :
  case TWEEK       : 
  case TMONTH      :
  case TYEAR       :
    d.set(c, value);
    break;
  default          :
    throwInvalidTimeComponent(_T("set"), c);
  }
  return *this = Timestamp(d, t);
}

bool operator==(const Timestamp &l, const Timestamp &r) {
  return l.m_factor == r.m_factor;
}

bool operator!=(const Timestamp &l, const Timestamp &r) {
  return l.m_factor != r.m_factor;
}

bool operator<(const Timestamp &l, const Timestamp &r) {
  return l.m_factor < r.m_factor;
}

bool operator<=(const Timestamp &l, const Timestamp &r) {
  return l.m_factor <= r.m_factor;
}

bool operator>(const Timestamp &l, const Timestamp &r) {
  return l.m_factor > r.m_factor;
}

bool operator>=(const Timestamp &l, const Timestamp &r) {
  return l.m_factor >= r.m_factor;
}

int timestampCmp(const Timestamp &l, const Timestamp &r) {
  return sign(l.m_factor - r.m_factor);
}

int Timestamp::getHour() const {
  return getTime().getHour();
}

int Timestamp::getMinute() const {
  return getTime().getMinute();
}

int Timestamp::getSecond() const {
  return getTime().getSecond();
}

int Timestamp::getMilliSecond() const {
  return getTime().getMilliSecond();
}

int Timestamp::getYear() const {
  return getDate().getYear();
}

int Timestamp::getMonth() const {
  return getDate().getMonth();
}

int Timestamp::getWeek() const {
  return getDate().getWeek();
}

int Timestamp::getDayOfMonth() const {
  return getDate().getDayOfMonth();
}

int Timestamp::getDayOfYear() const {
  return getDate().getDayOfYear();
}

WeekDay Timestamp::getWeekDay() const {
  return getDate().getWeekDay();
}

void Timestamp::getDMY(int &day, int &month, int &year) const {
  getDate().getDMY(day, month, year);
}

void Timestamp::getHMS(int &hour, int &minute, int &second, int &millisecond) const {
  getTime().getHMS(hour, minute, second, millisecond);
}

Date Timestamp::getDate() const {
  return Date((int)(m_factor/Time::getMaxFactor()));
}

Time Timestamp::getTime() const {
  return Time((int)(m_factor % Time::getMaxFactor()));
}

double Timestamp::getDATE() const {
  return getDate().getDATE() + fraction((double)m_factor / Time::getMaxFactor());
}

#define CASECH(ch, comp) case ch: {                         \
  int count = 1, scale = 10;                                \
  for(cp++; *cp == ch; cp++) {                              \
    count++;                                                \
    scale *= 10;                                            \
  }                                                         \
  String tmp = ::format(_T("%0*d"), count, (comp) % scale); \
  _tcscpy(t, tmp.cstr());                                    \
  t += tmp.length();                                        \
}                                                           \
break

TCHAR *Timestamp::tostr(TCHAR *dst, const String &format) const {
  int year, month, day, hour, minute, second, millisecond;
  getDMY(day, month, year);
  getHMS(hour, minute, second, millisecond);
  TCHAR *t = dst;
  for(const TCHAR *cp = format.cstr(); *cp;) {
    switch(*cp) {
    CASECH('y', year          );
    CASECH('M', month         );
    CASECH('w', getWeek()     );
    CASECH('d', day           );
    CASECH('D', getDayOfYear());
    CASECH('h', hour          );
    CASECH('m', minute        );
    CASECH('s', second        );
    CASECH('S', millisecond   );
    default :
      *(t++) = *(cp++);
      break;
    }
  }
  *t = 0;
  return dst;
}

String Timestamp::toString(const String &format) const {
  TCHAR tmp[1024];
  return tostr(tmp, format);
}

unsigned long Timestamp::hashCode() const {
  return uint64Hash(m_factor);
}

Timestamp::Timestamp(const SYSTEMTIME &st) {
  m_factor = getFactor(Date(st.wDay, st.wMonth, st.wYear), Time(st.wHour, st.wMinute, st.wSecond, st.wMilliseconds));
  TIME_ZONE_INFORMATION timeZoneInfo;
  DWORD ret = GetTimeZoneInformation(&timeZoneInfo);
  add(TMINUTE, -timeZoneInfo.Bias-timeZoneInfo.DaylightBias);
}

Timestamp::operator SYSTEMTIME() const {
  TIME_ZONE_INFORMATION timeZoneInfo;
  DWORD ret = GetTimeZoneInformation(&timeZoneInfo);
  Timestamp uts = *this;
  uts.add(TMINUTE, timeZoneInfo.Bias+timeZoneInfo.DaylightBias);
  
  SYSTEMTIME ust;
  int day, month, year, hour, minute, second, ms;
  uts.getDMY(day, month, year);
  uts.getHMS(hour, minute, second, ms);
  ust.wYear         = year;
  ust.wMonth        = month;
  ust.wDay          = day;
  ust.wHour         = hour;
  ust.wMinute       = minute;
  ust.wSecond       = second;
  ust.wMilliseconds = ms;
  ust.wDayOfWeek    = ((int)uts.getWeekDay() + 1) % 7;

  return ust;
}
