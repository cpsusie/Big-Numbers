#pragma once

#include "AbstractAxisPainter.h"

class LinearAxisPainter : public AbstractAxisPainter {
protected:
  void paintXData() override;
  void paintYData() override;

public:
  LinearAxisPainter(SystemPainter &systemPainter, AxisIndex axisIndex, AxisType type = AXIS_LINEAR, bool initialize = true);
  double getAxisPoint() const override;
};

