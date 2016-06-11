#pragma once

#include "MyUtil.h"
#include "Packer.h"
#include <time.h>

typedef enum {
  MONDAY
 ,TUESDAY
 ,WEDNESDAY
 ,THURSDAY
 ,FRIDAY
 ,SATURDAY
 ,SUNDAY
} WeekDay;

typedef enum {
  TMILLISECOND
 ,TSECOND
 ,TMINUTE
 ,THOUR
 ,TDAYOFMONTH
 ,TDAYOFYEAR
 ,TWEEK
 ,TMONTH
 ,TYEAR
} TimeComponent;

extern const String ddMMyy;
extern const String ddMMyyyy;
extern const String ddMM;
extern const String MMyyyy;
extern const String MMyy;
extern const String yyyy;
extern const String yy;
extern const String hhmm;
extern const String hhmmss;
extern const String hhmmssSSS;
extern const String ddMMyyyyhhmm;
extern const String ddMMyyyyhhmmss;
extern const String ddMMyyyyhhmmssSSS;

class Date {
private:
  int m_factor;
  void init(const TCHAR *src);
  Date(int factor);
  static void checkFactor(__int64 factor);
  static int getDATE0Factor();
public:
  static const short ydaynl[];
  static const short ydayl[];
  static const TCHAR *daynames[];
  static int  getFactor(int day, int month, int year);  // Calculate factor from day,month,year
  static int  getFactor(time_t tt);
  static void checkFactor(int factor);
  static bool isLeapYear(    int year);                 // Is year a leapYear
  static int  getDaysInMonth(int year, int month);      // Number of days of month in the given year 
  static int  getDaysInYear( int year);                 // Number of days in year
  static int  getWeeksInYear(int year);                 // Number of weeks in year
  static Date getEaster(     int year);                 // Get the date of easter in the given year in the gregorian calender
  static bool dateValidate(  int day, int month, int year);
  static int  adjustYear100( int year);                 // return (year<currentyear%100+30)?(year+2000) : (year<100)?(year+1900):year;

  Date();
  Date(int day, int month, int year);
  explicit Date(const TCHAR *src);
  explicit Date(const String &src);
  explicit Date(time_t tt);
  explicit Date(double d);                              // ie type DATE
  double   getDATE() const;
  Date     operator+(int days) const;
  Date     operator-(int days) const;
  Date    &operator+=(int days);                        // Add days to this
  Date    &operator-=(int days);                        // Subtract days from this
  Date    &operator++() { return operator+=(1); }       // Prefix form. Add 1 day
  Date    &operator--() { return operator-=(1); }       // Prefix form. Subtract 1 day
  Date     operator++(int);                             // Postfix form. Add 1 day
  Date     operator--(int);                             // Postfix form. Subtract 1 day
  int      operator-(const Date &r) const;              // Distance in days
  Date &add(TimeComponent c, int count);
  int   get(TimeComponent c);
  Date &set(TimeComponent c, int value);
  static int diff(const Date &from, const Date &to, TimeComponent c = TDAYOFMONTH);
  friend int dateCmp(    const Date &l, const Date &r);
  friend bool operator==(const Date &l, const Date &r);
  friend bool operator!=(const Date &l, const Date &r);
  friend bool operator< (const Date &l, const Date &r);
  friend bool operator<=(const Date &l, const Date &r);
  friend bool operator> (const Date &l, const Date &r);
  friend bool operator>=(const Date &l, const Date &r);
  void getDMY(int &day, int &month, int &year) const;
  int getYear()        const;
  int getMonth()       const;
  int getDayOfMonth()  const;
  int getDayOfYear()   const;
  WeekDay getWeekDay() const;
  int getWeek()        const;
  bool isLeapYear()    const;
  unsigned long hashCode() const {
    return m_factor;
  }
  int getFactor() const {
    return m_factor;
  }

  static const Date &getMinDate();
  static const Date &getMaxDate();

  static __int64 getMinFactor() {
    return getMinDate().m_factor;
  }

  static __int64 getMaxFactor() {
    return getMaxDate().m_factor;
  }

  TCHAR *tostr(TCHAR *dst, const String &format = ddMMyyyy) const;
  String toString(const String &format = ddMMyyyy) const;
  friend class Timestamp;
  friend Packer &operator<<(Packer &p, const Date &d);
  friend Packer &operator>>(Packer &p,       Date &d);
};

class Time {
private:
  int m_factor;
  explicit Time(double msec); // msec:milliseconds from midnight
  void init(const TCHAR *src);
  static int getFactor(time_t tt);
  Time(int factor);
  static void checkFactor(__int64 factor);
public:

  static int getFactor(int hour, int minute, int second, int millisecond);
  static bool timeValidate(int hour, int minute, int second, int millisecond=0);

