#include "stdafx.h"
#include <Math.h>
#include <BitSet.h>
#include "Game.h"

#define POSITION_DEFINED()              STATUS_DEFINED(m_statusTable[m_board])
#define DEFINE_POSITION(win, pliesToEnd) m_statusTable[m_board] = (0x80 | MAKE_STATUS(win, pliesToEnd))

#define HASLOST()            (m_bricksOnBoard == 1)
#define HASWON()             (m_bricksOnBoard == 0)

void line(CDC &dc, const Point2D &p1, const Point2D &p2) {
  Point2DP from(p1), to(p2);
  dc.MoveTo(from);
  dc.LineTo(to);
}

AbstractGame::AbstractGame(GameType           gameType
                          ,UINT               positionCount
                          ,PositionStatus    *statusTable
                          ,UINT               moveCount
                          ,const PositionSet *moveTable) 
: m_gameType(gameType)
, m_positionCount(positionCount)
, m_statusTable(statusTable)
, m_moveCount(moveCount)
, m_moveTable(moveTable)
, m_lastMove(       moveTable + moveCount)
, m_lastTakeOneMove(moveTable + positionCount)
{
  init(HUMANPLAYER);
  m_showNumbers = false;

  m_emptyBrush.CreateSolidBrush(RGB(255,255,255));
  m_occupiedBrush.CreateSolidBrush(RGB(0,0,0));
  m_markedBrush.CreateSolidBrush(RGB(255,0,0));
  m_linePen.CreatePen(    PS_SOLID, 2, RGB(0,0,0));
  m_positionPen.CreatePen(PS_SOLID, 1, RGB(0,0,0));
}

void AbstractGame::init(Player startPlayer) {
  m_playerInTurn  = startPlayer;
  m_board         = (1<<m_positionCount) - 1;
  m_bricksOnBoard = m_positionCount;
}

void AbstractGame::doMove(Move m) {
  m_board         &= ~(*m);
  m_bricksOnBoard -= ((m < m_lastTakeOneMove) ? 1 : 2);
}

void AbstractGame::undoMove(Move m) {
  m_board         |= *m;
  m_bricksOnBoard += ((m < m_lastTakeOneMove) ? 1 : 2);
}

PositionStatus AbstractGame::getPositionStatus() {
  if(!POSITION_DEFINED()) {
    if(m_bricksOnBoard <= 1) {
      DEFINE_POSITION(m_bricksOnBoard == 0, m_bricksOnBoard);
    } else {
      
      bool winnerMoveFound = false;
      int  pliesToWin      = 30;
      int  pliesToLoose    = 0;
      for(Move m = firstMove(); m && !winnerMoveFound; m = nextMove(m)) {
        doMove(m);
        const PositionStatus status = getPositionStatus();
        if(IS_LOOSERSTATUS(status)) {
          winnerMoveFound = true;
          if(PLIESTOEND(status) < pliesToWin) {
            pliesToWin = PLIESTOEND(status);
          }
        } else if(!winnerMoveFound) {
          if(PLIESTOEND(status) > pliesToLoose) {
            pliesToLoose = PLIESTOEND(status);
          }
        }
        undoMove(m);
      }
      if(winnerMoveFound) {
        DEFINE_POSITION(true , pliesToWin   + 1);
      } else {
        DEFINE_POSITION(false, pliesToLoose + 1);
      }
    }
  }
  return m_statusTable[m_board];
}

Move AbstractGame::findMove(PositionSet markedPositions) const {
  for(Move m = firstMove(); m; m = nextMove(m)) {
    if(*m == markedPositions) {
      return m;
    }
  }
  return NULL;
}

Move AbstractGame::firstMove() const {
  for(Move m = m_moveTable; m < m_lastMove; m++) {
    if(((*m) & m_board) == *m) {
      return m;
    }
  }
  return NULL;
}

Move AbstractGame::nextMove(Move m) const {
  while(++m < m_lastMove) {
    if(((*m) & m_board) == *m) {
      return m;
    }
  }
  return NULL;
}

Move AbstractGame::findBestMove(MoveResultArray &moveArray, PlayLevel level) {
  moveArray.clear();
  for(Move m = firstMove(); m; m = nextMove(m)) {
    doMove(m);
    const PositionStatus status = getPositionStatus();
    moveArray.add(MoveWithResult(m, MAKE_STATUS(IS_LOOSERSTATUS(status),PLIESTOEND(status)+1)));
    undoMove(m);
  }
  moveArray.sort();
  return moveArray.selectBestMove(level, m_bricksOnBoard);
}

