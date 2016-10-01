#include "stdafx.h"
#include <Math.h>
#include "ChessGraphicsAnimation.h"

RotatePieceAnimation::RotatePieceAnimation(ChessGraphics *graphics, HDC dc, int position)
: m_graphics(*graphics)
, m_dc(dc)
, m_position(position)
{
}

void RotatePieceAnimation::animate() {
  const Image          *pieceImage  = m_graphics.getPieceImage(m_position);
  const Point2DP        pos         = m_graphics.getFieldPosition(m_position, true);
  const CSize           boardSize   = m_graphics.getBoardSize(true);
  const CSize           fieldSize   = m_graphics.getFieldSize(true);
  const int             fieldSize45 = (int)(fieldSize.cx*1.42); // maximal size of rotated square. (= fieldsize/cos(45) = ceil(fieldsize * sqrt(2)))
  const double          scale       = m_graphics.getResources().getScale();
  PixRect               pr45(theApp.m_device, PIXRECT_PLAINSURFACE, fieldSize45, fieldSize45);
  PixRect               boardWithoutKing(theApp.m_device, PIXRECT_PLAINSURFACE, boardSize);

  PixRect::bitBlt(&boardWithoutKing, ORIGIN,boardSize, SRCCOPY, m_dc, ORIGIN);
  HDC tmpDC = boardWithoutKing.getDC();
  m_graphics.getResources().getFieldMarkImage(CHECKEDKING)->paintImage(tmpDC, pos, scale);
  boardWithoutKing.releaseDC(tmpDC);

  const CPoint pos1((fieldSize45-fieldSize.cx)/2,(fieldSize45-fieldSize.cy)/2);
  const CPoint pos2((int)(pos.x-pos1.x),(int)(pos.y-pos1.y));
  
  tmpDC = pr45.getDC();

  for(SigmoidIterator it(0,180,30); it.hasNext();) {
    PixRect::bitBlt(tmpDC, 0,0,fieldSize45,fieldSize45, SRCCOPY, &boardWithoutKing, pos2.x,pos2.y); // paint background (checkedKing) on tmpDC
    pieceImage->paintImage(tmpDC, pos1, scale, it.next());
    BitBlt(m_dc, pos2.x,pos2.y,fieldSize45,fieldSize45, tmpDC, 0,0, SRCCOPY);
    Sleep(30);
  }

  pr45.releaseDC(tmpDC);
}
