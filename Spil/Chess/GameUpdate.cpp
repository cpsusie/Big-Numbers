#include "stdafx.h"

void Game::clearLeft(PlayerState &state, const FieldInfo &fieldInfo) {
  FieldAttacks *attackTable = state.m_attackTable;
  PositionArray pa          = fieldInfo.m_rowLine.m_lower;
  const Piece  *piece;

  for(int count = pa[0], pos = pa[1]; count--; pos += DPOS_LEFT) {
    ATT_LEFT(attackTable[pos].m_attackInfo) = 0;
    if(piece = m_board[pos]) {
      if(&piece->m_playerState == &state) {
        switch(KING_DIRECTION(state, pos)) {
        case MD_LEFT      :
          piece->m_pinnedState = GET_LDA_LEFT(piece->m_enemyState, pos) ? PINNED_TO_ROW : NOT_PINNED;
          return;
        case MD_RIGHT:
          piece->m_pinnedState = GET_LDA_RIGHT(piece->m_enemyState, pos) ? PINNED_TO_ROW : NOT_PINNED;
          return;
        }
      } else { // enemy piece
        switch(KING_DIRECTION(piece->m_playerState, pos)) {
        case MD_LEFT :
          piece->m_pinnedState = NOT_PINNED;
          return;
        case MD_RIGHT:
          piece->m_pinnedState = GET_LDA_RIGHT(state, pos) ? PINNED_TO_ROW : NOT_PINNED;
          return;
        }
      }
      return;
    }
  }
}

void Game::clearRight(PlayerState &state, const FieldInfo &fieldInfo) {
  FieldAttacks *attackTable = state.m_attackTable;
  PositionArray pa          = fieldInfo.m_rowLine.m_upper;
  const Piece  *piece;

  for(int count = pa[0], pos = pa[1]; count--; pos += DPOS_RIGHT) {
    ATT_RIGHT(attackTable[pos].m_attackInfo) = 0;
    if(piece = m_board[pos]) {
      if(&piece->m_playerState == &state) {
        switch(KING_DIRECTION(state, pos)) {
        case MD_LEFT :
          piece->m_pinnedState = GET_LDA_LEFT(piece->m_enemyState, pos) ? PINNED_TO_ROW : NOT_PINNED;
          return;
        case MD_RIGHT:
          piece->m_pinnedState = GET_LDA_RIGHT(piece->m_enemyState, pos) ? PINNED_TO_ROW : NOT_PINNED;
          return;
        }
      } else { // enemy piece
        switch(KING_DIRECTION(piece->m_playerState, pos)) {
        case MD_LEFT :
          piece->m_pinnedState = GET_LDA_LEFT(state, pos) ? PINNED_TO_ROW : NOT_PINNED;
          return;
        case MD_RIGHT:
          piece->m_pinnedState = NOT_PINNED;
          return;
        }
      }
      return;
    }
  }
}

void Game::clearDown(PlayerState &state, const FieldInfo &fieldInfo) {
  FieldAttacks *attackTable = state.m_attackTable;
  PositionArray pa          = fieldInfo.m_colLine.m_lower;
  const Piece  *piece;

  for(int count = pa[0], pos = pa[1]; count--; pos += DPOS_DOWN) {
    ATT_DOWN(attackTable[pos].m_attackInfo) = 0;
    if(piece = m_board[pos]) {
      if(&piece->m_playerState == &state) {
        switch(KING_DIRECTION(state, pos)) {
        case MD_DOWN:
          piece->m_pinnedState = GET_LDA_DOWN(piece->m_enemyState, pos) ? PINNED_TO_COL : NOT_PINNED;
          return;
        case MD_UP  :
          piece->m_pinnedState = GET_LDA_UP(piece->m_enemyState, pos) ? PINNED_TO_COL : NOT_PINNED;
          return;
        }
      } else { // enemy piece
        switch(KING_DIRECTION(piece->m_playerState, pos)) {
        case MD_DOWN :
          piece->m_pinnedState = NOT_PINNED;
          return;
        case MD_UP   :
          piece->m_pinnedState = GET_LDA_UP(state, pos) ? PINNED_TO_COL : NOT_PINNED;
          return;
        }
      }
      return;
    }
  }
}

