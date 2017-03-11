#include "stdafx.h"
#include "DrawTool.h"

void LineTool::OnLButtonDown(UINT nFlags, const CPoint &point) {
  m_p0 = m_p1 = point;
  m_container->saveDocState();
  getImage()->line(m_p0,m_p1,0,true);
  repaint();
}

void LineTool::OnMouseMove(UINT nFlags, const CPoint &point) {
  if(nFlags & MK_LBUTTON) {
    getImage()->line(m_p0,m_p1,0,true);
    m_p1 = point;
    getImage()->line(m_p0,m_p1,0,true);
    repaint();
  }
}

void LineTool::OnLButtonUp(UINT nFlags, const CPoint &point) {
  getImage()->line(m_p0,m_p1,m_container->getColor());
  m_p0 = point;
  repaint();
}

