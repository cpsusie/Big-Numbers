#include "stdafx.h"

void Game::updateGameMoveKing(const Move &m) {
  switch(m.m_type) {
  case NORMALMOVE   :
    updateGameNormalKingMove(m);
    break;

  case SHORTCASTLING:
    updateGameShortCastling();
    break;

  case LONGCASTLING :
    updateGameLongCastling();
    break;

  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("moveType=%d"), m.m_type);
  }
}

#ifdef TABLEBASE_BUILDER
void Game::updateGameBackMoveKing(const Move &m) {
  const Move m1 = m.makeBackMove();
  kingLeaveField(m1);
  SET_PIECE(m.m_from, m.m_piece);
  setKingBackMove(m1);
}

void Game::setKingBackMove(const Move &m) {
  const FieldInfo &toInfo  = fieldInfo[m.m_to];
  PlayerState     &state   = m_playerState[PLAYERINTURN];
  AttackInfo      &oldKing = ATTINFO(state, m.m_from);
  AttackInfo      &newKing = ATTINFO(state, m.m_to);

  switch(m.m_direction) {
  case MD_LEFT     :  // Enter column and both diagonals
    { kingEnterColumn(   toInfo);
      kingEnterDiag12(   toInfo);
      if(toInfo.m_innerRow) {
        blockColumn(toInfo);
      }
      if(toInfo.m_innerField) {
        blockDiag12(toInfo);
      }
      ATT_RIGHT(    oldKing) = 0;
      ATT_LEFT(     newKing) = ATT_LEFT(     oldKing);
    }
    break;

  case MD_RIGHT    :  // Enter column and both diagonals
    { kingEnterColumn(   toInfo);
      kingEnterDiag12(   toInfo);
      if(toInfo.m_innerRow) {
        blockColumn(toInfo);
      }
      if(toInfo.m_innerField) {
        blockDiag12(toInfo);
      }
      ATT_LEFT(     oldKing) = 0;
      ATT_RIGHT(    newKing) = ATT_RIGHT(    oldKing);
    }
    break;

  case MD_DOWN     :  // Enter row and both diagonals
    { kingEnterRow(      toInfo);
      kingEnterDiag12(   toInfo);
      if(toInfo.m_innerCol) {
        blockRow(toInfo);
      }
      if(toInfo.m_innerField) {
        blockDiag12(toInfo);
      }
      ATT_UP(       oldKing) = 0;
      ATT_DOWN(     newKing) = ATT_DOWN(     oldKing);
    }
    break;

  case MD_UP       :  // Enter row and both diagonals
    { kingEnterRow(      toInfo);
      kingEnterDiag12(   toInfo);
      if(toInfo.m_innerCol) {
        blockRow(toInfo);
      }
      if(toInfo.m_innerField) {
        blockDiag12(toInfo);
      }
      ATT_DOWN(     oldKing) = 0;
      ATT_UP(       newKing) = ATT_UP(       oldKing);
    }
    break;

  case MD_DOWNDIAG1:  // Enter row, column and diag2
    { kingEnterRC(       toInfo);
      kingEnterDiag2(    toInfo);
      if(toInfo.m_innerCol) {
        blockRow(toInfo);
      }
      if(toInfo.m_innerRow) {
        blockColumn(toInfo);
      }
      if(toInfo.m_innerField) {
        blockDiag2(toInfo);
      }
      ATT_UPDIAG1(  oldKing) = 0;
      ATT_DOWNDIAG1(newKing) = ATT_DOWNDIAG1(oldKing);
    }
    break;

  case MD_UPDIAG1  :  // Enter row, column and diag2
    { kingEnterRC(       toInfo);
      kingEnterDiag2(    toInfo);
      if(toInfo.m_innerCol) {
        blockRow(toInfo);
      }
      if(toInfo.m_innerRow) {
        blockColumn(toInfo);
      }
      if(toInfo.m_innerField) {
        blockDiag2(toInfo);
      }
      ATT_DOWNDIAG1(oldKing) = 0;
      ATT_UPDIAG1(  newKing) = ATT_UPDIAG1(  oldKing);
    }
    break;

  case MD_DOWNDIAG2:  // Enter row, column and diag1
    { kingEnterRC(       toInfo);
      kingEnterDiag1(    toInfo);
      if(toInfo.m_innerCol) {
        blockRow(toInfo);
      }
      if(toInfo.m_innerRow) {
        blockColumn(toInfo);
      }
      if(toInfo.m_innerField) {
        blockDiag1(toInfo);
      }
      ATT_UPDIAG2(  oldKing) = 0;
      ATT_DOWNDIAG2(newKing) = ATT_DOWNDIAG2(oldKing);
    }
    break;

  case MD_UPDIAG2  :  // Enter row, column and diag1
    { kingEnterRC(       toInfo);
      kingEnterDiag1(    toInfo);
      if(toInfo.m_innerCol) {
        blockRow(toInfo);
      }
      if(toInfo.m_innerRow) {
        blockColumn(toInfo);
      }
      if(toInfo.m_innerField) {
        blockDiag1(toInfo);
      }
      ATT_DOWNDIAG2(oldKing) = 0;
      ATT_UPDIAG2(  newKing) = ATT_UPDIAG2(  oldKing);
    }
    break;

  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("m.direction=%d"), m.m_direction);
  }

  KING_DIRECTION(state, m.m_to) = MD_NONE;
  UPDATE_KINGATTACKS(PLAYERINTURN, toInfo, 1);
}
#endif

