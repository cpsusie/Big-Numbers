#include "stdafx.h"
#include "DrawTool.h"

void RectangleTool::OnLButtonDown(UINT nFlags, const CPoint &point) {
  m_container->saveDocState();
  m_p0 = m_p1 = point;
  getImage()->rectangle(CRect(m_p0,m_p1),0,true);
  repaint();
}

void RectangleTool::OnMouseMove(UINT nFlags, const CPoint &point) {
  if(nFlags & MK_LBUTTON) {
    getImage()->rectangle(CRect(m_p0,m_p1),0,true);
    m_p1 = point;
    getImage()->rectangle(CRect(m_p0,m_p1),0,true);
    repaint();
  }
}

void RectangleTool::OnLButtonUp(UINT nFlags, const CPoint &point) {
  getImage()->rectangle(CRect(m_p0,m_p1),m_container->getColor());
  m_p1 = point;
  repaint();
}

