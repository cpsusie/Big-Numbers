#include "pch.h"
#include <Tokenizer.h>
#include <Date.h>

const TCHAR *Date::s_daynames[] = {
  _T("Mon")
 ,_T("Tue")
 ,_T("Wed")
 ,_T("Thu")
 ,_T("Fri")
 ,_T("Sat")
 ,_T("Sun")
};

//                             31.1 28.2 31.3 30.4 31.5 30.6 31.7...
const short Date::s_ydaynl[] = { 30  ,58  ,89  ,119 ,150 ,180 ,211 ,242 ,272 ,303 ,333 ,364 }; // nonleapyear
const short Date::s_ydayl[]  = { 30  ,59  ,90  ,120 ,151 ,181 ,212 ,243 ,273 ,304 ,334 ,365 }; // leapYear

const String ddMMyy            = _T("dd.MM.yy");
const String ddMMyyyy          = _T("dd.MM.yyyy");
const String ddMM              = _T("dd.MM");
const String MMyyyy            = _T("MM.yyyy");
const String MMyy              = _T("MM.yy");
const String yyyy              = _T("yyyy");
const String yy                = _T("yy");
const String hhmm              = _T("hh:mm");
const String hhmmss            = _T("hh:mm:ss");
const String hhmmssSSS         = _T("hh:mm:ss:SSS");

const String ddMMyyyyhhmm      = ddMMyyyy + _T(" ") + hhmm;
const String ddMMyyyyhhmmss    = ddMMyyyy + _T(" ") + hhmmss;
const String ddMMyyyyhhmmssSSS = ddMMyyyy + _T(" ") + hhmmssSSS;

Date::Date() {
  time_t tt;
  time(&tt);
  m_factor = getFactor(tt);
}

Date::Date(int day, int month, int year) {
  if(!dateValidate(day, month, year)) {
    throwException(_T("Invalid Date:%02d.%02d.%d"), day, month, year);
  }
  m_factor = getFactor(day, month, year);
}

Date::Date(int factor) {
  checkFactor(factor);
  m_factor = factor;
}

int Date::adjustYear100(int year) { // static
  static const int thisYear = Date().getYear();
  if(year < thisYear % 100 + 30) {
    return year + 2000;
  } else if(year < 100) {
    return year + 1900;
  } else {
    return year;
  }
}

void Date::init(const TCHAR *src) {
  static const TCHAR *errmsg = _T("Invalid date-format <%s>");
  
  int i = 0;
  int d[3];
  for(Tokenizer tok(src, _T("./-")); tok.hasNext() && i < 3; i++) {
    if(_stscanf(tok.next().cstr(), _T("%d"), &d[i]) != 1) {
      throwException(errmsg, src);
    }
  }
  if(i < 3) {
    throwException(errmsg, src);
  }
  *this = Date(d[0], d[1], adjustYear100(d[2]));
}

Date::Date(double d) {
  m_factor = (long)d + getDATE0Factor();
  checkFactor(m_factor);
}

int Date::getDATE0Factor() {
  static int DATE0Factor = Date::getFactor(30, 12, 1899); // 693959L
  return DATE0Factor;
}

int Date::getFactor(time_t tt) {
  struct tm *tm = localtime(&tt);
  return Date::getFactor(tm->tm_mday, tm->tm_mon+1, tm->tm_year+1900);
}

int Date::getFactor(int day, int month, int year) { // calculate factor from day, month, year
  if(month <= 2) {
    return 365*year + day + 31*(month-1) + (year-1)/4 - ((year-1)/100 + 1)*3/4;
  } else {
    return 365*year + day + s_ydayl[month-2] + year/4 - (year/100 + 1)*3/4;
  }
}

bool Date::dateValidate(int day, int month, int year) {
  if(year < 1582) {
    return false;
  }
  if(month < 1 || month > 12 || day < 1 || day > Date::getDaysInMonth(year, month)) {
    return false;
  }
  return true;
}

static void throwDateUnderflow() {
  throwException(_T("Date becomes to small. First date in Gregorian calendar is %s"), Date::getMinDate().toString().cstr());
}

static void throwDateOverflow() {
  throwException(_T("Date becomes to big. Last date is %s"), Date::getMaxDate().toString().cstr());
}

void Date::checkFactor(INT64 factor) {
  if(factor < getMinFactor()) {
    throwDateUnderflow();
  }
  if(factor > getMaxFactor()) {
    throwDateOverflow();
  }
}

const Date &Date::getMinDate() {  // static
  static const Date minDate(15, 10, 1582); // Gregorian calender starts here
  return minDate;
}

const Date &Date::getMaxDate() {  // static
  static const Date maxDate(31, 12, 9999); // Our calender stops here !!
  return maxDate;
}

Date Date::operator+(int days) const {
  Date result(*this);
  result.m_factor += days;
  checkFactor(result.m_factor);
  return result;
}

Date Date::operator-(int days) const {
  Date result(*this);
  result.m_factor -= days;
  Date::checkFactor(result.m_factor);
  return result;
}

Date &Date::operator+=(int days) {
  checkFactor(m_factor + days);
  m_factor += days;
  return *this;
}

