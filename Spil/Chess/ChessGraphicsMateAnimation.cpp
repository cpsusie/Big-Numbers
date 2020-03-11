#include "stdafx.h"
#include "ChessGraphicsAnimation.h"

MateAnimation::MateAnimation(ChessGraphics *graphics, int position)
: ChessAnimation(graphics)
, m_position(position)
{
}

void MateAnimation::animate() {
  const Image          *pieceImage    = m_graphics.getPieceImage(m_position);
  const Point2DP        pos           = m_graphics.getFieldPosition(m_position, false);
  const Size2DS         fieldSize     = m_graphics.getFieldSize(false);
  const Size2DS         field45Size   = PixRect::getRotatedSize(fieldSize,45); // maximal size of rotated square. (= fieldsize/cos(45) = ceil(fieldsize * sqrt(2)))
  const Size2DS         margin        = (field45Size-fieldSize)/2;
  const Point2DP        pos2          = pos - margin;
  PixRect               background(theApp.m_device, PIXRECT_PLAINSURFACE, field45Size);
  PixRect               helper(    theApp.m_device, PIXRECT_PLAINSURFACE, field45Size);

  background.rop(ORIGIN, field45Size, SRCCOPY, m_graphics.m_bufferPr, pos2);
  getResources().getFieldMarkImage(CHECKEDKING)->paintImage(background, Point2DP(margin.cx,margin.cy));

  for(SigmoidIterator it(0,180,30); it.hasNext();) {
    const double degree = it.next();
    const Size2DS rotatedSize = PixRect::getRotatedSize(fieldSize, degree), pos((field45Size - rotatedSize) / 2);
    helper.rop(ORIGIN, field45Size, SRCCOPY, &background, ORIGIN);
    pieceImage->paintImage(helper, Point2DP(pos.cx,pos.cy), 1, degree);
    m_graphics.m_bufferPr->rop(pos2, field45Size, SRCCOPY, &helper, ORIGIN);
    m_graphics.render();
    Sleep(40);
  }
}
