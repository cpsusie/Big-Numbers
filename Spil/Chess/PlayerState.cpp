#include "stdafx.h"

PlayerState::PlayerState() {
  for(int i = 0; i < ARRAYSIZE(m_pieces); i++) {
    m_pieces[i] = NULL;
  }
}

PlayerState::~PlayerState() {
  for(int i = 0; i < ARRAYSIZE(m_pieces); i++) {
    delete m_pieces[i];
    m_pieces[i] = NULL;
  }
}

void PlayerState::allocatePieceTable(Player player, PlayerState &enemyState, const CastleState *castleState) {
  m_player      = player;
  m_castleState = castleState;
  m_enemyState  = &enemyState;

  for(int i = 0; i < ARRAYSIZE(m_pieces); i++) {
    PieceType type;
    int row, col;
    if(i < ARRAYSIZE(Game::officersStartConfiguration)) {
      type = Game::officersStartConfiguration[i];
      row  = player == WHITEPLAYER ? 0 : 7;
      col  = i % 8;
    } else {
      type = Pawn;
      row  = player == WHITEPLAYER ? 1 : 6;
      col  = i % 8;
    }
    m_pieces[i] = new Piece(type, *this, enemyState, i, row, col);
    if(type == King) {
      m_king = m_pieces[i];
    }
  }
}

PlayerState &PlayerState::operator=(const PlayerState &src) {
  if(this == &src) {
    return *this;
  }
  (*(PlayerStateToPush*)this) = src;
  m_player = src.m_player;
  for(int i = 0; i < ARRAYSIZE(m_pieces); i++) {
    Piece *p = m_pieces[i];
    const Piece *srcP = src.m_pieces[i];
    p->setType(srcP->getType());
    p->m_onBoard     = srcP->m_onBoard;
    p->m_position    = srcP->m_position;
    p->m_pinnedState = srcP->m_pinnedState;
    p->m_next        = NULL;
  }
  return makeLinks();
}

void PlayerState::clear() {
  for(int pieceIndex = 0; pieceIndex < ARRAYSIZE(m_pieces); pieceIndex++) {
    m_pieces[pieceIndex]->m_onBoard = false;
  }

  memset(m_attackTable,0, sizeof(m_attackTable));
  m_checkingSDAPosition = -1;
#ifndef TABLEBASE_BUILDER
  m_bishopFlags         = 0;
#endif
  m_kingAttackState     = 0;
  m_first               = NULL;
}

PlayerState &PlayerState::makeLinks() {

#ifdef TABLEBASE_BUILDER

  Piece *next = NULL;
  for(int i = ARRAYSIZE(m_pieces); i--;) {
    Piece *piece = m_pieces[i];
    if(piece->getType() == King) {
      m_king = piece;
    }
    if(piece->m_onBoard) {
      piece->m_next = next;
      next          = piece;
    }
  }
  m_first = next;

#else

  Piece *next         = NULL;
  m_king              = NULL;

  m_totalMaterial     = 0;
  m_bishopFlags       = 0;
  m_positionalScore   = 0;

  // Create linked list of all pieces on the board
  for(int i = ARRAYSIZE(m_pieces); i--;) {
    Piece *piece = m_pieces[i];
    if(piece->getType() == King) {
      if(m_king != NULL) {
        throwException(_T("Internal error. Two kings in PlayerState for %s"), toLowerCase(getPlayerName(m_player)).cstr());
      } else {
        m_king = piece;
      }
    }

    if(piece->m_onBoard) {
      piece->m_next = next;
      next          = piece;

      piece->initBishopMask(piece->m_position);
      m_totalMaterial += piece->m_materialValue;
      switch(piece->getType()) {
      case King  :
      case Queen :
      case Rook  :
        break;
      case Bishop:
        m_bishopFlags |= piece->m_bishopFlag;
        break;

      case Knight:
        m_positionalScore += MoveTable::knightPositionScore[m_player][piece->getPosition()];
        break;
      case Pawn  :
        m_positionalScore += MoveTable::pawnPositionScore[m_player][piece->getPosition()];
        break;
      }
    }
  }
  m_first = next;

  if(m_king == NULL) {
    throwException(_T("Internal error. No king in PlayerState for %s"), toLowerCase(getPlayerName(m_player)).cstr());
  }
  if(m_bishopFlags == BISHOPPAIR) {
    m_positionalScore += BISHOPPAIRBONUS;
  }
  initPawnCount();
#endif

  return *this;
}