void Game::updateGameCaptureKing(const Move &m) {
  throwException(_T("It should not be possible to capture the king!!"));
}

void Game::updateGameNormalKingMove(const Move &m) {
  Piece *piece = m.m_piece;

  if(m.m_capturedPiece == NULL) {
    kingLeaveField(m);
    // King never blocks any lines on the new position, which would imply he goes into a check!
    SET_PIECE(m.m_to, piece);
    setNonCapturingKing(m);
  } else {
    capturePiece(m);
    kingLeaveField(m);
    SET_PIECE(m.m_to, piece);
    setCapturingKing(m);
  }
}

void Game::kingLeaveField(const Move &m) {
  const FieldInfo &fromInfo = fieldInfo[m.m_from];

  updateKingDirKingMove(m);

  switch(m.m_direction) {
  case MD_LEFT     :  // Leave column and both diagonals
  case MD_RIGHT    :
    { kingLeaveColumn(   fromInfo);
      kingLeaveDiag12(   fromInfo);
    }
    break;

  case MD_DOWN     :  // Leave row and both diagonals
  case MD_UP       :
    { kingLeaveRow(      fromInfo);
      kingLeaveDiag12(   fromInfo);
    }
    break;

  case MD_DOWNDIAG1:  // Leave row, column and diag2
  case MD_UPDIAG1  :
    { kingLeaveRC(       fromInfo);
      kingLeaveDiag2(    fromInfo);
    }
    break;

  case MD_DOWNDIAG2:  // Leave row, column and diag1
  case MD_UPDIAG2  :
    { kingLeaveRC(       fromInfo);
      kingLeaveDiag1(    fromInfo);
    }
    break;

  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("m.direction=%d"), m.m_direction);
  }

  KING_DIRECTION(m_playerState[PLAYERINTURN],m.m_from) = m.m_direction;
  m_gameKey.d.m_castleState[PLAYERINTURN] &= ~(SHORTCASTLE_ALLOWED | LONGCASTLE_ALLOWED);

  UPDATE_KINGATTACKS(PLAYERINTURN, fromInfo, -1);
  SET_EMPTYFIELD(m.m_from);
}

