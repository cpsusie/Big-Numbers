#include "stdafx.h"

#ifdef TABLEBASE_BUILDER

class HelperFieldSets {
private:
  const FieldSet  m_rank1, m_rank8, m_fileA, m_fileH;
  bool            m_initDone;
public:
  FieldSet        m_knightAttacks[64], m_pawnAttacks[2][64];
  const FieldSet *m_promoteFieldSet;
  int             m_kingSourceField;
  HelperFieldSets();
  void init();
  void setPromoteFieldSet(Player kingOwner, SymmetricTransformation transformation);
};

HelperFieldSets::HelperFieldSets()
  : m_rank1(A1,B1,C1,D1,E1,F1,G1,H1,-1)
  , m_rank8(A8,B8,C8,D8,E8,F8,G8,H8,-1)
  , m_fileA(A1,A2,A3,A4,A5,A6,A7,A8,-1)
  , m_fileH(H1,H2,H3,H4,H5,H6,H7,H8,-1)
  , m_initDone(false)
{
}

void HelperFieldSets::init() {
  if(m_initDone) return;
  for(int pos = 0; pos < 64; pos++) {
    const FieldInfo &info = Game::s_fieldInfo[pos];
    PositionArray pa = info.m_knightAttacks;
    FieldSet &knightAttacks = m_knightAttacks[pos];
    for(int count = *(pa++); count--;) knightAttacks.add(*(pa++));
    pa = info.m_whitePawnAttacks;
    if(pa) {
      FieldSet &wpAttacks = m_pawnAttacks[WHITEPLAYER][pos];
      for(int count = *(pa++); count--;) wpAttacks.add(*(pa++));
    }
    pa = info.m_blackPawnAttacks;
    if(pa) {
      FieldSet &bpAttacks = m_pawnAttacks[BLACKPLAYER][pos];
      for(int count = *(pa++); count--;) bpAttacks.add(*(pa++));
    }
  }
  m_initDone = true;
}

void HelperFieldSets::setPromoteFieldSet(Player kingOwner, SymmetricTransformation transformation) {
  switch(transformation) {
  case 0                     :
  case TRANSFORM_MIRRORCOL   : m_promoteFieldSet = (kingOwner==WHITEPLAYER)?&m_rank1:&m_rank8; break;
  case TRANSFORM_ROTATE180   :
  case TRANSFORM_MIRRORROW   : m_promoteFieldSet = (kingOwner==WHITEPLAYER)?&m_rank8:&m_rank1; break;
  case TRANSFORM_MIRRORDIAG2 :
  case TRANSFORM_ROTATERIGHT : m_promoteFieldSet = (kingOwner==WHITEPLAYER)?&m_fileA:&m_fileH; break;
  case TRANSFORM_MIRRORDIAG1 :
  case TRANSFORM_ROTATELEFT  : m_promoteFieldSet = (kingOwner==WHITEPLAYER)?&m_fileH:&m_fileA; break;
  default                    : throwException(_T("%s:Unexpected transformation:%s")
                                             ,__TFUNCTION__
                                             , getSymmetricTransformationToString(transformation).cstr());
  }
}

class BackMoveGenerator : public MoveGenerator {
private:
  static HelperFieldSets s_hfs;

  void resetPromoteFieldSet(int kingFrom) const;
  void findPromoteFieldSet() const;
  bool isPossibleRowColCheck(  const FieldAttacks &attInfo, int pos) const;
  bool isNonCapturingPromotion(int promotedPos,                   const FieldSet &sourceFieldSet) const;
  bool isCapturingPromotion(   int promotedPos, int uncoveredPos, const FieldSet &sourceFieldSet) const;
  bool isPromotePosition(int pos) const;
protected:
  BackMoveGenerator(const Game &game) : MoveGenerator(game) {
    s_hfs.init();
  }

  static void invalidPawnPosition(bool firstMove, const Piece *piece);
  bool checksEnemyKing(int kingPos, const Piece *piece, int from, MoveDirection direction) const;
  bool isPossibleCheck(             const Piece *king , int pos) const;
};

class BackMoveGeneratorNoCheck : public BackMoveGenerator {
public:
  BackMoveGeneratorNoCheck(const Game &game) : BackMoveGenerator(game) {
  }
  bool firstMove(Move &m) const;
  bool nextMove( Move &m) const;
};

class BackMoveGeneratorLDCheck : public BackMoveGenerator {
private:
  Piece *findCheckingLDAPiece(int kingPos) const;
public:
  BackMoveGeneratorLDCheck(const Game &game) : BackMoveGenerator(game) {
  }
  bool firstMove(Move &m) const;
  bool nextMove( Move &m) const;
};

class BackMoveGeneratorSDCheck : public BackMoveGenerator {
public:
  BackMoveGeneratorSDCheck(const Game &game) : BackMoveGenerator(game) {
  }
  bool firstMove(Move &m) const;
  bool nextMove( Move &m) const;
};

class BackMoveGeneratorLDDoubleCheck : public BackMoveGenerator {
private:
  Piece *findMovedPiece(int &sourceFields, MoveDirection &direction) const;
public:
  BackMoveGeneratorLDDoubleCheck(const Game &game) : BackMoveGenerator(game) {
  }
  bool firstMove(Move &m) const;
  bool nextMove( Move &m) const {
    return false; // There is only one possible move which could lead to this double-check
  }
};

class BackMoveGeneratorSDDoubleCheck : public BackMoveGenerator {
private:
  FieldSet findPossibleSourceFields(int kingPos) const;
public:
  BackMoveGeneratorSDDoubleCheck(const Game &game) : BackMoveGenerator(game) {
  }
  bool firstMove(Move &m) const;
  bool nextMove( Move &m) const {
    return false; // There is only one possible move which could lead to this double-check
  }
};

class BackMoveGeneratorNoMoves : public BackMoveGenerator {
private:
public:
  BackMoveGeneratorNoMoves(const Game &game) : BackMoveGenerator(game) {
  }
  bool firstMove(Move &m) const {
    return false;
  }
  bool nextMove( Move &m) const {
    return false;
  }
};

#define BACKMOVEGEN_NO_CHECK        0
#define BACKMOVEGEN_LD_CHECK        1
#define BACKMOVEGEN_SD_CHECK        2
#define BACKMOVEGEN_LD_DOUBLE_CHECK 3
#define BACKMOVEGEN_SD_DOUBLE_CHECK 4
#define BACKMOVEGEN_NOMOVES         5

void Game::allocateBackMoveGenerators() {
  m_backMoveGenerator[BACKMOVEGEN_NO_CHECK       ] = new BackMoveGeneratorNoCheck(      *this); TRACE_NEW(m_backMoveGenerator[BACKMOVEGEN_NO_CHECK       ]);
  m_backMoveGenerator[BACKMOVEGEN_LD_CHECK       ] = new BackMoveGeneratorLDCheck(      *this); TRACE_NEW(m_backMoveGenerator[BACKMOVEGEN_LD_CHECK       ]);
  m_backMoveGenerator[BACKMOVEGEN_SD_CHECK       ] = new BackMoveGeneratorSDCheck(      *this); TRACE_NEW(m_backMoveGenerator[BACKMOVEGEN_SD_CHECK       ]);
  m_backMoveGenerator[BACKMOVEGEN_LD_DOUBLE_CHECK] = new BackMoveGeneratorLDDoubleCheck(*this); TRACE_NEW(m_backMoveGenerator[BACKMOVEGEN_LD_DOUBLE_CHECK]);
  m_backMoveGenerator[BACKMOVEGEN_SD_DOUBLE_CHECK] = new BackMoveGeneratorSDDoubleCheck(*this); TRACE_NEW(m_backMoveGenerator[BACKMOVEGEN_SD_DOUBLE_CHECK]);
  m_backMoveGenerator[BACKMOVEGEN_NOMOVES        ] = new BackMoveGeneratorNoMoves(      *this); TRACE_NEW(m_backMoveGenerator[BACKMOVEGEN_NOMOVES        ]);
}

void Game::deallocateBackMoveGenerators() {
  for(int i = 0; i < ARRAYSIZE(m_backMoveGenerator); i++) {
    SAFEDELETE(m_backMoveGenerator[i]);
  }
}

