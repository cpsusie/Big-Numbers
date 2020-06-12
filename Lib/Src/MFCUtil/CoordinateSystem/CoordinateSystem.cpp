#include "pch.h"
#include <Date.h>
#include <MFCUtil/WinTools.h>
#include <MFCUtil/ColorSpace.h>
#include <MFCUtil/Coordinatesystem/SystemPainter.h>
#include <MFCUtil/Coordinatesystem/CoordinateSystem.h>

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

DEFINECLASSNAME(CCoordinateSystem);

const Viewport2D &CoordinateSystemObject::getViewport() const {
  return getSystem().getViewport();
}

CCoordinateSystem::CCoordinateSystem() {
  m_backgroundColor   = WHITE;
  m_retainAspectRatio = m_vp.isRetainingAspectRatio();
  m_autoScale         = m_autoSpace = true;
  m_systemPainter     = NULL;
}

CCoordinateSystem::~CCoordinateSystem() {
  destroySystemPainter();
}

void CCoordinateSystem::createSystemPainter() {
  destroySystemPainter();
  m_systemPainter = new SystemPainter(this); TRACE_NEW(m_systemPainter);
}

void CCoordinateSystem::destroySystemPainter() {
  SAFEDELETE(m_systemPainter);
}

BEGIN_MESSAGE_MAP(CCoordinateSystem, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CCoordinateSystem::substituteControl(CWnd *parent, int id) {
  DEFINEMETHODNAME;
  CWnd *ctrl = parent->GetDlgItem(id);
  if(ctrl == NULL) {
    showError(_T("%s:Control %d not found"), method, id);
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
    showError(_T("%s:Create failed"), method);
    return;
  }
  ModifyStyleEx(0, exStyle);
  SetFont(font, FALSE);
  RectangleTransformation &tr = m_vp.getTransformation();
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
    CoordinateSystemObject *obj = m_objectArray[i];
    SAFEDELETE(obj);
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
  const CRect cr    = getClientRect(this);
  CDC        *oldDC = m_vp.setDC(&dc);

  try {
    m_vp.setToRectangle(Rectangle2DR::makeBottomUpRectangle(cr));
    m_occupationMap.clear();
    m_occupationMap.setWindowSize(cr.Size());
    if(m_autoScale && m_objectArray.size() > 0) {
      setDataRange(findSmallestDataRange(), m_autoSpace);
    }

    m_vp.setClipping(true);
    createSystemPainter();
    m_systemPainter->paint();
    for(size_t i = 0; i < m_objectArray.size(); i++) {
      m_objectArray[i]->paint(dc);
    }
    m_vp.setClipping(false);
    m_vp.setDC(oldDC);
  } catch(...) {
    m_vp.setClipping(false);
    m_vp.setDC(oldDC);
    throw;
  }
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
    throwInvalidArgumentException(__TFUNCTION__, _T("axistype=%d"), type);
    return DoubleInterval(0,0);
  }
}

DataRange CCoordinateSystem::getDefaultDataRange(AxisType xType, AxisType yType) { // static
  return DataRange(getDefaultInterval(xType), getDefaultInterval(yType));
}

// should only be called when m_objectArray.size() > 0
DataRange CCoordinateSystem::findSmallestDataRange() const {
  DataRange result = m_objectArray[0]->getDataRange();
  for(size_t i = 1; i < m_objectArray.size(); i++) {
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
  for(const Point2D *p = &src[0], *end = &src.last(); p <= end;) {
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
  inline void initDataRange() {
    m_dataRange = DataRange(m_points);
  }
public:
  PointArrayObject(CCoordinateSystem &system, COLORREF color)
    : CoordinateSystemObject(system)
    , m_color(color)
  {
  }
  PointArrayObject(CCoordinateSystem &system, const Point2DArray &points, COLORREF color)
    : CoordinateSystemObject(system)
    , m_points(points)
    , m_color(color)
  {
    initDataRange();
  }
  void paint(CDC &dc);
  const DataRange &getDataRange() const {
    return m_dataRange;
  }
};

void PointArrayObject::paint(CDC &dc) {
  if(m_points.size() <= 1) return;
  CPen pen;
  pen.CreatePen(PS_SOLID, 1, m_color);
  const Viewport2D &vp     = getViewport();
  CDC              *oldDC  = vp.setDC(&dc);
  CPen             *oldPen = vp.SelectObject(&pen);
  const Point2D *p = &m_points[0], *end = &m_points.last();
  vp.MoveTo(*p);
  while(p++ < end) {
    vp.LineTo(*p);
  }
  getSystem().setOccupiedConnectedPoints(m_points);
  vp.SelectObject(oldPen);
  vp.setDC(oldDC);
}

void CCoordinateSystem::addPointObject(const Point2DArray &a, COLORREF color) {
  CoordinateSystemObject *obj = new PointArrayObject(*this, a, color); TRACE_NEW(obj);
  addObject(obj);
}

class FunctionObject : public PointArrayObject {
public:
  FunctionObject(CCoordinateSystem &system, Function &f, const DoubleInterval &range, UINT n, COLORREF color);
};

FunctionObject::FunctionObject(CCoordinateSystem &system, Function &f, const DoubleInterval &range, UINT n, COLORREF color)
: PointArrayObject(system, color)
{
  m_points.setCapacity(n);
  for(UINT i = 0; i < n; i++) {
    try {
      const Real t = (Real)i / (n-1);
      const Real x = range.getFrom() * (1.0-t) + t * range.getTo();
      const Real y = f(x);
      m_points.add(Point2D(x, y));
    } catch(...) {
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
  CoordinateSystemObject       *obj     = new FunctionObject(*this, f, xRange, n, color); TRACE_NEW(obj);
  addObject(obj);
}

void CCoordinateSystem::setDataRange(const DataRange &dataRange, bool makeSpace) {
  int makeSpaceFlags = 0;
  if(makeSpace) {
    makeSpaceFlags |= m_vp.getXTransformation().isLinear() ? X_AXIS : 0;
    makeSpaceFlags |= m_vp.getYTransformation().isLinear() ? Y_AXIS : 0;
  }
  setFromRectangle(dataRange, makeSpaceFlags);
}

void CCoordinateSystem::setFromRectangle(const Rectangle2D &rectangle, int makeSpaceFlags) {
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
  if(makeSpaceFlags) {
    tr.zoom(tr.getToRectangle().getCenter(), -0.08, makeSpaceFlags);
  }
}

void CCoordinateSystem::setRetainAspectRatio(bool retainAspectRatio) {
  m_retainAspectRatio = retainAspectRatio;
  if(m_retainAspectRatio && canRetainAspectRatio() || !m_retainAspectRatio) {
    m_vp.setRetainAspectRatio(m_retainAspectRatio);
  }
}

String CCoordinateSystem::getPointText(const Point2D &p) const {
  return (m_systemPainter == NULL) ? EMPTYSTRING : m_systemPainter->getPointText(p);
}

String CCoordinateSystem::getValueText(AxisIndex axis, double value) const {
  return (m_systemPainter == NULL) ? EMPTYSTRING : m_systemPainter->getValueText(axis, value);
}