class PromoteTypeSet {
private:
  unsigned char m_data;
  void add(PieceType type) {
    m_data |= (1<<type);
  }
    ;
public:
  PromoteTypeSet();
  inline bool contains(PieceType type) const {
    return (m_data & (1<<type)) != 0;
  }
};

PromoteTypeSet::PromoteTypeSet() {
  m_data = 0;
  add(Queen);
  add(Rook);
  add(Bishop);
  add(Knight);
}

static const PromoteTypeSet legalPromoteTypeSet;

Piece *PlayerState::findUnusedPiece(PieceType pieceType) {
  for(int pieceIndex = 0; pieceIndex < ARRAYSIZE(m_pieces);) { // first try to find a captured piece of the wanted type
    Piece *piece = m_pieces[pieceIndex++];
    if(!piece->isOnBoard() && (piece->getType() == pieceType)) {
      return piece;
    }
  }

  switch(pieceType) {
  case King:                                                       // king is never a promoted pawn!.
    break;
  case Pawn:
    { for(int pieceIndex = 0; pieceIndex < ARRAYSIZE(m_pieces);) {     // try to unpromote a piece back to pawn
        Piece *piece = m_pieces[pieceIndex++];
        if(!piece->isOnBoard() && (piece->getType() != King)) {    // king cannot be unpromoted
          piece->setType(Pawn);
          return piece;
        }
      }
    }
    break;
  default:
    { for(int pieceIndex = 0; pieceIndex < ARRAYSIZE(m_pieces);) {     // first try to find a captured pawn to promote
        Piece *piece = m_pieces[pieceIndex++];
        if(!piece->isOnBoard() && piece->getType() == Pawn) {
          piece->setType(pieceType);
          return piece;
        }
      }
      for(int pieceIndex = 0; pieceIndex < ARRAYSIZE(m_pieces);) {     // then try to find a captured piece of another type
        Piece *piece = m_pieces[pieceIndex++];
        if(!piece->isOnBoard() && piece->getType() != King) {
          piece->setType(pieceType);
          return piece;
        }
      }
    }
    break;
  }

  verbose(_T("Failed to find an unused %s\n"), getPieceNameEnglish(MAKE_PIECEKEY(m_player, pieceType)).cstr());
  verbose(_T("%s offboardpieces:\n"), getPlayerNameEnglish(m_player));
  for(int pieceIndex = 0; pieceIndex < ARRAYSIZE(m_pieces);) {
    const Piece *piece = m_pieces[pieceIndex++];
    if(!piece->isOnBoard()) {
      verbose(_T("%s\n"), getPieceNameEnglish(piece->getKey()).cstr());
    }
  }
  throwUserException(IDS_MSG_CANNOT_ADD_ANOTHER_s_s, toLowerCase(getPlayerName(m_player)).cstr(), toLowerCase(getPieceTypeName(pieceType)).cstr());
  return NULL;
}

#ifndef TABLEBASE_BUILDER

ScoreEvaluator PlayerState::getScoreFunction() const {
  switch(getPieceCountOnBoard()) {
  case 1 :
    return &PlayerState::evaluateKingSeachMiddle;
  default:
    if((getNonPawnCountOnBoard() <= 3) && (m_pawnMask != 0)) { // 1 or 2 non-pawn pieces + king + a number of pawns on the board => endgame
      return &PlayerState::evaluateEndGame;
    } else {
      return (*m_castleState & (SHORTCASTLE_ALLOWED|LONGCASTLE_ALLOWED))
             ? &PlayerState::evaluateNormalPlay
             : &PlayerState::evaluateNormalPlayWithoutCastleScore;
    }
  }
}

int PlayerState::evaluateNormalPlay() const {
  return m_totalMaterial + m_positionalScore + GameKey::castleScore[*m_castleState];
}

