#include "stdafx.h"

// ---------------------------------------- LDA enter capture --------------------------------------

void Game::LDAenterRowCapture(const FieldInfo &fieldInfo) {
  PlayerState &state = m_playerState[PLAYERINTURN];

  if(fieldInfo.m_rowLine.m_lower) {
    SET_LDA_LEFT( state, fieldInfo);
  }
  if(fieldInfo.m_rowLine.m_upper) {
    SET_LDA_RIGHT(state, fieldInfo);
  }
}

void Game::LDAenterColumnCapture(const FieldInfo &fieldInfo) {
  PlayerState &state = m_playerState[PLAYERINTURN];

  if(fieldInfo.m_colLine.m_lower) {
    SET_LDA_DOWN(state, fieldInfo);
  }
  if(fieldInfo.m_colLine.m_upper) {
    SET_LDA_UP(  state, fieldInfo);
  }
}

void Game::LDAenterRCCapture(const FieldInfo &fieldInfo) {
  PlayerState &state = m_playerState[PLAYERINTURN];

  if(fieldInfo.m_rowLine.m_lower) {
    SET_LDA_LEFT( state, fieldInfo);
  }
  if(fieldInfo.m_rowLine.m_upper) {
    SET_LDA_RIGHT(state, fieldInfo);
  }
  if(fieldInfo.m_colLine.m_lower) {
    SET_LDA_DOWN( state, fieldInfo);
  }
  if(fieldInfo.m_colLine.m_upper) {
    SET_LDA_UP(   state, fieldInfo);
  }
}

void Game::LDAenterDiag1Capture(const FieldInfo &fieldInfo) {
  PlayerState &state = m_playerState[PLAYERINTURN];

  if(fieldInfo.m_diag1Line.m_lower) {
    SET_LDA_DOWNDIAG1(state, fieldInfo);
  }
  if(fieldInfo.m_diag1Line.m_upper) {
    SET_LDA_UPDIAG1(  state, fieldInfo);
  }
}

void Game::LDAenterDiag2Capture(const FieldInfo &fieldInfo) {
  PlayerState &state = m_playerState[PLAYERINTURN];

  if(fieldInfo.m_diag2Line.m_lower) {
    SET_LDA_DOWNDIAG2(state, fieldInfo);
  }
  if(fieldInfo.m_diag2Line.m_upper) {
    SET_LDA_UPDIAG2(  state, fieldInfo);
  }
}

void Game::LDAenterDiag12Capture(const FieldInfo &fieldInfo) {
  PlayerState &state = m_playerState[PLAYERINTURN];

  if(fieldInfo.m_diag1Line.m_lower) {
    SET_LDA_DOWNDIAG1(state, fieldInfo);
  }
  if(fieldInfo.m_diag1Line.m_upper) {
    SET_LDA_UPDIAG1(  state, fieldInfo);
  }
  if(fieldInfo.m_diag2Line.m_lower) {
    SET_LDA_DOWNDIAG2(state, fieldInfo);
  }
  if(fieldInfo.m_diag2Line.m_upper) {
    SET_LDA_UPDIAG2(  state, fieldInfo);
  }
}
