#include "stdafx.h"

static const int whitePawnPositionScore[64] = {
    0 ,   0,   0,   0,   0,   0,   0,   0   // A1 - H1
 ,  0 ,   0,   0,   0,   0,   6,  10,  10   // A2 - H2
 ,  2 ,   2,   2,   1,   1,   2,   2,   2   // A3 - H3
 ,  2 ,   2,   4,   5,   5,   4,   2,   2   // A4 - H4
 ,  3 ,   3,   5,  10,  10,   5,   3,   3   // A5 - H5
 , 30 ,  30,  30,  30,  30,  30,  30,  30   // A6 - H6
 ,200 , 200, 200, 200, 200, 200, 200, 200   // A7 - H7
 ,  0 ,   0,   0,   0,   0,   0,   0,   0   // A8 - H8
};

static const int blackPawnPositionScore[64] = {
    0 ,   0,   0,   0,   0,   0,   0,   0   // A1 - H1
 ,200 , 200, 200, 200, 200, 200, 200, 200   // A2 - H2
 , 30 ,  30,  30,  30,  30,  30,  30,  30   // A3 - H3
  , 3 ,   3,   5,  10,  10,   5,   3,   3   // A4 - H4
  , 2 ,   2,   4,   5,   5,   4,   2,   2   // A5 - H5
  , 2 ,   2,   2,   1,   1,   2,   2,   2   // A6 - H6
  , 0 ,   0,   0,   0,   0,   6,  10,  10   // A7 - H7
  , 0 ,   0,   0,   0,   0,   0,   0,   0   // A8 - H8
};

const int *MoveTable::pawnPositionScore[2] = {
  whitePawnPositionScore
 ,blackPawnPositionScore
};

void Game::updateGameMovePawn(const Move &m) {
#ifndef TABLEBASE_BUILDER
  m_lastCaptureOrPawnMove = m_stackSize;
#endif

  switch(m.m_type) {
  case NORMALMOVE:
    updateGameNormalPawnMove(m);
    break;

  case PROMOTION:
    updateGamePromotion(m);
    break;

  case ENPASSANT:
    updateGameEP(m);
    break;

  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("moveType=%d"), m.m_type);
  }
}

#ifdef TABLEBASE_BUILDER
void Game::updateGameBackMovePawn(const Move &m) {
  const Move m1 = m.makeBackMove();
  const FieldInfo &fromInfo = s_fieldInfo[m1.m_from];

  updateKingDir(m1);

  // Pawn always leaves the row when moving
  if(fromInfo.m_innerCol) {
    unblockRow(fromInfo);
  }

  switch(m1.m_direction) {
  case MD_DOWN     : // When going backwards, the pawn leaves both diagonals
    if(fromInfo.m_innerField) {
      unblockDiag12(fromInfo);
    }
    UPDATE_WHITEPAWNATTACKS(fromInfo, -1);
    break;

  case MD_UP       :
    if(fromInfo.m_innerField) {
      unblockDiag12(fromInfo);
    }
    UPDATE_BLACKPAWNATTACKS(fromInfo, -1);
    break;

  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("m.direction=%d"), m.m_direction);
  }
  SET_EMPTYFIELD(m1.m_from);

  const FieldInfo &toInfo = s_fieldInfo[m1.m_to];

  switch(m1.m_direction) {
  case MD_DOWN     : // white
    { if(toInfo.m_innerCol) {
        blockRow(toInfo);
      }
      whitePawnWalkBackward(toInfo, m1.m_from);
      if(toInfo.m_innerField) {
        blockDiag12(toInfo);
      }
      SET_PIECE(m1.m_to, m1.m_piece);
      UPDATE_WHITEPAWNATTACKS(toInfo, 1);
    }
    break;

  case MD_UP       : // black
    { if(toInfo.m_innerCol) {
        blockRow(toInfo);
      }
      blackPawnWalkBackward(toInfo, m1.m_from);
      if(toInfo.m_innerField) {
        blockDiag12(toInfo);
      }
      SET_PIECE(m1.m_to, m1.m_piece);
      UPDATE_BLACKPAWNATTACKS(toInfo, 1);
    }
    break;
  }
  m1.m_piece->m_pinnedState = getPinnedState(m1.m_to);
}
#endif

void Game::updateGameCapturePawn(const Move &m) {
  const Piece *pawn  = m.m_capturedPiece;

  switch(pawn->getPlayer()) {
  case WHITEPLAYER: UPDATE_WHITEPAWNATTACKS(s_fieldInfo[pawn->m_position], -1); break;
  case BLACKPLAYER: UPDATE_BLACKPAWNATTACKS(s_fieldInfo[pawn->m_position], -1); break;
  }
#ifndef TABLEBASE_BUILDER
  pawn->m_playerState.decrementPawnCount(GETCOL(pawn->m_position));
  pawn->m_playerState.m_positionalScore -= MoveTable::pawnPositionScore[pawn->getPlayer()][pawn->m_position];
#endif
}

