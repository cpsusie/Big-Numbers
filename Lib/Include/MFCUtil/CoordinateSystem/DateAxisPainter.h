#pragma once

#include <Date.h>
#include "LinearAxisPainter.h"

class DateAxisPainter : public LinearAxisPainter {
private:
  static double findFirstMillisecond(double factor, int step);
  static double firstInSecond(double factor, int step);
  static double firstInMinute(double factor, int step);
  static double firstInHour(  double factor, int step);
  static double firstInDay(   double factor, int step);
  static double firstInMonth( double factor, int step);
  static double firstInYear(  double factor, int step);
  TimeComponent m_timeComponent;
  String        m_timeFormat;

  void   setMinMaxStep(double min, double max, double step, TimeComponent timeComponent);
protected:
  void init() override;
  double next(double x) const override;

public:
  DateAxisPainter(SystemPainter &systemPainter, AxisIndex axisIndex);
  String getValueText(double v) const override;
};
