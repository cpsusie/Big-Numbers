#include "pch.h"
#include <Math.h>
#include <Tokenizer.h>
#include <Date.h>

#pragma warning(disable : 4244)

Timestamp::Timestamp() {
  SYSTEMTIME st;
  GetLocalTime(&st);
  *this = st;
}

Timestamp::operator SYSTEMTIME() const {
  SYSTEMTIME st;
  int day, month, year, hour, minute, second, ms;
  getDMY(day, month, year);
  getHMS(hour, minute, second, ms);
  st.wYear         = year;
  st.wMonth        = month;
  st.wDay          = day;
  st.wHour         = hour;
  st.wMinute       = minute;
  st.wSecond       = second;
  st.wMilliseconds = ms;
  st.wDayOfWeek = ((int)getWeekDay() + 1) % 7;
  return st;
}

Timestamp::Timestamp(const FILETIME &ft) {
  SYSTEMTIME st;
  FileTimeToSystemTime(&ft, &st);
  *this = st;
}

Timestamp::operator FILETIME() const {
  const SYSTEMTIME st = *this;
  FILETIME ft;
  SystemTimeToFileTime(&st, &ft);
  return ft;
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

Timestamp::Timestamp(double d) { // ie type DATE
  m_factor = getFactor(Date(d), Time(fraction(d) * Time::getMaxFactor()));
}

double Timestamp::getDATE() const {
  return getDate().getDATE() + fraction((double)m_factor / Time::getMaxFactor());
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
  return mktime(&tm);
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
  result.m_factor += (INT64)count * Time::getMaxFactor();
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

void Timestamp::checkFactor(INT64 factor) { // static
  if(factor < getMinFactor()) {
    throwTimestampUnderflow();
  }
  if(factor > getMaxFactor()) {
    throwTimestampOverflow();
  }
}

Timestamp Timestamp::operator-(int count) const {
  Timestamp result(*this);
  result.m_factor -= (INT64)count * Time::getMaxFactor();
  Timestamp::checkFactor(result.m_factor);
  return result;
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

const double Timestamp::s_daysPerYear = 146097.0 / 400; // 365.2425

static void throwInvalidTimeComponent(const TCHAR *function, TimeComponent c) {
  throwException(_T("%s:Invalid TimeComponent (=%d)."), function, c);
}

double Timestamp::diff(const Timestamp &from, const Timestamp &to, TimeComponent c) { // static
  switch(c) {
  case TMILLISECOND: return  to.m_factor - from.m_factor;
  case TSECOND     : return (to.m_factor - from.m_factor) / 1000;
  case TMINUTE     : return (to.m_factor - from.m_factor) / 60 / 1000;
  case THOUR       : return (to.m_factor - from.m_factor) / 3600 / 1000;
  case TDAYOFYEAR  : return (to.m_factor - from.m_factor) / 3600 / 1000 / 24;
  case TMONTH      : return (to.m_factor - from.m_factor) / 3600 / 1000 / 2  / s_daysPerYear;
  case TYEAR       : return (to.m_factor - from.m_factor) / 3600 / 1000 / 24 / s_daysPerYear;
  default          : throwInvalidTimeComponent(__TFUNCTION__, c);
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
    throwInvalidTimeComponent(__TFUNCTION__, c);
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
  default          : throwInvalidTimeComponent(__TFUNCTION__, c);
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
    throwInvalidTimeComponent(__TFUNCTION__, c);
  }
  return *this = Timestamp(d, t);
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

FILETIME Timestamp::time_tToFILETIME(time_t t) { // static
  // Note that LONGLONG is a 64-bit value
  const LONGLONG ll = Int32x32To64(t, 10000000) + 116444736000000000ui64;
  FILETIME ft;
  ft.dwLowDateTime = (DWORD)ll;
  ft.dwHighDateTime = ll >> 32;
  return ft;
}

SYSTEMTIME Timestamp::time_tToSYSTEMTIME(time_t t) { // static
  FILETIME   ft = time_tToFILETIME(t);
  SYSTEMTIME st;
  FileTimeToSystemTime(&ft, &st);
  return st;
}