MoveGenerator &Game::getBackMoveGenerator() const {
  switch(m_playerState[PLAYERINTURN].m_kingAttackState) {
  case KING_NOT_ATTACKED:
    return *m_backMoveGenerator[BACKMOVEGEN_NO_CHECK];

  case KING_LD_ATTACKED_FROM_ROW  :
  case KING_LD_ATTACKED_FROM_COL  :
  case KING_LD_ATTACKED_FROM_DIAG1:
  case KING_LD_ATTACKED_FROM_DIAG2:
    return *m_backMoveGenerator[BACKMOVEGEN_LD_CHECK];

  case KING_SD_ATTACKED           :
    return *m_backMoveGenerator[BACKMOVEGEN_SD_CHECK];

  case KING_LD_ATTACKED_FROM_ROW   | KING_LD_ATTACKED_FROM_DIAG1:
  case KING_LD_ATTACKED_FROM_ROW   | KING_LD_ATTACKED_FROM_DIAG2:
  case KING_LD_ATTACKED_FROM_COL   | KING_LD_ATTACKED_FROM_DIAG1:
  case KING_LD_ATTACKED_FROM_COL   | KING_LD_ATTACKED_FROM_DIAG2:
    return *m_backMoveGenerator[BACKMOVEGEN_LD_DOUBLE_CHECK];

  case KING_LD_ATTACKED_FROM_ROW   | KING_SD_ATTACKED:
  case KING_LD_ATTACKED_FROM_COL   | KING_SD_ATTACKED:
  case KING_LD_ATTACKED_FROM_DIAG1 | KING_SD_ATTACKED:
  case KING_LD_ATTACKED_FROM_DIAG2 | KING_SD_ATTACKED:
    return *m_backMoveGenerator[BACKMOVEGEN_SD_DOUBLE_CHECK];

  default:
    return *m_backMoveGenerator[BACKMOVEGEN_NOMOVES];
/*
  default:
    throwException(_T("getBackMoveGenerator:%s king attackstate=%s (=%#02x). No previous move is possible")
                  ,getPlayerNameEnglish(PLAYERINTURN)
                  ,getKingAttackStateToString(m_playerState[PLAYERINTURN].m_kingAttackState).cstr()
                  ,m_playerState[PLAYERINTURN].m_kingAttackState);
    return *m_backMoveGenerator[0];
*/
  }
}

void Game::setEndGameKeyDefinition() {
  const EndGameTablebase *t = EndGameTablebase::getInstanceBySignature(getPositionSignature(), m_swapPlayers);
  m_keydef = t ? &t->getKeyDefinition() : NULL;
}

void Game::setEndGameKeyDefinition(const EndGameKeyDefinition &keydef) {
  m_keydef      = &keydef;
  m_swapPlayers = false;
  initState();
}

#undef PLAYERINTURN
#define PLAYERINTURN m_game.m_gameKey.getPlayerInTurn()

HelperFieldSets BackMoveGenerator::s_hfs;

static const PinnedState kingDirToPinnedState[9] = {
  NOT_PINNED
 ,PINNED_TO_ROW
 ,PINNED_TO_ROW
 ,PINNED_TO_COL
 ,PINNED_TO_COL
 ,PINNED_TO_DIAG1
 ,PINNED_TO_DIAG1
 ,PINNED_TO_DIAG2
 ,PINNED_TO_DIAG2
};

bool FieldAttacks::isLDAttackedFromDir(MoveDirection direction) const {
  switch(direction) {
  case MD_NONE     : return false;
  case MD_LEFT     : return m_attackInfo.m_fromLeft;
  case MD_RIGHT    : return m_attackInfo.m_fromRight;
  case MD_DOWN     : return m_attackInfo.m_fromBelove;
  case MD_UP       : return m_attackInfo.m_fromAbove;
  case MD_DOWNDIAG1: return m_attackInfo.m_fromLowerDiag1;
  case MD_UPDIAG1  : return m_attackInfo.m_fromUpperDiag1;
  case MD_DOWNDIAG2: return m_attackInfo.m_fromLowerDiag2;
  case MD_UPDIAG2  : return m_attackInfo.m_fromUpperDiag2;
  default          : throwInvalidArgumentException(__TFUNCTION__, _T("direction=%d"), direction);
                     return false;
  }
}

bool BackMoveGenerator::checksEnemyKing(int kingPos, const Piece *piece, int from, MoveDirection direction) const {
  const MoveDirection kingDir = KING_DIRECTION(piece->m_enemyState, piece->m_position);
  if(kingDir != MD_NONE) {
    if(piece->m_playerState.m_attackTable[piece->m_position].isLDAttackedFromDir(getOppositeDirection(kingDir))
      && MoveTable::uncoversKing[kingDirToPinnedState[kingDir]][direction]) {
      return true;
    }
    if(((piece->m_attackAttribute & ATTACKS_LONGDISTANCE) != 0)
      && (kingDir == direction)) { // LDA-piece cannot come from a position moving directly against the king
                                   // as it would already attack the king from that position
      return true;
    }
  }
  switch(piece->getType()) {
  case King  :
    return POSADJACENT(kingPos, from);

  case Queen :
    switch(KING_DIRECTION(piece->m_enemyState, from)) {
    case MD_NONE:
      return false;
    default     :
      return true;
    }

  case Rook  :
    switch(KING_DIRECTION(piece->m_enemyState, from)) {
    case MD_NONE     :
    case MD_DOWNDIAG1:
    case MD_UPDIAG1  :
    case MD_DOWNDIAG2:
    case MD_UPDIAG2  :
      return false;
    default:
      return true;
    }

  case Bishop:
    switch(KING_DIRECTION(piece->m_enemyState, from)) {
    case MD_NONE :
    case MD_LEFT :
    case MD_RIGHT:
    case MD_DOWN :
    case MD_UP   :
      return false;
    default:
      return true;
    }

  case Knight:
    return s_hfs.m_knightAttacks[from].contains(kingPos);

  case Pawn  :
    return s_hfs.m_pawnAttacks[piece->getPlayer()][from].contains(kingPos);

  default    :
    return false;
  }
}

