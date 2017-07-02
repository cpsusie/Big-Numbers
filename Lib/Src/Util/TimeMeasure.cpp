#include "pch.h"
#include <Thread.h>
#include <TimeMeasure.h>

//#pragma comment(lib, "Mincore.lib")

typedef struct {
  double m;
  int factor;
} TimeLimit;

static const TimeLimit timeLimit[] = {
  2e-6,11
 ,2e-6,11
 ,2e-5,11
 ,2e-4,11
 ,2e-3,11
 ,2e-2,11
 ,2e-1,3
};

double static getTime(TimeMeasureType timeType) {
  switch(timeType) {
  case MEASURE_THREADTIME : return getThreadTime();
  case MEASURE_PROCESSTIME: return getProcessTime();
  case MEASURE_REALTIME   : return getSystemTime();
  default                 : throwInvalidArgumentException(__TFUNCTION__, _T("timeType=%d"), timeType);
                            return 1000;
  }
}

static double measureThreadTime(MeasurableFunction &ftm) {
//  UINT64 interruptTime;
//  QueryInterruptTimePrecise(&interruptTime);
//  const double interruptTimeSec = (double)interruptTime / 1e7;
  const double interruptTimeSec = 150E-6;

  LARGE_INTEGER freq;
  QueryPerformanceFrequency(&freq);
  UINT  callCount    = 1;
  UINT  sumCallCount = 0;
  INT64 sumTicks     = 0;
  for(;;) {
    LARGE_INTEGER startTicks;
    LARGE_INTEGER endTicks;
    QueryPerformanceCounter(&startTicks);
    for(UINT i = 0; i < callCount; i++) {
      ftm.f();
    }
    QueryPerformanceCounter(&endTicks);
    sumTicks     += endTicks.QuadPart - startTicks.QuadPart;
    sumCallCount += callCount;
    const double totalTime = (double)sumTicks / freq.QuadPart;
    if (totalTime < 4 * interruptTimeSec) {
      callCount <<= 1;
    } else {
      return totalTime / sumCallCount;
    }
  }
}

double measureTime(MeasurableFunction &ftm, TimeMeasureType timeType) {
  switch (timeType) {
  case MEASURE_THREADTIME : return measureThreadTime( ftm);
  case MEASURE_PROCESSTIME:
  case MEASURE_REALTIME   : break;
  default                 : throwInvalidArgumentException(__TFUNCTION__, _T("timeType=%d"), timeType);
                            return 1000;

  }
  double totalUsage;
  double count = 1;

StartMeasure:
  double startTime = getTime(timeType);
  for(double j = 0; j < count; j++) {
    ftm.f();
  }
  totalUsage = (getTime(timeType) - startTime)/1e6;
  for(int i = 0; i < ARRAYSIZE(timeLimit); i++) {
    if(totalUsage < timeLimit[i].m) {
      count *= timeLimit[i].factor;
      goto StartMeasure;
    }
    break;
  }
//  printf("Count:%10le Time:%.3le\n",count,totalUsage);
  return totalUsage / count;
}
