#include "stdafx.h"
#include "EasyRectMaze.h"

EasyRectMaze::EasyRectMaze(const CRect &rect, int doorWidth) : RectMaze(rect,doorWidth) {
}

void EasyRectMaze::frame(CDC &dc, const CRect &rect) {
  drawWall(dc,rect.left ,rect.top   ,rect.right,rect.top     );
  drawWall(dc,rect.right,rect.top   ,rect.right,rect.bottom-1);
  drawWall(dc,rect.right,rect.bottom,rect.left ,rect.bottom  );
  drawWall(dc,rect.left ,rect.bottom,rect.left ,rect.top+1   );
}

CRect EasyRectMaze::makeWall(CDC &dc, const CRect &rect) {
  CPoint p1,p2,q1,q2;
  if(rect.Width() > rect.Height()) {
    p1 = CPoint(randInt(rect.left+1,rect.right-1),rect.top);
    p2 = CPoint(p1.x,randInt(rect.top,rect.bottom-1));
    q1 = CPoint(p1.x,p2.y+1);
    q2 = CPoint(p1.x,rect.bottom);
  } else {
    p1 = CPoint(rect.left,randInt(rect.top+1,rect.bottom-1));
    p2 = CPoint(randInt(rect.left,rect.right-1),p1.y);
    q1 = CPoint(p2.x+1,p1.y);
    q2 = CPoint(rect.right,p1.y);
  }
  drawWall(dc,p1.x,p1.y,p2.x,p2.y);
  drawWall(dc,q1.x,q1.y,q2.x,q2.y);
  return CRect(p1.x,p1.y,q2.x,q2.y);
}

void EasyRectMaze::makePartition(CDC &dc, const CRect &rect) {
  if(rect.Width() <= 1 || rect.Height() <= 1) {
    return;
  }
  CRect r = makeWall(dc,rect);
//  Sleep(50);
  makePartition(dc,CRect(rect.left,rect.top,r.right,r.bottom));
  makePartition(dc,CRect(r.left,r.top,rect.right,rect.bottom));
}

void EasyRectMaze::paint(CDC &dc) {
  createPens();
  fillRectangle(dc,getRect(),WHITE);

  CPoint size = getMazeSize();
  CRect  rect = CRect(1,1,size.x,size.y);
  frame(dc,rect);
  makePartition(dc,rect);
}