// ------------------------------------------------ normal Pawn move ---------------------------------------

void Game::updateGameNormalPawnMove(const Move &m) {
  if(m.m_capturedPiece != NULL) {
    capturePiece(m);
  } else if(((m.m_from^m.m_to) == 0x10) && hasEnemyPawnBeside(m.m_to)) { // 2-step move
    m_gameKey.d.m_EPSquare = m.m_to;
  }
  pawnLeaveField(m);
  setPawn(m);
}

bool Game::hasEnemyPawnBeside(int to) const { // assume to is at 5. row (white to move) or 4. (black to move)
  switch(to) {
  case A5: return m_gameKey.m_pieceKey[B5] == WHITEPAWN;
  case B5:
  case C5:
  case D5:
  case E5:
  case F5:
  case G5: return (m_gameKey.m_pieceKey[to+DPOS_LEFT ] == WHITEPAWN) || (m_gameKey.m_pieceKey[to+DPOS_RIGHT] == WHITEPAWN);
  case H5: return m_gameKey.m_pieceKey[G5] == WHITEPAWN;

  case A4: return m_gameKey.m_pieceKey[B4] == BLACKPAWN;
  case B4:
  case C4:
  case D4:
  case E4:
  case F4:
  case G4: return (m_gameKey.m_pieceKey[to+DPOS_LEFT ] == BLACKPAWN) || (m_gameKey.m_pieceKey[to+DPOS_RIGHT] == BLACKPAWN);
  case H4: return m_gameKey.m_pieceKey[G4] == BLACKPAWN;
  default: throwException(_T("%s:Illegal position:%s"), __TFUNCTION__, getFieldName(to));
           return false;
  }
}

void Game::pawnLeaveField(const Move &m) {
  const FieldInfo &fromInfo = s_fieldInfo[m.m_from];

  updateKingDir(m);

  // Pawn always leaves the row when moving
  if(fromInfo.m_innerCol) {
    unblockRow(fromInfo);
  }

  switch(m.m_direction) {
  case MD_DOWN     : // When going forward, the pawn leaves both diagonals
    if(fromInfo.m_innerField) {
      unblockDiag12(fromInfo);
    }
    UPDATE_BLACKPAWNATTACKS(fromInfo, -1);
    break;

  case MD_UP       :
    if(fromInfo.m_innerField) {
      unblockDiag12(fromInfo);
    }
    UPDATE_WHITEPAWNATTACKS(fromInfo, -1);
    break;

  case MD_DOWNDIAG1:
    unblockColumn(fromInfo); // Always an inner row. Pawns live from row 2(7) to row 7(2).
    if(fromInfo.m_innerField) {
      unblockDiag2(fromInfo);
    }
    UPDATE_BLACKPAWNATTACKS(fromInfo, -1);
    break;

  case MD_UPDIAG1  :
    unblockColumn(fromInfo);
    if(fromInfo.m_innerField) {
      unblockDiag2(fromInfo);
    }
    UPDATE_WHITEPAWNATTACKS(fromInfo, -1);
    break;

  case MD_DOWNDIAG2:
    unblockColumn(fromInfo);
    if(fromInfo.m_innerField) {
      unblockDiag1(fromInfo);
    }
    UPDATE_BLACKPAWNATTACKS(fromInfo, -1);
    break;

  case MD_UPDIAG2  :
    unblockColumn(fromInfo);
    if(fromInfo.m_innerField) {
      unblockDiag1(fromInfo);
    }
    UPDATE_WHITEPAWNATTACKS(fromInfo, -1);
    break;

  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("m.direction=%d"), m.m_direction);
  }
  SET_EMPTYFIELD(m.m_from);
}

