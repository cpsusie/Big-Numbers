#include "stdafx.h"
#include <BitSet.h>
#include "Game.h"

PositionStatus GamePentagon::statusTable[0x400];

const PositionSet GamePentagon::moveTable[30] = {
  PS1(0)     // 5 single points in outer pentagon
 ,PS1(1)
 ,PS1(2)
 ,PS1(3)
 ,PS1(4)

 ,PS1(5)     // 5 single points in inner pentagon
 ,PS1(6)
 ,PS1(7)
 ,PS1(8)
 ,PS1(9)

 ,PS2(0, 1)  // 5 connections between points in outer pentagon
 ,PS2(1, 2)
 ,PS2(2, 3)
 ,PS2(3, 4)
 ,PS2(4, 0)

 ,PS2(5, 6)  // 5 connections between points in inner pentagon
 ,PS2(6, 7)
 ,PS2(7, 8)
 ,PS2(8, 9)
 ,PS2(9, 5)

 ,PS2(0, 8)  // 10 connections between inner and outer pentagon
 ,PS2(0, 9)
 ,PS2(1, 9)
 ,PS2(1, 5) 
 ,PS2(2, 5)  
 ,PS2(2, 6) 
 ,PS2(3, 6) 
 ,PS2(3, 7)
 ,PS2(4, 7)
 ,PS2(4, 8)
};

void GamePentagon::paint(CDC &dc, const CRect &rect, PositionSet markedPositions) {
  m_brickPositions.clear();
  CPen *oldPen = dc.SelectObject(&m_linePen);

  int size = min(rect.Width()-24, rect.Height())/2;
  if(size <= 0) {
    size = 10;
  }

  paintRegularPolygon(dc, 5, size, rect.CenterPoint() - CSize(0, size - 20), 36);

  for(int i = 0; i < 5; i++) {
    line(dc, m_brickPositions[i], m_brickPositions[(i+2)%5]);
    addPositionAtIntersection(dc, i,(i+2)%5, (i+1)%5,(i+3)%5);
  }
  dc.SelectObject(oldPen);

  paintAllPositions(dc, markedPositions);
}
