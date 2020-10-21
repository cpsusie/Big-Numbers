#include "pch.h"
#include <MFCUtil/Coordinatesystem/SystemPainter.h>
#include <MFCUtil/Coordinatesystem/NormalDistributionAxisPainter.h>

const String NormalDistributionAxisPainter::s_startDecadeFormat = _T("%0.8lg");

NormalDistributionAxisPainter::NormalDistributionAxisPainter(SystemPainter &systemPainter, AxisIndex axisIndex)
: AbstractAxisPainter(systemPainter,axisIndex, AXIS_NORMAL_DISTRIBUTION)
{
  doInvisiblePaint();
}

void NormalDistributionAxisPainter::init() {
  if(getDecadeCount() <= 2) {
    __super::init();
  } else {
    const DoubleInterval dataRange = getDataRange();
    const double         step = pow(10, -abs(getDecade(dataRange.getMin())));
    setMinMaxStep(step, dataRange.getMax(), step);
  }
}

double NormalDistributionAxisPainter::getAxisPoint() const {
  return getTransformation().getToInterval().getFrom();
}

String NormalDistributionAxisPainter::getValueText(double v) const {
  if(v == 0) {
    return _T("0");
  }
  const int decade = getDecade(v);
  return fabs(decade) <= 2
       ? format(getDoubleFormat(),v)
       : format(s_startDecadeFormat.cstr(),v);
}

const TCHAR *NormalDistributionAxisPainter::getDoubleFormat() const {
  return useSuperAxisScale() ? __super::getDoubleFormat() : _T("%.8lg");
}

void NormalDistributionAxisPainter::paintXData() {
  if(useSuperAxisScale()) {
    paintXDataSingleDecade();
  } else {
    paintXDataMultipleDecades();
  }
}

void NormalDistributionAxisPainter::paintYData() {
  if(useSuperAxisScale()) {
    paintYDataSingleDecade();
  } else {
    paintYDataMultipleDecades();
  }
}

void NormalDistributionAxisPainter::paintXDataMultipleDecades() {
  const DoubleInterval dataRange = getDataRange();
  const double         max       = dataRange.getMax();
  double               t         = 0;
  for(int decade = getDecade(dataRange.getMin()); t <= max; decade++) {
    const double decadeStop = getDecadeStop(decade);
    const double step       = getDecadeStep(decade);
    for(t = getDecadeStart(decade); t <= decadeStop; t += step) {
      if(!dataRange.contains(t)) {
        continue;
      }
      const double xt = transform(t);
      if(isPainting() && xt <= getOrigin().x) {
        continue;
      }
      const String tmp = getValueText(t);
      if(xTextPossible(xt, tmp)) {
        xTextOut(xt, tmp, 0);
      }
      paintVerticalPin(xt, true);
    }
    t = decadeStop;
  }
}

void NormalDistributionAxisPainter::paintYDataMultipleDecades() {
  const DoubleInterval dataRange = getDataRange();
  const double         max       = dataRange.getMax();
  double               t         = 0;
  for(int decade = getDecade(dataRange.getMin()); t <= max; decade++) {
    const double decadeStop = getDecadeStop(decade);
    const double step       = getDecadeStep(decade);
    for(t = getDecadeStart(decade); t <= decadeStop; t += step) {
      if(!dataRange.contains(t)) {
        continue;
      }
      const double yt = transform(t);
      if(isPainting() && yt >= getOrigin().y) {
        continue;
      }
      const String tmp = getValueText(t);
      if(yTextPossible(yt, tmp)) {
        yTextOut(yt, tmp, 0);
      }
      paintHorizontalPin(yt, true);
    }
    t = decadeStop;
  }
}

bool NormalDistributionAxisPainter::useSuperAxisScale() const {
  return getDecadeCount() <= 2;
}

int NormalDistributionAxisPainter::getDecadeCount() const {
  return getDecade(getDataRange().getMax()) - getDecade(getDataRange().getMin());
}

// Values between 0.1 and 0.9 have decade 0.
// Values below 0.1 has negative decade. Values above 0.9 has positive decade
int NormalDistributionAxisPainter::getDecade(double x) { // static
  if((x >= 0.1-1e-6) && (x <= 0.9+1e-6)) {
    return 0;
  } else if(x < 0.1) {
    return (int)floor(log10(x)) + 1;
  } else {
    return -1-(int)floor(log10(1-x));
  }
}

double NormalDistributionAxisPainter::getDecadeStep(int decade) { // static
  return (decade == 0) ? 0.1 : pow(10,-fabs(decade-1))*10;
}

double NormalDistributionAxisPainter::getDecadeStart(int decade) { // static
  if(decade < 0) {
    return pow(10, decade-1);
  } else if(decade > 0) {
    return 1 - pow(10, -(decade+1));
  } else {
    return 0.1;
  }
}

double NormalDistributionAxisPainter::getDecadeStop(int decade) { // static
  const double decadeStart = getDecadeStart(decade);
  return (decade <= 0) ? 9*decadeStart : (decadeStart + ((1-decadeStart)/10*9));
}
