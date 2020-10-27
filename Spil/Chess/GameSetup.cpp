#include "stdafx.h"

const PieceType Game::s_officersStartConfiguration[8] = { Rook, Knight, Bishop, Queen, King, Bishop, Knight, Rook };
const PieceType Game::s_legalPromotions[4]            = { Queen, Knight, Rook, Bishop };

int Game::getPromoteIndex(PieceType pt) { // static
  switch (pt) {
  case Queen : return 0;
  case Knight: return 1;
  case Rook  : return 2;
  case Bishop: return 3;
  default    : throwInvalidArgumentException(__TFUNCTION__, _T("pt=%d"), pt);
               return 0;
  }
}

Piece *Game::findUnusedPiece(PieceType pieceType, Player player) {
  return m_playerState[player].findUnusedPiece(pieceType);
}

Game &Game::endSetup(const GameKey *key) {
  checkSetupMode(true);
  initState(false, key);
  m_setupMode = false;
  return *this;
}

int Game::getPieceCountOnBoard(Player player) const {
  return m_playerState[player].getPieceCountOnBoard();
}

int Game::getPieceCountOnBoard(PieceType pieceType, Player player) const {
  return m_playerState[player].getPieceCountOnBoard(pieceType);
}

int Game::getPieceCountOnBoard() const {
  return getPieceCountOnBoard(WHITEPLAYER) + getPieceCountOnBoard(BLACKPLAYER);
}

int Game::getNonPawnCountOnBoard(Player player) const {
  return m_playerState[player].getNonPawnCountOnBoard();
}

int Game::getNonPawnCountOnBoard() const {
  return getNonPawnCountOnBoard(WHITEPLAYER) + getNonPawnCountOnBoard(BLACKPLAYER);
}

PieceKey Game::setPieceAtPosition(PieceKey key, int pos) {
  if(key == EMPTYPIECEKEY) {
    return removePieceAtPosition(pos);
  } else {
    return setPieceAtPosition(GET_TYPE_FROMKEY(key), GET_PLAYER_FROMKEY(key), pos);
  }
}

PieceKey Game::setPieceAtPosition(PieceType pieceType, Player player, int pos) {
#if !defined(TABLEBASE_BUILDER)
  checkSetupMode(true);
  validateAddPieceAtPosition(pieceType, player, pos, false);
#endif

  Piece *old = m_board[pos];
  PieceKey result;
  if(old) {
    old->m_onBoard = false;
    result = old->getKey();
  } else {
    result = EMPTYPIECEKEY;
  }
  try {
    Piece *piece = findUnusedPiece(pieceType, player);
    m_board[piece->m_position = pos] = piece;
    piece->m_onBoard = true;
    return result;
  } catch(...) {
    if(old) {
      old->m_onBoard = true; // set it back again
    }
    throw;
  }
}

PieceKey Game::removePieceAtPosition(int pos, bool resetType) {
  checkSetupMode(true);
  VALIDATEPOS(pos);

  Piece *piece = m_board[pos];
  if(piece == nullptr) {
    return EMPTYPIECEKEY;
  }
  m_board[pos] = nullptr;
  if(resetType && piece->isPromotedPawn()) {
    piece->setType(Pawn);
  }
  piece->m_onBoard = false;
  return piece->getKey();
}

bool Game::isPositionEmpty(int pos) const {
  VALIDATEPOS(pos);
  return m_board[pos] == nullptr;
}

bool Game::isAddPieceLegal(PieceKey key, int pos, bool validatePromotions) const {
  return isAddPieceLegal(GET_TYPE_FROMKEY(key), GET_PLAYER_FROMKEY(key), pos, validatePromotions);
}

bool Game::isAddPieceLegal(PieceType pieceType, Player player, int pos, bool validatePromotions) const {
  try {
    validateAddPieceAtPosition(pieceType, player, pos, validatePromotions);
    return true;
  } catch(Exception e) {
    return false;
  }
}

