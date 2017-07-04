#include "stdafx.h"
#include "DrawTool.h"

void LineTool::OnLButtonDown(UINT nFlags, const CPoint &point) {
  m_p0 = m_p1 = point;
  m_container->saveDocState();
  getPixRect()->line(m_p0,m_p1,0,true);
  m_container->repaint();
}

void LineTool::OnMouseMove(UINT nFlags, const CPoint &point) {
  if(nFlags & MK_LBUTTON) {
    getPixRect()->line(m_p0,m_p1,0,true);
    m_p1 = point;
    getPixRect()->line(m_p0,m_p1,0,true);
    m_container->repaint();
  }
}

void LineTool::OnLButtonUp(UINT nFlags, const CPoint &point) {
  getPixRect()->line(m_p0,m_p1,m_container->getColor());
  m_p0 = point;
  m_container->repaint();
}

