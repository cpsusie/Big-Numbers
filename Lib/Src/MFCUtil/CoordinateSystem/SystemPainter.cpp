#include "pch.h"
#include <Math/MathLib.h>
#include <MFCUtil/Coordinatesystem/CoordinateSystem.h>
#include <MFCUtil/Coordinatesystem/SystemPainter.h>
#include <MFCUtil/Coordinatesystem/LinearAxisPainter.h>
#include <MFCUtil/Coordinatesystem/LogarithmicAxisPainter.h>
#include <MFCUtil/Coordinatesystem/NormalDistributionAxisPainter.h>
#include <MFCUtil/Coordinatesystem/DateAxisPainter.h>

#define FOR_BOTH_AXIS(i) for(AxisIndex i = XAXIS_INDEX; i <= YAXIS_INDEX; ((int&)i)++)

SystemPainter::SystemPainter(CCoordinateSystem *system)
: m_system(       *system)
, m_font(          NULL  )
, m_oldFont(       NULL  )
, m_origin(0,0)
, m_mouseMode(false)
{
  m_axisPainter[0] = m_axisPainter[1] = NULL;
  getViewport().setScale(getScale(getAxisAttr(XAXIS_INDEX).getType()), X_AXIS);
  getViewport().setScale(getScale(getAxisAttr(YAXIS_INDEX).getType()), Y_AXIS);
  if(m_font) {
    m_oldFont = getViewport().getDC()->SelectObject(m_font);
  }

  FOR_BOTH_AXIS(i) {
    m_axisPainter[i] = createAxisPainter(i, getAxisAttr(i).getType());
  }
  makeSpaceForText();
}

SystemPainter::~SystemPainter() {
  FOR_BOTH_AXIS(i) {
    SAFEDELETE(m_axisPainter[i]);
  }
  if(m_oldFont) {
    CDC *dc = getViewport().getDC();
    dc->SelectObject(m_oldFont);
  }
}

void SystemPainter::paint() {
  CDC *dc = getViewport().getDC();
  const CRect r = Rectangle2DR::makePositiveRectangle(getToRectangle());
  dc->FillSolidRect(r, m_system.m_backgroundColor);
//  dc->Rectangle(&r);

  setMouseMode(false);
  FOR_BOTH_AXIS(i) {
    m_axisPainter[i]->paintAxisData();
  }
  FOR_BOTH_AXIS(i) {
    m_axisPainter[i]->paintAxis();
  }
  setMouseMode(true);
}

Viewport2D &SystemPainter::getViewport() {
  return m_system.m_vp;
}
const Viewport2D &SystemPainter::getViewport() const {
  return m_system.m_vp;
}
const AxisAttribute &SystemPainter::getAxisAttr(AxisIndex axis) const {
  return m_system.getAxisAttr(axis);
}
const CPoint &SystemPainter::getOrigin() const {
  return m_origin;
}
CSize SystemPainter::getTextExtent(const String &s) {
  return getViewport().getDC()->GetTextExtent(s.cstr());
}
void SystemPainter::makeSpaceForText() {
  const AbstractAxisPainter &xPainter = *m_axisPainter[XAXIS_INDEX], &yPainter = *m_axisPainter[YAXIS_INDEX];
  const int leftMargin   = yPainter.getMaxTextOffset() + 2;
  const int rightMargin  = AbstractAxisPainter::ARROW_SIZE / 2;
  const int topMargin    = AbstractAxisPainter::ARROW_SIZE / 2;
  const int bottomMargin = xPainter.getMaxTextOffset() + getTextExtent(yPainter.getMaxValueText()).cy + 1;

  Rectangle2D fr = getViewport().getFromRectangle();
  const IntervalTransformation &xtr = getViewport().getXTransformation();
  const IntervalTransformation &ytr = getViewport().getYTransformation();
  bool adjustRectangle = false;
  if(xtr.isLinear() && fr.getMinX() == 0) {
    const double dx = -xtr.backwardTransform(getToRectangle().left + leftMargin);
    fr.m_x += dx;
    fr.m_w -= dx;
    adjustRectangle = true;
  }
  if(ytr.isLinear() && fr.getMinY() == 0) {
    const double dy = -ytr.backwardTransform(getToRectangle().top - bottomMargin);
    fr.m_y += dy;
    fr.m_h -= dy;
    adjustRectangle = true;
  }

  if(adjustRectangle) {
    getViewport().setFromRectangle(fr);
  }
  Rectangle2DR innerRectangle = getToRectangle();

  innerRectangle.m_x += leftMargin;
  innerRectangle.m_w -= leftMargin + rightMargin;
  innerRectangle.m_y -= bottomMargin;
  innerRectangle.m_h += bottomMargin + topMargin;

  Point2D orig = innerRectangle.getProjection(Point2D(xPainter.getAxisPoint(),yPainter.getAxisPoint()));

  const double dx = min(orig.x - innerRectangle.m_x, leftMargin   - getTextExtent(xPainter.getMinValueText()).cx/2 - 1);
  const double dy = min(innerRectangle.m_y - orig.y, bottomMargin - getTextExtent(yPainter.getMinValueText()).cy/2 - 1);

  if(dx > 0) {
    innerRectangle.m_x -= dx;
    innerRectangle.m_w += dx;
  }
  if(dy > 0) {
    innerRectangle.m_y += dy;
    innerRectangle.m_h -= dy;
  }
  m_origin = innerRectangle.getProjection(orig);
}

CRect SystemPainter::getToRectangle() const {
  return getViewport().getToRectangle();
}

AbstractAxisPainter *SystemPainter::createAxisPainter(AxisIndex axis, AxisType type) {
  AbstractAxisPainter *result;
  switch(type) {
  case AXIS_LINEAR             : result = new LinearAxisPainter(            *this, axis); break;
  case AXIS_LOGARITHMIC        : result = new LogarithmicAxisPainter(       *this, axis); break;
  case AXIS_NORMAL_DISTRIBUTION: result = new NormalDistributionAxisPainter(*this, axis); break;
  case AXIS_DATE               : result = new DateAxisPainter(              *this, axis); break;
  default                      : throwInvalidArgumentException(__TFUNCTION__,_T("type (=%d)"),type);
                                 return NULL;
  }
  TRACE_NEW(result);
  return result;
}

void SystemPainter::setOccupiedRect(const CRect &r) {
  m_system.m_occupationMap.setOccupiedRect(r);
}

void SystemPainter::setOccupiedLine(const CPoint &from, const CPoint &to) { // line
  m_system.m_occupationMap.setOccupiedLine(from, to);
}
