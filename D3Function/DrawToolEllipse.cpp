#include "stdafx.h"
#include "DrawToolEllipse.h"

bool DrawToolEllipse::OnLButtonDown(UINT nFlags, CPoint point) {
  m_p0 = m_p1 = point;
  repaintScreen();
  return true;
}

bool DrawToolEllipse::OnMouseMove(UINT nFlags, CPoint point) {
  if(nFlags & MK_LBUTTON) {
    m_p1 = point;
    repaintScreen();
    return true;
  }
  return false;
}

bool DrawToolEllipse::OnLButtonUp(UINT nFlags, CPoint point) {
  repaintScreen();
  return true;
}
