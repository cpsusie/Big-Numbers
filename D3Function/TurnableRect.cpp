#include "stdafx.h"
#include <Math/Point2DToPoint2DFunctions.h>
#include "TurnableRect.h"
#include "BitmapRotate.h"

RectMark::RectMark(MarkId markId, const CPoint &p, int imageId, int degree) {
  m_markId = markId;
  m_image  = getRotatedBitmapResource(imageId, degree);
  const CSize bmSize = getBitmapSize(*m_image);
  left   = p.x  - bmSize.cx/2;
  top    = p.y  - bmSize.cy/2;
  right  = left + bmSize.cx;
  bottom = top  + bmSize.cy;
}

TurnableRect::TurnableRect() {
  Point2D p(0,0);
  for(int i = 0; i < 4; i++) add(p);
  m_selectedMarkIndex = -1;
  m_rotationCenter = getCenter();
}

TurnableRect::TurnableRect(const Rectangle2D &r) {
  add(r.LT());
  add(r.RT());
  add(r.RB());
  add(r.LB());
  m_selectedMarkIndex = -1;
  m_rotationCenter = getCenter();
}

Point2D TurnableRect::getMarkPoint(MarkId id) {
  switch(id) {
  case TL: return getTopLeft();
  case TR: return getTopRight();
  case BL: return getBottomLeft();
  case BR: return getBottomRight();
  case TC: return getTopCenter();
  case BC: return getBottomCenter();
  case LC: return getLeftCenter();
  case RC: return getRightCenter();
  case C : return m_rotationCenter;
  default: throwException(_T("getMarkPoint:Invalid id:%d"),id);
  }
  return getTopLeft();
}

Point2D TurnableRect::getSelectedMarkPoint() {
  return getMarkPoint(getSelectedMark());
}

Point2D TurnableRect::getStretchOrigin() {
  switch(getSelectedMark()) {
  case TL: return getBottomRight();
  case TR: return getBottomLeft();
  case BL: return getTopRight();
  case BR: return getTopLeft();
  case TC: return getBottomCenter();
  case BC: return getTopCenter();
  case LC: return getRightCenter();
  case RC: return getLeftCenter();
  default: throwException(_T("StretchOrigin not defined for mark %d"),getSelectedMark());
  }
  return getBottomRight();
}

Point2D TurnableRect::getStretchDir() {
  return getSelectedMarkPoint() - getCenter();
}

Point2D TurnableRect::getSkewDir() {
  switch(getSelectedMark()) {
  case TC:
  case BC: return getU1();
  case LC:
  case RC: return getU2();
  default:
    throwException(_T("Skewdir not defined for mark %d"), getSelectedMark());
  }
  return getU1();
}

Point2D TurnableRect::getRotateDir() {
  switch(getSelectedMark()) {
  case TL:
  case TR:
  case BL:
  case BR:
    return ::rotate(Point2D(getSelectedMarkPoint() - m_rotationCenter),M_PI/2);
  default:
    throwException(_T("Rotatedir not defined for mark %d"), getSelectedMark());
  }
  return getU1();
}

// ----------------------------------- PointTransformations ----------------------------------

FunctionR2R2 *TurnableRect::getMoveTransformation(const Point2D &dp) {
  FunctionR2R2  *result = new MoveTransformation(dp); TRACE_NEW(result);
  return result;
}

FunctionR2R2 *TurnableRect::getStretchTransformation(const Point2D &dp) {
  const Point2D  dir    = unitVector(getStretchDir());
  const Point2D  step   = (dp * dir) * dir;
  FunctionR2R2  *result = new StretchTransformation(getStretchOrigin(),getU1(),getU2(),getSelectedMarkPoint(),step); TRACE_NEW(result);
  return result;
}

