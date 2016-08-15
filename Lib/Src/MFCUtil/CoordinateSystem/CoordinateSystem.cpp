#include "stdafx.h"
#include <Date.h>
#include <MFCUtil/WinTools.h>
#include <MFCUtil/Coordinatesystem/SystemPainter.h>
#include <MFCUtil/Coordinatesystem/CoordinateSystem.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WHITE RGB(255,255,255)
#define BLACK RGB(0,0,0)

DEFINECLASSNAME(CCoordinateSystem);

CCoordinateSystem::CCoordinateSystem() {
  m_retainAspectRatio = m_vp.isRetainingAspectRatio();
  m_grid              = false;
  m_xAxisType         = AXIS_LINEAR;
  m_yAxisType         = AXIS_LINEAR;
  m_backgroundColor   = WHITE;
  m_axisColor         = BLACK;
}

CCoordinateSystem::~CCoordinateSystem() {
}

BEGIN_MESSAGE_MAP(CCoordinateSystem, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CCoordinateSystem::substituteControl(CWnd *parent, int id) {
  DEFINEMETHODNAME;
  CWnd *ctrl = parent->GetDlgItem(id);
  if(ctrl == NULL) {
    parent->MessageBox(format(_T("%s:Control %d not found"), method, id).cstr(), _T("Error"), MB_ICONWARNING);
    return;
  }
  const DWORD  style   = ctrl->GetStyle();
  const DWORD  exStyle = ctrl->GetExStyle();
  const CRect  rect    = getWindowRect(ctrl);
//  const String str     = getWindowText(ctrl);
  CFont *font = ctrl->GetFont();
  if(font == NULL) {
    font = parent->GetFont();
  }
  ctrl->DestroyWindow();

  if(!Create(_T(""), style, rect, parent, id)) {
    parent->MessageBox(format(_T("%s:Create failed"), method).cstr(), _T("Error"), MB_ICONWARNING);
    return;
  }
  ModifyStyleEx(0, exStyle);
  SetFont(font, FALSE);
  RectangleTransformation &tr = getTransformation();
  tr.setFromRectangle(getDefaultDataRange(AXIS_LINEAR,AXIS_LINEAR));
  tr.setToRectangle(Rectangle2DR(rect));
}

void CCoordinateSystem::addObject(CoordinateSystemObject *object) {
  if(findObject(object) < 0) {
    m_objectArray.add(object);
  }
}

void CCoordinateSystem::removeObject(CoordinateSystemObject *object) {
  const int index = findObject(object);
  if(index >= 0) {
    m_objectArray.remove(index);
  }
}

int CCoordinateSystem::findObject(const CoordinateSystemObject *object) const {
  for(int i = 0; i < (int)m_objectArray.size(); i++) {
    if(m_objectArray[i] == object) {
      return i;
    }
  }
  return -1;
}

void CCoordinateSystem::OnPaint() {
  paint(CPaintDC(this));
}

void CCoordinateSystem::paint(CDC &dc) {
  m_vp.setToRectangle(Rectangle2DR::makeBottomUpRectangle(getClientRect(this)));
  m_vp.setDC(&dc);
  m_vp.setClipping(true);
  SystemPainter painter(m_vp, GetFont(), m_backgroundColor, m_axisColor, m_xAxisType, m_yAxisType, hasGrid());
  painter.paint();
  for(size_t i = 0; i < m_objectArray.size(); i++) {
    m_objectArray[i]->paint(m_vp);
  }
  m_vp.setClipping(false);
}

DoubleInterval CCoordinateSystem::getDefaultInterval(AxisType type) { // static
  switch(type) {
  case AXIS_LINEAR              : return DoubleInterval(-10,10);
  case AXIS_LOGARITHMIC         : return DoubleInterval(0.1,10);
  case AXIS_NORMAL_DISTRIBUTION : return DoubleInterval(0.001,0.999);
  case AXIS_DATE                :
    { Timestamp now = Timestamp();
      return DoubleInterval(now.getDATE(), (now+1).getDATE());
    }
  default:
    throwException(_T("getDefaultInterval:Illegal axistype:%d"), type);
    return DoubleInterval(0,0);
  }
}

DataRange CCoordinateSystem::getDefaultDataRange(AxisType xType, AxisType yType) { // static 
  return DataRange(getDefaultInterval(xType),getDefaultInterval(yType));
}

void CCoordinateSystem::paintCurve(CDC &dc, const Point2DArray &a, COLORREF color) {
  const int n = (int)a.size();
  if(n > 1) {
    Viewport2D *vp = getViewport(dc);
    CPen pen;
    pen.CreatePen(PS_SOLID,1,color);
    CPen *oldPen = vp->SelectObject(&pen);
    vp->MoveTo(a[0].x, a[0].y);
    for(int i = 1; i < n; i++) {
      const Point2D &p = a[i];
      vp->LineTo(p.x,p.y);
    }
    vp->SelectObject(oldPen);
    vp->setDC(NULL);
  }
}

void CCoordinateSystem::plotfunction(CDC &dc, Function &f, const DoubleInterval *range, COLORREF color) {
  Viewport2D                   *vp      = getViewport(dc);
  const DoubleInterval          toRange = vp->getToRectangle().getXInterval();
  const IntervalTransformation &tr      = vp->getXTransformation();
  const DoubleInterval          xRange  = range ? *range : tr.backwardTransform(toRange);
  const double                  step    = xRange.getLength() / toRange.getLength();

  CPen pen;
  pen.CreatePen(PS_SOLID,1,color);
  CPen *oldPen = vp->SelectObject(&pen);

  try {
    double x = xRange.getFrom();
    double y = getDouble(f(x));
    vp->MoveTo(x, getDouble(f(x)));
    for(x += step; x <= xRange.getTo(); x += step) {
      vp->LineTo(x,getDouble(f(x)));
    }
    vp->SelectObject(oldPen);
    vp->setDC(NULL);
  } catch(...) {
    vp->SelectObject(oldPen);
    vp->setDC(NULL);
    throw;
  }
}

Viewport2D *CCoordinateSystem::getViewport(CDC &dc) {
  m_vp.setDC(&dc);
  return &m_vp;
}

void CCoordinateSystem::setDataRange(const DataRange &dataRange, bool makeSpace) {
  setFromRectangle(dataRange, makeSpace);
}

void CCoordinateSystem::setFromRectangle(const Rectangle2D &rectangle, bool makeSpace) {
  Rectangle2D r = Rectangle2D::makePositiveRectangle(rectangle);
  if(r.getWidth() == 0) {
    double dw = r.m_x == 0 ? 20 : r.m_x / 20;
    r.m_x -= dw / 2;
    r.m_w += dw;
  }
  if(r.getHeight() == 0) {
    double dh = r.m_y == 0 ? 20 : r.m_y / 20;
    r.m_y -= dh / 2;
    r.m_h += dh;
  }
  RectangleTransformation &tr = getTransformation();
  tr.setFromRectangle(r);
  if(makeSpace) {
    tr.zoom(tr.getToRectangle().getCenter(), -0.08);
  }
}

void CCoordinateSystem::setRetainAspectRatio(bool retainAspectRatio) {
  m_retainAspectRatio = retainAspectRatio;
  if(m_retainAspectRatio && canRetainAspectRatio() || !m_retainAspectRatio) {
    m_vp.setRetainAspectRatio(m_retainAspectRatio);
  }
}
