#include "stdafx.h"
#include <BitSet.h>
#include <CompactHashSet.h>
#include <ByteFile.h>
#include <ByteMemoryStream.h>
#include <CompressFilter.h>
#include "Game.h"

PositionStatus GameOctagon::statusTable[0x2000000];

const UINT GameOctagon::moveTable[89] = {
  PS1( 0)    // 8 single points in outer octagon
 ,PS1( 1)
 ,PS1( 2)
 ,PS1( 3)
 ,PS1( 4)
 ,PS1( 5)
 ,PS1( 6)
 ,PS1( 7)

 ,PS1( 8)    // 17 single inner points
 ,PS1( 9)
 ,PS1(10)
 ,PS1(11)    // center point
 ,PS1(12)
 ,PS1(13)
 ,PS1(14)
 ,PS1(15)
 ,PS1(16)
 ,PS1(17)
 ,PS1(18)
 ,PS1(19)
 ,PS1(20)
 ,PS1(21)
 ,PS1(22)
 ,PS1(23)
 ,PS1(24)


 ,PS2( 0, 1) // 8 connections between points in outer octagon
 ,PS2( 1, 2)
 ,PS2( 2, 3)
 ,PS2( 3, 4)
 ,PS2( 4, 5)
 ,PS2( 5, 6)
 ,PS2( 6, 7)
 ,PS2( 7, 0)

 ,PS2( 0,24) // 24 connections between outer octagon and innerpoints
 ,PS2( 0,12)
 ,PS2( 0,10)
 ,PS2( 1,10)
 ,PS2( 1, 9)
 ,PS2( 1,15)
 ,PS2( 2,15)
 ,PS2( 2, 8)
 ,PS2( 2,17)
 ,PS2( 3,17)
 ,PS2( 3,14)
 ,PS2( 3,19)
 ,PS2( 4,19)
 ,PS2( 4,16)
 ,PS2( 4,21)
 ,PS2( 5,21)
 ,PS2( 5,18)
 ,PS2( 5,22)
 ,PS2( 6,22)
 ,PS2( 6,20)
 ,PS2( 6,23)
 ,PS2( 7,23)
 ,PS2( 7,13)
 ,PS2( 7,24)

 ,PS2( 9,10) // 8 connection between inner octagon and tip of star
 ,PS2( 9,15)
 ,PS2( 8,15)
 ,PS2( 8,17)
 ,PS2(14,17)
 ,PS2(14,19)
 ,PS2(16,19)
 ,PS2(16,21)
 ,PS2(18,21)
 ,PS2(18,22)
 ,PS2(20,22)
 ,PS2(20,23)
 ,PS2(13,23)
 ,PS2(13,24)
 ,PS2(12,24)
 ,PS2(12,10)

 ,PS2( 9, 8) // 8 connection between points in inner octagon
 ,PS2( 8,14)
 ,PS2(14,16)
 ,PS2(16,18)
 ,PS2(18,20)
 ,PS2(20,13)
 ,PS2(13,12)
 ,PS2(12, 9)


 ,PS2( 9,11) // 8 connections between inner octagon and center point
 ,PS2( 8,11)
 ,PS2(14,11)
 ,PS2(16,11)
 ,PS2(18,11)
 ,PS2(20,11)
 ,PS2(13,11)
 ,PS2(12,11)
};

// #define GENERATE_WINNERTABLE_FILE

bool GameOctagon::gotStatus = false;

GameOctagon::GameOctagon() : AbstractGame(OCTAGON, 25, statusTable, ARRAYSIZE(moveTable), moveTable) {
#if !defined(GENERATE_WINNERTABLE_FILE)
  if(!gotStatus) {
    BitSet s(10);
    s.load(DecompressFilter(ByteMemoryInputStream(ByteArray().loadFromResource(IDR_OCTAGONPOSITIONSTATUS, _T("POSITIONSTATUS")))));
    for(int i = 0; i < ARRAYSIZE(statusTable); i++) {
      statusTable[i] = (0x80 | MAKE_STATUS(s.contains(i), 1));
    }
    gotStatus = true;
  }
#endif
}

GameOctagon::~GameOctagon() {
#if defined(GENERATE_WINNERTABLE_FILE)
  for(m_board = 0; m_board < ARRAYSIZE(statusTable); m_board++) {
    const PositionStatus status = getPositionStatus();
  }
  BitSet winnerPositions(ARRAYSIZE(statusTable));
  for(int i = 0; i < ARRAYSIZE(statusTable); i++) {
    if(IS_WINNERSTATUS(statusTable[i])) {
      winnerPositions.add(i);
    }
  }
  winnerPositions.save(CompressFilter(ByteOutputFile(_T("c:\\temp\\octagon.win"))));
#endif
}

void GameOctagon::paint(CDC &dc, const CRect &rect, PositionSet markedPositions) {
  m_brickPositions.clear();
  CPen *oldPen = dc.SelectObject(&m_linePen);

  int size = (int)(min(rect.Width(), rect.Height())/2.15);
  if(size <= 0) {
    size = 10;
  }

  paintRegularPolygon(dc, 8, size, rect.CenterPoint() - CSize(0,size), 360.0/16);

  for(int i = 0; i < 8; i++) {
    for(int j = i+3; (j < 8) && ((j + 2) % 8 != i); j++) {
      line(dc, m_brickPositions[i], m_brickPositions[j]);
    }
  }

  CompactHashSet<LineIntersectionKey> usedCombination(787);
  for(int p1 = 0; p1 <= 8; p1++) {
    const int p2 = (p1+1) % 8;
    for(int k1 = 0; k1 < 3; k1++) {
      const int p3 = (p1 + k1 + 3) % 8;
      for(int k2 = 0; k2 < 3; k2++) {
        const int p4 = (p2 + k2 + 3) % 8;
        if(p3 == p1 || p4 == p2 || p3 == p4 || p3 == p2 || p4 == p1) {
          continue;
        }
        const LineIntersectionKey key(8, p1, p3, p2, p4);
        if(!usedCombination.contains(key)) {
          if(addPositionAtIntersection(dc, p1, p3, p2, p4)) {
            usedCombination.add(key);
          }
        }
      }
    }
  }
  paintAllPositions(dc, markedPositions);
}
