#include "stdafx.h"
#include "Game.h"

BYTE Brick::attr[FIELDCOUNT];

#define MIN_SCORE -9999
#define MAX_SCORE 9999

class InitAttributes {
public:
  InitAttributes();
};

InitAttributes::InitAttributes() {
  for(int i = 0; i < FIELDCOUNT; i++) {
    Brick::attr[i] = ((~i & 0xf) << 4) | i;
  }
}

static InitAttributes dummy;

bool Field::isValid() const {
  return (m_row >= 0) && (m_row < ROWCOUNT) && (m_col >= 0) && (m_col < COLCOUNT);
}

String Field::toString() const {
  return format(_T("%d,%d"), m_row, m_col);
}

const Move  NOMOVE(0,0,NOBRICK);
const Field NOFIELD = { -1,-1 };

Move::Move() {
  m_field.m_row = 0;
  m_field.m_col = 0;
  m_brick       = NOBRICK;
}

Move::Move(BYTE r, BYTE c, char brick) {
  m_field.m_row = r;
  m_field.m_col = c;
  m_brick       = brick;
}

static String getString(Tokenizer &tok) {
  if(!tok.hasNext()) {
    throwException(_T("Unexpected end of string"));
  }
  return tok.next();
}

static int getInt(Tokenizer &tok) {
  const String s = getString(tok);
  int value;
  if(_stscanf(s.cstr(), _T("%d"), &value) != 1) {
    throwException(_T("Expected int:<%s>"), s.cstr());
  }
  return value;
}

Move::Move(const String &s) {
  Tokenizer tok(s, _T(","));
  m_field.m_row = getInt(tok);
  m_field.m_col = getInt(tok);
  m_brick       = getInt(tok);
  if(tok.hasNext()) {
    throwException(_T("Expected end-of-line:<%s>"), s.cstr());
  }
  if(m_field.m_row < 0 || m_field.m_col >= ROWCOUNT) {
    throwException(_T("Invalid row:<%s>"), s.cstr());
  }
  if(m_field.m_col < 0 || m_field.m_col >= COLCOUNT) {
    throwException(_T("Invalid column:<%s>"), s.cstr());
  }
  if(m_brick < 0 || m_brick >= FIELDCOUNT) {
    throwException(_T("Invalid brick:<%s>"), s.cstr());
  }
}

String Move::toString() const {
  return format(_T("%d,%d,%d"), m_field.m_row, m_field.m_col, m_brick);
}

Game::Game() {
  newGame(true, HUMAN_PLAYER);
}

Game::Game(const String &s) {
  Tokenizer tok(s, _T("\n"));
  int intValue;
  intValue = getInt(tok);
  if(intValue != COMPUTER_PLAYER && intValue != HUMAN_PLAYER) {
    throwException(_T("Unexepected value for startplayer:%d"), intValue);
  }
  const Player startPlayer = (Player)intValue;
  intValue = getInt(tok);
  const bool coloredGame = (intValue != 0);

  CompactArray<Move> tmpList;
  while(tok.hasNext()) {
    tmpList.add(Move(getString(tok)));
  }
  newGame(coloredGame, startPlayer);
  for(size_t i = 0; i < tmpList.size(); i++) {
    executeMove(tmpList[i]);
  }
}

String Game::toString() const {
  String result;
  result = format(_T("%d\n"), getStartPlayer());
  result += format(_T("%d\n"), isColored() ? 1 : 0);
  for(size_t i = 0; i < m_moveList.size(); i++) {
    result += format(_T("%s\n"),m_moveList[i].toString().cstr());
  }
  return result;
}

void Game::newGame(bool colored, Player startPlayer) {
  m_colored      = colored;
  m_startPlayer  = startPlayer;
  m_moveList.clear();

  for(int r = 0; r < ROWCOUNT; r++) {
    for(int c = 0; c < COLCOUNT; c++) {
      m_state.m_board[r][c] = 0;
    }
  }
  m_state.m_playerInTurn = m_startPlayer;
  m_state.m_unused       = 0xffff;
  m_state.m_gameOver     = false;
  m_state.m_hasWinner    = false;
};

#define BRICKUNUSED(b) ((1 << (b)) & m_state.m_unused) 
#define ONBOARD(r,c)   ((r) >= 0 && (r) < ROWCOUNT && (c) >= 0 && (c) < COLCOUNT)

int Game::firstBrick() const {
  if(m_colored) {
    if(m_state.m_playerInTurn == COMPUTER_PLAYER) {
      for(int b = FIELDCOUNT/2; b < FIELDCOUNT; b++) {
        if(BRICKUNUSED(b)) {
          return b;
        }
      }
    } else {
      for(int b = 0; b < FIELDCOUNT/2; b++) {
        if(BRICKUNUSED(b)) {
          return b;
        }
      }
    }
  } else {
    for(int b = 0; b < FIELDCOUNT; b++) {
      if(BRICKUNUSED(b)) {
        return b;
      }
    }
  }
  return NOBRICK;
}

