#include "stdafx.h"
#include <Thread.h>
#include "Game.h"

long               Game::m_evalCount;
double             Game::m_evaluationTime;
#ifdef USE_COMPACTSTACK
CompactStack<Game> Game::m_gameStack;
#else
Stack<Game>        Game::m_gameStack;
#endif
int                Game::m_maxDepth;
int                Game::m_bestScore;
Move               Game::m_bestMove;
PosMatrix          Game::m_posValue[3];

void Game::init(Player startPlayer) {
  memset(m_pos,0,sizeof(m_pos));
  memset(m_colh,0,sizeof(m_colh));
  m_winner         = (Player)0;
  m_score          = 0;
  m_playerInTurn   = startPlayer;
  m_gameOver       = false;
  m_evalCount      = 0;
  m_evaluationTime = 0;
}

#define ONBOARD(r,c) ((r)>=0 && (r) < BOARDHEIGHT && (c) >= 0 && (c) < BOARDWIDTH)

void Game::findPosValue(PosMatrix p, Player who) const {
  who = OPPONENT(who);
  memset(p,0,sizeof(PosMatrix));
  for(int r = 0; r < BOARDHEIGHT; r++) {
    for(int c = 0; c < BOARDWIDTH; c++) {
      char d[3][3];
      if(m_pos[r][c] == who) {
        continue;
      }
      for(int dr = -1; dr < 2; dr++) {
        for(int dc = -1; dc < 2; dc++) {
          char &dp = d[dr+1][dc+1];
          dp = 0;
          for(int i = 0, tr = r+dr, tc = c+dc; i < 3 && ONBOARD(tr,tc) && m_pos[tr][tc] != who; i++, tr+=dr, tc+=dc) {
            dp++;
          }
        }
      }
      char tmp, &pp = p[r][c];
      pp = 0;
      tmp = d[0][0] + d[2][2] - 2; pp += max(0, tmp); /* \ */
      tmp = d[2][0] + d[0][2] - 2; pp += max(0, tmp); /* / */
      tmp = d[1][0] + d[1][2] - 2; pp += max(0, tmp); /* - */
      tmp = d[0][1] + d[2][1] - 2; pp += max(0, tmp); /* | */
    }
  }
}

#define C_POS(r,c) (m_pos[r][c]==cm)

#define C_R0 C_POS(r,0)
#define C_R1 C_POS(r,1)
#define C_R2 C_POS(r,2)
#define C_R3 C_POS(r,3)
#define C_R4 C_POS(r,4)
#define C_R5 C_POS(r,5)
#define C_R6 C_POS(r,6)

#define C_C0 C_POS(0,c)
#define C_C1 C_POS(1,c)
#define C_C2 C_POS(2,c)
#define C_C3 C_POS(3,c)
#define C_C4 C_POS(4,c)
#define C_C5 C_POS(5,c)

bool Game::has4(int r, int c) const {
  const char *p = &m_pos[r][c];
  const char cm = *p;
  switch(r) { // check vertical stribe
  case 3:
    if(C_C2 && C_C1 && C_C0) return true;
    break;
  case 4:
    if(C_C3 && C_C2 && C_C1) return true;
    break;
  case 5:
    if(C_C4 && C_C3 && C_C2) return true;
    break;
  }
  switch(c) { // check horinzontal stribe
  case 0:
    if(C_R1 && C_R2 && C_R3) return true;
    break;
  case 1:
    if(C_R2 && C_R3 && (C_R0 || C_R4)) return true;
    break;
  case 2:
    if(C_R1 && C_R3 && (C_R0 || C_R4) || C_R3 && C_R4 && C_R5) return true;
    break;
  case 3:
    if(C_R2 && C_R1 && (C_R0 || C_R4) || C_R4 && C_R5 && (C_R2 || C_R6) ) return true;
    break;
  case 4:
    if(C_R3 && C_R2 && (C_R1 || C_R5) || C_R3 && C_R5 && C_R6) return true;
    break;
  case 5:
    if(C_R4 && C_R3 && (C_R2 || C_R6)) return true;
    break;
  case 6:
    if(C_R5 && C_R4 && C_R3) return true;
    break;
  }

  const char *tmp;
  char i;
  char count = 0;
  for(tmp = p - (BOARDWIDTH+1), i = min(r, c);                            i-- && *tmp == cm; tmp -= BOARDWIDTH+1) { // go south-west
    count++;
  }
  if(count >= 3) return true;
  for(tmp = p + (BOARDWIDTH+1), i = min(BOARDHEIGHT-1-r, BOARDWIDTH-1-c); i-- && *tmp == cm; tmp += BOARDWIDTH+1) { // go north-east
    count++;
  }
  if(count >= 3) return true;

  count = 0; // !!!!
  for(tmp = p - (BOARDWIDTH-1), i = min(r, BOARDWIDTH-1-c);               i-- && *tmp == cm; tmp -= BOARDWIDTH-1) { // go south-east
    count++;
  }
  if(count >= 3) return true;
  for(tmp = p + (BOARDWIDTH-1), i = min(BOARDHEIGHT-1-r, c);              i-- && *tmp == cm; tmp += BOARDWIDTH-1) { // go north-west
    count++;
  }
  if(count >= 3) return true;
  return false;
}