Date &Date::operator-=(int days) {
  checkFactor(m_factor - days);
  m_factor -= days;
  return *this;
}

Date Date::operator++(int) {
  const Date result = *this;
  *this += 1;
  return result;
}

Date Date::operator--(int) {
  const Date result = *this;
  *this -= 1;
  return result;
}

static void throwInvalidTimeComponent(const TCHAR *function, TimeComponent c) {
  throwException(_T("%s:Invalid timeComponent (=%d)."), function, c);
}

Date &Date::add(TimeComponent c, int count) {
  switch(c) {
  case TDAYOFMONTH:
  case TDAYOFYEAR :
    *this += count;
    break;
  case TWEEK :
    *this += 7 * count;
    break;
  case TMONTH:
    { int day, month, year;
      getDMY(day, month, year);
      bool lastday = (day == getDaysInMonth(year, month));
      month += count;
      if(month > 12) {
        year += (month-1) / 12;
        month = (month-1) % 12 + 1;
      } else if(month < 1) {
        year += month / 12 - 1;
        month = 12 + month % 12;
      }
      int md = getDaysInMonth(year, month);
      if(lastday || day > md) {
        day = md;
      }
      *this = Date(day, month, year);
      break;
    }
  case TYEAR :
    { int day, month, year;
      getDMY(day, month, year);
      year += count;
      if(month == 2 && day == 29 && !isLeapYear(year)) {
        day = 28;
      }
      *this = Date(day, month, year);
      break;
    }
  default   :
    throwInvalidTimeComponent(__TFUNCTION__, c);
  }
  return *this;
}

int Date::get(TimeComponent c) {
  switch(c) {
  case TDAYOFMONTH: return getDayOfMonth();
  case TDAYOFYEAR : return getDayOfYear();
  case TWEEK      : return getWeek();
  case TMONTH     : return getMonth();
  case TYEAR      : return getYear();
  default         : throwInvalidTimeComponent(__TFUNCTION__, c);
                    return 0;
  }
}

Date &Date::set(TimeComponent c, int value) {
  int day, month, year;
  getDMY(day, month, year);
  switch(c) {
  case TDAYOFMONTH:
    return *this = Date(value, month, year);

  case TDAYOFYEAR:
    m_factor = getFactor(1, 1, year) + value - 1;
    return *this;

  case TWEEK      :
    { Date    january1    = Date(1, 1, year);
      int     week11      = january1.getWeek();
      WeekDay weekDay11   = january1.getWeekDay();
      Date    mondayWeek1 = (week11 == 1) ? (january1 - (int)weekDay11) : (january1 + (7 - (int)weekDay11));
      WeekDay weekDay     = getWeekDay();
      return *this = mondayWeek1 + (7*(value-1) + (int)weekDay);
    }

  case TMONTH     :
    { int maxDay = getDaysInMonth(year, value);
      if(day == getDaysInMonth(year, month) || day > maxDay) {
        day = maxDay;
      }
      return *this = Date(day, value, year);
    }

  case TYEAR      :
    { if(month == 2 && day == 29 && isLeapYear(year) && !isLeapYear(value)) {
        day = 28;
      }
      return *this = Date(day, month, value);
    }

  default    :
    throwInvalidTimeComponent(__TFUNCTION__, c);
    return *this;
  }
}

int Date::diff(const Date &from, const Date &to, TimeComponent c) {
  switch(c) {
  case TDAYOFMONTH  :
  case TDAYOFYEAR   :
    return to.m_factor - from.m_factor;

  case TWEEK :
    return to.getWeek() - from.getWeek();

  case TMONTH:
    { int toDay, toMonth, toYear;
      int fromDay, fromMonth, fromYear;
      to.getDMY(toDay, toMonth, toYear);
      from.getDMY(fromDay, fromMonth, fromYear);
      return (toMonth + toYear * 12) - (fromMonth + fromYear * 12);
    }
  case TYEAR :
    return to.getYear() - from.getYear();

  default       :
    throwInvalidTimeComponent(__TFUNCTION__, c);
    return 0;
  }
}

static int getYear(int factor, int &f11) { // return year (and factor of 1.1.year) of factor
  int year = (factor+1)*400/146097; // 146097/400 = 365 + 1/4 - 1/100 + 1/400
  f11 = Date::getFactor(1, 1, year);
  if(factor < f11) {
    year--;
    f11 = Date::getFactor(1, 1, year);
  }
  return year;
}

static int getMonth(int factor, int &year, int &f11, const short *&yd) { // return month (and year and factor of 1.1.year, and s_ydaynl or s_ydayl) of factor
  year  = getYear(factor, f11);
  yd = Date::isLeapYear(year) ? Date::s_ydayl : Date::s_ydaynl;
  int d = factor - f11;
  for(int m = d/32;m<12;m++) {
    if(yd[m] >= d) {
      return m + 1;
    }
  }
  return 0;
}

static int getDay(int factor, int &year, int &m) { // return day (and month, year) of factor
  int f11;         // datefactor of 1.1.year
  const short *yd; // s_ydaynl/s_ydayl depending on leapYear(year)
  m = getMonth(factor, year, f11, yd);
  if(m == 1) {
    return factor - f11 + 1;
  } else {
    return factor - f11 - yd[m-2];
  }
}

