#pragma once

#include <MatrixTemplate.h>
#include "MazeCell.h"

class MazeCellMatrix : public MatrixTemplate<MazeCell> {
private:
  const int m_wallCount;
  void initWallArrays();
public:
  MazeCellMatrix(int wallCount, const MatrixDimension &dim)
  : m_wallCount(wallCount)
  , MatrixTemplate<MazeCell>(dim)
  {
    initWallArrays();
  }

  MazeCell &findRightBottomCompleteCell();

  inline void markCell(CDC &dc, int r, int c) {
    (*this)(r, c).mark(dc);
  }

  void unVisitAll();
};
