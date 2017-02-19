#include "StdAfx.h"
#include "MazeCell.h"

#define MARKSIZE 4

bool MazeCell::allWallsVisible() const{
  for(size_t i = 0; i < m_walls.size(); i++) {
    const MazeWall *w = m_walls[i];
    if(w == NULL || !w->isVisible()) {
      return false;
    }
  }
  return true;
}

MazeCell *MazeCell::getNeighbor(int index) const {
  MazeWall *w = m_walls[index];
  if(w == NULL) {
    return NULL;
  }
  return (w->m_c1 == this) ? w->m_c2 : w->m_c1;
}

CompactArray<MazeCell*> MazeCell::findCompleteNeighbors() {
  CompactArray<MazeCell*> result;
  for(int i = 0; i < getWallCount(); i++) {
     MazeCell *nc = getNeighbor(i);
     if(nc != NULL && nc->allWallsVisible()) {
       result.add(nc);
     }
  }
  return result;
}

void MazeCell::markCenter(CDC &dc) {
  dc.FillSolidRect(m_center.x-MARKSIZE/2,m_center.y-MARKSIZE/2,MARKSIZE,MARKSIZE, _CYAN);
}

void MazeCell::mark(CDC &dc) {
  markCenter(dc);
  ColorIndex colors[] = { Red, Green, Blue };
  int index = 0;
  for(size_t i = 0; i < m_walls.size(); i++) {
    const MazeWall *wall = m_walls[i];
    if(wall == NULL) {
      continue;
    }
    wall->paint(dc, wall->isVisible() ? colors[index] : Gray);
    index = (index+1) % ARRAYSIZE(colors);
  }
}

MazeWall *MazeCell::getRightMostVerticalOuterWall() const {
  MazeWall *result = NULL;
  for(size_t i = 0; i < m_walls.size(); i++) {
    MazeWall *wall = m_walls[i];
    if((wall != NULL) && wall->m_visible && (wall->m_c1 == NULL || wall->m_c2 == NULL) && (wall->m_p1.y != wall->m_p2.y)) {
      if(result == NULL) {
        result = wall;
      } else {
        const int sr = result->m_p1.x + result->m_p2.x;
        const int sw = wall->m_p1.x + wall->m_p2.x;
        if(sw > sr) {
          result = wall;
        }
      }
    }
  }
  return result;
}

MazeWall *MazeCell::getLeftMostVerticalOuterWall() const {
  MazeWall *result = NULL;
  for(size_t i = 0; i < m_walls.size(); i++) {
    MazeWall *wall = m_walls[i];
    if((wall != NULL) && wall->m_visible && (wall->m_c1 == NULL || wall->m_c2 == NULL) && (wall->m_p1.y != wall->m_p2.y)) {
      if(result == NULL) {
        result = wall;
      } else {
        const int sr = result->m_p1.x + result->m_p2.x;
        const int sw = wall->m_p1.x + wall->m_p2.x;
        if(sw < sr) {
          result = wall;
        }
      }
    }
  }
  return result;
}