void Game::clearUp(PlayerState &state, const FieldInfo &fieldInfo) {
  FieldAttacks *attackTable = state.m_attackTable;
  PositionArray pa          = fieldInfo.m_colLine.m_upper;
  const Piece  *piece;

  for(int count = pa[0], pos = pa[1]; count--; pos += DPOS_UP) {
    ATT_UP(attackTable[pos].m_attackInfo) = 0;
    if(piece = m_board[pos]) {
      if(&piece->m_playerState == &state) {
        switch(KING_DIRECTION(state, pos)) {
        case MD_DOWN:
          piece->m_pinnedState = GET_LDA_DOWN(piece->m_enemyState, pos) ? PINNED_TO_COL : NOT_PINNED;
          return;
        case MD_UP  :
          piece->m_pinnedState = GET_LDA_UP(piece->m_enemyState, pos) ? PINNED_TO_COL : NOT_PINNED;
          return;
        }
      } else { // enemy piece
        switch(KING_DIRECTION(piece->m_playerState, pos)) {
        case MD_DOWN :
          piece->m_pinnedState = GET_LDA_DOWN(state, pos) ? PINNED_TO_COL : NOT_PINNED;
          return;
        case MD_UP   :
          piece->m_pinnedState = NOT_PINNED;
          return;
        }
      }
      return;
    }
  }
}

void Game::clearDownDiag1(PlayerState &state, const FieldInfo &fieldInfo) {
  FieldAttacks *attackTable = state.m_attackTable;
  PositionArray pa          = fieldInfo.m_diag1Line.m_lower;
  const Piece  *piece;

  for(int count = pa[0], pos = pa[1]; count--; pos += DPOS_DOWNDIAG1) {
    ATT_DOWNDIAG1(attackTable[pos].m_attackInfo) = 0;
    if(piece = m_board[pos]) {
      if(&piece->m_playerState == &state) {
        switch(KING_DIRECTION(state, pos)) {
        case MD_DOWNDIAG1:
          piece->m_pinnedState = GET_LDA_DOWNDIAG1(piece->m_enemyState, pos) ? PINNED_TO_DIAG1 : NOT_PINNED;
          return;
        case MD_UPDIAG1  :
          piece->m_pinnedState = GET_LDA_UPDIAG1(piece->m_enemyState, pos) ? PINNED_TO_DIAG1 : NOT_PINNED;
          return;
        }
      } else { // enemy piece
        switch(KING_DIRECTION(piece->m_playerState, pos)) {
        case MD_DOWNDIAG1:
          piece->m_pinnedState = NOT_PINNED;
          return;
        case MD_UPDIAG1:
          piece->m_pinnedState = GET_LDA_UPDIAG1(  state, pos) ? PINNED_TO_DIAG1 : NOT_PINNED;
          return;
        }
      }
      return;
    }
  }
}

void Game::clearUpDiag1(PlayerState &state, const FieldInfo &fieldInfo) {
  FieldAttacks *attackTable = state.m_attackTable;
  PositionArray pa          = fieldInfo.m_diag1Line.m_upper;
  const Piece  *piece;

  for(int count = pa[0], pos = pa[1]; count--; pos += DPOS_UPDIAG1  ) {
    ATT_UPDIAG1(attackTable[pos].m_attackInfo) = 0;
    if(piece = m_board[pos]) {
      if(&piece->m_playerState == &state) {
        switch(KING_DIRECTION(state, pos)) {
        case MD_DOWNDIAG1:
          piece->m_pinnedState = GET_LDA_DOWNDIAG1(piece->m_enemyState, pos) ? PINNED_TO_DIAG1 : NOT_PINNED;
          return;
        case MD_UPDIAG1  :
          piece->m_pinnedState = GET_LDA_UPDIAG1(piece->m_enemyState, pos) ? PINNED_TO_DIAG1 : NOT_PINNED;
          return;
        }
      } else { // enemy piece
        switch(KING_DIRECTION(piece->m_playerState, pos)) {
        case MD_DOWNDIAG1:
          piece->m_pinnedState = GET_LDA_DOWNDIAG1(state, pos) ? PINNED_TO_DIAG1 : NOT_PINNED;
          return;
        case MD_UPDIAG1  :
          piece->m_pinnedState = NOT_PINNED;
          return;
        }
      }
      return;
    }
  }
}