bool BackMoveGenerator::isPossibleCheck(const Piece *king, int pos) const {
  const FieldAttacks &attInfo = king->m_enemyState.m_attackTable[pos];
  if(!attInfo.m_isAttacked) {
    return true;
  }

  if((attInfo.m_attackDirectionInfo1.m_rowAttacked   == 3)
  || (attInfo.m_attackDirectionInfo1.m_colAttacked   == 3)
  || (attInfo.m_attackDirectionInfo1.m_diag1Attacked == 3)
  || (attInfo.m_attackDirectionInfo1.m_diag2Attacked == 3)
  || (attInfo.m_attackDirectionInfo1.m_diag1Attacked && attInfo.m_attackDirectionInfo1.m_diag2Attacked)) {
    return false;
  }
  resetPromoteFieldSet(pos);
  if(attInfo.m_attackDirectionInfo1.m_rowAttacked && attInfo.m_attackDirectionInfo1.m_colAttacked) {
    if(attInfo.m_attackInfo.m_sdAttacks) {
      return false;
    }
    return isPossibleRowColCheck(attInfo, pos);
  }
  switch(attInfo.m_attackInfo.m_sdAttacks) {
  case 0:
    break;

  case 1:
    { const Player enemy = king->getEnemy();
      const int pawnPos = m_game.findAttackingPawnPosition(enemy, pos);
      if(pawnPos >= 0) {
        if(GETROW(pawnPos) == GETPAWNSTARTROW(enemy)) {
          return false; // Check by an unmoved pawn: Impossible
        }
        if(!attInfo.m_attackTypeInfo.m_ldAttacked) {
          return true;
        } else { // Check by a pawn + LD-attack. Impossible unless pawn just captured.
          if(attInfo.m_attackDirectionInfo2.m_diagAttacked || attInfo.m_attackDirectionInfo1.m_rowAttacked) {
            return false;
          } else { // colAttacked. Must be from a rook or queen behind the pawns original position, ie. distance > 2
            switch(enemy) {
            case WHITEPLAYER:
              { const Piece *ldaPiece = m_game.findFirstPieceInDirection(pos, MD_DOWN);
                return ldaPiece && LONGDISTANCE_ATTACKS(WHITEPLAYER, ldaPiece, ATTACKS_PARALLEL)
                    && (Game::getWalkDistance(pos, ldaPiece->m_position) > 2);
              }
            case BLACKPLAYER:
              { const Piece *ldaPiece = m_game.findFirstPieceInDirection(pos, MD_UP);
                return ldaPiece && LONGDISTANCE_ATTACKS(BLACKPLAYER, ldaPiece, ATTACKS_PARALLEL)
                    && (Game::getWalkDistance(pos, ldaPiece->m_position) > 2);
              }
            default:INVALIDPLAYERERROR(enemy);
                    return false;
            }
          }
        }
      } else { // Must be a knight
        const bool rcAttack   = attInfo.isRCAttacked();
        const bool diagAttack = attInfo.isDiagAttacked();
        if(rcAttack) {
          if(diagAttack) {
            return false;       // Check by a knight + 2 LD-attacking pieces: Triple-check:Impossible!
          } else {              // double-check by a knight + (queen or rook)
            const int       knightPos  = m_game.findAttackingKnightPosition(enemy, pos);
            const int       rcPiecePos = m_game.findLDAttackingPiece(enemy, pos, false)->m_position;
            const FieldSet &fieldsBetween = getFieldsBetween(pos, rcPiecePos);
            return !(s_hfs.m_knightAttacks[knightPos] * fieldsBetween).isEmpty()
                 || isNonCapturingPromotion(knightPos,      fieldsBetween)
                 || isCapturingPromotion(   knightPos, pos, fieldsBetween);
          }
        } else if(diagAttack) { // && !rcAttack => double-check by a knight + (queen or bishop)
          const int       knightPos     = m_game.findAttackingKnightPosition(enemy, pos);
          const FieldSet &fieldsBetween = getFieldsBetween(pos, m_game.findLDAttackingPiece(enemy, pos, true)->m_position);
          return !(s_hfs.m_knightAttacks[knightPos] * fieldsBetween).isEmpty()
               || isNonCapturingPromotion(knightPos, fieldsBetween);
        }
      }
    }
    break;

  default:
    return false; // more than 1 SD-attack:Impossible
  }
  if(attInfo.isRCAttacked() && attInfo.isDiagAttacked()) { // Check if legal doublecheck
    const Player enemy     = king->getEnemy();
    const Piece *rcPiece   = m_game.findLDAttackingPiece(enemy, pos, false);
    const Piece *diagPiece = m_game.findLDAttackingPiece(enemy, pos, true );
    const int    rcDist    = Game::getWalkDistance(pos, rcPiece->m_position  );
    const int    diagDist  = Game::getWalkDistance(pos, diagPiece->m_position);

    if(rcDist > diagDist) {                // Moved piece must be the diagonal-moving piece, ie a bishop
      if(diagPiece->getType() != Bishop) { // a queen will do, if its a newly promoted pawn, moving forward, not capturing.
        assert(diagPiece->getType() == Queen);
        return isNonCapturingPromotion(diagPiece->m_position, getFieldsBetween(pos, rcPiece->m_position));
      }
      const FieldSet &fieldsBetween = getFieldsBetween(pos, rcPiece->m_position);
      return m_game.pieceCanGotoAnyPosition(diagPiece, fieldsBetween) || isNonCapturingPromotion(diagPiece->m_position, fieldsBetween);
    } else if(rcDist < diagDist) {         // Moved piece must be the parallel-moving piece, ie. a rook
      if(rcPiece->getType() != Rook) {
        return false;                      // No need to check for P->Q-promotions. A queen can't do it
      }
      return m_game.pieceCanGotoAnyPosition(rcPiece, getFieldsBetween(pos, diagPiece->m_position));
    } else { // LDA-pieces have the same distance to pos. Impossible, unless a pawn just promoted to (Q or R) capturing a piece
             // uncovering (Q or B) on the same edge as rcPiece, ie diagpiece must be on an edgefield
      return (rcDist == 2) && isPromotePosition(diagPiece->m_position) && isCapturingPromotion(rcPiece->m_position, diagPiece->m_position, getFieldsBetween(pos, diagPiece->m_position));
    }
  }
  return true;
}

bool BackMoveGenerator::isNonCapturingPromotion(int promotedPos, const FieldSet &sourceFieldSet) const {
  if(IS_CORNERFIELD(promotedPos) || !isPromotePosition(promotedPos)) {
    // All edgefields (except corners) are promotion-fields, because of rotations and reflections in diagonals.
    // The corners will never do, because the fields where the pawn would come from (b1,g1,a2,h2,a7,h7,b8,g8)
    // are all edgefields, which are never included in sourceFieldSet (=fields between king and the uncovered piece)
    return false;
  }
  const int r = GETROW(promotedPos);
  const int c = GETCOL(promotedPos);
  int srcField;
  if(r == 0) {
    srcField = MAKE_POSITION(r+1,c);
  } else if(r == 7) {
    srcField = MAKE_POSITION(r-1,c);
  } else if(c == 0) {
    srcField = MAKE_POSITION(r,c+1);
  } else if(c == 7) {
    srcField = MAKE_POSITION(r,c-1);
  } else {
    throwException(_T("%s:promotedPos %s is an edgefield, (r,c)=(%d,%d)")
                  ,__TFUNCTION__
                  ,getFieldName(promotedPos), r, c);
    srcField = 0; // just to make compiler happy
  }
  return sourceFieldSet.contains(srcField);
}

#ifdef _DEBUG

#define PRINTF(...) verbose(__VA_ARGS__)
#define CLEARTRACE() clearVerbose()

#define CHECK_NOT_CORNER(r, c)                                                                                           \
{ if(!INSIDEBOARD(r, c) || IS_CORNERFIELD(MAKE_POSITION(r, c))) {                                                        \
    _tprintf(_T("line %d:Invalid (r,c):(%d,%d) sourceFieldSet:<%s>"), __LINE__, r, c, sourceFieldSet.toString().cstr()); \
    pause();                                                                                                             \
  }                                                                                                                      \
}

#else

#define PRINTF(...)
#define CLEARTRACE()
#define CHECK_NOT_CORNER(r, c)
#endif

bool BackMoveGenerator::isCapturingPromotion(int promotedPos, int uncoveredPos, const FieldSet &sourceFieldSet) const {
  if(!isPromotePosition(promotedPos)) { // See comment in isNonCapturingPromotion
                                        // Corners allowed, because b2,g2,b7,g7 could be legal sourceFields uncovering pos
    return false;
  }
  int r = GETROW(promotedPos);
  int c = GETCOL(promotedPos);
  int srcField;
  if(r == 0) {
    if(GETCOL(uncoveredPos) < c) c--; else c++;
    CHECK_NOT_CORNER(r+1,c);
    srcField = MAKE_POSITION(r+1,c);
  } else if(r == 7) {
    if(GETCOL(uncoveredPos) < c) c--; else c++;
    CHECK_NOT_CORNER(r-1,c);
    srcField = MAKE_POSITION(r-1,c);
  } else if(c == 0) {
    if(GETROW(uncoveredPos) < r) r--; else r++;
    CHECK_NOT_CORNER(r,c+1);
    srcField = MAKE_POSITION(r,c+1);
  } else if(c == 7) {
    if(GETROW(uncoveredPos) < r) r--; else r++;
    CHECK_NOT_CORNER(r,c-1);
    srcField = MAKE_POSITION(r,c-1);
  } else {
    throwException(_T("%s:promotedPos %s is an edgefield, (r,c)=(%d,%d)")
                  ,__TFUNCTION__
                  ,getFieldName(promotedPos), r, c);
    srcField = 0; // just to make compiler happy
  }

  return sourceFieldSet.contains(srcField);
}

bool BackMoveGenerator::isPossibleRowColCheck(const FieldAttacks &attInfo, int pos) const {
  const int rowPos = m_game.findFirstPieceInDirection(pos, attInfo.m_attackInfo.m_fromLeft  ?MD_LEFT:MD_RIGHT)->getPosition();
  const int colPos = m_game.findFirstPieceInDirection(pos, attInfo.m_attackInfo.m_fromBelove?MD_DOWN:MD_UP   )->getPosition();
  const int rowDistance = Game::getWalkDistance(pos, rowPos);
  const int colDistance = Game::getWalkDistance(pos, colPos);
  if(IS_INNERFIELD(pos)) { // One of the attacking (Q or R) must be a newly promoted pawn capturing and uncovering the other (Q or R)
    if(rowDistance == 1) {
      return (colDistance > 1) && isPromotePosition(rowPos);
    } else if(colDistance == 1) {
      return (rowDistance > 1) && isPromotePosition(colPos);
    } else {
      return false;
    }
  } else { // pos is on the edge
    if(IS_CORNERFIELD(pos)) {
      if(rowDistance == 1) {
        return (colDistance > 1) && isPromotePosition(rowPos);
      } else if(colDistance == 1) {
        return (rowDistance > 1) && isPromotePosition(colPos);
      } else {
        return false;
      }
    } else if(IS_INNERROW(pos)) { // colPos must be a promotion
      return (rowDistance > 1) && (colDistance == 1) && isPromotePosition(colPos);
    } else { // IS_INNERCOL => rowPos must be a promotion
      return (rowDistance == 1) && (colDistance > 1) && isPromotePosition(rowPos);
    }
  }
}

#define LASTROW ((PLAYERINTURN == BLACKPLAYER) ? 0 : 7)