static int moveWithResultCmp(const MoveWithResult &mr1, const MoveWithResult &mr2) {
  int m1win = IS_WINNERSTATUS(mr1.m_status);
  int m2win = IS_WINNERSTATUS(mr2.m_status);
  if(m1win != m2win) {
    return m2win - m1win;
  }
  int c = PLIESTOEND(mr1.m_status) - PLIESTOEND(mr2.m_status);
  if(c) {
    return m1win ? c : -c;
  }
  return sign(mr1.m_move - mr2.m_move);
}

void MoveResultArray::sort() {
  CompactArray<MoveWithResult>::sort(moveWithResultCmp);
}

void AbstractGame::executeMove(PositionSet markedPositions) {
  Move m = findMove(markedPositions);
  if(m == NULL) {
    AfxMessageBox(format(_T("Illegal move:%s"), sprintbin(markedPositions).cstr()).cstr(), MB_ICONSTOP);
    return;
  }
  doMove(m);
  m_playerInTurn = GETOPPONENT(m_playerInTurn);
}

Move MoveResultArray::selectBestMove(PlayLevel level, unsigned short bricksOnBoard) const {
  switch(level) {
  case LEVEL_BEGINNER    : return selectBeginnerMove(bricksOnBoard);
  case LEVEL_INTERMEDIATE: return selectIntermediateMove(bricksOnBoard);
  case LEVEL_EXPERT      : return selectExpertMove();
  default                : return selectExpertMove();
  }
}

Move MoveResultArray::selectBeginnerMove(unsigned short bricksOnBoard) const {
  if(bricksOnBoard > 3) {
    return select().m_move;
  } else {
    return selectExpertMove();
  }
}

Move MoveResultArray::selectIntermediateMove(unsigned short bricksOnBoard) const {
  if(bricksOnBoard > 6) {
    return select().m_move;
  } else {
    return selectExpertMove();
  }
}

Move MoveResultArray::selectExpertMove() const {
  BitSet set(size());
  for(size_t i = 0; i < size(); i++) {
    const MoveWithResult &mr = (*this)[i];
    if(IS_WINNERSTATUS(mr.m_status)) {
      set.add(i);
    }
  }
  if(!set.isEmpty()) {
    return (*this)[set.select()].m_move;
  }
  int pliesToLoose = 0;
  for(size_t i = 0; i < size(); i++) {
    const MoveWithResult &mr = (*this)[i];
    if(PLIESTOEND(mr.m_status) > pliesToLoose) {
      pliesToLoose = PLIESTOEND(mr.m_status);
    }
  }
  for(size_t i = 0; i < size(); i++) {
    const MoveWithResult &mr = (*this)[i];
    if(PLIESTOEND(mr.m_status) == pliesToLoose) {
      set.add(i);
    }
  }
  return (*this)[set.select()].m_move;
}

String MoveWithResult::toString() const {
  return format(_T("{%-5s}:%s"), setToString(*m_move).cstr(), statusToString(m_status).cstr());
}

String setToString(const PositionSet &s) {
  String result;
  const TCHAR *delimiter = EMPTYSTRING;
  for(int p = 0; p < 32; p++) {
    if(s & (1<<p)) {
      result += format(_T("%s%2d"), delimiter, p);
      delimiter = _T(",");
    }
  }
  return result;
}

String statusToString(PositionStatus status) {
  return format(_T("%-5s") ,(IS_WINNERSTATUS(status) ? _T("Vinder") : _T("Taber")));
}

LineIntersectionKey::LineIntersectionKey(int n, int i1, int i2, int i3, int i4) {
  if(i1 < i2) {
    std::swap(i1,i2);
  }
  if(i3 < i4) {
    std::swap(i3,i4);
  }
  int l1 = i1 * n + i2;
  int l2 = i3 * n + i4;
  if(l1 < l2) {
    std::swap(l1,l2);
  }
  m_hashCode = l1 * n * n + l2;
}

void AbstractGame::paintRegularPolygon(CDC &dc, int edgeCount, int size, const CPoint &start, double startDir) {
  Point2DP p = start;

  dc.MoveTo(p);
  const double dg         = 360.0/edgeCount;
  const double sideLength = sqrt(2.0 * size * size * (1.0 - cos(GRAD2RAD(dg))));
  double dir = startDir;
  for(int i = 0; i < edgeCount; i++, dir += dg) {
    addPosition(dc, p);
    p.x += sideLength * cos(GRAD2RAD(dir));
    p.y += sideLength * sin(GRAD2RAD(dir));
    dc.LineTo(p);
  }
}

