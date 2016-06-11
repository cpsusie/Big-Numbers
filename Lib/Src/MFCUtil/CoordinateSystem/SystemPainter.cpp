#include "stdafx.h"
#include <Math/MathFunctions.h>
#include <MFCUtil/Coordinatesystem/SystemPainter.h>
#include <MFCUtil/Coordinatesystem/LinearAxisPainter.h>
#include <MFCUtil/Coordinatesystem/LogarithmicAxisPainter.h>
#include <MFCUtil/Coordinatesystem/NormalDistributionAxisPainter.h>
#include <MFCUtil/Coordinatesystem/DateAxisPainter.h>

SystemPainter::SystemPainter(Viewport2D &vp, CFont *font, COLORREF backgroundColor, COLORREF axisColor, AxisType xAxisType, AxisType yAxisType, bool grid) : m_vp(vp), m_font(font) {
  m_backgroundColor = backgroundColor;
  m_axisColor       = axisColor;
  m_grid            = grid;
  m_origin          = CPoint(0,0);
  m_oldFont         = NULL;
  m_vp.setScale(getScale(xAxisType), X_AXIS);
  m_vp.setScale(getScale(yAxisType), Y_AXIS);
  if(m_font) {
    m_oldFont = m_vp.getDC()->SelectObject(m_font);
  }
  m_xAxisPainter    = createAxisPainter(true , xAxisType);
  m_yAxisPainter    = createAxisPainter(false, yAxisType);
  makeSpaceForText();
}

SystemPainter::~SystemPainter() {
  if(m_xAxisPainter != NULL) {
    delete m_xAxisPainter;
  }
  if(m_yAxisPainter != NULL) {
    delete m_yAxisPainter;
  }
  if(m_oldFont) {
    CDC *dc = m_vp.getDC();
    dc->SelectObject(m_oldFont);
  }
}

void SystemPainter::paint() {
  CDC *dc = m_vp.getDC();
  CRect r = Rectangle2DR::makePositiveRectangle(getToRectangle());
  dc->FillSolidRect(r, m_backgroundColor);
//  dc->Rectangle(&r);

  m_xAxisPainter->paintAxisData();
  m_yAxisPainter->paintAxisData();
  m_xAxisPainter->paintAxis();
  m_yAxisPainter->paintAxis();
}

Viewport2D &SystemPainter::getViewport() {
  return m_vp;
}

COLORREF SystemPainter::getAxisColor() const {
  return m_axisColor;
}

bool SystemPainter::hasGrid() const {
  return m_grid;
}

const CPoint &SystemPainter::getOrigin() const {
  return m_origin;
}

CSize SystemPainter::getTextExtent(const String &s) {
  return m_vp.getDC()->GetTextExtent(s.cstr());
}

void SystemPainter::makeSpaceForText() {
  int leftMargin   = m_yAxisPainter->getMaxTextOffset() + 2;
  int rightMargin  = AbstractAxisPainter::ARROW_SIZE / 2;
  int topMargin    = AbstractAxisPainter::ARROW_SIZE / 2;
  int bottomMargin = m_xAxisPainter->getMaxTextOffset() + getTextExtent(m_yAxisPainter->getMaxText()).cy + 1;

  Rectangle2D fr = m_vp.getFromRectangle();
  const IntervalTransformation &xtr = m_vp.getXTransformation();
  const IntervalTransformation &ytr = m_vp.getYTransformation();
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
    m_vp.setFromRectangle(fr);
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
  return m_vp.getToRectangle();
}

AbstractAxisPainter *SystemPainter::createAxisPainter(bool xAxis, AxisType type) {
  switch(type) {
  case AXIS_LINEAR             : return new LinearAxisPainter(            *this, xAxis);
  case AXIS_LOGARITHMIC        : return new LogarithmicAxisPainter(       *this, xAxis);
  case AXIS_NORMAL_DISTRIBUTION: return new NormalDistributionAxisPainter(*this, xAxis);
  case AXIS_DATE               : return new DateAxisPainter(              *this, xAxis);
  default                      : throwException(_T("Invalid AxisType (=%d)"),type);
                                 return NULL;
  }
}