void Game::setNonCapturingKing(const Move &m) {
  const FieldInfo &toInfo  = fieldInfo[m.m_to];
  PlayerState     &state   = m_playerState[PLAYERINTURN];
  AttackInfo      &oldKing = ATTINFO(state, m.m_from);
  AttackInfo      &newKing = ATTINFO(state, m.m_to);

  switch(m.m_direction) {
  case MD_LEFT     :  // Enter column and both diagonals
    { kingEnterColumn(   toInfo);
      kingEnterDiag12(   toInfo);
      ATT_RIGHT(    oldKing) = 0;
      ATT_LEFT(     newKing) = ATT_LEFT(     oldKing);
    }
    break;

  case MD_RIGHT    :  // Enter column and both diagonals
    { kingEnterColumn(   toInfo);
      kingEnterDiag12(   toInfo);
      ATT_LEFT(     oldKing) = 0;
      ATT_RIGHT(    newKing) = ATT_RIGHT(    oldKing);
    }
    break;

  case MD_DOWN     :  // Enter row and both diagonals
    { kingEnterRow(      toInfo);
      kingEnterDiag12(   toInfo);
      ATT_UP(       oldKing) = 0;
      ATT_DOWN(     newKing) = ATT_DOWN(     oldKing);
    }
    break;

  case MD_UP       :  // Enter row and both diagonals
    { kingEnterRow(      toInfo);
      kingEnterDiag12(   toInfo);
      ATT_DOWN(     oldKing) = 0;
      ATT_UP(       newKing) = ATT_UP(       oldKing);
    }
    break;

  case MD_DOWNDIAG1:  // Enter row, column and diag2
    { kingEnterRC(       toInfo);
      kingEnterDiag2(    toInfo);
      ATT_UPDIAG1(  oldKing) = 0;
      ATT_DOWNDIAG1(newKing) = ATT_DOWNDIAG1(oldKing);
    }
    break;

  case MD_UPDIAG1  :  // Enter row, column and diag2
    { kingEnterRC(       toInfo);
      kingEnterDiag2(    toInfo);
      ATT_DOWNDIAG1(oldKing) = 0;
      ATT_UPDIAG1(  newKing) = ATT_UPDIAG1(  oldKing);
    }
    break;

  case MD_DOWNDIAG2:  // Enter row, column and diag1
    { kingEnterRC(       toInfo);
      kingEnterDiag1(    toInfo);
      ATT_UPDIAG2(  oldKing) = 0;
      ATT_DOWNDIAG2(newKing) = ATT_DOWNDIAG2(oldKing);
    }
    break;

  case MD_UPDIAG2  :  // Enter row, column and diag1
    { kingEnterRC(       toInfo);
      kingEnterDiag1(    toInfo);
      ATT_DOWNDIAG2(oldKing) = 0;
      ATT_UPDIAG2(  newKing) = ATT_UPDIAG2(  oldKing);
    }
    break;

  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("m.direction=%d"), m.m_direction);
  }

  KING_DIRECTION(state, m.m_to) = MD_NONE;
  UPDATE_KINGATTACKS(PLAYERINTURN, toInfo, 1);
}

void Game::setCapturingKing(const Move &m) {
  const FieldInfo &toInfo  = fieldInfo[m.m_to];
  PlayerState     &state   = m_playerState[PLAYERINTURN];
  AttackInfo      &oldKing = ATTINFO(state, m.m_from);
  AttackInfo      &newKing = ATTINFO(state, m.m_to);

  switch(m.m_direction) {
  case MD_LEFT     : // Enter column and both diagonals
    { kingEnterColumnCapture(toInfo);
      kingEnterDiag12Capture(toInfo);
      ATT_RIGHT(    oldKing) = 0;
      ATT_LEFT(     newKing) = ATT_LEFT(     oldKing);
      if(toInfo.m_rowLine.m_lower) {
        setKingRight(state,toInfo);
      }
    }
    break;

  case MD_RIGHT    : // Enter column and both diagonals
    { kingEnterColumnCapture(toInfo);
      kingEnterDiag12Capture(toInfo);
      ATT_LEFT(     oldKing) = 0;
      ATT_RIGHT(    newKing) = ATT_RIGHT(    oldKing);
      if(toInfo.m_rowLine.m_upper) {
        setKingLeft(state,toInfo);
      }
    }
    break;

  case MD_DOWN     : // Enter row and both diagonals
    { kingEnterRowCapture(   toInfo);
      kingEnterDiag12Capture(toInfo);
      ATT_UP(       oldKing) = 0;
      ATT_DOWN(     newKing) = ATT_DOWN(     oldKing);
      if(toInfo.m_colLine.m_lower) {
        setKingUp(state,toInfo);
      }
    }
    break;

  case MD_UP       : // Enter row and both diagonals
    { kingEnterRowCapture(   toInfo);
      kingEnterDiag12Capture(toInfo);
      ATT_DOWN(     oldKing) = 0;
      ATT_UP(       newKing) = ATT_UP(       oldKing);
      if(toInfo.m_colLine.m_upper) {
        setKingDown(state, toInfo);
      }
    }
    break;

  case MD_DOWNDIAG1: // Enter row, column and diag2
    { kingEnterRCCapture(    toInfo);
      kingEnterDiag2Capture( toInfo);
      ATT_UPDIAG1(  oldKing) = 0;
      ATT_DOWNDIAG1(newKing) = ATT_DOWNDIAG1(oldKing);
      if(toInfo.m_diag1Line.m_lower) {
        setKingUpDiag1(state, toInfo);
      }
    }
    break;

  case MD_UPDIAG1  : // Enter row, column and diag2
    { kingEnterRCCapture(    toInfo);
      kingEnterDiag2Capture( toInfo);
      ATT_DOWNDIAG1(oldKing) = 0;
      ATT_UPDIAG1(  newKing) = ATT_UPDIAG1(  oldKing);
      if(toInfo.m_diag1Line.m_upper) {
        setKingDownDiag1(state, toInfo);
      }
    }
    break;

  case MD_DOWNDIAG2: // Enter row, column and diag1
    { kingEnterRCCapture(    toInfo);
      kingEnterDiag1Capture( toInfo);
      ATT_UPDIAG2(  oldKing) = 0;
      ATT_DOWNDIAG2(newKing) = ATT_DOWNDIAG2(oldKing);
      if(toInfo.m_diag2Line.m_lower) {
        setKingUpDiag2(state, toInfo);
      }
    }
    break;

  case MD_UPDIAG2  : // Enter row, column and diag1
    { kingEnterRCCapture(    toInfo);
      kingEnterDiag1Capture( toInfo);
      ATT_DOWNDIAG2(oldKing) = 0;
      ATT_UPDIAG2(  newKing) = ATT_UPDIAG2(  oldKing);
      if(toInfo.m_diag2Line.m_upper) {
        setKingDownDiag2(state, toInfo);
      }
    }
    break;

  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("m.direction=%d"), m.m_direction);
  }

  KING_DIRECTION(state, m.m_to) = MD_NONE;
  UPDATE_KINGATTACKS(PLAYERINTURN, toInfo, 1);
}

