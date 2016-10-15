#include "stdafx.h"

// Checks if the king is uncovered by En passant.
bool Game::uncoversKingEP(const Piece *pawn, const int to) const {
  switch(pawn->m_pinnedState) {
  case NOT_PINNED:
    { const int        kingPos  = pawn->m_playerState.m_king->m_position;
      const FieldInfo &fromInfo = fieldInfo[pawn->m_position];

      // No need to check, if the captured pawn will uncover any diagonal attacking pieces to the king by being captured.
      // This can not happen, because the last move was the pawn moving 2 steps forward, and if the king was not diagonal-attacked before
      // the move (which is always the case!!), then it cannot be after capturing the pawn either.
      // NB. The captured and the capturing pawn does not share any diagonals.
      // The capturing pawn is leaving the row, and at the same time removes the enemy pawn from the neighborfield (same row)

      if(fromInfo.m_innerCol && IS_INNERCOL(to) && GETROW(kingPos) == fromInfo.m_row) {
        if(GETCOL(to) > fromInfo.m_col) {
          if(GETCOL(kingPos) < fromInfo.m_col) {      // Pawn is capturing right, king is at the same row somewhere to the left
            return KING_IS_LEFT( pawn->m_playerState, pawn->m_position       ) && GET_LDA_LEFT( pawn->m_enemyState, m_gameKey.getEPSquare());
          } else {                                    // Pawn is capturing right, king is at the same row somewhere to the right
            return KING_IS_RIGHT(pawn->m_playerState, m_gameKey.getEPSquare()) && GET_LDA_RIGHT(pawn->m_enemyState, pawn->m_position       );
          }
        } else if(GETCOL(kingPos) > fromInfo.m_col) { // Pawn is capturing left,   king is at the same row somewhere to the right
          return KING_IS_RIGHT(  pawn->m_playerState, pawn->m_position       ) && GET_LDA_RIGHT(pawn->m_enemyState, m_gameKey.getEPSquare());
        } else {                                      // Pawn is capturing left,   king is at the same row somewhere to the left
          return KING_IS_LEFT(   pawn->m_playerState, m_gameKey.getEPSquare()) && GET_LDA_LEFT( pawn->m_enemyState, pawn->m_position       );
        }
      }
    }
    return false;

  case PINNED_TO_ROW      :
    throwException(_T("Pawn has PinnedState=PINNED_TO_ROW, when doing en passant, which is impossible!!"));
    return true;

  case PINNED_TO_COL   :
    return true;

  case PINNED_TO_DIAG1:
    return GETDIAG1(to) != GETDIAG1(pawn->m_position);

  case PINNED_TO_DIAG2:
    return GETDIAG2(to) != GETDIAG2(pawn->m_position);

  default:
    throwException(_T("Game::uncoversKingEP:Invalid pinnedState:%d"), pawn->m_pinnedState);
    return true;
  }
}

bool Game::isShortCastleAllowed() const {
  if(PLAYERINTURN == WHITEPLAYER) {
    if(!(m_gameKey.d.m_castleState[WHITEPLAYER] & SHORTCASTLE_ALLOWED)) {
      return false;
    }
    if((m_gameKey.m_pieceKey[H1] != WHITEROOK) || m_board[F1] || m_board[G1]) {
      return false;
    }
    const FieldAttacks *attackTable = m_playerState[BLACKPLAYER].m_attackTable;
    if(attackTable[F1].m_isAttacked || attackTable[G1].m_isAttacked) { // No need to check E1, because we are not called, if king is in check
      return false;
    }
  } else {
    if(!(m_gameKey.d.m_castleState[BLACKPLAYER] & SHORTCASTLE_ALLOWED)) {
      return false;
    }
    if((m_gameKey.m_pieceKey[H8] != BLACKROOK) || m_board[F8] || m_board[G8]) {
      return false;
    }
    const FieldAttacks *attackTable = m_playerState[WHITEPLAYER].m_attackTable;
    if(attackTable[F8].m_isAttacked || attackTable[G8].m_isAttacked) { // No need to check E8, because we are not called, if king is in check
      return false;
    }
  }
  return true;
}

bool Game::isLongCastleAllowed() const {
  if(PLAYERINTURN == WHITEPLAYER) {
    if(!(m_gameKey.d.m_castleState[WHITEPLAYER] & LONGCASTLE_ALLOWED)) {
      return false;
    }
    if((m_gameKey.m_pieceKey[A1] != WHITEROOK) || m_board[B1] || m_board[C1] || m_board[D1]) {
      return false;
    }
    const FieldAttacks *attackTable = m_playerState[BLACKPLAYER].m_attackTable;
    if(attackTable[D1].m_isAttacked || attackTable[C1].m_isAttacked) { // No need to check E1, because we are not called, if king is in check
      return false;
    }
  } else {
    if(!(m_gameKey.d.m_castleState[BLACKPLAYER] & LONGCASTLE_ALLOWED)) {
      return false;
    }
    if((m_gameKey.m_pieceKey[A8] != BLACKROOK) || m_board[B8] || m_board[C8] || m_board[D8]) {
      return false;
    }
    const FieldAttacks *attackTable = m_playerState[WHITEPLAYER].m_attackTable;
    if(attackTable[D8].m_isAttacked || attackTable[C8].m_isAttacked) { // No need to check E8, because we are not called, if king is in check
      return false;
    }
  }
  return true;
}

#define _SETMOVE(move, type, piece, capturedPiece, dirIndex, moveIndex, to, direction)            \
{ move.m_type          = type;                                                                    \
  move.m_piece         = piece;                                                                   \
  move.m_capturedPiece = capturedPiece;                                                           \
  move.m_dirIndex      = dirIndex;                                                                \
  move.m_moveIndex     = moveIndex;                                                               \
  move.m_from          = piece->m_position;                                                       \
  move.m_to            = to;                                                                      \
  move.m_direction     = direction;                                                               \
}

#define SETMOVE_NORMAL(move, piece, capturedPiece, dirIndex, moveIndex, to, direction)            \
  _SETMOVE(move, NORMALMOVE, piece, capturedPiece, dirIndex, moveIndex, to, direction)

#define SETMOVE_SHORTCASTLE(move, piece)                                                          \
  _SETMOVE(move, SHORTCASTLING, piece, NULL, 0, 0, (piece->getPlayer()==WHITEPLAYER)?G1:G8,MD_RIGHT)

