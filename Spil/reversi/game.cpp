#include "stdafx.h"

void GameState::init() {
  for(int row = 0; row < BSIZE; row++) {
    for(int col = 0; col < BSIZE; col++) {
      board[row][col] = 0;
    }
  }
  board[3][3] = board[4][4] = YOU;
  board[3][4] = board[4][3] = ME;
//  board[3][3] = board[3][4] = YOU;
//  board[4][3] = board[4][4] = ME;
}


void GameState::getBoardCount(int &me, int &you) {
  me = you = 0;
  for(int row = 0; row < BSIZE; row++) {
    for(int col = 0; col < BSIZE; col++) {
      switch(board[row][col]) {
      case ME : me++ ; break;
      case YOU: you++; break;
      }
    }
  }
}

static Field fs[] = {
  {0,0},
  {3,2}, {4,2}, {5,2}, {5,3}, {5,4}, {5,5},
  {4,5}, {3,5}, {2,5}, {2,4}, {2,3}, {2,2},
  {2,1}, {3,1}, {4,1}, {5,1}, {6,1}, {6,2},
  {6,3}, {6,4}, {6,5}, {6,6}, {5,6}, {4,6},
  {3,6}, {2,6}, {1,6}, {1,5}, {1,4}, {1,3},
  {1,2}, {1,1}, {1,0}, {2,0}, {3,0}, {4,0},
  {5,0}, {6,0}, {7,0}, {7,1}, {7,2}, {7,3},
  {7,4}, {7,5}, {7,6}, {7,7}, {6,7}, {5,7},
  {4,7}, {3,7}, {2,7}, {1,7}, {0,7}, {0,6},
  {0,5}, {0,4}, {0,3}, {0,2}, {0,1}, {0,0},
};

int GameState::findField(int row, int col) {
  for(int f = 1; f < ARRAYSIZE(fs); f++) {
    if(fs[f].row == row && fs[f].col == col) {
      return f;
    }
  }
  return -1;
}

#define MIN_SCORE -9999
#define MAX_SCORE 9999

static int fieldValue[BSIZE][BSIZE] = {
  100,  0, 25, 10, 10, 25,  0,100,
    0,-75, -5, -5, -5, -5,-75,  0,
   25, -5, 15,  1,  1, 15, -5, 25,
   10, -5,  1,  1,  1,  1, -5, 10,
   10, -5,  1,  1,  1,  1, -5, 10,
   25, -5, 15,  1,  1, 15, -5, 25,
    0,-75, -5, -5, -5, -5,-75,  0,
  100,  0, 25, 10, 10, 25,  0,100
};

int GameState::evaluateStart(int who) {
  int sum = 0;
  int *b = &board[0][0];
  int countOpponent = 0;
  for(int i = 0; i < NFIELD; i++) {
    if(*(b++) == -who) {
      countOpponent++;
    }
  }

  if(countOpponent == 0)
    return MAX_SCORE;

  if(board[0][0] != 0) {
    fieldValue[1][1] = 75;
    fieldValue[1][0] = fieldValue[0][1] = 75;
  } else {
    fieldValue[1][1] = -75;
    fieldValue[1][0] = fieldValue[0][1] = -30;
  }
  if(board[BSIZE-1][0] != 0) {
    fieldValue[BSIZE-2][1] = 75;
    fieldValue[BSIZE-2][0] = fieldValue[BSIZE-1][1] = 75;
  } else {
    fieldValue[BSIZE-2][1] = -75;
    fieldValue[BSIZE-2][0] = fieldValue[BSIZE-1][1] = -30;
  }
  if(board[0][BSIZE-1] != 0) {
    fieldValue[1][BSIZE-2] = 75;
    fieldValue[1][BSIZE-1] = fieldValue[0][BSIZE-2] = 75;
  } else {
    fieldValue[1][BSIZE-2] = -75;
    fieldValue[1][BSIZE-1] = fieldValue[0][BSIZE-2] = -30;
  }
  if(board[BSIZE-1][BSIZE-1] != 0) {
    fieldValue[BSIZE-2][BSIZE-2] = 75;
    fieldValue[BSIZE-2][BSIZE-1] = fieldValue[BSIZE-1][BSIZE-2] = 75;
  } else {
    fieldValue[BSIZE-2][BSIZE-2] = -75;
    fieldValue[BSIZE-2][BSIZE-1] = fieldValue[BSIZE-1][BSIZE-2] = -30;
  }
  int *s = &fieldValue[0][0];
  b = &board[0][0];
  for(int i = 0; i < NFIELD; i++) {
    sum += *(b++) * *(s++);
  }

/*
  for(int r = 0; r < BSIZE; r++) {
    for(int c = 0; c < BSIZE; c++)
      printf("%3d ",fieldValue[r][c]);
    printf("\n");
  }
  fflush(stdout);
*/
  return who * sum;
}

