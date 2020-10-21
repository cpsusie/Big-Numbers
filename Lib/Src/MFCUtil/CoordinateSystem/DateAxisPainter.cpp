#include "pch.h"
#include <MFCUtil/Coordinatesystem/DateAxisPainter.h>

// #define _TEST

DateAxisPainter::DateAxisPainter(SystemPainter &systemPainter, AxisIndex axisIndex)
: LinearAxisPainter(systemPainter,axisIndex, AXIS_DATE, false)
{
  doInvisiblePaint();
}

static const int possible60Step[]    = {  1, 2, 5, 10, 15, 20, 30, 60     };
static const int possibleHourStep[]  = {  1, 2, 3,  4,  6,  8, 12, 24     };
static const int possibleMonthStep[] = {  1, 2, 3,  6, 12, 24, 36, 48     };

static int findNiceStep(double range, const int *possibleStep, size_t stepCount, const TCHAR *unitName) {
  int result = 0;
  for(size_t i = 0; i < stepCount; i++) {
    const int step = possibleStep[i];
    if(range <= step) {
      result = step;
      break;
    }
  }
  if(result == 0) {
    result = possibleStep[stepCount-1];
  }
#if defined(_TEST)
  debugLog(_T("%-20s:range:%.10le step:%d\n"), unitName, range, result);
#endif
  return result;
}

static int findNiceStep(double range, const TCHAR *unitName) {
  int result = (int)AbstractAxisPainter::findNiceDecimalStep(range);
  result = max(1, result);
#if defined(_TEST)
  debugLog(_T("%-20s:range:%.10le step:%d\n"), unitName, range, result);
#endif
  return result;
}

class ScaleFactors {
#if defined(_TEST)
private:
  void checkName(const String &str, const String &expected) const;
  void readData();
  bool      m_loaded;
  Timestamp m_lastUpdateTime;
#endif _TEST
public:
  UINT   msDiv     , secDiv     , minDiv     , hrDiv                                ,monDiv       , yrDiv;
  double maxMsCount, maxSecCount, maxMinCount, maxHrCount, maxDayCount, maxWeekCount,maxMonthCount;
  ScaleFactors();
#if defined(_TEST)
  ~ScaleFactors();
  void update();
#endif _TEST
};

ScaleFactors::ScaleFactors() :
  msDiv(1)
, secDiv(20)
, minDiv(20)
, hrDiv(14)
, monDiv(8)
, yrDiv(1)
, maxMsCount(10000)
, maxSecCount(600)
, maxMinCount(480)
, maxHrCount(112)
, maxDayCount(60)
, maxWeekCount(25)
, maxMonthCount(60)
#if defined(_TEST)
, m_loaded(false)
{
  update();
}
#else
{
}
#endif _TEST

#if defined(_TEST)
#define LOADUINT(field) { _ftscanf( f,_T("%s %u\n" ), tmpStr, &field); checkName(tmpStr,_T(#field)); }
#define LOADDBL( field) { _ftscanf( f,_T("%s %le\n"), tmpStr, &field); checkName(tmpStr,_T(#field)); }
#define DUMPUINT(field)   _ftprintf(f,_T("%-20s %u\n" ), _T(#field), field)
#define DUMPDBL( field)   _ftprintf(f,_T("%-20s %le\n"), _T(#field), field)
#define SCALEFILENAME  _T("c:\\temp\\factors.txt")

void ScaleFactors::checkName(const String &str, const String &expected) const {
  if(str != expected) {
    int fisk = 1;
  }
}

void ScaleFactors::readData() {
  FILE *f = FOPEN(SCALEFILENAME, _T("r"));
  TCHAR tmpStr[100];
  LOADUINT(msDiv          );
  LOADUINT(secDiv         );
  LOADUINT(minDiv         );
  LOADUINT(hrDiv          );
  LOADUINT(monDiv         );
  LOADUINT(yrDiv          );
  LOADDBL( maxMsCount     );
  LOADDBL( maxSecCount    );
  LOADDBL( maxMinCount    );
  LOADDBL( maxHrCount     );
  LOADDBL( maxDayCount    );
  LOADDBL( maxWeekCount   );
  LOADDBL( maxMonthCount  );
  fclose(f);
}