#define SETMOVE_LONGCASTLE(move, piece)                                                           \
  _SETMOVE(move, LONGCASTLING, piece, NULL, 0, 0, (piece->getPlayer()==WHITEPLAYER)?C1:C8,MD_LEFT)


#define SETMOVE_CHECKFORPROMOTION(move, piece, capturedPiece, dirIndex, moveIndex, to, direction) \
{ if(piece->m_moveTable[to].m_count == 0) {                                                       \
    _SETMOVE(move, PROMOTION, piece, capturedPiece, dirIndex, moveIndex, to, direction);          \
    move.m_promoteIndex = 0;                                                                      \
  } else {                                                                                        \
    SETMOVE_NORMAL(move, piece, capturedPiece, dirIndex, moveIndex, to, direction);               \
  }                                                                                               \
}

// Direction is diagonal
#define RETURN_IF_LEGAL_ENPASSANT(move, piece, dirIndex, moveIndex, to, direction)                \
{ if(((to^8) == m_game.m_gameKey.getEPSquare())                                                   \
  && !m_game.uncoversKingEP(piece, to)) {                                                         \
    _SETMOVE(move, ENPASSANT, piece, m_game.m_board[m_game.m_gameKey.getEPSquare()]               \
            ,dirIndex, moveIndex, to, direction);                                                 \
    return true;                                                                                  \
  }                                                                                               \
}

// Direction is diagonal
#define _CHECKFOR_EP(move, piece, dirIndex, moveIndex, to, direction)                             \
{ if(m_game.m_gameKey.getEPSquare() < 0) {                                                        \
    break; /* Only positive if last move was a 2-step pawn-move. */                               \
  }                                                                                               \
  RETURN_IF_LEGAL_ENPASSANT(move, piece, dirIndex, moveIndex, to, direction)                      \
}

#ifdef TABLEBASE_BUILDER

// Special pawn captures with no captured piece are generated if m_game.m_generateFictivePawnCaptures = true
// and the pawn is NOT promoted at the same time. Used in generateAllPositions, to generate positions within the same
// endgame, as if the pawn has just captured an enemypiece to get into this endgame.
// Direction is diagonal
#define CHECKFOR_EP(move, piece, dirIndex, moveIndex, to, direction)                                                      \
{ if(m_game.m_generateFictivePawnCaptures) { /* allow diagonal pawn move even if there is no piece to capture */          \
    if(m_game.m_gameKey.getEPSquare() > 0) {                                                                              \
      RETURN_IF_LEGAL_ENPASSANT(move, piece, dirIndex, moveIndex, to, direction);                                         \
    }                                                                                                                     \
    if(piece->m_moveTable[to].m_count > 0) { /* do not generate promotions here */                                        \
      SETMOVE_NORMAL(move, piece, m_game.m_board[to], dirIndex, moveIndex, to, direction);                                \
      return true;                                                                                                        \
    }                                                                                                                     \
  } else { /* just do it the normal way */                                                                                \
    _CHECKFOR_EP(move, piece, dirIndex, moveIndex, to, direction);                                                        \
  }                                                                                                                       \
}

#else

#define CHECKFOR_EP(move, piece, dirIndex, moveIndex, to, direction) _CHECKFOR_EP(move, piece, dirIndex, moveIndex, to, direction)

#endif

// Direction is diagonal
#define CHECKFOR_EP_AND_CHECKINGPIECE(move, piece, dirIndex, moveIndex, to, direction)                                    \
{ if(m_game.m_gameKey.getEPSquare() < 0) {                                                                                \
    break; /* Only positive if last move was a 2-step pawn-move. */                                                       \
  }                                                                                                                       \
  const int epSquare = to^8;                                                                                              \
  Piece *cp;                                                                                                              \
  if((epSquare == m_game.m_gameKey.getEPSquare()) && isCheckingPiece(cp = m_game.m_board[epSquare])                       \
  && !m_game.uncoversKingEP(piece, to)) {                                                                                 \
    _SETMOVE(move, ENPASSANT, piece, cp, dirIndex, moveIndex, to, direction);                                             \
    return true;                                                                                                          \
  }                                                                                                                       \
}

class MoveGeneratorNoCheck : public MoveGenerator {
public:
  MoveGeneratorNoCheck(const Game &game) : MoveGenerator(game) {
  }
  bool firstMove(Move &m) const;
  bool nextMove( Move &m) const;
};

class MoveGeneratorCheck : public MoveGenerator {
protected:
  bool isValidKingMove(const Move &m, KingAttackState kingAttacks) const;
public:
  MoveGeneratorCheck(const Game &game) : MoveGenerator(game) {
  }
};

class MoveGeneratorLDCheck : public MoveGeneratorCheck {
private:
  bool coversKing(int to) const;
  bool isCheckingLDAPiece(const Piece *piece) const;
public:
  MoveGeneratorLDCheck(const Game &game) : MoveGeneratorCheck(game) {
  }
  bool firstMove(Move &m) const;
  bool nextMove( Move &m) const;
};

class MoveGeneratorSDCheck : public MoveGeneratorCheck {
private:
  bool isCheckingPiece(const Piece *piece) const;
public:
  MoveGeneratorSDCheck(const Game &game) : MoveGeneratorCheck(game) {
  }
  bool firstMove(Move &m) const;
  bool nextMove( Move &m) const;
};

class MoveGeneratorDoubleCheck : public MoveGeneratorCheck {
public:
  MoveGeneratorDoubleCheck(const Game &game) : MoveGeneratorCheck(game) {
  }
  bool firstMove(Move &m) const;
  bool nextMove( Move &m) const;
};


#define MOVEGEN_NO_CHECK         0
#define MOVEGEN_LD_CHECK         1
#define MOVEGEN_SD_CHECK         2
#define MOVEGEN_DOUBLE_CHECK     3

void Game::allocateMoveGenerators() {
  m_moveGenerator[MOVEGEN_NO_CHECK    ] = new MoveGeneratorNoCheck(    *this);
  m_moveGenerator[MOVEGEN_LD_CHECK    ] = new MoveGeneratorLDCheck(    *this);
  m_moveGenerator[MOVEGEN_SD_CHECK    ] = new MoveGeneratorSDCheck(    *this);
  m_moveGenerator[MOVEGEN_DOUBLE_CHECK] = new MoveGeneratorDoubleCheck(*this);

#ifdef TABLEBASE_BUILDER
  allocateBackMoveGenerators();
#endif
}

