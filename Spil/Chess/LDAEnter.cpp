#include "stdafx.h"

// ---------------------------------------- LDA enter --------------------------------------

void Game::LDAenterRow(const FieldInfo &fieldInfo) {
  PlayerState &state = m_playerState[PLAYERINTURN];

  if(fieldInfo.m_rowLine.m_isBlockable) {
    PlayerState &enemyState = m_playerState[CURRENTENEMY];
    if(GET_LDA_LEFT(enemyState, fieldInfo.m_pos)) {
      CLR_LDA_LEFT( enemyState, fieldInfo);
    }
    if(GET_LDA_RIGHT(enemyState, fieldInfo.m_pos)) {
      CLR_LDA_RIGHT( enemyState, fieldInfo);
    }
  }
  if(fieldInfo.m_rowLine.m_lower && !GET_LDA_LEFT( state, fieldInfo.m_pos)) {
    SET_LDA_LEFT( state, fieldInfo);
  }
  if(fieldInfo.m_rowLine.m_upper && !GET_LDA_RIGHT(state, fieldInfo.m_pos)) {
    SET_LDA_RIGHT(state, fieldInfo);
  }
}

void Game::LDAenterColumn(const FieldInfo &fieldInfo) {
  PlayerState &state = m_playerState[PLAYERINTURN];

  if(fieldInfo.m_colLine.m_isBlockable) {
    PlayerState &enemyState = m_playerState[CURRENTENEMY];
    if(GET_LDA_DOWN(enemyState, fieldInfo.m_pos)) {
      CLR_LDA_DOWN( enemyState, fieldInfo);
    }
    if(GET_LDA_UP(enemyState, fieldInfo.m_pos)) {
      CLR_LDA_UP( enemyState, fieldInfo);
    }
  }
  if(fieldInfo.m_colLine.m_lower && !GET_LDA_DOWN(state, fieldInfo.m_pos)) {
    SET_LDA_DOWN(state, fieldInfo);
  }
  if(fieldInfo.m_colLine.m_upper && !GET_LDA_UP(  state, fieldInfo.m_pos)) {
    SET_LDA_UP(  state, fieldInfo);
  }
}

// faster than { LDAenterRow(f); LDAenterColumn(f); }
void Game::LDAenterRC(const FieldInfo &fieldInfo) {
  PlayerState &state      = m_playerState[PLAYERINTURN];
  PlayerState &enemyState = m_playerState[CURRENTENEMY];

  if(fieldInfo.m_rowLine.m_isBlockable) {
    if(GET_LDA_LEFT( enemyState, fieldInfo.m_pos)) {
      CLR_LDA_LEFT(  enemyState, fieldInfo);
    }
    if(GET_LDA_RIGHT(enemyState, fieldInfo.m_pos)) {
      CLR_LDA_RIGHT( enemyState, fieldInfo);
    }
  }
  if(fieldInfo.m_colLine.m_isBlockable) {
    if(GET_LDA_DOWN( enemyState, fieldInfo.m_pos)) {
      CLR_LDA_DOWN(  enemyState, fieldInfo);
    }
    if(GET_LDA_UP(   enemyState, fieldInfo.m_pos)) {
      CLR_LDA_UP(    enemyState, fieldInfo);
    }
  }
  if(fieldInfo.m_rowLine.m_lower && !GET_LDA_LEFT( state, fieldInfo.m_pos)) {
    SET_LDA_LEFT( state, fieldInfo);
  }
  if(fieldInfo.m_rowLine.m_upper && !GET_LDA_RIGHT(state, fieldInfo.m_pos)) {
    SET_LDA_RIGHT(state, fieldInfo);
  }
  if(fieldInfo.m_colLine.m_lower && !GET_LDA_DOWN( state, fieldInfo.m_pos)) {
    SET_LDA_DOWN( state, fieldInfo);
  }
  if(fieldInfo.m_colLine.m_upper && !GET_LDA_UP(   state, fieldInfo.m_pos)) {
    SET_LDA_UP(   state, fieldInfo);
  }
}