void Game::validateAddPieceAtPosition(PieceType pieceType, Player player, int pos, bool validatePromotions) const {
  m_playerState[player].validateAddPieceAtPosition(pieceType, pos, validatePromotions);
}

const Piece *Game::getPieceAtPosition(int pos) const {
  VALIDATEPOS(pos);
  return m_board[pos];
}

PieceKey Game::getPieceKeyAtPosition(int pos) const {
  const Piece *piece = getPieceAtPosition(pos);
  return piece ? piece->getKey() : EMPTYPIECEKEY;
}

Game &Game::clearBoard() {
  checkSetupMode(true);

  const Player savePlayerInTurn = PLAYERINTURN;
  setupStartPosition();
  memset(m_board,0, sizeof(m_board));
  forEachPlayer(p) {
    m_playerState[p].clear();
  }
  m_gameKey.clear(savePlayerInTurn);
  return *this;
}

Game &Game::setPlayerInTurn(Player player) {
  checkSetupMode(true);
  m_gameKey.d.m_playerInTurn = player;
  return *this;
}

int Game::getWalkDistance(int pos1, int pos2) { // static
  const int dr = ::abs(GETROW(pos1)-GETROW(pos2));
  const int dc = ::abs(GETCOL(pos1)-GETCOL(pos2));
  return max(dr, dc);
}

Game &Game::validateBoard(bool intensive) {
  forEachPlayer(p) {
    PlayerState &state = m_playerState[p];
    if(state.m_king == nullptr || !state.m_king->m_onBoard) {
      throwUserException(IDS_MSG_s_HAS_NO_KING, getPlayerName(p));
    }
  }

  if(POSADJACENT(m_playerState[WHITEPLAYER].m_king->getPosition(),m_playerState[BLACKPLAYER].m_king->getPosition())) {
    throwUserException(IDS_MSG_KINGS_ATTACK_EACHOTHER);
  }

  initState(intensive);

  forEachPlayer(player) {
    PlayerState &state = m_playerState[player];
    if(player != PLAYERINTURN && IS_KINGINCHECK(player)) {
      throwUserException(IDS_MSG_s_KING_IS_IN_CHECK, getPlayerName(player));
    }
    const int pawnsOnBoard = getPieceCountOnBoard(Pawn, player);
    if(pawnsOnBoard > 8) {
      throwUserException(IDS_MSG_s_CAN_ONLY_HAVE_8_PAWNS, getPlayerName(player));
    }
    state.validatePromotionCount();
  }
  setGameResult();

  return *this;
}

Game &Game::setupStartPosition() {
  checkSetupMode(true);

  memset(m_board,0, sizeof(m_board));

  forEachPlayer(player) {
    PlayerState &state = m_playerState[player];
    state.clear();
    state.m_player = player;
    int i = 0;
    for(; i < 8; i++) {
      const int r = (player==WHITEPLAYER) ? 0 : 7;
      const int c = i;
      Piece *p = state.m_pieces[i];
      p->setType(s_officersStartConfiguration[i]);
      m_board[p->m_position = MAKE_POSITION(r, c)] = p;
      p->m_onBoard = true;
    }

    for(int c = 0; i < 16; i++, c++) {
      const int r = (player==WHITEPLAYER) ? 1 : 6;
      Piece *p = state.m_pieces[i];
      p->setType(Pawn);
      m_board[p->m_position = MAKE_POSITION(r, c)] = p;
      p->m_onBoard = true;
    }
  }
  m_gameKey.d.m_playerInTurn = WHITEPLAYER;
  resetGameHistory();
  return *this;
};

void Game::initState(bool validate, const GameKey *key) {
  updateLinks();
  initAttackCounters(validate);
  initKingDirections();
  initPinnedState();

  if(key) {
    m_gameKey = *key;
  } else {
    initGameKey();
  }
#if !defined(TABLEBASE_BUILDER)
  setGameResult();
#endif
}

