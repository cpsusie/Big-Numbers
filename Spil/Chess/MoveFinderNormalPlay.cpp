#include "stdafx.h"
#include "MoveFinderNormalPlay.h"

#ifdef _DEBUG
#define TRACELINE(msg) verbose(_T("%s\n"), msg)
#else
#define TRACELINE(msg)
#endif

ExecutableMove MoveFinderNormalPlay::findBestMove(Game &game, const TimeLimit &timeLimit, bool talking, bool hint) {
  initSearch(game, timeLimit, talking);
  try {
    const ExecutableMove m = checkForSingleMove();
    if(m.isMove()) {
      return m;
    }
    
#ifndef TABLEBASE_BUILDER
    m_evaluationCount   = 0;
    m_bestScore         = MIN_SCORE;
    m_sign              = (m_game->getPlayerInTurn()==WHITEPLAYER) ? 1 : -1;
    m_maxDepth          = getLookahead(4);
    m_bestMove.setNoMove();

    maximize();
#else
    m_bestMove.setNoMove();
#endif
  } catch(int) { // interrupted by user
    if(m_stopCode & STOP_IMMEDIATELY) {
      m_bestMove.setNoMove();
    } else if(m_stopCode & STOP_WHENMOVEFOUND) {
      // ignore
    } else {
      throwException(_T("Unknown stopcode:%04x"), m_stopCode);
    }
  }
  m_bestMove.setAnnotation(NOANNOTATION);
  return ExecutableMove(*m_game, m_bestMove);
}

String MoveFinderNormalPlay::getName() const {
  return _T("Intern engine");
}

#ifndef TABLEBASE_BUILDER
int MoveFinderNormalPlay::maximize() {
  MoveGenerator &mg   = m_game->getMoveGenerator();
  Move           m;
  bool           more = mg.firstMove(m);
  if(!more) {
    return m_game->isKingInCheck() ? MIN_SCORE : 0; // Checkmate/stalemate
  }
  m_game->pushState();
  int v;
  for(v = MIN_SCORE; more; more = mg.nextMove(m)) {
#ifdef _DEBUG
    String moveStr = m.toString(MOVE_SHORTFORMAT);
#endif
    int e;
    if(m_game->tryMove(m)) {
      e = minimize(v, m_maxDepth + (m_game->isKingInCheck()?1:0));
    } else {
      e = 0;
    }
    if((e > v) || !m_bestMove.isMove()) {
      v          = e;
      m_bestMove = m;
    }
    m_game->unTryMove();
  }
  m_game->popState();
  m_bestScore = v;
  return v;
}

int MoveFinderNormalPlay::minimize(int cMax, int depth) {
  if(depth == 0) {
    m_evaluationCount++;
    return m_sign * m_game->evaluateScore();
  } else {
    MoveGenerator &mg   = m_game->getMoveGenerator();
    Move           m;
    bool           more = mg.firstMove(m);
    if(!more) {
      return m_game->isKingInCheck() ? (MAX_SCORE - (m_maxDepth - depth)) : 0;
    }
    m_game->pushState();
    int v;
    for(v = MAX_SCORE; more; more = mg.nextMove(m)) {

#ifdef _DEBUG1
      String moveStr = m.toString(MOVE_SHORTFORMAT);
      switch(depth) {
      case 0: { int i = 1; } break;
      case 1: { int i = 1; } break;
      case 2: { int i = 1; } break;
      case 3: { int i = 1; } break;
      case 4: { int i = 1; } break;
      case 5: { int i = 1; } break;
      case 6: { int i = 1; } break;
      case 7: { int i = 1; } break;
      case 8: { int i = 1; } break;
      }
#endif

      int e;
      if(m_game->tryMove(m)) {
        e = maximize(v, depth-(m_game->isKingInCheck()?0:1));
      } else {
        e = 0;
      }
      if(e < v) {
        if((v = e) <= cMax) {
          TRACELINE(format(_T("Minimize(%d, cMax=%+5d). Cut tree. Score=%+5d<=cMax"), depth, cMax, v).cstr());
          m_game->unTryMove(); // exit loop
          break;
        }
        TRACELINE(format(_T("Minimize(%d, cMax=%+5d). Score=%+5d"), depth, cMax, v).cstr());
      }
      m_game->unTryMove();
    }
    m_game->popState();
    return v;
  }
}

int MoveFinderNormalPlay::maximize(int cMin, int depth) {
  if(depth == 0) {
    if(m_stopCode) {
      if(m_stopCode & STOP_IMMEDIATELY || m_bestMove.isMove()) {
        throw 0;
      }
    }
    m_evaluationCount++;
    return m_sign * m_game->evaluateScore();
  } else {
    MoveGenerator &mg   = m_game->getMoveGenerator();
    Move           m;
    bool           more = mg.firstMove(m);
    if(!more) {
      return m_game->isKingInCheck() ? (MIN_SCORE + (m_maxDepth - depth)) : 0;
    }
    m_game->pushState();
    int v;
    for(v = MIN_SCORE; more; more = mg.nextMove(m)) {

#ifdef _DEBUG1
      String moveStr = m.toString(MOVE_SHORTFORMAT);
      switch(depth) {
      case 0: { int i = 1; } break;
      case 1: { int i = 1; } break;
      case 2: { int i = 1; } break;
      case 3: { int i = 1; } break;
      case 4: { int i = 1; } break;
      case 5: { int i = 1; } break;
      case 6: { int i = 1; } break;
      case 7: { int i = 1; } break;
      case 8: { int i = 1; } break;
      }
#endif

      int e;
      if(m_game->tryMove(m)) {
        e = minimize(v, depth-(m_game->isKingInCheck()?0:1));
      } else {
        e = 0;
      }
      if(e > v) {
        if((v = e) >= cMin) {
          TRACELINE(format(_T("Maximize(%d, cMin=%+5d). Cut tree. Score=%+5d>=cMin"), depth, cMin, v).cstr());
          m_game->unTryMove(); // exit loop
          break;
        }
        TRACELINE(format(_T("Maximize(%d, cMin=%+5d). Score=%+5d"), depth, cMin, v).cstr());
      }
      m_game->unTryMove();
    }
    m_game->popState();
    return v;
  }
}

int MoveFinderNormalPlay::getLookahead(int level) const {
  switch(level) {
  case 1:
  case 2:
    return 1;
  case 3:
    return 2;
  case 4:
    return 4;
  case 5:
  case 6:
    if(m_game->getNonPawnCountOnBoard() <= 6) {
      return 6;
    } else {
      return 5;
    }
  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("level=%d. Must be [1..6]"), level);
    return 4;
  }
}
#endif

String MoveFinderNormalPlay::getStateString(Player computerPlayer, bool detailed) {
  return _T("No state for MoveFinderNormalPlay\n");
}

