#pragma once

#include "CellMaze.h"

class TriangularMaze : public CellMaze {
private:
  static const double s_SIN60, s_COS60;
  double         m_DWSIN60;
  double         m_DWCOS60;
  int            m_leftMargin, m_topMargin;
  MazeWallMatrix m_horizontalWall, m_verticalWall;

  void   initialize();
  void   reduceWalls();
  CPoint wallPoint(int r, int c) const;

public:
  TriangularMaze(const CRect &rectangle, int doorWidth);

  int getStartDirection() const {
    return 0;
  }

  void paint(CDC &dc);
  Turtle *getTurtle(CDialog *dialog, CDC &workDC, bool keepTrack);
  void removeWalls();
};
