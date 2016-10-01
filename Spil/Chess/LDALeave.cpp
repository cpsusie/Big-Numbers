#include "stdafx.h"

// ---------------------------------------- LDA leave --------------------------------------

void Game::LDAleaveRow(const FieldInfo &fieldInfo) {
  PlayerState &state = m_playerState[PLAYERINTURN];

  if(fieldInfo.m_rowLine.m_isBlockable) {
    PlayerState &enemyState = m_playerState[CURRENTENEMY];
    if(GET_LDA_LEFT(enemyState, fieldInfo.m_pos)) {
      SET_LDA_LEFT( enemyState, fieldInfo);
    }
    if(GET_LDA_RIGHT(enemyState, fieldInfo.m_pos)) {
      SET_LDA_RIGHT( enemyState, fieldInfo);
    }
  }
  if(fieldInfo.m_rowLine.m_lower && !GET_LDA_LEFT( state, fieldInfo.m_pos)) {
    CLR_LDA_LEFT( state, fieldInfo);
  }
  if(fieldInfo.m_rowLine.m_upper && !GET_LDA_RIGHT(state, fieldInfo.m_pos)) {
    CLR_LDA_RIGHT(state, fieldInfo);
  }
}

void Game::LDAleaveColumn(const FieldInfo &fieldInfo) {
  PlayerState &state = m_playerState[PLAYERINTURN];

  if(fieldInfo.m_colLine.m_isBlockable) {
    PlayerState &enemyState = m_playerState[CURRENTENEMY];
    if(GET_LDA_DOWN(enemyState, fieldInfo.m_pos)) {
      SET_LDA_DOWN( enemyState, fieldInfo);
    }
    if(GET_LDA_UP(enemyState, fieldInfo.m_pos)) {
      SET_LDA_UP( enemyState, fieldInfo);
    }
  }
  if(fieldInfo.m_colLine.m_lower && !GET_LDA_DOWN(state, fieldInfo.m_pos)) {
    CLR_LDA_DOWN(state, fieldInfo);
  }
  if(fieldInfo.m_colLine.m_upper && !GET_LDA_UP(  state, fieldInfo.m_pos)) {
    CLR_LDA_UP(  state, fieldInfo);
  }
}

// faster than { LDAleaveRow(f); LDAleaveColumn(f); }
void Game::LDAleaveRC(const FieldInfo &fieldInfo) {
  PlayerState      &state      = m_playerState[PLAYERINTURN];
  PlayerState      &enemyState = m_playerState[CURRENTENEMY];

  if(fieldInfo.m_rowLine.m_isBlockable) {
    if(GET_LDA_LEFT( enemyState, fieldInfo.m_pos)) {
      SET_LDA_LEFT(  enemyState, fieldInfo);
    }
    if(GET_LDA_RIGHT(enemyState, fieldInfo.m_pos)) {
      SET_LDA_RIGHT( enemyState, fieldInfo);
    }
  }
  if(fieldInfo.m_colLine.m_isBlockable) {
    if(GET_LDA_DOWN( enemyState, fieldInfo.m_pos)) {
      SET_LDA_DOWN(  enemyState, fieldInfo);
    }
    if(GET_LDA_UP(   enemyState, fieldInfo.m_pos)) {
      SET_LDA_UP(    enemyState, fieldInfo);
    }
  }
  if(fieldInfo.m_rowLine.m_lower && !GET_LDA_LEFT( state, fieldInfo.m_pos)) {
    CLR_LDA_LEFT( state, fieldInfo);
  }
  if(fieldInfo.m_rowLine.m_upper && !GET_LDA_RIGHT(state, fieldInfo.m_pos)) {
    CLR_LDA_RIGHT(state, fieldInfo);
  }
  if(fieldInfo.m_colLine.m_lower && !GET_LDA_DOWN( state, fieldInfo.m_pos)) {
    CLR_LDA_DOWN( state, fieldInfo);
  }
  if(fieldInfo.m_colLine.m_upper && !GET_LDA_UP(   state, fieldInfo.m_pos)) {
    CLR_LDA_UP(   state, fieldInfo);
  }
}

