#include "stdafx.h"

void Game::updateGameMoveBishop(const Move &m) {
  Piece *piece = m.m_piece;

  if(m.m_capturedPiece == nullptr) {
    bishopLeaveField(m);
    SET_PIECE(m.m_to, piece);
    setNonCapturingBishop(m);
  } else {
    capturePiece(m);
    bishopLeaveField(m);
    SET_PIECE(m.m_to, piece);
    setCapturingBishop(m);
  }

  piece->m_pinnedState = getPinnedState(m.m_to);
}

#if defined(TABLEBASE_BUILDER)
void Game::updateGameBackMoveBishop(const Move &m) {
  const Move m1 = m.makeBackMove();

  bishopLeaveField(m1);
  SET_PIECE(m.m_from, m.m_piece);
  setNonCapturingBishop(m1);
  m.m_piece->m_pinnedState = getPinnedState(m.m_from);
}
#endif

void Game::updateGameCaptureBishop(const Move &m) {
  PlayerState     &state  = m_playerState[CURRENTENEMY];
  const FieldInfo &toInfo = s_fieldInfo[m.m_to];

  if(toInfo.m_diag1Line.m_upper) {
    CLR_LDA_UPDIAG1(  state, toInfo);
  }
  if(toInfo.m_diag1Line.m_lower) {
    CLR_LDA_DOWNDIAG1(state, toInfo);
  }
  if(toInfo.m_diag2Line.m_upper) {
    CLR_LDA_UPDIAG2(  state, toInfo);
  }
  if(toInfo.m_diag2Line.m_lower) {
    CLR_LDA_DOWNDIAG2(state, toInfo);
  }
#if !defined(TABLEBASE_BUILDER)
  if(state.m_bishopFlags == BISHOPPAIR) {
    state.m_positionalScore -= BISHOPPAIRBONUS; // no pair of bishops anymore.
  }
  state.m_bishopFlags &= ~m.m_capturedPiece->m_bishopFlag;
#endif
}

void Game::bishopLeaveField(const Move &m) {
  const FieldInfo &fromInfo = s_fieldInfo[m.m_from];

  updateKingDir(m);

  // Bishop always leaves both row and column when moving
  if(fromInfo.m_innerCol) {
    unblockRow(   fromInfo);
  }

  if(fromInfo.m_innerRow) {
    unblockColumn(fromInfo);
  }

  switch(m.m_direction) {
  case MD_DOWNDIAG1:
  case MD_UPDIAG1  :
    LDAleaveDiag2(fromInfo);
    break;

  case MD_DOWNDIAG2:
  case MD_UPDIAG2  :
    LDAleaveDiag1(fromInfo);
    break;

  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("m.direction=%d"), m.m_direction);
  }

  SET_EMPTYFIELD(m.m_from);
}

void Game::setNonCapturingBishop(const Move &m) {
  const FieldInfo &toInfo = s_fieldInfo[m.m_to];

  if(toInfo.m_innerCol) {
    blockRow(toInfo);
  }
  if(toInfo.m_innerRow) {
    blockColumn(toInfo);
  }

  switch(m.m_direction) {
  case MD_DOWNDIAG1:
    LDAwalkDownDiag1(       toInfo, m.m_from);
    LDAenterDiag2(          toInfo);
    break;

  case MD_UPDIAG1  :
    LDAwalkUpDiag1(         toInfo, m.m_from);
    LDAenterDiag2(          toInfo);
    break;

  case MD_DOWNDIAG2:
    LDAwalkDownDiag2(       toInfo, m.m_from);
    LDAenterDiag1(          toInfo);
    break;

  case MD_UPDIAG2  :
    LDAwalkUpDiag2(         toInfo, m.m_from);
    LDAenterDiag1(          toInfo);
    break;

  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("m.direction=%d"), m.m_direction);
  }
}

void Game::setCapturingBishop(const Move &m) {
  const FieldInfo &toInfo = s_fieldInfo[m.m_to];

  switch(m.m_direction) {
  case MD_DOWNDIAG1:
    LDAwalkDownDiag1Capture(toInfo,  m.m_from);
    LDAenterDiag2Capture(   toInfo);
    break;

  case MD_UPDIAG1  :
    LDAwalkUpDiag1Capture(  toInfo,  m.m_from);
    LDAenterDiag2Capture(   toInfo);
    break;

  case MD_DOWNDIAG2:
    LDAwalkDownDiag2Capture(toInfo,  m.m_from);
    LDAenterDiag1Capture(   toInfo);
    break;

  case MD_UPDIAG2  :
    LDAwalkUpDiag2Capture(  toInfo,  m.m_from);
    LDAenterDiag1Capture(   toInfo);
    break;

  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("m.direction=%d"), m.m_direction);
  }
}