  Time();
  Time(int hour, int minute, int second, int millisecond = 0);
  explicit Time(const String &src);
  explicit Time(const TCHAR *src);
  explicit Time(time_t t);
  Time  operator+( int seconds) const;
  Time  operator-( int seconds) const;
  Time &operator+=(int seconds);
  Time &operator-=(int seconds);
  Time &operator++() { return operator+=(1); }       // Prefix form. Add 1 second
  Time &operator--() { return operator-=(1); }       // Prefix form. Subtract 1 second
  Time  operator++(int);                             // Postfix form. Add 1 second
  Time  operator--(int);                             // Postfix form. Subtract 1 second
  int   operator-(const Time &r) const;              // Distance in seconds
  Time &add(TimeComponent c, int count);
  int   get(TimeComponent c);
  Time &set(TimeComponent c, int value);
  friend int timeCmp(    const Time &l, const Time &r);
  friend bool operator==(const Time &l, const Time &r);
  friend bool operator!=(const Time &l, const Time &r);
  friend bool operator< (const Time &l, const Time &r);
  friend bool operator<=(const Time &l, const Time &r);
  friend bool operator> (const Time &l, const Time &r);
  friend bool operator>=(const Time &l, const Time &r);
  void getHMS(int &hour, int &minute, int &second, int &millisecond) const;
  int getHour() const;
  int getMinute() const;
  int getSecond() const;
  int getMilliSecond() const;
  unsigned long hashCode() const {
    return m_factor;
  }

  static const Time &getMinTime();
  static const Time &getMaxTime();

  static int getMinFactor() {
    return 0;
  }
  static int getMaxFactor() {
    return getMaxTime().m_factor + 1;
  }

  TCHAR *tostr(TCHAR *dst, const String &format = hhmm) const;
  String toString(const String &format = hhmm) const;
  friend class Timestamp;
  friend Packer &operator<<(Packer &p, const Time &t);
  friend Packer &operator>>(Packer &p,       Time &t);
};

class Timestamp {
private:
  __int64 m_factor;
  void init(const TCHAR *src);
  static void checkFactor(__int64 factor);
public:
  static __int64 getFactor(const Date &d, const Time &t);

  Timestamp();
  Timestamp(int day, int month, int year, int hour, int minute, int second = 0, int millisecond = 0);
  Timestamp(const Date &d, const Time &t);
  explicit Timestamp(const String &src);
  explicit Timestamp(const TCHAR *src);
  Timestamp(const Date &d);
  Timestamp(const SYSTEMTIME &st);
  explicit Timestamp(time_t t);
  explicit Timestamp(double d); // ie type DATE
  double getDATE() const;
  Timestamp  operator+( int count) const;                 // Add count days
  Timestamp  operator-( int count) const;                 // Subtract count days
  Timestamp &operator+=(int count);                       // Add count days
  Timestamp &operator-=(int count);                       // Subtract count days
  Timestamp &operator++() { return operator+=(1); }       // Prefix form. Add 1 day
  Timestamp &operator--() { return operator-=(1); }       // Prefix form. Subtract 1 day
  Timestamp  operator++(int);                             // Postfix form. Add 1 day
  Timestamp  operator--(int);                             // Postfix form. Subtract 1 day
  double operator-(const Timestamp &r) const;             // Difference in days
  friend double diff(const Timestamp &from, const Timestamp &to, TimeComponent c = TDAYOFMONTH);
  Timestamp &add(TimeComponent c, int count);
  int        get(TimeComponent c);
  Timestamp &set(TimeComponent c, int value);
  friend int timestampCmp(const Timestamp &l, const Timestamp &r);
  friend bool operator==( const Timestamp &l, const Timestamp &r);
  friend bool operator!=( const Timestamp &l, const Timestamp &r);
  friend bool operator< ( const Timestamp &l, const Timestamp &r);
  friend bool operator<=( const Timestamp &l, const Timestamp &r);
  friend bool operator> ( const Timestamp &l, const Timestamp &r);
  friend bool operator>=( const Timestamp &l, const Timestamp &r);
  int getYear()        const;
  int getMonth()       const;
  int getWeek()        const;
  int getDayOfMonth()  const;
  int getHour()        const;
  int getDayOfYear()   const;
  int getMinute()      const;
  int getSecond()      const;
  int getMilliSecond() const;
  WeekDay getWeekDay() const;
  void getDMY(int &day, int &month, int &year) const;
  void getHMS(int &hour, int &minute, int &second, int &millisecond) const;
  Date getDate() const;
  Time getTime() const;
  operator SYSTEMTIME() const;
  unsigned long hashCode() const;
  time_t gettime_t() const;
  TCHAR *tostr(TCHAR *dst, const String &DateFormat = ddMMyyyyhhmm) const;
  String toString(const String &format = ddMMyyyyhhmm) const;

  static const Timestamp &getMinTimestamp();
  static const Timestamp &getMaxTimestamp();
  static __int64 getMinFactor() {
    return getMinTimestamp().m_factor;
  }
  static __int64 getMaxFactor() {
    return getMaxTimestamp().m_factor;
  }

  static String cctime(); // cctime(getSystemTime());
  static String cctime(time_t tt);
  static time_t getSystemTime();
  friend Packer &operator<<(Packer &p, const Timestamp &ts);
  friend Packer &operator>>(Packer &p,       Timestamp &ts);
};
