#include "pch.h"
#include <Thread.h>
#include <TimeMeasure.h>

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
  default                 : throwInvalidArgumentException(_T("getTime"), _T("timeType=%d"), timeType);
                            return 1000;
  }
}

double measureTime(MeasurableFunction &ftm, TimeMeasureType timeType) {
  double totalUsage;
  for(double count = 1;;) {
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
    }
    break;
//  printf("Count:%10le Time:%.3le\n",count,totalUsage);
  }
  return totalUsage / count;
}

