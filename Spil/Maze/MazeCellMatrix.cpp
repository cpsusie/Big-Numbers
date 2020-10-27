#include "StdAfx.h"
#include "MazeCellMatrix.h"

void MazeCellMatrix::initWallArrays() {
  for(size_t r = 0; r < getRowCount(); r++) {
    for(size_t c = 0; c < getColumnCount(); c++) {
      WallArray &wa = (*this)(r,c).getWalls();
      wa.setCapacity(m_wallCount);
      for(int i = 0; i < m_wallCount; i++) {
        wa.add(nullptr);
      }
    }
  }
}

MazeCell &MazeCellMatrix::findRightBottomCompleteCell() {
  size_t maxRow = 0, maxColumn = 0;
  for(size_t r = 0; r < getRowCount(); r++) {
    for(size_t c = 0; c < getColumnCount(); c++) {
      MazeCell &cell = (*this)(r,c);
      if(cell.allWallsVisible() && (r >= maxRow) && (c >= maxColumn)) {
        maxRow    = r;
        maxColumn = c;
      }
    }
  }
  return (*this)(maxRow, maxColumn);
}

void MazeCellMatrix::unVisitAll() {
  for(size_t r = 0; r < getRowCount(); r++) {
    for(size_t c = 0; c < getColumnCount(); c++) {
      (*this)(r,c).setVisited(false);
    }
  }
}
