#include "stdafx.h"
#include "DrawToolSelect.h"

void DrawToolSelect::startDragRect() {
  m_dragRect = CRect(m_mouseDownPoint.x,m_mouseDownPoint.y,m_mouseDownPoint.x,m_mouseDownPoint.y);
  SIZE size;
  size.cx = size.cy = 1;
  m_editor.getViewport().getDC()->DrawDragRect(&m_dragRect,size,m_dragRect,size);
  drawState();
}

void DrawToolSelect::redrawDragRect(const CPoint &p) {
  CRect newRect(m_mouseDownPoint,p);
  if(newRect.left > newRect.right ) std::swap(newRect.left,newRect.right);
  if(newRect.top  > newRect.bottom) std::swap(newRect.top,newRect.bottom);
  SIZE size;
  size.cx = size.cy = 1;
  m_editor.getViewport().getDC()->DrawDragRect(&newRect,size,m_dragRect,size);
  drawState();

  m_dragRect = newRect;
}

void DrawToolSelect::endDragRect() {
  SIZE size,newSize;
  size.cx = size.cy = 1;
  newSize.cx = newSize.cy = 0;
  m_editor.getViewport().getDC()->DrawDragRect(m_dragRect,newSize,m_dragRect,size);
  drawState();
}

class DistanceFinder: public CurveOperator {
private:
  Viewport2D &m_vp;
  CPoint    m_point;
public:
  double    m_minDist;
  DistanceFinder(Viewport2D &vp, const CPoint &p);
  void line(const Point2D &from, const Point2D &to);
};

DistanceFinder::DistanceFinder(Viewport2D &vp, const CPoint &p) : m_vp(vp) {
  m_minDist = -1;
  m_point   = p;
}

void DistanceFinder::line(const Point2D &from, const Point2D &to) {
  double dist = distanceFromLineSegment(m_vp.forwardTransform(from),m_vp.forwardTransform(to),Point2D(m_point));
  if(m_minDist < 0 || dist < m_minDist) {
    m_minDist = dist;
  }
}

#define MAXDIST 8

ProfilePolygon *DrawToolSelect::findNearestPolygon(const CPoint &p) {
  Profile        &profile        = m_editor.getProfile();
  double          minDist        = -1;
  ProfilePolygon *nearestPolygon = NULL;

  for(size_t i = 0; i < profile.m_polygonArray.size(); i++) {
    ProfilePolygon &polygon = profile.m_polygonArray[i];
    DistanceFinder df(m_editor.getViewport(),p);
    polygon.apply(df);

    if((minDist < 0) || (df.m_minDist < minDist)) {
      minDist        = df.m_minDist;
      nearestPolygon = &polygon;
    }
  }
  return minDist < MAXDIST ? nearestPolygon : NULL;
}

Point2D *DrawToolSelect::findNearestPoint(const CPoint &p) {
  CompactArray<Point2D*> points = m_editor.getProfile().getAllPointsRef();
  double minDist        = -1;
  Point2D *nearestPoint = NULL;

  for(size_t i = 0; i < points.size(); i++) {
    Point2D     *point = points[i];
    const double dist  = distance(Point2D(p),m_editor.getViewport().forwardTransform(*point));
    if((minDist < 0) || (dist < minDist)) {
      minDist      = dist;
      nearestPoint = point;
    }
  }
  return minDist < MAXDIST ? nearestPoint : NULL;
}

void DrawToolSelect::moveSelectedPoints(const Point2D &dp) {
  for(size_t i = 0; i < m_selectedPoints.size(); i++) {
    *m_selectedPoints[i] += dp;
  }
}

bool DrawToolSelect::OnLButtonDown(UINT nFlags, CPoint point) {
  m_mouseDownPoint = point;
  ProfilePolygon *np = findNearestPolygon(point);
  switch(m_state) {
  case IDLE       :
  case MOVING    :
    if(np != NULL) { // click on polygon
      if(nFlags & MK_SHIFT) { // shift pressed
        if(isSelected(np)) {
          unselect(np);
        } else {
          select(np);
        }
      } else { // shift not pressed
        if(!isSelected(np)) {
          unselectAll();
          select(np);
        }
      }
      if(!m_polygonSet.isEmpty()) {
        m_polygonSet.evaluateBox();
        m_state = MOVING;
      } else {
        m_state = IDLE;
      }
      repaintAll();
    } else if(m_polygonSet.pointInRect(m_editor.getViewport().backwardTransform(point)) || m_polygonSet.pointOnMarkRect(point)) {
//      m_polygonSet.evaluateBox();
      m_state = MOVING;
      repaintAll();
    } else { // click outside everything
      unselectAll();
      m_state = DRAGGING;
      repaintProfile();
      startDragRect();
      repaintScreen();
    }
    break;

  case STRETCHING :
    if(!m_polygonSet.pointOnMarkRect(point)) {
      m_state = IDLE;
    }
    repaintAll();
    break;

  case ROTATING   :
    if(!m_polygonSet.pointOnMarkRect(point)) {
      m_state = IDLE;
    }
    repaintAll();
    break;

  case DRAGGING   :
    break;
  }
  m_lastMousePoint = point;
  return true;
}

