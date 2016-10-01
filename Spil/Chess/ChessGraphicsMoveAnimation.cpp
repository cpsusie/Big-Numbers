#include "stdafx.h"
#include <Math.h>
#include "ChessGraphicsAnimation.h"

PixRect *MovePieceAnimation::background          = NULL;
int      MovePieceAnimation::animationsAllocated = 0;

MovePieceAnimation::MovePieceAnimation(ChessGraphics *graphics, HDC dc, const int from, const int to, int steps) 
: m_resources(graphics->getResources())
, m_dc(dc)
, m_from(graphics->getFieldPosition(from, true))
, m_to(  graphics->getFieldPosition(to  , true))
, m_pieceImage(graphics->getPieceImage(from))
, m_size(graphics->getFieldSize(true))
, m_scale(graphics->getResources().getScale())
, m_it(0,1, steps ? steps : (int)(sqrt(Game::getKingDistance(from,to))*28))
{
  const CSize boardSize = m_resources.getBoardSize();
  if(animationsAllocated++ == 0) {
    background = new PixRect(theApp.m_device, PIXRECT_PLAINSURFACE, boardSize);
  };

  m_oldBackground = new PixRect(theApp.m_device, PIXRECT_PLAINSURFACE, m_size);
  m_pos           = m_from;
  initPaintedPoint();
  initBackgroundPoint();

  graphics->restoreBackground(graphics->getFieldPosition(from, false), graphics->getFieldSize(false), true, dc);
  PixRect::bitBlt(background, ORIGIN, boardSize, SRCCOPY, dc, ORIGIN);
  m_restoredPoint.x = m_restoredPoint.y = -1;

  paintImage(m_pos);
}

MovePieceAnimation::~MovePieceAnimation() {
  delete m_oldBackground;
  if(--animationsAllocated == 0) {
    delete background;
  }
}

void MovePieceAnimation::paint() {
  paintImage(m_pos);
}

void MovePieceAnimation::unpaint() {
  restoreBackground();
}

void MovePieceAnimation::flush() {
  if(m_restoredPoint.x >= 0) {
    PixRect::bitBlt(m_dc, m_restoredPoint, m_size, SRCCOPY, background, m_restoredPoint);
  }
  PixRect::bitBlt(m_dc, m_paintedPoint, m_size, SRCCOPY, background, m_paintedPoint);
}

bool MovePieceAnimation::step() {
  if(!m_it.hasNext()) {
    return false;
  }
  const double d = m_it.next();
  m_pos = (1-d) * m_from + d * m_to;
  return true;
}

void MovePieceAnimation::paintImage(const CPoint &p) {
  restoreBackground();
  saveBackground(p);
  m_pieceImage->paintImage(*background, p, m_scale);
  m_paintedPoint = p;
}

void MovePieceAnimation::initPaintedPoint() {
  m_paintedPoint.x = m_paintedPoint.y = -1;
}

void MovePieceAnimation::saveBackground(const CPoint &p) {
  if(p.x >= 0 && p.y >= 0) {
    m_oldBackground->rop(ORIGIN, m_size, SRCCOPY, background, p);
    m_backgroundPoint = p;
  }
}

void MovePieceAnimation::restoreBackground() {
  if(m_backgroundPoint.x >= 0) {
    background->rop(m_backgroundPoint, m_size, SRCCOPY, m_oldBackground, ORIGIN);
    m_restoredPoint = m_backgroundPoint;
    initBackgroundPoint();
  }
}

void MovePieceAnimation::initBackgroundPoint() {
  m_backgroundPoint.x = m_backgroundPoint.y = -1;
}