bool Game::nextBrick(Move &m) const {
  if(m_colored) {
    if(m_state.m_playerInTurn == COMPUTER_PLAYER) {
      for(int b = m.m_brick+1; b < FIELDCOUNT; b++) {
        if(BRICKUNUSED(b)) {
          m.m_brick = b;
          return true;
        }
      }
    } else {
      for(int b = m.m_brick+1; b < FIELDCOUNT/2; b++) {
        if(BRICKUNUSED(b)) {
          m.m_brick = b;
          return true;
        }
      }
    }
  } else {
    for(int b = m.m_brick+1; b < FIELDCOUNT; b++) {
      if(BRICKUNUSED(b)) {
        m.m_brick = b;
        return true;
      }
    }
  }
  return false;
}

bool Game::nextPos(Field &f) const {
  for(int c = f.m_col+1; c < COLCOUNT; c++) {
    if(m_state.m_board[f.m_row][c] == 0) {
      f.m_col = c;
      return true;
    }
  }
  for(int r = f.m_row + 1; r < ROWCOUNT; r++) {
    for(int c = 0; c < COLCOUNT; c++) {
      if(m_state.m_board[r][c] == 0) {
        f.m_row = r;
        f.m_col = c;
        return true;
      }
    }
  }
  return false;
}

Move Game::firstMove() const {
  if(m_state.m_gameOver) {
    return NOMOVE;
  }
  for(int r = 0; r < ROWCOUNT; r++) {
    for(int c = 0; c < COLCOUNT; c++) {
      if(m_state.m_board[r][c] == 0) {
        return Move(r,c,firstBrick());
      }
    }
  }
  return NOMOVE;
}

void Game::nextMove(Move &m) const {
  if(!nextBrick(m)) {
    if(nextPos(m.m_field)) {
      m.m_brick = firstBrick();
    } else {
      m = NOMOVE;
    }
  }
}

void Game::doMove(const Move &m, int lookahead) {
  m_state.m_board[m.m_field.m_row][m.m_field.m_col] = Brick::attr[m.m_brick];
  m_state.m_unused &= ~(1 << m.m_brick); 
  if(isWinnerMove(m)) {
    m_state.m_gameOver  = true;
    m_state.m_hasWinner = true;
    m_state.m_score     = m_state.m_playerInTurn * (MAX_SCORE - lookahead);
  } else if(m_state.m_unused == 0) {
    m_state.m_gameOver = true;
  } else {
    m_state.m_playerInTurn = OPPONENT(m_state.m_playerInTurn);
    m_state.m_score    = 0;
  }
}

void Game::executeMove(const Move &m) {
  if((m.m_brick < 0) || (m.m_brick >= FIELDCOUNT) || brickUsed(m.m_brick)) {
    throwException(_T("%s not a valid move"), m.toString().cstr());
  }
  if(isColored() && (getBrickOwner(m.m_brick) != getPlayerInTurn())) {
    throwException(_T("%s not a valid move (invalid owner)"), m.toString().cstr());
  }
  if(!m.m_field.isValid() || !isEmpty(m.m_field)) {
    throwException(_T("%s not a valid move (field not empty)"), m.toString().cstr());
  }
  doMove(m);
  m_moveList.add(m);
}

bool Game::isSelectableBrick(int b) const {
  return brickUnUsed(b) && (!isColored() || (getBrickOwner(b) == getPlayerInTurn()));
}

int Game::getBrickOnField(const Field &f) const {
  if(isEmpty(f)) {
    return NOBRICK;
  }
  const BYTE attr = m_state.m_board[f.m_row][f.m_col];
  for(int b = 0; b < FIELDCOUNT; b++) {
    if(Brick::attr[b] == attr) {
      return b;
    }
  }
  throwException(_T("Unknown brick on field %s:%s"), f.toString().cstr(), sprintbin(attr).cstr());
  return NOBRICK;
}

static Field createField(int r, int c) {
  Field f;
  f.m_row = r;
  f.m_col = c;
  return f;
}

static FieldArray getFieldsInRow(int r) {
  FieldArray result;
  for(int c = 0; c < COLCOUNT; c++) result.add(createField(r, c));
  return result;
}

static FieldArray getFieldsInColumn(int c) {
  FieldArray result;
  for(int r = 0; r < ROWCOUNT; r++) result.add(createField(r, c));
  return result;
}

static FieldArray getFieldsInDiag1() {
  FieldArray result;
  for(int r = 0; r < ROWCOUNT; r++) result.add(createField(r, r));
  return result;
}

static FieldArray getFieldsInDiag2() {
  FieldArray result;
  for(int r = 0; r < ROWCOUNT; r++) result.add(createField(r, COLCOUNT-1-r));
  return result;
}