Stribe4::Stribe4(const Game &g, Move lastmove) {
  Player cm = g.winner();

  p[0] = Position(g.colh(lastmove)-1,lastmove);
  for(int i = 1; i < 4; i++) {
    p[i] = p[0];
  }
  int r0 = p[0].m_r;
  int c0 = p[0].m_c;

  for(int dr = -1; dr <= 1; dr++) {
    for(int dc = -1; dc <= 1; dc++) {
      if(dr == 0 && dc == 0) continue;
      int i,r1,c1;
      for(i = 1, r1 = r0+dr, c1 = c0+dc; i < 4 && ONBOARD(r1,c1) && g.pos(r1,c1) == cm; i++, r1+=dr, c1+=dc) {
        p[i] = Position(r1,c1);
      }
      for(/*not i=1!*/r1 = r0-dr, c1 = c0-dc; i < 4 && ONBOARD(r1,c1) && g.pos(r1,c1) == cm; i++, r1-= dr, c1-=dc) {
        p[i] = Position(r1,c1);
      }
      if(i == 4) {
        return;
      }
    }
  }
}

int Game::getBrickCount() const {
  int count = 0;
  for(int i = 0; i < BOARDWIDTH; i++) {
    count += m_colh[i];
  }
  return count;
}

Move Game::findRandomMove() const {
  int c;
  INITMOVE(c);
  bool found = false;
  for(c = 0; c < BOARDWIDTH; c++) {
    if(m_colh[c] < BOARDHEIGHT) {
      found = true;
    }
  }
  if(!found) {
    return c;
  }
  for(;;) {
    c = rand() % BOARDWIDTH;
    if(m_colh[c] < BOARDHEIGHT) {
      return c;
    }
  }
}

void Game::doMove(Move m) {
  char r = m_colh[m]++;
  m_pos[r][m] = m_playerInTurn;
  m_score += m_playerInTurn * m_posValue[m_playerInTurn+1][r][m];
  if(has4(r, m)) {
    m_gameOver = true;
    m_score = m_playerInTurn * (MAX_SCORE - m_gameStack.getHeight());
/* - gamestaktop giver højest score til det træk der fører til
     hurtigste/langsomste gevinst/tab
*/
    m_winner = m_playerInTurn;
  } else {
    m_playerInTurn = OPPONENT(m_playerInTurn);
  }
}

Move Game::firstMoveFrw() const {
  if(gameOver()) {
    return -1;
  }
  for(Move m = 0; m < BOARDWIDTH; m++) {
    if(m_colh[m] < BOARDHEIGHT ) {
      return m;
    }
  }
  return -1;
}

Move Game::nextMoveFrw(Move lastmove) const {
  for(Move m = lastmove+1; m < BOARDWIDTH; m++) {
    if(m_colh[m] < BOARDHEIGHT ) {
      return m;
    }
  }
  return -1;
}

int Game::evaluateMe() const {
  m_evalCount++;
  return m_score;
}

int Game::evaluateYou() const {
  m_evalCount++;
  return -m_score;
}

int Game::getEvalCountPerSecond() const {
  return (m_evaluationTime == 0) ? 0 : (int)(((double)m_evalCount * 1000000) / m_evaluationTime);
}
// ---------------- A L P H A - B E T A  P R U N I N G ----------------

//static int (*evaluate)(void) = evaluate_me;

void Game::tryMove(Move m) {
  m_gameStack.push(*this);
  doMove(m);
}

void Game::untryMove() {
  *this = m_gameStack.pop();
}

int Game::maximize(int c_min, int depth) {
  int v;
  Move r;
//  movescanner *ms = movescan + (rand()%2);

  if(depth >= m_maxDepth) {
    v = evaluate();
  } else {
    Move p = firstMove();
    if(!ISMOVE(p)) {
      return evaluate();
    }
    bool done = false;
    for(v = MIN_SCORE; ISMOVE(p) && !done; p = nextMove(p)) {
      tryMove(p);

      int e = minimize(v, depth+1);
      if(e > v) {
        v = e;
        r = p;
      }

      if(v >= c_min) {
        done = true;
      }

      untryMove();
    }
  }
  if(v > m_bestScore && depth == 0) {
    m_bestScore = v;
    m_bestMove  = r;
  }
  return v;
}

int Game::minimize(int c_max, int depth) {
  int v;
//  movescanner *ms = movescan + (rand()%2);

  if(depth >= m_maxDepth) {
    v = evaluate();
  } else {
    Move p = firstMove();
    if(!ISMOVE(p)) {
      return evaluate();
    }
    bool done = false;
    for(v = MAX_SCORE; ISMOVE(p) && !done; p = nextMove(p)) {
      tryMove(p);

      int e = maximize(v, depth+1);
      if(e < v)
        v = e;

      if(v <= c_max) {
        done = true;
      }

      untryMove();
    }
  }
  return v;
}

Move Game::findMove1(int lookahead) {
  m_bestScore = MIN_SCORE;
  INITMOVE(m_bestMove);

  if(getBrickCount() == 0) { // just to speed up first Move
    m_bestScore = 0;
    return 3;
  }

  m_maxDepth = lookahead;
  m_gameStack.clear();
//  evaluate = player_in_turn == ME ? evaluate_me : evaluate_you;

  findPosValue(m_posValue[1+m_playerInTurn], m_playerInTurn);
  findPosValue(m_posValue[1-m_playerInTurn], OPPONENT(m_playerInTurn));
  m_score = 0;

  maximize(MAX_SCORE, 0);
  if(!ISMOVE(m_bestMove)) {
    return findRandomMove();
  } else {
    return m_bestMove;
  }
}

Move Game::findMove(int lookahead) {
  const double startTime = getThreadTime();
  m_evalCount = 0;
  const Move result = findMove1(lookahead);
  m_evaluationTime = getThreadTime() - startTime;
  return result;
}