int PlayerState::evaluateNormalPlayWithoutCastleScore() const {
  return m_totalMaterial + m_positionalScore; // dont care about castlescore, its either done or no longer allowed for this player
}

int PlayerState::evaluateEndGame() const {
  return m_totalMaterial + m_positionalScore + 50 * getFreePawnCount();
}

int PlayerState::getFreePawnCount() const {
  return BitSet::setBitsCount[m_pawnMask & ~m_enemyState->m_pawnMask]; // Returns the number of pawns not block by enemypawns
}

static const int kingSearchMiddle[64] = {
  0,1,2,3,3,2,1,0
 ,1,2,3,4,4,3,2,1
 ,2,3,4,5,5,4,3,2
 ,3,4,5,6,6,5,4,3
 ,3,4,5,6,6,5,4,3
 ,2,3,4,5,5,4,3,2
 ,1,2,3,4,4,3,2,1
 ,0,1,2,3,3,2,1,0
};

int PlayerState::evaluateKingSeachMiddle() const {
  return 2*kingSearchMiddle[m_king->m_position] + Game::getWalkDistance(m_king->m_position,m_king->m_enemyState.m_king->m_position);
}

int PlayerState::noEvaluation() const {
  throwException(_T("noEvaluation called. Game is set to fast mode"));
  return 0;
}

void PlayerState::initPawnCount() {
  m_pawnMask       = 0; // Has score = 0 => incrementPawnCount will not decreement positionScore the first time we add a pawn
  m_doublePawnMask = 0;
  memset(m_pawnCount, 0, sizeof(m_pawnCount));
  for(const Piece *p = m_first; p; p = p->m_next) {
    if(p->getType() == Pawn) {
      incrementPawnCount(GETCOL(p->getPosition()));
    }
  }
}

void PlayerStateToPush::incrementPawnCount(int col) {
  switch(++m_pawnCount[col]) {
  case 1 :
    m_positionalScore -= MoveTable::pawnColumnScore[m_pawnMask];
    m_pawnMask       |= (1 << col);
    m_positionalScore += MoveTable::pawnColumnScore[m_pawnMask];
    break;
  default:
    m_doublePawnMask |= (1 << col);
    break;
  }
}

void PlayerStateToPush::decrementPawnCount(int col) {
  switch(--m_pawnCount[col]) {
  case 0 :
    m_positionalScore -= MoveTable::pawnColumnScore[m_pawnMask];
    m_pawnMask       &= ~(1 << col);
    m_positionalScore += MoveTable::pawnColumnScore[m_pawnMask];
    break;
  case 1 :
    m_doublePawnMask &= ~(1 << col);
    break;
  default:
    break;
  }
}
#endif

int PlayerState::getBishopCountOnBoard(FieldColor color) const {
  int count = 0;
  for(int i = 0; i < ARRAYSIZE(m_pieces); i++) {
    const Piece *p = m_pieces[i];
    if(p->getType() == Bishop && p->isOnBoard() && getFieldColor(p->getPosition()) == color) {
      count++;
    }
  }
  return count;
}

int PlayerState::getNonPawnCountOnBoard() const {
  int count = 0;
  for(int i = 0; i < ARRAYSIZE(m_pieces); i++) {
    const Piece *p = m_pieces[i];
    if(p->getType() != Pawn && p->isOnBoard()) {
      count++;
    }
  }
  return count;
}

int PlayerState::getPieceCountOnBoard(PieceType pieceType) const {
  int count = 0;
  for(int i = 0; i < ARRAYSIZE(m_pieces); i++) {
    const Piece *p = m_pieces[i];
    if(p->getType() == pieceType && p->isOnBoard()) {
      count++;
    }
  }
  return count;
}

int PlayerState::getPieceCountOnBoard() const {
  int count = 0;
  for(int i = 0; i < ARRAYSIZE(m_pieces); i++) {
    if(m_pieces[i]->isOnBoard()) {
      count++;
    }
  }
  return count;
}

int PlayerState::getPieceCount(PieceType pieceType) const {
  int count = 0;
  for(int i = 0; i < ARRAYSIZE(m_pieces); i++) {
    const Piece *p = m_pieces[i];
    if(p->getType() == pieceType) {
      count++;
    }
  }
  return count;
}

