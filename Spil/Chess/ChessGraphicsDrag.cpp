#include "stdafx.h"
#include "ChessGraphicsAnimation.h"

PieceDragger::PieceDragger(ChessGraphics *graphics, const CPoint &point, const OffboardPiece *obp)
: AbstractPieceMoveAnimation(graphics, getResources().getPieceImage0(obp->getKey()), graphics->getResources().getImageSize0())
, m_obp(*obp)
{
  restoreBackground(m_obp);
  m_offset = unscalePoint(point) - m_obp.TopLeft();
  saveImageRect(m_obp);
  paintImage(m_obp.TopLeft());
}

void PieceDragger::drag(const CPoint &point) {
  CPoint newTopLeft = unscalePoint(point) - m_offset;
  if(newTopLeft.x < 0) {
    newTopLeft.x = 0;
  }
  if(newTopLeft.y < 0) {
    newTopLeft.y = 0;
  }
  paintImage(newTopLeft);
}

void PieceDragger::endDrag() {
  restoreImageRect();
}