FunctionR2R2 *TurnableRect::getRotateTransformation(const Point2D &dp) {
  const Point2D  dir    = unitVector(getRotateDir());
  const Point2D  step   = (dp * dir) * dir;
  const double   theta  = angle2D(Point2D(getSelectedMarkPoint() - m_rotationCenter), Point2D(getSelectedMarkPoint() + step - m_rotationCenter));
  FunctionR2R2  *result = new RotateTransformation(m_rotationCenter, theta); TRACE_NEW(result);
  return result;
}

FunctionR2R2 *TurnableRect::getSkewTransformation(const Point2D &dp) {
  const Point2D  dir    = unitVector(getSkewDir());
  const Point2D  step   = (dp * dir) * dir;
  FunctionR2R2  *result = new SkewTransformation(getStretchOrigin(),getU1(),getU2(),getSelectedMarkPoint(),step); TRACE_NEW(result);
  return result;
}

FunctionR2R2 *TurnableRect::getMirrorTransformation(bool horizontal) {
  FunctionR2R2  *result;
  if(horizontal) {
    result = new MirrorTransformation(getLeftCenter(),getRightCenter()); TRACE_NEW(result);
  } else {
    result = new MirrorTransformation(getTopCenter(),getBottomCenter()); TRACE_NEW(result);
  }
  return result;
}

void TurnableRect::applyFunction(FunctionR2R2 *f, Point2DRefArray &pointArray) {
  for(size_t i = 0; i < size(); i++) {
    (*this)[i] = (*f)((*this)[i]);
  }
  for(size_t i = 0; i < pointArray.size(); i++) {
    *pointArray[i] = (*f)(*pointArray[i]);
  }
  m_rotationCenter = (*f)(m_rotationCenter);
}

void TurnableRect::move(const Point2D &dp, Point2DRefArray &pointArray) {
  FunctionR2R2 *f = getMoveTransformation(dp);
  applyFunction(f, pointArray);
  SAFEDELETE(f);
}

void TurnableRect::stretch(const Point2D &dp, Point2DRefArray &pointArray) {
  FunctionR2R2 *f = getStretchTransformation(dp);
  applyFunction(f, pointArray);
  SAFEDELETE(f);
}

void TurnableRect::rotate(const Point2D &dp, Point2DRefArray &pointArray) {
  FunctionR2R2 *f = NULL;
  switch(getSelectedMark()) {
  case TL:
  case TR:
  case BL:
  case BR:
    f = getRotateTransformation(dp);
    applyFunction(f, pointArray);
    SAFEDELETE(f);
    break;
  case TC:
  case BC:
  case LC:
  case RC:
    f = getSkewTransformation(dp);
    applyFunction(f, pointArray);
    SAFEDELETE(f);
    break;
  case C:
    m_rotationCenter += dp;
    break;
  }
}

void TurnableRect::mirror(bool horizontal, Point2DRefArray &pointArray) {
  FunctionR2R2 *f = getMirrorTransformation(horizontal);
  applyFunction(f, pointArray);
  SAFEDELETE(f);
}

void TurnableRect::scale(double factor) {
  Point2D center = getCenter();
  double f = (factor-1) / 2;
  getTopLeft()     += (getTopLeft()     - center) * f;
  getTopRight()    += (getTopRight()    - center) * f;
  getBottomRight() += (getBottomRight() - center) * f;
  getBottomLeft()  += (getBottomLeft()  - center) * f;
}

Point2D TurnableRect::getCenter() {
  return (getTopLeft() + getBottomRight())/2;
}

Point2D TurnableRect::getTopCenter() {
  return (getTopLeft() + getTopRight())/2;
}

Point2D TurnableRect::getBottomCenter() {
  return (getBottomLeft() + getBottomRight())/2;
}

Point2D TurnableRect::getLeftCenter() {
  return (getTopLeft() + getBottomLeft())/2;
}

Point2D TurnableRect::getRightCenter() {
  return (getTopRight() + getBottomRight())/2;
}