void Game::initAttackCounters(bool validate) {
  forEachPlayer(p) {
    PlayerState &state = m_playerState[p];

    memset(state.m_attackTable, 0, sizeof(state.m_attackTable));
    state.m_checkingSDAPosition = -1;

    for(const Piece *piece = state.m_first; piece; piece = piece->m_next) {
      const FieldInfo &posInfo = s_fieldInfo[piece->m_position];

      switch(piece->getType()) {
      // SDA-pieces
      case King  :
        UPDATE_KINGATTACKS(p, posInfo, 1);
        break;

      case Knight:
        UPDATE_KNIGHTATTACKS(p, posInfo, 1);
        break;

      case Pawn  :
        if(isValidPawnPosition(posInfo.m_pos)) {
          if(p==WHITEPLAYER) {
            UPDATE_WHITEPAWNATTACKS(posInfo, 1);
          } else {
            UPDATE_BLACKPAWNATTACKS(posInfo, 1);
          }
        }
        break;

      // LDA-pieces
      case Queen :
      case Rook  :
        if(posInfo.m_rowLine.m_lower  ) {
          SET_LDA_LEFT(     state, posInfo);
        }
        if(posInfo.m_rowLine.m_upper  ) {
          SET_LDA_RIGHT(    state, posInfo);
        }
        if(posInfo.m_colLine.m_lower  ) {
          SET_LDA_DOWN(     state, posInfo);
        }
        if(posInfo.m_colLine.m_upper  ) {
          SET_LDA_UP(       state, posInfo);
        }
        if(piece->getType() == Rook) {
          break;
        }
        // Must be Qeen. continue case
      case Bishop:
        if(posInfo.m_diag1Line.m_lower) {
          SET_LDA_DOWNDIAG1(state, posInfo);
        }
        if(posInfo.m_diag1Line.m_upper) {
          SET_LDA_UPDIAG1(  state, posInfo);
        }
        if(posInfo.m_diag2Line.m_lower) {
          SET_LDA_DOWNDIAG2(state, posInfo);
        }
        if(posInfo.m_diag2Line.m_upper) {
          SET_LDA_UPDIAG2(  state, posInfo);
        }
        break;
      }
    }
  }

  m_playerState[WHITEPLAYER].setKingAttackState();
  m_playerState[BLACKPLAYER].setKingAttackState();

  forEachPlayer(player) {
    PlayerState &state = m_playerState[player];
    const int enemyKingPos = m_playerState[GETENEMY(player)].m_king->m_position;
    if(state.m_attackTable[enemyKingPos].m_attackInfo.m_sdAttacks) { // must be a pawn or knight
      if((state.m_checkingSDAPosition = findAttackingPawnPosition(player,enemyKingPos)) < 0) {
        state.m_checkingSDAPosition = findAttackingKnightPosition(player,enemyKingPos);
      }
    }
  }

  if(validate) {
    forEachPlayer(player) {
      const PlayerState  &playerState  = m_playerState[player];
      const PlayerState  &enemyState   = m_playerState[GETENEMY(player)];
      const int           enemyKingPos = enemyState.m_king->getPosition();
      const FieldAttacks &fieldAttacks = FIELDATTACKS(playerState,enemyKingPos);
      const AttackInfo   &attackInfo   = fieldAttacks.m_attackInfo;

      if(fieldAttacks.m_isAttacked) {
        const UINT knightAttacks         = getKnightAttackCount(player, enemyKingPos   );
        const UINT pawnDiag1             = getPawnAttack(       player, enemyKingPos, 1);
        const UINT pawnDiag2             = getPawnAttack(       player, enemyKingPos, 2);
        const int rowAttacks             = ATT_LEFT(     attackInfo)   + ATT_RIGHT(  attackInfo);
        const int colAttacks             = ATT_DOWN(     attackInfo)   + ATT_UP(     attackInfo);
        const int diag1Attacks           = ATT_DOWNDIAG1(attackInfo)   + ATT_UPDIAG1(attackInfo);
        const int diag2Attacks           = ATT_DOWNDIAG2(attackInfo)   + ATT_UPDIAG2(attackInfo);
        const int pawnAttacks            = pawnDiag1 + pawnDiag2;

        const String kingStr       = format(_T("%s %s"), getPlayerName(GETENEMY(player)), toLowerCase(getPieceTypeName(King)).cstr());
        const String impossibleStr = loadString(IDS_IMPOSSIBLE);

        if(ATT_LEFT(attackInfo) && ATT_RIGHT(attackInfo)) {
          throwUserException(IDS_MSG_s_KING_ATTACKED_LEFT_RIGHT_s, kingStr.cstr(), impossibleStr.cstr());
        }
        if(ATT_DOWN(attackInfo) && ATT_UP(attackInfo)) {
          throwUserException(IDS_MSG_s_KING_ATTACKED_LOWER_UPPER_s, kingStr.cstr(), impossibleStr.cstr());
        }
        if(ATT_DOWNDIAG1(attackInfo) && ATT_UPDIAG1(attackInfo)) {
          throwUserException(IDS_MSG_s_KING_ATTACKED_LOWER_UPPER_DIAGONAL_s, kingStr.cstr(), impossibleStr.cstr());
        }
        if(ATT_DOWNDIAG2(attackInfo) && ATT_UPDIAG2(attackInfo)) {
          throwUserException(IDS_MSG_s_KING_ATTACKED_LOWER_UPPER_DIAGONAL_s, kingStr.cstr(), impossibleStr.cstr());
        }

#if !defined(TABLEBASE_BUILDER)
        if(rowAttacks && colAttacks) {
          // Can only happen, if attacked by a newly promoted pawn (promoted to a queen og rook),
          // that has just captured a piece beside the king !
          const int attackColumn = GETCOL(getKingRowAttackedFrom(enemyState.m_player));
          const int attackRow    = GETROW(getKingColAttackedFrom(enemyState.m_player));

          if((GETROW(enemyKingPos) != GETPROMOTEROW(player))
            || (abs(attackColumn - GETCOL(enemyKingPos)) > 1)
            || (abs(attackRow    - GETROW(enemyKingPos)) == 1)) {
            throwUserException(IDS_MSG_s_KING_ATTACKED_VERTICAL_HORIZONTAL, kingStr.cstr());
          }
        }
#endif
        if(diag1Attacks && diag2Attacks) {
          throwUserException(IDS_MSG_s_KING_ATTACKED_BOTH_DIAGONALS_s, kingStr.cstr(), impossibleStr.cstr());
        }
        if(knightAttacks > 1) {
          throwUserException(IDS_MSG_s_KING_ATTACKED_BY_d_s_s
                            , kingStr.cstr()
                            , knightAttacks, toLowerCase(getPieceTypeName(Knight, true)).cstr()
                            , impossibleStr.cstr());
        }
        if(pawnAttacks) {
          if(pawnAttacks > 1) {
            throwUserException(IDS_MSG_s_KING_ATTACKED_BY_d_s_s
                              , kingStr.cstr()
                              , pawnAttacks, toLowerCase(getPieceTypeName(Pawn, true)).cstr()
                              , impossibleStr.cstr());
          }

          if(rowAttacks) {
            throwUserException(IDS_MSG_s_KING_ATTACKED_BY_d_s_AND_FROM_SIDE_s
                              , kingStr.cstr()
                              , pawnAttacks, toLowerCase(getPieceTypeName(Pawn, pawnAttacks>1)).cstr()
                              , impossibleStr.cstr());
          }
          if(knightAttacks) {
            throwUserException(IDS_MSG_s_KING_ATTACKED_BY_d_s_AND_d_s_s
                              , kingStr.cstr()
                              , pawnAttacks  , toLowerCase(getPieceTypeName(Pawn  , pawnAttacks   > 1)).cstr()
                              , knightAttacks, toLowerCase(getPieceTypeName(Knight, knightAttacks > 1)).cstr()
                              , impossibleStr.cstr());
          }
          if(colAttacks) {
            if((player == WHITEPLAYER) && ATT_DOWN(attackInfo) || (player == BLACKPLAYER) && ATT_UP(attackInfo)) {
              throwUserException(IDS_MSG_s_KING_ATTACKED_BY_d_s_AND_FROM_ABOVE_s
                                , kingStr.cstr()
                                , pawnAttacks, toLowerCase(getPieceTypeName(Pawn, pawnAttacks>1)).cstr()
                                , impossibleStr.cstr());
            }
          }
          int enemyKingRow = GETROW(enemyKingPos);
          if(((player == WHITEPLAYER) && (enemyKingRow == 2)) || ((player == BLACKPLAYER) && (enemyKingRow == 5))) {
            throwUserException(IDS_MSG_s_KING_ATTACKED_BY_d_s_UNMOVED_PAWN_s
                              , kingStr.cstr()
                              , pawnAttacks, toLowerCase(getPieceTypeName(Pawn, pawnAttacks>1)).cstr()
                              , impossibleStr.cstr());
          }
        }
      }
    }
  }
}

