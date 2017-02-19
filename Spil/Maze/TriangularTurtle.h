#pragma once

#include <Stack.h>
#include "CellMazeTurtle.h"
#include "TriangularMaze.h"

class TriangularTurtle : public CellMazeTurtle {
public:
  TriangularTurtle(TriangularMaze &maze, CDialog *dialog, CDC &workDC, bool keepTrack)
    : CellMazeTurtle(maze, dialog, workDC, keepTrack)
  {
  }
  void firstDirection();
  void lastDirection();
  int getDirectionCount() const {
    return 3;
  }
  bool isOppositeDirection(int dir) const {
    return false;
  }
};