bool BackMoveGenerator::isPromotePosition(int pos) const {
  if(IS_INNERFIELD(pos)) {
    return false;
  }
  PRINTF(_T("Check if %s is promoteField\n"), getFieldName(pos));
  bool result;
  if(m_game.m_keydef == NULL) { // No symmetric transformation or swapping of players
    result = GETROW(pos) == LASTROW;
  } else {
    if(s_hfs.m_promoteFieldSet == NULL) { // lazy evaluering
      findPromoteFieldSet();
    }
    __assume(s_hfs.m_promoteFieldSet);
    result = s_hfs.m_promoteFieldSet->contains(pos);
  }
  PRINTF(_T("result=%s\n"), boolToStr(result));
  return result;
}

void BackMoveGenerator::resetPromoteFieldSet(int kingFrom) const {
  s_hfs.m_promoteFieldSet = NULL;
  s_hfs.m_kingSourceField = kingFrom;
}

void BackMoveGenerator::findPromoteFieldSet() const {
  GameKey key = m_game.getKey();
  const EndGameKeyDefinition &keydef = *m_game.m_keydef;
//  String origKeyStr = key.toString();
  EndGameKey egk;
  Player kingOwner = PLAYERINTURN;
  if(m_game.m_swapPlayers) {
    key = key.transform(TRANSFORM_SWAPPLAYERS);
    egk = keydef.getEndGameKey(key);
  } else {
    kingOwner = GETENEMY(kingOwner);
    egk = keydef.getEndGameKey(key);
  }
  if(kingOwner == WHITEPLAYER) {
    egk.setWhiteKingPosition(s_hfs.m_kingSourceField);
  } else {
    egk.setBlackKingPosition(s_hfs.m_kingSourceField);
  }
  const SymmetricTransformation transformation = keydef.getSymTransformation(egk);
  s_hfs.setPromoteFieldSet(kingOwner, transformation);
  PRINTF(_T("Trans:%s\nKingowner:%s\nKing sourcefield:%s\nKey.tr:%s\nPromoteFields:%s\n")
        , getSymmetricTransformationToString(transformation).cstr()
        , getPlayerNameEnglish(kingOwner)
        , getFieldName(s_hfs.m_kingSourceField)
        , keydef.getTransformedKey(egk, transformation).toString(keydef).cstr()
        , s_hfs.m_promoteFieldSet->toString().cstr());
}

void BackMoveGenerator::invalidPawnPosition(bool firstMove, const Piece *piece) { // static
  throwException(_T("BackMoveGenerator::%sMove:Invalid Pawnposition:%s"), firstMove?_T("first"):_T("next"), getFieldName(piece->m_position));
}

#define SETBACKMOVE(move, piece, dirIndex, moveIndex, from, direction)                            \
{ move.m_type          = NORMALMOVE;                                                              \
  move.m_piece         = piece;                                                                   \
  move.m_capturedPiece = NULL;                                                                    \
  move.m_dirIndex      = dirIndex;                                                                \
  move.m_moveIndex     = moveIndex;                                                               \
  move.m_from          = from;                                                                    \
  move.m_to            = piece->m_position;                                                       \
  move.m_direction     = direction;                                                               \
  move.m_promoteIndex  = 0;                                                                       \
}

// --------------------------------------------- BackMoveGeneratorNoCheck ------------------------------------------------
// Used to generate backward moves, when king is not in check, Used when generating endgame tablebases.
// Castle-, promotion- and capture-moves are not generated,
bool BackMoveGeneratorNoCheck::firstMove(Move &m) const {
  CLEARTRACE();

  const int kingPos = m_game.m_playerState[PLAYERINTURN].m_king->m_position;

  for(Piece *piece = m_game.m_playerState[CURRENTENEMY].m_first; piece; piece = piece->m_next) {
    if(!piece->m_onBoard) {
      continue;
    }
    switch(piece->getType()) {
    case King:
      { const DirectionArray &da          = MoveTable::kingMoves[piece->m_position];
        const Direction      *direction   = da.m_directions;

        for(int dirIndex = 0; dirIndex < da.m_count; dirIndex++, direction++) {
          PositionArray fields = direction->m_fields;
          const int     count  = *(fields++);
          for(int moveIndex = 0; moveIndex < count; moveIndex++) {
            const int from = *(fields++);
            if(m_game.m_board[from]) {
              break;
            }
            if(!checksEnemyKing(kingPos, piece, from, direction->m_oppositeDirection) && isPossibleCheck(piece, from)) {
              SETBACKMOVE(m, piece, dirIndex, moveIndex, from, direction->m_oppositeDirection);
              return true;
            }
          }
        }
      }
      continue;

    case Knight:
      { const DirectionArray &da        = MoveTable::knightMoves[piece->m_position];
        const Direction      *direction = da.m_directions;

        for(int dirIndex = 0; dirIndex < da.m_count; dirIndex++, direction++) {
          PositionArray fields = direction->m_fields;
          const int     count  = *(fields++);
          for(int moveIndex = 0; moveIndex < count; moveIndex++) {
            const int from = *(fields++);
            if(m_game.m_board[from]) {
              break;
            }
            if(!checksEnemyKing(kingPos, piece, from, MD_NONE)) {
              SETBACKMOVE(m, piece, dirIndex, moveIndex, from, MD_NONE);
              return true;
            }
          }
        }
      }
      continue;

    case Pawn:
      { const int row = GETROW(piece->m_position);
        int maxStep = 1;
        switch(piece->getPlayer()) {
        case WHITEPLAYER:
          switch(row) {
          case 0:
          case 7:
            invalidPawnPosition(true, piece);
          case 1:
            continue;
          case 3:
            maxStep = 2;
            // continue case
          default:
            { for(int step = 1; step <= maxStep; step++) {
                const int from = MAKE_POSITION(row-step,GETCOL(piece->m_position));
                if(m_game.m_board[from]) {
                  break;
                }
                if(!checksEnemyKing(kingPos, piece, from, MD_UP)) {
                  SETBACKMOVE(m, piece, 0, 0, from, MD_UP);
                  return true;
                }
              }
            }
            break;
          }
          break;
        case BLACKPLAYER:
          switch(row) {
          case 0:
          case 7:
            invalidPawnPosition(true, piece);
          case 6 :
            continue;
          case 4:
            maxStep = 2;
            // continue case
          default:
            { for(int step = 1; step <= maxStep; step++) {
                const int from = MAKE_POSITION(row+step,GETCOL(piece->m_position));
                if(m_game.m_board[from]) {
                  break;
                }
                if(!checksEnemyKing(kingPos, piece, from, MD_DOWN)) {
                  SETBACKMOVE(m, piece, 0, 0, from, MD_DOWN);
                  return true;
                }
              }
            }
            break;
          }
        }
      }
      continue;

    default:
      { const DirectionArray &da        = piece->m_moveTable[piece->m_position];
        const Direction      *direction = da.m_directions;

        for(int dirIndex = 0; dirIndex < da.m_count; dirIndex++, direction++) {
          PositionArray fields = direction->m_fields;
          const int     count  = *(fields++);
          for(int moveIndex = 0; moveIndex < count; moveIndex++) {
            const int from = *(fields++);
            if(m_game.m_board[from]) {
              break;
            }
            if(!checksEnemyKing(kingPos, piece, from, direction->m_oppositeDirection)) {
              SETBACKMOVE(m, piece, dirIndex, moveIndex, from, direction->m_oppositeDirection);
              return true;
            }
          }
        }
      }
      continue;
    }
  }
  return false;
}

