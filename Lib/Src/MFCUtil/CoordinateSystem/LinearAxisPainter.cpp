#include "pch.h"
#include <MFCUtil/Coordinatesystem/LinearAxisPainter.h>

LinearAxisPainter::LinearAxisPainter(SystemPainter &systemPainter, AxisIndex axisIndex, AxisType type, bool initialize)
: AbstractAxisPainter(systemPainter,axisIndex, type)
{
  if(initialize) {
    doInvisiblePaint();
  }
}

double LinearAxisPainter::getAxisPoint() const {
  return transform(0);
}

void LinearAxisPainter::paintXData() {
  const DoubleInterval dataRange = getDataRange();
  const double         end       = next(dataRange.getMax());

  for(double t = getMin(); t <= end; t = next(t)) {
    if(!dataRange.contains(t)) {
      continue;
    }
    if(fabs(t) <= 1e-14 && getVisibleDataRangeLength() > 1e-5) {
      t = 0;
    }
    const double xt  = transform(t);
    const String tmp = getValueText(t);
    if(xTextPossible(xt, tmp)) {
      xTextOut(xt, tmp, 0);
      paintVerticalPin(xt, true);
    } else {
      paintVerticalPin(xt, false);
    }
  }
}

void LinearAxisPainter::paintYData() {
  const DoubleInterval dataRange = getDataRange();
  const double         end       = dataRange.getMax() + getStep();

  for(double t = getMin(); t <= end; t = next(t)) {
    if(!dataRange.contains(t)) {
      continue;
    }
    if(fabs(t) <= 1e-14 && getVisibleDataRangeLength() > 1e-5) {
      t = 0;
    }
    const double yt  = transform(t);
    const String tmp = getValueText(t);
    if(yTextPossible(yt, tmp)) {
      yTextOut(yt, tmp, 0);
      paintHorizontalPin(yt, true);
    } else {
      paintHorizontalPin(yt, false);
    }
  }
}
