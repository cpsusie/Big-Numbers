#include "StdAfx.h"
#include "CellMaze.h"

void CellMaze::reduceCellMatrix() {
  CPoint center(0,0);
  size_t maxRow = 0, maxColumn = 0;
  for(size_t r = 0; r < m_cell.getRowCount(); r++) {
    for(size_t c = 0; c < m_cell.getColumnCount(); c++) {
      MazeCell &cell = m_cell(r,c);
      if(cell.allWallsVisible()) {
        maxRow    = max(maxRow, r);
        maxColumn = max(maxColumn, c);
        int count = 0;
        center = CPoint(0,0);
        const WallArray &wa = cell.getWalls();
        for(size_t w = 0; w < wa.size(); w++) {
          const MazeWall &wall = *wa[w];
          center.x += wall.m_p1.x;
          center.y += wall.m_p1.y;
          center.x += wall.m_p2.x;
          center.y += wall.m_p2.y;
          count += 2;
        }
        center.x /= count;
        center.y /= count;
        cell.setCenter(center);
      }
    }
  }
#ifdef _DEBUG
  debugLog(_T("reduceCellMatrix:Size:(%d,%d)")
          ,(int)m_cell.getRowCount(), (int)m_cell.getColumnCount()
          );
#endif

  m_cell.setDimension(maxRow+1,maxColumn+1);

#ifdef _DEBUG
  debugLog(_T("->(%d,%d)\n")
          ,(int)m_cell.getRowCount(), (int)m_cell.getColumnCount()
          );
#endif

  connectNeighbours();
}

void CellMaze::connectNeighbours() {
  for(size_t r = 0; r < m_cell.getRowCount(); r++) {
    for(size_t c = 0; c < m_cell.getColumnCount(); c++) {
      MazeCell &cell = m_cell(r, c);
      const WallArray &wa = cell.getWalls();
      for(size_t w = 0; w < wa.size(); w++) {
        MazeWall *wall = wa[w];
        if(wall != NULL) {
          if(wall->m_c1 == NULL) {
            wall->m_c1 = &cell;
          } else {
            wall->m_c2 = &cell;
          }
        }
      }
    }
  }
}

void CellMaze::reduceWallMatrix1(MazeWallMatrix &m) {
  CRect rect = getRect();
  if((m.getRowCount() < 2) || (m.getColumnCount() < 2)) {
    return;
  }
  size_t maxr = 0, maxc = 0;
  for(size_t  r = 0; r < m.getRowCount(); r++) {
    for(size_t c = 0; c < m.getColumnCount(); c++) {
      MazeWall &w = m(r, c);
      if(rect.PtInRect(w.m_p1) && rect.PtInRect(w.m_p2)) {
        maxr = max(maxr, r);
        maxc = max(maxc, c);
      } else {
        w.m_visible = false;
      }
    }
  }

#ifdef _DEBUG
  debugLog(_T("reduceWallMatrix1:%s. Size:(%d,%d)")
          ,m.getName().cstr()
          ,(int)m.getRowCount(), (int)m.getColumnCount()
          );
#endif

  m.setDimension(min(m.getRowCount(), maxr+1), min(m.getColumnCount(), maxc+1));

#ifdef _DEBUG
  debugLog(_T("->(%d,%d)\n")
          ,(int)m.getRowCount(), (int)m.getColumnCount()
          );
#endif
}

// set all walls adjacent to 2 incomplete cells, to invisible
void CellMaze::reduceWallMatrix2(MazeWallMatrix &m) {
  for(size_t r = 0; r < m.getRowCount(); r++) {
    for(size_t c = 0; c < m.getColumnCount(); c++) {
      MazeWall &w = m(r, c);
      if(!w.isVisible()) {
        continue;
      }
      MazeCell *c1 = w.m_c1;
      MazeCell *c2 = w.m_c2;
      const bool c1Complete = (c1 != NULL) && c1->allWallsVisible();
      const bool c2Complete = (c2 != NULL) && c2->allWallsVisible();
      if(c1Complete && c2Complete
      || c1 == NULL && c2Complete
      || c2 == NULL && c1Complete) {
        continue;
      }
      if(!c1Complete && !c2Complete) {
        w.m_visible       = false;
        m_reductionStable = false;
      }
    }
  }
}

void CellMaze::removeWalls() {
  Stack<MazeCell*> cellStack;
  MazeCell *currentCell = &getRandomCell();
  for(;;) {
    CompactArray<MazeCell*> neighbors = currentCell->findCompleteNeighbors();
    if(neighbors.size() > 0) {
      MazeCell *newCell = neighbors.select();
      removeWallBetween(currentCell,newCell);
      cellStack.push(currentCell);
      currentCell = newCell;
    } else {
      if(cellStack.isEmpty()) {
        break;
      }
      currentCell = cellStack.pop();
    }
  }
}

void CellMaze::removeWallBetween(MazeCell *c1, MazeCell *c2) {
  WallArray &wa = c1->getWalls();
  for(size_t i = 0; i < wa.size(); i++) {
    MazeWall *w = wa[i];
    if(((w->m_c1 == c1) && (w->m_c2 == c2)) || ((w->m_c2 == c1) && (w->m_c1 == c2))) {
      w->m_visible = false;
      return;
    }
  }
}

void CellMaze::initStartEnd() {
  m_startCell = &m_cell.findRightBottomCompleteCell();
  m_endCell   = &m_cell(0, 0);
}
