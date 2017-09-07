#include "pch.h"
#include <MFCUtil/Coordinatesystem/DateAxisPainter.h>

DateAxisPainter::DateAxisPainter(SystemPainter &systemPainter, bool xAxis) : LinearAxisPainter(systemPainter,xAxis, false) {
  doInvisiblePaint();
}

static const int possible60Step[]    = {  1, 2, 5, 10, 15, 20, 30, 60     };
static const int possibleHourStep[]  = {  1, 2, 3,  4,  6,  8, 12, 24     };
static const int possibleMonthStep[] = {  1, 2, 3,  4,  6, 12, 24, 36, 48 };
/*
static const char *logName = "c:\\temp\\axisStep.log";

static void logStep(const char *format,...) {
  va_list argptr;
  va_start(argptr,format);
  FILE *f = FOPEN(logName,"a");
  vfprintf(f,format,argptr);
  fclose(f);
  va_end(argptr);
}
*/

static int findNiceStep(double range, const int *possibleStep, int stepCount, const char *unitName) {
  int result = 0;
  for(int i = 0; i < stepCount; i++) {
    const int step = possibleStep[i];
    if(range <= step) {
      result = step;
      break;
    }
  }
  if(result == 0) {
    result = possibleStep[stepCount-1];
  }
//  logStep("%-8s:range:%.10le step:%d\n", unitName, range, result);
  return result;
}

static int findNiceStep(double range, const char *unitName) {
  const int result = (int)AbstractAxisPainter::findNiceDecimalStep(range);
//  logStep("%-8s:range:%.10le step:%d\n", unitName, range, result);
  return result;
}

#define LENGTH_OF_DAY          1.0
#define LENGTH_OF_HOUR        (LENGTH_OF_DAY    /   24.0 )
#define LENGTH_OF_MINUTE      (LENGTH_OF_HOUR   /   60.0 )
#define LENGTH_OF_SECOND      (LENGTH_OF_MINUTE /   60.0 )
#define LENGTH_OF_MILLISECOND (LENGTH_OF_SECOND / 1000.0 )
#define LENGTH_OF_MONTH       (LENGTH_OF_DAY    *   30.5 )
#define LENGTH_OF_YEAR        (LENGTH_OF_DAY    *  365.25)

#define FIND_NICESTEP(range, stepTable, unitName) findNiceStep(range, stepTable, ARRAYSIZE(stepTable), unitName)

#define FIND_NICEMILLISECONDSTEP(range)     findNiceStep( (range)/LENGTH_OF_MILLISECOND                 , "msecond")
#define FIND_NICESECONDSTEP(     range)     FIND_NICESTEP((range)/20/LENGTH_OF_SECOND, possible60Step   , "second" )
#define FIND_NICEMINUTSTEP(      range)     FIND_NICESTEP((range)/20/LENGTH_OF_MINUTE, possible60Step   , "minute" )
#define FIND_NICEHOURSTEP(       range)     FIND_NICESTEP((range)/20/LENGTH_OF_HOUR  , possibleHourStep , "hour"   )
#define FIND_NICEDAYSTEP(        range)     findNiceStep( (range)/LENGTH_OF_DAY                         , "day"    )
#define FIND_NICEMONTHSTEP(      range)     FIND_NICESTEP((range)/20/LENGTH_OF_MONTH , possibleMonthStep, "month"  )
#define FIND_NICEYEARSTEP(       range)     findNiceStep( (range)/LENGTH_OF_YEAR                        , "year"   )