void Game::setPawn(const Move &m) {
  const FieldInfo &toInfo = s_fieldInfo[m.m_to];

  switch(m.m_direction) {
  case MD_DOWN     : // black
    { if(toInfo.m_innerCol) {
        blockRow(toInfo);
      }
      blackPawnWalkForward(toInfo, m.m_from);
      if(toInfo.m_innerField) {
        blockDiag12(toInfo);
      }
      SET_PIECE(m.m_to, m.m_piece);
      UPDATE_BLACKPAWNATTACKS(toInfo, 1);
#ifndef TABLEBASE_BUILDER
      m_playerState[BLACKPLAYER].m_positionalScore += blackPawnPositionScore[m.m_to] - blackPawnPositionScore[m.m_from];
#endif
    }
    break;

  case MD_UP       : // white
    { if(toInfo.m_innerCol) {
        blockRow(toInfo);
      }
      whitePawnWalkForward(toInfo, m.m_from);
      if(toInfo.m_innerField) {
        blockDiag12(toInfo);
      }
      SET_PIECE(m.m_to, m.m_piece);
      UPDATE_WHITEPAWNATTACKS(toInfo, 1);
#ifndef TABLEBASE_BUILDER
      m_playerState[WHITEPLAYER].m_positionalScore += whitePawnPositionScore[m.m_to] - whitePawnPositionScore[m.m_from];
#endif
    }
    break;

  case MD_DOWNDIAG1: // black. This is a capture. Doesn't block any fields
    { PlayerState &whiteState = m_playerState[WHITEPLAYER];
      PlayerState &blackState = m_playerState[BLACKPLAYER];
      SET_PIECE(m.m_to, m.m_piece);
      if(GET_LDA_DOWNDIAG1(whiteState,m.m_from)) {
        LDA_DOWNDIAG1(whiteState,m.m_to) = 1;
      } else if(GET_LDA_DOWNDIAG1(blackState,m.m_from)) {
        LDA_DOWNDIAG1(blackState,m.m_to) = 1;
      }
      UPDATE_BLACKPAWNATTACKS(toInfo, 1);
#ifndef TABLEBASE_BUILDER
      blackState.decrementPawnCount(GETCOL(m.m_from));
      blackState.incrementPawnCount(GETCOL(m.m_to  ));
      blackState.m_positionalScore += blackPawnPositionScore[m.m_to] - blackPawnPositionScore[m.m_from];
#endif
    }
    break;

  case MD_DOWNDIAG2: // black. This is a capture. Doesn't block any fields
    { PlayerState &whiteState = m_playerState[WHITEPLAYER];
      PlayerState &blackState = m_playerState[BLACKPLAYER];
      SET_PIECE(m.m_to, m.m_piece);
      if(GET_LDA_DOWNDIAG2(whiteState,m.m_from)) {
        LDA_DOWNDIAG2(whiteState,m.m_to) = 1;
      } else if(GET_LDA_DOWNDIAG2(blackState,m.m_from)) {
        LDA_DOWNDIAG2(blackState,m.m_to) = 1;
      }
      UPDATE_BLACKPAWNATTACKS(toInfo, 1);
#ifndef TABLEBASE_BUILDER
      blackState.decrementPawnCount(GETCOL(m.m_from));
      blackState.incrementPawnCount(GETCOL(m.m_to  ));
      blackState.m_positionalScore += blackPawnPositionScore[m.m_to] - blackPawnPositionScore[m.m_from];
#endif
    }
    break;

  case MD_UPDIAG1  : // white. This is a capture. Doesn't block any fields
    { PlayerState &whiteState = m_playerState[WHITEPLAYER];
      PlayerState &blackState = m_playerState[BLACKPLAYER];
      SET_PIECE(m.m_to, m.m_piece);
      if(GET_LDA_UPDIAG1(whiteState,m.m_from)) {
        LDA_UPDIAG1(whiteState,m.m_to) = 1;
      } else if(GET_LDA_UPDIAG1(blackState,m.m_from)) {
        LDA_UPDIAG1(blackState,m.m_to) = 1;
      }
      UPDATE_WHITEPAWNATTACKS(toInfo, 1);
#ifndef TABLEBASE_BUILDER
      whiteState.decrementPawnCount(GETCOL(m.m_from));
      whiteState.incrementPawnCount(GETCOL(m.m_to  ));
      whiteState.m_positionalScore += whitePawnPositionScore[m.m_to] - whitePawnPositionScore[m.m_from];
#endif
    }
    break;

  case MD_UPDIAG2  : // white. This is a capture. Doesn't block any fields
    { PlayerState &whiteState = m_playerState[WHITEPLAYER];
      PlayerState &blackState = m_playerState[BLACKPLAYER];
      SET_PIECE(m.m_to, m.m_piece);
      if(GET_LDA_UPDIAG2(whiteState,m.m_from)) {
        LDA_UPDIAG2(whiteState,m.m_to) = 1;
      } else if(GET_LDA_UPDIAG2(blackState,m.m_from)) {
        LDA_UPDIAG2(blackState,m.m_to) = 1;
      }
      UPDATE_WHITEPAWNATTACKS(toInfo, 1);
#ifndef TABLEBASE_BUILDER
      whiteState.decrementPawnCount(GETCOL(m.m_from));
      whiteState.incrementPawnCount(GETCOL(m.m_to  ));
      whiteState.m_positionalScore += whitePawnPositionScore[m.m_to] - whitePawnPositionScore[m.m_from];
#endif
    }
    break;

  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("m.direction=%d"), m.m_direction);
  }

  m.m_piece->m_pinnedState = getPinnedState(m.m_to);
  m.m_piece->m_playerState.m_checkingSDAPosition = m.m_to; // Dont care if we really check the king.
}

