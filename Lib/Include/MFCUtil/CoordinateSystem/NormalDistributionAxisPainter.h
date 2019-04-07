#pragma once

#include "AbstractAxisPainter.h"

class NormalDistributionAxisPainter : public AbstractAxisPainter {
private:
  mutable String m_doubleFormat;
  static const String startDecadeFormat;

  int getDecadeCount() const;

  // Values between 0.1 and 0.9 have decade 0.
  // Values below 0.1 has negative decade. Values above 0.9 has positive decade
  static int getDecade(double x);
  static double getDecadeStep( int decade);
  static double getDecadeStart(int decade);
  static double getDecadeStop( int decade);

  void paintXDataMultipleDecades();
  void paintYDataMultipleDecades();
  bool useSuperAxisScale() const;

protected:
  void init();
  void paintXData();
  void paintYData();

public:
  NormalDistributionAxisPainter(SystemPainter &systemPainter, AxisIndex axis);
  AxisType getType() const { return AXIS_NORMAL_DISTRIBUTION; }
  double getAxisPoint() const;
  String getValueText(double v) const;
  const TCHAR *getDoubleFormat() const;
};
