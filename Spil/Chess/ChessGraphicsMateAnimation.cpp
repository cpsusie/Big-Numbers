#include "stdafx.h"
#include "ChessGraphicsAnimation.h"

MateAnimation::MateAnimation(ChessGraphics *graphics, int position)
: ChessAnimation(graphics)
, m_position(position)
{
}

void MateAnimation::animate() {
  const Image   *pieceImage    = m_graphics.getPieceImage0(m_position);
  const CPoint   pos0          = m_graphics.getFieldPosition0(m_position);
  const CSize    fieldSize0    = FIELDSIZE0;
  const CSize    fieldSize45   = PixRect::getRotatedSize(fieldSize0,45); // maximal size of rotated square. (= fieldsize/cos(45) = ceil(fieldsize * sqrt(2)))
  const CSize    margin        = (fieldSize45-fieldSize0)/2;
  const CPoint   pos2          = pos0 - margin;
  PixRect        background(theApp.m_device, PIXRECT_PLAINSURFACE, fieldSize45);
  PixRect        helper(    theApp.m_device, PIXRECT_PLAINSURFACE, fieldSize45);

  background.rop(ORIGIN, fieldSize45, SRCCOPY, m_graphics.m_bufferPr, pos2);
  getResources().getFieldMarkImage0(CHECKEDKING)->paintImage(background, margin);

  for(SigmoidIterator it(0,180,30); it.hasNext();) {
    const double degree      = it.next();
    const CSize  rotatedSize = PixRect::getRotatedSize(fieldSize0, degree), pos((fieldSize45 - rotatedSize) / 2);
    helper.rop(ORIGIN, fieldSize45, SRCCOPY, &background, ORIGIN);
    pieceImage->paintImage(helper, pos, 1, degree);
    m_graphics.m_bufferPr->rop(pos2, fieldSize45, SRCCOPY, &helper, ORIGIN);
    m_graphics.render();
    Sleep(40);
  }
}