bool DrawToolSelect::OnLButtonDblClk(UINT nFlags, CPoint point) {
  switch(m_state) {
  case IDLE       :
    break;

  case MOVING     :
    if(m_polygonSet.pointOnMarkRect(point)) {
      m_state = STRETCHING;
      repaintAll();
    }
    break;
  case STRETCHING :
    if(m_polygonSet.pointOnMarkRect(point)) {
      m_state = ROTATING;
      repaintAll();
    }
    break;
  case ROTATING   :
    if(m_polygonSet.pointOnMarkRect(point)) {
      m_state = MOVING;
      repaintAll();
    }
    break;

  case DRAGGING   :
    break;
  }
  return true;
}

bool DrawToolSelect::OnLButtonUp(UINT nFlags, CPoint point) {
  switch(m_state) {
  case IDLE       :
  case MOVING     :
  case STRETCHING :
  case ROTATING   :
    break;
  case DRAGGING   :
    selectPolygonsInRect(m_editor.getViewport().backwardTransform(m_dragRect));
    m_state = m_polygonSet.isEmpty() ? IDLE : MOVING;
    endDragRect();
    m_polygonSet.evaluateBox();
    repaintAll();
    break;
  }
  return true;
}

bool DrawToolSelect::OnRButtonDown(UINT nFlags, CPoint point) {
  m_mouseDownPoint = point;
  Point2D *np = findNearestPoint(point);
  switch(m_state) {
  case IDLE       :
  case MOVING     :
  case STRETCHING :
  case ROTATING   :
    if(np != NULL) { // click on point
      if(nFlags & MK_SHIFT) { // shift pressed
        if(isSelected(np)) {
          unselect(np);
        } else {
          select(np);
        }
      } else { // shift not pressed
        if(!isSelected(np)) {
          unselectAll();
          select(np);
        }
      }
      if(!m_selectedPoints.isEmpty()) {
        m_state = MOVING;
      }
      repaintAll();
    } else {
      m_state = DRAGGING;
      unselectAll();
      repaintProfile();
      startDragRect();
      repaintScreen();
    }
    break;
  case DRAGGING   :
    break;
  }
  m_lastMousePoint = point;
  return true;
}

bool DrawToolSelect::OnRButtonUp(UINT nFlags, CPoint point) {
  switch(m_state) {
  case IDLE       :
  case MOVING     :
  case STRETCHING :
  case ROTATING   :
    break;
  case DRAGGING   :
    selectPointsInRect(m_editor.getViewport().backwardTransform(m_dragRect));
    endDragRect();
    m_state = m_selectedPoints.isEmpty() ? IDLE : MOVING;
    repaintAll();
    break;
  }
  return true;
}

void DrawToolSelect::adjustMousePosition() {
//  m_editor.setMousePosition(m_polygonSet.getSelectedMarkPoint());
}

bool DrawToolSelect::OnMouseMove(UINT nFlags, CPoint point) {
  Point2D dp = m_editor.getViewport().backwardTransform(point) - m_editor.getViewport().backwardTransform(m_lastMousePoint);
  switch(m_state) {
  case IDLE       :
    break;

  case MOVING     :
    if(nFlags & MK_LBUTTON && !m_polygonSet.isEmpty()) {
      m_polygonSet.move(dp);
      repaintAll();
    } else if(nFlags & MK_RBUTTON && !m_selectedPoints.isEmpty()) {
      moveSelectedPoints(dp);
      repaintAll();
    }
    break;
  case STRETCHING :
    if(nFlags & MK_LBUTTON) {
      m_polygonSet.stretch(dp);
      adjustMousePosition();
      repaintAll();
    }
    break;
  case ROTATING   :
    if(nFlags & MK_LBUTTON) {
      m_polygonSet.rotate(dp);
      adjustMousePosition();
      repaintAll();
    }
    break;
  case DRAGGING   :
    if(nFlags & (MK_LBUTTON | MK_RBUTTON)) {
      redrawDragRect(point);
      repaintScreen();
    }
    break;

  }
  m_lastMousePoint = point;
  return true;
}
