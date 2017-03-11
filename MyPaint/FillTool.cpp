#include "stdafx.h"
#include "DrawTool.h"

void FillTool::OnLButtonDown(UINT nFlags, const CPoint &point) {
  m_container->saveDocState();
  getImage()->fill(point,m_container->getColor());
  repaint();
}

void FillTransparentTool::OnLButtonDown(UINT nFlags, const CPoint &point) {
  m_container->saveDocState();
  getImage()->fillTransparent(point);
  repaint();
}


void ApproximateFillTool::OnLButtonDown(UINT nFlags, const CPoint &point) {
  m_container->saveDocState();
  getImage()->approximateFill(point,m_container->getColor(),m_container->getApproximateFillTolerance());
  repaint();
}

