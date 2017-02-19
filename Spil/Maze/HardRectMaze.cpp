#include "stdafx.h"
#include <Stack.h>
#include "HardRectMaze.h"

HardRectMaze::HardRectMaze(const CRect &rect, int doorWidth) : RectMaze(rect,doorWidth) {
}

bool HardRectMaze::isHorizontalWall(CDC &dc, int x, int y) {
  return dc.GetPixel(getDoorWidth()*x+getDoorWidth()/2,getDoorWidth()*y) == BLACK;
}

bool HardRectMaze::isVerticalWall(CDC &dc, int x, int y) {
  return dc.GetPixel(getDoorWidth()*x,getDoorWidth()*y+getDoorWidth()/2) == BLACK;
}

bool HardRectMaze::has4Walls(CDC &dc, const Cell &c) {
  return isHorizontalWall(dc,c.x,c.y  ) && isVerticalWall(dc,c.x  ,c.y)
      && isHorizontalWall(dc,c.x,c.y+1) && isVerticalWall(dc,c.x+1,c.y);
}

void HardRectMaze::findCompleteNeighbors(CDC &dc, const Cell &currentCell, const CPoint &size, CompactArray<Cell> &result) {
  if(currentCell.x > 1) {
    Cell tmp(currentCell.x-1,currentCell.y);
    if(has4Walls(dc,tmp)) result.add(tmp);
  }
  if(currentCell.x < size.x - 1) {
    Cell tmp(currentCell.x+1,currentCell.y);
    if(has4Walls(dc,tmp)) result.add(tmp);
  }
  if(currentCell.y > 1) {
    Cell tmp(currentCell.x,currentCell.y-1);
    if(has4Walls(dc,tmp)) result.add(tmp);
  }
  if(currentCell.y < size.y - 1) {
    Cell tmp(currentCell.x,currentCell.y+1);
    if(has4Walls(dc,tmp)) result.add(tmp);
  }
}

void HardRectMaze::removeWallBetween(CDC &dc, const Cell &c1, const Cell &c2) {
  if(c1.x != c2.x) {
    int x = max(c1.x,c2.x);
    eraseWall(dc,x,c1.y,x,c1.y+1);
  } else { // c1.y != c2.y
    int y = max(c1.y,c2.y);
    eraseWall(dc,c1.x,y,c1.x+1,y);
  }
}

void HardRectMaze::paint(CDC &dc) {
  createPens();
  fillRectangle(dc,getRect(),WHITE);

  CPoint size = getMazeSize();

  for(int c = 1; c <= size.x; c++) {
    drawWall(dc,c,1,c,size.y);
  }
  for(int r = 1; r <= size.y; r++) {
    drawWall(dc,1,r,size.x,r);
  }

  if(size.x < 2 || size.y < 2) {
    return;
  }

  Stack<Cell> cellStack;
  Cell currentCell(randInt(1,size.x-1),randInt(1,size.y-1));
  for(;;) {
    CompactArray<Cell> neighbors(4);
    findCompleteNeighbors(dc,currentCell,size,neighbors);
    if(neighbors.size() > 0) {
      Cell newCell = neighbors.select();
      removeWallBetween(dc,currentCell,newCell);
      cellStack.push(currentCell);
      currentCell = newCell;
    } else {
      if(cellStack.isEmpty()) break;
      currentCell = cellStack.pop();
    }
  }
  removeWallBetween(dc,Cell(0,1),Cell(1,1));
  removeWallBetween(dc,Cell(size.x-1,size.y-1),Cell(size.x,size.y-1));
}