void Game::initKingDirections() {
  forEachPlayer(p) {
    PlayerState &state = m_playerState[p];
    if(state.m_king->m_onBoard) {
      const FieldInfo &kingInfo = s_fieldInfo[state.m_king->m_position];
      if(kingInfo.m_rowLine.m_lower) {
        setKingRight(    state, kingInfo);
      }
      if(kingInfo.m_rowLine.m_upper) {
        setKingLeft(     state, kingInfo);
      }
      if(kingInfo.m_colLine.m_lower) {
        setKingUp(       state, kingInfo);
      }
      if(kingInfo.m_colLine.m_upper) {
        setKingDown(     state, kingInfo);
      }
      if(kingInfo.m_diag1Line.m_lower) {
        setKingUpDiag1(  state, kingInfo);
      }
      if(kingInfo.m_diag1Line.m_upper) {
        setKingDownDiag1(state, kingInfo);
      }
      if(kingInfo.m_diag2Line.m_lower) {
        setKingUpDiag2(  state, kingInfo);
      }
      if(kingInfo.m_diag2Line.m_upper) {
        setKingDownDiag2(state, kingInfo);
      }
    }
  }
}

int Game::getKingRowAttackedFrom(Player player) const {
  const PlayerState &state = m_playerState[player];
  if(state.m_king == nullptr || !state.m_king->isOnBoard()) {
    return -1;
  }
  const int         kingPos = state.m_king->getPosition();
  const DoubleLine &row     = s_fieldInfo[kingPos].m_rowLine;
  const Piece      *p1      = findFirstPieceInDirection(row.m_lower);
  const Piece      *p2      = findFirstPieceInDirection(row.m_upper);
  const Player      enemy   = GETENEMY(player);
  if(p1 && LONGDISTANCE_ATTACKS(enemy, p1, ATTACKS_PARALLEL)) {
    return p1->getPosition();
  } else if(p2 && LONGDISTANCE_ATTACKS(enemy, p2, ATTACKS_PARALLEL)) {
    return p2->getPosition();
  } else {
    return -1;
  }
}

