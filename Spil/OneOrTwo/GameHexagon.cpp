#include "stdafx.h"
#include <Math.h>
#include <BitSet.h>
#include <CompactHashSet.h>
#include "Game.h"

PositionStatus GameHexagon::statusTable[0x80000];

const PositionSet GameHexagon::moveTable[67] = {
  PS1( 0)    // 6 single points in outer hexaagon
 ,PS1( 1)
 ,PS1( 2)
 ,PS1( 3)
 ,PS1( 4)
 ,PS1( 5)

 ,PS1( 6)    // 12 single points in intersections between diagonals
 ,PS1( 7)
 ,PS1( 8)
 ,PS1( 9)
 ,PS1(10)
 ,PS1(11)
 ,PS1(12)
 ,PS1(13)
 ,PS1(14)
 ,PS1(15)
 ,PS1(16)
 ,PS1(17)

 ,PS1(18)    // center point

 ,PS2( 0, 1) // 6 connections between points in outer hexagon
 ,PS2( 1, 2)
 ,PS2( 2, 3)
 ,PS2( 3, 4)
 ,PS2( 4, 5)
 ,PS2( 5, 0)

 ,PS2( 6, 7) // 12 connections between points in middle hexaagon
 ,PS2( 7, 8)
 ,PS2( 8,17)
 ,PS2(17,15)
 ,PS2(15,16)
 ,PS2(16,13)
 ,PS2(13,14)
 ,PS2(14,11)
 ,PS2(11,12)
 ,PS2(12, 9)
 ,PS2( 9,10)
 ,PS2(10, 6)

 ,PS2( 7,17) // 6 connections between points in inner hexagon
 ,PS2(17,16)
 ,PS2(16,14)
 ,PS2(14,12)
 ,PS2(12,10)
 ,PS2(10, 7)

 ,PS2( 0, 8) // 18 connections between middle and outer hexagon
 ,PS2( 0,17)
 ,PS2( 0,15)
 ,PS2( 1, 6)
 ,PS2( 1, 7)
 ,PS2( 1, 8)
 ,PS2( 2, 9)
 ,PS2( 2,10)
 ,PS2( 2, 6)
 ,PS2( 3,11)
 ,PS2( 3,12)
 ,PS2( 3, 9)
 ,PS2( 4,13)
 ,PS2( 4,14)
 ,PS2( 4,11)
 ,PS2( 5,15)
 ,PS2( 5,16)
 ,PS2( 5,13)

 ,PS2( 7,18) // 6 connections between inner points and center point
 ,PS2(10,18)
 ,PS2(12,18)
 ,PS2(14,18)
 ,PS2(16,18)
 ,PS2(17,18)
};

void GameHexagon::paint(CDC &dc, const CRect &rect, PositionSet markedPositions) {
  int i, j;
  m_brickPositions.clear();
  CPen *oldPen = dc.SelectObject(&m_linePen);

  int size = min(rect.Width()-28, rect.Height())/2;
  if(size <= 0) {
    size = 10;
  }
  paintRegularPolygon(dc, 6, size, rect.CenterPoint() + CSize(size,0), 120);

  for(i = 0; i < 6; i++) {
    for(j = i + 2; j < 6; j++) {
      line(dc, m_brickPositions[i], m_brickPositions[j]);
    }
  }

  CompactHashSet<LineIntersectionKey> usedCombination(787);
  for(i = 0; i < 6; i++) {
    for(j = 3; j < 6; j++) {
      const LineIntersectionKey key(6, i,(i+2)%6, (i+1)%6, (i+j)%6);
      if(!usedCombination.contains(key)) {
        addPositionAtIntersection(dc, i,(i+2)%6, (i+1)%6, (i+j)%6);
        usedCombination.add(key);
      }
    }
  }
  addPositionAtIntersection(dc, 0, 3, 1, 4);

  const BYTE innerHexagon[] = { 7, 10, 12, 14, 16, 17 };
  i = 0;
  dc.MoveTo(Point2D(m_brickPositions[innerHexagon[i++]]));
  for(;i <= ARRAYSIZE(innerHexagon); i++) {
    dc.LineTo(Point2D(m_brickPositions[innerHexagon[i%6]]));
  }

  dc.SelectObject(oldPen);

  paintAllPositions(dc, markedPositions);
}
