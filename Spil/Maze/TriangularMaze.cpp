#include "StdAfx.h"
#include <MathUtil.h>
#include "TriangularMaze.h"
#include "TriangularTurtle.h"

const double TriangularMaze::s_SIN60 = sin(GRAD2RAD(60));
const double TriangularMaze::s_COS60 = cos(GRAD2RAD(60));

TriangularMaze::TriangularMaze(const CRect &rectangle, int doorWidth)
: CellMaze(rectangle, doorWidth, 3, MatrixDimension((int)(rectangle.Height()/doorWidth/s_SIN60+1), (int)(rectangle.Width()/doorWidth/s_COS60)+1))
, m_horizontalWall(_T("Horz"), (int)((double)rectangle.Height()/doorWidth/s_SIN60+1)
                             , (int)((double)rectangle.Width()/doorWidth+1))
, m_verticalWall(  _T("Vert"), (int)((double)rectangle.Height()/doorWidth/s_SIN60+1)
                             , (int)((double)2*rectangle.Width()/doorWidth+1))
{

  m_DWSIN60    = (double)doorWidth * s_SIN60;
  m_DWCOS60    = (double)doorWidth * s_COS60;
  m_leftMargin = rectangle.left;
  m_topMargin  = rectangle.top;

  initialize();
}

void TriangularMaze::initialize() {
  UINT r,c;
  for(r = 0; r < m_horizontalWall.getRowCount(); r++) {
    for(c = 0; c < m_horizontalWall.getColumnCount(); c++) {
      m_horizontalWall(r,c) = MazeWall(wallPoint(r,c),wallPoint(r,c+1));
    }
  }
  for(r = 0; r < m_verticalWall.getRowCount(); r++) {
    for(c = 0; c < m_verticalWall.getColumnCount(); c+=2) {
      m_verticalWall(r,c) = MazeWall(wallPoint(r,c/2),wallPoint(r+1,c/2));
    }
    int m1 = (r % 2 == 0) ? -1 :  1;
    for(c = 1; c < m_verticalWall.getColumnCount(); c+=2) {
      m_verticalWall(r,c) = MazeWall(wallPoint(r+1,(c+m1)/2),wallPoint(r,(c-m1)/2));
    }
  }

  reduceWallMatrix1(m_horizontalWall);
  reduceWallMatrix1(m_verticalWall);

  for(r = 0; r < m_cell.getRowCount(); r++) {
    for(c = 0; c < m_cell.getColumnCount(); c++) {
      MazeCell  &cell = m_cell(r, c);
      WallArray &wa   = cell.getWalls();
      int        wc   = 0;

      if(r % 2 == 0) {
        if(c % 2 == 0) {
          ADDWALL(m_horizontalWall,r  , c/2    );
          ADDWALL(m_verticalWall  ,r  , c+1    );
          ADDWALL(m_verticalWall  ,r  , c      );
        } else {
          ADDWALL(m_horizontalWall,r+1, (c-1)/2);
          ADDWALL(m_verticalWall  ,r  , c      );
          ADDWALL(m_verticalWall  ,r  , c+1    );
        }
      } else {
        if(c % 2 == 0) {
          ADDWALL(m_horizontalWall,r+1, (c+1)/2);
          ADDWALL(m_verticalWall  ,r  , c      );
          ADDWALL(m_verticalWall  ,r  , c+1    );
        } else {
          ADDWALL(m_horizontalWall,r  , (c-1)/2);
          ADDWALL(m_verticalWall  ,r  ,  c+1   );
          ADDWALL(m_verticalWall  ,r  ,  c     );
        }
      }
    }
  }
  reduceCellMatrix();
  reduceWalls();
  initStartEnd();
  if(isOk()) {
    removeWalls();
  }
}

void TriangularMaze::paint(CDC &dc) {
  fillRectangle(dc,getRect(),WHITE);
  m_horizontalWall.paint(dc);
  m_verticalWall.paint(dc);

/*
  for(size_t r = 0; r < m_cell.getRowCount(); r++) {
    for(size_t  c = 0; c < m_cell.getColumnCount(); c++)
      m_cell.get(r,c).markCenter(dc);
  }
  MazeCell &cell = m_cell(5,6);
  for(size_t I = 0; I < cell.getWallCount(); I++) {
    MazeCell nc = cell.getNeighbor(I);
    nc.mark(g);
  }

  m_cell.markCell(g,0,0);
  m_cell.markCell(g,0,2);
  m_cell.markCell(g,0,4);

  m_cell.markCell(g,2,1);
  m_cell.markCell(g,2,3);
  m_cell.markCell(g,2,5);


  m_cell.markCell(g,5,0);
  m_cell.markCell(g,5,2);
  m_cell.markCell(g,5,4);

  m_cell.markCell(g,7,1);
  m_cell.markCell(g,7,3);
  m_cell.markCell(g,7,5);


  g.setColor(Color.RED);
  m_verticalWall.get(0,0).paint(g);
  g.setColor(Color.GREEN);
  m_verticalWall.get(0,1).paint(g);
  g.setColor(Color.BLUE);
  m_verticalWall.get(0,2).paint(g);

  g.setColor(Color.RED);
  m_verticalWall.get(0,m_verticalWall.getColumns()-2).paint(g);
  g.setColor(Color.GREEN);
  m_verticalWall.get(0,m_verticalWall.getColumns()-1).paint(g);
  g.setColor(Color.BLUE);
  m_verticalWall.get(0,m_verticalWall.getColumns()).paint(g);

  g.setColor(Color.RED);
  m_verticalWall.get(3,0).paint(g);
  g.setColor(Color.GREEN);
  m_verticalWall.get(3,1).paint(g);
  g.setColor(Color.BLUE);
  m_verticalWall.get(3,2).paint(g);

  g.setColor(Color.RED);
  m_verticalWall.get(3,m_verticalWall.getColumns()-2).paint(g);
  g.setColor(Color.GREEN);
  m_verticalWall.get(3,m_verticalWall.getColumns()-1).paint(g);
  g.setColor(Color.BLUE);
  m_verticalWall.get(3,m_verticalWall.getColumns()).paint(g);

  getStartCell().markCenter(g);
  getEndCell().markCenter(g);
*/
}

Turtle *TriangularMaze::getTurtle(CDialog *dialog, CDC &workDC, bool keepTrack) {
  return new TriangularTurtle(*this,dialog, workDC,keepTrack);
}

void TriangularMaze::reduceWalls() {
  do {
    m_reductionStable = true;
    reduceWallMatrix2(m_horizontalWall);
    reduceWallMatrix2(m_verticalWall);
  } while(!m_reductionStable);
}

void TriangularMaze::removeWalls() {
  CellMaze::removeWalls();
  getStartCell().getRightMostVerticalOuterWall()->m_visible = false;
  getEndCell().getLeftMostVerticalOuterWall()->m_visible = false;
}

CPoint TriangularMaze::wallPoint(int r, int c) const {
  const int y = (int)(m_DWSIN60*r+m_topMargin);
  if(r % 2 == 0) {
    return CPoint(getDoorWidth()*c+m_leftMargin, y);
  } else {
    return CPoint((int)(getDoorWidth()*c + m_DWCOS60)+m_leftMargin, y);
  }
}