int PlayerState::getStartPieceCount(PieceType pieceType) { // static
  DEFINEMETHODNAME;
  switch(pieceType) {
  case King :
  case Queen:
    return 1;
  case Rook:
  case Bishop:
  case Knight:
    return 2;
  case Pawn:
    return 8;
  default:
    throwInvalidArgumentException(method, _T("type=%d"), pieceType);
    return 1;
  }
}

// Returns the n'th non-king piece on board. n=[0..15]
const Piece *PlayerState::getNonKingPieceOnBoardByIndex(int n) const {
  int count = 0;
  for(int i = 0; i < ARRAYSIZE(m_pieces); i++) {
    const Piece *p = m_pieces[i];
    if(p->getType() != King && p->isOnBoard()) {
      if(count++ == n) {
        return p;
      }
    }
  }
  return NULL;
}

static int pieceKeyCmp(const PieceKey &k1, const PieceKey &k2) {
  return GET_TYPE_FROMKEY(k1) - GET_TYPE_FROMKEY(k2);
}

void PlayerState::resetCapturedPieceTypes() {
  if(getPieceCount(Pawn) == 8) {
    return;
  }
  for(int i = 0; i < ARRAYSIZE(m_pieces); i++) {
    Piece *p = m_pieces[i];
    if(!p->isOnBoard() && isPromotedPawn(p)) {
      p->setType(Pawn);
    }
  }
}

CompactArray<PieceKey> PlayerState::getCapturedPieces() const {
  CompactArray<PieceKey> result;
  for(int i = 0; i < ARRAYSIZE(m_pieces); i++) {
    const Piece *piece = m_pieces[i];
    if(!piece->isOnBoard()) {
      result.add(piece->getKey());
    }
  }
  if(result.size() > 1) {
    result.sort(pieceKeyCmp);
  }
  return result;
}

PlayerSignature PlayerState::getPlayerSignature() const {
  PlayerSignature result;
  for(const Piece *p = m_first; p; p = p->m_next) {
    if(p->isOnBoard()) {
      result.add(p->getType());
    }
  }
  return result;
}

bool PlayerState::isPromotedPawn(const Piece *piece) const {
  DEFINEMETHODNAME;
//  if(getPieceCount(Pawn) == 8) {
//    return false;
//  }
  switch(piece->getType()) {
  case King:
    return false;

  case Queen :
  case Rook  :
  case Knight:
    return getPieceCount(piece->getType()) > getStartPieceCount(piece->getType());

  case Bishop:
    if(piece->isOnBoard()) {
      return getBishopCountOnBoard(getFieldColor(piece->getPosition())) > 1;
    } else {
      return getPieceCount(Bishop) > 2;
    }
  case Pawn:
    return false;

  default    :
    throwInvalidArgumentException(method, _T("piece.type=%d"), piece->getType());
    return false;
  }
}

int PlayerState::getPromotionCount() const {
  return max(getPieceCountOnBoard(Queen)    - getStartPieceCount(Queen ),0)
       + max(getPieceCountOnBoard(Rook)     - getStartPieceCount(Rook  ),0)
       + max(getPieceCountOnBoard(Knight)   - getStartPieceCount(Knight),0)
       + max(getBishopCountOnBoard(WHITEFIELD) - 1,0)
       + max(getBishopCountOnBoard(BLACKFIELD) - 1,0);
}

void PlayerState::validatePromotionCount() const {
  const int promotionCount = getPromotionCount();
  const int pawnsOnBoard   = getPieceCountOnBoard(Pawn);
  if(promotionCount + pawnsOnBoard > 8) {
    const String pmcStr = loadString(promotionCount>1?IDS_PROMOTED_PAWN_PLUR:IDS_PROMOTED_PAWN); // PromotionCount
    const String pobStr = getPieceTypeName(Pawn,pawnsOnBoard>1);                                 // PawnOnBoard
    throwUserException(IDS_MSG_s_HAVE_d_s_AND_d_s_ON_THE_BOARD_s
                      ,getPlayerName(m_player)
                      ,promotionCount
                      ,pmcStr.cstr()
                      ,pawnsOnBoard
                      ,toLowerCase(pobStr).cstr()
                      ,loadString(IDS_IMPOSSIBLE).cstr());
  }
}

