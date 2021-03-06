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
, m_font(          nullptr  )
, m_oldFont(       nullptr  )
, m_origin(0,0)
, m_mouseMode(false)
{
  m_axisPainter[0] = m_axisPainter[1] = nullptr;
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
  const CRect r = (CRect)Rectangle2D::makePositiveRectangle(getToRectangle());
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

static CPoint getProjection(const CPoint &p, const CRect &r) {
  return CPoint(minMax(p.x, r.left, r.right), minMax(p.y, r.top, r.bottom));
}

CSize SystemPainter::getTextExtent(const String &s) {
  return getViewport().getDC()->GetTextExtent(s.cstr());
}
void SystemPainter::makeSpaceForText() {
  const AbstractAxisPainter    &xPainter     = *m_axisPainter[XAXIS_INDEX], &yPainter = *m_axisPainter[YAXIS_INDEX];
  const int                     leftMargin   = yPainter.getMaxTextOffset() + 2;
  const int                     rightMargin  = AbstractAxisPainter::ARROW_SIZE / 2;
  const int                     topMargin    = AbstractAxisPainter::ARROW_SIZE / 2;
  const int                     bottomMargin = xPainter.getMaxTextOffset() + getTextExtent(yPainter.getMaxValueText()).cy + 1;
  const CRect                   toRect       = getToRectangle();
  Rectangle2D                   fromRect     = getViewport().getFromRectangle();
  const IntervalTransformation &xtr = getViewport().getXTransformation();
  const IntervalTransformation &ytr = getViewport().getYTransformation();
  bool                          adjustFromRect = false;
  if(xtr.isLinear() && (fromRect.getMinX() == 0)) {
    const double dx = -xtr.backwardTransform(toRect.left + leftMargin);
    fromRect.p0()[0]   += dx;
    fromRect.size()[0] -= dx;
    adjustFromRect = true;
  }
  if(ytr.isLinear() && (fromRect.getMinY() == 0)) {
    const double dy = -ytr.backwardTransform(toRect.bottom + bottomMargin);
    fromRect.p0()[1]   += dy;
    fromRect.size()[1] -= dy;
    adjustFromRect = true;
  }

  if(adjustFromRect) {
    getViewport().setFromRectangle(fromRect);
  }

  CRect innerRect  = makePositiveRect(getToRectangle());
  innerRect.left   += leftMargin;
  innerRect.right  -= rightMargin;
  innerRect.top    += topMargin;
  innerRect.bottom -= bottomMargin;

  const CPoint orig = getProjection((CPoint)Point2D(xPainter.getAxisPoint(), yPainter.getAxisPoint()), innerRect);

  const int dx = min(orig.x - innerRect.left  , leftMargin   - getTextExtent(xPainter.getMinValueText()).cx/2 - 1);
  const int dy = min(innerRect.bottom - orig.y, bottomMargin - getTextExtent(yPainter.getMinValueText()).cy/2 - 1);

  if(dx > 0) {
    innerRect.left   -= dx;
  }
  if(dy > 0) {
    innerRect.bottom += dy;
  }

  m_origin = getProjection(orig, innerRect);
}

CRect SystemPainter::getToRectangle() const {
  return (CRect)getViewport().getToRectangle();
}

AbstractAxisPainter *SystemPainter::createAxisPainter(AxisIndex axis, AxisType type) {
  AbstractAxisPainter *result = nullptr;
  switch(type) {
  case AXIS_LINEAR             : result = new LinearAxisPainter(            *this, axis); TRACE_NEW(result); break;
  case AXIS_LOGARITHMIC        : result = new LogarithmicAxisPainter(       *this, axis); TRACE_NEW(result); break;
  case AXIS_NORMAL_DISTRIBUTION: result = new NormalDistributionAxisPainter(*this, axis); TRACE_NEW(result); break;
  case AXIS_DATE               : result = new DateAxisPainter(              *this, axis); TRACE_NEW(result); break;
  default                      : throwInvalidArgumentException(__TFUNCTION__, _T("type (=%d)"),type);
  }
  return result;
}

void SystemPainter::setOccupiedRect(const CRect &r) {
  m_system.m_occupationMap.setOccupiedRect(r);
}

void SystemPainter::setOccupiedLine(const CPoint &from, const CPoint &to) { // line
  m_system.m_occupationMap.setOccupiedLine(from, to);
}