void Game::deallocateMoveGenerators() {
  for(int i = 0; i < ARRAYSIZE(m_moveGenerator); i++) {
    delete m_moveGenerator[i];
  }

#ifdef TABLEBASE_BUILDER
  deallocateBackMoveGenerators();
#endif
}

MoveGenerator &Game::getMoveGenerator() const {
  switch(m_playerState[PLAYERINTURN].m_kingAttackState) {
  case KING_NOT_ATTACKED:
    return *m_moveGenerator[MOVEGEN_NO_CHECK];

  case KING_LD_ATTACKED_FROM_ROW  :
  case KING_LD_ATTACKED_FROM_COL  :
  case KING_LD_ATTACKED_FROM_DIAG1:
  case KING_LD_ATTACKED_FROM_DIAG2:
    return *m_moveGenerator[MOVEGEN_LD_CHECK];

  case KING_SD_ATTACKED           :
    return *m_moveGenerator[MOVEGEN_SD_CHECK];

  default:
    return *m_moveGenerator[MOVEGEN_DOUBLE_CHECK];
  }
}

#undef PLAYERINTURN
#define PLAYERINTURN m_game.m_gameKey.getPlayerInTurn()

bool MoveGeneratorCheck::isValidKingMove(const Move &m, KingAttackState kingAttacks) const {
  switch(m.m_direction) {
  case MD_LEFT     :
    if(kingAttacks & KING_LD_ATTACKED_FROM_ROW) {
      return GET_LDA_LEFT(     m_game.m_playerState[CURRENTENEMY], m.m_from) == 0;
    }
    break;
  case MD_RIGHT    :
    if(kingAttacks & KING_LD_ATTACKED_FROM_ROW) {
      return GET_LDA_RIGHT(    m_game.m_playerState[CURRENTENEMY], m.m_from) == 0;
    }
    break;
  case MD_DOWN     :
    if(kingAttacks & KING_LD_ATTACKED_FROM_COL) {
      return GET_LDA_DOWN(     m_game.m_playerState[CURRENTENEMY], m.m_from) == 0;
    }
    break;
  case MD_UP       :
    if(kingAttacks & KING_LD_ATTACKED_FROM_COL) {
      return GET_LDA_UP(       m_game.m_playerState[CURRENTENEMY], m.m_from) == 0;
    }
    break;
  case MD_DOWNDIAG1:
    if(kingAttacks & KING_LD_ATTACKED_FROM_DIAG1) {
      return GET_LDA_DOWNDIAG1(m_game.m_playerState[CURRENTENEMY], m.m_from) == 0;
    }
    break;
  case MD_UPDIAG1  :
    if(kingAttacks & KING_LD_ATTACKED_FROM_DIAG1) {
      return GET_LDA_UPDIAG1(  m_game.m_playerState[CURRENTENEMY], m.m_from) == 0;
    }
    break;
  case MD_DOWNDIAG2:
    if(kingAttacks & KING_LD_ATTACKED_FROM_DIAG2) {
      return GET_LDA_DOWNDIAG2(m_game.m_playerState[CURRENTENEMY], m.m_from) == 0;
    }
    break;
  case MD_UPDIAG2  :
    if(kingAttacks & KING_LD_ATTACKED_FROM_DIAG2) {
      return GET_LDA_UPDIAG2(  m_game.m_playerState[CURRENTENEMY], m.m_from) == 0;
    }
    break;
  default          :
    throwInvalidArgumentException(__TFUNCTION__, _T("m.direction=%d"), m.m_direction);
    return false;
  }
  return true;
}

// --------------------------------------------- MoveGeneratorNoCheck ------------------------------------------------
// Used when king is not in check. All possible valid moves are generated, including castling, en passant and promotion.
bool MoveGeneratorNoCheck::firstMove(Move &m) const {
  Piece *cp;
  for(Piece *piece = m_game.m_playerState[PLAYERINTURN].m_first; piece; piece = piece->m_next) {
    if(!piece->m_onBoard) {
      continue;
    }
    switch(piece->getType()) {
    case King:
      { if(m_game.isShortCastleAllowed()) {
          SETMOVE_SHORTCASTLE(m, piece);
          return true;
        }
        if(m_game.isLongCastleAllowed()) {
          SETMOVE_LONGCASTLE(m, piece);
          return true;
        }

        const DirectionArray &da          = MoveTable::kingMoves[piece->m_position];
        const Direction      *direction   = da.m_directions;
        const FieldAttacks   *attackTable = piece->m_enemyState.m_attackTable;

        for(int dirIndex = 0; dirIndex < da.m_count; dirIndex++, direction++) {
          PositionArray fields = direction->m_fields;
          const int     count  = *(fields++);
          for(int moveIndex = 0; moveIndex < count; moveIndex++) {
            const int to = *(fields++);
            if((cp = m_game.m_board[to]) && cp->getPlayer() == PLAYERINTURN || attackTable[to].m_isAttacked) {
              break; // Cannot capture my own piece or go to an attacked field. Try another direction
            }
            SETMOVE_NORMAL(m, piece, cp, dirIndex, moveIndex, to, direction->m_direction);
            return true;
          }
        }
      }
      continue;

    case Knight:
      { if(piece->m_pinnedState != NOT_PINNED) {
          // If Knight is pinned, it cannot be moved at all, because it always leaves all lines
          continue;
        }
        const DirectionArray &da        = MoveTable::knightMoves[piece->m_position];
        const Direction      *direction = da.m_directions;

        for(int dirIndex = 0; dirIndex < da.m_count; dirIndex++, direction++) {
          PositionArray fields = direction->m_fields;
          const int     count  = *(fields++);
          for(int moveIndex = 0; moveIndex < count; moveIndex++) {
            const int to = *(fields++);
            if((cp = m_game.m_board[to]) && cp->getPlayer() == PLAYERINTURN) {
              break; // Cannot capture my own piece. Try another direction
            }
            SETMOVE_NORMAL(m, piece, cp, dirIndex, moveIndex, to, MD_NONE);
            return true;
          }
        }
      }
      continue;

    case Pawn:
      { const DirectionArray &da        = piece->m_moveTable[piece->m_position];
        const Direction      *direction = da.m_directions;

        for(int dirIndex = 0; dirIndex < da.m_count; dirIndex++, direction++) {
          if(MoveTable::uncoversKing[piece->m_pinnedState][direction->m_direction]) {
            continue;
          }
          PositionArray fields = direction->m_fields;
          const int     count  = *(fields++);
          for(int moveIndex = 0; moveIndex < count; moveIndex++) {
            const int to = *(fields++);
            if(cp = m_game.m_board[to]) {
              if(cp->getPlayer() == PLAYERINTURN || !IS_DIAGONALMOVE(direction->m_direction)) {
                break;
              }
            } else if(IS_DIAGONALMOVE(direction->m_direction)) {
              CHECKFOR_EP(m, piece, dirIndex, moveIndex, to, direction->m_direction);
              break;
            }
            SETMOVE_CHECKFORPROMOTION(m, piece, cp, dirIndex, moveIndex, to, direction->m_direction);
            return true;
          }
        }
      }
      continue;

    default:
      { const DirectionArray &da        = piece->m_moveTable[piece->m_position];
        const Direction      *direction = da.m_directions;

        for(int dirIndex = 0; dirIndex < da.m_count; dirIndex++, direction++) {
          if(MoveTable::uncoversKing[piece->m_pinnedState][direction->m_direction]) {
            continue;
          }
          PositionArray fields = direction->m_fields;
          const int     count  = *(fields++);
          for(int moveIndex = 0; moveIndex < count; moveIndex++) {
            const int to = *(fields++);
            if((cp = m_game.m_board[to]) && cp->getPlayer() == PLAYERINTURN) {
              break; // Cannot capture my own piece. Try another direction
            }
            SETMOVE_NORMAL(m, piece, cp, dirIndex, moveIndex, to, direction->m_direction);
            return true;
          }
        }
      }
      continue;
    }
  }
  return false;
}