int Game::getKingColAttackedFrom(Player player) const {
  const PlayerState &state = m_playerState[player];
  if(state.m_king == nullptr || !state.m_king->isOnBoard()) {
    return -1;
  }
  const int         kingPos = state.m_king->getPosition();
  const DoubleLine &col     = s_fieldInfo[kingPos].m_colLine;
  const Piece      *p1      = findFirstPieceInDirection(col.m_lower);
  const Piece      *p2      = findFirstPieceInDirection(col.m_upper);
  const Player      enemy   = GETENEMY(player);
  if(p1 && LONGDISTANCE_ATTACKS(enemy, p1, ATTACKS_PARALLEL)) {
    return p1->getPosition();
  } else if(p2 && LONGDISTANCE_ATTACKS(enemy, p2, ATTACKS_PARALLEL)) {
    return p2->getPosition();
  } else {
    return -1;
  }
}

int Game::getKnightAttackCount(Player player, int pos) const {
  int result = 0;
  PositionArray positions = s_fieldInfo[pos].m_knightAttacks;
  for(int count = *(positions++); count--;) {
    const Piece *p = m_board[*(positions++)];
    if(p && (p->getPlayer() == player) && (p->getType() == Knight)) {
      result++;
    }
  }
  return result;
}

int Game::getPawnAttack(Player player, int pos, int diagonal) const { // diagonal = {1,2}
  DEFINEMETHODNAME;
  const int row = GETROW(pos);
  const int col = GETCOL(pos);
  int pp;
  switch(player) {
  case WHITEPLAYER:
    { switch(diagonal) {
      case 1:
        { if(col == 0 || row < 2) {
            return 0;
          }
          pp = MAKE_POSITION(row-1, col-1);
        }
        break;

      case 2:
        { if(col == 7 || row < 2) {
            return 0;
          }
          pp = MAKE_POSITION(row-1, col+1);
        }
        break;
      default:
        NODEFAULT;
      }
    }
    break;

  case BLACKPLAYER:
    { switch(diagonal) {
      case 1:
        { if(col == 7 || row > 5) {
            return 0;
          }
          pp = MAKE_POSITION(row+1, col+1);
        }
        break;

      case 2:
        { if(col == 0 || row > 5) {
            return 0;
          }
          pp = MAKE_POSITION(row+1, col-1);
        }
        break;
      default:
        NODEFAULT;
      }
    }
    break;
  default:
    NODEFAULT;
  }
  const Piece *p = m_board[pp];
  return ((p != nullptr) && (p->getType() == Pawn) && (p->getPlayer() == player)) ? 1 : 0;
}