void Game::clearDownDiag2(PlayerState &state, const FieldInfo &fieldInfo) {
  FieldAttacks *attackTable = state.m_attackTable;
  PositionArray pa          = fieldInfo.m_diag2Line.m_lower;
  const Piece  *piece;

  for(int count = pa[0], pos = pa[1]; count--; pos += DPOS_DOWNDIAG2) {
    ATT_DOWNDIAG2(attackTable[pos].m_attackInfo) = 0;
    if(piece = m_board[pos]) {
      if(&piece->m_playerState == &state) {
        switch(KING_DIRECTION(state, pos)) {
        case MD_DOWNDIAG2:
          piece->m_pinnedState = GET_LDA_DOWNDIAG2(piece->m_enemyState, pos) ? PINNED_TO_DIAG2 : NOT_PINNED;
          return;
        case MD_UPDIAG2  :
          piece->m_pinnedState = GET_LDA_UPDIAG2(piece->m_enemyState, pos) ? PINNED_TO_DIAG2 : NOT_PINNED;
          return;
        }
      } else { // enemy piece
        switch(KING_DIRECTION(piece->m_playerState, pos)) {
        case MD_DOWNDIAG2:
          piece->m_pinnedState = NOT_PINNED;
          return;
        case MD_UPDIAG2  :
          piece->m_pinnedState = GET_LDA_UPDIAG2(  state, pos) ? PINNED_TO_DIAG2 : NOT_PINNED;
          return;
        }
      }
      return;
    }
  }
}

void Game::clearUpDiag2(PlayerState &state, const FieldInfo &fieldInfo) {
  FieldAttacks *attackTable = state.m_attackTable;
  PositionArray pa          = fieldInfo.m_diag2Line.m_upper;
  const Piece  *piece;

  for(int count = pa[0], pos = pa[1]; count--; pos += DPOS_UPDIAG2  ) {
    ATT_UPDIAG2(attackTable[pos].m_attackInfo) = 0;
    if(piece = m_board[pos]) {
      if(&piece->m_playerState == &state) {
        switch(KING_DIRECTION(state, pos)) {
        case MD_DOWNDIAG2:
          piece->m_pinnedState = GET_LDA_DOWNDIAG2(piece->m_enemyState, pos) ? PINNED_TO_DIAG2 : NOT_PINNED;
          return;
        case MD_UPDIAG2  :
          piece->m_pinnedState = GET_LDA_UPDIAG2(piece->m_enemyState, pos) ? PINNED_TO_DIAG2 : NOT_PINNED;
          return;
        }
      } else { // enemy piece
        switch(KING_DIRECTION(piece->m_playerState, pos)) {
        case MD_DOWNDIAG2:
          piece->m_pinnedState = GET_LDA_DOWNDIAG2(state, pos) ? PINNED_TO_DIAG2 : NOT_PINNED;
          return;
        case MD_UPDIAG2  :
          piece->m_pinnedState = NOT_PINNED;
          return;
        }
      }
      return;
    }
  }
}

void Game::setLeft(PlayerState &state, const FieldInfo &fieldInfo) {
  FieldAttacks *attackTable = state.m_attackTable;
  PositionArray pa          = fieldInfo.m_rowLine.m_lower;
  const Piece  *piece;

  for(int count = pa[0], pos = pa[1]; count--; pos += DPOS_LEFT) {
    ATT_LEFT(attackTable[pos].m_attackInfo) = 1;
    if(piece = m_board[pos]) {
      if(&piece->m_playerState != &state && KING_IS_LEFT(piece->m_playerState, pos)) { // Cannot be RIGHT, because ATT_LEFT = 1
        piece->m_pinnedState = PINNED_TO_ROW;
      }
      return;
    }
  }
}

void Game::setRight(PlayerState &state, const FieldInfo &fieldInfo) {
  FieldAttacks *attackTable = state.m_attackTable;
  PositionArray pa          = fieldInfo.m_rowLine.m_upper;
  const Piece  *piece;

  for(int count = pa[0], pos = pa[1]; count--; pos += DPOS_RIGHT) {
    ATT_RIGHT(attackTable[pos].m_attackInfo) = 1;
    if(piece = m_board[pos]) {
      if(&piece->m_playerState != &state && KING_IS_RIGHT(piece->m_playerState, pos)) { // Cannot be LEFT, because ATT_RIGHT = 1
        piece->m_pinnedState = PINNED_TO_ROW;
      }
      return;
    }
  }
}

