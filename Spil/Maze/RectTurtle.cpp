#include "stdafx.h"
#include "RectTurtle.h"

RectTurtle::RectTurtle(const RectMaze &maze, CDialog *dialog, CDC &workDC, bool keepTrack)
:Turtle(maze,dialog,workDC, keepTrack)
{
  m_doorWidth = maze.getDoorWidth();
  m_endPos    = maze.getEndPoint();
}

void RectTurtle::firstDirection() {
  prevDirection();
}

void RectTurtle::lastDirection() {
  nextDirection();
}

bool RectTurtle::isOppositeDirection(int dir) const {
  return dir == (getCurrentDirection()+2) % 4;
}

static CPoint stepTable[] = {
  CPoint( 0,-1),  // NORTH
  CPoint( 1, 0),  // EAST
  CPoint( 0, 1),  // SOUTH
  CPoint(-1, 0)   // WEST
};

CPoint RectTurtle::findNextPos() const {
  const CPoint &pos  = getCurrentPosition();
  const CPoint &step = stepTable[getCurrentDirection()];
  return CPoint(pos.x + step.x * m_doorWidth, pos.y + step.y * m_doorWidth);
}

bool RectTurtle::lookingAtDoor() const {
  const CPoint &pos     = getCurrentPosition();
  const CPoint  nextPos = findNextPos();
  return m_workDC.GetPixel((pos.x+nextPos.x)/2,(pos.y+nextPos.y)/2) != BLACK;
}

bool RectTurtle::walkStep() {
  walkTo(findNextPos());
  return true;
}

bool RectTurtle::finished() const {
  return getCurrentPosition().x <= m_endPos.x;
}