bool MoveGeneratorNoCheck::nextMove(Move &m) const { // Assumes m is a valid move
  if(m.m_type == PROMOTION && (m.m_promoteIndex < ARRAYSIZE(Game::legalPromotions)-1)) {
    m.m_promoteIndex++;
    return true;
  }

  int startDir, startIndex;
  if(m.m_capturedPiece) { // Last move was a capture. Change to a new direction
    startDir   = m.m_dirIndex + 1;
    startIndex = 0;
  } else {
    startDir   = m.m_dirIndex;
    startIndex = m.m_moveIndex + 1;
  }

  Piece *cp;
  for(Piece *piece = m.m_piece; piece; piece = piece->m_next, startDir = startIndex = 0) {
    if(!piece->m_onBoard) {
      continue;
    }
    switch(piece->getType()) {
    case King:
      { switch(m.m_type) {
        case SHORTCASTLING:
          if(m_game.isLongCastleAllowed()) {
            SETMOVE_LONGCASTLE(m, piece);
            return true;
          }
          startDir = startIndex = 0;
          break;
        case LONGCASTLING:
          startDir = startIndex = 0;
          break; // go on to the normal moves
        default:
          if(piece != m.m_piece) {
            if(m_game.isShortCastleAllowed()) {
              SETMOVE_SHORTCASTLE(m, piece);
              return true;
            }
            if(m_game.isLongCastleAllowed()) {
              SETMOVE_LONGCASTLE(m, piece);
              return true;
            }
          }
          break;
        }
        const DirectionArray &da          = MoveTable::kingMoves[piece->m_position];
        const Direction      *direction   = da.m_directions + startDir;
        const FieldAttacks   *attackTable = piece->m_enemyState.m_attackTable;

        for(int dirIndex = startDir; dirIndex < da.m_count; dirIndex++, startIndex = 0, direction++) {
          PositionArray fields = direction->m_fields;
          const int     count  = *(fields++);
          for(int moveIndex = startIndex; moveIndex < count; moveIndex++) {
            const int to = fields[moveIndex]; // Do not increment fields !
            if((cp = m_game.m_board[to]) && cp->getPlayer() == PLAYERINTURN || attackTable[to].m_isAttacked) {
              break; // Cannot capture my own piece or go to an attacked field. Try another direction
            }
            SETMOVE_NORMAL(m, piece, cp, dirIndex, moveIndex, to, direction->m_direction);
            return true;
          }
        }
      }
      continue;

    case Knight:
      { if(piece->m_pinnedState != NOT_PINNED) {
          // If Knight is pinned, it cannot be moved at all, because it always leaves all lines
          continue;
        }
        const DirectionArray &da        = MoveTable::knightMoves[piece->m_position];
        const Direction      *direction = da.m_directions + startDir;

        for(int dirIndex = startDir; dirIndex < da.m_count; dirIndex++, startIndex = 0, direction++) {
          PositionArray fields = direction->m_fields;
          const int     count = *(fields++);
          for(int moveIndex = startIndex; moveIndex < count; moveIndex++) {
            const int to = fields[moveIndex]; // Do not increment fields !
            if((cp = m_game.m_board[to]) && cp->getPlayer() == PLAYERINTURN) {
              break; // Cannot capture my own piece. Try another direction
            }
            SETMOVE_NORMAL(m, piece, cp, dirIndex, moveIndex, to, MD_NONE);
            return true;
          }
        }
      }
      continue;

    case Pawn:
      { const DirectionArray &da        = piece->m_moveTable[piece->m_position];
        const Direction      *direction = da.m_directions + startDir;

        for(int dirIndex = startDir; dirIndex < da.m_count; dirIndex++, startIndex = 0, direction++) {
          if(MoveTable::uncoversKing[piece->m_pinnedState][direction->m_direction]) {
            continue;
          }
          PositionArray fields = direction->m_fields;
          const int     count  = *(fields++);
          for(int moveIndex = startIndex; moveIndex < count; moveIndex++) {
            const int to = fields[moveIndex]; // Do not increment fields !
            if(cp = m_game.m_board[to]) {
              if(cp->getPlayer() == PLAYERINTURN || !IS_DIAGONALMOVE(direction->m_direction)) {
                break;
              }
            } else if(IS_DIAGONALMOVE(direction->m_direction)) {
              CHECKFOR_EP(m, piece, dirIndex, moveIndex, to, direction->m_direction);
              break;
            }
            SETMOVE_CHECKFORPROMOTION(m, piece, cp, dirIndex, moveIndex, to, direction->m_direction);
            return true;
          }
        }
      }
      continue;

    default:
      { const DirectionArray &da        = piece->m_moveTable[piece->m_position];
        const Direction      *direction = da.m_directions + startDir;

        for(int dirIndex = startDir; dirIndex < da.m_count; dirIndex++, startIndex = 0, direction++) {
          if(MoveTable::uncoversKing[piece->m_pinnedState][direction->m_direction]) {
            continue;
          }
          PositionArray fields = direction->m_fields;
          const int     count  = *(fields++);
          for(int moveIndex = startIndex; moveIndex < count; moveIndex++) {
            const int to = fields[moveIndex]; // do not increment fields !
            if((cp = m_game.m_board[to]) && cp->getPlayer() == PLAYERINTURN) {
              break; // Cannot capture my own piece. Try another direction
            }
            SETMOVE_NORMAL(m, piece, cp, dirIndex, moveIndex, to, direction->m_direction);
            return true;
          }
        }
      }
      continue;
    }
  }
  return false;
}

