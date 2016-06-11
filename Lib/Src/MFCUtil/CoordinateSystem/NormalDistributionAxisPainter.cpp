#include "stdafx.h"
#include <MFCUtil/Coordinatesystem/SystemPainter.h>
#include <MFCUtil/Coordinatesystem/NormalDistributionAxisPainter.h>

const String NormalDistributionAxisPainter::startDecadeFormat = "%0.8lg";

NormalDistributionAxisPainter::NormalDistributionAxisPainter(SystemPainter &systemPainter, bool xAxis) : AbstractAxisPainter(systemPainter, xAxis) {
  doInvisiblePaint();
}

void NormalDistributionAxisPainter::init() {
  if(getDecadeCount() <= 2) {
    AbstractAxisPainter::init();
  } else {
    DoubleInterval dataRange = getDataRange();
    double step = pow(10, -abs(getDecade(dataRange.getMin())));
    setMinMaxStep(step, dataRange.getMax(), step);
  }
}

double NormalDistributionAxisPainter::getAxisPoint() const {
  return getTransformation().getToInterval().getFrom();
}

String NormalDistributionAxisPainter::getText(double x) {
  if(x == 0)
    return "0";
  int decade = getDecade(x);
  if(fabs(decade) <= 2)
    return format(getDoubleFormat(),x);
  else
    return format(startDecadeFormat.cstr(),x);
}

const TCHAR *NormalDistributionAxisPainter::getDoubleFormat() {
  if(m_doubleFormat.length() == 0) {
    if(useSuperAxisScale()) {
      m_doubleFormat = AbstractAxisPainter::getDoubleFormat();
    } else {
      m_doubleFormat = _T("%.8lg");
    }
  }
  return m_doubleFormat.cstr();
}

void NormalDistributionAxisPainter::paintXData() {
  if(useSuperAxisScale()) {
    paintXDataSingleDecade();
  }
  else {
    paintXDataMultipleDecades();
  }
}

void NormalDistributionAxisPainter::paintYData() {
  if(useSuperAxisScale()) {
    paintYDataSingleDecade();
  }
  else {
    paintYDataMultipleDecades();
  }
}

void NormalDistributionAxisPainter::paintXDataMultipleDecades() {
  DoubleInterval dataRange = getDataRange();
  double max = dataRange.getMax();
  double t = 0;
  for(int decade = getDecade(dataRange.getMin()); t <= max; decade++) {
    double decadeStop = getDecadeStop(decade);
    double step = getDecadeStep(decade);
    for(t = getDecadeStart(decade); t <= decadeStop; t += step) {
      if(!dataRange.contains(t))
        continue;
      double xt = transform(t);
      if(isPainting() && xt <= getOrigin().x)
        continue;
      String tmp = getText(t);
      if(xTextPossible(xt, tmp)) {
        xTextOut(xt, tmp, 0);
      }
      paintVerticalPin(xt, true);
    }
    t = decadeStop;
  }
}

void NormalDistributionAxisPainter::paintYDataMultipleDecades() {
  DoubleInterval dataRange = getDataRange();
  double max = dataRange.getMax();
  double t = 0;
  for(int decade = getDecade(dataRange.getMin()); t <= max; decade++) {
    double decadeStop = getDecadeStop(decade);
    double step = getDecadeStep(decade);
    for(t = getDecadeStart(decade); t <= decadeStop; t += step) {
      if(!dataRange.contains(t))
        continue;
      double yt = transform(t);
      if(isPainting() && yt >= getOrigin().y)
        continue;
      String tmp = getText(t);
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
  if(x >= 0.1-1e-6 && x <= 0.9+1e-6)
    return 0;
  else if(x < 0.1)
    return (int)floor(log10(x)) + 1;
  else
    return -1-(int)floor(log10(1-x));
}

double NormalDistributionAxisPainter::getDecadeStep(int decade) { // static 
  if(decade == 0)
    return 0.1;
  else
    return pow(10,-fabs(decade-1))*10;
}

double NormalDistributionAxisPainter::getDecadeStart(int decade) { // static 
  if(decade < 0)
    return pow(10,decade-1);
  else if(decade > 0)
    return 1 - pow(10,-(decade+1));
  else
    return 0.1;
}

double NormalDistributionAxisPainter::getDecadeStop(int decade) { // static 
  double decadeStart = getDecadeStart(decade);
  if(decade <= 0)
    return 9*decadeStart;
  else
    return decadeStart + ((1-decadeStart)/10*9);
}
