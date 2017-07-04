#include "stdafx.h"
#include "DrawTool.h"

void FillTool::OnLButtonDown(UINT nFlags, const CPoint &point) {
  m_container->saveDocState();
  getPixRect()->fill(point,m_container->getColor());
  repaint();
}

void FillTransparentTool::OnLButtonDown(UINT nFlags, const CPoint &point) {
  m_container->saveDocState();
  getPixRect()->fillTransparent(point);
  repaint();
}


void ApproximateFillTool::OnLButtonDown(UINT nFlags, const CPoint &point) {
  m_container->saveDocState();
  getPixRect()->approximateFill(point,m_container->getColor(),m_container->getApproximateFillTolerance());
  repaint();
}