int GameState::evaluateEnd(int who) {
  int sum = 0;
  int *b = &board[0][0];
  for(int i = 0;i < NFIELD;i++) {
    sum += *(b++);
  }
  return who * sum;
}


void GameState::setEvaluate() {
  int me,you;
  getBoardCount(me, you);
  endphase = (me + you > NFIELD - 12);
}

int GameState::evaluate(int who) {
  return endphase ? evaluateEnd(who) : evaluateStart(who);
}

static Field ds[] = {
  {-1,-1},{-1,0},{-1,1},
  { 0,-1},       { 0,1},
  { 1,-1},{ 1,0},{ 1,1}
};

int GameState::findTurners(int who, int row, int col, Move &m) {
  int count = 0;
  for(int dir = 0; dir < ARRAYSIZE(ds); dir++) {
    const Field &f = ds[dir];
    int r = row + f.row;
    int c = col + f.col;
    for(int dircount = count; r >= 0 && r < BSIZE && c >= 0 && c < BSIZE; dircount++) {
      int b = board[r][c];
      if(b != -who) {
        if(b == who) {
          count = dircount;
        }
        break;
      }

      m.turners[dircount].row = r;
      m.turners[dircount].col = c;
      r += f.row;
      c += f.col;
    }
  }
  m.count = count;
  return count;
}

bool GameState::checkMove(int who, int fieldIndex, Move &m) {
  int row = fs[fieldIndex].row;
  int col = fs[fieldIndex].col;

  if(board[row][col]) {
    return false; // field already occupied
  }
  if(findTurners(who,row,col,m) > 0) {
    m.fieldIndex = fieldIndex;
    return true;
  } else {
    return false;
  }
}

#define MOVE_PASS 0

int GameState::firstMove( int who, Move &m ) {
  for(int i = 1; i < ARRAYSIZE(fs); i++) {
    if(checkMove(who,i,m)) {
      return i;
    }
  }
  m.fieldIndex = MOVE_PASS;
  return MOVE_PASS;
}

int GameState::nextMove( int who, Move &m  ) {
  if(m.fieldIndex != MOVE_PASS) {
    for(int i = m.fieldIndex + 1; i < ARRAYSIZE(fs); i++) {
      if(checkMove(who,i,m)) {
        return i;
      }
    }
  }
  return -1;
}

void GameState::doMove( int who, Move &m) {
  if(m.fieldIndex != MOVE_PASS) {
    for(int i = 0; i < m.count;i++) {
      setField(m.turners[i].row,m.turners[i].col,who);
    }
    setField(fs[m.fieldIndex].row,fs[m.fieldIndex].col,who);
  }
}

bool GameState::executeMove(int who, int fieldIndex) {
  Move m;
  if(!checkMove(who,fieldIndex,m)) {
    return false;
  }
  doMove(who,m);
  return true;
}

/* --------------- A L P H A - B E T A  P R U N I N G -------- */

int MoveEvaluater::alfabeta(int alfa, int beta, int depth, int who) {
  if(depth >= maxdepth) {
    return state.evaluate(who);
  }

  push();
  Move m;
  int v,e,p,r;
  bool done = false;
  for(v = alfa, p = state.firstMove(who,m); p >= 0 && !done; p = state.nextMove(who,m)) {
    state.doMove(who, m);

    if((e = -alfabeta(-beta,-v,depth+1, -who)) > v) {
      v = e;
      r = p;
    }

    if(v >= beta) {
      done = true;
    }

    restore();
  };
  pop();

  if(depth == 0 && v > bestscore) {
    bestscore = v;
    bestmove  = r;
  }
  return v;
}

int MoveEvaluater::findmove(int who, int lookahead, const GameState &g) {
  bestscore = MIN_SCORE;
  bestmove  = 0;
  maxdepth  = lookahead;
  state     = g;
  stacktop  = 0;
  state.setEvaluate();

  alfabeta(MIN_SCORE,MAX_SCORE,0, who);
  return bestmove;
}