void Game::updateGameShortCastling() {
  switch(PLAYERINTURN) {
  case WHITEPLAYER:
    { PlayerState     &state  = m_playerState[WHITEPLAYER];
      const FieldInfo &e1Info = fieldInfo[E1];

      kingLeaveColumn( e1Info);
      kingLeaveDiag12( e1Info);
      UPDATE_KINGATTACKS(PLAYERINTURN, e1Info, -1);
      SET_NOKING_RIGHT(state, e1Info);

      LDAleaveColumn(fieldInfo[H1]);

      MOVEPIECE_UPDATEKEY(E1, G1);
      MOVEPIECE_UPDATEKEY(H1, F1);

      const FieldInfo &g1Info = fieldInfo[G1];
      kingEnterColumn( g1Info);
      kingEnterDiag12( g1Info);
      UPDATE_KINGATTACKS(PLAYERINTURN, g1Info, 1);
      KING_DIRECTION(state, F1) = MD_RIGHT;
      KING_DIRECTION(state, G1) = MD_NONE;

      LDAenterColumn(fieldInfo[F1]);

      AttackInfo &oldKing  = ATTINFO(state, E1);
      AttackInfo &newKing  = ATTINFO(state, G1);
      AttackInfo &rook     = ATTINFO(state, F1);
      ATT_LEFT( oldKing)   = 1;
      ATT_RIGHT(rook   )   = ATT_RIGHT(oldKing);
      ATT_LEFT( rook   )   = 0;
      ATT_RIGHT(newKing)   = 1;
      ATT_LEFT( newKing)   = 0;
      SET_LDA_LEFT(state, e1Info);

      m_gameKey.d.m_castleState[WHITEPLAYER] = SHORTCASTLE_DONE;
    }
    break;

  case BLACKPLAYER:
    { PlayerState     &state  = m_playerState[BLACKPLAYER];
      const FieldInfo &e8Info = fieldInfo[E8];

      kingLeaveColumn( e8Info);
      kingLeaveDiag12( e8Info);
      UPDATE_KINGATTACKS(PLAYERINTURN, e8Info, -1);
      SET_NOKING_RIGHT(state, e8Info);

      LDAleaveColumn(fieldInfo[H8]);

      MOVEPIECE_UPDATEKEY(E8, G8);
      MOVEPIECE_UPDATEKEY(H8, F8);

      const FieldInfo &g8Info = fieldInfo[G8];
      kingEnterColumn( g8Info);
      kingEnterDiag12( g8Info);
      UPDATE_KINGATTACKS(PLAYERINTURN, g8Info, 1);
      KING_DIRECTION(state, F8) = MD_RIGHT;
      KING_DIRECTION(state, G8) = MD_NONE;

      LDAenterColumn(fieldInfo[F8]);

      AttackInfo &oldKing  = ATTINFO(state, E8);
      AttackInfo &newKing  = ATTINFO(state, G8);
      AttackInfo &rook     = ATTINFO(state, F8);
      ATT_LEFT( oldKing)   = 1;
      ATT_RIGHT(rook   )   = ATT_RIGHT(oldKing);
      ATT_LEFT( rook   )   = 0;
      ATT_RIGHT(newKing)   = 1;
      ATT_LEFT( newKing)   = 0;
      SET_LDA_LEFT(state, e8Info);

      m_gameKey.d.m_castleState[BLACKPLAYER] = SHORTCASTLE_DONE;
    }
    break;
  }
}

