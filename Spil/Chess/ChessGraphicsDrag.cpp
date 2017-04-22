#include "stdafx.h"
#include "ChessGraphicsAnimation.h"

PieceDragger::PieceDragger(ChessGraphics *graphics, const CPoint &point, const OffboardPiece *obp)
: AbstractPieceMoveAnimation(graphics, getResources().getPieceImage(obp->getKey()), graphics->getResources().getImageSize())
, m_obp(*obp)
{
  graphics->restoreBackground(m_obp);
  const CRect r = getResources().scaleRect(m_obp);
  m_offset      = point - r.TopLeft();
  saveImageRect(m_hdc, r);
  paintImage(r.TopLeft());
}

void PieceDragger::drag(const CPoint &point) {
  CPoint newTopLeft = point - m_offset;
  if(newTopLeft.x < 0) {
    newTopLeft.x = 0;
  }
  if(newTopLeft.y < 0) {
    newTopLeft.y = 0;
  }
  paintImage(newTopLeft);
}

void PieceDragger::endDrag() {
  restoreImageRect(m_hdc);
}