// --------------------------------------------- MoveGeneratorLDCheck ------------------------------------------------
// Used when king is checked by Queen, Rook or Bishop. (L)ong (D)istance check. Not doublecheck
// No castling- or en passant move is generated by this generator.
bool MoveGeneratorLDCheck::coversKing(int to) const {
  switch(KING_DIRECTION(m_game.m_playerState[PLAYERINTURN],to)) {
  case MD_NONE     : return false;
  case MD_LEFT     : return GET_LDA_LEFT(      m_game.m_playerState[CURRENTENEMY], to) > 0;
  case MD_RIGHT    : return GET_LDA_RIGHT(     m_game.m_playerState[CURRENTENEMY], to) > 0;
  case MD_DOWN     : return GET_LDA_DOWN(      m_game.m_playerState[CURRENTENEMY], to) > 0;
  case MD_UP       : return GET_LDA_UP(        m_game.m_playerState[CURRENTENEMY], to) > 0;
  case MD_DOWNDIAG1: return GET_LDA_DOWNDIAG1( m_game.m_playerState[CURRENTENEMY], to) > 0;
  case MD_UPDIAG1  : return GET_LDA_UPDIAG1(   m_game.m_playerState[CURRENTENEMY], to) > 0;
  case MD_DOWNDIAG2: return GET_LDA_DOWNDIAG2( m_game.m_playerState[CURRENTENEMY], to) > 0;
  case MD_UPDIAG2  : return GET_LDA_UPDIAG2(   m_game.m_playerState[CURRENTENEMY], to) > 0;
  }
  return false;
}

bool MoveGeneratorLDCheck::isCheckingLDAPiece(const Piece *piece) const {
  switch(KING_DIRECTION(m_game.m_playerState[PLAYERINTURN],piece->m_position)) {
  case MD_NONE     : return false;
  case MD_LEFT     :
  case MD_RIGHT    :
  case MD_DOWN     :
  case MD_UP       : return (piece->m_attackAttribute & ATTACKS_PARALLEL) != 0;
  case MD_DOWNDIAG1:
  case MD_UPDIAG1  :
  case MD_DOWNDIAG2:
  case MD_UPDIAG2  : return LD_ATTACKS(piece, ATTACKS_DIAGONAL);
  }
  return false;
}

bool MoveGeneratorLDCheck::firstMove(Move &m) const {
  Piece *cp;
  for(Piece *piece = m_game.m_playerState[PLAYERINTURN].m_first; piece; piece = piece->m_next) {
    if(!piece->m_onBoard) {
      continue;
    }
    switch(piece->getType()) {
    case King:
      { const DirectionArray &da          = MoveTable::kingMoves[piece->m_position];
        const Direction      *direction   = da.m_directions;
        const KingAttackState kingAttacks = m_game.m_playerState[PLAYERINTURN].m_kingAttackState;
        const FieldAttacks   *attackTable = piece->m_enemyState.m_attackTable;

        for(int dirIndex = 0; dirIndex < da.m_count; dirIndex++, direction++) {
          PositionArray fields = direction->m_fields;
          const int     count  = *(fields++);
          for(int moveIndex = 0; moveIndex < count; moveIndex++) {
            const int to = *(fields++);
            if((cp = m_game.m_board[to]) && cp->getPlayer() == PLAYERINTURN || attackTable[to].m_isAttacked) {
              break; // Cannot capture my own piece, or go to an attacked field. Try another direction
            }
            SETMOVE_NORMAL(m, piece, cp, dirIndex, moveIndex, to, direction->m_direction);
            if(isValidKingMove(m, kingAttacks)) {
              return true;
            }
          }
        }
      }
      continue;

    case Knight:
      { if(piece->m_pinnedState != NOT_PINNED) {
          // If Knight is pinned, it cannot be moved at all, because it always leaves all lines
          continue;
        }
        const DirectionArray &da        = MoveTable::knightMoves[piece->m_position];
        const Direction      *direction = da.m_directions;

        for(int dirIndex = 0; dirIndex < da.m_count; dirIndex++, direction++) {
          PositionArray fields = direction->m_fields;
          const int     count  = *(fields++);
          for(int moveIndex = 0; moveIndex < count; moveIndex++) {
            const int to = *(fields++);
            if(cp = m_game.m_board[to]) {
              if(cp->getPlayer() == PLAYERINTURN || !isCheckingLDAPiece(cp)) {
                break;
              }
            } else if(!coversKing(to)) {
              break;
            }
            SETMOVE_NORMAL(m, piece, cp, dirIndex, moveIndex, to, MD_NONE);
            return true;
          }
        }
      }
      continue;

    case Pawn:
      { const DirectionArray &da        = piece->m_moveTable[piece->m_position];
        const Direction      *direction = da.m_directions;

        for(int dirIndex = 0; dirIndex < da.m_count; dirIndex++, direction++) {
          if(MoveTable::uncoversKing[piece->m_pinnedState][direction->m_direction]) {
            continue;
          }
          PositionArray fields = direction->m_fields;
          const int     count  = *(fields++);
          for(int moveIndex = 0; moveIndex < count; moveIndex++) {
            const int to = *(fields++);
            if(cp = m_game.m_board[to]) {
              if(cp->getPlayer() == PLAYERINTURN || !IS_DIAGONALMOVE(direction->m_direction) || !isCheckingLDAPiece(cp)) {
                break;
              }
            } else if(IS_DIAGONALMOVE(direction->m_direction)) { // Cannot cover the king from LDA-check by capturing EP
              break;
            } else if(!coversKing(to)) {
              continue;
            }
            SETMOVE_CHECKFORPROMOTION(m, piece, cp, dirIndex, moveIndex, to, direction->m_direction);
            return true;
          }
        }
      }
      continue;

    default:
      { const DirectionArray &da        = piece->m_moveTable[piece->m_position];
        const Direction      *direction = da.m_directions;

        for(int dirIndex = 0; dirIndex < da.m_count; dirIndex++, direction++) {
          if(MoveTable::uncoversKing[piece->m_pinnedState][direction->m_direction]) {
            continue;
          }
          PositionArray fields = direction->m_fields;
          const int     count  = *(fields++);
          for(int moveIndex = 0; moveIndex < count; moveIndex++) {
            const int to = *(fields++);
            if(cp = m_game.m_board[to]) {
              if(cp->getPlayer() == PLAYERINTURN || !isCheckingLDAPiece(cp)) {
                break;
              }
            } else if(!coversKing(to)) {
              continue;
            }
            SETMOVE_NORMAL(m, piece, cp, dirIndex, moveIndex, to, direction->m_direction);
            return true;
          }
        }
      }
      continue;
    }
  }
  return false;
}