void Game::setDown(PlayerState &state, const FieldInfo &fieldInfo) {
  FieldAttacks *attackTable = state.m_attackTable;
  PositionArray pa          = fieldInfo.m_colLine.m_lower;
  const Piece  *piece;

  for(int count = pa[0], pos = pa[1]; count--; pos += DPOS_DOWN) {
    ATT_DOWN(attackTable[pos].m_attackInfo) = 1;
    if(piece = m_board[pos]) {
      if(&piece->m_playerState != &state && KING_IS_DOWN(piece->m_playerState, pos)) { // Cannot be UP, because ATT_DOWN = 1
        piece->m_pinnedState = PINNED_TO_COL;
      }
      return;
    }
  }
}

void Game::setUp(PlayerState &state, const FieldInfo &fieldInfo) {
  FieldAttacks *attackTable = state.m_attackTable;
  PositionArray pa          = fieldInfo.m_colLine.m_upper;
  const Piece  *piece;

  for(int count = pa[0], pos = pa[1]; count--; pos += DPOS_UP) {
    ATT_UP(attackTable[pos].m_attackInfo) = 1;
    if(piece = m_board[pos]) {
      if(&piece->m_playerState != &state && KING_IS_UP(piece->m_playerState, pos)) { // Cannot be DOWN, because ATT_UP = 1
        piece->m_pinnedState = PINNED_TO_COL;
      }
      return;
    }
  }
}

void Game::setDownDiag1(PlayerState &state, const FieldInfo &fieldInfo) {
  FieldAttacks *attackTable = state.m_attackTable;
  PositionArray pa          = fieldInfo.m_diag1Line.m_lower;
  const Piece  *piece;

  for(int count = pa[0], pos = pa[1]; count--; pos += DPOS_DOWNDIAG1) {
    ATT_DOWNDIAG1(attackTable[pos].m_attackInfo) = 1;
    if(piece = m_board[pos]) {
      if(&piece->m_playerState != &state && KING_IS_DOWNDIAG1(piece->m_playerState, pos)) { // Cannot be UPDIAG1,   because ATT_DOWNDIAG1 = 1
        piece->m_pinnedState = PINNED_TO_DIAG1;
      }
      return;
    }
  }
}

void Game::setUpDiag1(PlayerState &state, const FieldInfo &fieldInfo) {
  FieldAttacks *attackTable = state.m_attackTable;
  PositionArray pa          = fieldInfo.m_diag1Line.m_upper;
  const Piece  *piece;

  for(int count = pa[0], pos = pa[1]; count--; pos += DPOS_UPDIAG1  ) {
    ATT_UPDIAG1(attackTable[pos].m_attackInfo) = 1;
    if(piece = m_board[pos]) {
      if(&piece->m_playerState != &state && KING_IS_UPDIAG1(piece->m_playerState, pos)) { // Cannot be DOWNDIAG1, because ATT_UPDIAG1 = 1
        piece->m_pinnedState = PINNED_TO_DIAG1;
      }
      return;
    }
  }
}

void Game::setDownDiag2(PlayerState &state, const FieldInfo &fieldInfo) {
  FieldAttacks *attackTable = state.m_attackTable;
  PositionArray pa          = fieldInfo.m_diag2Line.m_lower;
  const Piece  *piece;

  for(int count = pa[0], pos = pa[1]; count--; pos += DPOS_DOWNDIAG2) {
    ATT_DOWNDIAG2(attackTable[pos].m_attackInfo) = 1;
    if(piece = m_board[pos]) {
      if(&piece->m_playerState != &state && KING_IS_DOWNDIAG2(piece->m_playerState, pos)) { // Cannot be UPDIAG2,   because ATT_DOWNDIAG2 = 1
        piece->m_pinnedState = PINNED_TO_DIAG2;
      }
      return;
    }
  }
}

void Game::setUpDiag2(PlayerState &state, const FieldInfo &fieldInfo) {
  FieldAttacks *attackTable = state.m_attackTable;
  PositionArray pa          = fieldInfo.m_diag2Line.m_upper;
  const Piece  *piece;

  for(int count = pa[0], pos = pa[1]; count--; pos += DPOS_UPDIAG2) {
    ATT_UPDIAG2(attackTable[pos].m_attackInfo) = 1;
    if(piece = m_board[pos]) {
      if(&piece->m_playerState != &state && KING_IS_UPDIAG2(piece->m_playerState, pos)) { // Cannot be DOWNDIAG2, because ATT_UPDIAG2 = 1
        piece->m_pinnedState = PINNED_TO_DIAG2;
      }
      return;
    }
  }
}
