#include "stdafx.h"
#include "ChessGraphicsAnimation.h"

void AbstractPieceMoveAnimation::paintImage(const CPoint &p) {
  m_graphics.beginPaint();
  restoreImageRect();
  m_helper->rop(ORIGIN, m_helper->getSize(), SRCCOPY, m_graphics.m_bufferPr, p);
  m_pieceImage->paintImage(*m_helper, ORIGIN);
  saveImageRect(CRect(p, m_helper->getSize()));
  m_graphics.m_bufferPr->rop(p, m_helper->getSize(), SRCCOPY, m_helper, ORIGIN);
  m_graphics.endPaint();
}
