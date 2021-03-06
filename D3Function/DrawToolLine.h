#pragma once

#include "DrawTool.h"

class DrawToolLine : public DrawTool {
private:
  Point2D *m_p0,*m_p1;
public:
  DrawToolLine(ProfileEditor *editor)
    : DrawTool(editor)
    , m_p0(nullptr)
    , m_p1(nullptr)
  {
  }
  bool OnLButtonDown(  UINT nFlags, CPoint point) override;
  bool OnMouseMove(    UINT nFlags, CPoint point) override;
  bool OnLButtonUp(    UINT nFlags, CPoint point) override;
};
