#pragma once

#include <CompactArray.h>
#include <Math/Rectangle2D.h>

typedef UINT PositionSet;

typedef const PositionSet *Move;

#define SET_CONTAINS(set, pos) (((set) & (1<<(pos))) != 0)
#define SET_ADD(     set, pos)  set |=  (1<<(pos))
#define SET_REMOVE(  set, pos)  set &= ~(1<<(pos))
#define SET_ISEMPTY( set)       ((set) == 0)

String setToString(const PositionSet &s);

#define PS1(p) (1 << (p))
#define PS2(p1,p2) (PS1(p1) | PS1(p2))

typedef enum {
  COMPUTERPLAYER
 ,HUMANPLAYER
} Player;

#define GETOPPONENT(player) (Player)(1 - (player))

typedef BYTE PositionStatus;

#define MAKE_STATUS(win, pliesToEnd)    (((win)?0x40:0) | (pliesToEnd))
#define STATUS_DEFINED( status) ((status) & 0x80)
#define IS_WINNERSTATUS(status) ((status) & 0x40)
#define IS_LOOSERSTATUS(status) (STATUS_DEFINED(status) && !IS_WINNERSTATUS(status))
#define PLIESTOEND(status)      ((status) & 0x3f)

String statusToString(PositionStatus status);

typedef enum {
  PENTAGON
 ,HEXAGON
 ,OCTAGON
} GameType;

typedef enum {
  LEVEL_BEGINNER
 ,LEVEL_INTERMEDIATE
 ,LEVEL_EXPERT
} PlayLevel;

void line(CDC &dc, const Point2D &p1, const Point2D &p2);

class MoveWithResult {
public:
  Move           m_move;
  PositionStatus m_status;
  MoveWithResult() : m_move(nullptr), m_status(0) {
  }
  MoveWithResult(Move move, PositionStatus status) : m_move(move), m_status(status) {
  }
  String toString() const;
};

class MoveResultArray : public CompactArray<MoveWithResult> {
private:
  Move selectBeginnerMove(    unsigned short bricksOnBoard) const;
  Move selectIntermediateMove(unsigned short bricksOnBoard) const;
  Move selectExpertMove() const;
public:
  Move selectBestMove(PlayLevel level, unsigned short bricksOnBoard) const;
  void sort();
};

class LineIntersectionKey {
private:
  unsigned long m_hashCode;
public:
  LineIntersectionKey() : m_hashCode(0) {
  }
  LineIntersectionKey(int n, int i1, int i2, int i3, int i4);
  bool operator==(const LineIntersectionKey &k1) const {
    return m_hashCode == k1.m_hashCode;
  }
  unsigned long hashCode() const {
    return m_hashCode;
  }
};

class AbstractGame {
protected:
  const GameType        m_gameType;
  const unsigned short  m_positionCount;
  const UINT            m_moveCount;
  PositionStatus       *m_statusTable;
  const PositionSet    *m_moveTable;
  const PositionSet    *m_lastMove;
  const PositionSet    *m_lastTakeOneMove;
  Player                m_playerInTurn;
  PositionSet           m_board;
  unsigned short        m_bricksOnBoard;
  Point2DArray          m_brickPositions;
  bool                  m_showNumbers;
  CPen                  m_linePen, m_positionPen;
  CBrush                m_emptyBrush, m_occupiedBrush, m_markedBrush;

  friend class MoveWithResult;

  Move firstMove() const;
  Move nextMove(Move m)  const;
  Move findMove(PositionSet markedPositions) const;

  void doMove(  Move m);
  void undoMove(Move m);

  PositionStatus getPositionStatus();

  bool addPositionAtIntersection(CDC &dc, int m1, int m2, int m3, int m4);
  void addPosition(              CDC &dc, const Point2D &p);
  void paintRegularPolygon(      CDC &dc, int edgeCount, int size, const CPoint &start, double startDir);
  void paintPosition(            CDC &dc, int pos);
  void paintAllPositions(        CDC &dc, PositionSet markedPositions);
  void paintPositions(           CDC &dc, PositionSet positions, bool marked);
  void setTextColor(             CDC &dc, bool marked);

public:

  AbstractGame(GameType gameType, UINT positionCount, PositionStatus *statusTable, unsigned int moveCount, const PositionSet *moveTable);
  virtual ~AbstractGame() {
  }
  void init(Player startPlayer);

  Move findBestMove(MoveResultArray &moveArray, PlayLevel level);

  GameType getGameType() const {
    return m_gameType;
  }

  bool isOccupied(UINT pos) const {
    return SET_CONTAINS(m_board, pos);
  }

  bool isHumanToMove() const {
    return m_playerInTurn == HUMANPLAYER;
  }

  bool isGameOver() const {
    return SET_ISEMPTY(m_board);
  }

  bool isLegalMarks(PositionSet markedPositions) const {
    return findMove(markedPositions) != nullptr;
  }

  void setShowNumbers(bool show) {
    m_showNumbers = show;
  }

  virtual void paint(CDC &dc, const CRect &rect, PositionSet markedPositions) = 0;
  void paintPosition(CDC &dc, int pos, bool marked);
  int findPosition(const CPoint &p) const;
  void animateMove(CDC &dc, Move m);

  void executeMove(PositionSet markedPositions);
};

class GamePentagon : public AbstractGame {
private:
  static const PositionSet moveTable[30];
  static PositionStatus    statusTable[0x400];

public:
  GamePentagon() : AbstractGame(PENTAGON, 10, statusTable, ARRAYSIZE(moveTable), moveTable) {
  }

  void paint(CDC &dc, const CRect &rect, PositionSet markedPositions);
};

class GameHexagon : public AbstractGame {
private:
  static const PositionSet moveTable[67];
  static PositionStatus    statusTable[0x80000];

public:
  GameHexagon() : AbstractGame(HEXAGON, 19, statusTable, ARRAYSIZE(moveTable), moveTable) {
  }

  void paint(CDC &dc, const CRect &rect, PositionSet markedPositions);
};

class GameOctagon : public AbstractGame {
private:
  static const PositionSet moveTable[89];
  static bool              gotStatus;
  static PositionStatus    statusTable[0x2000000];

public:
  GameOctagon();
  ~GameOctagon();

  void paint(CDC &dc, const CRect &rect, PositionSet markedPositions);
};
