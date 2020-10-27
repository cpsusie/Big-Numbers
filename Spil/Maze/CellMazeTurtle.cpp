#include "StdAfx.h"
#include "CellMazeTurtle.h"

void CellMazeTurtle::createPens() {
  m_walkPen.CreatePen( PS_SOLID|PS_GEOMETRIC|PS_ENDCAP_ROUND, m_penWidth, getTurtleColor());
  m_erasePen.CreatePen(PS_SOLID|PS_GEOMETRIC|PS_ENDCAP_ROUND, m_penWidth, getEraseColor());
}

bool CellMazeTurtle::walkStep() {
  MazeCell *newCell = m_cellStack.top()->getNeighbor(getCurrentDirection());
  if(newCell->isVisited()) {
    return false;
  }
  newCell->setVisited(true);
  walkTo(newCell->getCenter());
  m_cellStack.push(newCell);
  return true;
}

void CellMazeTurtle::walkBack() {
  Turtle::walkBack();
  m_cellStack.pop();

  if(canWalkBack()) {
    const CPoint p = getCurrentPosition();
    Turtle::walkBack();
    walkTo(p);
  }
}

bool CellMazeTurtle::lookingAtDoor() const {
  const MazeCell *cell = m_cellStack.top();
  const int       dir  = getCurrentDirection();
  return !cell->getWall(dir)->isVisible() && (cell->getNeighbor(dir) != nullptr);
}