const Piece *Game::findLDAttackingPiece(Player player, int pos, bool diagonalAttack) const {
  const FieldAttacks &attInfo = m_playerState[player].m_attackTable[pos];
  const FieldInfo    &finfo   = s_fieldInfo[pos];
  if(diagonalAttack) {
    if(attInfo.m_attackDirectionInfo1.m_diag1Attacked) {
      return findFirstPieceInDirection(attInfo.m_attackInfo.m_fromLowerDiag1 ? finfo.m_diag1Line.m_lower : finfo.m_diag1Line.m_upper);
    } else if(attInfo.m_attackDirectionInfo1.m_diag2Attacked) {
      return findFirstPieceInDirection(attInfo.m_attackInfo.m_fromLowerDiag2 ? finfo.m_diag2Line.m_lower : finfo.m_diag2Line.m_upper);
    }
  } else {
    if(attInfo.m_attackDirectionInfo1.m_rowAttacked) {
      return findFirstPieceInDirection(attInfo.m_attackInfo.m_fromLeft       ? finfo.m_rowLine.m_lower   : finfo.m_rowLine.m_upper  );
    } else if(attInfo.m_attackDirectionInfo1.m_colAttacked) {
      return findFirstPieceInDirection(attInfo.m_attackInfo.m_fromBelove     ? finfo.m_colLine.m_lower   : finfo.m_colLine.m_upper  );
    }
  }
  return nullptr;
}

int Game::findAttackingKnightPosition(Player player, int pos) const {
  PositionArray positions = s_fieldInfo[pos].m_knightAttacks;
  const PieceKey knightKey = MAKE_PIECEKEY(player, Knight);
  for(int count = *(positions++); count--;) {
    const int kpos = *(positions++);
    const Piece *piece = m_board[kpos];
    if(piece && (piece->m_pieceKey == knightKey)) {
      return kpos;
    }
  }
  return -1;
}