void Game::LDAleaveDiag1(const FieldInfo &fieldInfo) {
  PlayerState &state = m_playerState[PLAYERINTURN];

  if(fieldInfo.m_diag1Line.m_isBlockable) {
    PlayerState &enemyState = m_playerState[CURRENTENEMY];
    if(GET_LDA_UPDIAG1(  enemyState, fieldInfo.m_pos)) {
      SET_LDA_UPDIAG1(   enemyState, fieldInfo);
    }
    if(GET_LDA_DOWNDIAG1(enemyState, fieldInfo.m_pos)) {
      SET_LDA_DOWNDIAG1( enemyState, fieldInfo);
    }
  }

  if(fieldInfo.m_diag1Line.m_lower && !GET_LDA_DOWNDIAG1(state, fieldInfo.m_pos)) {
    CLR_LDA_DOWNDIAG1(state, fieldInfo);
  }
  if(fieldInfo.m_diag1Line.m_upper && !GET_LDA_UPDIAG1(  state, fieldInfo.m_pos)) {
    CLR_LDA_UPDIAG1(  state, fieldInfo);
  }
}

void Game::LDAleaveDiag2(const FieldInfo &fieldInfo) {
  PlayerState &state = m_playerState[PLAYERINTURN];

  if(fieldInfo.m_diag2Line.m_isBlockable) {
    PlayerState &enemyState = m_playerState[CURRENTENEMY];
    if(GET_LDA_UPDIAG2(  enemyState, fieldInfo.m_pos)) {
      SET_LDA_UPDIAG2(   enemyState, fieldInfo);
    }
    if(GET_LDA_DOWNDIAG2(enemyState, fieldInfo.m_pos)) {
      SET_LDA_DOWNDIAG2( enemyState, fieldInfo);
    }
  }

  if(fieldInfo.m_diag2Line.m_lower && !GET_LDA_DOWNDIAG2(state, fieldInfo.m_pos)) {
    CLR_LDA_DOWNDIAG2(state, fieldInfo);
  }
  if(fieldInfo.m_diag2Line.m_upper && !GET_LDA_UPDIAG2(  state, fieldInfo.m_pos)) {
    CLR_LDA_UPDIAG2(  state, fieldInfo);
  }
}

// faster than { LDAleaveDiag1(f); LDAleaveDiag2(f); }
void Game::LDAleaveDiag12(const FieldInfo &fieldInfo) {
  PlayerState &state = m_playerState[PLAYERINTURN];

  if(fieldInfo.m_innerField) {
    PlayerState &enemyState = m_playerState[CURRENTENEMY];
    if(GET_LDA_UPDIAG1(  enemyState, fieldInfo.m_pos)) {
      SET_LDA_UPDIAG1(   enemyState, fieldInfo);
    }
    if(GET_LDA_DOWNDIAG1(enemyState, fieldInfo.m_pos)) {
      SET_LDA_DOWNDIAG1( enemyState, fieldInfo);
    }
    if(GET_LDA_UPDIAG2(  enemyState, fieldInfo.m_pos)) {
      SET_LDA_UPDIAG2(   enemyState, fieldInfo);
    }
    if(GET_LDA_DOWNDIAG2(enemyState, fieldInfo.m_pos)) {
      SET_LDA_DOWNDIAG2( enemyState, fieldInfo);
    }
  }

  if(fieldInfo.m_diag1Line.m_lower && !GET_LDA_DOWNDIAG1(state, fieldInfo.m_pos)) {
    CLR_LDA_DOWNDIAG1(state, fieldInfo);
  }
  if(fieldInfo.m_diag1Line.m_upper && !GET_LDA_UPDIAG1(  state, fieldInfo.m_pos)) {
    CLR_LDA_UPDIAG1(  state, fieldInfo);
  }
  if(fieldInfo.m_diag2Line.m_lower && !GET_LDA_DOWNDIAG2(state, fieldInfo.m_pos)) {
    CLR_LDA_DOWNDIAG2(state, fieldInfo);
  }
  if(fieldInfo.m_diag2Line.m_upper && !GET_LDA_UPDIAG2(  state, fieldInfo.m_pos)) {
    CLR_LDA_UPDIAG2(  state, fieldInfo);
  }
}