bool BackMoveGeneratorNoCheck::nextMove(Move &m) const { // Assumes m is a valid backmove
  const int kingPos = m_game.m_playerState[PLAYERINTURN].m_king->m_position;
  int startDir   = m.m_dirIndex;
  int startIndex = m.m_moveIndex + 1;

  for(Piece *piece = m.m_piece; piece; piece = piece->m_next, startDir = startIndex = 0) {
    if(!piece->m_onBoard) {
      continue;
    }
    switch(piece->getType()) {
    case King:
      { const DirectionArray &da          = MoveTable::kingMoves[piece->m_position];
        const Direction      *direction   = da.m_directions + startDir;

        for(int dirIndex = startDir; dirIndex < da.m_count; dirIndex++, startIndex = 0, direction++) {
          PositionArray fields = direction->m_fields;
          const int     count  = *(fields++);
          for(int moveIndex = startIndex; moveIndex < count; moveIndex++) {
            const int from = fields[moveIndex]; // Do not increment fields !
            if(m_game.m_board[from]) {
              break;
            }
            if(!checksEnemyKing(kingPos, piece, from, direction->m_oppositeDirection) && isPossibleCheck(piece, from)) {
              SETBACKMOVE(m, piece, dirIndex, moveIndex, from, direction->m_oppositeDirection);
              return true;
            }
          }
        }
      }
      continue;

    case Knight:
      { const DirectionArray &da        = MoveTable::knightMoves[piece->m_position];
        const Direction      *direction = da.m_directions + startDir;

        for(int dirIndex = startDir; dirIndex < da.m_count; dirIndex++, startIndex = 0, direction++) {
          PositionArray fields = direction->m_fields;
          const int     count = *(fields++);
          for(int moveIndex = startIndex; moveIndex < count; moveIndex++) {
            const int from = fields[moveIndex]; // Do not increment fields !
            if(m_game.m_board[from]) {
              break;
            }
            if(!checksEnemyKing(kingPos, piece, from, MD_NONE)) {
              SETBACKMOVE(m, piece, dirIndex, moveIndex, from, MD_NONE);
              return true;
            }
          }
        }
      }
      continue;

    case Pawn:
      { const int row = GETROW(piece->m_position);
        const int step = startIndex+1;
        int maxStep = 1;
        switch(piece->getPlayer()) {
        case WHITEPLAYER:
          switch(row) {
          case 0:
          case 7:
            invalidPawnPosition(false, piece);
          case 1 :
            continue;
          case 3 :
            maxStep = 2;
            // continue case
          default:
            { for(int step = startIndex+1; step <= maxStep; step++) {
                const int from = MAKE_POSITION(row-step,GETCOL(piece->m_position));
                if(m_game.m_board[from]) {
                  break;
                }
                if(!checksEnemyKing(kingPos, piece, from, MD_UP)) {
                  SETBACKMOVE(m, piece, 0, startIndex, from, MD_UP);
                  return true;
                }
              }
            }
            break;
          }
          break;
        case BLACKPLAYER:
          switch(row) {
          case 0:
          case 7 :
            invalidPawnPosition(false, piece);
          case 6 :
            continue;
          case 4:
            maxStep = 2;
            // continue case
          default:
            { for(int step = startIndex+1; step <= maxStep; step++) {
                const int from = MAKE_POSITION(row+step,GETCOL(piece->m_position));
                if(m_game.m_board[from]) {
                  break;
                }
                if(!checksEnemyKing(kingPos, piece, from, MD_DOWN)) {
                  SETBACKMOVE(m, piece, 0, startIndex, from, MD_DOWN);
                  return true;
                }
              }
            }
            break;
          }
        }
      }
      continue;

    default:
      { const DirectionArray &da        = piece->m_moveTable[piece->m_position];
        const Direction      *direction = da.m_directions + startDir;

        for(int dirIndex = startDir; dirIndex < da.m_count; dirIndex++, startIndex = 0, direction++) {
          PositionArray fields = direction->m_fields;
          const int     count  = *(fields++);
          for(int moveIndex = startIndex; moveIndex < count; moveIndex++) {
            const int from = fields[moveIndex]; // do not increment fields !
            if(m_game.m_board[from]) {
              break;
            }
            if(!checksEnemyKing(kingPos, piece, from, direction->m_oppositeDirection)) {
              SETBACKMOVE(m, piece, dirIndex, moveIndex, from, direction->m_oppositeDirection);
              return true;
            }
          }
        }
      }
      continue;
    }
  }
  return false;
}



// --------------------------------------------- BackMoveGeneratorLDCheck ------------------------------------------------
// Used to generate backward moves, when king is checked by LDA-piece, not double check,
// Castle-, promotion- and capture-moves are not generated,
bool BackMoveGeneratorLDCheck::firstMove(Move &m) const {
  CLEARTRACE();

  const int kingPos = m_game.m_playerState[PLAYERINTURN].m_king->m_position;

  Piece *checkingPiece = findCheckingLDAPiece(kingPos);
  if(checkingPiece == NULL) {
    throwException(_T("%s:No LDA-piece attacks %s king at %s"), __TFUNCTION__, getPlayerNameEnglish(PLAYERINTURN), getFieldName(kingPos));
  }
  const DirectionArray &da        = checkingPiece->m_moveTable[checkingPiece->m_position];
  const Direction      *direction = da.m_directions;

  for(int dirIndex = 0; dirIndex < da.m_count; dirIndex++, direction++) {
    PositionArray fields = direction->m_fields;
    const int     count  = *(fields++);
    for(int moveIndex = 0; moveIndex < count; moveIndex++) {
      const int from = *(fields++);
      if(m_game.m_board[from]) {
        break;
      }
      if(!checksEnemyKing(kingPos, checkingPiece, from, direction->m_oppositeDirection)) {
        SETBACKMOVE(m, checkingPiece, dirIndex, moveIndex, from, direction->m_oppositeDirection);
        return true;
      }
    }
  }

  const FieldSet fieldsBetween = getFieldsBetween(kingPos, checkingPiece->m_position);
  if(!fieldsBetween.isEmpty()) {
    for(Piece *piece = m_game.m_playerState[CURRENTENEMY].m_first; piece; piece = piece->m_next) {
      if(!piece->m_onBoard || piece == checkingPiece) {
        continue;
      }
      switch(piece->getType()) {
      case King:
      { const DirectionArray &da          = MoveTable::kingMoves[piece->m_position];
        const Direction      *direction   = da.m_directions;

        for(int dirIndex = 0; dirIndex < da.m_count; dirIndex++, direction++) {
          PositionArray fields = direction->m_fields;
          const int     count  = *(fields++);
          for(int moveIndex = 0; moveIndex < count; moveIndex++) {
            const int from = *(fields++);
            if(!fieldsBetween.contains(from)) {
              continue;
            }
            if(!checksEnemyKing(kingPos, piece, from, direction->m_oppositeDirection) && isPossibleCheck(piece, from)) {
              SETBACKMOVE(m, piece, dirIndex, moveIndex, from, direction->m_oppositeDirection);
              return true;
            }
          }
        }
      }
      continue;

      case Knight:
        { const DirectionArray &da        = MoveTable::knightMoves[piece->m_position];
          const Direction      *direction = da.m_directions;

          for(int dirIndex = 0; dirIndex < da.m_count; dirIndex++, direction++) {
            PositionArray fields = direction->m_fields;
            const int     count  = *(fields++);
            for(int moveIndex = 0; moveIndex < count; moveIndex++) {
              const int from = *(fields++);
              if(!fieldsBetween.contains(from)) {
                continue;
              }
              if(!checksEnemyKing(kingPos, piece, from, MD_NONE)) {
                SETBACKMOVE(m, piece, dirIndex, moveIndex, from, MD_NONE);
                return true;
              }
            }
          }
        }
        continue;

      case Pawn:
        { const int row = GETROW(piece->m_position);
          int maxStep = 1;
          switch(piece->getPlayer()) {
          case WHITEPLAYER:
            switch(row) {
            case 0:
            case 7:
              invalidPawnPosition(true, piece);
            case 1:
              continue;
            case 3:
              maxStep = 2;
              // continue case
            default:
              { for(int step = 1; step <= maxStep; step++) {
                  const int from = MAKE_POSITION(row-step,GETCOL(piece->m_position));
                  if(m_game.m_board[from]) {
                    break;
                  }
                  if(!fieldsBetween.contains(from)) {
                    continue;
                  }
                  if(!checksEnemyKing(kingPos, piece, from, MD_UP)) {
                    SETBACKMOVE(m, piece, 0, 0, from, MD_UP);
                    return true;
                  }
                }
              }
              break;
            }
            break;
          case BLACKPLAYER:
            switch(row) {
            case 0:
            case 7:
              invalidPawnPosition(true, piece);
            case 6 :
              continue;
            case 4:
              maxStep = 2;
              // continue case
            default:
              { for(int step = 1; step <= maxStep; step++) {
                  const int from = MAKE_POSITION(row+step,GETCOL(piece->m_position));
                  if(m_game.m_board[from]) {
                    break;
                  }
                  if(!fieldsBetween.contains(from)) {
                    continue;
                  }
                  if(!checksEnemyKing(kingPos, piece, from, MD_DOWN)) {
                    SETBACKMOVE(m, piece, 0, 0, from, MD_DOWN);
                    return true;
                  }
                }
              }
              break;
            }
          }
        }
        continue;

      case Queen:;
        continue; // No need to waste time on her. a queen can never move, and then uncover an LD-attacking piece.

      default:
        { const DirectionArray &da        = piece->m_moveTable[piece->m_position];
          const Direction      *direction = da.m_directions;

          for(int dirIndex = 0; dirIndex < da.m_count; dirIndex++, direction++) {
            PositionArray fields = direction->m_fields;
            const int     count  = *(fields++);
            for(int moveIndex = 0; moveIndex < count; moveIndex++) {
              const int from = *(fields++);
              if(m_game.m_board[from]) {
                break;
              }
              if(!fieldsBetween.contains(from)) {
                continue;
              }
              if(!checksEnemyKing(kingPos, piece, from, direction->m_oppositeDirection)) {
                SETBACKMOVE(m, piece, dirIndex, moveIndex, from, direction->m_oppositeDirection);
                return true;
              }
            }
          }
        }
        continue;
      }
    }
  }
  return false;
}

