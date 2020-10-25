#pragma once

#include "DrawTool.h"

class DrawToolPolygon : public DrawTool {
private:
  ProfilePolygon2D *m_pp;
public:
  DrawToolPolygon(ProfileEditor *editor)
    : DrawTool(editor)
    , m_pp(NULL)
  {
  }
  bool OnLButtonDown(  UINT nFlags, CPoint point) override;
  bool OnLButtonDblClk(UINT nFlags, CPoint point) override;
  bool OnMouseMove(    UINT nFlags, CPoint point) override;
  bool OnLButtonUp(    UINT nFlags, CPoint point) override;
};
