#include "pch.h"
#include <Tokenizer.h>
#include <Date.h>

#pragma warning(disable : 4244)

Time::Time() {
  SYSTEMTIME st;
  GetLocalTime(&st);
  m_factor = getFactor(st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
}

bool Time::timeValidate(int hour, int minute, int second, int millisecond) { // static
  return ((UINT)hour < 24) && ((UINT)minute < 60) && ((UINT)second < 60) && ((UINT)millisecond < 1000);
}

Time::Time(int hour, int minute, int second, int millisecond) {
  if(!timeValidate(hour, minute, second, millisecond)) {
    throwException(_T("Time::Invalid time:%02d:%02d:%02d.%d"), hour, minute, second, millisecond);
  }
  m_factor = getFactor(hour, minute, second, millisecond);
}

void Time::throwInvalidFactor(INT64 factor) { // static
  throwException(_T("Time::Invalid factor:%I64d. Must be in the range [0..%d["), factor, getMaxFactor());
}

void Time::init(const TCHAR *src) {
  static const TCHAR *errmsg = _T("Invalid time-format <%s>");

  int i = 0;
  int d[4];
  memset(d, 0, sizeof(d));
  for(Tokenizer tok(src, _T(".:-")); tok.hasNext() && i < 4; i++) {
    if(_stscanf(tok.next().cstr(), _T("%d"), &d[i]) != 1) {
      throwException(errmsg, src);
    }
  }
  if(i < 2) {
    throwException(errmsg, src);
  }
  *this = Time(d[0], d[1], d[2], d[3]);
}

Time Time::operator+(int seconds) const {
  Time result(*this);
  result.m_factor += 1000 * seconds;
  const int maxFactor = getMaxFactor();
  if(result.m_factor < 0) {
    result.m_factor %= maxFactor;
    result.m_factor = maxFactor + result.m_factor;
  } else {
    result.m_factor %= maxFactor;
  }
  return result;
}

Time Time::operator++(int) {
  const Time result = *this;
  *this += 1;
  return result;
}

Time Time::operator--(int) {
  const Time result = *this;
  *this -= 1;
  return result;
}

static void throwInvalidTimeComponent(const TCHAR *function, TimeComponent c) {
  throwException(_T("%s:Invalid TimeComponent (=%d)"), function, c);
}

Time &Time::add(TimeComponent c, int count) {
  switch(c) {
  case TMILLISECOND:
    { m_factor += count;
      const int maxFactor = getMaxFactor();
      if(m_factor < 0) {
        m_factor %= maxFactor;
        m_factor = maxFactor + m_factor;
      } else {
        m_factor %= maxFactor;
      }
    }
    break;
  case TSECOND     :
    *this += count;
    break;
  case TMINUTE     :
    *this += 60 * count;
    break;
  case THOUR       :
    *this += 3600 * count;
    break;
  default          :
    throwInvalidTimeComponent(__TFUNCTION__, c);
  }
  return *this;
}

int Time::get(TimeComponent c) {
  switch(c) {
  case TMILLISECOND: return getMilliSecond();
  case TSECOND     : return getSecond();
  case TMINUTE     : return getMinute();
  case THOUR       : return getHour();
  default          : throwInvalidTimeComponent(__TFUNCTION__, c);
                     return 0;
  }
}

Time &Time::set(TimeComponent c, int value) {
  int hour, minute, second, millisecond;
  getHMS(hour, minute, second, millisecond);
  switch(c) {
  case TMILLISECOND: millisecond = value; break;
  case TSECOND     : second      = value; break;
  case TMINUTE     : minute      = value; break;
  case THOUR       : hour        = value; break;
  default          : throwInvalidTimeComponent(__TFUNCTION__, c);
  }
  return *this = Time(hour, minute, second, millisecond);
}

int Time::getFactor(time_t tt) { // static
  struct tm *tm = localtime(&tt);
  return getFactor(tm->tm_hour, tm->tm_min, tm->tm_sec, 0);
}

const Time &Time::getMinTime() { // static
  static const Time minTime(0, 0, 0, 0);
  return minTime;
}

const Time &Time::getMaxTime() { // static
  static const Time maxTime(23, 59, 59, 999);
  return maxTime;
}

void Time::getHMS(int &hour, int &minute, int &second, int &millisecond) const {
  hour        = getHour();
  minute      = getMinute();
  second      = getSecond();
  millisecond = getMilliSecond();
}

#define CASECH(ch, comp) case ch: {                          \
  int count = 1, scale = 10;                                 \
  for(cp++; *cp == ch; cp++) {                               \
    count++;                                                 \
    scale *= 10;                                             \
  }                                                          \
  String tmp = ::format(_T("%0*d"), count, (comp) % scale);  \
  _tcscpy(t, tmp.cstr());                                    \
  t += tmp.length();                                         \
}                                                            \
break

TCHAR *Time::tostr(TCHAR *dst, const String &format) const {
  int hour, minute, second, millisecond;
  getHMS(hour, minute, second, millisecond);
  TCHAR *t = dst;
  for(const TCHAR *cp = format.cstr(); *cp;) {
    switch(*cp) {
    CASECH('h', hour       );
    CASECH('m', minute     );
    CASECH('s', second     );
    CASECH('S', millisecond);

    default :
      *(t++) = *(cp++);
      break;
    }
  }
  *t = 0;
  return dst;
}
