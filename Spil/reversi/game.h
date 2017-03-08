#pragma once

#define BSIZE 8
#define NFIELD (BSIZE*BSIZE)

#define ME  -1
#define YOU 1

class Field {
public:
  char row,col;
};

#define MAXTURNERS (4*(BSIZE-2))
class Move {
public:
  int fieldIndex;
  Field turners[MAXTURNERS];
  int count;
};

class GameState {
private:
  int board[BSIZE][BSIZE];
  bool endphase;
  int  evaluateStart( int who );
  int  evaluateEnd(   int who );
  int  findTurners(   int who, int row, int col, Move &m);
public:
  bool checkMove(     int who, int fieldIndex, Move &m);
  void init();
  void setEvaluate();
  void getBoardCount( int &me, int &you);
  int  firstMove(     int who, Move &m );
  int  nextMove(      int who, Move &m );
  void doMove(        int who, Move &m );
  bool executeMove(   int who, int fieldIndex);
  int  evaluate(      int who);
  void setField(      int row, int col, int who) { board[row][col] = who;  }
  int  getField(      int row, int col)          { return board[row][col]; }
  static int findField(int row, int col);
};

#define STACKSIZE 16
class MoveEvaluater {
private:
  int maxdepth;
  int bestscore,bestmove;
  GameState gamestack[STACKSIZE];
  GameState state;
  int stacktop;
  void push()    { gamestack[stacktop++] = state; }
  void pop()     { state = gamestack[--stacktop]; }
  void restore() { state = gamestack[stacktop-1]; }
  int alfabeta(int alfa, int beta, int depth, int who);
public:
  int findmove(int who, int lookahead, const GameState &g);
  int getScore() const { return bestscore; }
};