void PlayerState::validateAddPieceAtPosition(PieceType pieceType, int pos, bool validatePromotions) const {
  DEFINEMETHODNAME;
  validatePosition(method, pos);

  switch(pieceType) {
  case King  :
    { if(getPieceCountOnBoard(King) > 0) {
        throwUserException(IDS_MSG_s_CANNOT_HAVE_TWO_KINGS, getPlayerName(m_player));
      }
      const Piece *enemyKing = m_king->m_enemyState.m_king;
      if(enemyKing && enemyKing->isOnBoard() && POSADJACENT(pos, enemyKing->getPosition())) {
        throwUserException(IDS_MSG_KINGS_ATTACK_EACHOTHER);
      }
    }
    break;

  case Queen :
  case Rook  :
  case Bishop:
  case Knight:
    if((getPieceCountOnBoard(pieceType) < getStartPieceCount(pieceType)) || (getPieceCountOnBoard(Pawn) + getPromotionCount() < 8)) {
      break;
    }
    throwUserException(IDS_MSG_CANNOT_ADD_ANOTHER_s_s, toLowerCase(getPlayerName(m_player)).cstr(), toLowerCase(getPieceTypeName(pieceType)).cstr());
    break;

  case Pawn  :
    if(!isValidPawnPosition(pos)) {
      throwUserException(IDS_MSG_CANNOT_PLACE_A_PAWN_ON_s, getFieldName(pos));
    }
    if(validatePromotions) {
      if(getPieceCountOnBoard(Pawn) + getPromotionCount() >= 8) {
        throwUserException(IDS_MSG_CANNOT_ADD_ANOTHER_s_s, toLowerCase(getPlayerName(m_player)).cstr(), toLowerCase(getPieceTypeName(Pawn)).cstr());
      }
    } else if(getPieceCountOnBoard(Pawn) >= 8) {
      throwUserException(IDS_MSG_CANNOT_ADD_ANOTHER_s_s, toLowerCase(getPlayerName(m_player)).cstr(), toLowerCase(getPieceTypeName(Pawn)).cstr());
    }
    break;

  default    :
    throwInvalidArgumentException(method, _T("pieceType=%d"), pieceType);
  }
}

#ifndef TABLEBASE_BUILDER
String PlayerStateToPush::pawnCountToString() const {
  String tmp;
  for(int i = 0; i < ARRAYSIZE(m_pawnCount); i++) {
    tmp += format(_T("%d"), m_pawnCount[i]);
  }
  return format(_T("[%s],%s,%s:PawnScore:%+3d"), tmp.cstr(), rev(sprintbin(m_pawnMask)).cstr(), rev(sprintbin(m_doublePawnMask)).cstr(), MoveTable::pawnColumnScore[m_pawnMask]);
}

String PlayerStateToPush::bishopFlagsToString() const {
  return format(_T("[%s]"), getBishopFlagsToString(m_bishopFlags).cstr());
}
#endif

static const unsigned char rcAttackFlags[16] = {    // Indexed by m_attackDirectionInfo2.m_rcAttacked, which max value is 15.
  0                                                 // See comment to struct AttackInfo in game.h
 ,KING_LD_ATTACKED_FROM_ROW
 ,KING_LD_ATTACKED_FROM_ROW
 ,KING_MANY_ATTACKS                                 // Occurs when attacked from too many directions, which is impossible in normal play
 ,KING_LD_ATTACKED_FROM_COL
 ,KING_LD_ATTACKED_FROM_ROW | KING_LD_ATTACKED_FROM_COL
 ,KING_LD_ATTACKED_FROM_ROW | KING_LD_ATTACKED_FROM_COL
 ,KING_MANY_ATTACKS
 ,KING_LD_ATTACKED_FROM_COL
 ,KING_LD_ATTACKED_FROM_ROW | KING_LD_ATTACKED_FROM_COL
 ,KING_LD_ATTACKED_FROM_ROW | KING_LD_ATTACKED_FROM_COL
 ,KING_MANY_ATTACKS
 ,KING_MANY_ATTACKS
 ,KING_MANY_ATTACKS
 ,KING_MANY_ATTACKS
 ,KING_MANY_ATTACKS
};

