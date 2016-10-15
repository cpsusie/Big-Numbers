#include "stdafx.h"

void Game::updateGameMoveKnight(const Move &m) {
  Piece       *piece = m.m_piece;
  PlayerState &state = piece->m_playerState;

  if(m.m_capturedPiece == NULL) {
    knightLeaveField(m);
    SET_PIECE(m.m_to, piece);
    setNonCapturingKnight(m);
  } else {
    capturePiece(m);
    knightLeaveField(m);
    SET_PIECE(m.m_to, piece);
    UPDATE_KNIGHTATTACKS(piece->getPlayer(), fieldInfo[m.m_to], 1);
  }

#ifndef TABLEBASE_BUILDER
  state.m_positionalScore += MoveTable::knightPositionScore[piece->getPlayer()][m.m_to]
                           - MoveTable::knightPositionScore[piece->getPlayer()][m.m_from];
#endif
  piece->m_pinnedState = getPinnedState(m.m_to);
  state.m_checkingSDAPosition = m.m_to; // Dont care if we really check the king.
}

#ifdef TABLEBASE_BUILDER
void Game::updateGameBackMoveKnight(const Move &m) {
  const Move m1 = m.makeBackMove();
  knightLeaveField(m1);
  SET_PIECE(m.m_from, m.m_piece);
  setNonCapturingKnight(m1);
  m.m_piece->m_pinnedState = getPinnedState(m.m_from);
}
#endif

void Game::updateGameCaptureKnight(const Move &m) {
  const Piece *knight = m.m_capturedPiece;
  UPDATE_KNIGHTATTACKS(knight->getPlayer(), fieldInfo[m.m_to], -1);
#ifndef TABLEBASE_BUILDER
  knight->m_playerState.m_positionalScore -= MoveTable::knightPositionScore[knight->getPlayer()][knight->m_position];
#endif
}

void Game::knightLeaveField(const Move &m) {
  const FieldInfo &fromInfo = fieldInfo[m.m_from];

  updateKingDir(m);

  // Knight always leave row, column and both diagonals when moving
  if(fromInfo.m_innerCol) {
    unblockRow(fromInfo);
  }

  if(fromInfo.m_innerRow) {
    unblockColumn(fromInfo);
  }

  if(fromInfo.m_innerField) {
    unblockDiag12(fromInfo);
  }

  UPDATE_KNIGHTATTACKS(PLAYERINTURN, fromInfo, -1);
  SET_EMPTYFIELD(m.m_from);
}

void Game::setNonCapturingKnight(const Move &m) {
  const FieldInfo &toInfo = fieldInfo[m.m_to];

  if(toInfo.m_innerCol) {
    blockRow(toInfo);
  }

  if(toInfo.m_innerRow) {
    blockColumn(toInfo);
  }

  if(toInfo.m_innerField) {
    blockDiag12(toInfo);
  }
  UPDATE_KNIGHTATTACKS(PLAYERINTURN, toInfo, 1);
}

static const int whiteKnightPosScore[64] = {
  -5,  0,  0,  0,  0,  0,  0, -5   // A1 - H1
 , 0,  2,  2,  2,  2,  2,  2,  0   // A2 - H2
 , 2,  2,  5,  5,  5,  5,  2,  2   // A3 - H3
 , 5,  9, 11, 12, 12, 11,  9,  5   // A4 - H4
 , 9, 11, 17, 19, 19, 17, 11,  9   // A5 - H5
 , 8, 11, 15, 15, 15, 15, 11,  8   // A6 - H6
 , 5,  7, 12, 12, 12, 12,  7,  5   // A7 - H7
 ,-1,  0,  2,  2,  2,  2,  0, -1   // A8 - H8
};

static const int blackKnightPosScore[64] = {
  -1,  0,  2,  2,  2,  2,  0, -1   // A1 - H1
 , 5,  7, 12, 12, 12, 12,  7,  5   // A2 - H2
 , 8, 11, 15, 15, 15, 15, 11,  8   // A3 - H3
 , 9, 11, 17, 19, 19, 17, 11,  9   // A4 - H4
 , 5,  9, 11, 12, 12, 11,  9,  5   // A5 - H5
 , 2,  2,  5,  5,  5,  5,  2,  2   // A6 - H6
 , 0,  2,  2,  2,  2,  2,  2,  0   // A7 - H7
 ,-5,  0,  0,  0,  0,  0,  0, -5   // A8 - H8
};

const int *MoveTable::knightPositionScore[2] = {
  whiteKnightPosScore
 ,blackKnightPosScore
};
