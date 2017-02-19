#include "StdAfx.h"
#include "MazeWallMatrix.h"

void MazeWallMatrix::paint(CDC &dc) {
  for(size_t r = 0; r < getRowCount(); r++) {
    for(size_t c = 0; c < getColumnCount(); c++) {
      const MazeWall &w = (*this)(r, c);
      if(w.isVisible()) {
        w.paint(dc, m_colorIndex);
      }
    }
  }
}
