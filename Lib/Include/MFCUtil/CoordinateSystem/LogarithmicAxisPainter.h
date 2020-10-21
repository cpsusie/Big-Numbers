#pragma once

#include "AbstractAxisPainter.h"

class LogarithmicAxisPainter : public AbstractAxisPainter {
private:
  mutable String m_doubleFormat;
  static const String s_startDecadeFormat;

  String getStartDecadeText(double d);
  void paintXDataMultipleDecades();
  void paintYDataMultipleDecades();
  bool isSingleDecade() const;
  int  getDecadeCount() const;
  double getDecadeSize() const;
protected:
  void init() override;
  void paintXData() override;
  void paintYData() override;

public:
  LogarithmicAxisPainter(SystemPainter &systemPainter, AxisIndex axisIndex);
  double getAxisPoint() const override;
  const TCHAR *getDoubleFormat() const override;
};
