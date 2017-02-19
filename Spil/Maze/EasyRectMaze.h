#pragma once

#include "RectMaze.h"

class EasyRectMaze : public RectMaze {
private:
  void frame(CDC &dc, const CRect &rect);
  CRect makeWall(CDC &dc, const CRect &rect);
  void makePartition(CDC &dc, const CRect &rect);
public:
  EasyRectMaze(const CRect &rect, int doorWidth);
  void paint(CDC &dc);
};