bool BackMoveGeneratorLDCheck::nextMove(Move &m) const { // Assumes m is a valid backmove
  const int kingPos = m_game.m_playerState[PLAYERINTURN].m_king->m_position;
  int startDir   = m.m_dirIndex;
  int startIndex = m.m_moveIndex + 1;

  Piece *checkingPiece = findCheckingLDAPiece(kingPos);
  if(m.m_piece == checkingPiece) {
    const DirectionArray &da        = checkingPiece->m_moveTable[checkingPiece->m_position];
    const Direction      *direction = da.m_directions + startDir;

    for(int dirIndex = startDir; dirIndex < da.m_count; dirIndex++, startIndex = 0, direction++) {
      PositionArray fields = direction->m_fields;
      const int     count  = *(fields++);
      for(int moveIndex = startIndex; moveIndex < count; moveIndex++) {
        const int from = fields[moveIndex]; // do not increment fields !
        if(m_game.m_board[from]) {
          break;
        }
        if(!checksEnemyKing(kingPos, checkingPiece, from, direction->m_oppositeDirection)) {
          SETBACKMOVE(m, checkingPiece, dirIndex, moveIndex, from, direction->m_oppositeDirection);
          return true;
        }
      }
    }
    m.m_piece = m_game.m_playerState[CURRENTENEMY].m_first;
    startDir = startIndex = 0;
  }

  const FieldSet fieldsBetween = getFieldsBetween(kingPos, checkingPiece->m_position);
  if(!fieldsBetween.isEmpty()) { // Must have been af move by the checking piece
    for(Piece *piece = m.m_piece; piece; piece = piece->m_next, startDir = startIndex = 0) {
      if(!piece->m_onBoard || piece == checkingPiece) {
        continue;
      }
      switch(piece->getType()) {
      case King:
        { const DirectionArray &da          = MoveTable::kingMoves[piece->m_position];
          const Direction      *direction   = da.m_directions + startDir;

          for(int dirIndex = startDir; dirIndex < da.m_count; dirIndex++, startIndex = 0, direction++) {
            PositionArray fields = direction->m_fields;
            const int     count  = *(fields++);
            for(int moveIndex = startIndex; moveIndex < count; moveIndex++) {
              const int from = fields[moveIndex]; // Do not increment fields !
              if(m_game.m_board[from]) {
                break;
              }
              if(!fieldsBetween.contains(from)) {
                continue;
              }
              if(!checksEnemyKing(kingPos, piece, from, direction->m_oppositeDirection) && isPossibleCheck(piece, from)) {
                SETBACKMOVE(m, piece, dirIndex, moveIndex, from, direction->m_oppositeDirection);
                return true;
              }
            }
          }
        }
        continue;

      case Knight:
        { const DirectionArray &da        = MoveTable::knightMoves[piece->m_position];
          const Direction      *direction = da.m_directions + startDir;

          for(int dirIndex = startDir; dirIndex < da.m_count; dirIndex++, startIndex = 0, direction++) {
            PositionArray fields = direction->m_fields;
            const int     count = *(fields++);
            for(int moveIndex = startIndex; moveIndex < count; moveIndex++) {
              const int from = fields[moveIndex]; // Do not increment fields !
              if(m_game.m_board[from]) {
                break;
              }
              if(!fieldsBetween.contains(from)) {
                continue;
              }
              if(!checksEnemyKing(kingPos, piece, from, MD_NONE)) {
                SETBACKMOVE(m, piece, dirIndex, moveIndex, from, MD_NONE);
                return true;
              }
            }
          }
        }
        continue;

      case Pawn:
        { const int row = GETROW(piece->m_position);
          const int step = startIndex+1;
          int maxStep = 1;
          switch(piece->getPlayer()) {
          case WHITEPLAYER:
            switch(row) {
            case 0:
            case 7:
              invalidPawnPosition(false, piece);
            case 1 :
              continue;
            case 3 :
              maxStep = 2;
              // continue case
            default:
              { for(int step = startIndex+1; step <= maxStep; step++) {
                  const int from = MAKE_POSITION(row-step,GETCOL(piece->m_position));
                  if(m_game.m_board[from]) {
                    break;
                  }
                  if(!fieldsBetween.contains(from)) {
                    continue;
                  }
                  if(!checksEnemyKing(kingPos, piece, from, MD_UP)) {
                    SETBACKMOVE(m, piece, 0, startIndex, from, MD_UP);
                    return true;
                  }
                }
              }
              break;
            }
            break;
          case BLACKPLAYER:
            switch(row) {
            case 0:
            case 7 :
              invalidPawnPosition(false, piece);
            case 6 :
              continue;
            case 4:
              maxStep = 2;
              // continue case
            default:
              { for(int step = startIndex+1; step <= maxStep; step++) {
                  const int from = MAKE_POSITION(row+step,GETCOL(piece->m_position));
                  if(m_game.m_board[from]) {
                    break;
                  }
                  if(!fieldsBetween.contains(from)) {
                    continue;
                  }
                  if(!checksEnemyKing(kingPos, piece, from, MD_DOWN)) {
                    SETBACKMOVE(m, piece, 0, startIndex, from, MD_DOWN);
                    return true;
                  }
                }
              }
              break;
            }
          }
        }
        continue;

      case Queen:
        continue; // Queen can never make a move and partitcipate in a doublecheck

      default:
        { const DirectionArray &da        = piece->m_moveTable[piece->m_position];
          const Direction      *direction = da.m_directions + startDir;

          for(int dirIndex = startDir; dirIndex < da.m_count; dirIndex++, startIndex = 0, direction++) {
            PositionArray fields = direction->m_fields;
            const int     count  = *(fields++);
            for(int moveIndex = startIndex; moveIndex < count; moveIndex++) {
              const int from = fields[moveIndex]; // do not increment fields !
              if(m_game.m_board[from]) {
                break;
              }
              if(!fieldsBetween.contains(from)) {
                continue;
              }
              if(!checksEnemyKing(kingPos, piece, from, direction->m_oppositeDirection)) {
                SETBACKMOVE(m, piece, dirIndex, moveIndex, from, direction->m_oppositeDirection);
                return true;
              }
            }
          }
        }
        continue;
      }
    }
  }
  return false;
}

Piece *BackMoveGeneratorLDCheck::findCheckingLDAPiece(int kingPos) const {
  for(Piece *piece = m_game.m_playerState[CURRENTENEMY].m_first; piece; piece = piece->m_next) {
    if(!piece->m_onBoard || !(piece->m_attackAttribute & ATTACKS_LONGDISTANCE)) {
      continue;
    }

    const MoveDirection kingDir = KING_DIRECTION(piece->m_enemyState, piece->m_position);
    switch(piece->getType()) {
    case Queen :
      switch(kingDir) {
      case MD_NONE:
        continue;
      default     :
        return piece;
      }

    case Rook  :
      switch(kingDir) {
      case MD_LEFT :
      case MD_RIGHT:
      case MD_DOWN :
      case MD_UP   :
        return piece;
      }
      continue;

    case Bishop:
      switch(kingDir) {
      case MD_DOWNDIAG1:
      case MD_UPDIAG1  :
      case MD_DOWNDIAG2:
      case MD_UPDIAG2  :
        return piece;
      }
      continue;
    }
  }
  return NULL;
}