static const unsigned char diagAttackFlags[16] = {  // Indexed by m_attackDirectionInfo2.m_diagAttacked, which max value is 15.
  0                                                 // See comment to struct AttackInfo in game.h
 ,KING_LD_ATTACKED_FROM_DIAG1
 ,KING_LD_ATTACKED_FROM_DIAG1
 ,KING_MANY_ATTACKS                                 // Occurs when attacked from too many directions, which is impossible in normal play
 ,KING_LD_ATTACKED_FROM_DIAG2
 ,KING_LD_ATTACKED_FROM_DIAG1 | KING_LD_ATTACKED_FROM_DIAG2
 ,KING_LD_ATTACKED_FROM_DIAG1 | KING_LD_ATTACKED_FROM_DIAG2
 ,KING_MANY_ATTACKS
 ,KING_LD_ATTACKED_FROM_DIAG2
 ,KING_LD_ATTACKED_FROM_DIAG1 | KING_LD_ATTACKED_FROM_DIAG2
 ,KING_LD_ATTACKED_FROM_DIAG1 | KING_LD_ATTACKED_FROM_DIAG2
 ,KING_MANY_ATTACKS
 ,KING_MANY_ATTACKS
 ,KING_MANY_ATTACKS
 ,KING_MANY_ATTACKS
 ,KING_MANY_ATTACKS
};

static const unsigned char sdAttackFlags[] = {      // Indexed by m_attackDirectionInfo.m_sdAttacked, which max value is 11.
  0                                                 // See comment to struct AttackInfo in game.h
 ,KING_SD_ATTACKED
 ,KING_MANY_ATTACKS                                 // Occurs when attacked by more than one short distance attacking piece (King, Knight or Pawn)
 ,KING_MANY_ATTACKS                                 // For the king this is impossible in normal play
 ,KING_MANY_ATTACKS
 ,KING_MANY_ATTACKS
 ,KING_MANY_ATTACKS
 ,KING_MANY_ATTACKS
 ,KING_MANY_ATTACKS
 ,KING_MANY_ATTACKS
 ,KING_MANY_ATTACKS
 ,KING_MANY_ATTACKS
};

void PlayerState::setKingAttackState() {
  const FieldAttacks attacks = m_king->m_enemyState.m_attackTable[m_king->m_position];
  if(!attacks.m_isAttacked) {
    m_kingAttackState = KING_NOT_ATTACKED;
  } else {
    m_kingAttackState = rcAttackFlags[  attacks.m_attackDirectionInfo2.m_rcAttacked  ]
                      | diagAttackFlags[attacks.m_attackDirectionInfo2.m_diagAttacked]
                      | sdAttackFlags[  attacks.m_attackDirectionInfo2.m_sdAttacked  ];
  }
}

#ifdef TABLEBASE_BUILDER
bool PlayerState::setKingAttackStateBackMove() {
  const FieldAttacks attacks = m_king->m_enemyState.m_attackTable[m_king->m_position];
  if(!attacks.m_isAttacked) {
    m_kingAttackState = KING_NOT_ATTACKED;
    return false;
  } else {
    m_kingAttackState = rcAttackFlags[  attacks.m_attackDirectionInfo2.m_rcAttacked  ]
                      | diagAttackFlags[attacks.m_attackDirectionInfo2.m_diagAttacked]
                      | sdAttackFlags[  attacks.m_attackDirectionInfo2.m_sdAttacked  ];
    return (m_kingAttackState & KING_SD_ATTACKED) ? true : false;
  }
}
#endif

bool PlayerState::hasOpposition() const {
  const int kingPos1 = m_king->m_position;
  const int kingPos2 = m_king->m_enemyState.m_king->m_position;
  return Game::getWalkDistance(kingPos1,kingPos2) == 2
      && (GETROW(kingPos1) == GETROW(kingPos2) || GETCOL(kingPos1) == GETCOL(kingPos2));
}

