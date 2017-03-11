#include "stdafx.h"
#include <MyUtil.h>
#include "Game.h"

static int def_board[WIDTH][HEIGHT] = { 
    1,  0,  2, -4,  9, -5, -1,  0,
    7,  3, -2, 15,  8,  6, -3, -1,
   -7, -4, -6, -2,  3, -1, -9,  0,
    5,  2,  1,  3,  5,  7,  5,  4,
   -6,  2, -5,  0,  4,  0,  2,  9,
    3,  1, -2,  6,  6,  2, -1, -2,
   -1,  8,  0,  4,  5,  7, -3,  3,
   -4,  1, -3,  1,  4,  2, 10,  0
};

void Game::executeMove(int f) {
  switch(m_playerInTurn) {
  case YOU: doMove(m_currentRow,f,m_yourSum);
            m_currentCol   = f;
            m_playerInTurn = ME;
            break;
  case ME : doMove(f,m_currentCol,m_mySum);
            m_currentRow   = f;
            m_playerInTurn = YOU;
            break;
  }
}

bool Game::isGameOver() const {
  switch(m_playerInTurn) {
  case YOU: return ((Game*)this)->findMoveC(1) < 0;
  case ME : return ((Game*)this)->findMoveR(1) < 0;
  }
  return true;
}

void Game::doMove(int r,int c, int &thisSum) {
  thisSum += m_board[r][c];
  m_used[r][c] = true;
}

void Game::undoMove(int r,int c, int &thisSum) {
  thisSum -= m_board[r][c];
  m_used[r][c] = false;
};

#define UNDEF -9999

int Game::findRow(int s, int &sm) {
  int r,row,res,sm1;
  int max = UNDEF;
  int sum = 0;
  if(!m_depth--) { 
    sm = 0;
    m_depth++;
    return 0;
  };
  for(r = 0; r < HEIGHT; r++) {
    if(m_used[r][s]) continue;
    doMove(r,s,sum);
    if(findCol(r,sm1) >= 0) {
      if((res = (sum-sm1)) > max) {
        max = res;
        row = r;
      }
    } else if(sum > max) {
      max = sum;
      row = r;
    }
    undoMove(r,s,sum);
  }
  m_depth++;
  if(max == UNDEF) return -1;
  sm = max;
  return row;
}

int Game::findCol(int r, int &sm) {
  int s,col,res,sm1;
  int max = UNDEF;
  int sum = 0;
  if(!m_depth--) {
    sm = 0;
    m_depth++;
    return 0;
  }
  for(s = 0; s < WIDTH; s++) {
    if(m_used[r][s]) continue;
    doMove(r,s,sum);
    if(findRow(s,sm1) >= 0)  {
      if((res = (sum-sm1)) > max) {
        max = res;
        col = s;
      }
    } else if(sum > max) {
      max = sum;
      col = s;
    }
    undoMove(r,s,sum);
  }
  m_depth++;
  if(max == UNDEF) return -1;
  sm = max;
  return col;
}

int Game::findMoveR(int look) {
  int s;
  m_depth = look;
  return findRow(m_currentCol,s);
}

int Game::findMoveC(int look) {
  int s;
  m_depth = look;
  return findCol(m_currentRow,s);
}

void Game::init() { 
  for(int r = 0; r < HEIGHT; r++) {
    for(int s = 0; s < WIDTH; s++) {
      m_used[r][s]  = false;
      m_board[r][s] = def_board[r][s];
    }
  }
  m_currentRow = 0; m_currentCol = 1;
  m_used[m_currentRow][m_currentCol] = true;
  m_playerInTurn = YOU; // ((rand() % 2) == 1) ? YOU : ME;
  m_mySum = m_yourSum = 0;
  scramble();
}

class FieldInfo {
public:
  int  m_v;
  bool m_used;
  FieldInfo() {}
  FieldInfo(int v, bool used) : m_v(v), m_used(used) {
  }
};

void Game::scramble() { 
  CompactArray<FieldInfo> tmp(HEIGHT*WIDTH);
  for(int r = 0; r < HEIGHT; r++) {
    for(int s = 0; s < WIDTH; s++) { 
      tmp.add(FieldInfo(m_board[r][s], m_used[r][s]));
    }
  }
  tmp.shuffle();
  for(int r = 0, i = 0; r < HEIGHT; r++) {
    for(int s = 0; s < WIDTH; s++, i++) { 
      m_board[r][s] = tmp[i].m_v;
      m_used[r][s]  = tmp[i].m_used;
    }
  }

  for(int r = 0; r < HEIGHT; r++) {
    int sc;
    bool rowUnused = false, rowUsed = false;
    for(int s = 0; s < WIDTH; s++) { 
      if(m_used[r][s]) { 
        rowUsed = true;
        sc = s;
      } else {
        rowUnused = true;
      }
    }
    if(rowUsed && rowUnused) { 
      m_currentCol = sc;
      m_currentRow = r;
      break;
    }
  }
}