void Game::LDAenterDiag1(const FieldInfo &fieldInfo) {
  PlayerState &state = m_playerState[PLAYERINTURN];

  if(fieldInfo.m_diag1Line.m_isBlockable) {
    PlayerState &enemyState = m_playerState[CURRENTENEMY];
    if(GET_LDA_UPDIAG1(  enemyState, fieldInfo.m_pos)) {
      CLR_LDA_UPDIAG1(   enemyState, fieldInfo);
    }
    if(GET_LDA_DOWNDIAG1(enemyState, fieldInfo.m_pos)) {
      CLR_LDA_DOWNDIAG1( enemyState, fieldInfo);
    }
  }

  if(fieldInfo.m_diag1Line.m_lower && !GET_LDA_DOWNDIAG1(state, fieldInfo.m_pos)) {
    SET_LDA_DOWNDIAG1(state, fieldInfo);
  }
  if(fieldInfo.m_diag1Line.m_upper && !GET_LDA_UPDIAG1(  state, fieldInfo.m_pos)) {
    SET_LDA_UPDIAG1(  state, fieldInfo);
  }
}

void Game::LDAenterDiag2(const FieldInfo &fieldInfo) {
  PlayerState &state = m_playerState[PLAYERINTURN];

  if(fieldInfo.m_diag2Line.m_isBlockable) {
    PlayerState &enemyState = m_playerState[CURRENTENEMY];
    if(GET_LDA_UPDIAG2(  enemyState, fieldInfo.m_pos)) {
      CLR_LDA_UPDIAG2(   enemyState, fieldInfo);
    }
    if(GET_LDA_DOWNDIAG2(enemyState, fieldInfo.m_pos)) {
      CLR_LDA_DOWNDIAG2( enemyState, fieldInfo);
    }
  }

  if(fieldInfo.m_diag2Line.m_lower && !GET_LDA_DOWNDIAG2(state, fieldInfo.m_pos)) {
    SET_LDA_DOWNDIAG2(state, fieldInfo);
  }
  if(fieldInfo.m_diag2Line.m_upper && !GET_LDA_UPDIAG2(  state, fieldInfo.m_pos)) {
    SET_LDA_UPDIAG2(  state, fieldInfo);
  }
}

// faster than { LDAenterDiag1(f); LDAenterDiag2(f); }
void Game::LDAenterDiag12(const FieldInfo &fieldInfo) {
  PlayerState &state = m_playerState[PLAYERINTURN];

  if(fieldInfo.m_innerField) {
    PlayerState &enemyState = m_playerState[CURRENTENEMY];
    if(GET_LDA_UPDIAG1(  enemyState, fieldInfo.m_pos)) {
      CLR_LDA_UPDIAG1(   enemyState, fieldInfo);
    }
    if(GET_LDA_DOWNDIAG1(enemyState, fieldInfo.m_pos)) {
      CLR_LDA_DOWNDIAG1( enemyState, fieldInfo);
    }
    if(GET_LDA_UPDIAG2(  enemyState, fieldInfo.m_pos)) {
      CLR_LDA_UPDIAG2(   enemyState, fieldInfo);
    }
    if(GET_LDA_DOWNDIAG2(enemyState, fieldInfo.m_pos)) {
      CLR_LDA_DOWNDIAG2( enemyState, fieldInfo);
    }
  }

  if(fieldInfo.m_diag1Line.m_lower && !GET_LDA_DOWNDIAG1(state, fieldInfo.m_pos)) {
    SET_LDA_DOWNDIAG1(state, fieldInfo);
  }
  if(fieldInfo.m_diag1Line.m_upper && !GET_LDA_UPDIAG1(  state, fieldInfo.m_pos)) {
    SET_LDA_UPDIAG1(  state, fieldInfo);
  }
  if(fieldInfo.m_diag2Line.m_lower && !GET_LDA_DOWNDIAG2(state, fieldInfo.m_pos)) {
    SET_LDA_DOWNDIAG2(state, fieldInfo);
  }
  if(fieldInfo.m_diag2Line.m_upper && !GET_LDA_UPDIAG2(  state, fieldInfo.m_pos)) {
    SET_LDA_UPDIAG2(  state, fieldInfo);
  }
}
