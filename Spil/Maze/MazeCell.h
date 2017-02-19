#pragma once

#include "MazeWall.h"

class MazeCell {
private:
  CPoint    m_center;
  WallArray m_walls;
  bool      m_visited;

public:
  MazeCell(int wallCount=0) {
    m_visited = false;
  }

  inline CPoint getCenter() const {
    return m_center;
  }
  inline void setCenter(CPoint p) {
    m_center = p;
  }

  inline void setVisited(bool v) {
    m_visited = v;
  }
  inline bool isVisited() const {
    return m_visited;
  }

  inline WallArray &getWalls() {
    return m_walls;
  }
  inline MazeWall *getWall(int i) const {
    return m_walls[i];
  }
  inline int getWallCount() const {
    return (int)m_walls.size();
  }

  bool allWallsVisible() const;

  MazeCell *getNeighbor(int index) const;

  CompactArray<MazeCell*> findCompleteNeighbors();

  void markCenter(CDC &dc);
  void mark(      CDC &dc);

  MazeWall *getRightMostVerticalOuterWall() const;
  MazeWall *getLeftMostVerticalOuterWall() const;

  String toString() const {
    return format(_T("(%d,%d)"), m_center.x, m_center.y);
  }
};

inline String toString(const MazeCell &c) {
  return c.toString();
}