void Game::whitePawnWalkForward(const FieldInfo &toInfo, int from) {
  PlayerState &whiteState = m_playerState[WHITEPLAYER];
  PlayerState &blackState = m_playerState[BLACKPLAYER];

  if(GET_LDA_UP(whiteState, from)) {
    LDA_UP(whiteState, toInfo.m_pos) = 1;
    if(GET_LDA_DOWN(whiteState, toInfo.m_pos)) {
      for(int pos = toInfo.m_colLine.m_lower[1];; pos += DPOS_DOWN) {                              // white up, white down
        AttackInfo &whiteInfo = ATTINFO(whiteState,pos);
        ATT_DOWN(whiteInfo) = 0;
        if(pos == from) {
          break;
        }
        ATT_UP(  whiteInfo) = 1;
      }
    } else if(GET_LDA_DOWN(blackState, toInfo.m_pos)) {
      for(int pos = toInfo.m_colLine.m_lower[1];; pos += DPOS_DOWN) {                              // white up, black down
        LDA_DOWN(blackState, pos) = 0;
        if(pos == from) {
          break;
        }
        LDA_UP(  whiteState, pos) = 1;
      }
    } else {
      for(int pos = toInfo.m_colLine.m_lower[1]; pos != from; pos += DPOS_DOWN) {                  // white up, none  down
        LDA_UP(whiteState,pos) = 1;
      }
    }
  } else if(GET_LDA_UP(blackState, from)) {
    LDA_UP(blackState, toInfo.m_pos) = 1;
    if(GET_LDA_DOWN(whiteState, toInfo.m_pos)) {
      for(int pos = toInfo.m_colLine.m_lower[1];; pos += DPOS_DOWN) {                              // black up, white down
        LDA_DOWN(whiteState, pos) = 0;
        if(pos == from) {
          break;
        }
        LDA_UP(  blackState, pos) = 1;
      }
    } else if(GET_LDA_DOWN(blackState, toInfo.m_pos)) {
      for(int pos = toInfo.m_colLine.m_lower[1];; pos += DPOS_DOWN) {                              // black up, black down
        AttackInfo &blackInfo = ATTINFO(blackState, pos);
        ATT_DOWN(blackInfo) = 0;
        if(pos == from) {
          break;
        }
        ATT_UP(blackInfo  ) = 1;
      }
    } else {
      for(int pos = toInfo.m_colLine.m_lower[1]; pos != from; pos += DPOS_DOWN) {                  // black up, none  down
        LDA_UP(blackState, pos) = 1;
      }
    }
  } else if(GET_LDA_DOWN(whiteState, toInfo.m_pos)) {
    for(int pos = toInfo.m_colLine.m_lower[1]; DPOS_DOWN_LE(pos, from); pos += DPOS_DOWN) {        // none  up, white down
      LDA_DOWN(whiteState, pos) = 0;
    }
  } else if(GET_LDA_DOWN(blackState, toInfo.m_pos)) {
    for(int pos = toInfo.m_colLine.m_lower[1]; DPOS_DOWN_LE(pos, from); pos += DPOS_DOWN) {        // none  up, black down
      LDA_DOWN(blackState, pos) = 0;
    }
  }
}

void Game::blackPawnWalkForward(const FieldInfo &toInfo, int from) {
  PlayerState &whiteState = m_playerState[WHITEPLAYER];
  PlayerState &blackState = m_playerState[BLACKPLAYER];

  if(GET_LDA_DOWN(blackState, from)) {
    LDA_DOWN(blackState, toInfo.m_pos) = 1;
    if(GET_LDA_UP(blackState, toInfo.m_pos)) {
      for(int pos = toInfo.m_colLine.m_upper[1];; pos += DPOS_UP  ) {                              // black up, black down
        AttackInfo &blackInfo = ATTINFO(blackState,pos);
        ATT_UP(  blackInfo) = 0;
        if(pos == from) {
          break;
        }
        ATT_DOWN(blackInfo) = 1;
      }
    } else if(GET_LDA_UP(whiteState, toInfo.m_pos)) {
      for(int pos = toInfo.m_colLine.m_upper[1];; pos += DPOS_UP  ) {                              // white up, black down
        LDA_UP(  whiteState, pos) = 0;
        if(pos == from) {
          break;
        }
        LDA_DOWN(blackState, pos) = 1;
      }
    } else {
      for(int pos = toInfo.m_colLine.m_upper[1]; pos != from; pos += DPOS_UP  ) {                  // none  up, black down
        LDA_DOWN(blackState,pos) = 1;
      }
    }
  } else if(GET_LDA_DOWN(whiteState, from)) {
    LDA_DOWN(whiteState, toInfo.m_pos) = 1;
    if(GET_LDA_UP(blackState, toInfo.m_pos)) {
      for(int pos = toInfo.m_colLine.m_upper[1];; pos += DPOS_UP  ) {                              // black up, white down
        LDA_UP(blackState, pos) = 0;
        if(pos == from) {
          break;
        }
        LDA_DOWN(  whiteState, pos) = 1;
      }
    } else if(GET_LDA_UP(whiteState, toInfo.m_pos)) {
      for(int pos = toInfo.m_colLine.m_upper[1];; pos += DPOS_UP  ) {                              // white up, white down
        AttackInfo &whiteInfo = ATTINFO(whiteState, pos);
        ATT_UP(whiteInfo) = 0;
        if(pos == from) {
          break;
        }
        ATT_DOWN(whiteInfo  ) = 1;
      }
    } else {
      for(int pos = toInfo.m_colLine.m_upper[1]; pos != from; pos += DPOS_UP  ) {                  // none  up, white down
        LDA_DOWN(whiteState, pos) = 1;
      }
    }
  } else if(GET_LDA_UP(blackState, toInfo.m_pos)) {
    for(int pos = toInfo.m_colLine.m_upper[1]; DPOS_UP_LE(pos, from); pos += DPOS_UP  ) {          // black up, none  down
      LDA_UP(blackState, pos) = 0;
    }
  } else if(GET_LDA_UP(whiteState, toInfo.m_pos)) {
    for(int pos = toInfo.m_colLine.m_upper[1]; DPOS_UP_LE(pos, from); pos += DPOS_UP  ) {          // white up, none  down
      LDA_UP(whiteState, pos) = 0;
    }
  }
}

