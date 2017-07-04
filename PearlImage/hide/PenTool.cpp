#include "stdafx.h"
#include "DrawTool.h"

void PenTool::OnLButtonDown(UINT nFlags, const CPoint &point) {
  m_container->saveDocState();

  m_p0 = point;
  getPixRect()->setPixel(m_p0,m_container->getColor());
  repaint();
}

void PenTool::OnMouseMove(UINT nFlags, const CPoint &point) {
  if(nFlags & MK_LBUTTON) {
    getPixRect()->line(m_p0,point,m_container->getColor());
    m_p0 = point;
    repaint();
  }
}

