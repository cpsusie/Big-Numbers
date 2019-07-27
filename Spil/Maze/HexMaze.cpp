#include "stdafx.h"
#include <MathUtil.h>
#include <Random.h>
#include <Math.h>
#include "HexMaze.h"
#include "HexTurtle.h"

#define TOPMARGIN 5

HexMaze::HexMaze(const CRect &rect, int doorWidth)
: CellMaze(rect,doorWidth, 6, MatrixDimension(rect.Height()/doorWidth, rect.Width()/doorWidth))
,m_horizontalWall(_T("horz" ),(int)((double)rect.Height()/doorWidth/1.5)+1, (int)((double)rect.Width()/doorWidth/cos(GRAD2RAD(30))))
,m_verticalWall1( _T("vert1"),rect.Height()/doorWidth/3+1, (int)((double)rect.Width()/doorWidth/1.67))
,m_verticalWall2( _T("vert2"),rect.Height()/doorWidth/3+1, (int)((double)rect.Width()/doorWidth/1.67))
{
  m_DWCOS30    = doorWidth * cos(GRAD2RAD(30));
  m_leftMargin = rect.left;
  m_topMargin  = (int)(doorWidth / 1.67) + 1;
  m_topMargin  += rect.top;
  initialize();
}

void HexMaze::paint(CDC &dc) {
  fillRectangle(dc,getRect(),WHITE);
  m_horizontalWall.paint(dc);
  m_verticalWall1.paint( dc);
  m_verticalWall2.paint( dc);

/*
  markCell(dc,0,0);
  int startRow = m_cell.getRowCount()-1;
  int startCol = m_cell.getColumnCount()-1;
  markCell(dc,startRow,startCol);

  drawWall(dc,*m_cell(startRow,startCol).m_wall[5],YELLOW);
*/
}

Turtle *HexMaze::getTurtle(CDialog *dialog, CDC &workDC, bool keepTrack) {
  return new HexTurtle(*this,dialog,workDC,keepTrack);
}

void HexMaze::initialize() {
  UINT r,c;
  for(r = 0; r < m_horizontalWall.getRowCount(); r++) {
    for(c = 0; c < m_horizontalWall.getColumnCount(); c++) {
      m_horizontalWall(r,c) = MazeWall(wallPoint(r,c),wallPoint(r,c+1));
    }
  }
  for(r = 0; r < m_verticalWall1.getRowCount(); r++)  {
    for(c = 0; c < m_verticalWall1.getColumnCount(); c++) {
      m_verticalWall1(r,c) = MazeWall(wallPoint(2*r,2*c),wallPoint(2*r+1,2*c));
    }
  }
  for(r = 0; r < m_verticalWall2.getRowCount(); r++) {
    for(c = 0; c < m_verticalWall2.getColumnCount(); c++) {
      m_verticalWall2(r,c) = MazeWall(wallPoint(2*r+1,2*c+1),wallPoint(2*r+2,2*c+1));
    }
  }

  reduceWallMatrix1(m_horizontalWall);
  reduceWallMatrix1(m_verticalWall1);
  reduceWallMatrix1(m_verticalWall2);

  for(r = 0; r < m_cell.getRowCount(); r++) {
    for(c = 0; c < m_cell.getColumnCount(); c++) {
      MazeCell  &cell = m_cell(r,c);
      WallArray &wa   = cell.getWalls();
      int        wc   = 0;

      if(r % 2 == 0) {
        ADDWALL(m_horizontalWall,r  ,2*c  );
        ADDWALL(m_horizontalWall,r  ,2*c+1);
        ADDWALL(m_verticalWall1 ,r/2,c+1  );
        ADDWALL(m_horizontalWall,r+1,2*c+1);
        ADDWALL(m_horizontalWall,r+1,2*c  );
        ADDWALL(m_verticalWall1 ,r/2,c    );
      } else {
        ADDWALL(m_horizontalWall,r  ,2*c-1);
        ADDWALL(m_horizontalWall,r  ,2*c  );
        ADDWALL(m_verticalWall2 ,r/2,c    );
        ADDWALL(m_horizontalWall,r+1,2*c  );
        ADDWALL(m_horizontalWall,r+1,2*c-1);
        ADDWALL(m_verticalWall2 ,r/2,c-1  );
      }
    }
  }
  reduceCellMatrix();
  reduceWalls();
  initStartEnd();
  if (isOk()) {
    removeWalls();
  }
}

void HexMaze::reduceWalls() {
  do {
    m_reductionStable = true;
    reduceWallMatrix2(m_horizontalWall);
    reduceWallMatrix2(m_verticalWall1);
    reduceWallMatrix2(m_verticalWall2);
  } while(!m_reductionStable);
}

CPoint HexMaze::wallPoint(int r, int c) const {
  if(r % 2 == c % 2) {
    return CPoint((int)((double)c * m_DWCOS30),(int)((double)getDoorWidth()*(r*1.5))+TOPMARGIN);
  } else {
    return CPoint((int)((double)c * m_DWCOS30),(int)((double)getDoorWidth()*(r*1.5-0.5))+TOPMARGIN);
  }
}

