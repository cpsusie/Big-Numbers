#include "stdafx.h"
#include "CppUnitTest.h"
#include <Math.h>
#include <Time.h>
#include <Date.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestDate {

#include <UnitTestTraits.h>

  static CompactArray<Date> DateArray;

  class DateComparator : public Comparator<Date> {
  public:
    bool m_reverse;
    int compare(const Date &d1, const Date &d2) const;
  };

  int DateComparator::compare(const Date &d1, const Date &d2) const {
    return m_reverse ? dateCmp(d2, d1) : dateCmp(d1, d2);
  }

  static void nextDate(int &day, int &month, int &year) {
    if (++day > Date::getDaysInMonth(year, month)) {
      day = 1;
      if (++month > 12) {
        month = 1;
        year++;
      }
    }
  }

  static void fprintdate(FILE *f, const Date &d) {
    _ftprintf(f, _T("%s"), d.toString().cstr());
  }

  void printdate(const Date &d) {
    fprintdate(stdout, d);
  }

  typedef struct {
    TimeComponent c;
    int min, max;
    TCHAR *name;
  } TimeComponentMinMax;

  const TimeComponentMinMax components[] = {
     TMILLISECOND , 0   , 999   , _T("TMILLISECOND")
    ,TSECOND      , 0   , 59    , _T("TSECOND")
    ,TMINUTE      , 0   , 59    , _T("TMINUTE")
    ,THOUR        , 0   , 23    , _T("THOUR")
    ,TDAYOFMONTH  , 1   , 30    , _T("TDAYOFMONTH")
    ,TDAYOFYEAR   , 1   , 365   , _T("TDAYOFYEAR")
    ,TWEEK        , 1   , 52    , _T("TWEEK")
    ,TMONTH       , 1   , 12    , _T("TMONTH")
    ,TYEAR        , 1583, 9999  , _T("TYEAR")
  };


  TEST_CLASS(TestDate) {
  public:

    TEST_METHOD(DateTestFactor) {
      int year1 = 1582, month1 = 1, day1 = 1;
      int year2 = 1582, month2 = 1, day2 = 1;
      int daysBetween = 0;

      Date start(day1, month1, year1);
      for (;year2 < 10000;nextDate(day2, month2, year2), daysBetween++) {
        Date d2(day2, month2, year2);
        verify(d2 - start == daysBetween);
        /*        _tprintf(_T("%d.%d.%d\n"), day1, month1, year1);
                  _tprintf(_T("%d.%d.%d\n"), day2, month2, year2);
                  _tprintf(_T("d2-start = %d daysBetween = %d\n"), d2 - start, daysBetween);
                }
         */
      }
    }

    TEST_METHOD(DateSetComponent) {
      String format = ddMMyyyyhhmm + _T(".SSS");

      Timestamp endDate = Date(1, 1, 2011);
      for (Timestamp ts = Date(30, 4, 1583); ts < endDate; ts.add(TDAYOFMONTH,5).add(THOUR,9).add(TSECOND,5082)) {
        int day, month, year;
        ts.getDMY(day, month, year);
        WeekDay weekDay = ts.getWeekDay();
        int values[ARRAYSIZE(components)];
        for (int i = 0; i < ARRAYSIZE(components); i++) {
          values[i] = ts.get(components[i].c);
        }
        for (int i = 0; i < ARRAYSIZE(components); i++) {
          const TimeComponentMinMax &tmm = components[i];
          for (int k = 0; k < 2; k++) {
            Timestamp test = ts;
            int setValue;
            switch (k) {
            case 0:
              setValue = tmm.min;
              break;
            case 1:
              switch (tmm.c) {
              case TDAYOFMONTH:
                setValue = Date::getDaysInMonth(ts.getYear(), ts.getMonth());
                break;
              case TDAYOFYEAR:
                setValue = Date::getDaysInYear(ts.getYear());
                break;
              case TWEEK:
                setValue = Date::getWeeksInYear(ts.getYear());
                break;
              default:
                setValue = tmm.max;
                break;
              }  // switch(tm.c)
            } // switch(k)
            test.set(tmm.c, setValue);
            switch (tmm.c) {
            case TDAYOFYEAR:
              { const int testValue = test.get(tmm.c);
                verify(testValue == setValue);
                /*
                _tprintf(_T("Date.set(%s,%d) failed for date=%s. Before set:%s=%d. After:%s=%d. Result=%s\n")
                  , tmm.name
                  , setValue
                  , ts.toString(format).cstr()
                  , tmm.name
                  , values[(int)TDAYOFYEAR]
                  , tmm.name
                  , testValue
                  , test.toString(format).cstr()
                );
                */
              }
              break;

            case TWEEK:
              { const int testValue = test.get(tmm.c);
                verify((testValue == setValue) && (test.getWeekDay() == weekDay));
                /*
                _tprintf(_T("Date.set(%s,%d) failed for date=%s. Before set:%s=%d, weekDay=%d. After:%s=%d, weekDay=%d. Result=%s\n")
                  , tmm.name
                  , setValue
                  , ts.toString(format).cstr()
                  , tmm.name
                  , values[(int)TWEEK]
                  , weekDay
                  , tmm.name
                  , testValue
                  , test.getWeekDay()
                  , test.toString(format).cstr()
                );
                */
              }
              break;

            case TMONTH:
            case TYEAR:
              { int testDay, testMonth, testYear;
                test.getDMY(testDay, testMonth, testYear);
                const int testValue = test.get(tmm.c);
                verify((testValue == setValue)
                     || (day == Date::getDaysInMonth(year, month) && testDay == Date::getDaysInMonth(testYear, testMonth))
                     || (day < Date::getDaysInMonth(year, month) && day <= Date::getDaysInMonth(testYear, testMonth) && testDay == day));
                /*
                _tprintf(_T("Date.set(%s,%d) failed for date=%s. Result=%s\n")
                  , tmm.name
                  , setValue
                  , ts.toString(format).cstr()
                  , test.toString(format).cstr()
                );
                allOk = false;
                */
              }
              break;

            default:
              { for (int j = 0; j < ARRAYSIZE(components); j++) {
                  const TimeComponentMinMax &getTmm = components[j];
                  if (getTmm.c == TWEEK || getTmm.c == TDAYOFYEAR) {
                    continue;
                  }
                  const int testValue = test.get(getTmm.c);
                  verify(((j == i) && (testValue == setValue)) || ((j != i) && (testValue == values[j])));
                  /*
                  _tprintf(_T("Date.set(%s,%d) failed for date=%s. Before set:%s=%d. After:%s=%d. Result=%s\n")
                            , tmm.name
                            , setValue
                            , ts.toString(format).cstr()
                            , getTmm.name
                            , values[j]
                            , getTmm.name
                            , testValue
                            , test.toString(format).cstr()
                  allOk = false;
                  */
                } // for
              } // default
            } // switch(tmm.c)
          } // for(k...
        } // for(i
      } // for (Timestamp ts
    } // method

    TEST_METHOD(DateGetYear) {
      for (int y = 1582;y < 10000;y++) {
        Date d(1, 1, y);
        int y2 = d.getYear();
        verify(y2 == y);
//      _tprintf(_T("difference:%d 01.01.%d y2:%d\n"), y2 - y, y, y2);

        d = Date(31, 12, y);
        y2 = d.getYear();
        verify(y2 == y);
//      _tprintf(_T("difference:%d 31.12.%d y2:%d\n"), (int)y2 - y, y, y2);
      }
    }

    TEST_METHOD(DateGetMonth) {
      int y = 1582, m = 1, d = 1;

      for (;y < 2000; nextDate(d, m, y)) {
        Date date(d, m, y);
        const int m1 = date.getMonth();
        verify(m1 == m);
//          _tprintf(_T("%d.%d.%d -> m=%d\n"), d, m, y, m1);
      }
    }

    TEST_METHOD(DateGetDay) {
      int y = 1582, m = 1, d = 1;

      for (;y < 2000;nextDate(d, m, y)) {
        const Date dd(d, m, y);
        int y1, m1, d1;
        dd.getDMY(d1, m1, y1);
        verify(y1 == y && m1 == m && d1 == d);
//      _tprintf(_T("%d.%d.%d -> %d.%d.%d\n"), d, m, y, d1, m1, y1);
      }
    }

    TEST_METHOD(DateGetTime_t) {
      int year = 1970;
      int month = 1;
      int day = 2;
      int hour, minute, second;
      for (;year < 2000;nextDate(day, month, year)) {
        hour = randInt(24);
        minute = randInt(60);
        second = randInt(60);
        Timestamp timestamp(day, month, year, hour, minute, second);
        if (timestamp.getWeekDay() == SUNDAY && (month == 3 || month == 10) && day >= 25 && hour == 2) {
          continue; // skip the special hour when changing to/from summertime<->wintertime
        }
        time_t tt = timestamp.gettime_t();
        struct tm tm;
        localtime_s(&tm, &tt);
        verify(tm.tm_year == year - 1900 && tm.tm_mon == month - 1 && tm.tm_mday == day
             && tm.tm_hour == hour && tm.tm_min == minute && tm.tm_sec == second);
/*          _tprintf(_T("time(%s) != localtime(%02d.%02d.%d %02d:%02d:%02d)\n"),
            timestamp.toString().cstr(),
            tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
*/
      }
    } // method

    TEST_METHOD(DateGetDATE) {
      Date date;
      for (int i = 0; i < 1000; i++) {
        double d = date.getDATE();
        Date date1(d);
        verify(date == date1);
/*
          _tprintf(_T("date=<%s>, date1 = Date(date.getDATE())=<%s>\n")
            , date.toString(ddMMyyyy).cstr()
            , date1.toString(ddMMyyyy).cstr()
          );
*/
        date.add(TDAYOFMONTH, 1);
      }
    } // method

    void testGetWeek(int day, int month, int yy, int expectedWeek) {
      Date d(day, month, yy);
      DateArray.add(d);
      const int w = d.getWeek();
      verify(w == expectedWeek);
/*
        _tprintf(_T("Error in weeknumber for ")); printdate(d);
        _tprintf(_T(". Calculated week:%d, Expected week:%d\n"), w, expectedWeek);
*/
    }

    TEST_METHOD(DateGetWeek) {
      testGetWeek(29, 12, 2002, 52);
      testGetWeek(30, 12, 2002,  1);
      testGetWeek(31, 12, 2002,  1);
      testGetWeek( 1,  1, 2003,  1);
      testGetWeek( 5,  1, 2003,  1);
      testGetWeek( 6,  1, 2003,  2);
      testGetWeek(22, 12, 2003, 52);
      testGetWeek(28, 12, 2003, 52);
      testGetWeek(29, 12, 2003,  1);
      testGetWeek(31, 12, 2003,  1);
      testGetWeek( 1,  1, 2004,  1);
      testGetWeek( 4,  1, 2004,  1);
      testGetWeek( 5,  1, 2004,  2);
      testGetWeek(26, 12, 2004, 52);
      testGetWeek(27, 12, 2004, 53);
      testGetWeek(31, 12, 2004, 53);
      testGetWeek( 1,  1, 2005, 53);
      testGetWeek( 2,  1, 2005, 53);
      testGetWeek( 3,  1, 2005,  1);
      testGetWeek(25, 12, 2005, 51);
      testGetWeek(26, 12, 2005, 52);
      testGetWeek(31, 12, 2005, 52);
      testGetWeek( 1,  1, 2006, 52);
      testGetWeek( 2,  1, 2006,  1);
      testGetWeek( 8,  1, 2006,  1);
      testGetWeek( 9,  1, 2006,  2);
      testGetWeek(24, 12, 2006, 51);
      testGetWeek(25, 12, 2006, 52);
      testGetWeek(31, 12, 2006, 52);
      testGetWeek( 1,  1, 2007,  1);
      testGetWeek( 7,  1, 2007,  1);
      testGetWeek( 8,  1, 2007,  2);
      testGetWeek(23, 12, 2007, 51);
      testGetWeek(24, 12, 2007, 52);
      testGetWeek(30, 12, 2007, 52);
      testGetWeek(31, 12, 2007,  1);
      testGetWeek( 1,  1, 2008,  1);
      testGetWeek( 6,  1, 2008,  1);
      testGetWeek( 7,  1, 2008,  2);
      testGetWeek(21, 12, 2008, 51);
      testGetWeek(22, 12, 2008, 52);
      testGetWeek(28, 12, 2008, 52);
      testGetWeek(29, 12, 2008,  1);
      testGetWeek(31, 12, 2008,  1);
      testGetWeek( 1,  1, 2009,  1);
      testGetWeek( 4,  1, 2009,  1);
      testGetWeek( 5,  1, 2009,  2);
      testGetWeek(20, 12, 2009, 51);
      testGetWeek(21, 12, 2009, 52);
      testGetWeek(27, 12, 2009, 52);
      testGetWeek(28, 12, 2009, 53);
      testGetWeek(31, 12, 2009, 53);
      testGetWeek( 1,  1, 2010, 53);
      testGetWeek( 3,  1, 2010, 53);
      testGetWeek( 4,  1, 2010,  1);
      testGetWeek(10,  1, 2010,  1);
      testGetWeek(11,  1, 2010,  2);
      testGetWeek(20, 12, 2010, 51);
      testGetWeek(26, 12, 2010, 51);
      testGetWeek(27, 12, 2010, 52);
      testGetWeek(31, 12, 2010, 52);
      testGetWeek( 1,  1, 2011, 52);
      testGetWeek( 2,  1, 2011, 52);
      testGetWeek( 3,  1, 2011,  1);
      testGetWeek(19, 12, 2011, 51);
      testGetWeek(25, 12, 2011, 51);
      testGetWeek(26, 12, 2011, 52);
      testGetWeek(31, 12, 2011, 52);
      testGetWeek( 1,  1, 2012, 52);
      testGetWeek( 2,  1, 2012,  1);
      testGetWeek( 8,  1, 2012,  1);
      testGetWeek( 9,  1, 2012,  2);
      testGetWeek(24, 12, 2012, 52);
      testGetWeek(30, 12, 2012, 52);
      testGetWeek(31, 12, 2012,  1); // her siger outlook express uge 53 !!
      testGetWeek( 1,  1, 2013,  1);
      testGetWeek( 6,  1, 2013,  1);
      testGetWeek( 7,  1, 2013,  2);
      testGetWeek(22, 12, 2013, 51);
      testGetWeek(23, 12, 2013, 52);
      testGetWeek(29, 12, 2013, 52);
      testGetWeek(30, 12, 2013,  1);
      testGetWeek(31, 12, 2013,  1);
      testGetWeek( 1,  1, 2014,  1);
      testGetWeek( 5,  1, 2014,  1);
      testGetWeek( 6,  1, 2014,  2);
      testGetWeek(28, 12, 2014, 52);
      testGetWeek(29, 12, 2014,  1);
      testGetWeek(31, 12, 2014,  1);
      testGetWeek( 1,  1, 2015,  1);
      testGetWeek( 4,  1, 2015,  1);
      testGetWeek( 5,  1, 2015,  2);
      testGetWeek(27, 12, 2015, 52);
      testGetWeek(28, 12, 2015, 53);
      testGetWeek(31, 12, 2015, 53);
      testGetWeek( 1,  1, 2016, 53);
      testGetWeek( 4,  1, 2016,  1);

      testGetWeek(25, 12, 2016, 51);
      testGetWeek(26, 12, 2016, 52);
      testGetWeek(31, 12, 2016, 52);
      testGetWeek( 1,  1, 2017, 52);
      testGetWeek( 2,  1, 2017,  1);

      testGetWeek(24, 12, 2017, 51);
      testGetWeek(25, 12, 2017, 52);
      testGetWeek(31, 12, 2017, 52);
      testGetWeek( 1,  1, 2018,  1);

      testGetWeek(23, 12, 2018, 51);
      testGetWeek(24, 12, 2018, 52);
      testGetWeek(30, 12, 2018, 52);
      testGetWeek(31, 12, 2018,  1);
      testGetWeek( 1,  1, 2019,  1);
    }

    /*
      Author: Charles B. Chapman, London, Ontario, Canada [74370,516]
      Thanks to Leonard Erickson who supplied a test suite of values.

      Algorithm "E" from Knuth's "Art of Computer Programming", vol. 1
      Computes date of Easter for any year in the Gregorian calendar
      The local variables are the variable names used by Knuth.
      GIVEN:   Yr - year
      RETURNS: Mo - month of Easter (3 or 4)
                Da - day of Easter

      Bemærkning:
          Funktionen er testet op imod DATE.EasterDay og de giver samme
          output såfremt: 1500 < year < 2300.
    */
    bool DatoForPaaske(const int yyyy, int &mm, int &dd) {
      if (yyyy >= 10000) {
        return false;
      }

      // Golden number of the year in Metonic cycle
      int m_G = yyyy % 19 + 1;

      // Century
      int m_C = yyyy / 100 + 1;

      // Corrections:
      // <X> is the no. of years in which leap-year was dropped in
      // order to keep step with the sun
      // <Z> is a special correction to synchronize Easter with the
      // moon's orbit.
      int m_X = (3 * m_C) / 4 - 12;
      int m_Z = (8 * m_C + 5) / 25 - 5;
      // <D> Find Sunday
      int m_D = (5 * yyyy) / 4 - m_X - 10;
      // Set Epact
      int m_E = (11 * m_G + 20 + m_Z - m_X) % 30;

      if (m_E < 0) {
        m_E = m_E + 30;
      }

      if (((m_E == 25) && (m_G > 11)) || (m_E == 24)) {
        m_E = m_E + 1;
      }

      // Find full moon - the Nth of MARCH is a "calendar" full moon
      int m_N = 44 - m_E;

      if (m_N < 21) {
        m_N = m_N + 30;
      }

      // Advance to Sunday
      m_N = m_N + 7 - ((m_D + m_N) % 7);

      // Get Month and Day
      if (m_N > 31) {
        mm = 4;
        dd = m_N - 31;
      } else {
        mm = 3;
        dd = m_N;
      }
      return true;
    }

    // from http://astro.nmsu.edu/~lhuber/leaphist.html
    void ecclesiastical_DatoForPaaske(const int Y, int &mm, int &dd) {
      int C = Y / 100;
      int N = Y % 19;
      int I = (C - C/4 - (C - (C - 17)/25)/3 + 19*N + 15) % 30;
      I -= (I/28)*(1 - (I/28)*(29/(I + 1))*((21 - N)/11));
      int J = (Y + Y/4 + I + 2 - C + C/4) % 7;
      int L = I - J;
      int M = 3 + (L + 40)/44;
      int D = L + 28 - 31*(M/4);

      mm = M;
      dd = D;
    }

    TEST_METHOD(TestEaster) {
      for(int year = 1600; year < 4100; year++) {
        const Date date = Date::getEaster(year);
        int d,m,y;
        date.getDMY(d,m,y);
        int month1,day1,month2,day2;
        DatoForPaaske(year,month1,day1);
        ecclesiastical_DatoForPaaske(year,month2,day2);
        verify(y == year  );
        verify(d == day1  );
        verify(m == month1);
        verify((day1 == day2) && (month1 == month2));
      }
    }

    TEST_METHOD(DateConvertionSYSTEMTIME) {
      Timestamp ts;
      for (int i = 0; i < 1000; i++) {
        SYSTEMTIME sysTime = ts;
        Timestamp ts1 = sysTime;
        verify(ts1 == ts);
/*          _tprintf(_T("ts=<%s>, ts1 = Timestamp(SYSTEMTIME(ts))=<%s>\n")
            , ts.toString(ddMMyyyyhhmmssSSS).cstr()
            , ts1.toString(ddMMyyyyhhmmssSSS).cstr()
          );
*/
        ts.add(TDAYOFMONTH, 1).add(THOUR, 5).add(TMINUTE, 34).add(TSECOND, 17).add(TMILLISECOND, 389);
      }
    }

    TEST_METHOD(DateTimestampGetDATE) {
      Timestamp ts;
      for (int i = 0; i < 1000; i++) {
        double d = ts.getDATE();
        Timestamp ts1(d);
        verify(fabs(diff(ts1, ts, TMILLISECOND)) <= 1);
/*          _tprintf(_T("ts=<%s>, ts1 = Timestamp(ts.getDATE())=<%s>\n")
            , ts.toString(ddMMyyyyhhmmssSSS).cstr()
            , ts1.toString(ddMMyyyyhhmmssSSS).cstr()
          );
*/
        ts.add(TMONTH, 7).add(TDAYOFMONTH, 1).add(THOUR, 5).add(TMINUTE, 34).add(TSECOND, 17).add(TMILLISECOND, 389);
      }
    }

    TEST_METHOD(DateTimestamp) {
      int day, month, year, hour, minute, second, millisecond;
      Timestamp ts(_T("28.2.2008 23:59:59:999"));
      ts.getDMY(day, month, year);
      ts.getHMS(hour, minute, second, millisecond);
      verify(year == 2008 && month == 2 && day == 28 && hour == 23 && minute == 59 && second == 59 && millisecond == 999);
//        _tprintf(_T("Timestamp mismatch\n"));
      /*
      TCHAR tmp[100];
      Date d(_T("2.6.2457"));
      _tprintf(_T("%s\n"),d.tostr(tmp));
      d.add(MONTH,42);
      _tprintf(_T("%s\n"),d.tostr(tmp));
      return;
      */
      Timestamp t;
      Date d;
      verify(t.getDate() == d);
//        _tprintf(_T("Timestamp != Date"));
      for (int i = 0;i < 2000;i++) {
        const TimeComponentMinMax &comp = components[randInt(ARRAYSIZE(components))];
        const int count = randInt(-150, 150);
        Timestamp n = t;
        try {
          n.add(comp.c, count);
        }  catch (Exception e) {
          //      _tprintf(_T("caught Exception:%s\n"),e.what());
          t = Timestamp();
          continue;
        }
        double difference = n - t;
        double expected;
        switch (comp.c) {
        case TMILLISECOND : expected = (double)count / 86400000;    break;
        case TSECOND      : expected = (double)count / 86400;       break;
        case TMINUTE      : expected = (double)count / (24 * 60);   break;
        case THOUR        : expected = (double)count / 24;          break;
        case TDAYOFMONTH  :
        case TDAYOFYEAR   : expected = count;                       break;
        case TWEEK        : expected = count * 7;                   break;
        case TMONTH       :
          difference = count;
          expected = (n.getYear() - t.getYear()) * 12 + (n.getMonth() - t.getMonth());
          break;
        case TYEAR:
          difference = count;
          expected = n.getYear() - t.getYear();
          break;
        }
        verify(fabs(difference - expected) <= 1e-3);
/*          _tprintf(_T("%d comp=%s count:%d difference:%lf expected:%lf delta:%lf,t:%s n:%s\n")
            , i, comp.name, count, difference, expected, difference - expected, t.toString().cstr(), n.toString().cstr());
*/
        t = n;
      }
    } // method


    TEST_METHOD(DateToString) {
      Date date;
      String s = date.toString();
      Time time;
      s = time.toString();
      Timestamp timestamp;
      s = timestamp.toString();
    }

  }; // class
} // namespace
