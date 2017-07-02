#pragma once

#include "Maze.h"
#include "MazeCellMatrix.h"
#include "MazeWallMatrix.h"

class CellMaze : public Maze {
private:
  MazeCell *m_startCell, *m_endCell;
  void connectNeighbours();
  void removeWallBetween(MazeCell *c1, MazeCell *c2);

protected:
  bool           m_reductionStable;
  MazeCellMatrix m_cell;

  CellMaze(const CRect &rectangle, int doorWidth, int wallCount, const MatrixDimension &dim)
    : Maze(rectangle,doorWidth)
    , m_cell(wallCount, dim)
  {
  }

  inline bool isOk() const {
    return (m_cell.getRowCount() > 2) && (m_cell.getColumnCount() > 2);
  }

  MazeCell &getRandomCell() {
    return m_cell(randInt(1, (int)m_cell.getRowCount() - 2), randInt(1, (int)m_cell.getColumnCount() - 2));
  }

  inline CPoint getStartPoint() const {
    return m_startCell->getCenter();
  }

  void reduceCellMatrix();
  void reduceWallMatrix1(MazeWallMatrix &m);

  // set all walls adjacent to 2 incomplete cells, to invisible
  void reduceWallMatrix2(MazeWallMatrix &m);
  virtual void removeWalls();
  void initStartEnd();

public:
  inline const MazeCell &getEndCell() const {
    return *m_endCell;
  }

  inline MazeCell &getStartCell() const {
    return *m_startCell;
  }

  void unVisitAll() {
    m_cell.unVisitAll();
    m_startCell->setVisited(true);
  }
};
