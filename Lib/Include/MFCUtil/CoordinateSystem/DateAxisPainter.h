#pragma once

#include <Date.h>
#include "LinearAxisPainter.h"

class DateAxisPainter : public LinearAxisPainter {
private:
  static double findFirstMillisecond(double factor, int step);
  static double firstInSecond(double factor, int step);
  static double firstInMinute(double factor, int step);
  static double firstInHour(  double factor, int step);
  static double firstInMonth( double factor, int step);
  static double firstInYear(  double factor, int step);
  TimeComponent m_timeComponent;
  String        m_timeFormat;

  void   setMinMaxStep(double min, double max, double step, TimeComponent timeComponent);
protected:
  void init();
  double next(double x) const;

public:
  DateAxisPainter(SystemPainter &systemPainter, bool xAxis);
  AxisType getType() const { return AXIS_DATE; }
  String getValueText(double v);
};
