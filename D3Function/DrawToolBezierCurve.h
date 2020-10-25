#pragma once

#include "DrawTool.h"

class DrawToolBezierCurve : public DrawTool {
public:
  DrawToolBezierCurve(ProfileEditor *editor)
    : DrawTool(editor)
  {
  }
  bool OnLButtonDown(  UINT nFlags, CPoint point) override;
  bool OnMouseMove(    UINT nFlags, CPoint point) override;
  bool OnLButtonUp(    UINT nFlags, CPoint point) override;
};
