#include "pch.h"
#include <Math/Transformation.h>
#include <MFCUtil/Coordinatesystem/AbstractAxisPainter.h>
#include <MFCUtil/Coordinatesystem/SystemPainter.h>

const int AbstractAxisPainter::PIN_LENGTH = 3;
const int AbstractAxisPainter::ARROW_SIZE = 7;

AbstractAxisPainter::AbstractAxisPainter(SystemPainter &systemPainter, bool xAxis) : m_systemPainter(systemPainter) {
  m_vp              = &systemPainter.getViewport();
  m_xAxis           = xAxis;
  m_dataRange       = getTransformation().getFromInterval();
  if(m_dataRange.getLength() <= 0) {
    throwException(_T("AbstractAxisPainter::DataRange:[%le;%le] on %s-axis has negative length."),m_dataRange.getFrom(),m_dataRange.getTo(), m_xAxis?_T("X"):_T("Y"));
  }
  m_axisColor       = systemPainter.getAxisColor();
  m_grid            = systemPainter.hasGrid();
  m_solidPen.CreatePen(PS_SOLID,1,m_axisColor);
  m_gridPen.CreatePen( PS_DOT  ,1,m_axisColor);
}

AbstractAxisPainter::~AbstractAxisPainter() {
  m_solidPen.DeleteObject();
  m_gridPen.DeleteObject();
}

const IntervalTransformation &AbstractAxisPainter::getTransformation() const {
  return isXAxis() ? m_vp->getXTransformation() : m_vp->getYTransformation();
}

const CPoint &AbstractAxisPainter::getOrigin() const {
  return m_systemPainter.getOrigin();
}

void AbstractAxisPainter::doInvisiblePaint() {
  init();
  m_maxTextOffset = -1;
  m_isPainting = false;
  paintAxisData();
  m_isPainting = true;
}

bool AbstractAxisPainter::isXAxis() const {
  return m_xAxis;
}

void AbstractAxisPainter::paintAxisData() {
  m_lastTextPos      = -1;
  m_xRectInterval    = m_vp->getXTransformation().getToInterval();
  m_yRectInterval    = m_vp->getYTransformation().getToInterval();
  if(isXAxis()) {
    paintXData();
  } else {
    paintYData();
  }
}

void AbstractAxisPainter::paintAxis() {
  if(isXAxis()) {
    paintXAxis();
  } else {
    paintYAxis();
  }
}

void AbstractAxisPainter::init() {
  double minimum = m_dataRange.getMin();
  double maximum = m_dataRange.getMax();

  if(minimum == maximum) {
    if(minimum == 0) { // both 0
      maximum = 10;
    } else if(minimum < 0) {
      minimum *= 1.1;
      maximum /= 1.1;
    }  else {
      minimum /= 1.1;
      maximum *= 1.1;
    }
  }
  const double step = findNiceDecimalStep(maximum - minimum);
  setMinMaxStep(step * ceil(minimum / step), step * floor(maximum / step), step);
}

double AbstractAxisPainter::findNiceDecimalStep(double range) { // static
  int n;
  double step = range / 12;
  if(step < 1) {
    for(n = 0; step < 1; n--) {
      step *= 10;
    }
  } else {
    for(n = 0; step >= 10; n++) {
      step /= 10;
    }
  }

  assert(step >= 1 && step < 10);

  if(step < 2) {
    step = 1;
  } else if(step < 5) {
    step = 2;
  } else {
    step = 5;
  }

  for(;n < 0; n++) {
    step /= 10;
  }
  for(;n > 0; n--) {
    step *= 10;
  }
  return step;
}

String AbstractAxisPainter::getText(double d) {
  if(d == 0) {
    return _T("0");
  }
  return format(getDoubleFormat(),d);
}

void AbstractAxisPainter::setMinMaxStep(double min, double max, double step) {
  m_min  = min;
  m_max  = max;
  m_step = step;
}

double AbstractAxisPainter::next(double x) const {
  return x + m_step;
}

const TCHAR *AbstractAxisPainter::getDoubleFormat() {
  if(m_doubleFormat.length() == 0) {
    double magnitude         = max(fabs(m_max),fabs(m_min));
    int    e                 = (int)floor(log10(magnitude));
    int    significantDigits = (int)ceil(log10(magnitude/(m_max-m_min))) + 1;
    int    decimals          = max(significantDigits - e + 1,0);
    if(fabs(e) < 2 && significantDigits < 3) {
      m_doubleFormat = _T("%lg");
    } else {
      if(e < -4 || e > 5) {
        m_doubleFormat = format(_T("%%.%dle"),significantDigits);
      }
      else {
        m_doubleFormat = format(_T("%%.%dlf"),decimals);
      }
    }
  }
  return m_doubleFormat.cstr();
}

void AbstractAxisPainter::paintXAxis() {
  if(!isPainting()) {
    return;
  }
  double left  = m_xRectInterval.getMin();
  double right = m_xRectInterval.getMax();
  int origY = getOrigin().y;
  line(left , origY, right, origY, true);
  line(right, origY, right - ARROW_SIZE, origY - ARROW_SIZE / 2, false);
  line(right, origY, right - ARROW_SIZE, origY + ARROW_SIZE / 2, false);
}

void AbstractAxisPainter::paintYAxis() {
  if(!isPainting()) {
    return;
  }
  double top    = m_yRectInterval.getMin();
  double bottom = m_yRectInterval.getMax();
  int origX = getOrigin().x;
  line(origX, top, origX, bottom, true);
  line(origX, top, origX - ARROW_SIZE / 2, top + ARROW_SIZE, false);
  line(origX, top, origX + ARROW_SIZE / 2, top + ARROW_SIZE, false);
}

