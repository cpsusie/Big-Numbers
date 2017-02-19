#pragma once

#include "Turtle.h"
#include "RectMaze.h"

#define NORTH 0
#define EAST  1
#define SOUTH 2
#define WEST  3

class RectTurtle : public Turtle {
private:
  int m_doorWidth;
  CPoint m_endPos;
  CPoint findNextPos() const;
public:
  RectTurtle(const RectMaze &maze, CDialog *dialog, CDC &workDC, bool keepTrack);
  void firstDirection();
  void lastDirection();
  int  getDirectionCount() const {
    return 4;
  }
  bool walkStep();
  bool isOppositeDirection(int dir) const;
  bool lookingAtDoor() const;
  bool finished() const;
};
