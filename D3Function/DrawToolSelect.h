#pragma once

#include "DrawTool.h"

class DrawToolSelect : public DrawTool {
private:
  CPoint m_lastMousePoint;
  CPoint m_mouseDownPoint;
  CRect  m_dragRect;
  void              startDragRect();
  void              redrawDragRect(    const CPoint  &p);
  void              endDragRect();
  ProfilePolygon2D *findNearestPolygon(const CPoint  &p);
  Point2D          *findNearestPoint(  const CPoint  &p);
  void              moveSelectedPoints(const Point2D &dp);
  void              adjustMousePosition();
public:
  DrawToolSelect(ProfileEditor *editor)
    : DrawTool(editor)
  {
  }
  bool OnLButtonDown(  UINT nFlags, CPoint point) override;
  bool OnLButtonDblClk(UINT nFlags, CPoint point) override;
  bool OnLButtonUp(    UINT nFlags, CPoint point) override;
  bool OnRButtonDown(  UINT nFlags, CPoint point) override;
  bool OnRButtonUp(    UINT nFlags, CPoint point) override;
  bool OnMouseMove(    UINT nFlags, CPoint point) override;
};