void ScaleFactors::update() {
  try {
    if(!m_loaded || (diff(m_lastUpdateTime, Timestamp(), TSECOND) > 2)) {
      STAT(SCALEFILENAME);
      readData();
      m_loaded         = true;
      m_lastUpdateTime = Timestamp();
    }
  } catch(Exception e) {
  }
}

ScaleFactors::~ScaleFactors() {
  FILE *f = MKFOPEN(SCALEFILENAME, _T("w"));
  DUMPUINT(msDiv          );
  DUMPUINT(secDiv         );
  DUMPUINT(minDiv         );
  DUMPUINT(hrDiv          );
  DUMPUINT(monDiv         );
  DUMPUINT(yrDiv          );
  DUMPDBL( maxMsCount     );
  DUMPDBL( maxSecCount    );
  DUMPDBL( maxMinCount    );
  DUMPDBL( maxHrCount     );
  DUMPDBL( maxDayCount    );
  DUMPDBL( maxWeekCount   );
  DUMPDBL( maxMonthCount  );
  fclose(f);
}
#endif _TEST

static ScaleFactors s_f;

#define LENGTH_OF_DAY          1.0
#define LENGTH_OF_HOUR        (LENGTH_OF_DAY    /   24.0 )
#define LENGTH_OF_MINUTE      (LENGTH_OF_HOUR   /   60.0 )
#define LENGTH_OF_SECOND      (LENGTH_OF_MINUTE /   60.0 )
#define LENGTH_OF_MILLISECOND (LENGTH_OF_SECOND / 1000.0 )
#define LENGTH_OF_WEEK        (LENGTH_OF_DAY    *    7.0 )
#define LENGTH_OF_MONTH       (LENGTH_OF_DAY    *   30.5 )
#define LENGTH_OF_YEAR        (LENGTH_OF_DAY    *  365.25)

#define FIND_NICESTEP( range, unitName)            findNiceStep(range, _T(unitName))
#define FIND_NICESTEPT(range, stepTable, unitName) findNiceStep(range, stepTable, ARRAYSIZE(stepTable), _T(unitName))

#define FIND_NICEMILLISECONDSTEP(range,d)     FIND_NICESTEP ((range)/(d)/ LENGTH_OF_MILLISECOND                   , "msecond")
#define FIND_NICESECONDSTEP(     range,d)     FIND_NICESTEPT((range)/(d)/ LENGTH_OF_SECOND     , possible60Step   , "second" )
#define FIND_NICEMINUTSTEP(      range,d)     FIND_NICESTEPT((range)/(d)/ LENGTH_OF_MINUTE     , possible60Step   , "minute" )
#define FIND_NICEHOURSTEP(       range,d)     FIND_NICESTEPT((range)/(d)/ LENGTH_OF_HOUR       , possibleHourStep , "hour"   )
#define FIND_NICEMONTHSTEP(      range,d)     FIND_NICESTEPT((range)/(d)/ LENGTH_OF_MONTH      , possibleMonthStep, "month"  )
#define FIND_NICEYEARSTEP(       range,d)     FIND_NICESTEP ((range)/(d)/ LENGTH_OF_YEAR                          , "year"   )

void DateAxisPainter::setMinMaxStep(double min, double max, double step, TimeComponent timeComponent) {
  __super::setMinMaxStep(min,max,step);
  m_timeComponent = timeComponent;
}

#if !defined(_TEST)

#define SETMINMAXSTEP(Min,Max,Step,tc) setMinMaxStep(Min, Max, Step, tc)

