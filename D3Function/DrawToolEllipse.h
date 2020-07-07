#pragma once

#include "DrawTool.h"

class DrawToolEllipse : public DrawTool {
private:
  CPoint m_p0,m_p1;
public:
  DrawToolEllipse(ProfileEditor *editor)
    : DrawTool(editor)
  {
  }
  bool OnLButtonDown(  UINT nFlags, CPoint point);
  bool OnMouseMove(    UINT nFlags, CPoint point);
  bool OnLButtonUp(    UINT nFlags, CPoint point);
};
