#pragma once

#include "DrawTool.h"

class DrawToolBezierCurve : public DrawTool {
public:
  DrawToolBezierCurve(ProfileEditor *editor)
    : DrawTool(editor)
  {
  }
  bool OnLButtonDown(  UINT nFlags, CPoint point);
  bool OnMouseMove(    UINT nFlags, CPoint point);
  bool OnLButtonUp(    UINT nFlags, CPoint point);
};