#else
#define SETMINMAXSTEP(Min,Max,Step,tc)                                      \
{ const double _min  = Min, _max = Max, _step = Step;                       \
  const TimeComponent _tc = tc;                                             \
  debugLog(_T("%-20s:range:%s len:%8lg, min:%s, max:%s, step:%lg, tc:%s\n") \
          ,__TFUNCTION__                                                    \
          ,getDataRange().toString().cstr()                                 \
          ,range                                                            \
          ,Timestamp(_min).toString(ddMMyyyyhhmmssSSS).cstr()               \
          ,Timestamp(_max).toString(ddMMyyyyhhmmssSSS).cstr()               \
          ,_step                                                            \
          ,_T(#tc));                                                        \
  setMinMaxStep(_min, _max, _step, _tc);                                    \
}
#endif _TEST

void DateAxisPainter::init() {
#if defined(_TEST)
  s_f.update();
#endif _TEST

  const double minimum = getDataRange().getMin();
  const double maximum = getDataRange().getMax();
  const double range   = maximum - minimum;
  int          step;
  if(       range <= s_f.maxMsCount    * LENGTH_OF_MILLISECOND) { //                range <= 10 sec    => divide in milliseconds
    step = FIND_NICEMILLISECONDSTEP(range,s_f.msDiv);
    SETMINMAXSTEP(findFirstMillisecond(minimum,step), maximum  , step                      , TMILLISECOND);
  } else if(range <= s_f.maxSecCount   * LENGTH_OF_SECOND     ) { //  10 sec      < range <=  10 min   => divide in seconds
    step = FIND_NICESECONDSTEP(range,s_f.secDiv);
    SETMINMAXSTEP(firstInSecond(minimum,step)       , maximum  , step                      , TSECOND     );
  } else if(range <= s_f.maxMinCount   * LENGTH_OF_MINUTE     ) { //  10 min      < range <=   8 hours => divide in minutes
    step = FIND_NICEMINUTSTEP(range,s_f.minDiv);
    SETMINMAXSTEP(firstInMinute(minimum,step)       , maximum  , step                      , TMINUTE     );
  } else if(range <= s_f.maxHrCount    * LENGTH_OF_HOUR       ) { //   8 hours    < range <=   2 weeks => divide in hours
    step = FIND_NICEHOURSTEP(range,s_f.hrDiv  );
    SETMINMAXSTEP(firstInHour(  minimum,step)       , maximum  , step                      , THOUR       );
  } else if(range <= s_f.maxDayCount   * LENGTH_OF_DAY        ) { //   2 weeks    < range <=  60 days  => divide in days  (1)
    step = 1;
    SETMINMAXSTEP(firstInDay(   minimum,1   )       , maximum  , step                      , TDAYOFMONTH );
  } else if(range <= s_f.maxWeekCount  * LENGTH_OF_WEEK       ) { //  60 days     < range <= 180 days  => divide in 2 weeks (1,15,1,15...)
    step = 2;
    SETMINMAXSTEP(firstInMonth( minimum,1   )        , maximum  , step                      , TWEEK       );
  } else if(range <= s_f.maxMonthCount * LENGTH_OF_MONTH      ) { // 180 days     < range <=   5 years  => divide in month
    step = FIND_NICEMONTHSTEP(range,s_f.monDiv);
    SETMINMAXSTEP(firstInYear(  minimum,1   )       , maximum  , step                      , TMONTH      );
  } else {                                                        //   10 years   < range              => divide in years (2,5,10,20,50,...
    step = FIND_NICEYEARSTEP(range,s_f.yrDiv  );
    SETMINMAXSTEP(firstInYear(  minimum,step)       , maximum  , step                      , TYEAR       );
  }
}

String DateAxisPainter::getValueText(double v) const {
  Timestamp t(v);
  if(m_timeComponent == TMILLISECOND) {
    return t.toString(ssSSS);
  }

  t.add(TMILLISECOND, 50); // add a small amount to fix rounding error from double, so we wont get 1.59.59 instead of 2
  switch(m_timeComponent) {
  case TSECOND     : return t.toString(isMouseMode() ? ddMMhhmmss : mmss);
  case TMINUTE     : return t.toString(isMouseMode() ? ((getStep()<=5) ? ddMMhhmmss : ddMMhhmm) : hhmm  );
  case THOUR       : return t.toString(isMouseMode() ? ddMMyyhhmm : hhmm  );
  case TDAYOFMONTH : return t.toString(isMouseMode() ? ddMM : ddMMyy);
  case TWEEK       : return t.toString(isMouseMode() ? ddMM : ddMMyy);
  case TMONTH      : return t.toString(isMouseMode() ? ddMM : MMyyyy);
  case TYEAR       : return t.toString(yyyy);
  default          : throwException(_T("%s:Illegal timecomponent(=%d)"),__TFUNCTION__,(int)m_timeComponent);
                     return EMPTYSTRING;
  }
}

double DateAxisPainter::next(double x) const {
  const double step = getStep();
  switch(m_timeComponent) {
  case TWEEK:
    { Timestamp ts(x);
      int dd, mm, yyyy;
      ts.getDMY(dd, mm, yyyy);
      if(dd < 15) {
        ts.set(TDAYOFMONTH, 15);
      } else {
        ts.add(TMONTH, 1).set(TDAYOFMONTH, 1);
      }
      return ts.getDATE();
    }
  }
  return Timestamp(x).add(m_timeComponent,(int)step).getDATE();
}

#if !defined(_TEST)
#define RETURNTSTODATE(ts) return (ts).getDATE()
#else
#define RETURNTSTODATE(ts)                                                      \
{ const Timestamp &_t = (ts);                                                   \
  debugLog(_T("%-20s:%s, step:%d\n"),__TFUNCTION__,_t.toString().cstr(), step); \
  return _t.getDATE();                                                          \
}
#endif

double DateAxisPainter::findFirstMillisecond(double factor, int step) { // static
  Timestamp ts(factor);
  const int ms = ts.getMilliSecond();
  RETURNTSTODATE(ts.set(TMILLISECOND, ms - ms%step));
}

double DateAxisPainter::firstInSecond(double factor, int step) { // static
  Timestamp ts(factor);
  int h, m, s, ms;
  ts.getHMS(h, m, s, ms);
  RETURNTSTODATE(ts.setTime(Time(h,m,s - s%step)));
}

double DateAxisPainter::firstInMinute(double factor, int step) { // static
  Timestamp ts(factor);
  int h, m, s, ms;
  ts.getHMS(h, m, s, ms);
  RETURNTSTODATE(ts.setTime(Time(h, m - m%step,0)));
}

double DateAxisPainter::firstInHour(double factor, int step) { // static
  Timestamp ts(factor);
  int h, m, s, ms;
  ts.getHMS(h, m, s, ms);
  RETURNTSTODATE(ts.setTime(Time(h - h%step,0,0)));
}

static const Time midNight(0, 0, 0);

double DateAxisPainter::firstInDay(double factor, int step) { // static
  Timestamp ts(factor);
  RETURNTSTODATE(ts.setTime(midNight));
}

double DateAxisPainter::firstInMonth(double factor, int step) { // static
  Timestamp ts(factor);
  const int m = ts.getMonth()-1; // m = [0..11]
  RETURNTSTODATE(ts.setTime(midNight).set(TDAYOFMONTH, 1).set(TMONTH, (m - (m%step)%12) + 1));
}

double DateAxisPainter::firstInYear(double factor, int step) { // static
  Timestamp ts(factor);
  const int y = ts.getYear();
  if(step != 1) {
    ts.set(TYEAR, y - y%step);
  }
  RETURNTSTODATE(ts.setTime(midNight).set(TDAYOFYEAR,1));
}