#ifdef TABLEBASE_BUILDER
void Game::whitePawnWalkBackward(const FieldInfo &toInfo, int from) {
  blackPawnWalkForward(toInfo, from);
}

void Game::blackPawnWalkBackward(const FieldInfo &toInfo, int from) {
  whitePawnWalkForward(toInfo, from);
}
#endif

// ------------------------------------------------ En passant ---------------------------------------

EnPassantMove::EnPassantMove(const Move &m)
: m_move(m)
, m_capturedPosition(m.m_capturedPiece->m_position)
, m_fromInfo(Game::s_fieldInfo[m.m_from])
, m_toInfo(  Game::s_fieldInfo[m.m_to  ])
, m_capturedInfo(Game::s_fieldInfo[m.m_capturedPiece->m_position])
{
}

void Game::updateGameEP(const Move &m) { // En passant updates. Quite special move which involves 3 fields!
  EnPassantMove epMove(m);

  capturePiece(m);
  SET_EMPTYFIELD(m.m_from                 );
  SET_EMPTYFIELD(epMove.m_capturedPosition);

#ifdef _DEBUG
  const TCHAR *fromName      = getFieldName(m.m_from);
  const TCHAR *toName        = getFieldName(m.m_to  );
  const TCHAR *capturedField = getFieldName(epMove.m_capturedPosition);
#endif

  PlayerState     &whiteState        = m_playerState[WHITEPLAYER];
  PlayerState     &blackState        = m_playerState[BLACKPLAYER];

  AttackInfo      &whiteFromInfo     = ATTINFO(whiteState,m.m_from        );
  AttackInfo      &whiteToInfo       = ATTINFO(whiteState,m.m_to          );
  AttackInfo      &whiteCapturedInfo = ATTINFO(whiteState,epMove.m_capturedPosition);
  AttackInfo      &blackFromInfo     = ATTINFO(blackState,m.m_from        );
  AttackInfo      &blackToInfo       = ATTINFO(blackState,m.m_to          );
  AttackInfo      &blackCapturedInfo = ATTINFO(blackState,epMove.m_capturedPosition);

  switch(m.m_direction) {
  case MD_DOWNDIAG1: // black
    { updateKingDirEPDownDiag1(epMove);

      if(epMove.m_fromInfo.m_innerRow) {
        unblockColumn(epMove.m_fromInfo);
      }
      if(epMove.m_fromInfo.m_innerField) {
        unblockDiag2(epMove.m_fromInfo);
      }
      if(epMove.m_capturedInfo.m_innerField) {
        unblockDiag12(epMove.m_capturedInfo);
      }

      UPDATE_BLACKPAWNATTACKS(epMove.m_fromInfo, -1);
      UPDATE_BLACKPAWNATTACKS(epMove.m_toInfo  ,  1);

      unblockRowEP(epMove.m_capturedInfo, epMove.m_fromInfo);

      if(epMove.m_toInfo.m_innerField) {
        blockDiag2(epMove.m_toInfo);
      }

      ATT_DOWN(     whiteToInfo      ) = ATT_DOWN(whiteCapturedInfo);    // the white pawn is gone
      ATT_DOWN(     blackToInfo      ) = ATT_DOWN(blackCapturedInfo);

      ATT_UP(       whiteCapturedInfo) = 0;                              // now there is a black pawn
      ATT_UP(       blackCapturedInfo) = 0;

      ATT_UPDIAG1(  whiteFromInfo    ) = 0;                              // now there is a black pawn
      ATT_UPDIAG1(  blackFromInfo    ) = 0;

      if(ATT_DOWNDIAG1(       blackFromInfo)) {
        ATT_DOWNDIAG1(        blackToInfo   ) = 1;
      } else if(ATT_DOWNDIAG1(whiteFromInfo)) {
        ATT_DOWNDIAG1(        whiteToInfo   ) = 1;
      }

#ifndef TABLEBASE_BUILDER
      blackState.decrementPawnCount(epMove.m_fromInfo.m_col);
      blackState.incrementPawnCount(epMove.m_toInfo.m_col  );
      blackState.m_positionalScore += blackPawnPositionScore[m.m_to] - blackPawnPositionScore[m.m_from];
#endif
    }
    break;








  case MD_DOWNDIAG2: // black
    { updateKingDirEPDownDiag2(epMove);

      if(epMove.m_fromInfo.m_innerRow) {
        unblockColumn(epMove.m_fromInfo);
      }
      if(epMove.m_fromInfo.m_innerField) {
        unblockDiag1(epMove.m_fromInfo);
      }
      if(epMove.m_capturedInfo.m_innerField) {
        unblockDiag12(epMove.m_capturedInfo);
      }

      UPDATE_BLACKPAWNATTACKS(epMove.m_fromInfo, -1);
      UPDATE_BLACKPAWNATTACKS(epMove.m_toInfo  ,  1);

      unblockRowEP(epMove.m_fromInfo, epMove.m_capturedInfo);

      if(epMove.m_toInfo.m_innerField) {
        blockDiag1(epMove.m_toInfo);
      }

      ATT_DOWN(     whiteToInfo      ) = ATT_DOWN(whiteCapturedInfo);    // the white pawn is gone
      ATT_DOWN(     blackToInfo      ) = ATT_DOWN(blackCapturedInfo);

      ATT_UP(       whiteCapturedInfo) = 0;                              // now there is a black pawn
      ATT_UP(       blackCapturedInfo) = 0;

      ATT_UPDIAG2(  whiteFromInfo    ) = 0;                              // now there is a black pawn
      ATT_UPDIAG2(  blackFromInfo    ) = 0;

      if(ATT_DOWNDIAG2(       blackFromInfo)) {
        ATT_DOWNDIAG2(        blackToInfo   ) = 1;
      } else if(ATT_DOWNDIAG2(whiteFromInfo)) {
        ATT_DOWNDIAG2(        whiteToInfo   ) = 1;
      }

#ifndef TABLEBASE_BUILDER
      blackState.decrementPawnCount(epMove.m_fromInfo.m_col);
      blackState.incrementPawnCount(epMove.m_toInfo.m_col  );
      blackState.m_positionalScore += blackPawnPositionScore[m.m_to] - blackPawnPositionScore[m.m_from];
#endif
    }
    break;








  case MD_UPDIAG1  : // white
    { updateKingDirEPUpDiag1(epMove);

      if(epMove.m_fromInfo.m_innerRow) {
        unblockColumn(epMove.m_fromInfo);
      }
      if(epMove.m_fromInfo.m_innerField) {
        unblockDiag2(epMove.m_fromInfo);
      }
      if(epMove.m_capturedInfo.m_innerField) {
        unblockDiag12(epMove.m_capturedInfo);
      }

      UPDATE_WHITEPAWNATTACKS(epMove.m_fromInfo, -1);
      UPDATE_WHITEPAWNATTACKS(epMove.m_toInfo  ,  1);

      unblockRowEP(epMove.m_fromInfo, epMove.m_capturedInfo);

      if(epMove.m_toInfo.m_innerField) {
        blockDiag2(epMove.m_toInfo);
      }

      ATT_UP(       whiteToInfo      ) = ATT_UP(  whiteCapturedInfo);    // the black pawn is gone
      ATT_UP(       blackToInfo      ) = ATT_UP(  blackCapturedInfo);

      ATT_DOWN(     whiteCapturedInfo) = 0;                              // now there is a white pawn
      ATT_DOWN(     blackCapturedInfo) = 0;

      ATT_DOWNDIAG1(whiteFromInfo    ) = 0;                              // now there is a white pawn
      ATT_DOWNDIAG1(blackFromInfo    ) = 0;

      if(ATT_UPDIAG1(         whiteFromInfo)) {
        ATT_UPDIAG1(          whiteToInfo   ) = 1;
      } else if(ATT_UPDIAG1(  blackFromInfo)) {
        ATT_UPDIAG1(          blackToInfo   ) = 1;
      }

#ifndef TABLEBASE_BUILDER
      whiteState.decrementPawnCount(epMove.m_fromInfo.m_col);
      whiteState.incrementPawnCount(epMove.m_toInfo.m_col  );
      whiteState.m_positionalScore += whitePawnPositionScore[m.m_to] - whitePawnPositionScore[m.m_from];
#endif
    }
    break;








  case MD_UPDIAG2  : // white
    { updateKingDirEPUpDiag2(epMove);

      if(epMove.m_fromInfo.m_innerRow) {
        unblockColumn(epMove.m_fromInfo);
      }
      if(epMove.m_fromInfo.m_innerField) {
        unblockDiag1(epMove.m_fromInfo);
      }
      if(epMove.m_capturedInfo.m_innerField) {
        unblockDiag12(epMove.m_capturedInfo);
      }

      UPDATE_WHITEPAWNATTACKS(epMove.m_fromInfo, -1);
      UPDATE_WHITEPAWNATTACKS(epMove.m_toInfo  ,  1);

      unblockRowEP(epMove.m_capturedInfo, epMove.m_fromInfo);

      if(epMove.m_toInfo.m_innerField) {
        blockDiag1(epMove.m_toInfo);
      }

      ATT_UP(       whiteToInfo      ) = ATT_UP(  whiteCapturedInfo);    // the black pawn is gone
      ATT_UP(       blackToInfo      ) = ATT_UP(  blackCapturedInfo);

      ATT_DOWN(     whiteCapturedInfo) = 0;                              // now there is a white pawn
      ATT_DOWN(     blackCapturedInfo) = 0;

      ATT_DOWNDIAG2(whiteFromInfo    ) = 0;                              // now there is a white pawn
      ATT_DOWNDIAG2(blackFromInfo    ) = 0;

      if(ATT_UPDIAG2(         whiteFromInfo)) {
        ATT_UPDIAG2(          whiteToInfo   ) = 1;
      } else if(ATT_UPDIAG2(  blackFromInfo)) {
        ATT_UPDIAG2(          blackToInfo   ) = 1;
      }

#ifndef TABLEBASE_BUILDER
      whiteState.decrementPawnCount(epMove.m_fromInfo.m_col);
      whiteState.incrementPawnCount(epMove.m_toInfo.m_col  );
      whiteState.m_positionalScore += whitePawnPositionScore[m.m_to] - whitePawnPositionScore[m.m_from];
#endif
    }
    break;

  default:
    throwException(_T("%s:Invalid moveDirection for En passant:%d"), __TFUNCTION__, m.m_direction);
  }

  SET_PIECE(m.m_to, m.m_piece);
  if(epMove.m_toInfo.m_innerCol) {
    blockRow(epMove.m_toInfo);
  }

  m.m_piece->m_pinnedState = getPinnedState(m.m_to);
  m.m_piece->m_playerState.m_checkingSDAPosition = m.m_to; // Dont care if we really check the king.
}