void TurnableRect::addMarkRect(Viewport2D &vp, MarkId markId, int imageId, int degree) {
  m_marks.add(RectMark(markId, (CPoint)vp.forwardTransform(getMarkPoint(markId)),imageId,degree));
}

void TurnableRect::repaint(Viewport2D &vp, ProfileEditorState state) {

#define DEGREE(v)     (int)RAD2GRAD(angle2D(v, Point2D(1,0)))
#define VDEGREE(id,p) DEGREE(Point2D(getMarkPoint(id)-p))
#define VSTRETCH(id)  VDEGREE(id, getCenter())
#define VROT(id)      VDEGREE(id, m_rotationCenter)

#define IDB_SQUARE  IDB_BLACKSQUAREBITMAP
#define IDB_LRARROW IDB_LEFTRIGHTARROWBITMAP
#define IDB_CORNER  IDB_ROTATECORNERBITMAP
#define IDB_CIRCLE  IDB_CIRCLEARROWBITMAP


  int v1 = DEGREE(getU1());
  int v2 = DEGREE(getU2());

  m_marks.clear();
  switch(state) {
  case IDLE       :
  case DRAGGING   :
    return;

  case MOVING     :
    addMarkRect(vp, TL, IDB_SQUARE   , v1          );
    addMarkRect(vp, TC, IDB_SQUARE   , v1          );
    addMarkRect(vp, TR, IDB_SQUARE   , v1          );
    addMarkRect(vp, RC, IDB_SQUARE   , v1          );
    addMarkRect(vp, BR, IDB_SQUARE   , v1          );
    addMarkRect(vp, BC, IDB_SQUARE   , v1          );
    addMarkRect(vp, BL, IDB_SQUARE   , v1          );
    addMarkRect(vp, LC, IDB_SQUARE   , v1          );
    break;

  case STRETCHING  :
    addMarkRect(vp, TL, IDB_LRARROW  , VSTRETCH(TL));
    addMarkRect(vp, TC, IDB_LRARROW  , v2          );
    addMarkRect(vp, TR, IDB_LRARROW  , VSTRETCH(TR));
    addMarkRect(vp, RC, IDB_LRARROW  , v1          );
    addMarkRect(vp, BR, IDB_LRARROW  , VSTRETCH(BR));
    addMarkRect(vp, BC, IDB_LRARROW  , v2          );
    addMarkRect(vp, BL, IDB_LRARROW  , VSTRETCH(BL));
    addMarkRect(vp, LC, IDB_LRARROW  , v1          );
    break;

  case ROTATING   :
    addMarkRect(vp, TL, IDB_CORNER   , VROT(TL)    );
    addMarkRect(vp, TC, IDB_LRARROW  , v1          );
    addMarkRect(vp, TR, IDB_CORNER   , VROT(TR)    );
    addMarkRect(vp, RC, IDB_LRARROW  , v2          );
    addMarkRect(vp, BR, IDB_CORNER   , VROT(BR)    );
    addMarkRect(vp, BC, IDB_LRARROW  , v1          );
    addMarkRect(vp, BL, IDB_CORNER   , VROT(BL)    );
    addMarkRect(vp, LC, IDB_LRARROW  , v2          );
    addMarkRect(vp, C , IDB_CIRCLE   , v1          );
    break;
  }

  CDC dc;
  dc.CreateCompatibleDC(vp. getDC());

  for(size_t i = 0; i < m_marks.size(); i++) {
    RectMark &m = m_marks[i];
    dc.SelectObject(m.m_image);
    vp.getDC()->BitBlt(m.left, m.top, m.Width(), m.Height(), &dc, 0, 0, SRCAND);
  }
  dc.DeleteDC();
}

bool TurnableRect::pointOnMarkRect(const CPoint &p) {
  for(size_t i = 0; i < m_marks.size(); i++) {
    if(m_marks[i].PtInRect(p)) {
      m_selectedMarkIndex = (int)i;
      return true;
    }
  }
  return false;
}
