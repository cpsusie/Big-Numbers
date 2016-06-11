#pragma once

#include "AbstractAxisPainter.h"

class LinearAxisPainter : public AbstractAxisPainter {
protected:
  void paintXData();
  void paintYData();

public:
  LinearAxisPainter(SystemPainter &systemPainter, bool xAxis, bool initialize = true);
  AxisType getType() const { return AXIS_LINEAR; }
  double getAxisPoint() const;
};

