#pragma once

#include <Stack.h>
#include <MFCUtil/ColorSpace.h>

class MyPen : public CPen {
public:
  int      m_style;
  int      m_width;
  COLORREF m_color;
  void CreatePen(int style, int width, COLORREF color);
};

void drawLine(CDC &dc, int x1, int y1, int x2, int y2, const MyPen &pen);
void drawLine(CDC &dc, const CPoint &p1, const CPoint &p2, const MyPen &pen);

class Maze;

class Turtle {
private:
  Stack<CPoint> m_positionStack;
  int           m_dir;
  void drawLine(const CPoint &p1, const CPoint &p2, MyPen &pen);
protected:
  CClientDC m_dc;
  CDC      &m_workDC;
  bool      m_keepTrack;
  int       m_penWidth;
  MyPen     m_walkPen,m_erasePen;

  Turtle(const Maze &maze, CDialog *dialog, CDC &workDC, bool keepTrack);
  bool canWalkBack() const;
  COLORREF getTurtleColor() const { return GREEN; }
  COLORREF getEraseColor()  const { return m_keepTrack?RED:WHITE; }
public:
  virtual void createPens();
  virtual void firstDirection() = 0;
  virtual void lastDirection()  = 0;
  virtual void nextDirection();
  virtual void prevDirection();
  virtual int  getDirectionCount() const = 0;
  virtual bool walkStep() = 0;
  virtual void walkTo(const CPoint &p);
  virtual void walkBack();
  virtual int  getCurrentDirection() const {
    return m_dir;
  }
  virtual void turnTo(int dir) {
    m_dir = dir;
  }
  virtual bool isOppositeDirection(int dir) const = 0;
  const CPoint &getCurrentPosition() const {
    return m_positionStack.top();
  }
  virtual bool lookingAtDoor() const = 0;
  virtual bool finished() const = 0;
};