// --------------------------------------------- BackMoveGeneratorSDCheck ------------------------------------------------
// Used to generate backward moves, when king is checked by SDA-piece, not double check,
// Castle-, promotion- and capture-moves are not generated,
bool BackMoveGeneratorSDCheck::firstMove(Move &m) const {
  CLEARTRACE();

  const int kingPos = m_game.m_playerState[PLAYERINTURN].m_king->m_position;

  const int sdaPos = m_game.m_playerState[CURRENTENEMY].m_checkingSDAPosition;
  if(sdaPos < 0) {
    return false;
  }
  Piece *piece = m_game.m_board[sdaPos];
  if(piece == NULL) {
    return false;
  }
  switch(piece->getType()) {
  case Knight:
    { const DirectionArray &da        = MoveTable::knightMoves[piece->m_position];
      const Direction      *direction = da.m_directions;

      for(int dirIndex = 0; dirIndex < da.m_count; dirIndex++, direction++) {
        PositionArray fields = direction->m_fields;
        const int     count  = *(fields++);
        for(int moveIndex = 0; moveIndex < count; moveIndex++) {
          const int from = *(fields++);
          if(m_game.m_board[from]) {
            break;
          }
          if(!checksEnemyKing(kingPos, piece, from, MD_NONE)) {
            SETBACKMOVE(m, piece, dirIndex, moveIndex, from, MD_NONE);
            return true;
          }
        }
      }
    }
    return false;

  case Pawn:
    { const int row = GETROW(piece->m_position);
      int maxStep = 1;
      switch(piece->getPlayer()) {
      case WHITEPLAYER:
        switch(row) {
        case 0:
        case 7:
          invalidPawnPosition(true, piece);
        case 1:
          return false;
        case 3:
          maxStep = 2;
          // continue case
        default:
          { for(int step = 1; step <= maxStep; step++) {
              const int from = MAKE_POSITION(row-step,GETCOL(piece->m_position));
              if(m_game.m_board[from]) {
                break;
              }
              if(!checksEnemyKing(kingPos, piece, from, MD_UP)) {
                SETBACKMOVE(m, piece, 0, 0, from, MD_UP);
                return true;
              }
            }
          }
          break;
        }
        break;
      case BLACKPLAYER:
        switch(row) {
        case 0:
        case 7:
          invalidPawnPosition(true, piece);
        case 6 :
          return false;
        case 4:
          maxStep = 2;
          // continue case
        default:
          { for(int step = 1; step <= maxStep; step++) {
              const int from = MAKE_POSITION(row+step,GETCOL(piece->m_position));
              if(m_game.m_board[from]) {
                break;
              }
              if(!checksEnemyKing(kingPos, piece, from, MD_DOWN)) {
                SETBACKMOVE(m, piece, 0, 0, from, MD_DOWN);
                return true;
              }
            }
          }
          break;
        }
      }
    }
    break;
  }
  return false;
}

// No need to try to find another pawn or knight. There is only one piece that could give the check
bool BackMoveGeneratorSDCheck::nextMove(Move &m) const { // Assumes m is a valid backmove
  const int kingPos = m_game.m_playerState[PLAYERINTURN].m_king->m_position;
  int startDir   = m.m_dirIndex;
  int startIndex = m.m_moveIndex + 1;

  Piece *piece = m.m_piece;
  switch(piece->getType()) {
  case Knight:
    { const DirectionArray &da        = MoveTable::knightMoves[piece->m_position];
      const Direction      *direction = da.m_directions + startDir;

      for(int dirIndex = startDir; dirIndex < da.m_count; dirIndex++, startIndex = 0, direction++) {
        PositionArray fields = direction->m_fields;
        const int     count = *(fields++);
        for(int moveIndex = startIndex; moveIndex < count; moveIndex++) {
          const int from = fields[moveIndex]; // Do not increment fields !
          if(m_game.m_board[from]) {
            break;
          }
          if(!checksEnemyKing(kingPos, piece, from, MD_NONE)) {
            SETBACKMOVE(m, piece, dirIndex, moveIndex, from, MD_NONE);
            return true;
          }
        }
      }
    }
    break;

  case Pawn:
    { const int row = GETROW(piece->m_position);
      const int step = startIndex+1;
      int maxStep = 1;
      switch(piece->getPlayer()) {
      case WHITEPLAYER:
        switch(row) {
        case 0:
        case 7:
          invalidPawnPosition(false, piece);
        case 1 :
          return false;
        case 3 :
          maxStep = 2;
          // continue case
        default:
          { for(int step = startIndex+1; step <= maxStep; step++) {
              const int from = MAKE_POSITION(row-step,GETCOL(piece->m_position));
              if(m_game.m_board[from]) {
                break;
              }
              if(!checksEnemyKing(kingPos, piece, from, MD_UP)) {
                SETBACKMOVE(m, piece, 0, startIndex, from, MD_UP);
                return true;
              }
            }
          }
          break;
        }
        break;
      case BLACKPLAYER:
        switch(row) {
        case 0:
        case 7 :
          invalidPawnPosition(false, piece);
        case 6 :
          return false;
        case 4:
          maxStep = 2;
          // continue case
        default:
          { for(int step = startIndex+1; step <= maxStep; step++) {
              const int from = MAKE_POSITION(row+step,GETCOL(piece->m_position));
              if(m_game.m_board[from]) {
                break;
              }
              if(!checksEnemyKing(kingPos, piece, from, MD_DOWN)) {
                SETBACKMOVE(m, piece, 0, startIndex, from, MD_DOWN);
                return true;
              }
            }
          }
          break;
        }
      }
    }
    break;
  }
  return false;
}

// --------------------------------------------- BackMoveGeneratorLDDoubleCheck ------------------------------------------------
// Used to generate backward moves, when king is in double check. (attacked from row or column) and diagonal.
// Moved piece must be af rook or bishop
bool BackMoveGeneratorLDDoubleCheck::firstMove(Move &m) const {
  CLEARTRACE();

  int            sourceField;
  MoveDirection  direction;
  Piece *piece = findMovedPiece(sourceField, direction);
  if(piece == NULL) {
    return false;
  }
  SETBACKMOVE(m, piece, 0, 0, sourceField, direction);
  return true;
}

static int commonFieldRowDiag1(int row, int diag1) {
  const int col = row - diag1 + 7;
  return isValidLine(col) ? MAKE_POSITION(row, col) : -1;
}

static int commonFieldColDiag1(int col, int diag1) {
  const int row = col + diag1 - 7;
  return isValidLine(row) ? MAKE_POSITION(row, col) : -1;
}

static int commonFieldRowDiag2(int row, int diag2) {
  const int col = diag2 - row;
  return isValidLine(col) ? MAKE_POSITION(row, col) : -1;
}

static int commonFieldColDiag2(int col, int diag2) {
  const int row = diag2 - col;
  return isValidLine(row) ? MAKE_POSITION(row, col) : -1;
}