void Game::updateGameLongCastling() {
  switch(PLAYERINTURN) {
  case WHITEPLAYER:
    { PlayerState     &state  = m_playerState[WHITEPLAYER];
      const FieldInfo &e1Info = fieldInfo[E1];

      kingLeaveColumn( e1Info);
      kingLeaveDiag12( e1Info);
      UPDATE_KINGATTACKS(PLAYERINTURN, e1Info, -1);
      SET_NOKING_LEFT(state, e1Info);

      LDAleaveColumn(fieldInfo[A1]);

      MOVEPIECE_UPDATEKEY(E1, C1);
      MOVEPIECE_UPDATEKEY(A1, D1);

      const FieldInfo &c1Info = fieldInfo[C1];
      kingEnterColumn( c1Info);
      kingEnterDiag12( c1Info);
      UPDATE_KINGATTACKS(PLAYERINTURN, c1Info, 1);
      KING_DIRECTION(state, D1) = MD_LEFT;
      KING_DIRECTION(state, C1) = MD_NONE;

      LDAenterColumn(fieldInfo[D1]);

      AttackInfo &oldKing  = ATTINFO(state, E1);
      AttackInfo &newKing  = ATTINFO(state, C1);
      AttackInfo &rook     = ATTINFO(state, D1);
      ATT_RIGHT(oldKing)   = 1;
      ATT_LEFT( rook   )   = ATT_LEFT(oldKing);
      ATT_RIGHT(rook   )   = 0;
      ATT_LEFT( newKing)   = 1;
      ATT_RIGHT(newKing)   = 0;
      LDA_RIGHT(state, B1) = 0;
      SET_LDA_RIGHT(state, e1Info);

      m_gameKey.d.m_castleState[WHITEPLAYER] = LONGCASTLE_DONE;
    }
    break;

  case BLACKPLAYER:
    { PlayerState     &state  = m_playerState[BLACKPLAYER];
      const FieldInfo &e8Info = fieldInfo[E8];

      kingLeaveColumn( e8Info);
      kingLeaveDiag12( e8Info);
      UPDATE_KINGATTACKS(PLAYERINTURN, e8Info, -1);
      SET_NOKING_LEFT(state, e8Info);

      LDAleaveColumn(fieldInfo[A8]);

      MOVEPIECE_UPDATEKEY(E8, C8);
      MOVEPIECE_UPDATEKEY(A8, D8);

      const FieldInfo &c8Info = fieldInfo[C8];
      kingEnterColumn( c8Info);
      kingEnterDiag12( c8Info);
      UPDATE_KINGATTACKS(PLAYERINTURN, c8Info, 1);
      KING_DIRECTION(state, D8) = MD_LEFT;
      KING_DIRECTION(state, C8) = MD_NONE;

      LDAenterColumn(fieldInfo[D8]);

      AttackInfo &oldKing  = ATTINFO(state, E8);
      AttackInfo &newKing  = ATTINFO(state, C8);
      AttackInfo &rook     = ATTINFO(state, D8);
      ATT_RIGHT(oldKing)   = 1;
      ATT_LEFT( rook   )   = ATT_LEFT(oldKing);
      ATT_RIGHT(rook   )   = 0;
      ATT_LEFT( newKing)   = 1;
      ATT_RIGHT(newKing)   = 0;
      LDA_RIGHT(state, B8) = 0;
      SET_LDA_RIGHT(state, e8Info);

      m_gameKey.d.m_castleState[BLACKPLAYER] = LONGCASTLE_DONE;
    }
    break;
  }
}
