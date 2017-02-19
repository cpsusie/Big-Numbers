#pragma once

#include "RectMaze.h"

typedef CPoint Cell;

class HardRectMaze : public RectMaze {
private:
  bool has4Walls(CDC &dc, const Cell &c);
  void findCompleteNeighbors(CDC &dc, const Cell &currentCell, const CPoint &size, CompactArray<Cell> &result);
  void removeWallBetween(CDC &dc, const Cell &c1, const Cell &c2);
  bool isHorizontalWall(CDC &dc, int x, int y);
  bool isVerticalWall(CDC &dc, int x, int y);
public:
  HardRectMaze(const CRect &rect, int doorWidth);
  void paint(CDC &dc);
};