// ------------------------------------------------ promote Pawn ---------------------------------------

void Game::updateGamePromotion(const Move &m) {
  Piece       *pawn  = m.m_piece;
  PlayerState &state = pawn->m_playerState;

#ifndef TABLEBASE_BUILDER
  state.m_totalMaterial -= pawn->m_materialValue;
#endif

  const PieceType promoteTo = s_legalPromotions[m.m_promoteIndex];
  switch(promoteTo) {
  case Queen :
  case Rook  :
    { if(m.m_capturedPiece == NULL) {
        pawnLeaveField(m);
        pawn->setType(promoteTo);
        SET_PIECE(m.m_to, pawn);
        pawnWalkAndPromote(m);
        setNonCapturingPromotedLDA(m, promoteTo);
      } else {
        capturePiece(m);
        pawnLeaveField(m);
        pawn->setType(promoteTo);
        SET_PIECE(m.m_to, pawn);
        pawnCaptureAndPromote(m);
        setCapturingPromotedLDA(m, promoteTo);
      }
    }
    break;

  case Bishop:
    { if(m.m_capturedPiece == NULL) {
        pawnLeaveField(m);
        pawn->setType(Bishop,m.m_to);
        SET_PIECE(m.m_to, pawn);
        pawnWalkAndPromote(m);
        setNonCapturingPromotedLDA(m, Bishop);
      } else {
        capturePiece(m);
        pawnLeaveField(m);
        pawn->setType(Bishop,m.m_to);
        SET_PIECE(m.m_to, pawn);
        pawnCaptureAndPromote(m);
        setCapturingPromotedLDA(m, Bishop);
      }
#ifndef TABLEBASE_BUILDER
      if(state.m_bishopFlags != BISHOPPAIR) {
        if((state.m_bishopFlags |= pawn->m_bishopFlag) == BISHOPPAIR) {
          state.m_positionalScore += BISHOPPAIRBONUS;
        }
      } else {
        state.m_bishopFlags |= pawn->m_bishopFlag;
      }
#endif
    }
    break;

  case Knight:
    { if(m.m_capturedPiece == NULL) {
        pawnLeaveField(m);
        pawn->setType(Knight);
        SET_PIECE(m.m_to, pawn);
        pawnWalkAndPromote(m);
        setNonCapturingPromotedKnight(m);
      } else {
        capturePiece(m);
        pawnLeaveField(m);
        pawn->setType(Knight);
        SET_PIECE(m.m_to, pawn);
        pawnCaptureAndPromote(m);
        setCapturingPromotedKnight(m);
      }
    }
    break;

  default:
    throwException(_T("Illegal promotionType:(=%d)"), promoteTo);
  }

#ifndef TABLEBASE_BUILDER
  state.m_totalMaterial += pawn->m_materialValue;
  state.decrementPawnCount(GETCOL(m.m_from));
#endif
  pawn->m_pinnedState = getPinnedState(m.m_to);
}

