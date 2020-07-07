#pragma once

#include "DrawTool.h"

class DrawToolLine : public DrawTool {
private:
  Point2D *m_p0,*m_p1;
public:
  DrawToolLine(ProfileEditor *editor)
    : DrawTool(editor)
    , m_p0(NULL)
    , m_p1(NULL)
  {
  }
  bool OnLButtonDown(  UINT nFlags, CPoint point);
  bool OnMouseMove(    UINT nFlags, CPoint point);
  bool OnLButtonUp(    UINT nFlags, CPoint point);
};
