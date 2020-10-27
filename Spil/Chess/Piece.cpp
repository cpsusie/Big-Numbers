#include "stdafx.h"

Piece::Piece(PieceType pieceType, PlayerState &playerState, PlayerState &enemyState, int index, int row, int col)
: m_playerState(playerState)
, m_enemyState(enemyState)
, m_index(index)
, m_pieceKey(MAKE_PIECEKEY(playerState.m_player, NoPiece))
, m_pinnedState(playerState.m_pinnedState[index])
{
  m_position  = MAKE_POSITION(row, col);
  m_onBoard   = true;
  setType(pieceType);
}

const DirectionArray *Piece::getMoveTable(PieceKey key) { // static
  DEFINEMETHODNAME;
  switch(GET_TYPE_FROMKEY(key)) {
  case King   : return MoveTable::kingMoves;
  case Queen  : return MoveTable::queenMoves;
  case Rook   : return MoveTable::rookMoves;
  case Bishop : return MoveTable::bishopMoves;
  case Knight : return MoveTable::knightMoves;
  case Pawn   : return (GET_PLAYER_FROMKEY(key) == WHITEPLAYER) ? MoveTable::whitePawnMoves : MoveTable::blackPawnMoves;
  default     : throwInvalidArgumentException(method, _T("key=%u"), key);
                return nullptr;
  }
}

AttackAttribute Piece::getAttackAttribute(PieceKey key) { // static
  DEFINEMETHODNAME;
  switch(GET_TYPE_FROMKEY(key)) {
  case King   : return ATTACKS_PARALLEL | ATTACKS_DIAGONAL;
  case Queen  : return ATTACKS_PARALLEL | ATTACKS_DIAGONAL | ATTACKS_LONGDISTANCE;
  case Rook   : return ATTACKS_PARALLEL                    | ATTACKS_LONGDISTANCE;
  case Bishop : return                    ATTACKS_DIAGONAL | ATTACKS_LONGDISTANCE;
  case Knight : return 0;
  case Pawn   : return (GET_PLAYER_FROMKEY(key) == WHITEPLAYER) ? ATTACKS_UPPERDIAGONAL : ATTACKS_LOWERDIAGONAL;
  default     : throwInvalidArgumentException(method, _T("key=%u"), key);
                return 0;
  }
}

UINT Piece::getMaterialValue(PieceType pieceType) { // static
  DEFINEMETHODNAME;
  switch(pieceType) {
  case King   : return 0;
  case Queen  : return 900;
  case Rook   : return 500;
  case Bishop : return 300;
  case Knight : return 300;
  case Pawn   : return 100;
  default     : throwInvalidArgumentException(method, _T("pieceType=%d"), pieceType);
                return 0;
  }
}

void Piece::setType(PieceType pieceType) {
  if(pieceType == getType()) {
    return;
  }
  m_pieceKey           = MAKE_PIECEKEY(getPlayer(), pieceType);
  m_moveTable          = getMoveTable(              m_pieceKey);
  m_attackAttribute    = getAttackAttribute(        m_pieceKey);
  m_doMove             = Game::getMoveFunction(     pieceType );
#if !defined(TABLEBASE_BUILDER)
  m_materialValue      = getMaterialValue(          pieceType );
#else
  m_doBackMove         = Game::getBackMoveFunction( pieceType );
#endif
  m_updateWhenCaptured = Game::getCaptureFunction(  pieceType );
}

void Piece::setType(PieceType pieceType, int pos) {
  setType(pieceType);
#if !defined(TABLEBASE_BUILDER)
  initBishopMask(pos);
#endif
}

#if !defined(TABLEBASE_BUILDER)
void Piece::initBishopMask(int pos) {
  m_bishopFlag = (getFieldColor(pos) == WHITEFIELD) ? WHITEFIELD_BISHOP : BLACKFIELD_BISHOP;
}
#endif

bool Piece::isPromotedPawn() const {
  return m_playerState.isPromotedPawn(this);
}

String Piece::toString() const {
  return format(_T("%s%s"),getShortName().cstr(), getFieldName(m_position));
}

String Piece::toStringEnglish() const {
  return format(_T("%s%s"),getPieceTypeShortNameEnglish(getType()), getFieldName(m_position));
}

String Piece::getName() const {
  return ::getPieceTypeName(getType());
}

const TCHAR *Piece::getEnglishName() const {
  return ::getPieceTypeNameEnglish(getType());
}

String Piece::getShortName() const {
  return getPieceTypeShortName(getType());
}

int Piece::getPromoteField() const {
  switch(getPlayer()) {
  case WHITEPLAYER:
    return MAKE_POSITION(7, GETCOL(m_position));

  case BLACKPLAYER:
    return MAKE_POSITION(0, GETCOL(m_position));

  default:
    INVALIDPLAYERERROR(getPlayer());
    return 0;
  }
}
