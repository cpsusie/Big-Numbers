#include "pch.h"
#include <Math/MathFunctions.h>
#include <MFCUtil/Coordinatesystem/CoordinateSystem.h>
#include <MFCUtil/Coordinatesystem/SystemPainter.h>
#include <MFCUtil/Coordinatesystem/LinearAxisPainter.h>
#include <MFCUtil/Coordinatesystem/LogarithmicAxisPainter.h>
#include <MFCUtil/Coordinatesystem/NormalDistributionAxisPainter.h>
#include <MFCUtil/Coordinatesystem/DateAxisPainter.h>

SystemPainter::SystemPainter(CCoordinateSystem *system) : m_system(*system) {
  m_origin          = CPoint(0,0);
  m_oldFont         = NULL;
  getViewport().setScale(getScale(m_system.m_xAxisType), X_AXIS);
  getViewport().setScale(getScale(m_system.m_yAxisType), Y_AXIS);
  if(m_font) {
    m_oldFont = getViewport().getDC()->SelectObject(m_font);
  }
  m_xAxisPainter    = createAxisPainter(true , m_system.m_xAxisType);
  m_yAxisPainter    = createAxisPainter(false, m_system.m_yAxisType);
  makeSpaceForText();
}

SystemPainter::~SystemPainter() {
  SAFEDELETE(m_xAxisPainter);
  SAFEDELETE(m_yAxisPainter);
  if(m_oldFont) {
    CDC *dc = getViewport().getDC();
    dc->SelectObject(m_oldFont);
  }
}

void SystemPainter::paint() {
  CDC *dc = getViewport().getDC();
  CRect r = Rectangle2DR::makePositiveRectangle(getToRectangle());
  dc->FillSolidRect(r, m_system.m_backgroundColor);
//  dc->Rectangle(&r);

  m_xAxisPainter->paintAxisData();
  m_yAxisPainter->paintAxisData();
  m_xAxisPainter->paintAxis();
  m_yAxisPainter->paintAxis();
}

Viewport2D &SystemPainter::getViewport() {
  return m_system.m_vp;
}

const Viewport2D &SystemPainter::getViewport() const {
  return m_system.m_vp;
}

COLORREF SystemPainter::getAxisColor() const {
  return m_system.m_axisColor;
}

bool SystemPainter::hasGrid() const {
  return m_system.m_grid;
}

const CPoint &SystemPainter::getOrigin() const {
  return m_origin;
}

CSize SystemPainter::getTextExtent(const String &s) {
  return getViewport().getDC()->GetTextExtent(s.cstr());
}

void SystemPainter::makeSpaceForText() {
  int leftMargin   = m_yAxisPainter->getMaxTextOffset() + 2;
  int rightMargin  = AbstractAxisPainter::ARROW_SIZE / 2;
  int topMargin    = AbstractAxisPainter::ARROW_SIZE / 2;
  int bottomMargin = m_xAxisPainter->getMaxTextOffset() + getTextExtent(m_yAxisPainter->getMaxText()).cy + 1;

  Rectangle2D fr = getViewport().getFromRectangle();
  const IntervalTransformation &xtr = getViewport().getXTransformation();
  const IntervalTransformation &ytr = getViewport().getYTransformation();
  bool adjustRectangle = false;
  if(xtr.isLinear() && fr.getMinX() == 0) {
    double dx = -xtr.backwardTransform(getToRectangle().left + leftMargin);
    fr.m_x += dx;
    fr.m_w -= dx;
    adjustRectangle = true;
  }
  if(ytr.isLinear() && fr.getMinY() == 0) {
    double dy = -ytr.backwardTransform(getToRectangle().top - bottomMargin);
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

  Point2D orig = innerRectangle.getProjection(Point2D(m_xAxisPainter->getAxisPoint(),m_yAxisPainter->getAxisPoint()));

  double dx = min(orig.x - innerRectangle.m_x, leftMargin   - getTextExtent(m_xAxisPainter->getMinText()).cx/2 - 1);
  double dy = min(innerRectangle.m_y - orig.y, bottomMargin - getTextExtent(m_yAxisPainter->getMinText()).cy/2 - 1);

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

AbstractAxisPainter *SystemPainter::createAxisPainter(bool xAxis, AxisType type) {
  AbstractAxisPainter *result;
  switch(type) {
  case AXIS_LINEAR             : result = new LinearAxisPainter(            *this, xAxis); break;
  case AXIS_LOGARITHMIC        : result = new LogarithmicAxisPainter(       *this, xAxis); break;
  case AXIS_NORMAL_DISTRIBUTION: result = new NormalDistributionAxisPainter(*this, xAxis); break;
  case AXIS_DATE               : result = new DateAxisPainter(              *this, xAxis); break;
  default                      : throwException(_T("Invalid AxisType (=%d)"),type);
                                 return NULL;
  }
  TRACE_NEW(result);
}

void SystemPainter::setOccupiedRect(const CRect &r) {
  m_system.m_occupationMap.setOccupiedRect(r);
}

void SystemPainter::setOccupiedLine(const CPoint &from, const CPoint &to) { // line
  m_system.m_occupationMap.setOccupiedLine(from, to);
}
