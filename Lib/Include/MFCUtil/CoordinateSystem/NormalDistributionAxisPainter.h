#pragma once

#include "AbstractAxisPainter.h"

class NormalDistributionAxisPainter : public AbstractAxisPainter {
private:
  static const String s_startDecadeFormat;

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
  void init() override;
  void paintXData() override;
  void paintYData() override;

public:
  NormalDistributionAxisPainter(SystemPainter &systemPainter, AxisIndex axisIndex);
  double getAxisPoint() const override;
  String getValueText(double v) const override;
  const TCHAR *getDoubleFormat() const override;
};