void Game::pawnWalkAndPromote(const Move &m) {
  PlayerState &whiteState = m_playerState[WHITEPLAYER];
  PlayerState &blackState = m_playerState[BLACKPLAYER];

  switch(m.m_direction) {
  case MD_DOWN     : // black
    { if(GET_LDA_DOWN(whiteState,m.m_from)) {
        LDA_DOWN(whiteState, m.m_to) = 1;
      } else if(GET_LDA_DOWN(blackState,m.m_from)) {
        LDA_DOWN(blackState, m.m_to) = 1;
      }
    }
    break;

  case MD_UP       : // white
    { if(GET_LDA_UP(whiteState,m.m_from)) {
        LDA_UP(whiteState, m.m_to) = 1;
      } else if(GET_LDA_UP(blackState,m.m_from)) {
        LDA_UP(blackState, m.m_to) = 1;
      }
    }
    break;
  }
}

void Game::pawnCaptureAndPromote(const Move &m) {
  PlayerState &whiteState = m_playerState[WHITEPLAYER];
  PlayerState &blackState = m_playerState[BLACKPLAYER];

  switch(m.m_direction) {
  case MD_DOWNDIAG1: // black
    if(GET_LDA_DOWNDIAG1(whiteState, m.m_from)) {
      LDA_DOWNDIAG1(whiteState, m.m_to) = 1;
    } else if(GET_LDA_DOWNDIAG1(blackState, m.m_from)) {
      LDA_DOWNDIAG1(blackState, m.m_to) = 1;
    }
    break;

  case MD_DOWNDIAG2: // black
    if(GET_LDA_DOWNDIAG2(whiteState, m.m_from)) {
      LDA_DOWNDIAG2(whiteState, m.m_to) = 1;
    } else if(GET_LDA_DOWNDIAG2(blackState, m.m_from)) {
      LDA_DOWNDIAG2(blackState, m.m_to) = 1;
    }
    break;

  case MD_UPDIAG1  : // white
    if(GET_LDA_UPDIAG1(  whiteState, m.m_from)) {
      LDA_UPDIAG1(  whiteState, m.m_to) = 1;
    } else if(GET_LDA_UPDIAG1(  blackState, m.m_from)) {
      LDA_UPDIAG1(  blackState, m.m_to) = 1;
    }
    break;

  case MD_UPDIAG2  : // white
    if(GET_LDA_UPDIAG2(  whiteState, m.m_from)) {
      LDA_UPDIAG2(  whiteState, m.m_to) = 1;
    } else if(GET_LDA_UPDIAG2(  blackState, m.m_from)) {
      LDA_UPDIAG2(  blackState, m.m_to) = 1;
    }
    break;

  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("m.direction=%d"), m.m_direction);
  }
}

