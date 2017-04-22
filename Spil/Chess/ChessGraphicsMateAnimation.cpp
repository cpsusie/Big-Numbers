#include "stdafx.h"
#include <Math.h>
#include "ChessGraphicsAnimation.h"

MateAnimation::MateAnimation(ChessGraphics *graphics, int position)
: ChessAnimation(graphics)
, m_position(position)
{
}

void MateAnimation::animate() {
  const Image          *pieceImage  = m_graphics.getPieceImage(m_position);
  const Point2DP        pos         = m_graphics.getFieldPosition(m_position, false);
  const Size2DS         fieldSize   = m_graphics.getFieldSize(false);
  const Size2DS         fieldSize45 = fieldSize * 1.42;  // maximal size of rotated square. (= fieldsize/cos(45) = ceil(fieldsize * sqrt(2)))
  const Size2DS         margin      = (fieldSize45-fieldSize)/2;
  const Point2DP        pos1        = ORIGIN + margin;
  const Point2DP        pos2        = pos    - margin;
  PixRect               background(theApp.m_device, PIXRECT_PLAINSURFACE, fieldSize45);
  PixRect               helper(    theApp.m_device, PIXRECT_PLAINSURFACE, fieldSize45);

  background.rop(ORIGIN, fieldSize45, SRCCOPY, m_graphics.m_bufferPr, pos2);
  getResources().getFieldMarkImage(CHECKEDKING)->paintImage(background, ORIGIN+margin);

  for(SigmoidIterator it(0,180,30); it.hasNext();) {
    helper.rop(ORIGIN, fieldSize45, SRCCOPY, &background, ORIGIN);
    pieceImage->paintImage(helper, pos1, 1, it.next());
    m_graphics.m_bufferPr->rop(pos2, fieldSize45, SRCCOPY, &helper, ORIGIN);
    m_graphics.render();
    Sleep(40);
  }
}
