#include "stdafx.h"
#include "Turtle.h"
#include "Maze.h"

void MyPen::CreatePen(int style, int width, COLORREF color) {
  LOGBRUSH logBrush;
  logBrush.lbStyle = BS_SOLID;
  logBrush.lbColor = color;
  CPen::CreatePen(style, width, &logBrush);
  m_style = style;
  m_width = width;
  m_color = color;
}

void drawLine(CDC &dc, int x1, int y1, int x2, int y2, const MyPen &pen) {
  if(x1 == x2 && y1 == y2) {
    dc.SetPixel(x1,y1,pen.m_color);
  } else {
    CPen *origpen = dc.SelectObject(&(CPen&)pen);
    dc.MoveTo(x1,y1);
    dc.LineTo(x2,y2);
    dc.SelectObject(origpen);
  }
}

void drawLine(CDC &dc, const CPoint &p1, const CPoint &p2, const MyPen &pen) {
  drawLine(dc,p1.x,p1.y,p2.x,p2.y,pen);
}

Turtle::Turtle(const Maze &maze, CDialog *dialog, CDC &workDC, bool keepTrack)
: m_dc(dialog), m_workDC(workDC)
{
  m_positionStack.push(maze.getStartPoint());
  m_penWidth = maze.getDoorWidth() / 2;
  m_keepTrack = keepTrack;
  turnTo(maze.getStartDirection());
}

void Turtle::createPens() {
  m_walkPen.CreatePen( PS_SOLID|PS_GEOMETRIC|PS_ENDCAP_SQUARE, m_penWidth, getTurtleColor());
  m_erasePen.CreatePen(PS_SOLID|PS_GEOMETRIC|PS_ENDCAP_SQUARE, m_penWidth, getEraseColor());
}

void Turtle::drawLine(const CPoint &p1, const CPoint &p2, MyPen &pen) {
  ::drawLine(m_dc    ,p1,p2,pen);
  ::drawLine(m_workDC,p1,p2,pen);
}

void Turtle::walkTo(const CPoint &p) {
  CPoint &top = m_positionStack.top();
  drawLine(top,p,m_walkPen);
  m_positionStack.push(p);
}

bool Turtle::canWalkBack() const {
  return m_positionStack.getHeight() > 1;
}

void Turtle::walkBack() {
  CPoint pos = m_positionStack.pop();
  CPoint &top = m_positionStack.top();
  drawLine(pos,top,m_erasePen);
}

void Turtle::nextDirection() {
  turnTo((getCurrentDirection()+1) % getDirectionCount());
}

void Turtle::prevDirection() {
  int dir = getCurrentDirection();
  if(dir == 0) {
    turnTo(getDirectionCount()-1);
  } else {
    turnTo(dir-1);
  }
}

