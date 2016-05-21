#pragma once

#include "MyUtil.h"

class MeasurableFunction {
public:
  virtual void f() = 0;
};

typedef enum {
  MEASURE_THREADTIME
 ,MEASURE_PROCESSTIME
 ,MEASURE_REALTIME
} TimeMeasureType;

double measureTime(MeasurableFunction &ftm, TimeMeasureType timeType = MEASURE_THREADTIME); // return time in seconds