bool MoveGeneratorLDCheck::nextMove(Move &m) const { // Assumes m is a valid move
  if(m.m_type == PROMOTION && (m.m_promoteIndex < ARRAYSIZE(Game::legalPromotions)-1)) {
    m.m_promoteIndex++;
    return true;
  }

  int startDir, startIndex;
  if(m.m_capturedPiece) { // Last move was a capture. Change to a new direction
    startDir   = m.m_dirIndex + 1;
    startIndex = 0;
  } else {
    startDir   = m.m_dirIndex;
    startIndex = m.m_moveIndex + 1;
  }

  Piece *cp;
  for(Piece *piece = m.m_piece; piece; piece = piece->m_next, startDir = startIndex = 0) {
    if(!piece->m_onBoard) {
      continue;
    }
    switch(piece->getType()) {
    case King:
      { const DirectionArray &da          = MoveTable::kingMoves[piece->m_position];
        const Direction      *direction   = da.m_directions + startDir;
        const KingAttackState kingAttacks = m_game.m_playerState[PLAYERINTURN].m_kingAttackState;
        const FieldAttacks   *attackTable = piece->m_enemyState.m_attackTable;

        for(int dirIndex = startDir; dirIndex < da.m_count; dirIndex++, startIndex = 0, direction++) {
          PositionArray fields = direction->m_fields;
          const int     count  = *(fields++);
          for(int moveIndex = startIndex; moveIndex < count; moveIndex++) {
            const int to = fields[moveIndex]; // Do not increment fields !
            if((cp = m_game.m_board[to]) && cp->getPlayer() == PLAYERINTURN || attackTable[to].m_isAttacked) {
              break; // Cannot capture my own piece or go to an attacked field. Try another direction
            }
            SETMOVE_NORMAL(m, piece, cp, dirIndex, moveIndex, to, direction->m_direction);
            if(isValidKingMove(m, kingAttacks)) {
              return true;
            }
          }
        }
      }
      continue;

    case Knight:
      { if(piece->m_pinnedState != NOT_PINNED) {
          // If Knight is pinned, it cannot be moved at all, because it always leaves all lines
          continue;
        }
        const DirectionArray &da        = MoveTable::knightMoves[piece->m_position];
        const Direction      *direction = da.m_directions + startDir;

        for(int dirIndex = startDir; dirIndex < da.m_count; dirIndex++, startIndex = 0, direction++) {
          PositionArray fields = direction->m_fields;
          const int     count  = *(fields++);
          for(int moveIndex = startIndex; moveIndex < count; moveIndex++) {
            const int to = fields[moveIndex]; // Do not increment fields !
            if(cp = m_game.m_board[to]) {
              if(cp->getPlayer() == PLAYERINTURN || !isCheckingLDAPiece(cp)) {
                break;
              }
            } else if(!coversKing(to)) {
              break;
            }
            SETMOVE_NORMAL(m, piece, cp, dirIndex, moveIndex, to, MD_NONE);
            return true;
          }
        }
      }
      continue;

    case Pawn:
      { const DirectionArray &da        = piece->m_moveTable[piece->m_position];
        const Direction      *direction = da.m_directions + startDir;

        for(int dirIndex = startDir; dirIndex < da.m_count; dirIndex++, startIndex = 0, direction++) {
          if(MoveTable::uncoversKing[piece->m_pinnedState][direction->m_direction]) {
            continue;
          }
          PositionArray fields = direction->m_fields;
          const int     count  = *(fields++);
          for(int moveIndex = startIndex; moveIndex < count; moveIndex++) {
            const int to = fields[moveIndex]; // Do not increment fields !
            if(cp = m_game.m_board[to]) {
              if(cp->getPlayer() == PLAYERINTURN || !IS_DIAGONALMOVE(direction->m_direction) || !isCheckingLDAPiece(cp)) {
                break;
              }
            } else if(IS_DIAGONALMOVE(direction->m_direction)) { // Cannot cover the king from LDA-check by capturing EP
              break;
            } else if(!coversKing(to)) {
              continue;
            }
            SETMOVE_CHECKFORPROMOTION(m, piece, cp, dirIndex, moveIndex, to, direction->m_direction);
            return true;
          }
        }
      }
      continue;

    default:
      { const DirectionArray &da        = piece->m_moveTable[piece->m_position];
        const Direction      *direction = da.m_directions + startDir;

        for(int dirIndex = startDir; dirIndex < da.m_count; dirIndex++, startIndex = 0, direction++) {
          if(MoveTable::uncoversKing[piece->m_pinnedState][direction->m_direction]) {
            continue;
          }
          PositionArray fields = direction->m_fields;
          const int     count  = *(fields++);
          for(int moveIndex = startIndex; moveIndex < count; moveIndex++) {
            const int to = fields[moveIndex]; // Do not increment fields !
            if(cp = m_game.m_board[to]) {
              if(cp->getPlayer() == PLAYERINTURN || !isCheckingLDAPiece(cp)) {
                break;
              }
            } else if(!coversKing(to)) {
              continue;
            }
            SETMOVE_NORMAL(m, piece, cp, dirIndex, moveIndex, to, direction->m_direction);
            return true;
          }
        }
      }
      continue;
    }
  }
  return false;
}