void Game::setNonCapturingPromotedLDA(const Move &m, PieceType type) {
  const FieldInfo &toInfo = s_fieldInfo[m.m_to];
  switch(type) {
  case Queen :
    LDAenterRC(   toInfo);
    LDAenterDiag12(toInfo);
    break;

  case Rook  :
    // Never an inner field, => diagonals are not blockable
    LDAenterRC(   toInfo);
    break;

  case Bishop:
    if(toInfo.m_innerCol) {
      blockRow(toInfo);
    }
    // Column is not blockable, because toInfo is 1st or 8th row
    LDAenterDiag12(toInfo);
    break;

  default    :
    throwException(_T("%s:Invalid type (=%d). Must be {Queen,Rook,Bishop}"), __TFUNCTION__, type);
  }
}

void Game::setCapturingPromotedLDA(const Move &m, PieceType type) {
  const FieldInfo &toInfo = s_fieldInfo[m.m_to];
  switch(type) {
  case Queen :
    LDAenterRowCapture(toInfo);
    LDAenterColumn(    toInfo);
    LDAenterDiag12(    toInfo);
    break;

  case Rook  :
    LDAenterRowCapture(toInfo);
    LDAenterColumn(    toInfo);
    break;

  case Bishop:
    LDAenterDiag12(    toInfo);
    break;

  default    :
    throwException(_T("%s:Invalid type (=%d). Must be {Queen,Rook,Bishop}"), __TFUNCTION__, type);
  }
}

void Game::setNonCapturingPromotedKnight(const Move &m) {
  const FieldInfo &toInfo = s_fieldInfo[m.m_to];
  if(toInfo.m_innerCol) {
    blockRow(toInfo);
  }
  // Column and both diagonals are non-blockable
  UPDATE_KNIGHTATTACKS(PLAYERINTURN, toInfo, 1);
  m.m_piece->m_playerState.m_checkingSDAPosition = m.m_to; // Dont care if we really check the king.
}

void Game::setCapturingPromotedKnight(const Move &m) {
  UPDATE_KNIGHTATTACKS(PLAYERINTURN, s_fieldInfo[m.m_to], 1);
  m.m_piece->m_playerState.m_checkingSDAPosition = m.m_to; // Dont care if we really check the king.
}
