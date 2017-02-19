#pragma once

#include "CellMaze.h"

class HexMaze : public CellMaze {
private:
  double         m_DWCOS30;
  int            m_leftMargin, m_topMargin;
  MazeWallMatrix m_horizontalWall,m_verticalWall1,m_verticalWall2;
  void initialize();
  void reduceWalls();
  CPoint wallPoint(int r, int c) const;
public:
  HexMaze(const CRect &r, int doorWidth);

  int getStartDirection() const {
    return 5;
  }

  void paint(CDC &dc);

  Turtle *getTurtle(CDialog *dialog, CDC &workDC, bool keepTrack);

  inline void markCell(CDC &dc, int r, int c) {
    m_cell(r,c).mark(dc);
  }
};
