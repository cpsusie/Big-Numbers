#include "stdafx.h"
#include "Point2DToPoint2DFunctions.h"
#include "TurnableRect.h"
#include "BitmapRotate.h"

MarkId getOppositeMarkId(MarkId id) {
  switch(id) {
  case BL: return TR;
  case BR: return TL;
  case TR: return BL;
  case TL: return BR;
  case BC: return TC;
  case RC: return LC;
  case TC: return BC;
  case LC: return RC;
  case C : return C;
  }
  throwInvalidArgumentException(__TFUNCTION__, _T("id=%d"), id);
  return C;
}

RectMark::RectMark(MarkId markId, const CPoint &p, int imageId, int degree)
: m_markId(markId)
, m_image(getRotatedBitmapResource(imageId, degree))
{
  const CSize bmSize = getBitmapSize(*m_image);
  left   = p.x  - bmSize.cx/2;
  top    = p.y  - bmSize.cy/2;
  right  = left + bmSize.cx;
  bottom = top  + bmSize.cy;
}

TurnableRect::TurnableRect()
: m_selectedMarkIndex(-1)
{
  Point2D p(0,0);
  for(int i = 0; i < 4; i++) {
    add(p);
  }
  m_rotationCenter    = getCenter();
}

TurnableRect::TurnableRect(const Rectangle2D &r)
: m_selectedMarkIndex(-1)
{
  add(r.LB());
  add(r.RB());
  add(r.RT());
  add(r.LT());
  m_rotationCenter = getCenter();
}

Point2D TurnableRect::getMarkPoint(MarkId id) const {
  switch(id) {
  case BL: return getBottomLeft();
  case BR: return getBottomRight();
  case TR: return getTopRight();
  case TL: return getTopLeft();
  case BC: return getBottomCenter();
  case RC: return getRightCenter();
  case TC: return getTopCenter();
  case LC: return getLeftCenter();
  case C : return m_rotationCenter;
  }
  throwInvalidArgumentException(__TFUNCTION__, _T("id=%d"), id);
  return getTopLeft();
}

Point2D TurnableRect::getSkewDir() const {
  switch(getSelectedMark()) {
  case BC:
  case TC: return getU1();
  case LC:
  case RC: return getU2();
  default:
    throwException(_T("Skewdir not defined for mark %d"), getSelectedMark());
  }
  return getU1();
}

Point2D TurnableRect::getRotateDir() const {
  switch(getSelectedMark()) {
  case BL:
  case BR:
  case TR:
  case TL:
    return ::rotate(getSelectedMarkPoint() - m_rotationCenter, M_PI/2);
  default:
    throwException(_T("Rotatedir not defined for mark %d"), getSelectedMark());
  }
  return getU1();
}

// ----------------------------------- PointTransformations ----------------------------------

FunctionR2R2 *TurnableRect::getMoveTransformation(const Point2D &dp) const {
  return new MoveTransformation(dp);
}

FunctionR2R2 *TurnableRect::getStretchTransformation(const Point2D &dp) const {
  const Point2D  dir    = unitVector(getStretchDir());
  const Point2D  step   = (dp * dir) * dir;
  return new StretchTransformation(getStretchOrigin(),getU1(),getU2(),getSelectedMarkPoint(),step);
}

FunctionR2R2 *TurnableRect::getRotateTransformation(const Point2D &dp) const {
  const Point2D  dir    = unitVector(getRotateDir());
  const Point2D  step   = (dp * dir) * dir;
  const double   theta  = angle2D(getSelectedMarkPoint() - m_rotationCenter, getSelectedMarkPoint() + step - m_rotationCenter);
  return new RotateTransformation(m_rotationCenter, theta);
}

FunctionR2R2 *TurnableRect::getSkewTransformation(const Point2D &dp) const {
  const Point2D  dir    = unitVector(getSkewDir());
  const Point2D  step   = (dp * dir) * dir;
  return new SkewTransformation(getStretchOrigin(),getU1(),getU2(),getSelectedMarkPoint(),step);
}

FunctionR2R2 *TurnableRect::getMirrorTransformation(bool horizontal) const {
  return horizontal
       ? new MirrorTransformation(getLeftCenter(),getRightCenter())
       : new MirrorTransformation(getTopCenter(),getBottomCenter());
}

void TurnableRect::applyFunction(FunctionR2R2 *fp, Point2DRefArray &pointArray) {
  try {
    FunctionR2R2 &f = *fp;
    for(Point2D *p = begin(), *endp = end(); p < endp; p++) {
      *p = f(*p);
    }
    m_rotationCenter = f(m_rotationCenter);
    for(Point2D **p = pointArray.begin(), **endp = pointArray.end(); p < endp; p++) {
      **p = f(**p);
    }
    delete fp;
  } catch(...) {
    delete fp;
    throw;
  }
}