int Date::getYear() const {
  int f11; // not used
  return ::getYear(m_factor, f11);
}

void Date::getDMY(int &day, int &month, int &year) const {
  day = ::getDay(m_factor, year, month);
}

int Date::getMonth() const {
  int year, f11;   // not used
  const short *yd; // not used
  return ::getMonth(m_factor, year, f11, yd);
}

int Date::getDayOfMonth() const {
  int year, m; // not used
  return ::getDay(m_factor, year, m);
}

int Date::getDayOfYear() const {
  int f11;
  int year = ::getYear(m_factor, f11);
  return m_factor - f11 + 1;
}

int Date::getWeek() const {
  int day, month, year;
  getDMY(day, month, year);
  int w = getWeekDay();    /* ugedagen for this ( eller 1.1.år)    */

  /* special tilfælde 1: slutningen af året */
  /* torsdagen i samme uge som Date er i næste år */
  if( (month == 12) && (day - w >= 29) ) {
    return 1;
  }

  /* special tilfælde 2: starten af året    */
  /* torsdagen i samme uge som Date er i forrige år */
  if( (month == 1) && (w - day >= 3) ) {
    return Date(31, 12, year-1).getWeek();
  }

  /* 'normal' tilfældet */
  w = Date(1, 1, year).getWeekDay();
  int d = getDayOfYear();

  int result = (d+w-1)/7;
  if(w <= 3) { // er den 1.1 man, tirs, ons, eller tors
    result++;
  }
  return result;
}

// Number of weeks in year
int Date::getWeeksInYear(int year) { // static
  Date lastDay = Date(31, 12, year);
  switch(lastDay.getWeekDay()) {
  case MONDAY   :
  case TUESDAY  :
  case WEDNESDAY: return Date(28, 12, year).getWeek();

  case THURSDAY : 
  case FRIDAY   :
  case SATURDAY :
  case SUNDAY   : return lastDay.getWeek();
  default:
    throwException(_T("%s(%d):Illegal weekDay (=%d) for %s"), __TFUNCTION__, year, lastDay.getWeekDay(), lastDay.toString().cstr());
    return 30;
  }
}

// Number of days in month.year
int Date::getDaysInMonth(int year, int month) { // static
  switch(month) {
  case  1:
  case  3:
  case  5:
  case  7:
  case  8:
  case 10:
  case 12: return 31;
  case  4:
  case  6:
  case  9:
  case 11: return 30;
  case  2: return isLeapYear(year) ? 29 : 28;
  default:
    throwException(_T("%s:Illegal month:%d. Must be [1..12]."), __TFUNCTION__, month);
    return 30;
  }
}

// se "Beginning ATL COM programming" s. 78
Date Date::getEaster(int year) { // static
  if(year < 326 || year > 9999) {
    throwException(_T("Easter not defined for year=%d. Year must be in the range [326..9999]"), year);
  }

  short first = year / 100;
  short div19 = year % 19;

  short temp = (first - 15) / 2 + ((first > 26) ? -1 : 0) +
    ((first > 38) ? -1 : 0) + 202 - 11 * div19;

  if(first == 21 || first == 24 || first == 25 ||
     first == 33 || first == 36 || first == 37)
     temp += -1;

  temp %= 30;

  short ta = temp + ((temp == 29) ? -1 : 0) +
           ((temp == 28 && div19 > 10) ? -1 : 0) + 21;

  short tb = (ta - 19) % 7;
  temp = (40 - first) % 4;
  short tc = temp - ((temp > 1) ? -1 : 0) - ((temp == 3) ? -1 : 0);
  temp = year % 100;
  short td = (temp + temp / 4) % 7;
  short te = ((20 - tb - tc - td) % 7) + 1;

  int day = ta + te;
  int month;

  if(day > 61) {
    day -= 61;
    month = 5;
  } else if(day > 31) {
    day -= 31;
    month = 4;
  } else {
    month = 3;
  }
  return Date(day, month, year);
}

#define CASECH(ch, comp) case ch: {                         \
  int count = 1, scale = 10;                                \
  for(cp++; *cp == ch; cp++) {                              \
    count++;                                                \
    scale *= 10;                                            \
  }                                                         \
  String tmp = ::format(_T("%0*d"), count, (comp) % scale); \
  _tcscpy(t, tmp.cstr());                                   \
  t += tmp.length();                                        \
}                                                           \
break

TCHAR *Date::tostr(TCHAR *dst, const String &format) const {
  int year, month, day;
  getDMY(day, month, year);
  TCHAR *t  = dst;
  for(const TCHAR *cp = format.cstr(); *cp;) {
    switch(*cp) {
    CASECH(_T('y'), year          );
    CASECH(_T('M'), month         );
    CASECH(_T('w'), getWeek()     );
    CASECH(_T('d'), day           );
    CASECH(_T('D'), getDayOfYear());

    default :
      *(t++) = *(cp++);
      break;
    }
  }
  *t = 0;
  return dst;
}