Piece *BackMoveGeneratorLDDoubleCheck::findMovedPiece(int &sourceField, MoveDirection &direction) const {
  const PlayerState &playerState = m_game.m_playerState[PLAYERINTURN];
  const PlayerState &enemyState  = m_game.m_playerState[CURRENTENEMY];
  const int         kingPos      = playerState.m_king->m_position;
  const AttackInfo  attInfo      = enemyState.m_attackTable[kingPos].m_attackInfo;
  const FieldInfo  &kingInfo     = Game::s_fieldInfo[kingPos];
  PositionArray ppa, dpa;
  MoveDirection pDir, dDir;

  if(playerState.m_kingAttackState & KING_LD_ATTACKED_FROM_ROW) {
    if(attInfo.m_fromLeft) {
      ppa  = kingInfo.m_rowLine.m_lower;
      pDir = MD_LEFT;
    } else {
      ppa  = kingInfo.m_rowLine.m_upper;
      pDir = MD_RIGHT;
    }
  } else {
    if(attInfo.m_fromBelove) {
      ppa  = kingInfo.m_colLine.m_lower;
      pDir = MD_DOWN;
    } else {
      ppa  = kingInfo.m_colLine.m_upper;
      pDir = MD_UP;
    }
  }
  if(playerState.m_kingAttackState & KING_LD_ATTACKED_FROM_DIAG1) {
    if(attInfo.m_fromLowerDiag1) {
      dpa  = kingInfo.m_diag1Line.m_lower;
      dDir = MD_DOWNDIAG1;
    } else {
      dpa  = kingInfo.m_diag1Line.m_upper;
      dDir = MD_UPDIAG1;
    }
  } else {
    if(attInfo.m_fromLowerDiag2) {
      dpa  = kingInfo.m_diag2Line.m_lower;
      dDir = MD_DOWNDIAG2;
    } else {
      dpa  = kingInfo.m_diag2Line.m_upper;
      dDir = MD_UPDIAG2;
    }
  }
  const Piece *rcPiece   = m_game.findFirstPieceInDirection(ppa);
  const Piece *diagPiece = m_game.findFirstPieceInDirection(dpa);

  const int rcDist   = Game::getWalkDistance(kingPos, rcPiece->m_position );
  const int diagDist = Game::getWalkDistance(kingPos, diagPiece->m_position);

  if(rcDist > diagDist) {                                                           // Moved piece must be the diagonal-moving piece, ie a bishop
    if((diagPiece->getType() != Bishop)                                             // A queen can't do it
    || enemyState.m_attackTable[diagPiece->m_position].isLDAttackedFromDir(dDir)) { // And we cannot have another enemypiece behind the bishop
      return NULL;
    }
    switch(playerState.m_kingAttackState) {
    case KING_LD_ATTACKED_FROM_ROW | KING_LD_ATTACKED_FROM_DIAG1:
      sourceField = commonFieldRowDiag2(kingInfo.m_row, GETDIAG2(diagPiece->m_position));
      direction   = (GETROW(sourceField) < GETROW(diagPiece->m_position)) ? MD_UPDIAG2 : MD_DOWNDIAG2;
      break;

    case KING_LD_ATTACKED_FROM_COL | KING_LD_ATTACKED_FROM_DIAG1: // ok
      sourceField = commonFieldColDiag2(kingInfo.m_col, GETDIAG2(diagPiece->m_position));
      direction   = (GETROW(sourceField) < GETROW(diagPiece->m_position)) ? MD_UPDIAG2 : MD_DOWNDIAG2;
      break;

    case KING_LD_ATTACKED_FROM_ROW | KING_LD_ATTACKED_FROM_DIAG2:
      sourceField = commonFieldRowDiag1(kingInfo.m_row, GETDIAG1(diagPiece->m_position));
      direction   = (GETROW(sourceField) < GETROW(diagPiece->m_position)) ? MD_UPDIAG1 : MD_DOWNDIAG1;
      break;

    case KING_LD_ATTACKED_FROM_COL | KING_LD_ATTACKED_FROM_DIAG2: // ok
      sourceField = commonFieldColDiag1(kingInfo.m_col, GETDIAG1(diagPiece->m_position));
      direction   = (GETROW(sourceField) < GETROW(diagPiece->m_position)) ? MD_UPDIAG1 : MD_DOWNDIAG1;
      break;
    }
    if(!isValidPosition(sourceField) || (Game::getWalkDistance(sourceField, kingPos) >= rcDist)) {
      return NULL;
    }
    return (Piece*)diagPiece;
  } else if(rcDist < diagDist) {                                                  // Moved piece must be the parallel-moving piece, ie. a rook
    if((rcPiece->getType() != Rook)                                               // A queen can't do it
    || enemyState.m_attackTable[rcPiece->m_position].isLDAttackedFromDir(pDir)) { // And we cannot have another enemypiece behind the rook
      return NULL;
    }
    switch(playerState.m_kingAttackState) {
    case KING_LD_ATTACKED_FROM_ROW | KING_LD_ATTACKED_FROM_DIAG1:
      sourceField = commonFieldColDiag1(GETCOL(rcPiece->m_position), kingInfo.m_diag1);
      direction   = (GETROW(sourceField) < GETROW(rcPiece->m_position))   ? MD_UP    : MD_DOWN;
      break;

    case KING_LD_ATTACKED_FROM_COL | KING_LD_ATTACKED_FROM_DIAG1:
      sourceField = commonFieldRowDiag1(GETROW(rcPiece->m_position), kingInfo.m_diag1);
      direction   = (GETCOL(sourceField) < GETCOL(rcPiece->m_position))   ? MD_RIGHT : MD_LEFT;
      break;

    case KING_LD_ATTACKED_FROM_ROW | KING_LD_ATTACKED_FROM_DIAG2:
      sourceField = commonFieldColDiag2(GETCOL(rcPiece->m_position), kingInfo.m_diag2);
      direction   = (GETROW(sourceField) < GETROW(rcPiece->m_position))   ? MD_UP    : MD_DOWN;
      break;

    case KING_LD_ATTACKED_FROM_COL | KING_LD_ATTACKED_FROM_DIAG2:
      sourceField = commonFieldRowDiag2(GETROW(rcPiece->m_position), kingInfo.m_diag2);
      direction   = (GETCOL(sourceField) < GETCOL(rcPiece->m_position))   ? MD_RIGHT : MD_LEFT;
      break;
    }
    if(!isValidPosition(sourceField) || (Game::getWalkDistance(sourceField, kingPos) >= diagDist)) {
      return NULL;
    }
    return (Piece*)rcPiece;
  }
  return NULL; // The very special situation, where the distance to the diagonal- and parallel-attacking piece
               // are equal can happen in 2 different situations:
               // 1. As shown in "The Chess Mysteries of the Arabian Knights" by Raymond Smullyan:
               // W:Kb3, Ba4. B:Kd1, Rb5, Bd5,c3. This doublecheck must have been given by the black pawn capturing e.p
               // on c4. But this is an uninteresting back move as it takes us to another endgame, with 1 additional pawn.
               // 2. By a promotion to Q,R or B, capturing or not, and uncovering a Q,R or B. But this is an
               // uninteresting back move too, as it also takes us to another endgame, with the piece "unpromoted" to a pawn.
}

// --------------------------------------------- BackMoveGeneratorSDDoubleCheck ------------------------------------------------
// Used to generate backward moves, when king is in double check. (attacked from row or column or diagonal) and by an SDA piece
// knight/pawn
// Must be a knight or pawn move, uncovering an LDA piece (Queen,Rook or Bishop)
bool BackMoveGeneratorSDDoubleCheck::firstMove(Move &m) const {
  CLEARTRACE();

  const int kingPos = m_game.m_playerState[PLAYERINTURN].m_king->m_position;

  const int attPos = m_game.m_playerState[CURRENTENEMY].m_checkingSDAPosition;
  if(attPos < 0) {
    throwException(_T("%s:checkSDAPosition=-1"), __TFUNCTION__);
  }
  Piece *piece = m_game.m_board[attPos];
  if(piece->getType() != Knight) {
    if(piece->getType() == Pawn) {
      return false;
      // Can only happen, when a pawn just captured (diagonally) right beside the opponent king
      // thereby uncovering a queen or rook, to give the double-check. But capturing means that the original position
      // does not belong to the current endgame, so we can safely return false, to stop looking for predecessors
    }
    throwException(_T("Piece at %s attacking the %s king is neither a knight or pawn (=%s)")
                  ,getFieldName(attPos)
                  ,getPlayerNameEnglish(PLAYERINTURN)
                  ,getPieceTypeNameEnglish(piece->getType()));
  }
  const FieldSet possibleSourceFields = findPossibleSourceFields(kingPos);

  const DirectionArray &da        = MoveTable::knightMoves[piece->m_position];
  const Direction      *direction = da.m_directions;

  for(int dirIndex = 0; dirIndex < da.m_count; dirIndex++, direction++) {
    PositionArray fields = direction->m_fields;
    const int     count  = *(fields++);
    for(int moveIndex = 0; moveIndex < count; moveIndex++) {
      const int from = *(fields++);
      if(possibleSourceFields.contains(from)) {
        SETBACKMOVE(m, piece, dirIndex, moveIndex, from, MD_NONE);
        return true;
      }
    }
  }
  return false;
}

FieldSet BackMoveGeneratorSDDoubleCheck::findPossibleSourceFields(int kingPos) const {
  const PlayerState &playerState = m_game.m_playerState[PLAYERINTURN];
  const PlayerState &enemyState  = m_game.m_playerState[CURRENTENEMY];
  const AttackInfo   attInfo     = enemyState.m_attackTable[kingPos].m_attackInfo;
  const FieldInfo   &finfo       = Game::s_fieldInfo[kingPos];
  PositionArray pa;
  switch(playerState.m_kingAttackState & ~KING_SD_ATTACKED) {
  case KING_LD_ATTACKED_FROM_ROW  :
    pa = attInfo.m_fromLeft       ? finfo.m_rowLine.m_lower   : finfo.m_rowLine.m_upper;
    break;
  case KING_LD_ATTACKED_FROM_COL  :
    pa = attInfo.m_fromBelove     ? finfo.m_colLine.m_lower   : finfo.m_colLine.m_upper;
    break;
  case KING_LD_ATTACKED_FROM_DIAG1:
    pa = attInfo.m_fromLowerDiag1 ? finfo.m_diag1Line.m_lower : finfo.m_diag1Line.m_upper;
    break;
  case KING_LD_ATTACKED_FROM_DIAG2:
    pa = attInfo.m_fromLowerDiag2 ? finfo.m_diag2Line.m_lower : finfo.m_diag2Line.m_upper;
    break;
  default                         :
    return FieldSet();
  }

  FieldSet result;
  for(int count = *(pa++); count--;) {
    const int pos = *(pa++);
    if(m_game.m_board[pos]) {
      return result;
    }
    result.add(pos);
  }
  throwException(_T("No %s LDA-piece found in knight-doublecheck. %s kings position=%s")
                ,getPlayerNameEnglish(CURRENTENEMY)
                ,getPlayerNameEnglish(PLAYERINTURN)
                ,getFieldName(kingPos));
  return result;
}

#endif
