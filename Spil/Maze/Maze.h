#pragma once

#include "Turtle.h"

class Maze {
private:
  CRect m_rect;
  int   m_doorWidth;
protected:
  static void fillRectangle(CDC &dc, const CRect &r, COLORREF color);
protected:
  MyPen m_wallPen,m_erasePen;
public:
  Maze(const CRect &rect, int doorWidth);
  virtual void createPens();
  virtual ~Maze() {}
  const CRect &getRect() const {
    return m_rect;
  }
  int getDoorWidth() const {
    return m_doorWidth;
  }
  virtual CPoint  getStartPoint()     const = 0;
  virtual int     getStartDirection() const = 0;
  virtual Turtle *getTurtle(CDialog *dialog, CDC &workDC, bool keepTrack) = 0;
  virtual void    paint(CDC &dc)            = 0;
  virtual void    unVisitAll() {
  }
};

