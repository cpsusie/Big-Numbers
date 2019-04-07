#pragma once

#include "AbstractAxisPainter.h"

class LogarithmicAxisPainter : public AbstractAxisPainter {
private:
  String m_doubleFormat;
  static const String startDecadeFormat;

  String getStartDecadeText(double d);
  void paintXDataMultipleDecades();
  void paintYDataMultipleDecades();
  bool isSingleDecade() const;
  int  getDecadeCount() const;
  double getDecadeSize() const;
protected:
  void init();
  void paintXData();
  void paintYData();

public:
  LogarithmicAxisPainter(SystemPainter &systemPainter, AxisIndex axis);
  AxisType getType() const { return AXIS_LOGARITHMIC; }
  double getAxisPoint() const;
  const TCHAR *getDoubleFormat();
};