void AbstractAxisPainter::paintXDataSingleDecade() {
  for(double t = m_min; t <= m_dataRange.getMax(); t = next(t)) {
    if(!m_dataRange.contains(t)) {
      continue;
    }
    int xt = (int)transform(t);
    if(isPainting() && xt <= getOrigin().x) {
      continue;
    }
    String tmp = getText(t);
    if(xTextPossible(xt, tmp)) {
      xTextOut(xt, tmp, 0);
    }
    paintVerticalPin(xt, true);
  }
}

void AbstractAxisPainter::paintYDataSingleDecade() {
  for(double t = m_min; t <= m_dataRange.getMax(); t = next(t)) {
    if(!m_dataRange.contains(t)) {
      continue;
    }
    int yt = (int)transform(t);
    if(isPainting() && yt >= getOrigin().y) {
      continue;
    }
    String tmp = getText(t);
    if(yTextPossible(yt, tmp)) {
      yTextOut(yt, tmp, 0);
    }
    paintHorizontalPin(yt, true);
  }
}

double AbstractAxisPainter::transform(double x) const {
  return getTransformation().forwardTransform(x);
}

void AbstractAxisPainter::xTextOut(double x, const String &s, int yOffset) {
  const CPoint &origin = getOrigin();
  const CSize   dim    = getTextExtent(s);
  const int     xp     = (int)x + ((((int)round(x) == origin.x || m_grid) && m_yRectInterval.getMax() > origin.y) ? 1 : -dim.cx/2);
  yOffset += PIN_LENGTH + 3;
  const int     yp     = origin.y + yOffset;
  textOut(xp, yp, s, dim);
  if(!isPainting()) {
    const int m = (int)fabs(yOffset);
    if(m > m_maxTextOffset) {
      m_maxTextOffset = m;
    }
  }
  m_lastTextPos = (int)x + dim.cx/2;
}

void AbstractAxisPainter::yTextOut(double y, const String &s, int xOffset) {
  const CPoint &origin = getOrigin();
  const CSize   dim    = getTextExtent(s);
  xOffset -= dim.cx + PIN_LENGTH + 3;
  const int     xp     = origin.x + xOffset;
  const int     yp     = (int)(y - ((((int)round(y) == origin.y || m_grid) && m_xRectInterval.getMin() < origin.x) ? dim.cy : dim.cy/2));
  textOut(xp, yp, s, dim);
  if(!isPainting()) {
    const int m = (int)fabs(xOffset);
    if(m > m_maxTextOffset) {
      m_maxTextOffset = m;
    }
  }
  m_lastTextPos = (int)y + dim.cy/2;
}

CSize AbstractAxisPainter::getTextExtent(const String &s) {
  return m_systemPainter.getTextExtent(s);
}

void AbstractAxisPainter::textOut(int x, int y, const String &txt, const CSize &textExtent) {
  if(!isPainting()) {
    return;
  } else {
    m_systemPainter.setOccupiedRect(CRect(CPoint(x,y), textExtent));
  }
  CDC *dc = m_vp->getDC();
  dc->SetTextColor(m_axisColor);
  dc->TextOut(x, y, txt.cstr());
}

bool AbstractAxisPainter::xTextPossible(double x, const String &s) {
  if(m_lastTextPos == -1) {
    return true;
  } else {
    const CSize dim = getTextExtent(s);
    return fabs(x - m_lastTextPos) > (dim.cx+3);
  }
}

bool AbstractAxisPainter::yTextPossible(double y, const String &s) {
  if(m_lastTextPos == -1) {
    return true;
  } else {
    const CSize dim = getTextExtent(s);
    return fabs(y - m_lastTextPos) > (dim.cy+3);
  }
}

void AbstractAxisPainter::paintHorizontalPin(double y, bool big) {
  if(!isPainting()) {
    return;
  }
  const CPoint &origin = getOrigin();
  if(big) {
    if(m_grid) {
      horizontalGridLine(y);
    } else {
      line(origin.x,y, origin.x - PIN_LENGTH*2,y, false);
    }
  } else {
    line(origin.x,y, origin.x - PIN_LENGTH,y, false);
  }
}

void AbstractAxisPainter::paintVerticalPin(double x, bool big) {
  if(!isPainting()) {
    return;
  }
  const CPoint &origin = getOrigin();
  if(big) {
    if(m_grid) {
      verticalGridLine(x);
    } else {
      line(x,origin.y, x, origin.y + PIN_LENGTH*2, false);
    }
  } else {
    line(x, origin.y, x, origin.y + PIN_LENGTH, false);
  }
}

void AbstractAxisPainter::verticalGridLine(double x) {
  line(x, m_yRectInterval.getMin(), x, m_yRectInterval.getMax(), m_gridPen, false);
}

void AbstractAxisPainter::horizontalGridLine(double y) {
  line(m_xRectInterval.getMin(), y, m_xRectInterval.getMax(), y, m_gridPen, false);
}

void AbstractAxisPainter::line(double x1, double y1, double x2, double y2, bool setOccupied) {
  if(!isPainting()) {
    return;
  }
  line(x1, y1, x2, y2, m_solidPen, setOccupied);
}

void AbstractAxisPainter::line(double x1, double y1, double x2, double y2, CPen &pen, bool setOccupied) {
  if(!isPainting()) {
    return;
  }
  CDC *dc = m_vp->getDC();
  dc->SelectObject(&pen);
  CPoint p1((int)round(x1), (int)round(y1)), p2((int)round(x2), (int)round(y2));
  dc->MoveTo(p1);
  dc->LineTo(p2);
  if (setOccupied) {
    m_systemPainter.setOccupiedLine(p1,p2);
  }
}
