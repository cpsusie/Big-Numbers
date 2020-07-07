#pragma once

#include "DrawTool.h"

class DrawToolSelect : public DrawTool {
private:
  CPoint m_lastMousePoint;
  CPoint m_mouseDownPoint;
  CRect  m_dragRect;
  void            startDragRect();
  void            redrawDragRect(    const CPoint  &p);
  void            endDragRect();
  ProfilePolygon *findNearestPolygon(const CPoint  &p);
  Point2D        *findNearestPoint(  const CPoint  &p);
  void            moveSelectedPoints(const Point2D &dp);
  void            adjustMousePosition();
public:
  DrawToolSelect(ProfileEditor *editor)
    : DrawTool(editor)
  {
  }
  bool OnLButtonDown(  UINT nFlags, CPoint point);
  bool OnLButtonDblClk(UINT nFlags, CPoint point);
  bool OnLButtonUp(    UINT nFlags, CPoint point);
  bool OnRButtonDown(  UINT nFlags, CPoint point);
  bool OnRButtonUp(    UINT nFlags, CPoint point);
  bool OnMouseMove(    UINT nFlags, CPoint point);
};
