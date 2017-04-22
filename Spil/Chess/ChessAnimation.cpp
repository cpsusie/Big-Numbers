#include "stdafx.h"
#include "ChessGraphicsAnimation.h"

static bool needResize(const CSize &oldSize, const CSize &newSize) {
  return (newSize.cx > oldSize.cx) || (newSize.cy > oldSize.cy);
}

void ChessAnimation::saveImageRect(HDC src, const CRect &r) {
  if((r.Width() == 0) || (r.Height() == 0)) return;
  if((m_savedPr == NULL) || needResize(m_savedPr->getSize(), r.Size())) {
    if(m_savedPr) {
      delete m_savedPr;
      m_savedPr = NULL;
    }
    m_savedPr = new PixRect(theApp.m_device, PIXRECT_PLAINSURFACE, r.Size());
  }
  PixRect::bitBlt(m_savedPr, ORIGIN, r.Size(), SRCCOPY, src, r.TopLeft());
  m_savedRect = r;
}

void ChessAnimation::restoreImageRect(HDC dst) {
  if((m_savedRect.Width() == 0) || (m_savedRect.Height() == 0)) return;
  PixRect::bitBlt(dst, m_savedRect, SRCCOPY, m_savedPr, ORIGIN);
}

void AbstractPieceMoveAnimation::paintImage(const CPoint &p) {
  restoreImageRect(m_hdc);
  PixRect::bitBlt(m_helper, ORIGIN, m_helper->getSize(), SRCCOPY, m_hdc, p);
  m_pieceImage->paintImage(*m_helper, ORIGIN, getResources().getAvgScale());
  saveImageRect(m_hdc, CRect(p, m_helper->getSize()));
  PixRect::bitBlt(m_hdc, p, m_helper->getSize(), SRCCOPY, m_helper, ORIGIN);
}
