#include "stdafx.h"
#include "DrawTool.h"

void EllipseTool::OnLButtonDown(UINT nFlags, const CPoint &point) {
  m_container->saveDocState();
  m_p0 = m_p1 = point;
  getImage()->ellipse(CRect(m_p0,m_p1),0,true);
  repaint();
}

void EllipseTool::OnMouseMove(UINT nFlags, const CPoint &point) {
  if(nFlags & MK_LBUTTON) {
    getImage()->ellipse(CRect(m_p0,m_p1),0,true);
    m_p1 = point;
    getImage()->ellipse(CRect(m_p0,m_p1),0,true);
    repaint();
  }
}

void EllipseTool::OnLButtonUp(UINT nFlags, const CPoint &point) {
  getImage()->ellipse(CRect(m_p0,m_p1),m_container->getColor());
  repaint();
//  m_pixRect->fillEllipse(CRect(m_p0,m_p1),m_container->getColor());
}