// --------------------------------------------- MoveGeneratorSDCheck ------------------------------------------------
// Used when king is checked by a Knight or Pawn. ((S)hort (D)istance check. Not doublecheck
// No castling move is generated by this generator.
// The only possible replies to a check by a knight or pawn is moving the king or capturing the checking piece, perhaps e.p.

// It has been checked that the piece belongs to the enemy
bool MoveGeneratorSDCheck::isCheckingPiece(const Piece *piece) const {
  return (int)piece->m_position == piece->m_playerState.m_checkingSDAPosition;
}

bool MoveGeneratorSDCheck::firstMove(Move &m) const {
  Piece *cp;
  for(Piece *piece = m_game.m_playerState[PLAYERINTURN].m_first; piece; piece = piece->m_next) {
    if(!piece->m_onBoard) {
      continue;
    }
    switch(piece->getType()) {
    case King:
      { const DirectionArray &da          = MoveTable::kingMoves[piece->m_position];
        const Direction      *direction   = da.m_directions;
        const FieldAttacks   *attackTable = piece->m_enemyState.m_attackTable;

        for(int dirIndex = 0; dirIndex < da.m_count; dirIndex++, direction++) {
          PositionArray fields = direction->m_fields;
          const int     count  = *(fields++);
          for(int moveIndex = 0; moveIndex < count; moveIndex++) {
            const int to = *(fields++);
            if((cp = m_game.m_board[to]) && cp->getPlayer() == PLAYERINTURN || attackTable[to].m_isAttacked) {
              break; // Cannot capture my own piece, or go to an attacked field. Try another direction
            }
            SETMOVE_NORMAL(m, piece, cp, dirIndex, moveIndex, to, direction->m_direction);
            return true;
          }
        }
      }
      continue;

    case Knight:
      { if(piece->m_pinnedState != NOT_PINNED) {
          // If Knight is pinned, it cannot be moved at all, because it always leaves all lines
          continue;
        }
        const DirectionArray &da        = MoveTable::knightMoves[piece->m_position];
        const Direction      *direction = da.m_directions;

        for(int dirIndex = 0; dirIndex < da.m_count; dirIndex++, direction++) {
          PositionArray fields = direction->m_fields;
          const int     count  = *(fields++);
          for(int moveIndex = 0; moveIndex < count; moveIndex++) {
            const int to = *(fields++);
            if(cp = m_game.m_board[to]) {
              if(cp->getPlayer() == PLAYERINTURN || !isCheckingPiece(cp)) {
                break;
              }
            } else {
              break; // Must capture the cheking piece (pawn og knight)
            }
            SETMOVE_NORMAL(m, piece, cp, dirIndex, moveIndex, to, MD_NONE);
            return true;
          }
        }
      }
      continue;

    case Pawn:
      { const DirectionArray &da        = piece->m_moveTable[piece->m_position];
        const Direction      *direction = da.m_directions;

        for(int dirIndex = 0; dirIndex < da.m_count; dirIndex++, direction++) {
          if(MoveTable::uncoversKing[piece->m_pinnedState][direction->m_direction]) {
            continue;
          }
          PositionArray fields = direction->m_fields;
          const int     count  = *(fields++);
          for(int moveIndex = 0; moveIndex < count; moveIndex++) {
            const int to = *(fields++);
            if(cp = m_game.m_board[to]) {
              if(cp->getPlayer() == PLAYERINTURN || !IS_DIAGONALMOVE(direction->m_direction) || !isCheckingPiece(cp)) {
                break;
              }
            } else if(IS_DIAGONALMOVE(direction->m_direction)) {
              CHECKFOR_EP_AND_CHECKINGPIECE(m, piece, dirIndex, moveIndex, to, direction->m_direction);
              break;
            } else {
              break; // Must capture the cheking piece (pawn og knight)
            }
            SETMOVE_CHECKFORPROMOTION(m, piece, cp, dirIndex, moveIndex, to, direction->m_direction);
            return true;
          }
        }
      }
      continue;

    default:
      { const DirectionArray &da        = piece->m_moveTable[piece->m_position];
        const Direction      *direction = da.m_directions;

        for(int dirIndex = 0; dirIndex < da.m_count; dirIndex++, direction++) {
          if(MoveTable::uncoversKing[piece->m_pinnedState][direction->m_direction]) {
            continue;
          }
          PositionArray fields = direction->m_fields;
          const int     count  = *(fields++);
          for(int moveIndex = 0; moveIndex < count; moveIndex++) {
            const int to = *(fields++);
            if(cp = m_game.m_board[to]) {
              if(cp->getPlayer() == PLAYERINTURN || !isCheckingPiece(cp)) {
                break;
              }
            } else {
              continue; // Must capture the cheking piece (pawn og knight)
            }
            SETMOVE_NORMAL(m, piece, cp, dirIndex, moveIndex, to, direction->m_direction);
            return true;
          }
        }
      }
      continue;
    }
  }
  return false;
}

