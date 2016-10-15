#include "stdafx.h"
#include "ChessGraphicsAnimation.h"

PieceDragger::PieceDragger(ChessGraphics *graphics, const CPoint &point, const OffboardPiece *obp, HDC dc)
: m_resources(graphics->getResources())
, m_obp(*obp)
, m_pieceImage(m_resources.getPieceImage(obp->getKey()))
{
  const CSize boardSize = m_resources.getBoardSize();
  m_size           = m_resources.scaleSize(m_pieceImage->getSize());
  m_background     = new PixRect(theApp.m_device, PIXRECT_PLAINSURFACE, boardSize);
  m_oldBackground  = new PixRect(theApp.m_device, PIXRECT_PLAINSURFACE, m_size);
  m_helper         = new PixRect(theApp.m_device, PIXRECT_PLAINSURFACE, m_size);

  m_lastDragPoint = point;
  m_lastTopLeft   = m_resources.scalePoint(obp->TopLeft());
  initPaintedPoint();
  graphics->restoreBackground(m_obp, true, dc);
  PixRect::bitBlt(m_background, ORIGIN, boardSize, SRCCOPY, dc, ORIGIN);
  saveDragPosition(m_lastTopLeft);
  paintImage(m_lastTopLeft, dc);
}

PieceDragger::~PieceDragger() {
  delete m_helper;
  delete m_oldBackground;
  delete m_background;
}

void PieceDragger::drag(const CPoint &point, HDC dc) {
  CSize dp = point - m_lastDragPoint;

  const CPoint newTopLeft = m_lastTopLeft + dp;
  if(newTopLeft.x < 0) {
    dp.cx -= newTopLeft.x;
  }
  if(newTopLeft.y < 0) {
    dp.cy -= newTopLeft.y;
  }

  restoreDragPosition();

  m_lastTopLeft   += dp;
  m_lastDragPoint += dp;

  saveDragPosition(m_lastTopLeft);
  paintImage(m_lastTopLeft, dc);
}

void PieceDragger::paintImage(const CPoint &p, HDC dc) {
  m_helper->rop(ORIGIN,m_size, SRCCOPY, m_background, p);
  m_pieceImage->paintImage(*m_helper, ORIGIN, m_resources.getScale());
  unpaintImage(dc);
  PixRect::bitBlt(dc, p, m_size, SRCCOPY, m_helper, ORIGIN);
  m_lastPaintedPoint = p;
}

void PieceDragger::unpaintImage(HDC dc) {
  if(m_lastPaintedPoint.x >= 0) {
    PixRect::bitBlt(dc, m_lastPaintedPoint, m_size, SRCCOPY, m_background, m_lastPaintedPoint);
    initPaintedPoint();
  }
}

void PieceDragger::initPaintedPoint() {
  m_lastPaintedPoint.x = m_lastPaintedPoint.y = -1;
}

void PieceDragger::endDrag(HDC dc) {
  restoreDragPosition(dc);
}

void PieceDragger::saveDragPosition(const CPoint &p) {
  if(p.x >= 0 && p.y >= 0) {
    m_oldBackground->rop(ORIGIN, m_size, SRCCOPY, m_background, p);
    m_savedDragPoint = p;
  }
}

void PieceDragger::restoreDragPosition(HDC dc) {
  if(dc) {
    PixRect::bitBlt(dc, m_savedDragPoint, m_size, SRCCOPY, m_oldBackground, ORIGIN);
  } else {
    m_background->rop(m_savedDragPoint, m_size, SRCCOPY, m_oldBackground, ORIGIN);
  }
}


