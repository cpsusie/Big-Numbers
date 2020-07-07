#pragma once

#include "DrawTool.h"

class DrawToolPolygon : public DrawTool {
private:
  ProfilePolygon *m_pp;
public:
  DrawToolPolygon(ProfileEditor *editor)
    : DrawTool(editor)
    , m_pp(NULL)
  {
  }
  bool OnLButtonDown(  UINT nFlags, CPoint point);
  bool OnLButtonDblClk(UINT nFlags, CPoint point);
  bool OnMouseMove(    UINT nFlags, CPoint point);
  bool OnLButtonUp(    UINT nFlags, CPoint point);
};