bool MoveGeneratorSDCheck::nextMove(Move &m) const { // Assumes m is a valid move
  if(m.m_type == PROMOTION && (m.m_promoteIndex < ARRAYSIZE(Game::legalPromotions)-1)) {
    m.m_promoteIndex++;
    return true;
  }

  int startDir, startIndex;
  if(m.m_capturedPiece) { // Last move was a capture. Change to a new direction
    startDir   = m.m_dirIndex + 1;
    startIndex = 0;
  } else {
    startDir   = m.m_dirIndex;
    startIndex = m.m_moveIndex + 1;
  }

  Piece *cp;
  for(Piece *piece = m.m_piece; piece; piece = piece->m_next, startDir = startIndex = 0) {
    if(!piece->m_onBoard) {
      continue;
    }
    switch(piece->getType()) {
    case King:
      { const DirectionArray &da          = MoveTable::kingMoves[piece->m_position];
        const Direction      *direction   = da.m_directions + startDir;
        const FieldAttacks   *attackTable = piece->m_enemyState.m_attackTable;

        for(int dirIndex = startDir; dirIndex < da.m_count; dirIndex++, startIndex = 0, direction++) {
          PositionArray fields = direction->m_fields;
          const int     count  = *(fields++);
          for(int moveIndex = startIndex; moveIndex < count; moveIndex++) {
            const int to = fields[moveIndex]; // do not increment fields !
            if((cp = m_game.m_board[to]) && cp->getPlayer() == PLAYERINTURN || attackTable[to].m_isAttacked) {
              break; // Cannot capture my own piece or go to an attacked field. Try another direction
            }
            SETMOVE_NORMAL(m, piece, cp, dirIndex, moveIndex, to, direction->m_direction);
            return true;
          }
        }
      }
      continue;

    case Knight:
      { if(piece->m_pinnedState != NOT_PINNED) {
          // If Knight is pinned, it cannot be moved at all, because it always leaves all lines
          continue;
        }
        const DirectionArray &da        = MoveTable::knightMoves[piece->m_position];
        const Direction      *direction = da.m_directions + startDir;

        for(int dirIndex = startDir; dirIndex < da.m_count; dirIndex++, startIndex = 0, direction++) {
          PositionArray fields = direction->m_fields;
          const int     count  = *(fields++);
          for(int moveIndex = startIndex; moveIndex < count; moveIndex++) {
            const int to = fields[moveIndex]; // Do not increment fields !
            if(cp = m_game.m_board[to]) {
              if(cp->getPlayer() == PLAYERINTURN || !isCheckingPiece(cp)) {
                break;
              }
            } else {
              break;
            }
            SETMOVE_NORMAL(m, piece, cp, dirIndex, moveIndex, to, MD_NONE);
            return true;
          }
        }
      }
      continue;

    case Pawn:
      { const DirectionArray &da        = piece->m_moveTable[piece->m_position];
        const Direction      *direction = da.m_directions + startDir;

        for(int dirIndex = startDir; dirIndex < da.m_count; dirIndex++, startIndex = 0, direction++) {
          if(MoveTable::uncoversKing[piece->m_pinnedState][direction->m_direction]) {
            continue;
          }
          PositionArray fields = direction->m_fields;
          const int     count  = *(fields++);
          for(int moveIndex = startIndex; moveIndex < count; moveIndex++) {
            const int to = fields[moveIndex]; // Do not increment fields !
            if(cp = m_game.m_board[to]) {
              if(cp->getPlayer() == PLAYERINTURN || !IS_DIAGONALMOVE(direction->m_direction) || !isCheckingPiece(cp)) {
                break;
              }
            } else if(IS_DIAGONALMOVE(direction->m_direction)) {
              CHECKFOR_EP_AND_CHECKINGPIECE(m, piece, dirIndex, moveIndex, to, direction->m_direction);
              break;
            } else {
              break; // Must capture the cheking piece (pawn og knight)
            }
            SETMOVE_CHECKFORPROMOTION(m, piece, cp, dirIndex, moveIndex, to, direction->m_direction);
            return true;
          }
        }
      }
      continue;

    default:
      { const DirectionArray &da        = piece->m_moveTable[piece->m_position];
        const Direction      *direction = da.m_directions + startDir;

        for(int dirIndex = startDir; dirIndex < da.m_count; dirIndex++, startIndex = 0, direction++) {
          if(MoveTable::uncoversKing[piece->m_pinnedState][direction->m_direction]) {
            continue;
          }
          PositionArray fields = direction->m_fields;
          const int     count  = *(fields++);
          for(int moveIndex = startIndex; moveIndex < count; moveIndex++) {
            const int to = fields[moveIndex]; // Do not increment fields !
            if(cp = m_game.m_board[to]) {
              if(cp->getPlayer() == PLAYERINTURN || !isCheckingPiece(cp)) {
                break;
              }
            } else {
              continue; // Must capture the cheking piece (pawn og knight)
            }
            SETMOVE_NORMAL(m, piece, cp, dirIndex, moveIndex, to, direction->m_direction);
            return true;
          }
        }
      }
      continue;
    }
  }
  return false;
}

// --------------------------------------------- MoveGeneratorDoubleCheck ------------------------------------------------
// Used when king is in Double check.
// The only possible reply is a King move
bool MoveGeneratorDoubleCheck::firstMove(Move &m) const {
        Piece          *king        = m_game.m_playerState[PLAYERINTURN].m_king;
  const DirectionArray &da          = MoveTable::kingMoves[king->m_position];
  const Direction      *direction   = da.m_directions;
  const KingAttackState kingAttacks = king->m_playerState.m_kingAttackState;
  const FieldAttacks   *attackTable = king->m_enemyState.m_attackTable;
        Piece          *cp;

  for(int dirIndex = 0; dirIndex < da.m_count; dirIndex++, direction++) {
    PositionArray fields = direction->m_fields;
    const int     count  = *(fields++);
    for(int moveIndex = 0; moveIndex < count; moveIndex++) {
      const int to = *(fields++);
      if((cp = m_game.m_board[to]) && cp->getPlayer() == PLAYERINTURN || attackTable[to].m_isAttacked) {
        break; // Cannot capture my own piece, or go to an attacked field. Try another direction
      }
      SETMOVE_NORMAL(m, king, cp, dirIndex, moveIndex, to, direction->m_direction);
      if(isValidKingMove(m, kingAttacks)) {
        return true;
      }
    }
  }
  return false;
}

bool MoveGeneratorDoubleCheck::nextMove(Move &m) const { // Assumes m is a valid move
        Piece          *king        = m.m_piece;
  const DirectionArray &da          = MoveTable::kingMoves[king->m_position];
  const int             startDir    = m.m_dirIndex + 1;
  const Direction      *direction   = da.m_directions + startDir;
  const KingAttackState kingAttacks = king->m_playerState.m_kingAttackState;
  const FieldAttacks   *attackTable = king->m_enemyState.m_attackTable;
        Piece          *cp;

  for(int dirIndex = startDir; dirIndex < da.m_count; dirIndex++, direction++) {
    PositionArray fields = direction->m_fields;
    const int     count  = *(fields++);
    for(int moveIndex = 0; moveIndex < count; moveIndex++) {
      const int to = fields[moveIndex]; // Do not increment fields !
      if((cp = m_game.m_board[to]) && cp->getPlayer() == PLAYERINTURN || attackTable[to].m_isAttacked) {
        break;
      }
      SETMOVE_NORMAL(m, king, cp, dirIndex, moveIndex, to, direction->m_direction);
      if(isValidKingMove(m, kingAttacks)) {
        return true;
      }
    }
  }
  return false;
}