void DateAxisPainter::init() {
  double minimum = getDataRange().getMin();
  double maximum = getDataRange().getMax();
  double range   = maximum - minimum;

  if(       range <= 10 * LENGTH_OF_SECOND) {  //                range <= 10 sec    => divide in milliseconds
    int step = FIND_NICEMILLISECONDSTEP(range);
    step = max(step, 1);
    setMinMaxStep(findFirstMillisecond(minimum,step), maximum  , step                      , TMILLISECOND);
  } else if(range <= 10 * LENGTH_OF_MINUTE ) { //  10 sec      < range <=  10 min   => divide in seconds
    int step = FIND_NICESECONDSTEP(range);
    setMinMaxStep(firstInSecond(minimum,step)       , maximum  , step                      , TSECOND     );
  } else if(range <= 8 * LENGTH_OF_HOUR    ) { //  10 min      < range <=   8 hours => divide in minutes
    int step = FIND_NICEMINUTSTEP(range);
    setMinMaxStep(firstInMinute(minimum,step)       , maximum  , step                      , TMINUTE     );
  } else if(range <= 14.0 * LENGTH_OF_DAY  ) { //   8 hours    < range <=   2 weeks => divide in hours
    int step = FIND_NICEHOURSTEP(range);
    setMinMaxStep(firstInHour(  minimum,step)       , maximum  , step                      , THOUR       );
  } else if(range <= 60.0 * LENGTH_OF_DAY  ) { //   2 weeks    < range <=  60 days  => divide in days  (1)
    setMinMaxStep(minimum                           , maximum  , 1                         , TDAYOFMONTH );
  } else if(range <= 180.0 * LENGTH_OF_DAY ) { //  60 days     < range <= 180 days  => divide in days  (5,10,20)
    int step = FIND_NICEDAYSTEP(range);
    setMinMaxStep(minimum                           , maximum  , step                      , TDAYOFMONTH );
  } else if(range <= 10.0 * LENGTH_OF_YEAR) {  // 120 days     < range <= 10 years => divide in month
    int step = FIND_NICEMONTHSTEP(range);
    setMinMaxStep(firstInMonth( minimum,step)       , maximum  , step                      , TMONTH      );
  } else {                                     //   10 years   < range             => divide in years (2,5,10,20,50,...
    int step = FIND_NICEYEARSTEP(range);
    step = max(step,1);
    setMinMaxStep(firstInYear(minimum, step)        , maximum  , step                      , TYEAR       );
  }
}

String DateAxisPainter::getText(double x) {
  Timestamp t(x);
  if(m_timeComponent == TMILLISECOND) {
    return t.toString(_T("ss:SSS"));
  }

  t.add(TMILLISECOND, 50); // add a small amount to fix rounding error from double, so we wont get 1.59.59 instead of 2
  switch(m_timeComponent) {
  case TSECOND     : return t.toString(hhmmss);
  case TMINUTE     : return t.toString(hhmm);
  case THOUR       : return t.toString(hhmm);
  case TDAYOFMONTH : return t.toString(ddMMyy);
  case TMONTH      : return t.toString(MMyyyy);
  case TYEAR       : return t.toString(yyyy);
  default          : throwException(_T("%s:Illegal timecomponent(=%d)"),__TFUNCTION__,(int)m_timeComponent);
                     return EMPTYSTRING;
  }
}

void DateAxisPainter::setMinMaxStep(double min, double max, double step, TimeComponent timeComponent) {
  __super::setMinMaxStep(min,max,step);
  m_timeComponent = timeComponent;
}

double DateAxisPainter::next(double x) const {
  return Timestamp(x).add(m_timeComponent,(int)getStep()).getDATE();
}

double DateAxisPainter::findFirstMillisecond(double factor, int step) { // static
  Timestamp t(factor);
  const int ms = t.getMilliSecond();
  return t.set(TMILLISECOND, ms - ms%step).getDATE();
}

double DateAxisPainter::firstInSecond(double factor, int step) { // static
  Timestamp t(factor);
  const int s = t.getSecond();
  return t.set(TSECOND,s - s%step).set(TMILLISECOND,0).getDATE();
}

double DateAxisPainter::firstInMinute(double factor, int step) { // static
  Timestamp t(factor);
  const int m = t.getMinute();
  return t.set(TMINUTE,m - m%step).set(TSECOND,0).set(TMILLISECOND,0).getDATE();
}

double DateAxisPainter::firstInHour(double factor, int step) { // static
  Timestamp t(factor);
  const int h = t.getHour();
  return t.set(THOUR,h - h%step).set(TMINUTE,0).set(TSECOND,0).set(TMILLISECOND,0).getDATE();
}

double DateAxisPainter::firstInMonth(double factor, int step) { // static
  Timestamp t(factor);
  const int m = t.getMonth()-1; // m = [0..11]
  return t.set(TDAYOFMONTH,1).set(TMONTH,(m - (m%step)%12) + 1)
          .set(THOUR,0).set(TMINUTE,0).set(TSECOND,0).set(TMILLISECOND ,0).getDATE();
}

double DateAxisPainter::firstInYear(double factor, int step) { // static
  Timestamp t(factor);
  const int y = t.getYear();
  if(step != 1) {
    t.set(TYEAR, y - y%step);
  }
  return t.set(TDAYOFYEAR,1).set(TMINUTE,0).set(TSECOND,0).set(TMILLISECOND ,0).getDATE();
}
