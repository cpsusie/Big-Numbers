#pragma once

//#define USE_COMPACTSTACK

#if defined(USE_COMPACTSTACK)
#include <CompactStack.h>
#else
#include <Stack.h>
#endif

typedef enum {
  YOU = -1,
  ME  =  1
} Player;

#define OPPONENT(p) ((Player)(-p))

#define BOARDWIDTH  7
#define BOARDHEIGHT 6

#define ISMOVE(m) ((m) >= 0)
#define INITMOVE(m) m = -1

#define MIN_SCORE -999
#define MAX_SCORE 999

typedef char PosMatrix[BOARDHEIGHT][BOARDWIDTH];

typedef char Move;

class Position {
public:
  char m_r : 4;
  char m_c : 4;
  Position(int r = 0, int c = 0) { m_r = r; m_c = c; }
};

class Game;

class Stribe4 {
public:
  Position p[4];
  Stribe4(const Game &g, Move lastmove);
};

class Game {
private:
  bool has4(int r, int c) const;

  bool                      m_gameOver : 1;
  Player                    m_winner   : 2;
  Player                    m_playerInTurn : 2;
  int                       m_score        : 11;
  PosMatrix                 m_pos;
  char                      m_colh[BOARDWIDTH];
  static long               m_evalCount;
  static double             m_evaluationTime;
#if defined(USE_COMPACTSTACK)
  static CompactStack<Game> m_gameStack;
#else
  static Stack<Game>        m_gameStack;
#endif
  static int                m_maxDepth;
  static int                m_bestScore;
  static Move               m_bestMove;
  static PosMatrix          m_posValue[3];
  int maximize(int c_min, int depth);
  int minimize(int c_max, int depth);
  void tryMove(Move m);
  void untryMove();
  int evaluateMe() const;
  int evaluateYou() const;

  int evaluate() const {
    return evaluateMe();
  }

  Move firstMove() const {
    return firstMoveFrw();
  }

  Move nextMove(Move lastMove) const {
    return nextMoveFrw(lastMove);
  }

  Move findMove1(int lookahead);

public:
  void findPosValue(PosMatrix p, Player who) const;
  int colh(int c) const {
    return m_colh[c];
  }

  int pos(int r, int c) const {
    return m_pos[r][c];
  }

  Player playerInTurn() const {
    return m_playerInTurn;
  }

  Player winner() const {
    return m_winner;
  }

  bool gameOver() const {
    return m_gameOver;
  }

  int getBestScore() const {
    return m_bestScore;
  }

  void init(Player startPlayer);
  void doMove(Move m);

  static long getEvalCount() {
    return m_evalCount;
  }

  int getEvalCountPerSecond() const;

  Stribe4 findStribe(Move lastMove) const {
    return Stribe4(*this,lastMove);
  }

  int getBrickCount() const;
  Move findRandomMove() const;
  Move findMove(int lookahead);
  Move firstMoveFrw() const;
  Move nextMoveFrw(Move lastMove) const;
};
