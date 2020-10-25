#pragma once

#include "DrawTool.h"

class DrawToolRectangle : public DrawTool {
private:
  Point2D *m_ul,*m_ur,*m_ll,*m_lr;
public:
  DrawToolRectangle(ProfileEditor *editor)
    : DrawTool(editor)
  {
  }
  bool OnLButtonDown(  UINT nFlags, CPoint point) override;
  bool OnMouseMove(    UINT nFlags, CPoint point) override;
  bool OnLButtonUp(    UINT nFlags, CPoint point) override;
};
