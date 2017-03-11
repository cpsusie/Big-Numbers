#include "stdafx.h"
#include "DrawTool.h"

CRect EraseTool::makeRect(const CPoint &p) {
  int size = 8 / m_container->getCurrentZoomFactor();
  return CRect(p.x,p.y,p.x+size,p.y+size);
}

void EraseTool::OnLButtonDown(UINT nFlags, const CPoint &point) {
  m_container->saveDocState();

  m_lastPoint = point;
  getImage()->fillRect(makeRect(point),WHITE);
  repaint();
}

void EraseTool::apply(const CPoint &p) {
  getImage()->fillRect(makeRect(p),WHITE);
}

void EraseTool::OnMouseMove(UINT nFlags, const CPoint &point) {
  if(nFlags & MK_LBUTTON) {
    applyToLine(m_lastPoint,point,*this);
    m_lastPoint = point;
    repaint();
  }
}

void EraseTool::OnLButtonUp(UINT nFlags, const CPoint &point) {
  repaint();
}
