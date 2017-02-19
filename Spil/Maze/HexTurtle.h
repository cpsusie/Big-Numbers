#pragma once

#include <Stack.h>
#include "CellMazeTurtle.h"
#include "HexMaze.h"

class HexTurtle : public CellMazeTurtle {
public:
  HexTurtle(const HexMaze &maze, CDialog *dialog, CDC &workDC, bool keepTrack)
    : CellMazeTurtle(maze, dialog, workDC, keepTrack)
  {
  }
  void firstDirection();
  void lastDirection();
  int getDirectionCount() const {
    return 6;
  }
  bool isOppositeDirection(int dir) const {
    return dir == (getCurrentDirection()+3) % 6;
  }
};

