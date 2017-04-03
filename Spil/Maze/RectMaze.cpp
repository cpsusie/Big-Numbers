#include "stdafx.h"
#include <Stack.h>
#include "RectMaze.h"
#include "RectTurtle.h"

RectMaze::RectMaze(const CRect &rect, int doorWidth) : Maze(rect,doorWidth) {
}

CPoint RectMaze::getMazeSize() const { // returns mazesize in logical units
  int doorWidth = getDoorWidth();
  return CPoint(getRect().Width()/doorWidth-1, getRect().Height()/doorWidth-1);
}

void RectMaze::drawWall(CDC &dc, int x1, int y1, int x2, int y2) {
  int doorWidth = getDoorWidth();
  if(x1 == x2) {
    if(y1 == y2)
      return;
    if(y1 > y2) std::swap(y1,y2);
    drawLine(dc,doorWidth*x1,doorWidth*y1,doorWidth*x2,doorWidth*y2,m_wallPen);
  } else {
    if(x1 > x2) std::swap(x1,x2);
    drawLine(dc,doorWidth*x1,doorWidth*y1,doorWidth*x2,doorWidth*y2,m_wallPen);
  }
}

void RectMaze::eraseWall(CDC &dc, int x1, int y1, int x2, int y2) {
  int doorWidth = getDoorWidth();
  if(x1 == x2) {
    if(y1 == y2)
      return;
    if(y1 > y2) std::swap(y1,y2);
    drawLine(dc,doorWidth*x1,doorWidth*y1+1,doorWidth*x2,doorWidth*y2-1,m_erasePen);
  } else {
    if(x1 > x2) std::swap(x1,x2);
    drawLine(dc,doorWidth*x1+1,doorWidth*y1,doorWidth*x2-1,doorWidth*y2,m_erasePen);
  }
}

CPoint RectMaze::getStartPoint() const {
  CPoint start = getMazeSize();
  int doorWidth = getDoorWidth();
  start.x -= 1;
  start.y -= 1;
  return CPoint(doorWidth*start.x+doorWidth/2,doorWidth*start.y+doorWidth/2);
}

CPoint RectMaze::getEndPoint() const {
  int doorWidth = getDoorWidth();
  return CPoint(doorWidth,doorWidth);
}

int RectMaze::getStartDirection() const {
  return WEST;
}

Turtle *RectMaze::getTurtle(CDialog *dialog, CDC &workDC, bool keepTrack) {
  return new RectTurtle(*this,dialog,workDC,keepTrack);
}