void TurnableRect::rotate(const Point2D &dp, Point2DRefArray &pointArray) {
  switch(getSelectedMark()) {
  case BL:
  case BR:
  case TL:
  case TR:
    applyFunction(getRotateTransformation(dp), pointArray);
    break;
  case LC:
  case RC:
  case TC:
  case BC:
    applyFunction(getSkewTransformation(dp), pointArray);
    break;
  case C:
    m_rotationCenter += dp;
    break;
  }
}

void TurnableRect::scale(double factor) {
  const Point2D center = getCenter();
  const double  f      = (factor-1) / 2;
  getBottomLeft()  += (getBottomLeft()  - center) * f;
  getBottomRight() += (getBottomRight() - center) * f;
  getTopRight()    += (getTopRight()    - center) * f;
  getTopLeft()     += (getTopLeft()     - center) * f;
}

void TurnableRect::addMarkRect(Viewport2D &vp, MarkId markId, int imageId, int degree) {
  m_marks.add(RectMark(markId, (CPoint)vp.forwardTransform(getMarkPoint(markId)),imageId,degree));
}

void TurnableRect::repaint(Viewport2D &vp, ProfileEditorState state) {
  const Point2D e1(1, 0);
#define DEGREE(v)              (int)RAD2GRAD(angle2D(e1, v))
#define VDEGREE(from,toMarkId) DEGREE(Point2D(getMarkPoint(toMarkId))-from)
#define VSTRETCH(    markId  ) VDEGREE(getCenter()     , markId)
#define VROT(        markId  ) VDEGREE(m_rotationCenter, markId)

#define IDB_SQUARE             IDB_BLACKSQUAREBITMAP
#define IDB_LRARROW            IDB_LEFTRIGHTARROWBITMAP
#define IDB_CORNER             IDB_ROTATECORNERBITMAP
#define IDB_CIRCLE             IDB_CIRCLEARROWBITMAP

  const int v1 = DEGREE(getU1());
  const int v2 = DEGREE(getU2());

  m_marks.clear();
  switch(state) {
  case IDLE       :
  case DRAGGING   :
    return;

  case MOVING     :
    addMarkRect(vp, BL, IDB_SQUARE   , v1          );
    addMarkRect(vp, BR, IDB_SQUARE   , v1          );
    addMarkRect(vp, TR, IDB_SQUARE   , v1          );
    addMarkRect(vp, TL, IDB_SQUARE   , v1          );
    addMarkRect(vp, BC, IDB_SQUARE   , v1          );
    addMarkRect(vp, RC, IDB_SQUARE   , v1          );
    addMarkRect(vp, TC, IDB_SQUARE   , v1          );
    addMarkRect(vp, LC, IDB_SQUARE   , v1          );
    break;

  case STRETCHING  :
    addMarkRect(vp, BL, IDB_LRARROW  , VSTRETCH(BL));
    addMarkRect(vp, BR, IDB_LRARROW  , VSTRETCH(BR));
    addMarkRect(vp, TR, IDB_LRARROW  , VSTRETCH(TR));
    addMarkRect(vp, TL, IDB_LRARROW  , VSTRETCH(TL));
    addMarkRect(vp, BC, IDB_LRARROW  , v2          );
    addMarkRect(vp, RC, IDB_LRARROW  , v1          );
    addMarkRect(vp, TC, IDB_LRARROW  , v2          );
    addMarkRect(vp, LC, IDB_LRARROW  , v1          );
    break;

  case ROTATING   :
    addMarkRect(vp, BL, IDB_CORNER   , VROT(BL)    );
    addMarkRect(vp, BR, IDB_CORNER   , VROT(BR)    );
    addMarkRect(vp, TR, IDB_CORNER   , VROT(TR)    );
    addMarkRect(vp, TL, IDB_CORNER   , VROT(TL)    );
    addMarkRect(vp, BC, IDB_LRARROW  , v1          );
    addMarkRect(vp, RC, IDB_LRARROW  , v2          );
    addMarkRect(vp, TC, IDB_LRARROW  , v1          );
    addMarkRect(vp, LC, IDB_LRARROW  , v2          );
    addMarkRect(vp, C , IDB_CIRCLE   , v1          );
    break;
  }

  CDC dc;
  dc.CreateCompatibleDC(vp.getDC());
  for(const RectMark rm : m_marks) {
    dc.SelectObject(rm.m_image);
    vp.getDC()->BitBlt(rm.left, rm.top, rm.Width(), rm.Height(), &dc, 0, 0, SRCAND);
  }
  dc.DeleteDC();
}

bool TurnableRect::isPointOnMarkRect(const CPoint &p) const {
  int index = 0;
  for(const RectMark rm : m_marks) {
    if(rm.PtInRect(p)) {
      m_selectedMarkIndex = index;
      return true;
    } else {
      index++;
    }
  }
  return false;
}
