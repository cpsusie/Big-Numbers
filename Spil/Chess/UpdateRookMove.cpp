#include "stdafx.h"

void Game::updateGameMoveRook(const Move &m) {
  Piece *piece = m.m_piece;

  if(m.m_capturedPiece == nullptr) {
    rookLeaveField(m);
    SET_PIECE(m.m_to, piece);
    setNonCapturingRook(m);
  } else {
    capturePiece(m);
    rookLeaveField(m);
    SET_PIECE(m.m_to, piece);
    setCapturingRook(m);
  }

  piece->m_pinnedState = getPinnedState(m.m_to);
}

#if defined(TABLEBASE_BUILDER)
void Game::updateGameBackMoveRook(const Move &m) {
  const Move m1 = m.makeBackMove();
  rookLeaveField(m1);
  SET_PIECE(m.m_from, m.m_piece);
  setNonCapturingRook(m1);
  m.m_piece->m_pinnedState = getPinnedState(m.m_from);
}
#endif

void Game::updateGameCaptureRook(const Move &m) {
  PlayerState     &state  = m_playerState[CURRENTENEMY];
  const FieldInfo &toInfo = s_fieldInfo[m.m_to];

  if(toInfo.m_rowLine.m_lower) {
    CLR_LDA_LEFT( state, toInfo);
  }
  if(toInfo.m_rowLine.m_upper) {
    CLR_LDA_RIGHT(state, toInfo);
  }
  if(toInfo.m_colLine.m_lower) {
    CLR_LDA_DOWN( state, toInfo);
  }
  if(toInfo.m_colLine.m_upper) {
    CLR_LDA_UP(   state, toInfo);
  }
}

void Game::rookLeaveField(const Move &m) {
  const FieldInfo &fromInfo = s_fieldInfo[m.m_from];

  updateKingDir(m);

   // Rook always leaves both diagonals when moving
  if(fromInfo.m_innerField) {
    unblockDiag12(fromInfo);
  }

  switch(m.m_direction) {
  case MD_LEFT :
  case MD_RIGHT:
    LDAleaveColumn(fromInfo);
    break;

  case MD_DOWN :
  case MD_UP   :
    LDAleaveRow(   fromInfo);
    break;

  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("m.direction=%d"), m.m_direction);
  }

  switch(PLAYERINTURN) {
  case WHITEPLAYER:
    { switch(m.m_from) {
      case A1:
        m_gameKey.d.m_castleState[WHITEPLAYER] &= ~LONGCASTLE_ALLOWED;
        break;

      case H1:
        m_gameKey.d.m_castleState[WHITEPLAYER] &= ~SHORTCASTLE_ALLOWED;
        break;
      }
    }
    break;
  case BLACKPLAYER:
    { switch(m.m_from) {
      case A8:
        m_gameKey.d.m_castleState[BLACKPLAYER] &= ~LONGCASTLE_ALLOWED;
        break;

      case H8:
        m_gameKey.d.m_castleState[BLACKPLAYER] &= ~SHORTCASTLE_ALLOWED;
        break;
      }
    }
    break;
  }

  SET_EMPTYFIELD(m.m_from);
}

void Game::setNonCapturingRook(const Move &m) {
  const FieldInfo &toInfo = s_fieldInfo[m.m_to];

  if(toInfo.m_innerField) {
    blockDiag12(toInfo);
  }

  switch(m.m_direction) {
  case MD_LEFT   :
    LDAwalkLeft(              toInfo, m.m_from);
    LDAenterColumn(           toInfo);
    break;

  case MD_RIGHT  :
    LDAwalkRight(             toInfo, m.m_from);
    LDAenterColumn(           toInfo);
    break;

  case MD_DOWN   :
    LDAwalkDown(              toInfo, m.m_from);
    LDAenterRow(              toInfo);
    break;

  case MD_UP     :
    LDAwalkUp(                toInfo, m.m_from);
    LDAenterRow(              toInfo);
    break;

  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("m.direction=%d"), m.m_direction);
  }
}

void Game::setCapturingRook(const Move &m) {
  const FieldInfo &toInfo = s_fieldInfo[m.m_to];

  switch(m.m_direction) {
  case MD_LEFT   :
    LDAwalkLeftCapture(       toInfo, m.m_from);
    LDAenterColumnCapture(    toInfo);
    break;

  case MD_RIGHT  :
    LDAwalkRightCapture(      toInfo, m.m_from);
    LDAenterColumnCapture(    toInfo);
    break;

  case MD_DOWN   :
    LDAwalkDownCapture(       toInfo, m.m_from);
    LDAenterRowCapture(       toInfo);
    break;

  case MD_UP     :
    LDAwalkUpCapture(         toInfo, m.m_from);
    LDAenterRowCapture(       toInfo);
    break;

  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("m.direction=%d"), m.m_direction);
  }
}