int Game::findAttackingPawnPosition(Player player, int pos) const {
  PositionArray positions = (player==WHITEPLAYER) ? s_fieldInfo[pos].m_attackingWhitePawnPositions : s_fieldInfo[pos].m_attackingBlackPawnPositions;
  if(positions) {
    const PieceKey pawnKey = MAKE_PIECEKEY(player, Pawn);
    for(int count = *(positions++); count--;) {
      const int ppos = *(positions++);
      const Piece *piece = m_board[ppos];
      if(piece && (piece->m_pieceKey == pawnKey)) {
        return ppos;
      }
    }
  }
  return -1;
}

// should only be called from initState
// Assume all pieces on board are linked, and have their onBoard mark set to true
void Game::initPinnedState() {
  forEachPlayer(p) {
    for(Piece *piece = m_playerState[p].m_first; piece; piece = piece->m_next) {
      piece->m_pinnedState = (piece->getType() == King) ? NOT_PINNED : findPinnedState(piece);
    }
  }
}

// Should only be called from initPinnedState
// Assume piece is on the board and is not the king
PinnedState Game::findPinnedState(const Piece *piece) const {
  DEFINEMETHODNAME;
  const int pos = piece->m_position;
  switch(KING_DIRECTION(piece->m_playerState, pos)) {
  case MD_NONE     : return NOT_PINNED;
  case MD_LEFT     : return ATT_LEFT(     piece->m_enemyState.m_attackTable[pos].m_attackInfo) ? PINNED_TO_ROW   : NOT_PINNED;
  case MD_RIGHT    : return ATT_RIGHT(    piece->m_enemyState.m_attackTable[pos].m_attackInfo) ? PINNED_TO_ROW   : NOT_PINNED;
  case MD_DOWN     : return ATT_DOWN(     piece->m_enemyState.m_attackTable[pos].m_attackInfo) ? PINNED_TO_COL   : NOT_PINNED;
  case MD_UP       : return ATT_UP(       piece->m_enemyState.m_attackTable[pos].m_attackInfo) ? PINNED_TO_COL   : NOT_PINNED;
  case MD_DOWNDIAG1: return ATT_DOWNDIAG1(piece->m_enemyState.m_attackTable[pos].m_attackInfo) ? PINNED_TO_DIAG1 : NOT_PINNED;
  case MD_UPDIAG1  : return ATT_UPDIAG1(  piece->m_enemyState.m_attackTable[pos].m_attackInfo) ? PINNED_TO_DIAG1 : NOT_PINNED;
  case MD_DOWNDIAG2: return ATT_DOWNDIAG2(piece->m_enemyState.m_attackTable[pos].m_attackInfo) ? PINNED_TO_DIAG2 : NOT_PINNED;
  case MD_UPDIAG2  : return ATT_UPDIAG2(  piece->m_enemyState.m_attackTable[pos].m_attackInfo) ? PINNED_TO_DIAG2 : NOT_PINNED;
  default          : throwException(_T("%s:Unknown kingDirection:%d"), method, KING_DIRECTION(piece->m_playerState, pos));
  }
  return NOT_PINNED;
}

// NB! Don't touch m_playerInTurn.
void Game::initGameKey() {
  m_gameKey.clear(m_gameKey.getPlayerInTurn());
  forEachPlayer(p) {
    for(const Piece *piece = m_playerState[p].m_first; piece; piece = piece->m_next) {
      m_gameKey.m_pieceKey[piece->m_position] = piece->m_pieceKey;
    }
  }
  m_gameKey.initCastleState();
}

void Game::setCastleState(Player player, CastleState state) {
  m_gameKey.d.m_castleState[player] = state;
}