FieldArray Game::getWinnerFields() const {
  FieldArray empty;
  if(!m_state.m_gameOver || !m_state.m_hasWinner) {
    return empty;
  }
  const Move &m = m_moveList.last();
  const int r = m.m_field.m_row;
  const int c = m.m_field.m_col;

  if(m_state.m_board[r][0] & m_state.m_board[r][1] & m_state.m_board[r][2] & m_state.m_board[r][3]) {
    return getFieldsInRow(r);
  }
  if(m_state.m_board[0][c] & m_state.m_board[1][c] & m_state.m_board[2][c] & m_state.m_board[3][c]) {
    return getFieldsInColumn(c);
  }

  if((r == c) && (m_state.m_board[0][0] & m_state.m_board[1][1] & m_state.m_board[2][2] & m_state.m_board[3][3])) {
    return getFieldsInDiag1();
  }
  if((r == ROWCOUNT-1-c) && (m_state.m_board[0][3] & m_state.m_board[1][2] & m_state.m_board[2][1] & m_state.m_board[3][0])) {
    return getFieldsInDiag2();
  }
  return empty;
}

int Game::getBrickCount() const {
  int count = 0;
  for(int r = 0; r < ROWCOUNT; r++) {
    for(int c = 0; c < COLCOUNT; c++) {
      if(m_state.m_board[r][c] != 0) {
        count++;
      }
    }
  }
  return count;
}

bool Game::isWinnerMove(const Move &m) const {
  int r = m.m_field.m_row;
  int c = m.m_field.m_col;

  if( (m_state.m_board[r][0] & m_state.m_board[r][1] & m_state.m_board[r][2] & m_state.m_board[r][3])
   || (m_state.m_board[0][c] & m_state.m_board[1][c] & m_state.m_board[2][c] & m_state.m_board[3][c])) {
    return true;
  }

  if(r == c) {
    return (m_state.m_board[0][0] & m_state.m_board[1][1] & m_state.m_board[2][2] & m_state.m_board[3][3]) ? true : false;
  } else if((r == ROWCOUNT-1-c) && (m_state.m_board[0][3] & m_state.m_board[1][2] & m_state.m_board[2][1] & m_state.m_board[3][0])) {
    return true;
  }

  return false;
}

Move Game::findRandomMove() const {
  if(m_state.m_unused == 0) {
    return NOMOVE;
  }
  int r,c,b;
  for(;;) {
    r = rand() % ROWCOUNT;
    c = rand() % COLCOUNT;
    if(m_state.m_board[r][c] == 0) {
      break;
    }
  }
  for(;;) {
    if(m_colored) {
      if(m_state.m_playerInTurn == COMPUTER_PLAYER) {
        b = rand() % (FIELDCOUNT/2) + FIELDCOUNT/2;
      } else {
        b = rand() % (FIELDCOUNT/2);
      }
    } else {
      b = rand() % FIELDCOUNT;
    }
    if(BRICKUNUSED(b)) {
      break;
    }
  }
  return Move(r,c,b);
}


/* --------------- A L P H A - B E T A  P R U N I N G -------- */

int MoveEvaluator::maximize(int c_min, int depth) {
  if(depth >= m_maxDepth) {
    m_evalCount++;
    return m_game.evaluate();
  } else {
    Move p = m_game.firstMove();
    Move r;
    if(!p.isMove()) {
      m_evalCount++;
      return m_game.evaluate();
    }
    bool done = false;
    push();
    int v = MIN_SCORE;
    for(; p.isMove() && !done; m_game.nextMove(p)) {
      m_game.doMove(p,depth);

      const int e = minimize(v, depth+1);
      if(e > v) {
        v = e;
        r = p;
      }

      if(v >= c_min) {
        done = true;
      }

      restore();
    }
    pop();
    if(v > m_bestScore && depth == 0) {
      m_bestScore = v;
      m_bestMove  = r;
    }
    return v;
  }
}

int MoveEvaluator::minimize(int c_max, int depth) {
  if(depth >= m_maxDepth) {
    m_evalCount++;
    return m_game.evaluate();
  } else {
    Move p = m_game.firstMove();
    if(!p.isMove()) {
      m_evalCount++;
      return m_game.evaluate();
    }
    bool done = false;
    push();
    int v = MAX_SCORE;
    for(; p.isMove() && !done; m_game.nextMove(p)) {
      m_game.doMove(p,depth);

      const int e = maximize(v, depth+1);
      if(e < v) {
        v = e;
      }

      if(v <= c_max) {
        done = true;
      }

      restore();
    }
    pop();
    return v;
  }
}

Move MoveEvaluator::findMove(int lookahead) {
  m_bestScore = MIN_SCORE;
  m_bestMove  = NOMOVE;
  m_maxDepth  = lookahead;
  m_stackTop  = 0;
  m_evalCount = 0;

  if(m_game.getBrickCount() < 3) { // just to speed up first Move
    m_bestScore = 0;
    return m_game.findRandomMove();
  }

  maximize(MAX_SCORE, 0);
  if(!m_bestMove.isMove()) {
    return m_game.findRandomMove();
  } else {
    return m_bestMove;
  }
}

