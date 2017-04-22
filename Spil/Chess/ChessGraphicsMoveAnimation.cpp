#include "stdafx.h"
#include <Math.h>
#include "ChessGraphicsAnimation.h"

MoveSinglePieceAnimation::MoveSinglePieceAnimation(ChessGraphics *graphics, const int from, const int to, int steps)
: AbstractPieceMoveAnimation(graphics, graphics->getPieceImage(from), graphics->getFieldSize(true))
, m_from(graphics->getFieldPosition(from, true))
, m_to(  graphics->getFieldPosition(to  , true))
, m_it(0,1, steps ? steps : (int)(sqrt(Game::getKingDistance(from,to))*28))
{

  m_pos = m_from;
  const CRect startRect1(m_graphics.getFieldPosition(from, false), m_graphics.getFieldSize(false));
  m_graphics.restoreBackground(startRect1);
  paintImage(m_pos);
}

void MoveSinglePieceAnimation::paint() {
  paintImage(m_pos);
}

void MoveSinglePieceAnimation::unpaint() {
  restoreImageRect(m_hdc);
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
  add(new MoveSinglePieceAnimation(m_graphics, from, to, m_steps));
  if(size() == 1) {
    m_steps = (*this)[0]->getSteps();
  }
}

void MoveAnimation::animate() {
  const int sleepTime = (size() == 1) ? 15 : 10;
  const int n = (int)size();
  for(bool cont = true; cont;) {
    for(int i = n; i--;) (*this)[i]->paint();
    if(sleepTime) Sleep(sleepTime);
    cont = false;
    for(int i = 0; i < n; i++) {
      (*this)[i]->unpaint();
      cont |= (*this)[i]->step();
    }
  }
}
