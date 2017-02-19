#pragma once

#include "Maze.h"

class RectMaze : public Maze {
public:
  RectMaze(const CRect &rect, int doorWidth);
  void drawWall(CDC &dc, int x1, int y1, int x2, int y2);
  void eraseWall(CDC &dc, int x1, int y1, int x2, int y2);
  CPoint getMazeSize() const; // returns mazesize in logical units
  CPoint getStartPoint() const;
  CPoint getEndPoint() const;
  int getStartDirection() const;
  Turtle *getTurtle(CDialog *dialog, CDC &workDC, bool keepTrack);
};
