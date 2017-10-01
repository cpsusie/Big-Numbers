#include "stdafx.h"
#include <Math.h>
#include "ChessGraphicsAnimation.h"

MoveSinglePieceAnimation::MoveSinglePieceAnimation(ChessGraphics *graphics, const int from, const int to, int steps)
: AbstractPieceMoveAnimation(graphics, graphics->getPieceImage(from), graphics->getFieldSize(false))
, m_from(graphics->getFieldPosition(from, false))
, m_to(  graphics->getFieldPosition(to  , false))
, m_it(0,1, steps ? steps : (int)(sqrt(Game::getWalkDistance(from,to))*28))
{

  m_pos = m_from;
  const CRect startRect(m_from, m_fieldSize);
  restoreBackground(startRect);
  saveImageRect(startRect);
  paint();
}

void MoveSinglePieceAnimation::paint() {
  paintImage(m_pos);
}

void MoveSinglePieceAnimation::unpaint() {
  restoreImageRect();
}

bool MoveSinglePieceAnimation::step() {
  if(!m_it.hasNext()) {
    return false;
  }
  const double d = m_it.next();
  m_pos = (1-d) * m_from + d * m_to;
  return true;
}

void MoveAnimation::addMovePiece(const int from, const int to) {
  MoveSinglePieceAnimation *mspa = new MoveSinglePieceAnimation(m_graphics, from, to, m_steps); TRACE_NEW(mspa);
  add(mspa);
  if(size() == 1) {
    m_steps = (*this)[0]->getSteps();
  }
}

void MoveAnimation::animate() {
  const int sleepTime = 15;
  const int n = (int)size();
  beginPaint();
  for(bool cont = true; cont;) {
    for(int i = n; i--;) (*this)[i]->paint();
    endPaint();
    if(sleepTime) Sleep(sleepTime);
    cont = false;
    beginPaint();
    for(int i = 0; i < n; i++) {
      (*this)[i]->unpaint();
      cont |= (*this)[i]->step();
    }
  }
  endPaint();
}
