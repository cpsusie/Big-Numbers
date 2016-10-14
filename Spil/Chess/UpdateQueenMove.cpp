#include "stdafx.h"

void Game::updateGameMoveQueen(const Move &m) {
  Piece *piece = m.m_piece;

  if(m.m_capturedPiece == NULL) {
    queenLeaveField(m);
    SET_PIECE(m.m_to, piece);
    setNonCapturingQueen(m);
  } else {
    capturePiece(m);
    queenLeaveField(m);
    SET_PIECE(m.m_to, piece);
    setCapturingQueen(m);
  }

  piece->m_pinnedState = getPinnedState(m.m_to);
}

#ifdef TABLEBASE_BUILDER
void Game::updateGameBackMoveQueen(const Move &m) {
  const Move m1 = m.makeBackMove();
  queenLeaveField(m1);
  SET_PIECE(m.m_from, m.m_piece);
  setNonCapturingQueen(m1);
  m.m_piece->m_pinnedState = getPinnedState(m.m_from);
}
#endif

void Game::updateGameCaptureQueen(const Move &m) {
  PlayerState     &state  = m_playerState[CURRENTENEMY];
  const FieldInfo &toInfo = fieldInfo[m.m_to];

  if(toInfo.m_rowLine.m_lower) {
    CLR_LDA_LEFT(     state, toInfo);
  }
  if(toInfo.m_rowLine.m_upper) {
    CLR_LDA_RIGHT(    state, toInfo);
  }
  if(toInfo.m_colLine.m_lower) {
    CLR_LDA_DOWN(     state, toInfo);
  }
  if(toInfo.m_colLine.m_upper) {
    CLR_LDA_UP(       state, toInfo);
  }
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
}

void Game::queenLeaveField(const Move &m) {
  const FieldInfo &fromInfo = fieldInfo[m.m_from];

  updateKingDir(m);
  
  switch(m.m_direction) {
  case MD_LEFT    :   // Leave column and both diagonals
  case MD_RIGHT   :
    LDAleaveColumn(         fromInfo);
    LDAleaveDiag12(         fromInfo);
    break;

  case MD_DOWN    :   // Leave row and both diagonals
  case MD_UP      :
    LDAleaveRow(            fromInfo);
    LDAleaveDiag12(         fromInfo);
    break;

  case MD_UPDIAG1  :  // Leave row, column and diag2
  case MD_DOWNDIAG1:
    LDAleaveRC(             fromInfo);
    LDAleaveDiag2(          fromInfo);
    break;

  case MD_UPDIAG2  :  // Leave row, column and diag1
  case MD_DOWNDIAG2:
    LDAleaveRC(             fromInfo);
    LDAleaveDiag1(          fromInfo);
    break;

  default          :
    throwInvalidArgumentException(__TFUNCTION__, _T("m.direction=%d"), m.m_direction);
  }

  SET_EMPTYFIELD(m.m_from);
}

void Game::setNonCapturingQueen(const Move &m) {
  const FieldInfo &toInfo = fieldInfo[m.m_to];

  switch(m.m_direction) {
  case MD_LEFT     :
    LDAwalkLeft(            toInfo, m.m_from);
    LDAenterColumn(         toInfo);
    LDAenterDiag12(         toInfo);
    break;
    
  case MD_RIGHT    :
    LDAwalkRight(           toInfo, m.m_from);
    LDAenterColumn(         toInfo);
    LDAenterDiag12(         toInfo);
    break;

  case MD_DOWN     :
    LDAwalkDown(            toInfo, m.m_from);
    LDAenterRow(            toInfo);
    LDAenterDiag12(         toInfo);
    break;

  case MD_UP       :
    LDAwalkUp(              toInfo, m.m_from);
    LDAenterRow(            toInfo);
    LDAenterDiag12(         toInfo);
    break;

  case MD_DOWNDIAG1:
    LDAwalkDownDiag1(       toInfo, m.m_from);
    LDAenterRC(             toInfo);
    LDAenterDiag2(          toInfo);
    break;
    
  case MD_UPDIAG1  :
    LDAwalkUpDiag1(         toInfo, m.m_from);
    LDAenterRC(             toInfo);
    LDAenterDiag2(          toInfo);
    break;

  case MD_DOWNDIAG2:
    LDAwalkDownDiag2(       toInfo, m.m_from);
    LDAenterRC(             toInfo);
    LDAenterDiag1(          toInfo);
    break;

  case MD_UPDIAG2  :
    LDAwalkUpDiag2(         toInfo, m.m_from);
    LDAenterRC(             toInfo);
    LDAenterDiag1(          toInfo);
    break;

  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("m.direction=%d"), m.m_direction);
  }
}

void Game::setCapturingQueen(const Move &m) {
  const FieldInfo &toInfo = fieldInfo[m.m_to];

  switch(m.m_direction) {
  case MD_LEFT     :
    LDAwalkLeftCapture(     toInfo, m.m_from);
    LDAenterColumnCapture(  toInfo);
    LDAenterDiag12Capture(  toInfo);
    break;

  case MD_RIGHT    :
    LDAwalkRightCapture(    toInfo, m.m_from);
    LDAenterColumnCapture(  toInfo);
    LDAenterDiag12Capture(  toInfo);
    break;

  case MD_DOWN     :
    LDAwalkDownCapture(     toInfo, m.m_from);
    LDAenterRowCapture(     toInfo);
    LDAenterDiag12Capture(  toInfo);
    break;

  case MD_UP       :
    LDAwalkUpCapture(       toInfo, m.m_from);
    LDAenterRowCapture(     toInfo);
    LDAenterDiag12Capture(  toInfo);
    break;

  case MD_DOWNDIAG1:
    LDAwalkDownDiag1Capture(toInfo, m.m_from);
    LDAenterRCCapture(      toInfo);
    LDAenterDiag2Capture(   toInfo);
    break;

  case MD_UPDIAG1  :
    LDAwalkUpDiag1Capture(  toInfo, m.m_from);
    LDAenterRCCapture(      toInfo);
    LDAenterDiag2Capture(   toInfo);
    break;

  case MD_DOWNDIAG2:
    LDAwalkDownDiag2Capture(toInfo, m.m_from);
    LDAenterRCCapture(      toInfo);
    LDAenterDiag1Capture(   toInfo);
    break;

  case MD_UPDIAG2  :
    LDAwalkUpDiag2Capture(  toInfo, m.m_from);
    LDAenterRCCapture(      toInfo);
    LDAenterDiag1Capture(   toInfo);
    break;

  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("m.direction=%d"), m.m_direction);
  }
}
