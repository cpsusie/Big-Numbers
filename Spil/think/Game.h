#pragma once

#define BSIZE 8

#define WIDTH  BSIZE
#define HEIGHT BSIZE

#define YOU 1
#define ME -1

class Game {
private:
  int m_depth;
  void scramble();
  void doMove(   int r, int s, int &thisSum);
  void undoMove( int r, int s, int &thisSum);
  int  findRow(  int s, int &sm);
  int  findCol(  int r, int &sm);
public:
  int  m_board[WIDTH][HEIGHT];
  bool m_used[WIDTH][HEIGHT];
  int  m_playerInTurn;
  int  m_mySum,m_yourSum;
  int  m_currentRow,m_currentCol;
  void init();
  int  findMoveR( int look = 6);
  int  findMoveC( int look = 6);
  void executeMove(int f); // f = row or col
  bool isGameOver() const;
};