bool AbstractGame::addPositionAtIntersection(CDC &dc, int m1, int m2, int m3, int m4) {
  const Point2D &p1 = m_brickPositions[m1];
  const Point2D &p2 = m_brickPositions[m2];
  const Point2D &p3 = m_brickPositions[m3];
  const Point2D &p4 = m_brickPositions[m4];
  const double dx12 = p1.x - p2.x;
  const double dy12 = p1.y - p2.y;

  const double t = ((p4.y - p2.y)*dx12 - (p4.x-p2.x)*dy12) / ((p3.x-p4.x)*dy12 - (p3.y-p4.y)*dx12);
  if(t > 0 && t < 1) {
    const Point2D p = t * p3 + (1-t)*p4;
    const size_t n = m_brickPositions.size();
    for(size_t i = 0; i < n; i++) {
      if(distance(p, m_brickPositions[i]) < 1e-6) {
        return false;
      }
    }
    addPosition(dc, p);
    return true;
  } else {
    return false;
  }
}

void AbstractGame::addPosition(CDC &dc, const Point2D &p) {
/*
  const String label = format(_T("%d"), m_brickPositions.size(), (1<<m_brickPositions.size()));
  dc.TextOut(p.x+10, p.y, label.cstr(), label.length());
*/
  m_brickPositions.add(p);
}

#define POSSIZE 12

int AbstractGame::findPosition(const CPoint &p) const {
  for(int pos = 0; pos < m_positionCount; pos++) {
    if(distance(Point2DP(p), m_brickPositions[pos]) <= POSSIZE) {
      if(isOccupied(pos)) {
        return pos;
      }
      break;
    }
  }
  return -1;
}

void AbstractGame::paintAllPositions(CDC &dc, PositionSet markedPositions) {
  CPen *oldPen = dc.SelectObject(&m_positionPen);

  const size_t n = m_brickPositions.size();
  CBrush *oldBrush = dc.SelectObject(&m_occupiedBrush);
  setTextColor(dc, false);
  for(UINT pos = 0; pos < n; pos++) {
    if(isOccupied(pos) && !SET_CONTAINS(markedPositions, pos)) {
      paintPosition(dc, pos);
    }
  }

  dc.SelectObject(&m_emptyBrush);
  for(UINT pos = 0; pos < n; pos++) {
    if(!isOccupied(pos)) {
      paintPosition(dc, pos);
    }
  }

  if(!SET_ISEMPTY(markedPositions)) {
    dc.SelectObject(&m_markedBrush);
    setTextColor(dc, true);
    for(UINT pos = 0; pos < n; pos++) {
      if(SET_CONTAINS(markedPositions, pos)) {
        paintPosition(dc, pos);
      }
    }
  }
  dc.SelectObject(oldBrush);
  dc.SelectObject(oldPen);
}

void AbstractGame::animateMove(CDC &dc, Move m) {
  for(int t = 0; t < 2; t++) {
    paintPositions(dc, *m, true);
    Sleep(300);
    if(t == 1) return;
    paintPositions(dc, *m, false);
    Sleep(300);
  }
}

void AbstractGame::paintPositions(CDC &dc, PositionSet positions, bool marked) {
  const size_t n = m_brickPositions.size();
  for(UINT pos = 0; pos < n; pos++) {
    if(SET_CONTAINS(positions, pos)) {
      paintPosition(dc, pos, marked);
    }
  }
}

void AbstractGame::paintPosition(CDC &dc, int pos, bool marked) {
  CPen   *oldPen   = dc.SelectObject(&m_positionPen);
  CBrush *oldBrush = dc.SelectObject(marked ? &m_markedBrush : &m_occupiedBrush);

  setTextColor(dc, marked);

  paintPosition(dc, pos);

  dc.SelectObject(oldBrush);
  dc.SelectObject(oldPen);
}

void AbstractGame::paintPosition(CDC &dc, int pos) {
  const Point2D &p = m_brickPositions[pos];
  dc.Ellipse((int)(p.x-POSSIZE),(int)(p.y-POSSIZE),(int)(p.x+POSSIZE),(int)(p.y+POSSIZE));

  if(m_showNumbers && isOccupied(pos)) {
    textOut(dc, (int)(p.x - ((pos>9)?8:4)), (int)(p.y-8), format(_T("%d"), pos));
  }
}

void AbstractGame::setTextColor(CDC &dc, bool marked) {
  if(m_showNumbers) {
    dc.SetTextColor(RGB(255,255,255));
    dc.SetBkColor(marked ? RGB(255,0,0) : RGB(0,0,0));
  }
}
