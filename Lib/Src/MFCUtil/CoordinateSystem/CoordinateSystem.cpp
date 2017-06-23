#include "pch.h"
#include <Date.h>
#include <MFCUtil/WinTools.h>
#include <MFCUtil/ColorSpace.h>
#include <MFCUtil/Coordinatesystem/SystemPainter.h>
#include <MFCUtil/Coordinatesystem/CoordinateSystem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DEFINECLASSNAME(CCoordinateSystem);

CCoordinateSystem::CCoordinateSystem() {
  m_retainAspectRatio = m_vp.isRetainingAspectRatio();
  m_grid              = false;
  m_xAxisType         = AXIS_LINEAR;
  m_yAxisType         = AXIS_LINEAR;
  m_backgroundColor   = WHITE;
  m_axisColor         = BLACK;
  m_autoScale         = m_autoSpace = true;
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

  if(!Create(EMPTYSTRING, style, rect, parent, id)) {
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

void CCoordinateSystem::deleteAllObjects() { // remove AND delete all objects
  for(size_t i = 0; i < m_objectArray.size(); i++) {
    delete m_objectArray[i];
  }
  m_objectArray.clear();
}

int CCoordinateSystem::findObject(const CoordinateSystemObject *object) const {
  for(size_t i = 0; i < m_objectArray.size(); i++) {
    if(m_objectArray[i] == object) {
      return (int)i;
    }
  }
  return -1;
}

void CCoordinateSystem::OnPaint() {
  paint(CPaintDC(this));
}

void CCoordinateSystem::paint(CDC &dc) {
  const CRect cr = getClientRect(this);
  m_vp.setToRectangle(Rectangle2DR::makeBottomUpRectangle(cr));
  m_occupationMap.clear();
  m_occupationMap.setWindowSize(cr.Size());
  if (m_autoScale && m_objectArray.size() > 0) {
    setDataRange(findSmallestDataRange(), m_autoSpace);
  }
  m_vp.setDC(&dc);
  m_vp.setClipping(true);
  SystemPainter painter(this);
  painter.paint();
  for(size_t i = 0; i < m_objectArray.size(); i++) {
    m_objectArray[i]->paint(*this);
  }
  m_vp.setClipping(false);
}

DoubleInterval CCoordinateSystem::getDefaultInterval(AxisType type) { // static
  switch(type) {
  case AXIS_LINEAR              : return DoubleInterval(-10,10);
  case AXIS_LOGARITHMIC         : return DoubleInterval(0.1,10);
  case AXIS_NORMAL_DISTRIBUTION : return DoubleInterval(0.001,0.999);
  case AXIS_DATE                :
    { const Timestamp now = Timestamp();
      return DoubleInterval(now.getDATE(), (now+1).getDATE());
    }
  default:
    throwInvalidArgumentException(__TFUNCTION__,_T("axistype=%d"), type);
    return DoubleInterval(0,0);
  }
}

DataRange CCoordinateSystem::getDefaultDataRange(AxisType xType, AxisType yType) { // static 
  return DataRange(getDefaultInterval(xType), getDefaultInterval(yType));
}

// should only be called when m_objectArray.size() > 0
DataRange CCoordinateSystem::findSmallestDataRange() const {
  DataRange result = m_objectArray[0]->getDataRange();
  for (size_t i = 1; i < m_objectArray.size(); i++) {
    result += m_objectArray[i]->getDataRange();
  }
  return result;
}

void CCoordinateSystem::setOccupiedPoint(const Point2D &p) {
  m_occupationMap.setOccupiedPoint(m_vp.forwardTransform(p));
}

void CCoordinateSystem::setOccupiedLine(const Point2D &p1, const Point2D &p2) {
  m_occupationMap.setOccupiedLine(m_vp.forwardTransform(p1), m_vp.forwardTransform(p2));
}

void CCoordinateSystem::setOccupiedPoints(const Point2DArray &pa) {
  PointArray tmp(pa.size());
  m_occupationMap.setOccupiedPoints(transformPoint2DArray(tmp, pa));
}

void CCoordinateSystem::setOccupiedConnectedPoints(const Point2DArray &pa) {
  if(pa.size() < 2) return;
  PointArray tmp(pa.size());
  m_occupationMap.setOccupiedConnectedPoints(transformPoint2DArray(tmp, pa));
}

PointArray &CCoordinateSystem::transformPoint2DArray(PointArray &dst, const Point2DArray &src) const {
  dst.clear(src.size());
  if(src.size() == 0) return dst;
  for (const Point2D *p = &src[0], *end = &src.last(); p <= end;) {
    dst.add(m_vp.forwardTransform(*(p++)));
  }
  return dst;
}

class PointArrayObject : public CoordinateSystemObject {
private:
  DataRange    m_dataRange;
protected:
  COLORREF     m_color;
  Point2DArray m_points;
  void initDataRange();
public:
  PointArrayObject(COLORREF color) : m_color(color) {
  }
  PointArrayObject(const Point2DArray &points, COLORREF color);
  void paint(CCoordinateSystem &cs);
  const DataRange &getDataRange() const {
    return m_dataRange;
  }
};

PointArrayObject::PointArrayObject(const Point2DArray &points, COLORREF color) : m_color(color), m_points(points) {
  initDataRange();
}

void PointArrayObject::initDataRange() {
  m_dataRange = DataRange(m_points);
}

void PointArrayObject::paint(CCoordinateSystem &cs) {
  if(m_points.size() <= 1) return;
  CPen pen;
  pen.CreatePen(PS_SOLID, 1, m_color);
  Viewport2D &vp = cs.getViewport();
  CPen *oldPen = vp.SelectObject(&pen);
  const Point2D *p = &m_points[0], *end = &m_points.last();
  vp.MoveTo(*p);
  while(p++ < end) {
    vp.LineTo(*p);
  }
  cs.setOccupiedConnectedPoints(m_points);
  vp.SelectObject(oldPen);
}

void CCoordinateSystem::addPointObject(const Point2DArray &a, COLORREF color) {
  addObject(new PointArrayObject(a, color));
}

class FunctionObject : public PointArrayObject {
public:
  FunctionObject(Function &f, const DoubleInterval &range, UINT n, COLORREF color);
};

FunctionObject::FunctionObject(Function &f, const DoubleInterval &range, UINT n, COLORREF color) : PointArrayObject(color) {
  m_points.setCapacity(n);
  for(UINT i = 0; i < n; i++) {
    try {
      const Real t = (Real)i / (n-1);
      const Real x = range.getFrom() * (1.0-t) + t * range.getTo();
      const Real y = f(x);
      m_points.add(Point2D(x, y));
    }
    catch (...) {
      // ignore
    }
  }
  initDataRange();
}

void CCoordinateSystem::addFunctionObject(Function &f, const DoubleInterval *range, COLORREF color) {
  const DoubleInterval          toRange = m_vp.getToRectangle().getXInterval();
  const IntervalTransformation &tr      = m_vp.getXTransformation();
  const DoubleInterval          xRange  = range ? *range : tr.backwardTransform(toRange);
  const UINT                    n       = (UINT)toRange.getLength() + 1;

  addObject(new FunctionObject(f, xRange, n, color));
}

void CCoordinateSystem::setDC(CDC &dc) {
  m_vp.setDC(&dc);
}

void CCoordinateSystem::setDataRange(const DataRange &dataRange, bool makeSpace) {
  setFromRectangle(dataRange, makeSpace);
}

void CCoordinateSystem::setFromRectangle(const Rectangle2D &rectangle, bool makeSpace) {
  Rectangle2D r = Rectangle2D::makePositiveRectangle(rectangle);
  if(r.getWidth() == 0) {
    const double dw = r.m_x == 0 ? 20 : r.m_x / 20;
    r.m_x -= dw / 2;
    r.m_w += dw;
  }
  if(r.getHeight() == 0) {
    const double dh = r.m_y == 0 ? 20 : r.m_y / 20;
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
