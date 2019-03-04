#include "pch.h"
#include <MFCUtil/Coordinatesystem/SystemPainter.h>
#include <MFCUtil/Coordinatesystem/LogarithmicAxisPainter.h>

const String LogarithmicAxisPainter::startDecadeFormat = _T("%.0lg");

LogarithmicAxisPainter::LogarithmicAxisPainter(SystemPainter &systemPainter, bool xAxis) : AbstractAxisPainter(systemPainter,xAxis) {
  doInvisiblePaint();
}

void LogarithmicAxisPainter::init() {
  if(isSingleDecade()) {
    __super::init();
  } else {
    const double start = pow(10, floor(log10(getDataRange().getMin())));
    setMinMaxStep(start, getDataRange().getMax(), start);
  }
}

double LogarithmicAxisPainter::getAxisPoint() const {
  return getTransformation().getToInterval().getFrom();
}

const TCHAR *LogarithmicAxisPainter::getDoubleFormat() {
  if(m_doubleFormat.length() == 0) {
    if(isSingleDecade()) {
      m_doubleFormat = __super::getDoubleFormat();
    } else {
      m_doubleFormat = _T("%lg");
    }
  }
  return m_doubleFormat.cstr();
}

String LogarithmicAxisPainter::getStartDecadeText(double d) {
  if(fabs(log10(d)) < 5) {
    return format(getDoubleFormat(),d);
  } else {
    return format(startDecadeFormat.cstr(),d);
  }
}

void LogarithmicAxisPainter::paintXData() {
  if(isSingleDecade()) {
    paintXDataSingleDecade();
  } else {
    paintXDataMultipleDecades();
  }
}

void LogarithmicAxisPainter::paintYData() {
  if(isSingleDecade()) {
    paintYDataSingleDecade();
  } else {
    paintYDataMultipleDecades();
  }
}

void LogarithmicAxisPainter::paintXDataMultipleDecades() {
  const CSize          cw          = getTextExtent(_T("8"));
  const DoubleInterval dataRange   = getDataRange();
  const double         max         = dataRange.getMax();
  const double         decadeSize  = getDecadeSize();
  double               decadeStep  = getStep();
  double               lastLongPin = -1;

  for(double t = getMin(); t <= max; decadeStep *= decadeSize) {
    for(int i = 1; i < 10 && t <= max; i++, t += decadeStep) {
      if(!dataRange.contains(t)) {
        continue;
      }
      const double xt = transform(t);
      if(isPainting() && xt <= getOrigin().x) {
        continue;
      }
      String tmp;
      int z = 0;
      if(i == 1) {
        z = cw.cy;
        tmp = getStartDecadeText(t);
      } else {
        tmp = getValueText(i);
      }
      if(i == 1) {
        xTextOut(xt, tmp, z);
        paintVerticalPin(xt, true);
        lastLongPin = xt;
      } else if(xTextPossible(xt, tmp)) {
        xTextOut(xt, tmp, z);
        if(lastLongPin < 0 || (fabs(xt - lastLongPin) > 12)) {
          paintVerticalPin(xt, true);
          lastLongPin = xt;
        } else {
          paintVerticalPin(xt, false);
        }
      }
    }
  }
}

void LogarithmicAxisPainter::paintYDataMultipleDecades() {
  const CSize          cw          = getTextExtent(_T("8"));
  const DoubleInterval dataRange   = getDataRange();
  const double         max         = dataRange.getMax();
  const double         decadeSize  = getDecadeSize();
  double               decadeStep  = getStep();
  double               lastLongPin = -1;

  for(double t = getMin(); t <= max; decadeStep *= decadeSize) {
    for(int i = 1; i < 10 && t <= max; i++, t += decadeStep) {
      if(!dataRange.contains(t)) {
        continue;
      }
      const double yt = transform(t);
      if(isPainting() && yt >= getOrigin().y) {
        continue;
      }
      String tmp;
      int z = 0;
      if(i == 1) {
        z = -cw.cx;
        tmp = getStartDecadeText(t);
      } else {
        tmp = getValueText(i);
      }
      if(i == 1) {
        yTextOut(yt, tmp, z);
        paintHorizontalPin(yt, true);
        lastLongPin = yt;
      } else if(yTextPossible(yt, tmp)) {
        yTextOut(yt, tmp, z);
        if(lastLongPin < 0 || (fabs(yt - lastLongPin) > 12)) {
          paintHorizontalPin(yt, true);
          lastLongPin = yt;
        } else {
          paintHorizontalPin(yt, false);
        }
      }
    }
  }
}

double LogarithmicAxisPainter::getDecadeSize() const {
  return 10;
}

bool LogarithmicAxisPainter::isSingleDecade() const {
  return getDecadeCount() == 1;
}

int LogarithmicAxisPainter::getDecadeCount() const {
  return (int)log10(getDataRange().getMax() / getDataRange().getMin()) + 1;
}
